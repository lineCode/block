#ifndef _GSF_DISTRIBUTED_NODE_HEADER_
#define _GSF_DISTRIBUTED_NODE_HEADER_

#include <core/module.h>
#include <core/event.h>

#include <vector>
#include <map>
#include <tuple>
#include <functional>

namespace gsf
{
	namespace modules
	{
		struct CallbackInfo
		{
			gsf::RpcCallback callback;
			uint64_t timer_ = 0;
			uint32_t event_ = 0;
			int64_t id_ = 0;
		};
		typedef std::shared_ptr<CallbackInfo> CallbackPtr;

		struct ModuleInfo
		{
			std::string moduleName = "";
			gsf::ModuleID moduleID = gsf::ModuleNil;
			int32_t characteristic = 0;
		};

		struct NodeInfo
		{
			gsf::ModuleID connecotr_m_ = gsf::ModuleNil;
			std::string ip_ = "";
			int port_ = 0;
			int event_ = 0;
			gsf::ModuleID base_ = gsf::ModuleNil;
		};

		class NodeModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:

			NodeModule();
			virtual ~NodeModule();

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

		protected:

			void event_rpc(gsf::EventID event, gsf::ModuleID moduleid, const gsf::ArgsPtr &args, gsf::RpcCallback callback);

			void regist_node(gsf::ModuleID base, int event, const std::string &ip, int port);

			void event_create_node(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			void event_regist_node(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

		private:

			gsf::ModuleID log_m_ = gsf::ModuleNil;
			gsf::ModuleID timer_m_ = gsf::ModuleNil;
			gsf::SessionID connector_fd_ = gsf::SessionNil;

			std::string acceptor_ip_ = "";
			int32_t acceptor_port_ = 0;

			int32_t id_ = 0;
			std::string type_ = "";

			int32_t rpc_delay_ = 10000;
			bool service_ = false;
			gsf::ModuleID target_m_ = gsf::ModuleNil;

			std::vector<ModuleInfo> modules_;

			std::map<int64_t, CallbackPtr> callback_map_;
			std::map<uint64_t, CallbackPtr> timer_set_;

			std::map<int, NodeInfo> event_map_;
		};

	}
}


#endif