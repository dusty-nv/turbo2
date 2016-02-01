
cmake_minimum_required(VERSION 2.8)

project(turbo2)

file(GLOB turboSources *.cpp)
file(GLOB turboIncludes *.h)



add_executable(turbo2 ${turboSources})

target_link_libraries(turbo2 usb-1.0)

#target_link_libraries(gasket-console gasket ${QT_LIBRARIES} renderGL)


