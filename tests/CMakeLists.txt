#file(GLOB COMMON_SOURCES "common/*.cpp")

#find_package( Gperftools QUIET )
#if( GPERFTOOLS_FOUND )
#    message( STATUS "Found gperftools; compiling tests with TCMalloc")
#    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
#endif()

set(EOSIO_SRC_DIR "" CACHE PATH "Path to eosio source directory")
set(EOSIO_BUILD_DIR "" CACHE PATH "Path to eosio build directory")
set(BOOST_INCLUDEDIR $ENV{HOME}/opt/boost_1_66_0/include CACHE PATH "Path to boost instalation directory")

# FIXME: not all might be needed
set(EOSIO_LIB_BUILD_DIR "${EOSIO_BUILD_DIR}/libraries")
set(EOSIO_CHAIN_LIB "${EOSIO_LIB_BUILD_DIR}/chain/libeosio_chain.a")
set(EOSIO_CHAINBASE_LIB "${EOSIO_LIB_BUILD_DIR}/chainbase/libchainbase.a")
set(EOSIO_TESTING_LIB "${EOSIO_LIB_BUILD_DIR}/testing/libeosio_testing.a")
set(EOSIO_UTILITIES_LIB "${EOSIO_LIB_BUILD_DIR}/utilities/libeos_utilities.a")
set(EOSIO_FC_LIB "${EOSIO_LIB_BUILD_DIR}/fc/libfc_debug.a")

