#include <websocketpp/config/asio_client.hpp>

#include <websocketpp/client.hpp>

#include <iostream>
#include <chrono>

#include "WssClient.hpp"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> context_ptr;
typedef client::connection_ptr connection_ptr;

class WssClientImpl: public WssClient{
public:
    typedef WssClientImpl type;
    typedef std::chrono::duration<int,std::micro> dur_type;

    WssClientImpl () {
    	m_handle = {};
        m_endpoint.set_access_channels(websocketpp::log::alevel::all);
        m_endpoint.set_error_channels(websocketpp::log::elevel::all);

        // Initialize ASIO
        m_endpoint.init_asio();

        // Register our handlers
        m_endpoint.set_socket_init_handler(bind(&type::on_socket_init,this,::_1));
        m_endpoint.set_tls_init_handler(bind(&type::on_tls_init,this,::_1));
        m_endpoint.set_message_handler(bind(&type::on_message,this,::_1,::_2));
        m_endpoint.set_open_handler(bind(&type::on_open,this,::_1));
        m_endpoint.set_close_handler(bind(&type::on_close,this,::_1));
        m_endpoint.set_fail_handler(bind(&type::on_fail,this,::_1));
    }
private:
    void start(std::string uri) {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_endpoint.get_connection(uri, ec);
        if (ec) {
            m_endpoint.get_alog().write(websocketpp::log::alevel::app,ec.message());
            return;
        }
        //con->set_proxy("http://humupdates.uchicago.edu:8443");
        con->replace_header("Sec-WebSocket-Protocol","protoo");
        m_endpoint.connect(con);
        // Start the ASIO io_service run loop
        m_start = std::chrono::high_resolution_clock::now();
        m_endpoint.run();
    }

    void stop() {
    	m_endpoint.stop();
    }

    void on_socket_init(websocketpp::connection_hdl hdl) {
        m_socket_init = std::chrono::high_resolution_clock::now();
        m_handle = hdl;
    }

    context_ptr on_tls_init(websocketpp::connection_hdl) {
        m_tls_init = std::chrono::high_resolution_clock::now();
        context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);

        try {
            ctx->set_options(boost::asio::ssl::context::default_workarounds |
                             boost::asio::ssl::context::no_sslv2 |
                             boost::asio::ssl::context::no_sslv3 |
                             boost::asio::ssl::context::single_dh_use);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        return ctx;
    }

    void on_fail(websocketpp::connection_hdl hdl) {
        client::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);

        std::cout << "Fail handler" << std::endl;
        std::cout << con->get_state() << std::endl;
        std::cout << con->get_local_close_code() << std::endl;
        std::cout << con->get_local_close_reason() << std::endl;
        std::cout << con->get_remote_close_code() << std::endl;
        std::cout << con->get_remote_close_reason() << std::endl;
        std::cout << con->get_ec() << " - " << con->get_ec().message() << std::endl;
    }

    void on_open(websocketpp::connection_hdl) {
        m_open = std::chrono::high_resolution_clock::now();
        //m_endpoint.send(hdl, "", websocketpp::frame::opcode::text);
        if (listener_) {
        	listener_->onOpen();
        }
    }
    void on_message(websocketpp::connection_hdl , message_ptr msg) {
        m_message = std::chrono::high_resolution_clock::now();
        if (listener_) {
        	listener_->onMessage(msg->get_payload());
        }
    }
    void on_close(websocketpp::connection_hdl) {
        m_close = std::chrono::high_resolution_clock::now();

        std::cout << "Socket Init: " << std::chrono::duration_cast<dur_type>(m_socket_init-m_start).count() << std::endl;
        std::cout << "TLS Init: " << std::chrono::duration_cast<dur_type>(m_tls_init-m_start).count() << std::endl;
        std::cout << "Open: " << std::chrono::duration_cast<dur_type>(m_open-m_start).count() << std::endl;
        std::cout << "Message: " << std::chrono::duration_cast<dur_type>(m_message-m_start).count() << std::endl;
        std::cout << "Close: " << std::chrono::duration_cast<dur_type>(m_close-m_start).count() << std::endl;


    }
    void send_mseeage(std::string &msg) {
		m_endpoint.send(m_handle, msg, websocketpp::frame::opcode::text);

    }

public:
    void StartAndRun(std::string url) override {
    	sleep(1);
    	start(url);
    }
    void Quit() override {
    	m_endpoint.close(m_handle,websocketpp::close::status::going_away,"");
    	stop();
    }

    void Send(std::string msg) override {
    	send_mseeage(msg);
    }
private:
    client m_endpoint;
    websocketpp::connection_hdl m_handle;
    std::chrono::high_resolution_clock::time_point m_start;
    std::chrono::high_resolution_clock::time_point m_socket_init;
    std::chrono::high_resolution_clock::time_point m_tls_init;
    std::chrono::high_resolution_clock::time_point m_open;
    std::chrono::high_resolution_clock::time_point m_message;
    std::chrono::high_resolution_clock::time_point m_close;
};



WssClient::WssClient() : listener_(NULL){

}
WssClient::~WssClient() {

}

WssClient* WssClient::Create() {
	return new WssClientImpl();
}

void WssClient::setEventListener(WssEventListener* listener)
{
	listener_ = listener;
}

//
////#include "WssClient.hpp"
//int main(int argc, char* argv[]) {
//
//    std::string uri = "wss://127.0.0.1:4443/?roomId=9nhl6r9k&peerId=5rtgbhy";
//
//    if (argc == 2) {
//        uri = argv[1];
//    }
//
//    try {
//        WssClientImpl endpoint;
//        endpoint.start(uri);
//    } catch (websocketpp::exception const & e) {
//        std::cout << e.what() << std::endl;
//    } catch (std::exception const & e) {
//        std::cout << e.what() << std::endl;
//    } catch (...) {
//        std::cout << "other exception" << std::endl;
//    }
//}

