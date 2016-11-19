#include "session.h"
#include "network_thread.h"

#include <event2/buffer.h>


#include <iostream>

gsf::Session::Session(int32_t id, ::bufferevent *bev, int fd)
    : id_(id)
    , bev_(bev)
    , fd_(fd)
	, session_handler_(nullptr)
	, close_handler_(nullptr)
	, need_write_(false)
	, need_read_(false)
{
}

gsf::Session::~Session()
{
	
}

int gsf::Session::open(SessionHandler *session_handler, SessionCloseHandler *close_handler)
{
	session_handler_ = session_handler;
	close_handler_ = close_handler;

    in_buf_ = evbuffer_new();
    out_buf_ = evbuffer_new();

	return 0;
}

void gsf::Session::read_cb(::bufferevent *bev, void *ctx)
{
	Session *_session_ptr = static_cast<Session*>(ctx);

	bufferevent_read_buffer(bev, _session_ptr->get_inbuf());


}

void gsf::Session::err_cb(::bufferevent *bev, short what, void *ctx)
{
	if (what & BEV_EVENT_EOF)  
	{  
		/* connection has been closed, do any clean up here */  
		printf("connection closed\n");  
	}  
	else if (what & BEV_EVENT_ERROR)  
	{  
		/* check errno to see what error occurred */  
		printf("some other error\n");  
	}  
	else if (what & BEV_EVENT_TIMEOUT)  
	{  
		/* must be a timeout event handle, handle it */  
		printf("Timed out\n");  
	}  
	bufferevent_free(bev);
}

int gsf::Session::write(const char *data, int len)
{
	int _ret = need_write_ ? 1 : 0;

	evbuffer_add(out_buf_, data, len);
	need_write_ = true;

	return _ret;
}

void gsf::Session::write_impl()
{
	if (need_write_){
		evbuffer_write(out_buf_, fd_);
		need_write_ = false;
	}

}
