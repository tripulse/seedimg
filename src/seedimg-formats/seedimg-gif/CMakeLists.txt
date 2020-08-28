add_library(seedimg-gif ${LIB_TYPE} seedimg-gif.cpp)

find_package(GIF REQUIRED)
target_link_libraries(seedimg-gif PRIVATE ${GIF_LIBRARIES})
