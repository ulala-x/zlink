#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libzmq" for configuration "Debug"
set_property(TARGET libzmq APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libzmq PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libzmq.so.5.2.5"
  IMPORTED_SONAME_DEBUG "libzmq.so.5"
  )

list(APPEND _cmake_import_check_targets libzmq )
list(APPEND _cmake_import_check_files_for_libzmq "${_IMPORT_PREFIX}/lib/libzmq.so.5.2.5" )

# Import target "libzmq-static" for configuration "Debug"
set_property(TARGET libzmq-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libzmq-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libzmq.a"
  )

list(APPEND _cmake_import_check_targets libzmq-static )
list(APPEND _cmake_import_check_files_for_libzmq-static "${_IMPORT_PREFIX}/lib/libzmq.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
