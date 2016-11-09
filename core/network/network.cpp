#include "network.h"

#include "network_thread.h"
#include "network_config.h"

#include "acceptor.h"
#include "session.h"

#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <errno.h>
#endif // WIN32

#include <event2/listener.h>
#include <event2/bufferevent.h>

#include <thread>



gsf::Network* gsf::Network::instance_ = NULL;

gsf::Network::Network()
{

}

gsf::Network::~Network()
{

}

gsf::Network& gsf::Network::instance()
{
	if (instance_ == NULL)
	{
		instance_ = new gsf::Network();
	}
	return *instance_;
}

int gsf::Network::init(const NetworkConfig &config)
{
	config_ = config;

	main_thread_ptr_ = std::make_shared<NetworkThread>();

	main_thread_ptr_->event_base_ptr_ = event_base_new();

	init_work_thread();

	return 0;
}

int gsf::Network::start()
{
	for (auto &worker : worker_thread_vec_)
	{
		worker->th = new std::thread(worker_thread_run, worker); //非空构造会直接启动线程
	}

	event_base_dispatch(main_thread_ptr_->event_base_ptr_);

	return 0;
}

int32_t gsf::Network::init_work_thread()
{
	for (int i = 0; i < config_.worker_thread_count_; ++i)
	{
		auto thread_ptr = std::make_shared<NetworkThread>();

		thread_ptr->event_base_ptr_ = event_base_new();
		
		thread_ptr->connect_queue_ = new CQ();
		NetworkConnect::instance().cq_init(thread_ptr->connect_queue_);

		evutil_socket_t pipe[2];
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, pipe) < 0){
                     printf("evutil_socketpair err!\n");
		}
		thread_ptr->notify_send_fd_ = pipe[1];
		thread_ptr->notify_receive_fd_ = pipe[0];
		evutil_make_socket_nonblocking(thread_ptr->notify_send_fd_);
		evutil_make_socket_nonblocking(thread_ptr->notify_receive_fd_);


		struct event *signal;
		signal = event_new(thread_ptr->event_base_ptr_
			, thread_ptr->notify_receive_fd_
			, EV_READ | EV_PERSIST
			, worker_thread_process
			, thread_ptr.get());
		if (signal){
			thread_ptr->notify_event_ = signal;
			event_add(signal, NULL);
		}

		worker_thread_vec_.push_back(thread_ptr);
	}

	return 0;
}

void gsf::Network::open_acceptor(AcceptorPtr acceptor_ptr)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
	sin.sin_port = htons(acceptor_ptr->get_config().port);

	acceptor_ptr_ = acceptor_ptr;

    ::evconnlistener *listener;

    listener = evconnlistener_new_bind(main_thread_ptr_->event_base_ptr_
			,accept_listen_cb
            ,(void*)this
            ,LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE
            ,-1
            ,(sockaddr*)&sin
            ,sizeof(sockaddr_in));
    if (NULL == listener){
        printf("evconnlistener new bind err!/n");
    }
}


static int last_thread = -1;

void gsf::Network::accept_conn_new(evutil_socket_t fd)
{
	CQ_ITEM *item = NetworkConnect::instance().cqi_new();
	if (!item){
		printf("cqi_new err!\n");
		return;
	}
	
	int tid = (last_thread + 1) % config_.worker_thread_count_;
	
	auto thread_ptr = worker_thread_vec_[tid];
	
	last_thread = tid;
	
	item->sfd = fd;
	//temp
	item->ptr = acceptor_ptr_.get();
	
	NetworkConnect::instance().cq_push(thread_ptr->connect_queue_, item);

	char buf[1];
	buf[0] = 'c';
	if (send(thread_ptr->notify_send_fd_, buf, 1, 0) < 0){
		printf("pipe send err!\n"); //evutil_socket_geterror
	}
}

void gsf::Network::worker_thread_process(evutil_socket_t fd, short event, void * arg)
{
	//! just use point not copy
	NetworkThread *threadPtr = static_cast<NetworkThread*>(arg);

	char buf[1];
	int n = recv(fd, buf, 1, 0);
	if (n == -1){
		int err = evutil_socket_geterror(fd);
                printf("pipe recv err!\n");
	}

	switch (buf[0])
	{
	case 'c':
		CQ_ITEM *item = NetworkConnect::instance().cq_pop(threadPtr->connect_queue_);
		if (item){
			::bufferevent *bev;
			bev = bufferevent_socket_new(threadPtr->event_base_ptr_, item->sfd, BEV_OPT_CLOSE_ON_FREE);
			if (!bev){
				printf("bufferevent_socket_new err!\n");
			}

			Acceptor *acceptor_ptr = static_cast<Acceptor*>(item->ptr);
			Session *session = acceptor_ptr->make_session(bev, item->sfd);
			bufferevent_setcb(bev, Session::read_cb, NULL, Session::err_cb, session);
            bufferevent_enable(bev, EV_WRITE);
            bufferevent_enable(bev, EV_READ);
			
			//send connection event
			acceptor_ptr->handler_new_connect(session->getid());
		}
		break;
	}
}

void gsf::Network::worker_thread_run(NetworkThreadPtr thread_ptr)
{
	event_base_dispatch(thread_ptr->event_base_ptr_);
}


void gsf::Network::accept_listen_cb(::evconnlistener *listener, evutil_socket_t fd, sockaddr *sa, int socklen, void *arg)
{
	gsf::Network *network = (gsf::Network*)(arg);
	network->accept_conn_new(fd);
}
