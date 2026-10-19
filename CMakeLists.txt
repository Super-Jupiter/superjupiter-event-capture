cmake_minimum_required(VERSION 3.16)
project(SuperJupiterEventCapture VERSION 0.1.0 LANGUAGES CXX)

set(_superjupiter_event_capture_top_level OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(_superjupiter_event_capture_top_level ON)
endif()
option(SUPERJUPITER_EVENT_CAPTURE_BUILD_TESTS "Build EventCapture tests" ${_superjupiter_event_capture_top_level})
option(SUPERJUPITER_EVENT_CAPTURE_BUILD_EXAMPLES "Build EventCapture host example" ${_superjupiter_event_capture_top_level})
option(SUPERJUPITER_EVENT_CAPTURE_INSTALL "Generate EventCapture install rules" ${_superjupiter_event_capture_top_level})

add_library(SuperJupiterEventCapture INTERFACE)
add_library(SuperJupiter::EventCapture ALIAS SuperJupiterEventCapture)
set_target_properties(SuperJupiterEventCapture PROPERTIES EXPORT_NAME EventCapture)
target_include_directories(SuperJupiterEventCapture INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)
target_compile_features(SuperJupiterEventCapture INTERFACE cxx_std_11)

if(SUPERJUPITER_EVENT_CAPTURE_BUILD_TESTS)
  enable_testing()
  add_executable(superjupiter-event-capture-tests tests/test.cpp)
  target_link_libraries(superjupiter-event-capture-tests PRIVATE SuperJupiter::EventCapture)
  add_test(NAME superjupiter-event-capture-tests COMMAND superjupiter-event-capture-tests)
  add_executable(superjupiter-event-capture-namespace-test tests/namespace_isolation.cpp)
  target_link_libraries(superjupiter-event-capture-namespace-test PRIVATE SuperJupiter::EventCapture)
  add_test(NAME superjupiter-event-capture-namespace-test COMMAND superjupiter-event-capture-namespace-test)
endif()
if(SUPERJUPITER_EVENT_CAPTURE_BUILD_EXAMPLES)
  add_executable(superjupiter-event-capture-replay tools/replay.cpp)
  target_link_libraries(superjupiter-event-capture-replay PRIVATE SuperJupiter::EventCapture)
endif()
if(SUPERJUPITER_EVENT_CAPTURE_INSTALL)
  include(CMakePackageConfigHelpers)
  set(_superjupiter_event_capture_package_dir lib/cmake/SuperJupiterEventCapture)
  install(TARGETS SuperJupiterEventCapture EXPORT SuperJupiterEventCaptureTargets)
  install(FILES src/SuperJupiterEventCapture.h DESTINATION include)
  install(EXPORT SuperJupiterEventCaptureTargets NAMESPACE SuperJupiter::
    DESTINATION ${_superjupiter_event_capture_package_dir})
  configure_package_config_file(cmake/SuperJupiterEventCaptureConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterEventCaptureConfig.cmake
    INSTALL_DESTINATION ${_superjupiter_event_capture_package_dir})
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterEventCaptureConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion ARCH_INDEPENDENT)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterEventCaptureConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterEventCaptureConfigVersion.cmake
    DESTINATION ${_superjupiter_event_capture_package_dir})
  install(FILES LICENSE DESTINATION share/licenses/SuperJupiterEventCapture)
endif()
