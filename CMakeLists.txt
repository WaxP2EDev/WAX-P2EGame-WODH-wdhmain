cmake_minimum_required(VERSION 3.5)

project(smart-contract VERSION 2.0.0)

include(ExternalProject)

find_package(eosio.cdt)

set(DEBUG FALSE CACHE BOOL "Preparing build contract")

ExternalProject_Add(
   drhemsworlds
   SOURCE_DIR ${CMAKE_SOURCE_DIR}/drhemsworlds
   BINARY_DIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/drhemsworlds
   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DBUILD_TESTS=${BUILD_TESTS}
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)

# set(BUILD_TESTS FALSE CACHE BOOL "Build unit tests")

# if(BUILD_TESTS AND ${CMAKE_BUILD_TYPE} MATCHES "Debug")
  
#    message(STATUS "Building unit tests.")

#    ExternalProject_Add(
#      contracts_unit_tests
#      LIST_SEPARATOR | # Use the alternate list separator
#      CMAKE_ARGS -DCMAKE_BUILD_TYPE=${TEST_BUILD_TYPE} -DCMAKE_PREFIX_PATH=${TEST_PREFIX_PATH} -DCMAKE_FRAMEWORK_PATH=${TEST_FRAMEWORK_PATH} -DCMAKE_MODULE_PATH=${TEST_MODULE_PATH} -DEOSIO_ROOT=${EOSIO_ROOT} -DLLVM_DIR=${LLVM_DIR} -DBOOST_ROOT=${BOOST_ROOT}
#      SOURCE_DIR ${CMAKE_SOURCE_DIR}/tests
#      BINARY_DIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/tests
#      BUILD_ALWAYS 1
#      TEST_COMMAND   ""
#      INSTALL_COMMAND ""
#    )

# else()
#    message(STATUS "Unit tests will not be built. To build unit tests, set BUILD_TESTS to true.")
# endif()