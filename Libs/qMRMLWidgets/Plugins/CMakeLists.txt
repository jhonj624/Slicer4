PROJECT(qMRMLWidgetsPlugin)

#
# See CMake/qCTKDesignerPluginMacros.cmake for details
#

# Source files
SET(designer_plugin_SRCS
  qMRMLCollapsibleButtonPlugin.cxx
  qMRMLCollapsibleButtonPlugin.h
  qMRMLColorTableComboBoxPlugin.cxx
  qMRMLColorTableComboBoxPlugin.h
  qMRMLLabelComboBoxPlugin.cxx
  qMRMLLabelComboBoxPlugin.h
  qMRMLLayoutWidgetPlugin.cxx
  qMRMLLayoutWidgetPlugin.h
  qMRMLLinearTransformSliderPlugin.cxx
  qMRMLLinearTransformSliderPlugin.h
  qMRMLListWidgetPlugin.cxx
  qMRMLListWidgetPlugin.h
  qMRMLMatrixWidgetPlugin.cxx
  qMRMLMatrixWidgetPlugin.h
  qMRMLNodeComboBoxPlugin.cxx
  qMRMLNodeComboBoxPlugin.h
  qMRMLRangeWidgetPlugin.cxx
  qMRMLRangeWidgetPlugin.h
  qMRMLROIWidgetPlugin.cxx
  qMRMLROIWidgetPlugin.h
  qMRMLSliceControllerWidgetPlugin.cxx
  qMRMLSliceControllerWidgetPlugin.h
  qMRMLSliceInformationWidgetPlugin.cxx
  qMRMLSliceInformationWidgetPlugin.h
  qMRMLSliceWidgetPlugin.cxx
  qMRMLSliceWidgetPlugin.h
  qMRMLSlicesControllerToolBarPlugin.cxx
  qMRMLSlicesControllerToolBarPlugin.h
  qMRMLThreeDViewPlugin.cxx
  qMRMLThreeDViewPlugin.h
  qMRMLThreeDViewsControllerWidgetPlugin.cxx
  qMRMLThreeDViewsControllerWidgetPlugin.h
  qMRMLTransformSlidersPlugin.cxx
  qMRMLTransformSlidersPlugin.h
  qMRMLTreeWidgetPlugin.cxx
  qMRMLTreeWidgetPlugin.h
  qMRMLSceneFactoryWidgetPlugin.cxx
  qMRMLSceneFactoryWidgetPlugin.h
  qMRMLVolumeThresholdWidgetPlugin.cxx
  qMRMLVolumeThresholdWidgetPlugin.h
  qMRMLWidgetsAbstractPlugin.cxx
  qMRMLWidgetsAbstractPlugin.h
  qMRMLWidgetPlugin.cxx
  qMRMLWidgetPlugin.h
  qMRMLWidgetsPlugin.cxx
  qMRMLWidgetsPlugin.h
  qMRMLWindowLevelWidgetPlugin.cxx
  qMRMLWindowLevelWidgetPlugin.h
  )

# Headers that should run through moc
SET(designer_plugin_MOC_SRCS
  qMRMLWidgetsPlugin.h

  qMRMLCollapsibleButtonPlugin.h
  qMRMLColorTableComboBoxPlugin.h
  qMRMLLabelComboBoxPlugin.h
  qMRMLLayoutWidgetPlugin.h
  qMRMLLinearTransformSliderPlugin.h
  qMRMLListWidgetPlugin.h
  qMRMLMatrixWidgetPlugin.h
  qMRMLNodeComboBoxPlugin.h
  qMRMLRangeWidgetPlugin.h
  qMRMLROIWidgetPlugin.h
  qMRMLSceneFactoryWidgetPlugin.h
  qMRMLSliceControllerWidgetPlugin.h
  qMRMLSliceInformationWidgetPlugin.h
  qMRMLSliceWidgetPlugin.h
  qMRMLSlicesControllerToolBarPlugin.h
  qMRMLThreeDViewPlugin.h
  qMRMLThreeDViewsControllerWidgetPlugin.h
  qMRMLTransformSlidersPlugin.h
  qMRMLTreeWidgetPlugin.h
  qMRMLVolumeThresholdWidgetPlugin.h
  qMRMLWidgetPlugin.h
  qMRMLWindowLevelWidgetPlugin.h
  )

SET(designer_plugin_TARGET_LIBRARIES
  qMRMLWidgets
  )

qctk_build_designer_plugin(
  NAME ${PROJECT_NAME}
  SRCS ${designer_plugin_SRCS}
  MOC_SRCS ${designer_plugin_MOC_SRCS}
  TARGET_LIBRARIES ${designer_plugin_TARGET_LIBRARIES}
  )
