#include <hpx/include/iostreams.hpp>
#include <hpx/hpx.hpp>
#include <hpx/runtime/components/component_factory.hpp>

#include "app_server_component.hpp"

#include <iostream>

namespace examples {
	namespace server
	{
		void remote_increment::print()
		{
			char const* msg = "remote_increment is computed from OS-thread {1} on locality {2}. The result is {3}\n";
			std::size_t current = hpx::get_worker_thread_num();
			hpx::util::format_to(hpx::cout, msg, current, hpx::get_locality_id(), a)
				<< hpx::flush;
		}
		void remote_increment::add(int arg) {
			a += arg;
		}
	}
}

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::component<
	examples::server::remote_increment
> remote_increment_type;

HPX_REGISTER_COMPONENT(remote_increment_type, remote_increment);

HPX_REGISTER_ACTION(
	examples::server::remote_increment::print_action, 
	remote_increment_print_action);

HPX_REGISTER_ACTION(
	examples::server::remote_increment::add_action, 
	remote_increment_add_action);