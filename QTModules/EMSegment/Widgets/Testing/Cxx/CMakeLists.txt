
SET(KIT EMSegmentMRMLWidgets)

SET(CMAKE_TESTDRIVER_AFTER_TESTMAIN  "DEBUG_LEAKS_ENABLE_EXIT_ERROR();")
CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  qSlicerEMSegmentAnatomicalTreeWidgetTest1.cxx
  qSlicerEMSegmentGraphWidgetTest1.cxx
  vtkPlotGaussianTest1.cxx
  
  EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CxxTests.cxx)

SET(LIBRARY_NAME ${lib_name})

ADD_EXECUTABLE(${KIT}CxxTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CxxTests ${LIBRARY_NAME})

SET( KIT_TESTS ${CXX_TEST_PATH}/${KIT}CxxTests)
IF(WIN32)
  SET(KIT_TESTS ${CXX_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CxxTests)
ENDIF(WIN32)

#
# Convenient macros
#

MACRO(SIMPLE_TEST  TESTNAME)
  ADD_TEST( ${TESTNAME} ${LAUNCH_EXE} ${KIT_TESTS} ${TESTNAME} )
ENDMACRO()

MACRO(SCENE_TEST TESTNAME SCENEFILENAME)
  ADD_TEST( ${TESTNAME}_${SCENEFILENAME}
    ${LAUNCH_EXE}  ${KIT_TESTS} ${TESTNAME}
    ${EMSegment_TESTING_DATA_SOURCE_DIR}/${SCENEFILENAME}
    )
ENDMACRO()

#
# Add Tests
#

SCENE_TEST(qSlicerEMSegmentAnatomicalTreeWidgetTest1 EMSeg-Brain-MRT1T2_small.mrml)
SIMPLE_TEST(qSlicerEMSegmentGraphWidgetTest1)
SIMPLE_TEST(vtkPlotGaussianTest1)
