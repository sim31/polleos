cmake_minimum_required(VERSION 3.9)
project(polleos CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

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

set(EOSIO_INSTALL_DIR /usr/local CACHE PATH "Path to eosio intstall dir")
set(S2WASM_PATH ${EOSIO_INSTALL_DIR}/bin/eosio-s2wasm)
set(WAST2WASM_PATH ${EOSIO_INSTALL_DIR}/bin/eosio-wast2wasm)
set(EOSIOCPP_PATH ${EOSIO_INSTALL_DIR}/bin/eosiocpp)
set(EOSIO_CONTRACTSDK_PATH ${EOSIO_INSTALL_DIR}/usr/share/eosio/contractsdk/lib)

include("wasm")

set(DEFAULT_SYSTEM_INCLUDE_FOLDERS ${EOSIO_INSTALL_DIR}/include/libc++/upstream/include
        ${EOSIO_INSTALL_DIR}/include/musl/upstream/include ${Boost_INCLUDE_DIR})
set(EOSIO_LIBS ${EOSIO_CONTRACTSDK_PATH}/eosiolib.bc ${EOSIO_CONTRACTSDK_PATH}/libc++.bc
        ${EOSIO_CONTRACTSDK_PATH}/libc.bc ${EOSIO_CONTRACTSDK_PATH}/eosiolib.bc)

set(INCLUDE_DIRS "${EOSIO_INSTALL_DIR}/include" "${CMAKE_SOURCE_DIR}/include")

#include_directories(include)
#include_directories(${EOSIO_INSTALL_DIR}/include)
#include_directories(${EOSIO_INSTALL_DIR}/include/libc++/upstream/include)
#include_directories(${EOSIO_INSTALL_DIR}/include/musl/upstream/include)
#include_directories(${BOOST_INCLUDE_DIR})
#include_directories(${EOSIO_TOKEN_INCLUDE_DIR})

set(CPP_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.cpp)

set(HPP_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.hpp)

#set(CONTRACT_ASSEMBLY_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.s)
#set(CONTRACT_WAST_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.wast)


add_wast_executable(TARGET ${CMAKE_PROJECT_NAME}
        INCLUDE_FOLDERS "${INCLUDE_DIRS}"
        LIBRARIES ${EOSIO_LIBS}
        DESTINATION_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
        SOURCE_FILES ${CPP_FILES})


set(CONTRACT_ABI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.abi)

configure_file("${CONTRACT_ABI_SRC}" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

add_subdirectory(tests)
