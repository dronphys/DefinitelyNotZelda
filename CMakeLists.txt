cmake_minimum_required(VERSION 3.15)


set(CMAKE_CXX_STANDARD 20)


project(
    GameAndEngine
    LANGUAGES CXX
    VERSION 0.1.0
)

add_subdirectory(dependencies)


# make executable app
set(SRC_FILES
    src/main.cpp 
    src/EntityManager.cpp 
    src/Components.cpp
    src/Entity.cpp
    src/Vec2.cpp 
    src/Game.cpp
    src/GameEngine.cpp
    src/Scene.cpp
    src/Scene_Play.cpp
    src/Scene_Menu.cpp
    src/Scene_Editor.cpp
    src/Physics.cpp
    src/Animation.cpp
    src/Assets.cpp
    src/PathFinding.cpp
    src/MemoryPool.cpp
    src/ParticleSystem.cpp)

set(exec_name DefinitelyNotZelda)
add_executable(${exec_name} ${SRC_FILES})


set(MY_PROJECT_PATH "${CMAKE_SOURCE_DIR}")
target_compile_definitions(${exec_name} PRIVATE MY_PROJECT_PATH="${MY_PROJECT_PATH}")

target_link_libraries(${exec_name} PRIVATE ImGui-SFML::ImGui-SFML sfml-audio)
include_directories(include)

if(WIN32)
  if (BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${exec_name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
          $<TARGET_FILE:sfml-graphics>
          $<TARGET_FILE:sfml-audio>
          $<TARGET_FILE:sfml-window>
          $<TARGET_FILE:sfml-system>
          $<TARGET_FILE_DIR:${exec_name}>)
  endif()
endif()

if(WIN32)
add_custom_command(
    TARGET ${exec_name}
    COMMENT "Copy OpenAL DLL"
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${exec_name}>
    VERBATIM)
endif()



file(
    COPY ${CMAKE_SOURCE_DIR}/assets/ 
    DESTINATION ${CMAKE_BINARY_DIR}/assets/
)


