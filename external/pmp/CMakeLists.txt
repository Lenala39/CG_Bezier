file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)


if (WIN32 OR EMSCRIPTEN)
  add_library(pmp STATIC ${SOURCES} ${HEADERS})
elseif(UNIX)
  add_library(pmp SHARED ${SOURCES} ${HEADERS})
endif()

set(PMP_MAJOR_VERSION 0)
set(PMP_MINOR_VERSION 1)
set(PMP_PATCH_VERSION 0)
set(PMP_VERSION ${PMP_MAJOR_VERSION}.${PMP_MINOR_VERSION}.${PMP_PATCH_VERSION})
set_target_properties(pmp PROPERTIES VERSION ${PMP_VERSION})

# check for recent cmake version
if(${CMAKE_VERSION} VERSION_GREATER "3.6.0")
  if(CLANG_TIDY_EXE AND FALSE) # disabled by default
    set_target_properties(
      pmp PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
      )
  endif()
endif()


if(NOT EMSCRIPTEN)

  target_include_directories(pmp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:include/>)

  if((UNIX OR APPLE) AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(pmp PUBLIC "-std=c++11")
  endif()

  install(TARGETS pmp EXPORT pmpTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

  install(FILES ${HEADERS} DESTINATION include/pmp/)

endif()

add_subdirectory(gl)

include(algorithms/CMakeLists.txt)
include(io/CMakeLists.txt)
