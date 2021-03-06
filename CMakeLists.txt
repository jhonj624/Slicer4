#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2)
#-----------------------------------------------------------------------------

PROJECT(Slicer)

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

#-----------------------------------------------------------------------------
# CMake Function(s) and Macro(s)
#-----------------------------------------------------------------------------

INCLUDE(CMake/Slicer3PersistenceMacros.cmake)
INCLUDE(CMake/SlicerParseArgumentsMacro.cmake)
INCLUDE(CMake/Slicer3PluginsMacros.cmake)
INCLUDE(CMake/SlicerMacroEmptyExternalProject.cmake)
INCLUDE(CMake/SlicerFunctionCheckCompilerFlags.cmake)

#-----------------------------------------------------------------------------
# Update CMake module path
#------------------------------------------------------------------------------

SET(CMAKE_MODULE_PATH
  ${VTK_CMAKE_DIR}
  ${Slicer_SOURCE_DIR}/CMake
  ${Slicer_BINARY_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  )
  
SET(Slicer_CMAKE_DIR "${Slicer_SOURCE_DIR}/CMake" CACHE INTERNAL "")

#-----------------------------------------------------------------------------
# Platform check
#-----------------------------------------------------------------------------

SET(PLATFORM_CHECK true)

if(PLATFORM_CHECK)
  # See CMake/Modules/Platform/Darwin.cmake)
  #   6.x == Mac OSX 10.2 (Jaguar)
  #   7.x == Mac OSX 10.3 (Panther)
  #   8.x == Mac OSX 10.4 (Tiger)
  #   9.x == Mac OSX 10.5 (Leopard)
  #  10.x == Mac OSX 10.6 (Snow Leopard)
  if (DARWIN_MAJOR_VERSION LESS "8")
    message(FATAL_ERROR "Only Mac OSX >= 10.4 are supported !")
  endif()
endif()

#-----------------------------------------------------------------------------
# Determine endian type
#-----------------------------------------------------------------------------

INCLUDE(TestBigEndian)
test_big_endian(CMAKE_WORDS_BIGENDIAN)

#-----------------------------------------------------------------------------
# Prerequisites
#-----------------------------------------------------------------------------

FIND_PACKAGE(Subversion)
IF(NOT Subversion_FOUND)
  MESSAGE(FATAL_ERROR "error: Install SVN and try to re-configure")
ENDIF()

FIND_PACKAGE(Git)
IF(NOT GIT_FOUND)
  MESSAGE(FATAL_ERROR "error: Install Git and try to re-configure")
ENDIF()

SET(minimum_required_qt_version "4.6.2")

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#-----------------------------------------------------------------------------

OPTION(Slicer_SUPERBUILD "Build Slicer and the projects it depends on via SuperBuild.cmake." ON)
MARK_AS_ADVANCED(Slicer_SUPERBUILD)

#-----------------------------------------------------------------------------
# Build shared lib by default
#------------------------------------------------------------------------------
# option(BUILD_SHARED_LIBS "Build Slicer with shared libraries." ON)
SET(BUILD_SHARED_LIBS ON)

#-----------------------------------------------------------------------------
# Append the library version information to the library target properties.
#------------------------------------------------------------------------------
OPTION(Slicer_WITH_LIBRARY_VERSION "Build with library version information" OFF)
MARK_AS_ADVANCED(Slicer_WITH_LIBRARY_VERSION)
IF(Slicer_WITH_LIBRARY_VERSION)
  SET(Slicer_LIBRARY_PROPERTIES ${Slicer_LIBRARY_PROPERTIES}
    VERSION ${Slicer_VERSION_FULL}
    SOVERSION ${Slicer_VERSION}
  )
ENDIF()

#-----------------------------------------------------------------------------
# Additional Slicer Options
#-----------------------------------------------------------------------------
OPTION(BUILD_DOCUMENTATION "Build Slicer Documentation" OFF)
OPTION(BUILD_TESTING "Test the project" ON)
OPTION(WITH_MEMCHECK "Run tests through valgrind." OFF)
OPTION(WITH_COVERAGE "Enable/Disable coverage" OFF)
OPTION(Slicer_USE_VTK_DEBUG_LEAKS "Enable VTKs Debug Leaks functionality in both VTK and Slicer." ON)
OPTION(Slicer_BUILD_QTLOADABLEMODULES "Build Slicer QT Loadable Modules" ON)
MARK_AS_ADVANCED(Slicer_BUILD_QTLOADABLEMODULES)
OPTION(Slicer_BUILD_CLI       "Build Slicer CLI Plugins" ON)
OPTION(Slicer_USE_PYTHONQT    "Integrate a python-QT interpreter into Slicer." ON)
OPTION(Slicer_USE_PYTHONQT_WITH_TCL "Enable PythonQt Tcl adapter layer" ON)
#MARK_AS_ADVANCED(Slicer_USE_PYTHONQT_WITH_TCL)
OPTION(Slicer_USE_BatchMake   "Build Slicer with BatchMake support." ON)
OPTION(Slicer_USE_MIDAS       "Build Slicer with MIDAS support." OFF)
OPTION(Slicer_USE_OPENIGTLINK "Build Slicer with OpenIGTLink support." ON)
OPTION(Slicer_USE_OPENCV      "Build Slicer with OpenCV support." OFF)
OPTION(Slicer_USE_CTKAPPLAUNCHER "Configure ctkAppLauncher." OFF)
MARK_AS_ADVANCED(Slicer_USE_CTKAPPLAUNCHER)

# Slicer implicitly uses Qt
SET(Slicer_USE_QT ON CACHE INTERNAL "Build Slicer with Qt")

# The following options are deprecated and will be removed 
# when the port is complete
OPTION(Slicer_USE_KWWIDGETS "Deprecated - Build Slicer with KWWidgets" OFF)
MARK_AS_ADVANCED(Slicer_USE_KWWIDGETS)
if(Slicer_USE_KWWIDGETS)
  OPTION(Slicer_USE_PYTHON "Deprecated - Integrate a python interpreter into Slicer." OFF)
  OPTION(Slicer_BUILD_MODULES "Deprecated - Build Slicer Modules" ON)
  MARK_AS_ADVANCED(Slicer_USE_PYTHON Slicer_BUILD_MODULES)
endif()

#-----------------------------------------------------------------------------
# CTestCustom
#-----------------------------------------------------------------------------
IF(BUILD_TESTING)
  CONFIGURE_FILE(
    CMake/CTestCustom.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake 
    @ONLY)
ENDIF()

#-----------------------------------------------------------------------------
# Additional CXX/C Flags
#-----------------------------------------------------------------------------

SET(ADDITIONAL_C_FLAGS "" CACHE STRING "Additional C Flags")
MARK_AS_ADVANCED(ADDITIONAL_C_FLAGS)
SET(ADDITIONAL_CXX_FLAGS "" CACHE STRING "Additional CXX Flags")
MARK_AS_ADVANCED(ADDITIONAL_CXX_FLAGS)

#-----------------------------------------------------------------------------
# Set coverage Flags
#-----------------------------------------------------------------------------
IF(WITH_COVERAGE)
  IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(coverage_flags "-g -fprofile-arcs -ftest-coverage -O0 -DNDEBUG")
    SET(COVERAGE_CXX_FLAGS ${coverage_flags})
    SET(COVERAGE_C_FLAGS ${coverage_flags})
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Slicer C/CXX Flags
#-----------------------------------------------------------------------------
SET(Slicer_C_FLAGS "${CMAKE_C_FLAGS_INIT} ${COVERAGE_C_FLAGS} ${ADDITIONAL_C_FLAGS}")
SET(Slicer_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} ${COVERAGE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(cflags "-Wall -Wextra -Wpointer-arith -Winvalid-pch -Wcast-align -Wwrite-strings -D_FORTIFY_SOURCE=2")
  SlicerFunctionCheckCompilerFlags("-fdiagnostics-show-option" cflags)
  SlicerFunctionCheckCompilerFlags("-fstack-protector-all" cflags)
  IF(MINGW)
    # suppress warnings about auto imported symbols
    SET(Slicer_CXX_FLAGS "-Wl,--enable-auto-import ${Slicer_CXX_FLAGS}")
  ENDIF()

  # Note: -Wold-style-cast is too verbose
  #       Let's postpone the use of -Wsign-promo 
  SET(Slicer_C_FLAGS "${cflags} ${Slicer_C_FLAGS}")
  SET(Slicer_CXX_FLAGS "${cflags} -Wno-deprecated -Woverloaded-virtual -Wstrict-null-sentinel ${Slicer_CXX_FLAGS}")
ELSEIF(MSVC)
  SET(cflags "/Zm1000 /W3")
  SET(Slicer_C_FLAGS "${cflags} ${Slicer_C_FLAGS}")
  SET(Slicer_CXX_FLAGS "${cflags} /EHsc /GR ${Slicer_CXX_FLAGS}")
ENDIF()

#-----------------------------------------------------------------------------
# Superbuild script
#-----------------------------------------------------------------------------

IF(Slicer_SUPERBUILD)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  RETURN()
ENDIF()

#-----------------------------------------------------------------------------
# Output directories.
#------------------------------------------------------------------------------

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Slicer_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${Slicer_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${Slicer_BINARY_DIR}/lib)

#-----------------------------------------------------------------------------
# Slicer version number.
#-----------------------------------------------------------------------------

SET(Slicer_VERSION_MAJOR "4")
SET(Slicer_VERSION_MINOR "0")
IF(NOT Slicer_VERSION_PATCH)
  SET(Slicer_VERSION_PATCH "alpha")
ENDIF()
SET(Slicer_VERSION      "${Slicer_VERSION_MAJOR}.${Slicer_VERSION_MINOR}")
SET(Slicer_VERSION_FULL "${Slicer_VERSION}.${Slicer_VERSION_PATCH}")

#-----------------------------------------------------------------------------
# Install directories
#------------------------------------------------------------------------------

SET(Slicer_INSTALL_BIN_DIR "bin")
SET(Slicer_INSTALL_LIB_DIR "lib/Slicer3")
SET(Slicer_INSTALL_INCLUDE_DIR "include/Slicer")
SET(Slicer_INSTALL_SHARE_DIR "share/Slicer")
SET(Slicer_INSTALL_ITKFACTORIES_DIR "${Slicer_INSTALL_LIB_DIR}/ITKFactories")

SET(Slicer_INSTALL_PLUGINS_BIN_DIR "${Slicer_INSTALL_LIB_DIR}/Plugins")
SET(Slicer_INSTALL_PLUGINS_LIB_DIR "${Slicer_INSTALL_LIB_DIR}/Plugins")
SET(Slicer_INSTALL_PLUGINS_INCLUDE_DIR "${Slicer_INSTALL_INCLUDE_DIR}/Plugins")
SET(Slicer_INSTALL_PLUGINS_SHARE_DIR "${Slicer_INSTALL_SHARE_DIR}/Plugins")
SET(Slicer_INSTALL_PLUGINS_CACHE_DIR "${Slicer_INSTALL_LIB_DIR}/PluginsCache")

SET(Slicer_INSTALL_MODULES_BIN_DIR "${Slicer_INSTALL_LIB_DIR}/Modules")
SET(Slicer_INSTALL_MODULES_LIB_DIR "${Slicer_INSTALL_LIB_DIR}/Modules")
SET(Slicer_INSTALL_MODULES_INCLUDE_DIR "${Slicer_INSTALL_INCLUDE_DIR}/Modules")
SET(Slicer_INSTALL_MODULES_SHARE_DIR "${Slicer_INSTALL_SHARE_DIR}/Modules")

SET(Slicer_HOME "${Slicer_BINARY_DIR}")

#-----------------------------------------------------------------------------
# Disable the warnings that DevStudio 2005 emits wrt to sprintf, strcpu, etc.
#
# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings (copied from ITK's CMake files so that ITK templated code will compile
# correctly).
IF(WIN32)
  IF(NOT BORLAND)
    IF(NOT CYGWIN)
      IF(NOT MINGW)
        ADD_DEFINITIONS(
          -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
          -D_CRT_IS_WCTYPE_NO_DEPRECATE
          -D_CRT_MANAGED_FP_NO_DEPRECATE
          -D_CRT_NONSTDC_NO_DEPRECATE
          -D_CRT_SECURE_NO_DEPRECATE
          -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
          -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
          -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
          -D_CRT_VCCLRIT_NO_DEPRECATE
          -D_SCL_SECURE_NO_DEPRECATE
          )
      ENDIF()
    ENDIF()
  ENDIF()
ENDIF()

# Disable warning C4996: [...] This function or variable may be unsafe. [...]
# To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details
IF(MSVC90)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

#-----------------------------------------------------------------------------
# Avoid linker bug in Mac OS 10.5
# See http://wiki.finkproject.org/index.php/Fink:Packaging:Preparing_for_10.5#OpenGL_Bug
#
IF(APPLE)
  SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
  SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
ENDIF()

#-----------------------------------------------------------------------------
# Slicer include and libraries subdirectory
#
slicer3_set_persistent_property(Slicer_Libs_LIBRARIES "")
slicer3_set_persistent_property(Slicer_Base_LIBRARIES "")
slicer3_set_persistent_property(Slicer_Modules_LIBRARIES "")
slicer3_set_persistent_property(Slicer_QTModules_LIBRARIES "")

slicer3_set_persistent_property(Slicer_Libs_INCLUDE_DIRS "")
slicer3_set_persistent_property(Slicer_Base_INCLUDE_DIRS "")
slicer3_set_persistent_property(Slicer_Modules_INCLUDE_DIRS "")
slicer3_set_persistent_property(Slicer_QTModules_INCLUDE_DIRS "")

#-----------------------------------------------------------------------------
# Get sytem name and architecture
#-----------------------------------------------------------------------------

SET(Slicer_BUILD NA)
INCLUDE(CMake/DiscoverSystemNameAndBits.cmake)
MESSAGE(STATUS "Slicer_BUILD:${Slicer_BUILD}")

#-----------------------------------------------------------------------------
# Testing
#-----------------------------------------------------------------------------
IF(BUILD_TESTING)
  
  SET(LAUNCH_EXE ${Slicer_BINARY_DIR}/Slicer3 --launch)
  
  ENABLE_TESTING()
  
  INCLUDE(CTest)
  
  SET(CXX_TEST_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  MARK_AS_ADVANCED(TCL_TCLSH DART_ROOT)
    
  CONFIGURE_FILE(
    ${Slicer_SOURCE_DIR}/CTestConfig.cmake
    ${Slicer_BINARY_DIR}/${Slicer_INSTALL_LIB_DIR}/CTestConfig.cmake 
    COPYONLY)
  
  CONFIGURE_FILE(
    ${Slicer_SOURCE_DIR}/CMake/RemoveTemporaryFiles.cmake.in
    ${Slicer_BINARY_DIR}/CMake/RemoveTemporaryFiles.cmake
    @ONLY)
    
  CONFIGURE_FILE(
    ${Slicer_SOURCE_DIR}/CMake/Slicer3SampleBuildTest.cmake.in
    ${Slicer_BINARY_DIR}/CMake/Slicer3SampleBuildTest.cmake 
    @ONLY)
    
ENDIF()

#-----------------------------------------------------------------------------
# Since modules are KWWidgets based, unset there variables
IF(NOT Slicer_USE_KWWIDGETS)
  # Since the module Logic is shared between QTModules and regular Modules,
  # the associated BIN, LIB and INCLUDE directory shouldn't be unset
  #UNSET(Slicer_INSTALL_MODULES_BIN_DIR)
  #UNSET(Slicer_INSTALL_MODULES_LIB_DIR)
  #UNSET(Slicer_INSTALL_MODULES_INCLUDE_DIR)
  UNSET(Slicer_INSTALL_MODULES_SHARE_DIR)
ENDIF()

# Relax some constraints related to the build settings file. This was
# preventing people from using the same compiler but with a different generator,
# or just the same compiler installed in a non-standard location.
IF(Slicer_USE_KWWIDGETS)
  SET(SKIP_KWWidgets_BUILD_SETTINGS_FILE 1)
ENDIF()
SET(SKIP_VTK_BUILD_SETTINGS_FILE 1)
SET(SKIP_ITK_BUILD_SETTINGS_FILE 1)
SET(SKIP_Teem_BUILD_SETTINGS_FILE 1)

#-----------------------------------------------------------------------------
# Python
#-----------------------------------------------------------------------------

IF(Slicer_USE_PYTHON OR Slicer_USE_PYTHONQT)
  FIND_PACKAGE(PythonLibs REQUIRED)
  FIND_PACKAGE(PythonInterp REQUIRED)
ENDIF()

#-----------------------------------------------------------------------------
# ITK
#-----------------------------------------------------------------------------
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

IF(Slicer_USE_KWWIDGETS)
#-----------------------------------------------------------------------------
# Deprecated - KWWidgets 
#-----------------------------------------------------------------------------
  FIND_PACKAGE(KWWidgets REQUIRED)
  INCLUDE(${KWWidgets_USE_FILE})
  
ELSE()
#-----------------------------------------------------------------------------
# VTK
#-----------------------------------------------------------------------------
  FIND_PACKAGE(VTK REQUIRED)
  IF(NOT VTK_USE_QVTK)
    MESSAGE(FATAL_ERROR "error: VTK was not configured to use QT, you probably need to recompile it with VTK_USE_GUISUPPORT ON, VTK_USE_QVTK ON, DESIRED_QT_VERSION 4 and QT_QMAKE_EXECUTABLE set appropriatly. Note that Qt >= ${minimum_required_qt_version} is *required*")
  ENDIF()
  INCLUDE(${VTK_USE_FILE})
  
ENDIF()

#-----------------------------------------------------------------------------
# Qt 
#-----------------------------------------------------------------------------
IF(Slicer_USE_QT)
  SET(extra_error_message)
  IF(NOT DEFINED QT_QMAKE_EXECUTABLE AND EXISTS "${VTK_QT_QMAKE_EXECUTABLE}")
    SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE})
    SET(extra_error_message "You should probably reconfigure VTK.")
  ENDIF()
  
  FIND_PACKAGE(Qt4)
  IF(NOT QT4_FOUND)
    MESSAGE(FATAL_ERROR "error: Qt >= ${minimum_required_qt_version} was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable.")
  ENDIF()
  
  # Check version, note that ${QT_VERSION_PATCH} could also be used
  IF("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" VERSION_LESS "${minimum_required_qt_version}")
    MESSAGE(FATAL_ERROR "error: Slicer requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}. ${extra_error_message}")
  ENDIF()
  
  MESSAGE(STATUS "Configuring Slicer with Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
  
  # Enable modules
  SET(QT_USE_QTNETWORK ON)
  SET(QT_USE_QTTEST ${BUILD_TESTING})

  # Includes Qt headers
  INCLUDE(${QT_USE_FILE})
ENDIF()

#-----------------------------------------------------------------------------
# CTK
#-----------------------------------------------------------------------------
SET(git_repo http://github.com/commontk/CTK/raw/master/)

IF(Slicer_USE_QT)
  FIND_PACKAGE(CTK REQUIRED)
  INCLUDE(${CTK_USE_FILE})
  # Update CMake module path
  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CTK_CMAKE_UTILITIES_DIR})
  IF(Slicer_USE_PYTHONQT)
    SET(PYTHONQT_INSTALL_DIR ${CTK_PYTHONQT_INSTALL_DIR})
    LINK_DIRECTORIES(${PYTHONQT_INSTALL_DIR}/lib)
    INCLUDE(${CTK_CMAKE_DIR}/ctkMacroWrapPythonQt.cmake)
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# PythonQt
#-----------------------------------------------------------------------------
IF(Slicer_USE_QT)
  IF(Slicer_USE_PYTHONQT)
    IF(NOT VTK_WRAP_PYTHON)
      MESSAGE(FATAL_ERROR "Since both Slicer_USE_QT and Slicer_USE_PYTHONQT are ON, VTK_WRAP_PYTHON is expected to be ON. Disable Slicer_USE_PYTHONQT or re-configure VTK with python wrapping.")
    ENDIF()
    # Include convenient script allowing to copy and compile python scripts
    INCLUDE(${Slicer_CMAKE_DIR}/SlicerCompilePythonScripts.cmake)
    # Enable qMRMLWidgets python wrapping
    SET(qMRMLWidgets_WRAP_PYTHON OFF)
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Teem
#-----------------------------------------------------------------------------
FIND_PACKAGE(Teem REQUIRED)
INCLUDE(${Teem_USE_FILE})

#-----------------------------------------------------------------------------
# SlicerLibCurl
#-----------------------------------------------------------------------------
FIND_PACKAGE(SLICERLIBCURL REQUIRED)
INCLUDE(${SLICERLIBCURL_USE_FILE})

#-----------------------------------------------------------------------------
# BatchMake
#-----------------------------------------------------------------------------
IF(Slicer_USE_BatchMake)
  FIND_PACKAGE(BatchMake REQUIRED)
  INCLUDE(${BatchMake_USE_FILE})
ENDIF()

#-----------------------------------------------------------------------------
# OpenIGTLink
#-----------------------------------------------------------------------------
IF(Slicer_USE_OPENIGTLINK)
  FIND_PACKAGE(OpenIGTLink REQUIRED)
  INCLUDE(${OpenIGTLink_USE_FILE})
ENDIF()

#-----------------------------------------------------------------------------
# OpenCV (Built in Slicer3-lib)
#-----------------------------------------------------------------------------
IF(Slicer_USE_OPENCV)
  FIND_PACKAGE(OpenCV REQUIRED)

  IF(OPENCV_BUILD_SETTINGS_FILE AND NOT SKIP_OPENCV_BUILD_SETTINGS_FILE)
    INCLUDE(${CMAKE_ROOT}/Modules/CMakeImportBuildSettings.cmake)
    CMAKE_IMPORT_BUILD_SETTINGS(${OPENCV_BUILD_SETTINGS_FILE})
  ENDIF()
  
  # Add compiler flags needed to use OPENCV.
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPENCV_REQUIRED_C_FLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENCV_REQUIRED_CXX_FLAGS}")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OPENCV_REQUIRED_LINK_FLAGS}")
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OPENCV_REQUIRED_LINK_FLAGS}")
  SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${OPENCV_REQUIRED_LINK_FLAGS}")
  
  # Add include directories needed to use OPENCV.
  INCLUDE_DIRECTORIES(BEFORE ${OPENCV_INCLUDE_DIRS})
  
  # Add link directories needed to use OPENCV.
  LINK_DIRECTORIES(${OPENCV_LIBRARY_DIRS})
  #INCLUDE(${OPENCV_USE_FILE})
