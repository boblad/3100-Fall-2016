set(CMAKE_CXX_FLAGS "-std=c++11 -lpthread -O3 ${CMAKE_CXX_FLAGS}")

set(srcs
    main.cpp
   )

add_executable(assign6 ${srcs})
