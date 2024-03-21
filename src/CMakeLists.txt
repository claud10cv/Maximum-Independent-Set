cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(MaximumIndependentSet)

find_package(JlCxx REQUIRED)

SET(libname mis)
SET( CMAKE_CXX_FLAGS_DEBUG  "-g")
SET( CMAKE_CXX_FLAGS_RELEASE  "-O3")
SET( CMAKE_CXX_FLAGS  "-std=c++17 -D__STDC_FORMAT_MACROS")

add_library (mis SHARED Graph.cpp Reductions.cpp Alg.cpp Mis.cpp Util.cpp Graph.hpp Reductions.hpp Alg.hpp Mis.hpp Util.hpp Jlwrapper.cpp)
set_target_properties(mis PROPERTIES PUBLIC_HEADER "Jlwrapper.h")
target_link_libraries(mis PRIVATE JlCxx::cxxwrap_julia JlCxx::cxxwrap_julia_stl)

install(TARGETS mis
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# End of CMakeLists.Txt file. Save it as a plain text file and the name should be exactly CMakeLists.Txt
