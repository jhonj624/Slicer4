// AnnotationModule includes
#include "MRMLDisplayableManager/vtkMRMLAnnotationSplineThreeDViewDisplayableManager.h"
#include "Logic/vtkSlicerAnnotationModuleLogic.h"

// AnnotationModule/MRML includes
#include "vtkMRMLAnnotationSplineNode.h"
#include "vtkMRMLAnnotationNode.h"
#include "vtkMRMLAnnotationThreeDViewDisplayableManager.h"

// VTK includes
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkSplineWidget2.h>
#include <vtkRenderer.h>
#include <vtkHandleRepresentation.h>
#include <vtkSplineRepresentation.h>
#include <vtkInteractorEventRecorder.h>
#include <vtkAbstractWidget.h>

// std includes
#include <string>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkMRMLAnnotationSplineThreeDViewDisplayableManager);
vtkCxxRevisionMacro (vtkMRMLAnnotationSplineThreeDViewDisplayableManager, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
// vtkMRMLAnnotationSplineThreeDViewDisplayableManager Callback
class vtkAnnotationSplineWidgetCallback : public vtkCommand
{
public:
  static vtkAnnotationSplineWidgetCallback *New()
  { return new vtkAnnotationSplineWidgetCallback; }

  vtkAnnotationSplineWidgetCallback(){}

  virtual void Execute (vtkObject *caller, unsigned long event, void*)
  {
    if (event == vtkCommand::EndInteractionEvent)
      {

      // sanity checks
      if (!this->m_DisplayableManager)
        {
        return;
        }
      if (!this->m_Node)
        {
        return;
        }
      if (!this->m_Widget)
        {
        return;
        }
      // sanity checks end

      // the interaction with the widget ended, now propagate the changes to MRML
      this->m_DisplayableManager->PropagateWidgetToMRML(this->m_Widget, this->m_Node);

      }
  }

  void SetWidget(vtkAbstractWidget *w)
  {
    this->m_Widget = w;
  }
  void SetNode(vtkMRMLAnnotationNode *n)
  {
    this->m_Node = n;
  }
  void SetDisplayableManager(vtkMRMLAnnotationThreeDViewDisplayableManager * dm)
  {
    this->m_DisplayableManager = dm;
  }

  vtkAbstractWidget * m_Widget;
  vtkMRMLAnnotationNode * m_Node;
  vtkMRMLAnnotationThreeDViewDisplayableManager * m_DisplayableManager;
};

//---------------------------------------------------------------------------
// vtkMRMLAnnotationSplineThreeDViewDisplayableManager methods

//---------------------------------------------------------------------------
void vtkMRMLAnnotationSplineThreeDViewDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
/// Create a new text widget.
vtkAbstractWidget * vtkMRMLAnnotationSplineThreeDViewDisplayableManager::CreateWidget(vtkMRMLAnnotationNode* node)
{
  if (!this->IsCorrectDisplayableManager())
    {
    // jump out
    return 0;
    }

  if (!node)
    {
    vtkErrorMacro("CreateWidget: Node not set!")
    return 0;
    }

  vtkMRMLAnnotationSplineNode* splineNode = vtkMRMLAnnotationSplineNode::SafeDownCast(node);

  if (!splineNode)
    {
    vtkErrorMacro("CreateWidget: Could not get spline node!")
    return 0;
    }

  vtkSplineWidget2 * splineWidget = vtkSplineWidget2::New();

  splineWidget->SetInteractor(this->GetInteractor());
  splineWidget->SetCurrentRenderer(this->GetRenderer());

  splineWidget->On();

  splineWidget->CreateDefaultRepresentation();

  vtkSplineRepresentation * rep = vtkSplineRepresentation::SafeDownCast(splineWidget->GetRepresentation());
  rep->SetNumberOfHandles(5);

  rep->SetHandlePosition(0,splineNode->GetControlPointCoordinates(0));
  rep->SetHandlePosition(1,splineNode->GetControlPointCoordinates(1));
  rep->SetHandlePosition(2,splineNode->GetControlPointCoordinates(2));
  rep->SetHandlePosition(3,splineNode->GetControlPointCoordinates(3));
  rep->SetHandlePosition(4,splineNode->GetControlPointCoordinates(4));

  rep->EndWidgetInteraction(splineNode->GetControlPointCoordinates(4));

  // add observer for end interaction
  vtkAnnotationSplineWidgetCallback *myCallback = vtkAnnotationSplineWidgetCallback::New();
  myCallback->SetNode(splineNode);
  myCallback->SetWidget(splineWidget);
  myCallback->SetDisplayableManager(this);
  splineWidget->AddObserver(vtkCommand::EndInteractionEvent,myCallback);
  myCallback->Delete();

  vtkDebugMacro("CreateWidget: Widget was set up")

  return splineWidget;

}

//---------------------------------------------------------------------------
/// Tear down the widget creation
void vtkMRMLAnnotationSplineThreeDViewDisplayableManager::OnWidgetCreated(vtkAbstractWidget * widget, vtkMRMLAnnotationNode * node)
{

  if (!this->IsCorrectDisplayableManager())
    {
    // jump out
    return;
    }

  // propagate the widget to the MRML node
  this->PropagateWidgetToMRML(widget, node);
}


//---------------------------------------------------------------------------
/// Propagate properties of MRML node to widget.
void vtkMRMLAnnotationSplineThreeDViewDisplayableManager::PropagateMRMLToWidget(vtkMRMLAnnotationNode* node, vtkAbstractWidget * widget)
{

  if (!this->IsCorrectDisplayableManager())
    {
    // jump out
    return;
    }

  if (!widget)
    {
    vtkErrorMacro("PropagateMRMLToWidget: Widget was null!")
    return;
    }

  if (!node)
    {
    vtkErrorMacro("PropagateMRMLToWidget: MRML node was null!")
    return;
    }

  // cast to the specific widget
  vtkSplineWidget2 * splineWidget = vtkSplineWidget2::SafeDownCast(widget);

  if (!splineWidget)
    {
    vtkErrorMacro("PropagateMRMLToWidget: Could not get spline widget!")
    return;
    }

  // cast to the specific mrml node
  vtkMRMLAnnotationSplineNode * splineNode = vtkMRMLAnnotationSplineNode::SafeDownCast(node);

  if (!splineNode)
    {
    vtkErrorMacro("PropagateMRMLToWidget: Could not get spline node!")
    return;
    }

  if (this->m_Updating)
    {
    vtkDebugMacro("PropagateMRMLToWidget: Updating in progress.. Exit now.")
    return;
    }

  // disable processing of modified events
  this->m_Updating = 1;

  // if this flag is true after the checks below, the widget will be set to modified
  bool hasChanged = false;

  // now get the widget properties (coordinates, measurement etc.) and if the mrml node has changed, propagate the changes
  vtkSplineRepresentation * rep = vtkSplineRepresentation::SafeDownCast(splineWidget->GetRepresentation());

  double position1[3];
  double position2[3];
  double position3[3];
  double position4[3];
  double position5[3];


  rep->GetHandlePosition(0,position1);
  rep->GetHandlePosition(1,position2);
  rep->GetHandlePosition(2,position3);
  rep->GetHandlePosition(3,position4);
  rep->GetHandlePosition(4,position5);

  // Check if the MRML node has position set at all
  if (!splineNode->GetControlPointCoordinates(0))
    {
    splineNode->SetControlPoint(position1, 0);
    hasChanged = true;
    }

  if (!splineNode->GetControlPointCoordinates(1))
    {
    splineNode->SetControlPoint(position2, 1);
    hasChanged = true;
    }

  if (!splineNode->GetControlPointCoordinates(2))
    {
    splineNode->SetControlPoint(position3, 2);
    hasChanged = true;
    }

  if (!splineNode->GetControlPointCoordinates(3))
    {
    splineNode->SetControlPoint(position4, 3);
    hasChanged = true;
    }

  if (!splineNode->GetControlPointCoordinates(4))
    {
    splineNode->SetControlPoint(position5, 4);
    hasChanged = true;
    }

  //
  // Check if the position of the widget is different than the saved one in the mrml node
  // If yes, propagate the changes to widget
  //
  if (splineNode->GetControlPointCoordinates(0)[0] != position1[0] || splineNode->GetControlPointCoordinates(0)[1] != position1[1] || splineNode->GetControlPointCoordinates(0)[2] != position1[2])
    {
    // at least one coordinate has changed, so update the widget
    rep->SetHandlePosition(0,splineNode->GetControlPointCoordinates(0));
    hasChanged = true;
    }

  if (splineNode->GetControlPointCoordinates(1)[0] != position2[0] || splineNode->GetControlPointCoordinates(1)[1] != position2[1] || splineNode->GetControlPointCoordinates(1)[2] != position2[2])
    {
    // at least one coordinate has changed, so update the widget
    rep->SetHandlePosition(1,splineNode->GetControlPointCoordinates(1));
    hasChanged = true;
    }

  if (splineNode->GetControlPointCoordinates(2)[0] != position3[0] || splineNode->GetControlPointCoordinates(2)[1] != position3[1] || splineNode->GetControlPointCoordinates(2)[2] != position3[2])
    {
    // at least one coordinate has changed, so update the widget
    rep->SetHandlePosition(2,splineNode->GetControlPointCoordinates(2));
    hasChanged = true;
    }

  if (splineNode->GetControlPointCoordinates(3)[0] != position4[0] || splineNode->GetControlPointCoordinates(3)[1] != position4[1] || splineNode->GetControlPointCoordinates(3)[2] != position4[2])
    {
    // at least one coordinate has changed, so update the widget
    rep->SetHandlePosition(3,splineNode->GetControlPointCoordinates(3));
    hasChanged = true;
    }

  if (splineNode->GetControlPointCoordinates(4)[0] != position5[0] || splineNode->GetControlPointCoordinates(4)[1] != position5[1] || splineNode->GetControlPointCoordinates(4)[2] != position5[2])
    {
    // at least one coordinate has changed, so update the widget
    rep->SetHandlePosition(4,splineNode->GetControlPointCoordinates(4));
    hasChanged = true;
    }

  if (hasChanged)
    {
    // at least one value has changed, so set the widget to modified
    rep->NeedToRenderOn();
    splineWidget->Modified();
    }

  // enable processing of modified events
  this->m_Updating = 0;

}

//---------------------------------------------------------------------------
/// Propagate properties of widget to MRML node.
void vtkMRMLAnnotationSplineThreeDViewDisplayableManager::PropagateWidgetToMRML(vtkAbstractWidget * widget, vtkMRMLAnnotationNode* node)
{

  if (!this->IsCorrectDisplayableManager())
    {
    // jump out
    return;
    }

  if (!widget)
    {
    vtkErrorMacro("PropagateWidgetToMRML: Widget was null!")
    return;
    }

  if (!node)
    {
    vtkErrorMacro("PropagateWidgetToMRML: MRML node was null!")
    return;
    }

  // cast to the specific widget
  vtkSplineWidget2 * splineWidget = vtkSplineWidget2::SafeDownCast(widget);

  if (!splineWidget)
    {
    vtkErrorMacro("PropagateWidgetToMRML: Could not get spline widget!")
    return;
    }

  // cast to the specific mrml node
  vtkMRMLAnnotationSplineNode * splineNode = vtkMRMLAnnotationSplineNode::SafeDownCast(node);

  if (!splineNode)
    {
    vtkErrorMacro("PropagateWidgetToMRML: Could not get spline node!")
    return;
    }

  if (this->m_Updating)
    {
    vtkDebugMacro("PropagateWidgetToMRML: Updating in progress.. Exit now.")
    return;
    }

  // disable processing of modified events
  this->m_Updating = 1;

  // if this flag is true after the checks below, the modified event gets fired
  bool hasChanged = false;

  // now get the widget properties (coordinates, measurement etc.) and save it to the mrml node
  vtkSplineRepresentation * rep = vtkSplineRepresentation::SafeDownCast(splineWidget->GetRepresentation());

  double position1[3];
  double position2[3];
  double position3[3];
  double position4[3];
  double position5[3];


  rep->GetHandlePosition(0,position1);
  rep->GetHandlePosition(1,position2);
  rep->GetHandlePosition(2,position3);
  rep->GetHandlePosition(3,position4);
  rep->GetHandlePosition(4,position5);

  // Check if the MRML node has position set at all
  if (!splineNode->GetControlPointCoordinates(0))
    {
    splineNode->SetControlPoint(position1, 0);
    hasChanged = true;
    }

  if (!splineNode->GetControlPointCoordinates(1))
    {
    splineNode->SetControlPoint(position2, 1);
    hasChanged = true;
    }

  if (!splineNode->GetControlPointCoordinates(2))
    {
    splineNode->SetControlPoint(position3, 2);
    hasChanged = true;
    }

  if (!splineNode->GetControlPointCoordinates(3))
    {
    splineNode->SetControlPoint(position4, 3);
    hasChanged = true;
    }

  if (!splineNode->GetControlPointCoordinates(4))
    {
    splineNode->SetControlPoint(position5, 4);
    hasChanged = true;
    }


  //
  // Check if the position of the widget is different than the saved one in the mrml node
  // If yes, propagate the changes to the mrml node
  //
  if (splineNode->GetControlPointCoordinates(0)[0] != position1[0] || splineNode->GetControlPointCoordinates(0)[1] != position1[1] || splineNode->GetControlPointCoordinates(0)[2] != position1[2])
    {
    // at least one coordinate has changed, so update the mrml property
    splineNode->SetControlPoint(position1, 0);
    hasChanged = true;
    }

  if (splineNode->GetControlPointCoordinates(1)[0] != position2[0] || splineNode->GetControlPointCoordinates(1)[1] != position2[1] || splineNode->GetControlPointCoordinates(1)[2] != position2[2])
    {
    // at least one coordinate has changed, so update the mrml property
    splineNode->SetControlPoint(position2,1);
    hasChanged = true;
    }

  if (splineNode->GetControlPointCoordinates(2)[0] != position3[0] || splineNode->GetControlPointCoordinates(2)[1] != position3[1] || splineNode->GetControlPointCoordinates(2)[2] != position3[2])
    {
    // at least one coordinate has changed, so update the mrml property
    splineNode->SetControlPoint(position3,2);
    hasChanged = true;
    }

  if (splineNode->GetControlPointCoordinates(3)[0] != position4[0] || splineNode->GetControlPointCoordinates(3)[1] != position4[1] || splineNode->GetControlPointCoordinates(3)[2] != position4[2])
    {
    // at least one coordinate has changed, so update the mrml property
    splineNode->SetControlPoint(position4,3);
    hasChanged = true;
    }

  if (splineNode->GetControlPointCoordinates(4)[0] != position5[0] || splineNode->GetControlPointCoordinates(4)[1] != position5[1] || splineNode->GetControlPointCoordinates(4)[2] != position5[2])
    {
    // at least one coordinate has changed, so update the mrml property
    splineNode->SetControlPoint(position5,3);
    hasChanged = true;
    }

  if (hasChanged)
    {
    // at least one value has changed, so fire the modified event
    splineNode->GetScene()->InvokeEvent(vtkCommand::ModifiedEvent, splineNode);
    }

  // enable processing of modified events
  this->m_Updating = 0;

}

//---------------------------------------------------------------------------
/// Create a annotationMRMLnode
void vtkMRMLAnnotationSplineThreeDViewDisplayableManager::OnClickInThreeDRenderWindow(double x, double y)
{

  if (!this->IsCorrectDisplayableManager())
    {
    // jump out
    return;
    }

  // place the seed where the user clicked
  this->PlaceSeed(x,y);

  if (this->m_ClickCounter->HasEnoughClicks(5))
    {

    // switch to updating state to avoid events mess
    this->m_Updating = 1;

    vtkHandleWidget *h1 = this->m_HandleWidgetList[0];
    vtkHandleWidget *h2 = this->m_HandleWidgetList[1];
    vtkHandleWidget *h3 = this->m_HandleWidgetList[2];
    vtkHandleWidget *h4 = this->m_HandleWidgetList[3];
    vtkHandleWidget *h5 = this->m_HandleWidgetList[4];

    double* position1 = vtkHandleRepresentation::SafeDownCast(h1->GetRepresentation())->GetWorldPosition();
    double* position2 = vtkHandleRepresentation::SafeDownCast(h2->GetRepresentation())->GetWorldPosition();
    double* position3 = vtkHandleRepresentation::SafeDownCast(h3->GetRepresentation())->GetWorldPosition();
    double* position4 = vtkHandleRepresentation::SafeDownCast(h4->GetRepresentation())->GetWorldPosition();
    double* position5 = vtkHandleRepresentation::SafeDownCast(h5->GetRepresentation())->GetWorldPosition();

    vtkMRMLAnnotationSplineNode *splineNode = vtkMRMLAnnotationSplineNode::New();

    // save the world coordinates of the points to MRML
    splineNode->SetControlPoint(position1,0);
    splineNode->SetControlPoint(position2,1);
    splineNode->SetControlPoint(position3,2);
    splineNode->SetControlPoint(position4,3);
    splineNode->SetControlPoint(position5,4);

    splineNode->Initialize(this->GetMRMLScene());

    splineNode->SetName(splineNode->GetScene()->GetUniqueNameByString("AnnotationSpline"));

    splineNode->Delete();

    // reset updating state
    this->m_Updating = 0;

    }

  }