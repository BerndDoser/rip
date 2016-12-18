cmake_minimum_required(VERSION 3.0)
project(rip VERSION 1.0.0 LANGUAGES CXX)
message("${PROJECT_NAME} version ${PROJECT_VERSION}")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic-errors")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static") 

find_package(Lame REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(src)

if(MINGW)
    message( STATUS "Installing system-libraries: MinGW DLLs." )
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS /usr/lib/gcc/x86_64-w64-mingw32/4.8/libstdc++-6.dll
                                          /usr/lib/gcc/x86_64-w64-mingw32/4.8/libgcc_s_sjlj-1.dll
                                          /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll)
endif(MINGW)
include(InstallRequiredSystemLibraries)

SET(CPACK_GENERATOR "DEB;ZIP")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Bernd Doser")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
include(CPack)
