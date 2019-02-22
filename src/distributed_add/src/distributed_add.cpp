#include <hpx/hpx_main.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/lcos/local/spinlock.hpp>

int distributed_add(const int& a, const int& b) {
	char const* msg = "hello from OS-thread {1} on locality {2}.\n a = {3} b = {4}\n";
	std::size_t current = hpx::get_worker_thread_num();

	hpx::util::format_to(hpx::cout, msg, current, hpx::get_locality_id(), a, b)
		<< hpx::flush;
	return a + b;
}

HPX_PLAIN_ACTION(distributed_add, distributed_add_action);

int main() {
	// get a list of all available localities
	std::vector<hpx::id_type> localities = hpx::find_all_localities();

	typedef distributed_add_action action_type;
	hpx::future<int> future = hpx::async<action_type>(localities[1], 1, 2);

	future.wait();
	size_t add_res = future.get();

	char const* msg = "hello from OS-thread {1} on locality {2}.\n distributed_add_res = {3}\n";
	hpx::util::format_to(hpx::cout, msg, hpx::get_worker_thread_num(), hpx::get_locality_id(), add_res)
		<< hpx::flush;

	return 0;
}