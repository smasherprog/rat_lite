include (FindPackageHandleStandardArgs)

find_path(LIBJPEGTURBO_INCLUDE_DIRS turbojpeg.h
	PATHS /usr/local/opt/jpeg-turbo/include
	/usr/include
)
set(JPEG_NAMES jpeg turbojpeg)

FOREACH(LIB ${JPEG_NAMES})
        FIND_LIBRARY(FOUND_LIB_${LIB} ${LIB}
		   PATHS /usr/local/opt/jpeg-turbo/lib
                   /opt/libjpeg-turbo/lib64
                   /opt/libjpeg-turbo/lib
		)
		if(FOUND_LIB_${LIB})
			LIST(APPEND LIBJPEGTURBO_LIBRARIES ${FOUND_LIB_${LIB}})
		endif()

ENDFOREACH(LIB)
   
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBJPEGTURBO DEFAULT_MSG LIBJPEGTURBO_LIBRARIES LIBJPEGTURBO_INCLUDE_DIRS)

mark_as_advanced(LIBJPEGTURBO_LIBRARIES LIBJPEGTURBO_INCLUDE_DIRS)