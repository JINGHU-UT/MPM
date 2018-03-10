

# Git info
# SET( mpm_VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/generated/mpm_version.cc )

# add_custom_target(mpm_version ALL
#     ${CMAKE_COMMAND}
#     -DTEMPLATE=${CMAKE_CURRENT_SOURCE_DIR}/mpm_version.tpl.cc
#     -DOUTPUT=${mpm_VERSION_FILE}
#     -P ${CMAKE_SOURCE_DIR}/cmake/GenVersion.cmake )

# set_source_files_properties(${mpm_VERSION_FILE}
    # PROPERTIES GENERATED TRUE )

# Executables

include_directories( ../src )

add_executable ( mpm mpm.cc ${mpm_VERSION_FILE} )
# add_dependencies( mpm mpm_version )
target_link_libraries ( mpm mpm_simulate )




add_executable ( mpm2vtk mpm2vtk.cc )
target_link_libraries ( mpm2vtk mpm_visu )

