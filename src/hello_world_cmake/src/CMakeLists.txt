ADD_EXECUTABLE( hellocmake main.cpp)

# build your application using HPX
hpx_setup_target(hellocmake
    COMPONENT_DEPENDENCIES iostreams)