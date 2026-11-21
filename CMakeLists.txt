cmake_minimum_required(VERSION 3.5)

project(ezremote-server)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDONT_HAVE_STRUPR")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
set(APP_VERSION "1.12")

add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)

if(NOT DEFINED OPENORBIS)
  set(OPENORBIS "$ENV{OPENORBIS}")
endif()
if(NOT EXISTS "${OPENORBIS}/bin/create-fself")
  message(FATAL_ERROR "OpenOrbis SDK not found; set OPENORBIS or pass -DOPENORBIS=<sdk>")
endif()

set(EZREMOTE_DAEMON_AUTHINFO "010000000010003800000000001c004000ff00000000008000000000000000000000000000000000000000c000400040000000000000008000000000000000f00040ffff000000f000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" CACHE STRING "create-fself authinfo for the gdd daemon eboot")

include_directories(
  source
)

add_executable(${PROJECT_NAME}-${APP_VERSION}.elf
  source/http/httplib.cpp
  source/server/http_server.cpp
  source/clients/archiveorg.cpp
  source/clients/baseclient.cpp
  source/clients/ftpclient.cpp
  source/clients/gdrive.cpp
  source/clients/sftpclient.cpp
  source/clients/nfsclient.cpp
  source/clients/smbclient.cpp
  source/clients/webdav.cpp
  source/config.cpp
  source/crypt.c
  source/dbglogger.c
  source/fs.cpp
  source/inifile.c
  source/memccpy.c
  source/getentropy.c
  source/main.cpp
  source/orbis_jbc.c)

target_compile_definitions(${PROJECT_NAME}-${APP_VERSION}.elf PRIVATE
  EZREMOTE_VERSION=${APP_VERSION}
  CPPHTTPLIB_THREAD_POOL_COUNT=32)

target_compile_options(${PROJECT_NAME}-${APP_VERSION}.elf PRIVATE
  -ffunction-sections -fdata-sections)

target_link_libraries(${PROJECT_NAME}-${APP_VERSION}.elf
  c
  c++
  pthread
  z
  jbc
  json-c
  crypto
  ssl
  curl
  lexbor
  smb2
  ssh2
  nfs
  kernel
  SceSysmodule
  SceNet
  SceUserService
  SceSystemService
  SceLncUtil
  SceBgft
  SceAppInstUtil
)

set(EZREMOTE_SELF_OUT "${CMAKE_BINARY_DIR}/ezremote-server.oelf")
set(EZREMOTE_EBOOT "${CMAKE_BINARY_DIR}/eboot.bin")
set(EZREMOTE_STAGED_SELF "${CMAKE_CURRENT_SOURCE_DIR}/../data/daemon/ezremote-server.self")

add_custom_command(
  OUTPUT "${EZREMOTE_EBOOT}"
  COMMAND ${CMAKE_COMMAND} -E env "OO_PS4_TOOLCHAIN=${OPENORBIS}"
          "${OPENORBIS}/bin/create-fself"
          "-in=$<TARGET_FILE:${PROJECT_NAME}-${APP_VERSION}.elf>"
          "-out=${EZREMOTE_SELF_OUT}"
          "--eboot" "${EZREMOTE_EBOOT}"
          "--paid" "0x3800000000000035"
          "--authinfo" "${EZREMOTE_DAEMON_AUTHINFO}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/../data/daemon"
  COMMAND ${CMAKE_COMMAND} -E copy "${EZREMOTE_EBOOT}" "${EZREMOTE_STAGED_SELF}"
  DEPENDS ${PROJECT_NAME}-${APP_VERSION}.elf
  COMMENT "Creating ezRemote Server gdd daemon eboot.bin"
  VERBATIM)

add_custom_target(eboot ALL DEPENDS "${EZREMOTE_EBOOT}")
