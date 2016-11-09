#ifndef _GSF_ACCEPTOR_HEADER_
#define _GSF_ACCEPTOR_HEADER_

#include <stdint.h>
#include <string>

#include <unordered_map>

#include <event2/bufferevent.h>

namespace gsf
{
	class AcceptHandler;
    class Session;

	struct AcceptorConfig
	{
		int32_t port;
		std::string address;
	};

	class Acceptor
	{
	public:
		Acceptor(AcceptorConfig &config, AcceptHandler *handler);
		~Acceptor();

		Session * make_session(::bufferevent *bev, int fd);

        void handler_new_connect(int32_t session_id);

		AcceptorConfig &get_config();

	private:
		AcceptorConfig config_;
		AcceptHandler *handler_;

		typedef std::unordered_map<int32_t, Session*> SessionQueue;
		SessionQueue session_queue_;

		int32_t id_;
	};
}


#endif
