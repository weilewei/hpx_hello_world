#include <iostream>
#include <hpx/include/actions.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <unordered_map>
#include <hpx/lcos/local/spinlock.hpp>
#include <hpx/include/lcos.hpp>

using mutex_type = hpx::lcos::local::spinlock;

// base function saying hello world
// if the hello world fucntion is not running expected thread,
// return negative, and say i failed; otherwise 
// (running expected thread), return the current thread number
std::size_t hello_world(std::size_t expected) {
	std::size_t current = hpx::get_worker_thread_num();
	auto locality = hpx::get_locality_id();
	if (expected == current) {
		char const* msg = "hello world from OS-thread {1} on locality {2}\n";

		hpx::util::format_to(hpx::cout, msg, expected, hpx::get_locality_id())
			<< hpx::flush;
		return expected;
	}
	//std::cout << "i failed from thread" << current << "\n";
	return (-1);
}

HPX_PLAIN_ACTION(hello_world, hello_world_action);

// rescheduler 
void hello_world_foreman() {
	// get current avaliable threads
	std::size_t os_threads_count = hpx::get_os_thread_count();
	std::map<size_t, size_t> os_threads;

	// populate a map with the OS-threads numbers of all OS-threads on this locality.
	// When the hello world msg has been printed on a particular OS-thread, we will remove
	// it from the map
	for (std::size_t i = 0; i < os_threads_count; i++) {
		os_threads[i] = i;
	}

	// Find the global name of the current locality.
	hpx::naming::id_type const here = hpx::find_here();

	// As long as the map is not empty, keep scheduling threads
	while (!os_threads.empty()) {
		// launch future tasks asychronously from those threads 
	    // that are not matched with expected threads
		std::vector<hpx::future<std::size_t> > futures;
		for (auto each : os_threads) {
			if (each.second != -1) {
				typedef hello_world_action action_type;
				futures.push_back(hpx::async<action_type>(here, each.second));
			}
		}

		// mutex is the gate-keeper
		hpx::lcos::local::spinlock mtx;
		hpx::lcos::wait_each(
			hpx::util::unwrapping([&](std::size_t t) {
			if (std::size_t(-1) != t)
			{
				std::lock_guard<hpx::lcos::local::spinlock> lk(mtx);
				os_threads.erase(t);
			}
		}),
			futures);
	}
}

HPX_PLAIN_ACTION(hello_world_foreman, hello_world_foreman_action);

int main() {

	// get a list of all available localities
	std::vector<hpx::naming::id_type> localities = hpx::find_all_localities();

	// reserve storage space for futures, one for each locality
	std::vector<hpx::lcos::future<void> > futures;
	futures.reserve(localities.size());

	for (hpx::naming::id_type const& node : localities)
	{
		// Asynchronously start a new task. The task is encapsulated in a
		// future, which we can query to determine if the task has
		// completed.
		typedef hello_world_foreman_action action_type;
		futures.push_back(hpx::async<action_type>(node));
	}

	hpx::wait_all(futures);
	return 0;
}