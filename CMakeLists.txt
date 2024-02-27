cmake_minimum_required(VERSION 3.15)


set(CMAKE_CXX_STANDARD 20)


project(
    Assignment4
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

set(exec_name A4)
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
          $<TARGET_FILE:sfml-window>
          $<TARGET_FILE:sfml-system>
          $<TARGET_FILE_DIR:${exec_name}>)
  endif()
endif()

file(
    COPY ${CMAKE_SOURCE_DIR}/assets/ 
    DESTINATION ${CMAKE_BINARY_DIR}/assets/
)

file(
    COPY ${CMAKE_SOURCE_DIR}/config/
    DESTINATION ${CMAKE_BINARY_DIR}/config/
)
