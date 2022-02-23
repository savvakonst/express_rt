
set(PYBINDER_NAME pybinder)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

#message("Python_LIBRARIES:  ${Python_LIBRARIES}")

function(find_python_module module)
    string(TOUPPER ${module} module_upper)
    if (NOT PY_${module_upper})
        execute_process(COMMAND ${Python3_EXECUTABLE} "-c"
                "import re, ${module}; print(re.compile('/__init__.py.*').sub('',${module}.__file__))"
                RESULT_VARIABLE _${module}_status
                OUTPUT_VARIABLE _${module}_location
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )
        #ERROR_QUIET #COMMAND_ERROR_IS_FATAL=ANY
        if (_${module}_status)
            if (ARGC GREATER 1 AND ARGV1 STREQUAL "REQUIRED")
                message(FATAL_ERROR "-- Could NOT find Python module ${module}")
            else ()
                message(WARNING "-- Could NOT find Python module ${module}")
            endif ()
        else ()
            set(PY_${module_upper} "module \"${module}\": ${_${module}_location}" CACHE STRING
                    "cached name of available python module" FORCE)
        endif ()
    endif ()
    if (PY_${module_upper})
        message("-- Found python ${PY_${module_upper}}")
    endif ()
    #find_package_handle_standard_args(${module} DEFAULT_MSG PY_${module_upper})
endfunction(find_python_module)


find_python_module(mypy REQUIRED)
find_python_module(pybind11 REQUIRED)


pybind11_add_module(${PYBINDER_NAME} SHARED
        binder.cpp
        binder.h
        )


add_custom_command(TARGET ${PYBINDER_NAME} POST_BUILD
        COMMAND stubgen -m ${PYBINDER_NAME} -o ${PROJECT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        )

target_link_libraries(${PYBINDER_NAME} PRIVATE CommonLib)
target_compile_definitions(${PYBINDER_NAME} PRIVATE PY_BINDLIB_NAME=${PYBINDER_NAME})