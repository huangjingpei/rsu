#ifndef __ROOM_CLIENT_HPP__
#define __ROOM_CLIENT_HPP__
#include <memory>
#include <list>
#include <mutex>
#include "WssClient.hpp"
#include "Participant.hpp"

#include "mediasoupclient.hpp"
#include "json.hpp"

class RoomClient: public WssEventListener,
		public mediasoupclient::RecvTransport::Listener,
		public mediasoupclient::Consumer::Listener {

	enum {
		wss_none						= 0,
		wss_getRouterRtpCapabilities 	= 1,
		wss_createWebRtcTransport 		= 2,
		wss_join						= 3,
	};
public:
	RoomClient();
	virtual ~RoomClient();

	int AddParticipant(std::string name);
	int RemoveParticipant(std::string name);

	bool RunAndWaitForever(std::string wss);
private:
	void setWssListener();
public:

	/* Virtual methods inherited from RecvTransport::Listener. */
	std::future<void> OnConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters) override;
	void OnConnectionStateChange(mediasoupclient::Transport* transport, const std::string& connectionState) override;

	/* Virtual methods inherited from Consumer::Listener. */
	void OnTransportClose(mediasoupclient::Consumer* consumer) override;

	/* Virtual methods inherited from WssEventListener. */
	void onMessage(std::string msg) override;
	void onOpen() override;

	void GetRouterRtpCapabilities();
	void CreateWebRtcTransport();
	void Join();


private:

	std::mutex lock_;
	std::list<Participant> participants_;
	std::unique_ptr<WssClient> wss_client_;

	mediasoupclient::Device device;

	std::string id;
	std::string transportId;
	std::string baseUrl;
	int wss_state_;


//	static std::map<int , std::string> s_method_ = {
//			{wss_none						,"none"},
//			{wss_getRouterRtpCapabilities 	,"getRouterRtpCapabilities"},
//			{wss_createWebRtcTransport 		,"createWebRtcTransport"},
//			{wss_join						,"join"}
//
//	};
};
#endif//__ROOM_CLIENT_HPP__
