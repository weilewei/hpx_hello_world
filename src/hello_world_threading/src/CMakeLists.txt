ADD_EXECUTABLE(hello_world_threading hello_world_threading.cpp)

# build your application using HPX
hpx_setup_target(hello_world_threading 
COMPONENT_DEPENDENCIES iostreams)