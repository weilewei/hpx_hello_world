//  Copyright (c) 2019 Weile Wei
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//[hello_world_client_getting_started
#include "app_server_component.hpp"
#include <hpx/hpx_init.hpp>

int hpx_main(boost::program_options::variables_map&)
{
	{
		std::vector<hpx::id_type> localities = hpx::find_all_localities();

		// Create a single instance of the component on this locality.
		examples::remote_increment client1 =
			hpx::new_<examples::remote_increment>(localities[1]);

		client1.print();
		client1.add(2);
		client1.print();
		//hpx::id_type serv_id = client1.get_id();
		//hpx::async<examples::server::remote_increment::print_action>(serv_id);

		// Create a single instance of the component on this locality.
		examples::remote_increment client2 =
		hpx::new_<examples::remote_increment>(localities[0]);

		client2.print();
		client2.add(3);
		client2.print();

		// Create a single instance of the component on this locality.
		examples::remote_increment client3 =
			hpx::new_<examples::remote_increment>(localities[1]);

		client3.print();
		client3.add(4);
		client3.print();
	}

	return hpx::finalize(); // Initiate shutdown of the runtime system.
}

int main(int argc, char* argv[])
{
	return hpx::init(argc, argv); // Initialize and run HPX.
}

/*
dist_obj example;
	//Creates one server client pair per node
example.increment();

struct dist_obj
{
	 examples::remote_increment node0;
	 examples::remote_increment node1;
	 void increment();
}

dist_obj obj1;
*/