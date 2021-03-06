project(ModuleDescriptionParserTesting)

#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#-----------------------------------------------------------------------------

SET(project_policies
  CMP0001 # NEW: CMAKE_BACKWARDS_COMPATIBILITY should no longer be used.
  CMP0002 # NEW: Logical target names must be globally unique.
  CMP0003 # NEW: Libraries linked via full path no longer produce linker search paths.
  CMP0004 # NEW: Libraries linked may NOT have leading or trailing whitespace.
  CMP0005 # NEW: Preprocessor definition values are now escaped automatically.
  CMP0006 # NEW: Installing MACOSX_BUNDLE targets requires a BUNDLE DESTINATION.
  CMP0007 # NEW: List command no longer ignores empty elements.
  CMP0008 # NEW: Libraries linked by full-path must have a valid library file name.
  CMP0009 # NEW: FILE GLOB_RECURSE calls should not follow symlinks by default.
  CMP0010 # NEW: Bad variable reference syntax is an error.
  CMP0011 # NEW: Included scripts do automatic cmake_policy PUSH and POP.
  CMP0012 # NEW: if() recognizes numbers and boolean constants.
  CMP0013 # NEW: Duplicate binary directories are not allowed.
  CMP0014 # NEW: Input directories must have CMakeLists.txt
  )
FOREACH(policy ${project_policies})
  IF(POLICY ${policy})
    CMAKE_POLICY(SET ${policy} NEW)
  ENDIF()
ENDFOREACH()


set(PARSER1TEST_SOURCE Parser1Test )
add_executable(Parser1Test ${PARSER1TEST_SOURCE})
target_link_libraries(Parser1Test ModuleDescriptionParser )

set(ModuleFactoryTest_SOURCE ModuleFactoryTest )
add_executable(ModuleFactoryTest ${ModuleFactoryTest_SOURCE})
target_link_libraries(ModuleFactoryTest ModuleDescriptionParser )

include_directories(
  ${ModuleDescriptionParser_SOURCE_DIR}
  )

set(EXE_PREFIX ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
if(WIN32)
  set(EXE_PREFIX ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
endif(WIN32)

set(TEST_DATA ${ModuleDescriptionParserTesting_SOURCE_DIR}/TestData)
add_test(Parser1Test1 ${LAUNCH_EXE} ${EXE_PREFIX}/Parser1Test ${TEST_DATA}/ParserTest1.xml)

add_test(Parser1Test2 ${LAUNCH_EXE} ${EXE_PREFIX}/Parser1Test ${TEST_DATA}/ParserTest2.xml)

if(Slicer_BINARY_DIR)
  add_test(ModuleFactoryTest ${LAUNCH_EXE} ${EXE_PREFIX}/ModuleFactoryTest ${Slicer_BINARY_DIR}/${Slicer_INSTALL_PLUGINS_BIN_DIR}/)
endif(Slicer_BINARY_DIR)

