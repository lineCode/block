#ifndef _GSF_LOG_HEADER_
#define _GSF_LOG_HEADER_

#include <core/module.h>
#include <core/event.h>

#include <list>

namespace gsf
{
	namespace modules
	{
		class LogModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:
			LogModule();
			~LogModule() {}

			void before_init() override;

			void init() override;

			void execute() override;

			void shut() override;

		private:

			void init_impl(const std::string &exe_name);

			void event_print(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			void event_change_flag(gsf::ArgsPtr args, gsf::CallbackFunc callback =nullptr);

		private:
			bool ndebug = false;
			char path_[512];
		};
	}
}

#endif
