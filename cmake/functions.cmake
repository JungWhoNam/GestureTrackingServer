function(print_vars)
    foreach(var IN ITEMS ${ARGN})
        message("${var} = ${${var}}")
    endforeach()
endfunction()

# From the directory where the imported target is located, glob files with the suffix.
function(install_files target suffix config)
    string(TOUPPER ${config} _upper)
    get_target_property(
        ${target}_LIBNAME 
        ${target} 
        IMPORTED_LOCATION_${_upper}
    )

    string(REGEX MATCH "^.*/" _sharedlib_glob "${${target}_LIBNAME}")
    string(APPEND _sharedlib_glob "*${suffix}*")
    # message(STATUS "_sharedlib_glob: ${_sharedlib_glob}")
    file(GLOB _sharedlibs LIST_DIRECTORIES false "${_sharedlib_glob}")
    # message(STATUS "_sharedlibs: ${_sharedlibs}")

    install(
      FILES ${_sharedlibs} 
      CONFIGURATIONS ${config}
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${config}/
    )
endfunction()