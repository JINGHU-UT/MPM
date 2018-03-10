MACRO(ADD_MPM_LIB lib )

  execute_process( COMMAND ../updateCMakeSources.rb ${lib} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
  file(STRINGS ${lib}/CMakeSources.txt SRC_LIST)

  include_directories(. ${lib})
  add_library(${CMAKE_PROJECT_NAME}_${lib} SHARED ${SRC_LIST})
  target_link_libraries(${CMAKE_PROJECT_NAME}_${lib} ${ARGN} )

ENDMACRO( ADD_MPM_LIB )


ADD_MPM_LIB(utils)
ADD_MPM_LIB(geometry  mpm_utils )
ADD_MPM_LIB(solve ${EXTERNAL_LIBRARIES} )
ADD_MPM_LIB(simulate mpm_utils mpm_geometry mpm_solve ${EXTERNAL_LIBRARIES} )

ADD_MPM_LIB(visu mpm_utils mpm_geometry ${EXTERNAL_LIBRARIES} )

# ADD_MPM_LIB(mpm_utils mpm_geo ${EXTERNAL_LIBRARIES} )

if( OPENGL_FOUND )
    # ADD_MPM_LIB(gl mpm_utils mpm_geo mpm_visu ${OPENGL_LIBRARIES})
endif()


