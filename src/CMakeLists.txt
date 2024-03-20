cmake_minimum_required(VERSION 3.22)
project(maxindepset VERSION 1.0.1 DESCRIPTION "A shared library for the maximum independent set")
include(GNUInstallDirs)
add_library(maxindepset SHARED Alg.cpp Graph.cpp Mis.cpp Reductions.cpp Util.cpp)
set_target_properties(maxindepset PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER maxindepset.h)
configure_file(maxindepset.pc.in maxindepset.pc @ONLY)
target_include_directories(maxindepset PRIVATE .)
install(TARGETS maxindepset
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/maxindepset.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)