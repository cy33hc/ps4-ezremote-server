cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE "$ENV{OPENORBIS}/cmake/daemon.cmake")

project(ezremote-server)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDONT_HAVE_STRUPR")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
set(APP_VERSION "1.00")

add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)
add_definitions(-DCPPHTTPLIB_THREAD_POOL_COUNT=64)

include_directories(
  source
)

add_executable(${PROJECT_NAME}.elf
  source/http/httplib.cpp
  source/server/http_server.cpp
  source/clients/archiveorg.cpp
  source/clients/baseclient.cpp
  source/clients/ftpclient.cpp
  source/clients/sftpclient.cpp
  source/clients/nfsclient.cpp
  source/clients/smbclient.cpp
  source/clients/webdav.cpp
  source/config.cpp
  source/crypt.c
  source/fs.cpp
  source/getentropy.c
  source/orbis_jbc.c
  source/main.cpp)

add_self(${PROJECT_NAME}.elf)

add_pkg(${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/data "EZSR00001" "ezRemote Server" "01.00" 32 0)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE EZREMOTE_VERSION=${APP_VERSION} CPPHTTPLIB_THREAD_POOL_COUNT=64)

target_link_libraries(${PROJECT_NAME}.elf
  dbglogger
  z
  crypto
  ssl
  json-c
  jbc
  smb2
  ssh2
  nfs
  kernel_sys
  SceSysmodule
  SceNet
  SceSystemService
)