ENDIF()


#-----------------------------------------------------------------------------
# Need some Tcl/Tk info
#-----------------------------------------------------------------------------
IF(VTK_WRAP_TCL)
  INCLUDE("${VTK_CMAKE_DIR}/vtkTclTkMacros.cmake")
  VTK_GET_TCL_TK_VERSION(TCL_TK_MAJOR_VERSION TCL_TK_MINOR_VERSION)
  SET(TCL_TK_VERSION_DOT "${TCL_TK_MAJOR_VERSION}.${TCL_TK_MINOR_VERSION}")
  SET(TCL_TK_VERSION "${TCL_TK_MAJOR_VERSION}${TCL_TK_MINOR_VERSION}")
  IF(Slicer_USE_KWWIDGETS)
    INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsTclTkMacros.cmake")
    incr_tcl_get_version(INCR_TCL_MAJOR_VERSION INCR_TCL_MINOR_VERSION)
    SET(INCR_TCL_VERSION_DOT "${INCR_TCL_MAJOR_VERSION}.${INCR_TCL_MINOR_VERSION}")
    SET(INCR_TCL_VERSION "${INCR_TCL_MAJOR_VERSION}${INCR_TCL_MINOR_VERSION}")
  ENDIF()
ELSE()
  # TODO: these should be passed in by SuperBuild
  # For now, use 3.2 as a fallback - we haven't changed versions in years and probably 
  # never will, so this is always likely to work.
  # This is needed so the launcher can set the paths to the correct subdirectories
  # for incrTcl and incrTk
  SET(TCL_TK_VERSION_DOT "8.4")
  SET(TCL_TK_VERSION "84")
  SET(INCR_TCL_VERSION_DOT "3.2")
  SET(INCR_TCL_VERSION "32")
