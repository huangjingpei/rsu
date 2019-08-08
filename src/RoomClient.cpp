#include <iostream>
#include <string>
#include "RoomClient.hpp"
#include "Utils.hpp"

using json = nlohmann::json;


#define RandomInt32 (mediasoupclient::Utils::getRandomInteger(1000009, 9999999))
RoomClient::RoomClient()
{
	wss_client_.reset(WssClient::Create());
	wss_state_ = wss_none;

}

RoomClient::~RoomClient() {

}


int RoomClient::AddParticipant(std::string name)
{
	return -1;
}

int RoomClient::RemoveParticipant(std::string name)
{
	return -1;
}

bool RoomClient::RunAndWaitForever(std::string wss)
{
	wss_client_->setEventListener(this);
	wss_client_->StartAndRun(wss);
	return true;
}

void RoomClient::onOpen()
{
	GetRouterRtpCapabilities();
}


void RoomClient::GetRouterRtpCapabilities()
{
	json body =
	{
		{"request",	true},
		{"id",		RandomInt32},
		{"method",	"getRouterRtpCapabilities"},
		{"data",	{}}
	};

	wss_client_->Send(body.dump());
	wss_state_ = wss_getRouterRtpCapabilities;
}

void RoomClient::CreateWebRtcTransport()
{
	json body =
	{
		{"request", 			true},
		{"id", 					RandomInt32},
		{"method", 				"createWebRtcTransport"},
		{"data", 				{
								{"forceTcp", false},
								{"producing", false},
								{"consuming", true},
								{"sctpCapabilities", {
														"numStreams", {
															{"OS", 1024},
															{"MIS", 1024}
														}
													}
								}
							}
		}
	};

	printf("send %s:%s\n", __FUNCTION__, body.dump().c_str());
	wss_client_->Send(body.dump());
	wss_state_ = wss_createWebRtcTransport;
}

void RoomClient::Join()
{
	json body =
	{
			{"request",				true},
			{"id",				RandomInt32},
			{"method",			"join"},
			{"data",			{
				{"displayName","record"},
				{"device",{
							{"flag","chrome"},
							{"name","Chrome"},
							{"version","75.0.3770.100"}
				}

			}
		}
	}

	};
	printf("send %s:%s\n", __FUNCTION__, body.dump().c_str());

	wss_client_->Send(body.dump());
	wss_state_ = wss_join;

}


void RoomClient::onMessage(std::string msg)
{
	printf("recv msg:%s\n", msg.c_str());
	auto response = json::parse(msg);
	auto it = response.find("id");
	if (it == response.end()) {
		return ;
	}
	auto resp = response.find("response");
	if (resp != response.end()) {
		auto method = response.find("method");
		if (method != response.end()) {
		}
		printf("wss_state_ %d\n", wss_state_);
		if (wss_state_ == wss_getRouterRtpCapabilities) {
			auto routerRtpCapabilities = nlohmann::json::parse(msg);
			//this->device.Load(routerRtpCapabilities);
			CreateWebRtcTransport();
			return;
		} else if (wss_state_ == wss_createWebRtcTransport) {
			Join();
			return;
		}
		if (*resp == "createWebRtcTransport") {

		}
	} else { //it should be a notification. yes or no?

	}


}


void RoomClient::OnTransportClose(mediasoupclient::Consumer* consumer)
{
	std::cout << "OnTransportClose" << std::endl;

}


std::future<void> RoomClient::OnConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters)
{
	std::promise<void> promise;
	//TODO
	promise.set_value();

	return promise.get_future();
}
void RoomClient::OnConnectionStateChange(mediasoupclient::Transport* transport, const std::string& connectionState)
{
	std::cout << "OnConnectionStateChange: connectionState: " << connectionState << std::endl;

}

