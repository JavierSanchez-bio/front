find_package(Boost REQUIRED COMPONENTS date_time)
create_library(NAME Date)
target_link_libraries(Date PUBLIC Boost::date_time)
add_subdirectory(tests)