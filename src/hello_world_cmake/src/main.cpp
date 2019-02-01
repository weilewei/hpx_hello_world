#include <iostream>
#include <hpx/include/actions.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <unordered_map>

// base function saying hello world
// if the hello world fucntion is not running expected thread,
// return negative, and say i failed; otherwise 
// (running expected thread), return the current thread number
std::size_t hello_world(std::size_t expected) {
	std::size_t current = hpx::get_worker_thread_num();
	if (expected == current) {
		hpx::cout << "hello world from thread\n" << current << "\n";
		return expected;
	}
	//std::cout << "i failed from thread" << current << "\n";
	return (-1);
}

// rescheduler 
void hello_world_foreman(std::unordered_map<size_t, size_t>& os_threads) {
	
	// launch future tasks from those threads 
	// that are not matched with expected threads
	std::vector<hpx::future<std::size_t> > futures;
	for (size_t i = 0; i < os_threads.size(); i++) {
		if (os_threads[i] != -1)
			futures.push_back(hpx::async(hello_world, os_threads[i]));
	}

	// a barrier: wait all tasks to be finished
	hpx::wait_all(futures);

	// assign passed signal (-1) for those threads that matched correctly
	for (size_t i = 0; i < futures.size(); i++) {
		size_t passed = futures[i].get();
		if (passed != -1) {
			os_threads[passed] = -1;
		}
	}

	// recursively call of rescheduler if not every thread works correctly
	for (size_t i = 0; i < os_threads.size(); i++) {
		if (os_threads[i] != -1)
			hello_world_foreman(os_threads);
	}
}

int main() {

	std::size_t os_threads_count = hpx::get_os_thread_count();
	std::unordered_map<size_t, size_t> os_threads;

	// get current avaliable threads
	for (std::size_t i = 0; i < os_threads_count; i++) {
		os_threads[i] = i;
	}

	// boss starts assigning tasks to workers and workers must finish
	hello_world_foreman(os_threads);

	return 0;
}