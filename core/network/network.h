#ifndef _GSF_NEWWORK_HEADER_
#define _GSF_NEWWORK_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>

#include "network_config.h"

namespace gsf
{
	class NetworkThread;

	typedef std::shared_ptr<NetworkThread> NetworkThreadPtr;

	//! 网络模块核心类
	class Network
	{
	public:
		Network(const NetworkConfig &config);

		~Network();

		int start();

		int run();

	private:
		//! 设置工作线程
		int32_t setup_thread(NetworkThreadPtr threadPtr);

		static void worker_thread_process(int32_t fd, int16_t which, void *arg);

	private:

		NetworkThreadPtr main_thread_ptr_;
	
		std::vector<NetworkThreadPtr> worker_thread_vec_;

		const NetworkConfig config_;
	};
}


#endif
