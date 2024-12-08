# add_subdirectory(crypto)
# add_subdirectory(utils)
# add_subdirectory(VHCSSE)
set(CMAKE_CXX_STANDARD 17)
add_library(crypto_lib OBJECT utils/CryptoTools.cpp utils/SpookyV2.cpp utils/PrefixPrf.cpp)
add_library(filter_lib OBJECT filter/bloomfilter.cpp filter/xorfilter.cpp utils/CompressBF.cpp)

# OXT
add_library(OXT_lib OBJECT OXT/SSEClient.cpp OXT/SSEServer.cpp
    OXT/gen-cpp/SSEService.cpp)
target_link_libraries(OXT_lib thrift)

add_executable(client_oxt OXT/client_oxt.cpp)
add_executable(server_oxt OXT/server_oxt.cpp)
target_link_libraries(client_oxt crypto mysqlclient crypto_lib filter_lib gmp pbc
    OXT_lib)
target_link_libraries(server_oxt crypto mysqlclient crypto_lib filter_lib gmp pbc
    OXT_lib)
set_target_properties(client_oxt PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(server_oxt PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)