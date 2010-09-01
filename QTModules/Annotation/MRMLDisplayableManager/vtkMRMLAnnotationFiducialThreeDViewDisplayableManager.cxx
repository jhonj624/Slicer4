// AnnotationModule includes
#include "MRMLDisplayableManager/vtkMRMLAnnotationFiducialThreeDViewDisplayableManager.h"
#include "Logic/vtkSlicerAnnotationModuleLogic.h"

// AnnotationModule/MRML includes
#include "vtkMRMLAnnotationFiducialNode.h"
#include "vtkMRMLAnnotationNode.h"
#include "vtkMRMLAnnotationThreeDViewDisplayableManager.h"

// VTK includes
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSeedWidget.h>
#include <vtkHandleRepresentation.h>
#include <vtkSeedRepresentation.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkAbstractWidget.h>

// std includes
#include <string>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkMRMLAnnotationFiducialThreeDViewDisplayableManager);
vtkCxxRevisionMacro (vtkMRMLAnnotationFiducialThreeDViewDisplayableManager, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
// vtkMRMLAnnotationFiducialThreeDViewDisplayableManager Callback
class vtkAnnotationFiducialWidgetCallback : public vtkCommand
{
public:
  static vtkAnnotationFiducialWidgetCallback *New()
  { return new vtkAnnotationFiducialWidgetCallback; }

  vtkAnnotationFiducialWidgetCallback(){}

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
// vtkMRMLAnnotationFiducialThreeDViewDisplayableManager methods

//---------------------------------------------------------------------------
void vtkMRMLAnnotationFiducialThreeDViewDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
/// Create a new text widget.
vtkAbstractWidget * vtkMRMLAnnotationFiducialThreeDViewDisplayableManager::CreateWidget(vtkMRMLAnnotationNode* node)
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

  vtkMRMLAnnotationFiducialNode* fiducialNode = vtkMRMLAnnotationFiducialNode::SafeDownCast(node);

  if (!fiducialNode)
    {
    vtkErrorMacro("CreateWidget: Could not get fiducial node!")
    return 0;
    }

  VTK_CREATE(vtkPointHandleRepresentation3D, handle);
  handle->GetProperty()->SetColor(1,0,0);
  handle->SetHandleSize(5);

  VTK_CREATE(vtkSeedRepresentation, rep);
  rep->SetHandleRepresentation(handle);

  //seed widget
  vtkSeedWidget * seedWidget = vtkSeedWidget::New();
  seedWidget->CreateDefaultRepresentation();

  seedWidget->SetRepresentation(rep);

  seedWidget->SetInteractor(this->GetInteractor());
  seedWidget->SetCurrentRenderer(this->GetRenderer());

  //seedWidget->ProcessEventsOff();
  seedWidget->CompleteInteraction();

  seedWidget->On();

  vtkHandleWidget * h1 = this->m_HandleWidgetList[0];

  double* position1 = vtkHandleRepresentation::SafeDownCast(h1->GetRepresentation())->GetDisplayPosition();

  VTK_CREATE(vtkHandleWidget, newhandle);
  newhandle = seedWidget->CreateNewHandle();
  vtkHandleRepresentation::SafeDownCast(newhandle->GetRepresentation())->SetDisplayPosition(position1);

  // add the callback
  vtkAnnotationFiducialWidgetCallback *myCallback = vtkAnnotationFiducialWidgetCallback::New();
  myCallback->SetNode(fiducialNode);
  myCallback->SetWidget(seedWidget);
  myCallback->SetDisplayableManager(this);
  seedWidget->AddObserver(vtkCommand::EndInteractionEvent,myCallback);
  myCallback->Delete();

  seedWidget->On();

  return seedWidget;

  }

//---------------------------------------------------------------------------
/// Tear down the widget creation
void vtkMRMLAnnotationFiducialThreeDViewDisplayableManager::OnWidgetCreated(vtkAbstractWidget * widget, vtkMRMLAnnotationNode * node)
{

  if (!this->IsCorrectDisplayableManager())
    {
    // jump out
    return;
    }

  // nothing yet
}


//---------------------------------------------------------------------------
/// Propagate properties of MRML node to widget.
void vtkMRMLAnnotationFiducialThreeDViewDisplayableManager::PropagateMRMLToWidget(vtkMRMLAnnotationNode* node, vtkAbstractWidget * widget)
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
  vtkSeedWidget* seedWidget = vtkSeedWidget::SafeDownCast(widget);

  if (!seedWidget)
    {
    vtkErrorMacro("PropagateMRMLToWidget: Could not get seed widget!")
    return;
    }

  // cast to the specific mrml node
  vtkMRMLAnnotationFiducialNode* fiducialNode = vtkMRMLAnnotationFiducialNode::SafeDownCast(node);

  if (!fiducialNode)
    {
    vtkErrorMacro("PropagateMRMLToWidget: Could not get fiducial node!")
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
  vtkSeedRepresentation * rep = vtkSeedRepresentation::SafeDownCast(seedWidget->GetRepresentation());

  double position1[3];

  rep->GetSeedWorldPosition(0,position1);

  // Check if the MRML node has position set at all
  if (!fiducialNode->GetFiducialCoordinates())
    {
    fiducialNode->SetFiducialCoordinates(position1);
    hasChanged = true;
    }

  //
  // Check if the position of the widget is different than the saved one in the mrml node
  // If yes, propagate the changes to widget
  //
  if (fiducialNode->GetFiducialCoordinates()[0] != position1[0] || fiducialNode->GetFiducialCoordinates()[1] != position1[1] || fiducialNode->GetFiducialCoordinates()[2] != position1[2])
    {
    // at least one coordinate has changed, so update the widget
    // we need display coordinates here
    double * displayCoordinates = this->GetWorldToDisplayCoordinates(fiducialNode->GetFiducialCoordinates()[0],fiducialNode->GetFiducialCoordinates()[1],fiducialNode->GetFiducialCoordinates()[2]);
    rep->SetSeedDisplayPosition(0,displayCoordinates);
    hasChanged = true;
    }

  if (hasChanged)
    {
    // at least one value has changed, so set the widget to modified
    rep->NeedToRenderOn();
    seedWidget->Modified();
    }

  // enable processing of modified events
  this->m_Updating = 0;


}

//---------------------------------------------------------------------------
/// Propagate properties of widget to MRML node.
void vtkMRMLAnnotationFiducialThreeDViewDisplayableManager::PropagateWidgetToMRML(vtkAbstractWidget * widget, vtkMRMLAnnotationNode* node)
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
  vtkSeedWidget* seedWidget = vtkSeedWidget::SafeDownCast(widget);

  if (!seedWidget)
   {
   vtkErrorMacro("PropagateWidgetToMRML: Could not get seed widget!")
   return;
   }

  // cast to the specific mrml node
  vtkMRMLAnnotationFiducialNode* fiducialNode = vtkMRMLAnnotationFiducialNode::SafeDownCast(node);

  if (!fiducialNode)
   {
   vtkErrorMacro("PropagateWidgetToMRML: Could not get fiducial node!")
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
  vtkSeedRepresentation * rep = vtkSeedRepresentation::SafeDownCast(seedWidget->GetRepresentation());

  double position1[3];

  rep->GetSeedWorldPosition(0,position1);

  // Check if the MRML node has position set at all
  if (!fiducialNode->GetFiducialCoordinates())
    {
    fiducialNode->SetFiducialCoordinates(position1);
    hasChanged = true;
    }

  //
  // Check if the position of the widget is different than the saved one in the mrml node
  // If yes, propagate the changes to the mrml node
  //
  if (fiducialNode->GetFiducialCoordinates()[0] != position1[0] || fiducialNode->GetFiducialCoordinates()[1] != position1[1] || fiducialNode->GetFiducialCoordinates()[2] != position1[2])
    {
    // at least one coordinate has changed, so update the mrml property
    fiducialNode->SetFiducialCoordinates(position1);
    hasChanged = true;
    }

  if (hasChanged)
    {
    // at least one value has changed, so fire the modified event
    fiducialNode->GetScene()->InvokeEvent(vtkCommand::ModifiedEvent, fiducialNode);
    }

  // enable processing of modified events
  this->m_Updating = 0;

}

//---------------------------------------------------------------------------
/// Create a annotationMRMLnode
void vtkMRMLAnnotationFiducialThreeDViewDisplayableManager::OnClickInThreeDRenderWindow(double x, double y)
{

  if (!this->IsCorrectDisplayableManager())
    {
    // jump out
    return;
    }

  // place the seed where the user clicked
  this->PlaceSeed(x,y);

  if (this->m_ClickCounter->HasEnoughClicks(1))
    {

    // switch to updating state to avoid events mess
    this->m_Updating = 1;

    double* worldCoordinates = this->GetDisplayToWorldCoordinates(x,y);

    // create the MRML node
    vtkMRMLAnnotationFiducialNode *fiducialNode = vtkMRMLAnnotationFiducialNode::New();

    fiducialNode->SetFiducialCoordinates(worldCoordinates);

    fiducialNode->Initialize(this->GetMRMLScene());

    fiducialNode->SetName(fiducialNode->GetScene()->GetUniqueNameByString("AnnotationFiducial"));

    fiducialNode->Delete();

    // reset updating state
    this->m_Updating = 0;

    }

  }