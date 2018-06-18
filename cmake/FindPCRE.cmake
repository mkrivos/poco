#find_library(QWT_LIBRARY_RELEASE NAMES ${qwtLibNames} PATHS /usr/lib64 /usr/lib /usr/local/lib)

#find_library(QWT_LIBRARY_DEBUG NAMES ${qwtLibNamesd} PATHS /usr/lib64 /usr/lib /usr/local/lib)

#set(QWT_LIBRARY debug ${QWT_LIBRARY_DEBUG} optimized ${QWT_LIBRARY_RELEASE})

#select_library_configurations(QWT)

#
# $Id$
#
# - Find pcre
# Find the native PCRE includes and library
#
#  PCRE_INCLUDE_DIRS - where to find pcre.h, etc.
#  PCRE_LIBRARIES    - List of libraries when using pcre.
#  PCRE_FOUND        - True if pcre found.


IF (PCRE_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(PCRE_FIND_QUIETLY TRUE)
ENDIF (PCRE_INCLUDE_DIRS)

FIND_PATH(PCRE_INCLUDE_DIR pcre.h)

FIND_LIBRARY(PCRE_LIBRARY_DEBUG NAMES pcred PATH_SUFFIXES lib)
FIND_LIBRARY(PCRE_LIBRARY_RELEASE NAMES pcre PATH_SUFFIXES lib)
include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
select_library_configurations(PCRE)

# handle the QUIETLY and REQUIRED arguments and set PCRE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCRE DEFAULT_MSG PCRE_LIBRARY PCRE_INCLUDE_DIR)

IF(PCRE_FOUND)
  SET( PCRE_LIBRARIES ${PCRE_LIBRARY} )
  SET( PCRE_INCLUDE_DIRS ${PCRE_INCLUDE_DIR} )
ELSE(PCRE_FOUND)
  SET( PCRE_LIBRARIES )
  SET( PCRE_INCLUDE_DIRS )
ENDIF(PCRE_FOUND)

MARK_AS_ADVANCED( PCRE_LIBRARIES PCRE_INCLUDE_DIRS )
