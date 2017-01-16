find_library(LibUWS_LIBRARY
  NAMES uws
  )
mark_as_advanced(LibUWS_LIBRARY)

find_path(LibUWS_INCLUDE_DIR
  NAMES uws/UWS.h
  )
mark_as_advanced(LibUWS_INCLUDE_DIR)

#-----------------------------------------------------------------------------
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibUWS
  FOUND_VAR LibUWS_FOUND
  REQUIRED_VARS LibUWS_LIBRARY LibUWS_INCLUDE_DIR
  )
set(LIBUWS_FOUND ${LibUWS_FOUND})

#-----------------------------------------------------------------------------
# Provide documented result variables and targets.
if(LibUWS_FOUND)
  set(LibUWS_INCLUDE_DIRS ${LibUWS_INCLUDE_DIR})
  set(LibUWS_LIBRARIES ${LibUWS_LIBRARY})
  if(NOT TARGET LibUWS::LibUWS)
    add_library(LibUWS::LibUWS UNKNOWN IMPORTED)
    set_target_properties(LibUWS::LibUWS PROPERTIES
      IMPORTED_LOCATION "${LibUWS_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${LibUWS_INCLUDE_DIRS}"
      )
  endif()
endif()