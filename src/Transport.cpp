#define MSC_CLASS "Transport"

#include "Transport.hpp"
#include "Logger.hpp"
#include "MediaSoupClientErrors.hpp"
#include "ortc.hpp"
#include "PeerConnection.hpp"

using json = nlohmann::json;

namespace mediasoupclient
{
	/* Transport */

	Transport::Transport(
	  Transport::Listener* listener, const std::string& id, const json* extendedRtpCapabilities, const json& appData)
      : extendedRtpCapabilities(extendedRtpCapabilities), certificate(PeerConnection::GenerateCertificate()), 
      listener(listener), id(id), appData(appData)
	{
		MSC_TRACE();
	}

    Transport::Listener* Transport::GetListener() 
    {
        return listener;
    }

	const std::string& Transport::GetId() const
	{
		MSC_TRACE();

		return this->id;
	}

	bool Transport::IsClosed() const
	{
		MSC_TRACE();

		return this->closed;
	}

	const std::string& Transport::GetConnectionState() const
	{
		MSC_TRACE();

		return PeerConnection::iceConnectionState2String[this->connectionState];
	}

	json& Transport::GetAppData()
	{
		MSC_TRACE();

		return this->appData;
	}

	void Transport::Close()
	{
		MSC_TRACE();

		if (this->closed)
			return;

		this->closed = true;

		// Close the handler.
		this->handler->Close();
	}

	json Transport::GetStats() const
	{
		MSC_TRACE();

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("Transport closed");
		else
			return this->handler->GetTransportStats();
	}

	void Transport::RestartIce(const json& iceParameters)
	{
		MSC_TRACE();

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("Transport closed");
		else
			return this->handler->RestartIce(iceParameters);
	}

	void Transport::UpdateIceServers(const json& iceServers)
	{
		MSC_TRACE();

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("Transport closed");
		else
			return this->handler->UpdateIceServers(iceServers);
	}

    bool Transport::GetFingerprint(std::string& algorithm, std::string& fingerprint)
    {
        PeerConnection::GetFingerprint(certificate, algorithm, fingerprint);
        return true;
    }
    
	void Transport::SetHandler(Handler* handler)
	{
		MSC_TRACE();

		this->handler = handler;
	}

	void Transport::OnConnect(json& dtlsParameters)
	{
		MSC_TRACE();

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("Transport closed");

		return this->listener->OnConnect(this, dtlsParameters).get();
	}

	void Transport::OnConnectionStateChange(
	  webrtc::PeerConnectionInterface::IceConnectionState connectionState)
	{
		MSC_TRACE();

		// Update connection state.
		this->connectionState = connectionState;

		return this->listener->OnConnectionStateChange(
		  this, PeerConnection::iceConnectionState2String[connectionState]);
	}

	/* SendTransport */

	SendTransport::SendTransport(
      Transport::Listener* listener,
	  const std::string& id,
	  const json& iceParameters,
	  const json& iceCandidates,
	  const json& dtlsParameters,
	  const json& sctpParameters,
	  const PeerConnection::Options* peerConnectionOptions,
	  const json* extendedRtpCapabilities,
	  const std::map<std::string, bool>* canProduceByKind,
	  const json& appData)
	  : Transport(listener, id, extendedRtpCapabilities, appData),
	    canProduceByKind(canProduceByKind)
	{
		MSC_TRACE();

		if (sctpParameters != nullptr && sctpParameters.is_object())
		{
			this->hasSctpParameters = true;
			auto maxMessageSizeIt   = sctpParameters.find("maxMessageSize");

			if (maxMessageSizeIt->is_number_integer())
				this->maxSctpMessageSize = maxMessageSizeIt->get<size_t>();
		}

		json sendingRtpParametersByKind = {
			{ "audio", ortc::getSendingRtpParameters("audio", *extendedRtpCapabilities) },
			{ "video", ortc::getSendingRtpParameters("video", *extendedRtpCapabilities) }
		};

		json sendingRemoteRtpParametersByKind = {
			{ "audio", ortc::getSendingRemoteRtpParameters("audio", *extendedRtpCapabilities) },
			{ "video", ortc::getSendingRemoteRtpParameters("video", *extendedRtpCapabilities) }
		};

        PeerConnection::Options options;
        if (peerConnectionOptions != nullptr) {
            options = *peerConnectionOptions;
        }
        options.config.certificates.push_back(this->certificate);
        
		this->sendHandler.reset(new SendHandler(
		  dynamic_cast<SendHandler::PrivateListener*>(this),
		  iceParameters,
		  iceCandidates,
		  dtlsParameters,
		  sctpParameters,
		  &options,
		  sendingRtpParametersByKind,
		  sendingRemoteRtpParametersByKind));

		Transport::SetHandler(this->sendHandler.get());
	}

