cmake_minimum_required(VERSION 3.9)
project(polleos CXX)

set(BOOST_INCLUDE_DIR $ENV{HOME}/opt/boost_1_66_0/include CACHE PATH "Path to boost instalation directory") 
set(WASM_INSTALL_DIR $ENV{HOME}/opt/wasm CACHE PATH "Path to wasm compiler instalation directory")
set(EOSIO_TOKEN_INCLUDE_DIR ./ CACHE PATH "Path to directory where eosio.token.hpp is")

set(EOSIO_INSTALL_DIR /usr/local CACHE PATH "Path to eosio intstall dir")
set(S2WASM_PATH ${EOSIO_INSTALL_DIR}/bin/eosio-s2wasm)
set(EOSIOCPP_PATH ${EOSIO_INSTALL_DIR}/bin/eosiocpp)

set(EOSIO_CONTRACTSDK_PATH ${EOSIO_INSTALL_DIR}/usr/share/eosio/contractsdk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -emit-llvm")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlibinc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

include_directories(include)
include_directories(${EOSIO_INSTALL_DIR}/include)
include_directories(${EOSIO_INSTALL_DIR}/include/libc++/upstream/include)
include_directories(${EOSIO_INSTALL_DIR}/include/musl/upstream/include)
include_directories(${BOOST_INCLUDE_DIR})
include_directories(${EOSIO_TOKEN_INCLUDE_DIR})

set(CPP_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.cpp)

set(HPP_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.hpp)

add_library(link SHARED
        ${CPP_FILES}
        ${HPP_FILES})

target_link_libraries(link
        ${EOSIO_CONTRACTSDK_PATH}/lib/libc.bc
        ${EOSIO_CONTRACTSDK_PATH}/lib/libc++.bc
        ${EOSIO_CONTRACTSDK_PATH}/lib/eosiolib.bc)

set(CONTRACT_ASSEMBLY_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.s)
set(CONTRACT_WAST_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.wast)
set(CONTRACT_ABI_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.abi)
set(CONTRACT_ABI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.abi)

add_custom_target(assembly ALL
        DEPENDS link)
add_custom_command(OUTPUT ${CONTRACT_ASSEMBLY_FILE}
        COMMAND ${WASM_INSTALL_DIR}/bin/llc
        -thread-model=single
        --asm-verbose=false
        -o ${CONTRACT_ASSEMBLY_FILE} $<TARGET_FILE:link>
        DEPENDS $<TARGET_FILE:link>)

add_custom_target(wast ALL
        DEPENDS ${CONTRACT_WAST_FILE})
add_custom_command(OUTPUT ${CONTRACT_WAST_FILE}
        COMMAND ${S2WASM_PATH}
        -o ${CONTRACT_WAST_FILE}
        -s 16384
        ${CONTRACT_ASSEMBLY_FILE}
        DEPENDS ${CONTRACT_ASSEMBLY_FILE})


# Currently we don't generate abi, because it does not recognize symbol types.
# Instead we just copy our own .abi file from source into build dir
#add_custom_target(abi
#        DEPENDS ${CONTRACT_ABI_FILE})
#add_custom_command(OUTPUT ${CONTRACT_ABI_FILE}
#        COMMAND ${EOSIOCPP_PATH}
#        -g ${CONTRACT_ABI_FILE}
#        ${CPP_FILES}
#        DEPENDS ${CPP_FILES} ${HPP_FILES})

add_custom_target(abi-copy ALL
        DEPENDS ${CONTRACT_ABI_FILE})
add_custom_command(OUTPUT ${CONTRACT_ABI_FILE}
        COMMAND cp ${CONTRACT_ABI_SRC} ${CONTRACT_ABI_FILE}
        DEPENDS ${CONTRACT_ABI_SRC})
