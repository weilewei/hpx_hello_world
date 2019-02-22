#if !defined(REMOTE_ADD_COMPONENT_HPP)
#define REMOTE_ADD_COMPONENT_HPP

#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

#include <utility>

namespace examples {
	namespace server
	{
		struct HPX_COMPONENT_EXPORT remote_increment
			: hpx::components::component_base<remote_increment>
		{
			void print();
			void add(int arg);
			HPX_DEFINE_COMPONENT_ACTION(remote_increment, print);
			HPX_DEFINE_COMPONENT_ACTION(remote_increment, add);
		private:
			int a = 0;
		};
	}
}

HPX_REGISTER_ACTION_DECLARATION(
	examples::server::remote_increment::print_action, 
	remote_increment_print_action);

HPX_REGISTER_ACTION_DECLARATION(
	examples::server::remote_increment::add_action,
	remote_increment_add_action);

namespace examples
{
	struct remote_increment
		: hpx::components::client_base<remote_increment, server::remote_increment>
	{
		typedef hpx::components::client_base<remote_increment, server::remote_increment>
			base_type;

		// Create a client side representation for the existing
        // \a server::accumulator instance with the given GID.
		remote_increment(hpx::future<hpx::id_type> && id)
			: base_type(std::move(id))
		{}

		remote_increment(hpx::id_type && id)
			: base_type(std::move(id))
		{}

		void print()
		{
			hpx::async<server::remote_increment::print_action>(this->get_id()).get();
		}

		void add(int arg)
		{
			hpx::async<server::remote_increment::add_action>(this->get_id(), arg).get();
		}
	};
}

#endif // REMOTE_ADD_COMPONENT_HPP