	/**
	 * Create a Producer.
	 */
    SendHandler::SendResult SendTransport::Produce(
	  webrtc::MediaStreamTrackInterface* track,
	  const std::vector<webrtc::RtpEncodingParameters>* encodings,
	  const json* codecOptions,
	  const json* codec,
	  const json& appData)
	{
		MSC_TRACE();

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("SendTransport closed");
		else if (!track)
			MSC_THROW_TYPE_ERROR("missing track");
		else if (track->state() == webrtc::MediaStreamTrackInterface::TrackState::kEnded)
			MSC_THROW_INVALID_STATE_ERROR("track ended");
		else if (this->canProduceByKind->find(track->kind()) == this->canProduceByKind->end())
			MSC_THROW_UNSUPPORTED_ERROR("cannot produce track kind");

		if (codecOptions)
			ortc::validateProducerCodecOptions(const_cast<json&>(*codecOptions));

		std::string producerId;
		std::vector<webrtc::RtpEncodingParameters> normalizedEncodings;

		if (encodings)
		{
			std::for_each(
			  encodings->begin(),
			  encodings->end(),
			  [&normalizedEncodings](const webrtc::RtpEncodingParameters& entry) {
				  webrtc::RtpEncodingParameters encoding;

				  encoding.active                   = entry.active;
				  encoding.max_bitrate_bps          = entry.max_bitrate_bps;
				  encoding.max_framerate            = entry.max_framerate;
				  encoding.scale_resolution_down_by = entry.scale_resolution_down_by;
				  encoding.network_priority         = entry.network_priority;
				  encoding.scalability_mode         = entry.scalability_mode;

				  normalizedEncodings.push_back(encoding);
			  });
		}

		// May throw.
		auto sendResult = this->sendHandler->Send(track, &normalizedEncodings, codecOptions, codec);

		try
		{
			// This will fill rtpParameters's missing fields with default values.
			ortc::validateRtpParameters(sendResult.rtpParameters);

            return sendResult;
		}
		catch (MediaSoupClientError& error)
		{
			this->sendHandler->StopSending(sendResult.localId);

			throw;
		}

		/*auto* producer = new Producer(
		  this,
		  producerListener,
		  producerId,
		  sendResult.localId,
		  sendResult.rtpSender,
		  track,
		  sendResult.rtpParameters,
		  appData);

		this->producers[producer->GetId()] = producer;

		return producer;*/
	}

    Handler::DataChannel SendTransport::ProduceData(
	  const std::string& label,
	  const std::string& protocol,
	  bool ordered,
	  int maxRetransmits,
	  int maxPacketLifeTime,
	  const nlohmann::json& appData)
	{
		MSC_TRACE();

		if (!this->hasSctpParameters)
			MSC_THROW_ERROR("SctpParameters are mandatory when using data producer listener");

		webrtc::DataChannelInit dataChannelInit;
		dataChannelInit.protocol = protocol;
		dataChannelInit.ordered  = ordered;
		if (maxRetransmits != -1 && maxPacketLifeTime != 0)
		{
			MSC_THROW_ERROR("Cannot set both maxRetransmits and maxPacketLifeTime");
		}
		if (maxRetransmits != 0)
		{
			dataChannelInit.maxRetransmits = maxRetransmits;
		}
		if (maxPacketLifeTime != 0)
		{
			dataChannelInit.maxRetransmitTime = maxPacketLifeTime;
		}

		// This may throw.
		auto sendResult = this->sendHandler->SendDataChannel(label, dataChannelInit);

        return sendResult;
        /*
		auto dataChannelId =
		  this->listener->OnProduceData(this, sendResult.sctpStreamParameters, label, protocol, appData);

		auto* dataProducer = new DataProducer(
		  this,
		  dataProducerListener,
		  dataChannelId.get(),
		  sendResult.dataChannel,
		  sendResult.sctpStreamParameters,
		  appData);

		this->dataProducers[dataProducer->GetId()] = dataProducer;

		return dataProducer;*/
	}

	void SendTransport::Close()
	{
		MSC_TRACE();

		if (this->closed)
			return;

		Transport::Close();
	}

	void SendTransport::CloseProducer(const std::string& localId)
	{
		MSC_TRACE();


		if (this->closed)
			return;

		// May throw.
		this->sendHandler->StopSending(localId);
	}


	void SendTransport::ReplaceTrack(const std::string& localId, webrtc::MediaStreamTrackInterface* track)
	{
		MSC_TRACE();

		return this->sendHandler->ReplaceTrack(localId, track);
	}

	void SendTransport::SetMaxSpatialLayer(const std::string& localId, uint8_t maxSpatialLayer)
	{
		MSC_TRACE();

		return this->sendHandler->SetMaxSpatialLayer(localId, maxSpatialLayer);
	}

	json SendTransport::GetProducerStats(const std::string& localId)
	{
		MSC_TRACE();

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("SendTransport closed");

		return this->sendHandler->GetSenderStats(localId);
	}

	/* RecvTransport */

