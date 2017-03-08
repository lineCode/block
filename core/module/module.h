#ifndef _GSF_MODULE_HEADER_
#define _GSF_MODULE_HEADER_

#include <stdint.h>
#include <vector>

namespace gsf
{
	class Module
	{
		friend class Application;
	public:
		Module();
		virtual ~Module();

		uint32_t get_module_id() const { return module_id_; }

		std::pair<uint32_t, uint32_t> make_event(uint32_t event)
		{
			return std::make_pair(get_module_id(), event);
		}

		std::pair<uint32_t, uint32_t> make_event(uint32_t module_id, uint32_t event)
		{
			return std::make_pair(module_id, event);
		}

	protected:
		virtual void before_init();
		virtual void init();

		virtual void execute();

		virtual void shut();
		virtual void after_shut();

	protected:
		void set_id(uint32_t id) { module_id_ = id; }
		uint32_t module_id_;
	};
}


#endif
