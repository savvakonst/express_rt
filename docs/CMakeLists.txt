find_package(Doxygen REQUIRED)


# Find all the public headers
get_target_property(EXRT_PUBLIC_HEADER_DIR CommonLib INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE EXRT_PUBLIC_HEADERS_TEMP ${EXRT_PUBLIC_HEADER_DIR}/*.h)
list(APPEND EXRT_PUBLIC_HEADERS ${EXRT_PUBLIC_HEADERS_TEMP})

get_target_property(EXRT_PUBLIC_HEADER_DIR Plotter INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE EXRT_PUBLIC_HEADERS_TEMP ${EXRT_PUBLIC_HEADER_DIR}/*.h)
list(APPEND EXRT_PUBLIC_HEADERS ${EXRT_PUBLIC_HEADERS_TEMP})

get_target_property(EXRT_PUBLIC_HEADER_DIR Plotter INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE EXRT_PUBLIC_HEADERS_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/*.dox)
list(APPEND EXRT_PUBLIC_HEADERS ${EXRT_PUBLIC_HEADERS_TEMP})


#string(REPLACE ";" "\\\n " EXRT_PUBLIC_HEADERS_STR "${EXRT_PUBLIC_HEADERS}")
string(REPLACE ";" " " EXRT_PUBLIC_HEADERS_STR "${EXRT_PUBLIC_HEADERS}")

message(STATUS "---------------------EXRT_PUBLIC_HEADERS_STR ${EXRT_PUBLIC_HEADERS_STR}")
message(STATUS "---------------------EXRT_PUBLIC_HEADERS ${EXRT_PUBLIC_HEADERS}")
message(STATUS "---------------------CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "---------------------CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")

set(DOXYGEN_INPUT_DIR ${EXRT_PUBLIC_HEADERS_STR})
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

#Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us

add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
        DEPENDS ${EXRT_PUBLIC_HEADERS}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
        COMMENT "Generating docs")

add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})


find_program(SPHINX_EXECUTABLE
        NAMES sphinx-build
        DOC "Path to sphinx-build executable")

include(FindPackageHandleStandardArgs)

#Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Sphinx
        "Failed to find sphinx-build executable"
        SPHINX_EXECUTABLE)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)


add_custom_target(Sphinx ALL
        COMMAND
        ${SPHINX_EXECUTABLE} -b html

        -Dbreathe_projects.exrt=${DOXYGEN_OUTPUT_DIR}/xml
        ${SPHINX_SOURCE} ${SPHINX_BUILD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Sphinx")