	RecvTransport::RecvTransport(
      Transport::Listener* listener,
	  const std::string& id,
	  const json& iceParameters,
	  const json& iceCandidates,
	  const json& dtlsParameters,
	  const json& sctpParameters,
	  const PeerConnection::Options* peerConnectionOptions,
	  const json* extendedRtpCapabilities,
	  const json& appData)
	  : Transport(listener, id, extendedRtpCapabilities, appData)
	{
		MSC_TRACE();

		this->hasSctpParameters = sctpParameters != nullptr && sctpParameters.is_object();

        PeerConnection::Options options;
        if (peerConnectionOptions != nullptr) {
            options = *peerConnectionOptions;
        }
        options.config.certificates.push_back(this->certificate);        
		this->recvHandler.reset(new RecvHandler(
		  dynamic_cast<RecvHandler::PrivateListener*>(this),
		  iceParameters,
		  iceCandidates,
		  dtlsParameters,
		  sctpParameters,
		  &options));

		Transport::SetHandler(this->recvHandler.get());
	}

	/**
	 * Create a Consumer.
	 */
    RecvHandler::RecvResult RecvTransport::Consume(
	  const std::string& id,
	  const std::string& producerId,
	  const std::string& kind,
	  json* rtpParameters,
	  const json& appData)
	{
		MSC_TRACE();

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("RecvTransport closed");
		else if (id.empty())
			MSC_THROW_TYPE_ERROR("missing id");
		else if (producerId.empty())
			MSC_THROW_TYPE_ERROR("missing producerId");
		else if (kind != "audio" && kind != "video")
			MSC_THROW_TYPE_ERROR("invalid kind");
		else if (!rtpParameters)
			MSC_THROW_TYPE_ERROR("missing rtpParameters");
		else if (!appData.is_object())
			MSC_THROW_TYPE_ERROR("appData must be a JSON object");
		else if (!ortc::canReceive(*rtpParameters, *this->extendedRtpCapabilities))
			MSC_THROW_UNSUPPORTED_ERROR("cannot consume this Producer");

		// May throw.
		auto recvResult = this->recvHandler->Receive(id, kind, rtpParameters);

        /*		auto* consumer = new Consumer(
		  this,
		  consumerListener,
		  id,
		  recvResult.localId,
		  producerId,
		  recvResult.rtpReceiver,
		  recvResult.track,
		  *rtpParameters,
		  appData);

          this->consumers[consumer->GetId()] = consumer;*/

		// If this is the first video Consumer and the Consumer for RTP probation
		// has not yet been created, create it now.
		if (!this->probatorConsumerCreated && kind == "video")
		{
			try
			{
				auto probatorRtpParameters =
				  ortc::generateProbatorRtpParameters(*rtpParameters);
				std::string probatorId{ "probator" };

				// May throw.
				auto result = this->recvHandler->Receive(probatorId, kind, &probatorRtpParameters);

				MSC_DEBUG("Consumer for RTP probation created");

				this->probatorConsumerCreated = true;
			}
			catch (std::runtime_error& error)
			{
				MSC_ERROR("failed to create Consumer for RTP probation: %s", error.what());
			}
		}

        return recvResult;
	}

	/**
	 * Create a DataConsumer.
	 */
    Handler::DataChannel RecvTransport::ConsumeData(
	  const std::string& id,
	  const std::string& producerId,
	  const uint16_t streamId,
	  const std::string& label,
	  const std::string& protocol,
	  const nlohmann::json& appData)
	{
		MSC_TRACE();

		webrtc::DataChannelInit dataChannelInit;
		dataChannelInit.protocol = protocol;
		dataChannelInit.id       = streamId;

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("RecvTransport closed");
		else if (id.empty())
			MSC_THROW_TYPE_ERROR("missing id");
		else if (producerId.empty())
			MSC_THROW_TYPE_ERROR("missing producerId");
		else if (!this->hasSctpParameters)
			MSC_THROW_TYPE_ERROR("Cannot use DataChannels with this transport. SctpParameters are not set.");

		// This may throw.
		auto recvResult = this->recvHandler->ReceiveDataChannel(label, dataChannelInit);
        
        return recvResult;
        /*
		auto dataConsumer = new DataConsumer(
		  listener, this, id, producerId, recvResult.dataChannel, recvResult.sctpStreamParameters, appData);

		this->dataConsumers[dataConsumer->GetId()] = dataConsumer;

		return dataConsumer;*/
	}

	void RecvTransport::Close()
	{
		MSC_TRACE();

		if (this->closed)
			return;

		Transport::Close();
	}

	void RecvTransport::CloseConsumer(const std::string& localId)
	{
		MSC_TRACE();

		if (this->closed)
			return;

		// May throw.
		this->recvHandler->StopReceiving(localId);
	}


	json RecvTransport::GetConsumerStats(const std::string& localId)
	{
		MSC_TRACE();

		if (this->closed)
			MSC_THROW_INVALID_STATE_ERROR("RecvTransport closed");

		return this->recvHandler->GetReceiverStats(localId);
	}
} // namespace mediasoupclient
