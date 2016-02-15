
cmake_minimum_required(VERSION 2.8)

project(turbo2)

# if this is the first time running cmake, perform pre-build dependency install script (or if the user manually triggers re-building the dependencies)
set(BUILD_DEPS "YES" CACHE BOOL "If YES, will install dependencies.  Automatically reset to NO after dependencies are installed.")

if( ${BUILD_DEPS} )
	message("Launching pre-build dependency installer script...")

	execute_process(COMMAND sh ../CMakePreBuild.sh
				WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
				RESULT_VARIABLE PREBUILD_SCRIPT_RESULT)

	set(BUILD_DEPS "NO" CACHE BOOL "If YES, will install dependencies.  Automatically reset to NO after dependencies are installed." FORCE)
	message("Finished installing dependencies")
endif()

# build turbo2
file(GLOB turboSources *.cpp)
file(GLOB turboIncludes *.h)

# rpLIDAR sdk
file(GLOB rpSrcA rplidar-sdk/src/arch/linux/*.cpp)
file(GLOB rpSrcB rplidar-sdk/src/hal/*.cpp)
file(GLOB rpSrcC rplidar-sdk/src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rplidar-sdk/include ${CMAKE_CURRENT_SOURCE_DIR}/rplidar-sdk/src)

add_executable(turbo2 ${turboSources} ${rpSrcA} ${rpSrcB} ${rpSrcC})

target_link_libraries(turbo2 usb-1.0 pthread)




