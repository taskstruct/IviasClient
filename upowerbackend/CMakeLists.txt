project(upowerbackend)

set( upowerbackend_plugin_SRC upowerbackend.cpp )

include(GenerateExportHeader)

find_package( Qt5DBus REQUIRED )

add_definitions(-DQT_PLUGIN)

add_compiler_export_flags()

# set(CMAKE_CXX_VISIBILITY_PRESET hidden)
# set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library( upowerbackend SHARED ${upowerbackend_plugin_SRC} )

generate_export_header(upowerbackend EXPORT_FILE_NAME "upowerbackend_export.h")
     
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
 COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different "${${PROJECT_NAME}_BINARY_DIR}/upowerbackend_export.h" "${CMAKE_BINARY_DIR}"
 COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different "${${PROJECT_NAME}_BINARY_DIR}/upowerbackend_export.h" "${CMAKE_BINARY_DIR}/upowerbackend_export.h")

target_link_libraries( upowerbackend Qt5::Core Qt5::DBus )

install( TARGETS upowerbackend DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )

