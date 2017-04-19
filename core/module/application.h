#ifndef _GSF_APPLICATION_HEADER_
#define _GSF_APPLICATION_HEADER_

#include <event/event.h>
#include "module.h"

#include <stdint.h>
#include <list>
#include <unordered_map>
#include <map>

namespace gsf
{
	class Application
		: public Module
		, public IEvent
	{

	public:
		Application();

		void init_args();

		void init() override;

		template <typename T>
		void regist_module(T *module, bool dynamic = false);

		template <typename T>
		uint32_t find_module_id();

		template <typename M, typename T>
		void sendmsg(IEvent *event_ptr, uint32_t fd, uint32_t msg_id, T msg);

		void run();

		virtual void tick() {}

		void exit();

	protected:
		uint32_t delay_;

		//！ 临时先写在这里，未来如果支持分布式可能要放在其他地方生成，保证服务器集群唯一。
		uint32_t make_module_id();

	private:

		std::list<Module *> module_list_;
		std::unordered_map<uint32_t, uint32_t> module_id_map_;
		std::unordered_map<std::string, uint32_t> module_name_map_;

		bool shutdown_;

		uint32_t module_idx_;
	};

	template <typename M, typename T>
	void gsf::Application::sendmsg(IEvent *event_ptr, uint32_t fd, uint32_t msg_id, T msg)
	{
		uint32_t _nid = find_module_id<M>();

		int _len = msg.ByteSize();
		auto _msg = std::make_shared<gsf::Block>(fd, msg_id, _len);

		msg.SerializeToArray(_msg->buf_ + _msg->pos_, _len);

		event_ptr->dispatch_remote(_nid, fd, msg_id, _msg);
	}

	template <typename T>
	void gsf::Application::regist_module(T *module, bool dynamic /* = false */)
	{
		auto _type_id = typeid(T).hash_code();
		auto _id_itr = module_id_map_.find(_type_id);
		if (_id_itr != module_id_map_.end()){
			printf("regist repeated module!\n");
			return;
		}

		module_list_.push_back(module);
		module->set_id(make_module_id());

		module_id_map_.insert(std::make_pair(_type_id, module->get_module_id()));
		if (!dynamic) {
			module_name_map_.insert(std::make_pair(module->get_module_name(), module->get_module_id()));
		}
	}

	template <typename T>
	uint32_t gsf::Application::find_module_id()
	{
		auto _type_id = typeid(T).hash_code();
		auto _id_itr = module_id_map_.find(_type_id);
		if (_id_itr != module_id_map_.end()){
			return _id_itr->second;
		}

		return 0;
	}
}

#endif
