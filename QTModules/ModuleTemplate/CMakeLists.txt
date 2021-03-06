
# Add subdirectories
ADD_SUBDIRECTORY(Logic)

# Module name (usually identique to the directory name)
SET(qt_module_name "ModuleTemplate")

SET(qt_module_export_directive "Q_SLICER_QTMODULES_MODULETEMPLATE_EXPORT")

# Additional includes - Current_{source,binary} and Slicer_{Libs,Base} already included
SET(qt_module_include_directories
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  )

# Source files
SET(qt_module_SRCS
  qSlicerModuleTemplateModule.cxx
  qSlicerModuleTemplateModule.h
  qSlicerModuleTemplateModuleWidget.cxx
  qSlicerModuleTemplateModuleWidget.h
  )

# Headers that should run through moc
SET(qt_module_MOC_SRCS
  qSlicerModuleTemplateModule.h
  qSlicerModuleTemplateModuleWidget.h
  )

# UI files
SET(qt_module_UI_SRCS
  Resources/UI/qSlicerModuleTemplateModule.ui
)

# Additional Target libraries
SET(qt_module_target_libraries
  vtkSlicerModuleTemplateModuleLogic
  )

# Resources
SET(qt_module_resources
  Resources/qSlicerModuleTemplateModule.qrc
)

Slicer_build_qtmodule(
  NAME ${qt_module_name}
  EXPORT_DIRECTIVE ${qt_module_export_directive}
  INCLUDE_DIRECTORIES ${qt_module_include_directories}
  SRCS ${qt_module_SRCS}
  MOC_SRCS ${qt_module_MOC_SRCS}
  UI_SRCS ${qt_module_UI_SRCS}
  TARGET_LIBRARIES ${qt_module_target_libraries}
  RESOURCES ${qt_module_resources}
  )


if(BUILD_TESTING)
  #add_subdirectory(Testing)
endif(BUILD_TESTING)