message("Lib: ${EOSIO_CHAIN_LIB} ${EOSIO_CHAINBASE_LIB} ${EOSIO_TESTING_LIB}
    ${EOSIO_UTILITIES_LIB} ${EOSIO_FC_LIB}")

add_library(eosio_chain STATIC IMPORTED)
set_target_properties(eosio_chain PROPERTIES IMPORTED_LOCATION "${EOSIO_CHAIN_LIB}")

add_library(chainbase STATIC IMPORTED)
set_target_properties(chainbase PROPERTIES IMPORTED_LOCATION "${EOSIO_CHAINBASE_LIB}")

add_library(testing STATIC IMPORTED)
set_target_properties(testing PROPERTIES IMPORTED_LOCATION "${EOSIO_TESTING_LIB}")

add_library(utilities STATIC IMPORTED)
set_target_properties(utilities PROPERTIES IMPORTED_LOCATION "${EOSIO_UTILITIES_LIB}")

add_library(fc STATIC IMPORTED)
set_target_properties(fc PROPERTIES IMPORTED_LOCATION "${EOSIO_FC_LIB}")

add_library(binaryen STATIC IMPORTED)
set_target_properties(binaryen PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/externals/binaryen/lib/libbinaryen.a")

add_library(binaryen_support STATIC IMPORTED)
set_target_properties(binaryen_support PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/externals/binaryen/lib/libsupport.a")

add_library(binaryen_wasm STATIC IMPORTED)
set_target_properties(binaryen_wasm PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/externals/binaryen/lib/libwasm.a")

add_library(binaryen_cfg STATIC IMPORTED)
set_target_properties(binaryen_cfg PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/externals/binaryen/lib/libcfg.a")

add_library(binaryen_passes STATIC IMPORTED)
set_target_properties(binaryen_passes PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/externals/binaryen/lib/libpasses.a")

add_library(binaryen_asmjs STATIC IMPORTED)
set_target_properties(binaryen_asmjs PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/externals/binaryen/lib/libasmjs.a")

add_library(wasmjit_wasm STATIC IMPORTED)
set_target_properties(wasmjit_wasm PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/libraries/wasm-jit/Source/WASM/libWASM.a")

add_library(wasmjit_wast STATIC IMPORTED)
set_target_properties(wasmjit_wast PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/libraries/wasm-jit/Source/WAST/libWAST.a")

add_library(wasmjit_ir STATIC IMPORTED)
set_target_properties(wasmjit_ir PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/libraries/wasm-jit/Source/IR/libIR.a")

add_library(wasmjit_runtime STATIC IMPORTED)
set_target_properties(wasmjit_runtime PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/libraries/wasm-jit/Source/Runtime/libRuntime.a")

add_library(wasmjit_platform STATIC IMPORTED)
set_target_properties(wasmjit_platform PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/libraries/wasm-jit/Source/Platform/libPlatform.a")

add_library(wasmjit_logging STATIC IMPORTED)
set_target_properties(wasmjit_logging PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/libraries/wasm-jit/Source/Logging/libLogging.a")

add_library(builtins STATIC IMPORTED)
set_target_properties(builtins PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/libraries/builtins/libbuiltins.a")

add_library(softfloat STATIC IMPORTED)
set_target_properties(softfloat PROPERTIES IMPORTED_LOCATION
        "${EOSIO_BUILD_DIR}/libraries/softfloat/libsoftfloat.a")


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${EOSIO_SRC_DIR}/libraries/fc/CMakeModules")
find_package(LLVM 4.0 REQUIRED CONFIG)
find_package(Secp256k1 REQUIRED)
find_package(GMP REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(Boost 1.67 REQUIRED COMPONENTS
        thread
        date_time
        filesystem
        system
        program_options
        signals
        serialization
        chrono
        unit_test_framework
        context
        locale
        iostreams)

llvm_map_components_to_libnames(LLVM_LIBS support core passes mcjit native DebugInfoDWARF)
link_directories(${LLVM_LIBRARY_DIR})

include_directories("${CMAKE_BINARY_DIR}")  # For *.wast.hpp and *.abi.hpp
include_directories("${CMAKE_SOURCE_DIR}/")
include_directories("${EOSIO_SRC_DIR}/libraries/testing/include")
include_directories("${EOSIO_SRC_DIR}/libraries/chain/include/")
include_directories("${EOSIO_SRC_DIR}/libraries/fc/include/")
include_directories("${EOSIO_SRC_DIR}/libraries/chainbase/include/")
include_directories("${EOSIO_SRC_DIR}/libraries/utilities/include/")
include_directories("${EOSIO_SRC_DIR}/libraries/wasm-jit/Include/")
include_directories("${EOSIO_SRC_DIR}/libraries/softfloat/source/include")
include_directories("${EOSIO_BUILD_DIR}/libraries/chain/include")
include_directories("${EOSIO_BUILD_DIR}/contracts/") # For eosio.token.wast.hpp and abi
include_directories("${BOOST_INCLUDEDIR}")

#configure_file(${EOSIO_SRC_DIR}/include/config.hpp.in
#        ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp ESCAPE_QUOTES)

file(GLOB UNIT_TESTS "*.cpp")

#add_executable( polleos-tests ${UNIT_TESTS} ${WASM_UNIT_TESTS} )
add_executable( polleos-tests ${UNIT_TESTS})
#target_link_libraries( polleos-tests eosio_chain chainbase eosio_testing eos_utilities
#        abi_generator fc ${PLATFORM_SPECIFIC_LIBS} )
target_link_libraries( polleos-tests testing eosio_chain chainbase utilities fc
        ${OPENSSL_LIBRARIES} ${Secp256k1_LIBRARY} ${GMP_LIBRARIES} ${Boost_LIBRARIES}
        binaryen_wasm binaryen_support binaryen_cfg binaryen binaryen_passes
        binaryen_asmjs wasmjit_wasm wasmjit_wast wasmjit_ir wasmjit_runtime
        wasmjit_platform wasmjit_logging builtins softfloat ${LLVM_LIBS} -lpthread)

#target_include_directories( unit_test PUBLIC ${CMAKE_BINARY_DIR}/contracts ${CMAKE_CURRENT_BINARY_DIR}/tests/contracts )
#target_include_directories( unit_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wasm_tests )
#target_include_directories( unit_test PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include )
#add_dependencies(unit_test asserter test_api test_api_mem test_api_db test_ram_limit test_api_multi_index exchange eosio.token proxy identity identity_test stltest infinite eosio.system eosio.token eosio.bios test.inline multi_index_test noop dice eosio.msig payloadless tic_tac_toe)

#Manually run unit_test for all supported runtimes
#To run unit_test with all log from blockchain displayed, put --verbose after --, i.e. unit_test -- --verbose
#add_test(NAME unit_test_binaryen COMMAND unit_test
# -t \!wasm_tests/weighted_cpu_limit_tests
# --report_level=detailed --color_output -- --binaryen)
#add_test(NAME unit_test_wavm COMMAND unit_test
# -t \!wasm_tests/weighted_cpu_limit_tests
# --report_level=detailed --color_output --catch_system_errors=no -- --wavm)

if(ENABLE_COVERAGE_TESTING)

  set(Coverage_NAME ${PROJECT_NAME}_ut_coverage)

  if(NOT LCOV_PATH)
    message(FATAL_ERROR "lcov not found! Aborting...")
  endif() # NOT LCOV_PATH

  if(NOT LLVMCOV_PATH)
    message(FATAL_ERROR "llvm-cov not found! Aborting...")
  endif() # NOT LCOV_PATH

  if(NOT GENHTML_PATH)
    message(FATAL_ERROR "genhtml not found! Aborting...")
  endif() # NOT GENHTML_PATH

  # no spaces allowed within tests list
  set(ctest_tests 'unit_test_binaryen|unit_test_wavm')
  set(ctest_exclude_tests '')

  # Setup target
  add_custom_target(${Coverage_NAME}

    # Cleanup lcov
    COMMAND ${LCOV_PATH} --directory . --zerocounters

    # Run tests
    COMMAND ./tools/ctestwrapper.sh -R ${ctest_tests} -E ${ctest_exclude_tests}

    COMMAND ${LCOV_PATH} --directory . --capture --gcov-tool ./tools/llvm-gcov.sh --output-file ${Coverage_NAME}.info

    COMMAND ${LCOV_PATH} -remove ${Coverage_NAME}.info '*/boost/*' '/usr/lib/*' '/usr/include/*' '*/externals/*' '*/fc/*' '*/wasm-jit/*' --output-file ${Coverage_NAME}_filtered.info

    COMMAND ${GENHTML_PATH} -o ${Coverage_NAME} ${PROJECT_BINARY_DIR}/${Coverage_NAME}_filtered.info

    COMMAND if [ "$CI" != "true" ]\; then ${CMAKE_COMMAND} -E remove ${Coverage_NAME}.base ${Coverage_NAME}.info ${Coverage_NAME}_filtered.info ${Coverage_NAME}.total ${PROJECT_BINARY_DIR}/${Coverage_NAME}.info.cleaned ${PROJECT_BINARY_DIR}/${Coverage_NAME}_filtered.info.cleaned\; fi

    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Resetting code coverage counters to zero. Processing code coverage counters and generating report. Report published in ./${Coverage_NAME}"
    )

  # Show info where to find the report
  add_custom_command(TARGET ${Coverage_NAME} POST_BUILD
    COMMAND ;
    COMMENT "Open ./${Coverage_NAME}/index.html in your browser to view the coverage report."
    )
endif()
