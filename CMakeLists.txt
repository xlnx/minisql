cmake_minimum_required(VERSION 3.0)
project(minisql)
set(CMAKE_CXX_STANDARD 17)

include_directories(
	"${CMAKE_SOURCE_DIR}/include" 
	"${CMAKE_SOURCE_DIR}/src"
)

if (MSVC)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses -Wno-noexcept-type -Wno-unused-variable -static-libgcc -static-libstdc++")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-pg -DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pg")

file(GLOB SQL_SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB SQL_DIR "${CMAKE_SOURCE_DIR}/src/*")

foreach (f ${SQL_DIR})
	string(REGEX REPLACE ".+/(.+)\\..*" "" TEST_V ${f})
	if (NOT ${TEST_V} STREQUAL "")
		file(GLOB_RECURSE TEMP_CPP "${f}/*.cpp")
		set(SQL_SRC
			${SQL_SRC}
			${TEMP_CPP}
		)
	endif ()
endforeach ()

add_executable(minisql ${SQL_SRC})