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
 message(STATUS  "looking for libuv at " ${LIBUV_NAMES})
find_library(LIBUV_LIBRARY 
    NAMES ${LIBUV_NAMES}
    PATHS /usr/local/opt/libuv/lib
  )

mark_as_advanced(LIBUV_INCLUDE_DIR LIBUV_LIBRARY)

set(LIBUV_LIBRARIES ${LIBUV_LIBRARY} ${PC_LIBUV_LIBRARIES})
set(LIBUV_INCLUDE_DIRS ${LIBUV_INCLUDE_DIR})

# Deal with the fact that libuv.pc is missing important dependency information.

include(CheckLibraryExists)

check_library_exists(dl dlopen "dlfcn.h" HAVE_LIBDL)
if(HAVE_LIBDL)
  list(APPEND LIBUV_LIBRARIES dl)
endif()

check_library_exists(kstat kstat_lookup "kstat.h" HAVE_LIBKSTAT)
if(HAVE_LIBKSTAT)
  list(APPEND LIBUV_LIBRARIES kstat)
endif()

check_library_exists(kvm kvm_open "kvm.h" HAVE_LIBKVM)
if(HAVE_LIBKVM)
  list(APPEND LIBUV_LIBRARIES kvm)
endif()

check_library_exists(nsl gethostbyname "nsl.h" HAVE_LIBNSL)
if(HAVE_LIBNSL)
  list(APPEND LIBUV_LIBRARIES nsl)
endif()

check_library_exists(perfstat perfstat_cpu "libperfstat.h" HAVE_LIBPERFSTAT)
if(HAVE_LIBPERFSTAT)
  list(APPEND LIBUV_LIBRARIES perfstat)
endif()

check_library_exists(rt clock_gettime "time.h" HAVE_LIBRT)
if(HAVE_LIBRT)
  list(APPEND LIBUV_LIBRARIES rt)
endif()

check_library_exists(sendfile sendfile "" HAVE_LIBSENDFILE)
if(HAVE_LIBSENDFILE)
  list(APPEND LIBUV_LIBRARIES sendfile)
endif()

if(WIN32)
  # check_library_exists() does not work for Win32 API calls in X86 due to name
  # mangling calling conventions
  list(APPEND LIBUV_LIBRARIES iphlpapi)
  list(APPEND LIBUV_LIBRARIES psapi)
  list(APPEND LIBUV_LIBRARIES userenv)
  list(APPEND LIBUV_LIBRARIES ws2_32)
endif()

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBUV_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibUV DEFAULT_MSG
                                  LIBUV_LIBRARY LIBUV_INCLUDE_DIR)

mark_as_advanced(LIBUV_INCLUDE_DIR LIBUV_LIBRARY)
