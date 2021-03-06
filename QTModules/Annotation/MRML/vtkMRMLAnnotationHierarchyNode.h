// .NAME vtkMRMLAnnotationHierarchyNode - MRML node to represent hierarchy of Annotations.
// .SECTION Description
// n/a
//

#ifndef __vtkMRMLAnnotationHierarchyNode_h
#define __vtkMRMLAnnotationHierarchyNode_h

#include "qSlicerAnnotationModuleExport.h"
#include "vtkMRMLDisplayableHierarchyNode.h"
#include "vtkMRMLNode.h"

class  Q_SLICER_QTMODULES_ANNOTATIONS_EXPORT vtkMRMLAnnotationHierarchyNode : public vtkMRMLDisplayableHierarchyNode
{
public:
  static vtkMRMLAnnotationHierarchyNode *New();
  vtkTypeMacro(vtkMRMLAnnotationHierarchyNode,vtkMRMLDisplayableHierarchyNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Read node attributes from XML file
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

protected:
  vtkMRMLAnnotationHierarchyNode();
  ~vtkMRMLAnnotationHierarchyNode();
  vtkMRMLAnnotationHierarchyNode(const vtkMRMLAnnotationHierarchyNode&);
  void operator=(const vtkMRMLAnnotationHierarchyNode&);

};

#endif
