#ifndef __WSS_CLIENT_HPP__
#define __WSS_CLIENT_HPP__
#include <string>

class WssEventListener
{
public:
	virtual ~WssEventListener() {};
	virtual void onMessage(std::string msg) = 0;
	virtual void onOpen() = 0;

};

class WssClient {
public:
	WssClient();
	virtual ~WssClient();
	static WssClient* Create();
	virtual void StartAndRun(std::string url) = 0;
	virtual void Quit() = 0;
	virtual void Send(std::string msg) = 0;
	void setEventListener(WssEventListener* listener);

protected:
	WssEventListener *listener_;
};

#endif
