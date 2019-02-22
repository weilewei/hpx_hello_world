ADD_EXECUTABLE(distributed_add distributed_add.cpp)

# build your application using HPX
hpx_setup_target(distributed_add 
COMPONENT_DEPENDENCIES iostreams)