ENDIF()

#-----------------------------------------------------------------------------
# DEPRECATED - Remove the following section when port to Qt is completed
#-----------------------------------------------------------------------------
# Generate the default setup paths scripts.
# Generate some useful scripts that can be used to setup various environment 
# variables and launch the executable properly against external packages.
#-----------------------------------------------------------------------------
IF(Slicer_USE_KWWIDGETS)
  INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsPathsMacros.cmake")
  SET(extra_dirs
    "${Teem_DIR}/bin"
    "${BatchMake_DIR}/bin"
    "${OpenIGTLink_DIR}/bin"
    "${Slicer_HOME}/${Slicer_INSTALL_PLUGINS_BIN_DIR}"
    "${Slicer_HOME}/${Slicer_INSTALL_PLUGINS_LIB_DIR}"
    "${Slicer_HOME}/${Slicer_INSTALL_MODULES_BIN_DIR}"
    "${Slicer_HOME}/${Slicer_INSTALL_MODULES_LIB_DIR}"
    )
  kwwidgets_generate_setup_paths_scripts(
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" "Slicer3SetupPaths"
    "${extra_dirs}"
  )
ENDIF()

#-----------------------------------------------------------------------------
# DEPRECATED - Remove the following section when port to Qt is completed
#-----------------------------------------------------------------------------
# VTK 5.0 does not have VTK_CMAKE_DIR
# Alternatively you could use KWWidget's wrapping, which encapsulate
# the differences between various VTK versions:
#  INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake")
#  KWWidgets_WRAP_TCL(${LIB_NAME} LIB_TCL_SRCS "${LIB_SRCS}" "")
#-----------------------------------------------------------------------------
IF(Slicer_USE_KWWIDGETS)
  IF(NOT VTK_CMAKE_DIR)
    SET(VTK_CMAKE_DIR "${VTK_SOURCE_DIR}/CMake")
  ENDIF()
  INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
