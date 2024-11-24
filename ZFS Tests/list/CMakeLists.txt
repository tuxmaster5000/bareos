# rm -rf build-root&&cmake -B build-root&&cmake --build build-root --parallel

cmake_minimum_required(VERSION 3.30)
find_package(PkgConfig REQUIRED)
pkg_check_modules("ZFS" "libzfs" IMPORTED_TARGET GLOBAL REQUIRED)
project(TEST)
set(SOURCES
  code.cpp
)
set(COMMAND_LINE_CXX
	rpm --eval %build_cxxflags
)
set(COMMAND_LINE_LD
	rpm --eval %build_ldflags
)
execute_process(COMMAND ${COMMAND_LINE_CXX} OUTPUT_VARIABLE RPM_BUILD_CXX OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${COMMAND_LINE_LD} OUTPUT_VARIABLE RPM_BUILD_LD OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "BUILD Flags: ${RPM_BUILD_CXX}")
message(STATUS "Link Flags: ${RPM_BUILD_LD}")
set(CMAKE_CXX_FLAGS ${RPM_BUILD_CXX})
set(CMAKE_EXE_LINKER_FLAGS ${RPM_BUILD_LD})

add_executable(TEST ${SOURCES})
set_target_properties(TEST
	PROPERTIES
		OUTPUT_NAME "test"
)
target_include_directories(TEST PRIVATE ${ZFS_INCLUDE_DIRS})
target_link_libraries(TEST PkgConfig::ZFS)
