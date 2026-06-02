cmake_minimum_required(VERSION 3.5)

project(ezremote-server)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDONT_HAVE_STRUPR -fno-builtin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
set(APP_VERSION "1.01")

add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)
add_definitions(-DCPPHTTPLIB_THREAD_POOL_COUNT=64)

include_directories(
  source
)

add_executable(${PROJECT_NAME}-${APP_VERSION}.elf
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
  source/dbglogger.c
  source/fs.cpp
  source/memccpy.c
  source/main.cpp)

target_compile_definitions(${PROJECT_NAME}-${APP_VERSION}.elf PRIVATE EZREMOTE_VERSION=${APP_VERSION} CPPHTTPLIB_THREAD_POOL_COUNT=64)

target_link_libraries(${PROJECT_NAME}-${APP_VERSION}.elf
  z
  crypto
  ssl
  json-c
  smb2
  ssh2
  nfs
  kernel_sys
  SceSysmodule
  SceNet
  SceSystemService
)

add_custom_target(package
   DEPENDS ${PROJECT_NAME}-${APP_VERSION}.elf
   COMMAND /opt/ps4-payload-sdk/bin/orbis-strip ${PROJECT_NAME}-${APP_VERSION}.elf
   COMMAND cp ${PROJECT_NAME}-${APP_VERSION}.elf ${CMAKE_CURRENT_SOURCE_DIR}/../data/daemon/daemon.elf

)