ENDIF()


#-----------------------------------------------------------------------------
# DEPRECATED - Remove the following section when port to Qt is completed
#-----------------------------------------------------------------------------
# Check versions of VTK to track CVS HEAD
#-----------------------------------------------------------------------------
IF(Slicer_USE_KWWIDGETS)
  FIND_PACKAGE(VTK REQUIRED)
  IF(EXISTS ${VTK_CMAKE_DIR}/VTKVersionMacros.cmake)
    INCLUDE(${VTK_CMAKE_DIR}/VTKVersionMacros.cmake)
  else()
    #MESSAGE("Using local version of VTKVersionMacros for compatibility.")
    INCLUDE(${Slicer_SOURCE_DIR}/Scripts/VTKVersionMacros.cmake)
  ENDIF()

  vtk_get_source_revision_and_date(VTKsource_revision VTKsource_date)
  IF(VTKsource_revision LESS 1.2196)
    message(FATAL_ERROR "Sorry, your VTK library was last updated on ${VTKsource_date}. Its source revision, according to vtkKWWidgetsVersion.h, is ${VTKsource_revision}. Please update to a newer revision.")
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Set Slicer buildin libraries *_DIR variables
#-----------------------------------------------------------------------------
SET(TCLAP_DIR ${Slicer_BINARY_DIR}/Libs/SlicerExecutionModel/tclap)
SET(ModuleDescriptionParser_DIR ${Slicer_BINARY_DIR}/Libs/SlicerExecutionModel/ModuleDescriptionParser)
SET(GenerateCLP_DIR ${Slicer_BINARY_DIR}/Libs/SlicerExecutionModel/GenerateCLP)
SET(GenerateLM_DIR ${Slicer_BINARY_DIR}/Libs/GenerateLM)
SET(LoadableModule_DIR ${Slicer_BINARY_DIR}/Libs/LoadableModule)
SET(vtkITK_DIR ${Slicer_BINARY_DIR}/Libs/vtkITK)

