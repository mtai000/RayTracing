project(RayTracingNextWeek)

include_directories(/usr/bin/include)
file(GLOB src *.cpp *.h)

add_executable(RayTracingNextWeek ${src})
