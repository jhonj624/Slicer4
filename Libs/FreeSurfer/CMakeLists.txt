project(FreeSurfer)

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

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if(NOT DEFINED BUILD_SHARED_LIBS)
  option(BUILD_SHARED_LIBS "Build with shared libraries." ON)
endif(NOT DEFINED BUILD_SHARED_LIBS)
 
# --------------------------------------------------------------------------
# Include dirs

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

include_directories(${include_dirs})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/FreeSurferConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/FreeSurferConfigure.h
  )

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(
  FILES ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/FreeSurferConfigure.h" 
  DESTINATION include/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(FreeSurfer_SRCS
  vtkFSIO.cxx
  vtkFSSurfaceReader.cxx
  vtkFSSurfaceAnnotationReader.cxx
  vtkFSSurfaceScalarReader.cxx
  vtkFSSurfaceWFileReader.cxx
  vtkFSSurfaceLabelReader.cxx
  vtkFSLookupTable.cxx
  vtkFSSurfaceHelper.cxx
  )
  
set_source_files_properties(
  vtkFSIO.cxx
  WRAP_EXCLUDE
)

# --------------------------------------------------------------------------
# Wrapping
IF (VTK_WRAP_TCL)
  include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
  vtk_wrap_tcl3(FreeSurfer FreeSurfer_TCL_SRCS "${FreeSurfer_SRCS}" "")
ENDIF (VTK_WRAP_TCL)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name FreeSurfer)
SET(srcs ${FreeSurfer_SRCS})

IF (VTK_WRAP_TCL)
  SET(srcs ${srcs} ${FreeSurfer_TCL_SRCS} )
ENDIF (VTK_WRAP_TCL)

add_library(${lib_name} ${srcs})

SET(VTK_LIBRARIES
  vtkCommon
  vtkIO
  )
  
#SET(libs
#  )

#TARGET_LINK_LIBRARIES(${lib_name} ${libs})

FOREACH(c ${VTK_LIBRARIES})
  SET(tcl_wrap)
  IF (VTK_WRAP_TCL)
    SET(tcl_wrap TCL)
  ENDIF()
  TARGET_LINK_LIBRARIES(${lib_name} ${c}${tcl_wrap})
ENDFOREACH(c)

# Apply user-defined properties to the library target.
IF(Slicer_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES
     ${Slicer_LIBRARY_PROPERTIES}
  )
ENDIF(Slicer_LIBRARY_PROPERTIES)

install(TARGETS ${lib_name}
  RUNTIME DESTINATION bin COMPONENT RuntimeLibraries 
  LIBRARY DESTINATION lib/${PROJECT_NAME} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION lib/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Create Python wrappers if VTK is wrapped in Python
#

IF(VTK_WRAP_PYTHON AND BUILD_SHARED_LIBS)

  SET(KIT_PYTHON_LIBRARIES ${VTK_LIBRARIES})

  INCLUDE(${VTK_CMAKE_DIR}/vtkWrapPython.cmake)
  VTK_WRAP_PYTHON3(${lib_name}Python PYTHON_SRCS "${FreeSurfer_SRCS}")
  ADD_LIBRARY(${lib_name}PythonD ${PYTHON_SRCS})
  ADD_LIBRARY(${lib_name}Python MODULE ${lib_name}PythonInit.cxx)
  
  TARGET_LINK_LIBRARIES(${lib_name}PythonD ${lib_name})
  
  FOREACH(c ${KIT_PYTHON_LIBRARIES})
    TARGET_LINK_LIBRARIES(${lib_name}PythonD ${c}PythonD)
  ENDFOREACH(c)

  TARGET_LINK_LIBRARIES(${lib_name}Python ${lib_name}PythonD)

  ## Python modules on Windows must have the extension ".pyd"
  IF(WIN32 AND NOT CYGWIN)
    SET_TARGET_PROPERTIES(${lib_name}Python PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)

  INSTALL(TARGETS ${lib_name}PythonD ${lib_name}Python
    RUNTIME DESTINATION bin COMPONENT RuntimeLibraries 
    LIBRARY DESTINATION lib/${PROJECT_NAME} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION lib/${PROJECT_NAME} COMPONENT Development)

ENDIF(VTK_WRAP_PYTHON AND BUILD_SHARED_LIBS)

# --------------------------------------------------------------------------
# Install support files

# Should this location change, update Libs\MRML\vtkMRMLFreeSurferProceduralColorNode.cxx

# Copy the resource files to the build directory

set(RESFILES
  FreeSurferColorLUT.txt
  Simple_surface_labels2002.txt
  )
foreach(file ${RESFILES})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${file}
    ${CMAKE_BINARY_DIR}/share/${PROJECT_NAME}/${file}
    COPYONLY
    )
endforeach(file)
install(
  FILES ${RESFILES}
  DESTINATION share/${PROJECT_NAME}
  )

# --------------------------------------------------------------------------
# Testing (requires some of the examples)

# IF(BUILD_TESTING)
#  ADD_SUBDIRECTORY(Testing)
# ENDIF(BUILD_TESTING)

# Install the testing data

file(GLOB_RECURSE PNGFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Testing/*.png")
file(GLOB_RECURSE TCLFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Testing/*.tcl")
file(GLOB_RECURSE CURVFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Testing/*.curv")
file(GLOB_RECURSE ORIGFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Testing/*.orig")

foreach(file 
    ${PNGFILES}
    ${TCLFILES}
    ${CURVFILES}
    ${ORIGFILES}
    )
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${file}
    ${CMAKE_BINARY_DIR}/share/${PROJECT_NAME}/${file}
    COPYONLY
    )
endforeach(file) 

install(DIRECTORY
  Testing
  DESTINATION share/${PROJECT_NAME} COMPONENT Development
  PATTERN ".svn" EXCLUDE
  )
