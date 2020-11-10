# Find PostGreSQL C++ library and header file
# Sets
#   PQXX_FOUND                 to 0 or 1 depending on result
#   PQXX_INCLUDE_DIRECTORIES  to the directory containing mysql.h
#   PQXX_LIBRARIES            to the MySQL client library (and any dependents required)
# If PQXX_REQUIRED is defined, then a fatal error message will be generated if libpqxx is not found
if ( NOT PQXX_INCLUDE_DIRECTORIES OR NOT PQXX_LIBRARIES )
  set(PostgreSQL_KNOWN_VERSIONS ${PostgreSQL_ADDITIONAL_VERSIONS}
    "13" "12" "11" "10" "9.6" "9.5" "9.4" "9.3" "9.2" "9.1" "9.0" "8.4" "8.3" "8.2" "8.1" "8.0")
  FIND_PACKAGE( PostgreSQL REQUIRED )
  if ( PostgreSQL_FOUND )
    file( TO_CMAKE_PATH "$ENV{PQXX_DIR}" _PQXX_DIR )

    find_library( PQXX_LIBRARY
      NAMES libpqxx pqxx
      PATHS
        ${_PQXX_DIR}/lib
        ${_PQXX_DIR}
        ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/bin
        
        /usr/local/pgsql/lib/${CMAKE_LIBRARY_ARCHITECTURE}
        /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
        /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
        
        /usr/local/pgsql/lib
        /usr/local/lib
        /usr/lib
      DOC "Location of libpqxx library"
      NO_DEFAULT_PATH
    )

    find_path( PQXX_HEADER_PATH
      NAMES pqxx/pqxx
      PATHS
        ${_PQXX_DIR}/include
        ${_PQXX_DIR}
        ${CMAKE_INSTALL_PREFIX}/include
        /usr/local/pgsql/include
        /usr/local/include
        /usr/include
      DOC "Path to pqxx/pqxx header file. Do not include the 'pqxx' directory in this value."
      NO_DEFAULT_PATH
    )
  endif ()
  message("-- ${PQXX_HEADER_PATH}")
  message("-- ${PQXX_LIBRARY}")
  if ( PQXX_HEADER_PATH AND PQXX_LIBRARY )

    set( PQXX_FOUND 1 CACHE INTERNAL "PQXX found" FORCE )
    set( PQXX_INCLUDE_DIRECTORIES "${PQXX_HEADER_PATH};${PostgreSQL_LIBRARIES}" CACHE STRING "Include directories for PostGreSQL C++ library"  FORCE )
    set( PQXX_LIBRARIES "${PQXX_LIBRARY};${PostgreSQL_LIBRARIES}" CACHE STRING "Link libraries for PostGreSQL C++ interface" FORCE )

    mark_as_advanced( FORCE PQXX_INCLUDE_DIRECTORIES )
    mark_as_advanced( FORCE PQXX_LIBRARIES )

  else ()
    message( "PQXX NOT FOUND" )
  endif ()

endif ()

