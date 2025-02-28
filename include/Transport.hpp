#ifndef MSC_TRANSPORT_HPP
#define MSC_TRANSPORT_HPP

#include "Handler.hpp"

#include <json.hpp>
#include <api/media_stream_interface.h>    // webrtc::MediaStreamTrackInterface
#include <api/peer_connection_interface.h> // webrtc::PeerConnectionInterface
#include <api/rtp_parameters.h>            // webrtc::RtpEncodingParameters

#include <future>
#include <map>
#include <memory> // unique_ptr
#include <string>

namespace mediasoupclient
{
	// Fast forward declarations.
	class Device;

	class Transport : public Handler::PrivateListener
	{
	public:
		/* Public Listener API */
		class Listener
		{
		public:
            virtual ~Listener() {}
			virtual std::future<void> OnConnect(Transport* transport, const nlohmann::json& dtlsParameters) = 0;
			virtual void OnConnectionStateChange(Transport* transport, const std::string& connectionState) = 0;
		};

		/* Only child classes will create transport intances */
	protected:
		Transport(
		  Transport::Listener* listener,
		  const std::string& id,
		  const nlohmann::json* extendedRtpCapabilities,
		  const nlohmann::json& appData);

	public:
        virtual ~Transport() {}
   		virtual void Close();
        
		const std::string& GetId() const;
		bool IsClosed() const;
		const std::string& GetConnectionState() const;
		nlohmann::json& GetAppData();
		nlohmann::json GetStats() const;
		void RestartIce(const nlohmann::json& iceParameters);
		void UpdateIceServers(const nlohmann::json& iceServers);
        bool GetFingerprint(std::string& algorithm, std::string& fingerprint);
        Listener* GetListener();

	protected:
		void SetHandler(Handler* handler);

		/* Pure virtual methods inherited from Handler::PrivateListener */
	public:
		void OnConnect(nlohmann::json& dtlsParameters) override;
		void OnConnectionStateChange(
		  webrtc::PeerConnectionInterface::IceConnectionState connectionState) override;

	protected:
		// Closed flag.
		bool closed{ false };
		// Extended RTP capabilities.
		const nlohmann::json* extendedRtpCapabilities{ nullptr };
		// SCTP max message size if enabled, null otherwise.
		size_t maxSctpMessageSize{ 0u };
		// Whether the Consumer for RTP probation has been created.
		bool probatorConsumerCreated{ false };
		// Whether this transport supports DataChannel.
		bool hasSctpParameters{ false };

        rtc::scoped_refptr<rtc::RTCCertificate> certificate;
	private:
		// Listener.
		Listener* listener{ nullptr };
		// Id.
		std::string id;
		// Transport (IceConneciton) connection state.
		webrtc::PeerConnectionInterface::IceConnectionState connectionState{
			webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionNew
		};
		// Handler.
		Handler* handler{ nullptr };
		// App custom data.
		nlohmann::json appData = nlohmann::json::object();
	};

	class SendTransport : public Transport
	{
	private:
		SendTransport(
          Transport::Listener* listener,
		  const std::string& id,
		  const nlohmann::json& iceParameters,
		  const nlohmann::json& iceCandidates,
		  const nlohmann::json& dtlsParameters,
		  const nlohmann::json& sctpParameters,
		  const PeerConnection::Options* peerConnectionOptions,
		  const nlohmann::json* extendedRtpCapabilities,
		  const std::map<std::string, bool>* canProduceByKind,
		  const nlohmann::json& appData);

		/* Device is the only one constructing Transports. */
		friend Device;

	public:
        SendHandler::SendResult Produce(
		  webrtc::MediaStreamTrackInterface* track,
		  const std::vector<webrtc::RtpEncodingParameters>* encodings,
		  const nlohmann::json* codecOptions,
		  const nlohmann::json* codec,
		  const nlohmann::json& appData = nlohmann::json::object());

        Handler::DataChannel ProduceData(
		  const std::string& label      = "",
		  const std::string& protocol   = "",
		  bool ordered                  = true,
		  int maxRetransmits            = 0,
		  int maxPacketLifeTime         = 0,
		  const nlohmann::json& appData = nlohmann::json::object());

		/* Virtual methods inherited from Transport. */
	public:
		void Close() override;

		/* Virtual methods inherited from Producer::PrivateListener. */
	public:
		void CloseProducer(const std::string& localId);
		void ReplaceTrack(const std::string& localId, webrtc::MediaStreamTrackInterface* track);
		void SetMaxSpatialLayer(const std::string& localId, uint8_t maxSpatialLayer);
		nlohmann::json GetProducerStats(const std::string& localId);

	private:
		// Whether we can produce audio/video based on computed extended RTP
		// capabilities.
		const std::map<std::string, bool>* canProduceByKind{ nullptr };
		// SendHandler instance.
		std::unique_ptr<SendHandler> sendHandler;
	};

	class RecvTransport : public Transport
	{
	private:
		RecvTransport(
          Transport::Listener* listener,
		  const std::string& id,
		  const nlohmann::json& iceParameters,
		  const nlohmann::json& iceCandidates,
		  const nlohmann::json& dtlsParameters,
		  const nlohmann::json& sctpParameters,
		  const PeerConnection::Options* peerConnectionOptions,
		  const nlohmann::json* extendedRtpCapabilities,
		  const nlohmann::json& appData);

		/* Device is the only one constructing Transports */
		friend Device;

	public:
        RecvHandler::RecvResult Consume(
		  const std::string& id,
		  const std::string& producerId,
		  const std::string& kind,
		  nlohmann::json* rtpParameters,
		  const nlohmann::json& appData = nlohmann::json::object());

        Handler::DataChannel ConsumeData(
		  const std::string& id,
		  const std::string& producerId,
		  const uint16_t streamId,
		  const std::string& label,
		  const std::string& protocol   = std::string(),
		  const nlohmann::json& appData = nlohmann::json::object());

		/* Virtual methods inherited from Transport. */
	public:
		void Close() override;

		/* Virtual methods inherited from Consumer::PrivateListener. */
	public:
		void CloseConsumer(const std::string& localId);
		nlohmann::json GetConsumerStats(const std::string& localId);

	private:
		// SendHandler instance.
		std::unique_ptr<RecvHandler> recvHandler;
	};
} // namespace mediasoupclient
#endif