#-----------------------------------------------------------------------------
# Qt Loadable Modules
#-----------------------------------------------------------------------------
IF(Slicer_USE_QT)
  IF(Slicer_BUILD_QTLOADABLEMODULES)
    ADD_DEFINITIONS(-DSlicer_BUILD_QTLOADABLEMODULES)
    SET(Slicer_INSTALL_QTLOADABLEMODULES_BIN_DIR "${Slicer_INSTALL_LIB_DIR}/QTLoadableModules")
    SET(Slicer_INSTALL_QTLOADABLEMODULES_LIB_DIR "${Slicer_INSTALL_LIB_DIR}/QTLoadableModules")
    SET(Slicer_INSTALL_QTLOADABLEMODULES_INCLUDE_DIR "${Slicer_INSTALL_INCLUDE_DIR}/QTLoadableModules")
    SET(Slicer_INSTALL_QTLOADABLEMODULES_SHARE_DIR "${Slicer_INSTALL_SHARE_DIR}/QTLoadableModules")
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Configure and install headers
# --------------------------------------------------------------------------
SET(files 
  vtkSlicerConfigure.h
  vtkSlicerVersionConfigure.h
  Scripts/runValgrind.sh
  Scripts/runValgrindForOneModule.sh
  Scripts/runValgrindForOneModuleWithSlicer4AndQT.sh
  Scripts/computeCodeCoverageLocally.sh
  Scripts/computeCodeCoverageForOneTest.sh
  Scripts/runExperimentalOffScreen.sh
  )
