/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLSliceNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
///  vtkMRMLSliceNode - MRML node for storing a slice through RAS space
/// 
/// This node stores the information about how to map from RAS space to 
/// the desired slice plane.
/// -- SliceToRAS is the matrix that rotates and translates the slice plane
/// -- FieldOfView tells the size of  slice plane
//

#ifndef __vtkMRMLSliceNode_h
#define __vtkMRMLSliceNode_h

#include "vtkMRML.h"
#include "vtkMRMLNode.h"

#include "vtkMatrix4x4.h"
class vtkMRMLVolumeNode;

class VTK_MRML_EXPORT vtkMRMLSliceNode : public vtkMRMLNode
{
  public:
  static vtkMRMLSliceNode *New();
  vtkTypeMacro(vtkMRMLSliceNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// 
  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// 
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// 
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  ///
  /// Reimplemented to preserve orientation when reset
  virtual void Reset();

  /// 
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Slice";};

  /// 
  /// Mapping from RAS space onto the slice plane
  /// TODO: maybe this should be a quaternion and a translate to avoid shears/scales
  vtkGetObjectMacro (SliceToRAS, vtkMatrix4x4);
  vtkSetObjectMacro (SliceToRAS, vtkMatrix4x4);

  /// 
  /// The visibility of the slice in the 3DViewer.
  vtkGetMacro ( SliceVisible, int );
  vtkSetMacro ( SliceVisible, int );

  /// 
  /// The visibility of the slice plane widget in the 3DViewer.
  vtkGetMacro ( WidgetVisible, int );
  vtkSetMacro ( WidgetVisible, int );

  /// 
  /// Use the label outline filter on this slice?
  vtkGetMacro ( UseLabelOutline, int );
  vtkSetMacro ( UseLabelOutline, int );
  vtkBooleanMacro ( UseLabelOutline, int );
  
  /// 
  /// 'standard' radiological convention views of patient space
  /// these calls adjust the SliceToRAS matrix to position the slice
  /// cutting plane 
  void SetOrientationToAxial();
  void SetOrientationToSagittal();
  void SetOrientationToCoronal();

  /// 
  /// General 'reformat' view that allows for multiplanar reformat
  void SetOrientationToReformat();
  
  ///
  /// Convenient function that calls SetOrientationToAxial(),
  /// SetOrientationToSagittal(), SetOrientationToCoronal() or
  /// SetOrientationToReformat() depending on the value of the string
  void SetOrientation(const char* orientation);

  /// Description
  /// A description of the current orientation
  /// Warning, OrientationString doesn't change the matrices, use
  /// SetOrientation() instead.
  vtkGetStringMacro (OrientationString);
  vtkSetStringMacro (OrientationString);

  /// Description
  /// The OrientationReference is a place to store the last orientation
  /// that was explicitly selected.  This way if they RotateToVolumePlane
  /// is called repeatedly it will always return the same plane
  /// (without the hint, it would first try to match, say, Coronal, and then
  /// try to match 'Reformat' but would not know what overall orientation to pick).
  vtkGetStringMacro (OrientationReference);
  vtkSetStringMacro (OrientationReference);

  /// 
  /// Size of the slice plane in millimeters
  vtkGetVector3Macro (FieldOfView, double);
  void SetFieldOfView (double x, double y, double z);

  /// 
  /// Number of samples in each direction
  /// -- note that the spacing is implicitly FieldOfView / Dimensions
  vtkGetVectorMacro(Dimensions,int,3)
  void SetDimensions (int x, int y, int z);

  /// 
  /// Number of samples in each direction for the reslice operation
  /// -- this is the resolution that each slice layer is resliced into
  /// -- the outputs of the slice layers are then composited and upsampled 
  ///    to the full Dimensions
  /// -- note that z, the number of slices, should be the same for both 
  ///    Dimensions and ResliceDimensions
  vtkGetVectorMacro(ResliceDimensions,int,3)
  void SetResliceDimensions (int x, int y, int z);

  /// 
  /// Matrix mapping from XY pixel coordinates on an image window 
  /// into slice coordinates in mm
  vtkGetObjectMacro (XYToSlice, vtkMatrix4x4);

  /// 
  /// Matrix mapping from XY pixel coordinates on an image window 
  /// into RAS world coordinates
  vtkGetObjectMacro (XYToRAS, vtkMatrix4x4);

  /// 
  /// helper for comparing to matrices
  /// TODO: is there a standard VTK method?
  int Matrix4x4AreEqual(vtkMatrix4x4 *m1, vtkMatrix4x4 *m2);

  /// 
  /// Recalculate XYToSlice and XYToRAS in terms or fov, dim, SliceToRAS
  /// - called when any of the inputs change
  void UpdateMatrices();

  /// 
  /// Name of the layout
  void SetLayoutName(const char *layoutName) {
    this->SetSingletonTag(layoutName);
  }
  char *GetLayoutName() {
    return this->GetSingletonTag();
  }

  /// 
  /// Set the number of rows and columns to use in a LightBox display
  /// of the node
  void SetLayoutGrid( int rows, int columns );

  /// 
  /// Set/Get the number of rows to use ina LightBox display
  vtkGetMacro (LayoutGridRows, int);
  virtual void SetLayoutGridRows(int rows);
  
  /// 
  /// Set/Get the number of columns to use ina LightBox display
  vtkGetMacro (LayoutGridColumns, int);
  virtual void SetLayoutGridColumns(int cols);
  
  /// 
  /// Set the SliceToRAS matrix according to the position and orientation of the locator:
  /// N(x, y, z) - the direction vector of the locator
  /// T(x, y, z) - the transverse direction vector of the locator
  /// P(x, y, z) - the tip location of the locator
  /// All the above values are in RAS space. 
  void SetSliceToRASByNTP (double Nx, double Ny, double Nz,
                           double Tx, double Ty, double Tz,
                           double Px, double Py, double Pz,
                           int Orientation);

  /// 
  /// Set the RAS offset of the Slice to the passed values. JumpSlice
  /// and JumpAllSlices use the JumpMode to determine how to jump.
  void JumpSlice(double r, double a, double s);
  void JumpAllSlices(double r, double a, double s);
  void JumpSliceByOffsetting(double r, double a, double s);
  void JumpSliceByOffsetting(int k, double r, double a, double s);
  void JumpSliceByCentering(double r, double a, double s);

  /// 
  /// Enum to specify the method of jumping slices
  //BTX
  enum {CenteredJumpSlice=0, OffsetJumpSlice};
  //ETX

  /// 
  /// Control how JumpSlice operates. CenteredJumpMode puts the
  /// specified RAS position in the center of the slice. OffsetJumpMode
  /// does not change the slice position, merely adjusts the slice
  /// offset to get the RAS position on the slice.
  vtkSetMacro(JumpMode, int);
  vtkGetMacro(JumpMode, int);
  void SetJumpModeToCentered();
  void SetJumpModeToOffset();
  
  /// 
  /// Enum to specify whether the slice spacing is automatically
  /// determined or prescribed
  //BTX
  enum {AutomaticSliceSpacingMode=0, PrescribedSliceSpacingMode};
  //ETX
  
  /// 
  /// Get/Set the slice spacing mode. Slice spacing can be
  /// automatically calculated using GetLowestVolumeSliceSpacing() or prescribed
  vtkGetMacro(SliceSpacingMode, int);
  vtkSetMacro(SliceSpacingMode, int);
  void SetSliceSpacingModeToAutomatic();
  void SetSliceSpacingModeToPrescribed();

  /// 
  /// Set/get the slice spacing to use when the SliceSpacingMode is
  /// "Prescribed"
  vtkSetVector3Macro(PrescribedSliceSpacing, double);
  vtkGetVector3Macro(PrescribedSliceSpacing, double);

  /// 
  /// Get/Set the current distance from the origin to the slice plane
  double GetSliceOffset();
  void SetSliceOffset(double offset);

  ///
  /// Set/get the active slice in the lightbox. The active slice is
  /// shown in the 3D scene
  vtkSetMacro(ActiveSlice, int);
  vtkGetMacro(ActiveSlice, int);

  ///
  /// adjusts the slice node to align with the 
  /// native space of the image data so that no oblique resampling
  /// occurs when rendering (helps to see original acquisition data
  /// and for obluique volumes with few slices).
  void RotateToVolumePlane(vtkMRMLVolumeNode *volumeNode);
  
protected:


  vtkMRMLSliceNode();
  ~vtkMRMLSliceNode();
  vtkMRMLSliceNode(const vtkMRMLSliceNode&);
  void operator=(const vtkMRMLSliceNode&);


  vtkMatrix4x4 *SliceToRAS;
  vtkMatrix4x4 *XYToSlice;
  vtkMatrix4x4 *XYToRAS;

  int JumpMode;
  
  int SliceVisible;
  int WidgetVisible;
  int UseLabelOutline;
  double FieldOfView[3];
  int Dimensions[3];
  int ResliceDimensions[3];
  char *OrientationString;
  char *OrientationReference;

  int LayoutGridRows;
  int LayoutGridColumns;

  int SliceSpacingMode;
  double PrescribedSliceSpacing[3];

  int ActiveSlice;
};

#endif

