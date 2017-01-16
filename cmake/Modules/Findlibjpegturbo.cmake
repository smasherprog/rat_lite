find_library(LibLIBJPEGTURBO_LIBRARY
  NAMES jpeg turbojpeg
  )
mark_as_advanced(LibLIBJPEGTURBO_LIBRARY)

find_path(LibLIBJPEGTURBO_INCLUDE_DIR
  NAMES turbojpeg.h
  )
mark_as_advanced(LibLIBJPEGTURBO_INCLUDE_DIR)

#-----------------------------------------------------------------------------
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibLIBJPEGTURBO
  FOUND_VAR LibLIBJPEGTURBO_FOUND
  REQUIRED_VARS LibLIBJPEGTURBO_LIBRARY LibLIBJPEGTURBO_INCLUDE_DIR
  )
set(LIBLIBJPEGTURBO_FOUND ${LibLIBJPEGTURBO_FOUND})

#-----------------------------------------------------------------------------
# Provide documented result variables and targets.
if(LibLIBJPEGTURBO_FOUND)
  set(LibLIBJPEGTURBO_INCLUDE_DIRS ${LibLIBJPEGTURBO_INCLUDE_DIR})
  set(LibLIBJPEGTURBO_LIBRARIES ${LibLIBJPEGTURBO_LIBRARY})
  if(NOT TARGET LibLIBJPEGTURBO::LibLIBJPEGTURBO)
    add_library(LibLIBJPEGTURBO::LibLIBJPEGTURBO UNKNOWN IMPORTED)
    set_target_properties(LibLIBJPEGTURBO::LibLIBJPEGTURBO PROPERTIES
      IMPORTED_LOCATION "${LibLIBJPEGTURBO_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${LibLIBJPEGTURBO_INCLUDE_DIRS}"
      )
  endif()
endif()