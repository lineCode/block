#ifndef _GSF_MYSQL_PROXY_HEADER_
#define _GSF_MYSQL_PROXY_HEADER_

#include <core/module.h>
#include <core/event.h>

#include "mysql_connect.h"

namespace gsf
{
	namespace modules
	{
		class MysqlProxyModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:
			MysqlProxyModule();
			virtual ~MysqlProxyModule();

			MysqlProxyModule(const MysqlProxyModule &) = delete;
			MysqlProxyModule & operator = (const MysqlProxyModule &) = delete;

		protected:
			void before_init() override;

			void init() override;

			void execute() override;

			void shut() override;

			void after_shut() override;

		private:
			
			void event_init(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

			void event_query(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

			void event_update(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			//gsf::ArgsPtr execute_event(const gsf::ArgsPtr &args);

			// 03-05
			//void event_callback(gsf::ModuleID target, const gsf::ArgsPtr &args);

		private:
			gsf::ModuleID log_m_ = gsf::ModuleNil;

			MysqlConnect conn_;
		};
	}
}

#endif // !_GSF_MYSQL_PROXY_HEADER_
