# build your application using HPX
add_hpx_component(app_server_component
    SOURCES app_server_component.cpp
    HEADERS app_server_component.hpp
    COMPONENT_DEPENDENCIES iostreams)

add_hpx_executable(remote_add_component
    ESSENTIAL
    SOURCES remote_add_component.cpp
    COMPONENT_DEPENDENCIES app_server_component)

hpx_setup_target(remote_add_component
    COMPONENT_DEPENDENCIES iostreams)