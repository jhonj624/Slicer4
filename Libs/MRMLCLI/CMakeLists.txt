PROJECT(MRMLCLI)

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

# --------------------------------------------------------------------------
# Include dirs

SET(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

SET(packages
  MRML
  ModuleDescriptionParser
  )

FOREACH(package 
    ${packages}
    )
  # This is mostly to test building this lib as standalone outside Slicer3
  IF(NOT DEFINED ${package}_SOURCE_DIR)
    SET(${package}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../${package}")
  ENDIF(NOT DEFINED ${package}_SOURCE_DIR)
  IF(NOT DEFINED ${package}_BINARY_DIR)
    STRING(REPLACE ${PROJECT_NAME} ${package}
      ${package}_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    LINK_DIRECTORIES(${${package}_BINARY_DIR})
  ENDIF(NOT DEFINED ${package}_BINARY_DIR)

  SET(include_dirs ${include_dirs}
    ${${package}_SOURCE_DIR}
    ${${package}_BINARY_DIR}
    )
ENDFOREACH(package)

INCLUDE_DIRECTORIES(${include_dirs})

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLCLIConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMRMLCLIConfigure.h
  )

FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkMRMLCLIConfigure.h"
  DESTINATION include/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

SET(MRMLCLI_SRCS
  vtkMRMLCommandLineModuleNode.cxx
  )

# --------------------------------------------------------------------------
# Sources

# Wrapping
if (VTK_WRAP_TCL)
   include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake") 
   vtk_wrap_tcl3(MRMLCLI  MRMLCLI_TCL_SRCS   "${MRMLCLI_SRCS}" "")
endif (VTK_WRAP_TCL)

SET(srcs
 ${MRMLCLI_SRCS}
 )

if (VTK_WRAP_TCL)
  set(srcs
        ${srcs}
        ${MRMLCLI_TCL_SRCS}
   )
endif (VTK_WRAP_TCL)

# Build and install the library 
SET(lib_name MRMLCLI)
ADD_LIBRARY(${lib_name} 
  ${srcs}
 )

SET(libs
  MRML
  ModuleDescriptionParser
  )

TARGET_LINK_LIBRARIES(${lib_name}
  ${libs}
  )

# Apply user-defined properties to the library target.
IF(Slicer_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES
     ${Slicer_LIBRARY_PROPERTIES}
  )
ENDIF(Slicer_LIBRARY_PROPERTIES)

INSTALL(TARGETS ${lib_name}
  RUNTIME DESTINATION bin COMPONENT RuntimeLibraries 
  LIBRARY DESTINATION lib/${PROJECT_NAME} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION lib/${PROJECT_NAME} COMPONENT Development
  )
 
