# libmediasouphandler

MeidaSoup client for ios&android.


# Compile

* Fetch webrtc and build it for ios or android
* Put this repo into webrtc src directory.
* Apply a patch to webrtc src to change BUILD.gn.
  In the webrtc src directory, checkout M113 (commit id:f6ab0b438e22ea30db7ad3fbf9f870b0d4506235), git apply libmediasoupclient/build.patch
* Build webrtc again, the webrtc library contains the api for mediasoup.


# Android API

```
import org.mediasoup.Device;
import org.mediasoup.Fingerprint;
import org.mediasoup.MediaSoupClient;
import org.mediasoup.RecvTransport;
import org.mediasoup.SendTransport;
import org.mediasoup.Transport;



//init webrtc
PeerConnectionFactory.initialize(...)

//init mediasoupclient
MediaSoupClient.initialize();

//create peerconnectionfactory
PeerConnectionFactory factory = ...

//get router rtp capabilities from server
routeRtpCapabilities = ...

//load device
Device device = new Device();
device.load(routerRtpCapabilities, rtcConfig, factory);


//create send webrtc transport by server api 
String id = ...
String iceParameters = ...
String iceCandidates = ...
String dtlsParameters = ...

SendTransport sendTransport = device.createSendTransport(id, iceParameters, iceCandidates, dtlsParameters, rtcConfig, factory);



//create recv webrtc transport by server api
String id = ...
String iceParameters = ...
String iceCandidates = ...
String dtlsParameters = ...

RecvTransport recvTransport = device.createSendTransport(id, iceParameters, iceCandidates, dtlsParameters, rtcConfig, factory);



//connect webrtc transport with the certificate fingerprint

//send:"server", recv:"client"
String localDtlsRole = "server" if "send" else "client";

JSONObject fingerprint = new JSONObject();
Fingerprint fp = transport.getFingerprint();
fingerprint.put("algorithm", fp.algorithm);
fingerprint.put("value", fp.fingerprint);

JSONArray fingerprints = new JSONArray();
fingerprints.put(fingerprint);

JSONObject dtlsParameters = new JSONObject();
dtlsParameters.put("role", localDtlsRole);
dtlsParameters.put("fingerprints", fingerprints);

JSONObject object = new JSONObject();
object.put("transportId", transport.getId());
object.put("dtlsParameters", dtlsParameters);



//produce audio/video
MediaStreamTrack track = ...
SendTransport.SendResult sendResult = sendTransport.produce(track, [], nil, nil);

//consume audio/video
String id = ...
String producderId = ...
String kind = ...
JSONObject rtpParameters = ...

RecvTransport.RecvResult = recvTransport.consume(id, producerId, kind, rtpParameters.toString());
if (kind.equals("video")) {
    VideoTrack videoTrack = recvResult.track;
    //render the video track
}


```


# IOS API
```
#import <WebRTC/MSDevice.h>
#import <WebRTC/MSSendTransport.h>
#import <WebRTC/MSRecvTransport.h>
#import <WebRTC/MSClient.h>

//init webrtc
....

//init MSClient
//cause [MSClient initialize] be called
NSLog(@"mediasoup client version:%@", [MSClient version]);


//create peerconnectionfactory
RTCPeerConnectionFactory *factory = ...

//get router rtp capabilities from server
routeRtpCapabilities = ...


//load device
MSDevice *device = [[MSDevice alloc] init]
[device.load:routerRtpCapabilities rtcConfig:rtcConfig factory:factory];


//create send webrtc transport by server api
MSSendTransport *sendTransport = [device createSendTransport:transportId
                                                iceParameters:iceParameters
                                                iceCandidates:iceCandidates
                                               dtlsParameters:dtlsParameters
                                                    rtcConfig:rtcConfiguration
                                                      factory:factory];

//create recv webrtc transport by server api
MSRecvTransport *recvTransport = [device createRecvTransport:transportId
                                           iceParameters:iceParameters
                                           iceCandidates:iceCandidates
                                          dtlsParameters:dtlsParameters
                                               rtcConfig:rtcConfiguration
                                                 factory:factory];



//connect webrtc transport with the certificate fingerprint
MSFingerprint *fp = [transport fingerprint];
NSDictionary *fingerprint = @{@"algorithm":fp.algorithm, @"value":fp.fingerprint};
NSDictionary *dtlsParameters = @{@"role":localDtlsRole, @"fingerprints":@[fingerprint]};

NSDictionary *data = @{@"transportId":transport.transportId, @"dtlsParameters":dtlsParameters};


//produce audio/video

[sendTransport produce:track endodings:@[] codecOptions:nil codec:nil];


//consume audio/video

[recvTransport consume:id_ producerId:producerId kind:kind rtpParameters:rtpParameters]


```

## Website and Documentation

* [mediasoup.org][mediasoup-website]


## Support Forum

* [mediasoup.discourse.group][mediasoup-discourse]


## Authors

* José Luis Millán [[website](https://jssip.net)|[github](https://github.com/jmillan/)]
* Iñaki Baz Castillo [[website](https://inakibaz.me)|[github](https://github.com/ibc/)]



## License

[ISC](./LICENSE)




[mediasoup-website]: https://mediasoup.org
[mediasoup-discourse]: https://mediasoup.discourse.group
