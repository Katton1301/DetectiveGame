cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME "SecretsOfGlupchino")
project(${PROJECT_NAME} VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX .debug)

set(ENGINE_TARGET_NAME "${PROJECT_NAME}.Engine")

include_directories("$ENV{OPENGL_DIR}/include")

set(CMAKE_INSTALL_LIBDIR "lib" )
set(CMAKE_INSTALL_BINDIR "${CMAKE_SOURCE_DIR}/_binaries" )
set(CMAKE_SHARED_LIBRARY_PREFIX "")

find_package(assimp REQUIRED)

link_directories("$ENV{OPENGL_DIR}/lib")


set(static_libraries "glew32" assimp "libglfw3.a" "opengl32" "freeglut")
message(STATUS "static_libraries: ${static_libraries}")
add_compile_options(-Wall -Wextra -Wreturn-type -Wunused -Wno-terminate)

get_property(include_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
get_property(linker_options DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY LINK_OPTIONS)

function(create_target_engine LIST_DIR)
    set(CMAKE_AUTOMOC OFF)
    set(CMAKE_AUTOUIC OFF)
    set(CMAKE_AUTORCC OFF)
    set(CMAKE_AUTOUIC_SEARCH_PATHS "")
    set(sources "")
    file(
        GLOB_RECURSE
        sources
        "${LIST_DIR}/*.h"
        "${LIST_DIR}/*.hpp"
        "${LIST_DIR}/*.cpp"
    )
    include_directories(${LIST_DIR}/src)
    add_library(
        ${ENGINE_TARGET_NAME}
        STATIC ${sources}
    )
    target_link_libraries(${ENGINE_TARGET_NAME} ${static_libraries})
    target_include_directories(${ENGINE_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    install(TARGETS ${ENGINE_TARGET_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
endfunction()

function(create_target_sandbox LIST_DIR)
    set(SANDBOX_TARGET_NAME "${PROJECT_NAME}.Sandbox")
    include_directories( ${LIST_DIR}/src)
    set(sources "")
    file(
            GLOB_RECURSE
            sources
            "${LIST_DIR}/*.h"
            "${LIST_DIR}/*.hpp"
            "${LIST_DIR}/*.cpp"
    )

    file(COPY "${CMAKE_CURRENT_LIST_DIR}/bin/texture/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/texture/")

    add_executable( ${SANDBOX_TARGET_NAME} ${sources} )
    set(sandbox_libraries "")
    set(sandbox_libraries ${sandbox_libraries} "${ENGINE_TARGET_NAME}")
    target_link_libraries(${SANDBOX_TARGET_NAME} ${static_libraries})
    target_link_libraries(${SANDBOX_TARGET_NAME} ${sandbox_libraries})
    set_target_properties(${SANDBOX_TARGET_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})


    #foreach(lib ${dynamic_libraries})
    #    install_library("${lib}" "${CMAKE_INSTALL_BINDIR}/sandbox")
    #endforeach()
    #foreach(lib ${sandbox_libraries})
    #    install_library("${lib}" "${CMAKE_INSTALL_BINDIR}/sandbox")
    #endforeach()
    install( TARGETS ${SANDBOX_TARGET_NAME} DESTINATION "${CMAKE_INSTALL_BINDIR}/sandbox" )
endfunction()

create_target_engine(${CMAKE_CURRENT_LIST_DIR}/engine)
create_target_sandbox(${CMAKE_CURRENT_LIST_DIR}/sandbox)
#add_subdirectory(tests)
