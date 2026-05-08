cmake_minimum_required( VERSION 3.28 )
project( cacao LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_C_STANDARD 23 )
set( CMAKE_C_STANDARD_REQUIRED ON )

if( UNIX )
    add_definitions( -fpermissive )
elseif( _MSC_BUILD )
    #add_definitions( /std:latest /clatest /cpplatest )
endif()


#set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake-golang;${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules" )
#include( CMakeDetermineGoCompiler )
#include( CMakeGoCompiler )
#include( CMakeGoInformation )
#include( CMakeTestGoCompiler )
#include( golang )

#enable_language(Go)

include_directories( ${PROJECT_SOURCE_DIR}/lib/difflib/src/ )

include_directories( ${PROJECT_SOURCE_DIR}/lib/libtruth/ )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/lib/libtruth)

include_directories( ${PROJECT_SOURCE_DIR}/lib/psem )
set ( SOURCES_PSEM_TIME ${CMAKE_CURRENT_SOURCE_DIR}/lib/psem/ptime.c
                        ${CMAKE_CURRENT_SOURCE_DIR}/lib/psem/semaphore.c
                        ${CMAKE_CURRENT_SOURCE_DIR}/lib/psem/systime.c )
add_library( psem_time ${SOURCES_PSEM_TIME} )


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/cacao/ )
file( GLOB CPP_SRCFILES ${PROJECT_SOURCE_DIR}/cacao/*.cpp )
add_library( cacao STATIC ${CPP_SRCFILES} )
target_link_libraries( cacao psem_time oneptr_bool stdc++ )


find_package(Doxygen)
if(DOXYGEN_FOUND)
    message("-- Enabled doc_cacao target for make")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc_cacao ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation with Doxygen")
endif()