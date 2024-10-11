cmake_minimum_required( VERSION 3.28 )
project( cacao )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_C_STANDARD 23 )
set( CMAKE_C_STANDARD_REQUIRED ON )

if( UNIX )
    add_definitions( -fpermissive )
elseif( _MSC_BUILD )
    add_definitions( /std:latest /clatest /cpplatest )
endif()


#set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake-golang;${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules" )
#include( CMakeDetermineGoCompiler )
#include( CMakeGoCompiler )
#include( CMakeGoInformation )
#include( CMakeTestGoCompiler )
#include( golang )

#enable_language(Go)


file( GLOB CPP_SRCFILES cpp/*.cpp )
include_directories( cpp/ )
add_library( cacao STATIC ${CPP_SRCFILES} )
target_link_libraries( cacao stdc++ )

#file( GLOB GO_SRCFILES go/*.go )
#add_go_library( cacao_go STATIC ${GO_SRCFILES} )
