# ----------------------------------------------------------------------------
#  CMake file for SEM layers module
# ----------------------------------------------------------------------------

set(MODULE_NAME ${ROOT_PROJECT}_layers)

project(${MODULE_NAME})

file(GLOB SRC_LIST *.c*)
file(GLOB HEADERS  *.h*)

add_library(${MODULE_NAME} ${SRC_LIST} ${HEADERS})

list(APPEND ${ROOT_PROJECT}_MODULES ${MODULE_NAME})
target_link_libraries(${MODULE_NAME} ${${ROOT_PROJECT}_LIBS} ${ROOT_PROJECT}_neuron)
set(${ROOT_PROJECT}_MODULES ${${ROOT_PROJECT}_MODULES} PARENT_SCOPE)

# add module's install targets, header installation is centralized
install(TARGETS ${MODULE_NAME} DESTINATION lib)
