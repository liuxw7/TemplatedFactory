cmake_minimum_required(VERSION 3.10)
project(TemplatedFactory CXX)
option(enable_tests "Build Tests" ON)

set(${PROJECT_NAME}_src include/factory/factory.h src/factory.cpp)
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_src}) #if i make it an Interface it annoys me with global settings

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /std:c++17) # Had Problems with Ninja & Clang
endif(MSVC)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(${PROJECT_NAME} PUBLIC include/)

if(enable_tests)
	enable_testing()
	find_package(GTest REQUIRED)
	include(GoogleTest)
	add_executable(${PROJECT_NAME}-tests tests/factory_tests.cpp)

	if(MSVC)
		target_compile_options(${PROJECT_NAME}-tests PRIVATE /std:c++17)
	endif(MSVC)

	set_property(TARGET ${PROJECT_NAME}-tests PROPERTY CXX_STANDARD 17)
	set_property(TARGET ${PROJECT_NAME}-tests PROPERTY CXX_STANDARD_REQUIRED ON)


	target_include_directories(${PROJECT_NAME}-tests PRIVATE include/)
	target_link_libraries(${PROJECT_NAME}-tests PRIVATE GTest::Main ${PROJECT_NAME})
	
	gtest_discover_tests(${PROJECT_NAME}-tests TEST_PREFIX gtest::)
	add_test(NAME monolithic COMMAND ${PROJECT_NAME}-tests)
endif(enable_tests)