FOREACH(f ${files})
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/${f}.in
    ${CMAKE_CURRENT_BINARY_DIR}/${f}
    )
ENDFOREACH()

INSTALL(FILES 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkSlicerConfigure.h"
  "${CMAKE_CURRENT_BINARY_DIR}/vtkSlicerVersionConfigure.h"
  DESTINATION ${Slicer_INSTALL_INCLUDE_DIR} COMPONENT Development
  )

#-----------------------------------------------------------------------------
# Set C/CXX Flags
#-----------------------------------------------------------------------------
SET(CMAKE_CXX_FLAGS ${Slicer_CXX_FLAGS} CACHE STRING "CMake C Flags" FORCE)
SET(CMAKE_C_FLAGS ${Slicer_C_FLAGS} CACHE STRING "CMake CXX Flags" FORCE)

#-----------------------------------------------------------------------------
# Generate SlicerVersion.txt
#-----------------------------------------------------------------------------

INCLUDE(CMake/SlicerTodayMacro.cmake)

# Get build time
TODAY(Slicer_BUILDDATE)

# Note that Slicer_BUILD is set above

SET(Slicer_WC_URL NA)
SET(Slicer_WC_REVISION 0)

# Case where Slicer is checkout using SVN
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.svn)
  Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} Slicer3)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/SlicerVersion.txt.in 
    ${Slicer_HOME}/lib/Slicer3/SlicerVersion.txt
    )
ENDIF()

# Case where Slicer is checkout using Git-SVN
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git/svn/refs/remotes/trunk)
  GIT_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} Slicer3)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/SlicerVersion.txt.in
    ${Slicer_HOME}/lib/Slicer3/SlicerVersion.txt
    )
ENDIF()


#-----------------------------------------------------------------------------
# Includes
#-----------------------------------------------------------------------------

INCLUDE_DIRECTORIES(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
  )
  
#-----------------------------------------------------------------------------
# Subdirectories
#-----------------------------------------------------------------------------

ADD_SUBDIRECTORY(Libs)
ADD_SUBDIRECTORY(Base)
ADD_SUBDIRECTORY(Resources)

