#include <iostream>
#include <hpx/include/actions.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <unordered_map>
#include <hpx/lcos/local/spinlock.hpp>

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
	std::size_t os_threads_count = hpx::get_os_thread_count();
	std::unordered_map<size_t, size_t> os_threads;

	// get current avaliable threads
	for (std::size_t i = 0; i < os_threads_count; i++) {
		os_threads[i] = i;
	}

	// Find the global name of the current locality.
	hpx::naming::id_type const here = hpx::find_here();

	// launch future tasks from those threads 
	// that are not matched with expected threads
	std::vector<hpx::future<std::size_t> > futures;
	for (size_t i = 0; i < os_threads.size(); i++) {
		if (os_threads[i] != -1) {
			typedef hello_world_action action_type;
			futures.push_back(hpx::async<action_type>(here, os_threads[i]));
		}			
	}

	// a barrier: wait all tasks to be finished
	hpx::wait_all(futures);

	 using mutex_type = hpx::lcos::local::spinlock;
	 mutex_type mtx;
	// assign passed signal (-1) for those threads that matched correctly
	for (size_t i = 0; i < futures.size(); i++) {
		size_t passed = futures[i].get();
		if (passed != -1) {
			std::lock_guard<mutex_type> l(mtx);
			os_threads[passed] = -1;
		}
	}

	// recursively call of rescheduler if not every thread works correctly
	for (size_t i = 0; i < os_threads.size(); i++) {
		if (os_threads[i] != -1)
			hello_world_foreman();
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