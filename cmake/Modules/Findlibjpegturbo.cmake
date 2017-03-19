find_library(JPEG_LIBRARY
  NAMES jpeg
 )
 find_library(TURBOJPEG_LIBRARY
  NAMES turbojpeg
PATHS /usr/local/opt/libjpeg-turbo/lib
)
find_path(LibLIBJPEGTURBO_INCLUDE_DIR
  NAMES turbojpeg.h
   PATHS /usr/local/opt/libjpeg-turbo/include
)
mark_as_advanced(TURBOJPEG_LIBRARY JPEG_LIBRARY LibLIBJPEGTURBO_INCLUDE_DIR)


#-----------------------------------------------------------------------------
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibLIBJPEGTURBO
  FOUND_VAR LibLIBJPEGTURBO_FOUND
  REQUIRED_VARS JPEG_LIBRARY TURBOJPEG_LIBRARY LibLIBJPEGTURBO_INCLUDE_DIR
  )
set(LIBLIBJPEGTURBO_FOUND ${LibLIBJPEGTURBO_FOUND})

#-----------------------------------------------------------------------------
# Provide documented result variables and targets.
if(LibLIBJPEGTURBO_FOUND)
  set(LIBJPEGTURBO_INCLUDE_DIRS ${LibLIBJPEGTURBO_INCLUDE_DIR})
  set(LIBJPEGTURBO_LIBRARIES ${JPEG_LIBRARY} ${TURBOJPEG_LIBRARY})
  message(STATUS  "Finding")
    message(STATUS  ${LIBJPEGTURBO_LIBRARIES})
endif()