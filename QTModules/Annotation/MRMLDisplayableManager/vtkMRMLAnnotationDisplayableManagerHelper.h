/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer
 Module:    $RCSfile: vtkMRMLAnnotationDisplayableManagerHelper,v $
 Date:      $Date: Aug 4, 2010 10:44:52 AM $
 Version:   $Revision: 1.0 $

 =========================================================================auto=*/

#ifndef VTKMRMLANNOTATIONDISPLAYABLEMANAGERHELPER_H_
#define VTKMRMLANNOTATIONDISPLAYABLEMANAGERHELPER_H_

// AnnotationModule includes
#include "qSlicerAnnotationModuleExport.h"

#include "vtkMRMLAnnotationNode.h"

// VTK includes
#include <vtkObject.h>
#include <vtkAbstractWidget.h>
#include <vtkSeedWidget.h>
#include <vtkHandleWidget.h>

// MRML includes
#include <vtkMRMLSliceNode.h>

class Q_SLICER_QTMODULES_ANNOTATIONS_EXPORT vtkMRMLAnnotationDisplayableManagerHelper :
    public vtkObject
{
public:

  static vtkMRMLAnnotationDisplayableManagerHelper *New();
  vtkTypeRevisionMacro(vtkMRMLAnnotationDisplayableManagerHelper, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// Lock/Unlock a widget
  void UpdateLocked(vtkMRMLAnnotationNode* node);
  /// Hide/Show a widget
  void UpdateVisible(vtkMRMLAnnotationNode* node);
  /// Update lock and visibility of a widget
  void UpdateWidget(vtkMRMLAnnotationNode* node);

  /// Get a vtkAbstractWidget* given a node
  vtkAbstractWidget * GetWidget(vtkMRMLAnnotationNode * node);
  /// ...an its associated vtkAbstractWidget* for Slice intersection representation
  vtkAbstractWidget * GetIntersectionWidget(vtkMRMLAnnotationNode * node);
  /// Remove a node, its widget and its intersection widget
  void RemoveWidgetAndNode(vtkMRMLAnnotationNode *node);


  //----------------------------------------------------------------------------------
  // The Lists!!
  //
  // An annotation which is managed by a displayableManager consists of
  //   a) the Annotation MRML Node (AnnotationNodeList)
  //   b) the vtkWidget to show this annotation (Widgets)
  //   c) a vtkWidget to represent sliceIntersections in the slice viewers (WidgetIntersections)
  //

  /// List of Nodes managed by the DisplayableManager
  std::vector<vtkMRMLAnnotationNode*> AnnotationNodeList;

  /// .. and its associated convenient typedef
  typedef std::vector<vtkMRMLAnnotationNode*>::iterator AnnotationNodeListIt;

  /// Map of vtkWidget indexed using associated node ID
  std::map<vtkMRMLAnnotationNode*, vtkAbstractWidget *> Widgets;

  /// .. and its associated convenient typedef
  typedef std::map<vtkMRMLAnnotationNode*, vtkAbstractWidget *>::iterator WidgetsIt;

  /// Map of vtkWidgets to reflect the Slice intersections indexed using associated node ID
  std::map<vtkMRMLAnnotationNode*, vtkAbstractWidget *> WidgetIntersections;

  /// .. and its associated convenient typedef
  typedef std::map<vtkMRMLAnnotationNode*, vtkAbstractWidget *>::iterator WidgetIntersectionsIt;

  //
  // End of The Lists!!
  //
  //----------------------------------------------------------------------------------


  ///
  /// Placement of seeds for widget placement
  void PlaceSeed(double x, double y, vtkRenderWindowInteractor * interactor, vtkRenderer * renderer);

  /// Get a placed seed
  vtkHandleWidget * GetSeed(int index);

  /// Remove all placed seeds
  void RemoveSeeds();


protected:

  vtkMRMLAnnotationDisplayableManagerHelper();
  virtual ~vtkMRMLAnnotationDisplayableManagerHelper();

private:

  vtkMRMLAnnotationDisplayableManagerHelper(const vtkMRMLAnnotationDisplayableManagerHelper&); /// Not implemented
  void operator=(const vtkMRMLAnnotationDisplayableManagerHelper&); /// Not Implemented

  ///
  /// SeedWidget for point placement
  vtkSeedWidget * SeedWidget;
  ///
  /// List of Handles for the SeedWidget
  std::vector<vtkHandleWidget*> HandleWidgetList;
  ///
  /// .. and its associated convenient typedef
  typedef std::vector<vtkHandleWidget*>::iterator HandleWidgetListIt;

};

#endif /* VTKMRMLANNOTATIONDISPLAYABLEMANAGERHELPER_H_ */
