# - Find libsamplerate
# Find the libsamplerate library
# Once done this will define
#
#  This module defines the following variables:
#    LIBSAMPLERATE_FOUND        - True if LIBSAMPLERATE_NCLUDE_DIR and
#                                 LIBSAMPLERATE_LIBRARY are found.
#    LIBSAMPLERATE_INCLUDE_DIR  - Where to find samplerate.h, etc.
#    LIBSAMPLERATE_LIBRARY      - The libsamplerate library>
#    LIBSAMPLERATE_LIBRARIES    - Ditto but only if FOUND.

find_library(LIBSAMPLERATE_LIBRARY NAMES samplerate)

find_path(LIBSAMPLERATE_INCLUDE_DIR samplerate.h)
mark_as_advanced(LIBSAMPLERATE_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBSAMPLERATE DEFAULT_MSG LIBSAMPLERATE_LIBRARY LIBSAMPLERATE_INCLUDE_DIR)

if(LIBSAMPLERATE_FOUND)
	set(LIBSAMPLERATE_LIBRARIES ${LIBSAMPLERATE_LIBRARY})
endif(LIBSAMPLERATE_FOUND)