#-----------------------------------------------------------------------------
# At this point, let's not have the applications and modules guess which
# include paths or libraries they need, but use Slicer3_*_LIBRARIES and
# Slicer_*_INCLUDE_DIRS instead. As far as modules are concerned, both
# variables are actually set automatically by the import script when building
# against a Slicer build or installed tree. When in-source, we need to
# set both variables now. Retrieve them from the persistent property cache
# since they were collected in each subdir (which makes it impossible to
# use normal variables, since a new scope is created in each subdir).
#
slicer3_get_persistent_property(Slicer_Libs_LIBRARIES Slicer_Libs_LIBRARIES)
slicer3_get_persistent_property(Slicer_Base_LIBRARIES Slicer_Base_LIBRARIES)

slicer3_get_persistent_property(Slicer_Libs_INCLUDE_DIRS Slicer_Libs_INCLUDE_DIRS)
slicer3_get_persistent_property(Slicer_Base_INCLUDE_DIRS Slicer_Base_INCLUDE_DIRS)


#-----------------------------------------------------------------------------
# Module Logic
#-----------------------------------------------------------------------------

INCLUDE("${Slicer_CMAKE_DIR}/SlicerModuleLogicMacros.cmake")

IF(NOT Slicer_USE_KWWIDGETS)
  # KWWidgets and Qt modules share the same logics typically located in the
  # kwwdigets modules. When KWWidgets is not build, the logic are missing for Qt
  # we need to manually add them:
  # Loop through Modules subdirectory and try to include Logic subdirectory if it exists
  FILE(GLOB modules_subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} Modules/*)
  FOREACH(subdir ${modules_subdirs})
    IF(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/Logic)
      ADD_SUBDIRECTORY(${subdir}/Logic)
    ENDIF()
    FILE(GLOB subsubdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${subdir}/*)
    FOREACH(subsubdir ${subsubdirs})
      IF(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subsubdir}/Logic)
        ADD_SUBDIRECTORY(${subsubdir}/Logic)
      ENDIF()
    ENDFOREACH()
  ENDFOREACH()
ENDIF()


#-----------------------------------------------------------------------------
# DEPRECATED - Remove the following section when port to Qt is completed
#-----------------------------------------------------------------------------
# Modules and CLI
#-----------------------------------------------------------------------------
IF(Slicer_USE_KWWIDGETS)
  INCLUDE("${Slicer_CMAKE_DIR}/Slicer3ModulesMacros.cmake")
  
  IF(Slicer_BUILD_MODULES)
    ADD_SUBDIRECTORY(Modules)
  else(Slicer_BUILD_MODULES)
    # if Slicer_BUILD_CLI is ON, we still need at least the CommandLineModule
    # so that we can have CLI, without all the other modules
    IF(Slicer_BUILD_CLI)
      ADD_SUBDIRECTORY(Modules/CommandLineModule)
    ENDIF(Slicer_BUILD_CLI)
  ENDIF(Slicer_BUILD_MODULES)
ENDIF()

#-----------------------------------------------------------------------------
# Module EMSegment libraries
#-----------------------------------------------------------------------------
# The EMSegment{Registration,Graph,Algorithm,MRML} libraries are built from the top-level
# CMakeLists.txt. Doing so allow to build these libraries also if Slicer_USE_KWWIDGETS is OFF

IF(Slicer_BUILD_MODULES OR Slicer_BUILD_QTLOADABLEMODULES)
  INCLUDE("${Slicer_CMAKE_DIR}/Slicer3ModulesMacros.cmake")
  ADD_SUBDIRECTORY(Modules/EMSegment/Algorithm)
  ADD_SUBDIRECTORY(Modules/EMSegment/MRML)
  ADD_SUBDIRECTORY(Modules/EMSegment/Registration)
  ADD_SUBDIRECTORY(Modules/EMSegment/Graph)
ENDIF()

# Since EMSegment module expects Volume module and its associated library to be
# built beforehand,let's enfore that by building EMSegment here.
# Note that EMSegment has been commented in Modules/CMakeLists.txt
IF(Slicer_USE_KWWIDGETS)
  IF(Slicer_BUILD_MODULES)
    ADD_SUBDIRECTORY(Modules/EMSegment)
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Qt Loadable Modules
#-----------------------------------------------------------------------------

IF(Slicer_USE_QT AND Slicer_BUILD_QTLOADABLEMODULES)
  ADD_SUBDIRECTORY(QTModules)
ENDIF()

#-----------------------------------------------------------------------------
# Applications - Added after Modules to capture the TractTracing in the correct ordering.
#-----------------------------------------------------------------------------

ADD_SUBDIRECTORY(Applications)

#-----------------------------------------------------------------------------
# Testing
#-----------------------------------------------------------------------------

IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF()

# Install testing data

INSTALL(DIRECTORY
  Testing
  DESTINATION ${Slicer_INSTALL_SHARE_DIR} COMPONENT Development
  PATTERN ".svn" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE
  )

#-----------------------------------------------------------------------------
# Launcher for slicer
#-----------------------------------------------------------------------------

# let's build the launcher for Kww, KwwAndQt and also QtOnly slicer ...
ADD_SUBDIRECTORY(Utilities/Launcher)

# Compute Slicer_QT_PLUGIN_PATH used to configure launch.tcl.in
SET(Slicer_QT_PLUGIN_PATH "")
IF(Slicer_USE_QT)
  SET(designer_launcher_project_name SlicerQT)
  GET_TARGET_PROPERTY(output_directory ${designer_launcher_project_name} LIBRARY_OUTPUT_DIRECTORY)
  SET(Slicer_QT_PLUGIN_PATH ${output_directory})

  # on Mac we need to include the path of each Qt Framework in DYLD_LIBRARY_PATH in the launcher.
  IF(APPLE)
    SET (QTLIBLIST QTCORE QTGUI QTNETWORK QTXML QTTEST QTSQL QTOPENGL QTWEBKIT PHONON QTXMLPATTERNS)

    FOREACH(qtlib ${QTLIBLIST})
      IF(QT_${qtlib}_LIBRARY_RELEASE AND APPLE)
          GET_FILENAME_COMPONENT(QT_FRAMEWORK_NAME_tmp ${QT_${qtlib}_LIBRARY_RELEASE} NAME)
          SET(Slicer_QT_FRAMEWORK_DIRS "lib/Qt/${QT_FRAMEWORK_NAME_tmp}/Versions/4:${Slicer_QT_FRAMEWORK_DIRS}")
      ENDIF()
    ENDFOREACH()
  ENDIF()
  
  # ctkAppLauncher for slicer
  IF(Slicer_USE_CTKAPPLAUNCHER)
    INCLUDE(${CTKAPPLAUNCHER_DIR}/CMake/ctkAppLauncher.cmake)
    INCLUDE(${Slicer_CMAKE_DIR}/SlicerCTKAppLauncherSettings.cmake)
    ctkAppLauncherConfigure(
      APPLICATION_PATH ${Slicer_BINARY_DIR}/bin/SlicerQT-real${CMAKE_EXECUTABLE_SUFFIX}
      APPLICATION_INSTALL_SUBDIR ${Slicer_INSTALL_BIN_DIR}
      APPLICATION_NAME Slicer
      SPLASH_IMAGE_PATH ${Slicer_SOURCE_DIR}/Applications/SlicerQT/Resources/Images/SlicerSplashScreen.png
      SPLASH_IMAGE_INSTALL_SUBDIR ${Slicer_INSTALL_BIN_DIR}
      SPLASHSCREEN_HIDE_DELAY_MS 3000
      ADDITIONAL_HELP_SHORT_ARG "-h"
      ADDITIONAL_HELP_LONG_ARG "--help"
      ADDITIONAL_NOSPLASH_LONG_ARG "--no-splash"
      DESTINATION_DIR ${Slicer_BINARY_DIR}
      LIBRARY_PATHS_BUILD "${SLICER_LIBRARY_PATHS_BUILD}"
      PATHS_BUILD "${SLICER_PATHS_BUILD}"
      ENVVARS_BUILD "${SLICER_ENVVARS_BUILD}"
      LIBRARY_PATHS_INSTALLED "${SLICER_LIBRARY_PATHS_INSTALLED}"
      PATHS_INSTALLED "${SLICER_PATHS_INSTALLED}"
      ENVVARS_INSTALLED "${SLICER_ENVVARS_INSTALLED}"
      )
    INSTALL(PROGRAMS "${Slicer_BINARY_DIR}/Slicer${CMAKE_EXECUTABLE_SUFFIX}" DESTINATION ".")
    INSTALL(
      FILES "${Slicer_SOURCE_DIR}/Applications/SlicerQT/Resources/Images/SlicerSplashScreen.png" 
      DESTINATION ${Slicer_INSTALL_BIN_DIR}
      )
    INSTALL(
      FILES "${Slicer_BINARY_DIR}/SlicerLauncherSettingsToInstall.ini" 
      DESTINATION ${Slicer_INSTALL_BIN_DIR}
      RENAME SlicerLauncherSettings.ini
      )
  ENDIF()

ENDIF(Slicer_USE_QT)

CONFIGURE_FILE(
  ${Slicer_SOURCE_DIR}/launch.tcl.in
  "${Slicer_BINARY_DIR}/${Slicer_INSTALL_LIB_DIR}/launch.tcl"
  )
INSTALL(
  FILES "${Slicer_BINARY_DIR}/${Slicer_INSTALL_LIB_DIR}/launch.tcl"
  DESTINATION ${Slicer_INSTALL_LIB_DIR}
  )

#-----------------------------------------------------------------------------
# Launcher for QTDesigner
#-----------------------------------------------------------------------------

IF(Slicer_USE_QT)
  ADD_SUBDIRECTORY(Utilities/Designer)
ENDIF()

#-----------------------------------------------------------------------------
# Package slicer
#-----------------------------------------------------------------------------

ADD_SUBDIRECTORY(Utilities/Package)

#-----------------------------------------------------------------------------
# Documentation
#-----------------------------------------------------------------------------

IF(BUILD_DOCUMENTATION)
  ADD_SUBDIRECTORY(Utilities/Doxygen)
ENDIF()

# Add CMake directory so that install rules associated with *.cmake files are evaluated
ADD_SUBDIRECTORY(CMake)

#-----------------------------------------------------------------------------
# Set up for building against a Slicer build tree and a Slicer installation 
#
INCLUDE(${Slicer_SOURCE_DIR}/GenerateSlicerConfig.cmake)

CONFIGURE_FILE(${Slicer_SOURCE_DIR}/UseSlicer.cmake.in
  ${Slicer_BINARY_DIR}/UseSlicer.cmake COPYONLY IMMEDIATE)

INSTALL(FILES
  #   ${Slicer_BINARY_DIR}/Slicer3BuildSettings.cmake
  ${Slicer_BINARY_DIR}/UseSlicer.cmake
  ${Slicer_BINARY_DIR}/Utilities/SlicerConfig.cmake
  ${Slicer_BINARY_DIR}/lib/Slicer3/SlicerVersion.txt
  DESTINATION ${Slicer_INSTALL_LIB_DIR}
  COMPONENT Development
  )
