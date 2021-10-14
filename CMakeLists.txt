cmake_minimum_required(VERSION 3.5)

project(textstorage)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract(textstorage textstorage textstorage.cpp)
target_include_directories(textstorage PUBLIC ${CMAKE_SOURCE_DIR}/../include)