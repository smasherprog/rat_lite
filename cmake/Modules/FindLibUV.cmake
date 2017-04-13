# - Try to find libuv
# Once done, this will define
#
#  LIBUV_FOUND - system has libuv
#  LIBUV_INCLUDE_DIRS - the libuv include directories
#  LIBUV_LIBRARIES - link these to use libuv
#

find_path(LIBUV_INCLUDE_DIR 
    NAMES uv.h
    PATHS /usr/local/opt/libuv/include
)

if(MSVC)
  list(APPEND LIBUV_NAMES libuv)
else()
  list(APPEND LIBUV_NAMES uv)
endif()

find_library(LIBUV_LIBRARY 
    NAMES ${LIBUV_NAMES}
    PATHS /usr/local/opt/libuv/lib
  )

mark_as_advanced(LIBUV_INCLUDE_DIR LIBUV_LIBRARY)

set(LIBUV_LIBRARIES ${LIBUV_LIBRARY})
set(LIBUV_INCLUDE_DIRS ${LIBUV_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBUV
  FOUND_VAR LIBUV_FOUND
  REQUIRED_VARS LIBUV_LIBRARY LIBUV_INCLUDE_DIR
)

if(LIBUV_FOUND)
  set(LIBUV_INCLUDE_DIRS ${LIBUV_INCLUDE_DIR})
  set(LibUV_DIR ${LIBUV_INCLUDE_DIR})
  set(LIBUV_LIBRARIES ${LIBUV_LIBRARY})
endif()
