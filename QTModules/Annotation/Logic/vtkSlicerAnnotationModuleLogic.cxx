

// QTGUI includes
// Note: The logic shouldn't deal with UI Dialog
#include <qSlicerApplication.h>
#include <qSlicerIOManager.h>

// Annotation includes
#include "Logic/vtkSlicerAnnotationModuleLogic.h"

// Annotation/MRML includes
#include "vtkMRMLAnnotationRulerNode.h"
#include "vtkMRMLAnnotationAngleNode.h"
#include "vtkMRMLAnnotationAngleStorageNode.h"
#include "vtkMRMLAnnotationTextDisplayNode.h"
#include "vtkMRMLAnnotationLineDisplayNode.h"
#include "vtkMRMLAnnotationFiducialNode.h"
#include "vtkMRMLAnnotationPointDisplayNode.h"
#include "vtkMRMLAnnotationStickyNode.h"
#include "vtkMRMLAnnotationTextNode.h"
#include "vtkMRMLAnnotationROINode.h"
#include "vtkMRMLAnnotationBidimensionalNode.h"
#include "vtkMRMLAnnotationSplineNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLTransformNode.h> // for ruler
#include <vtkMRMLLinearTransformNode.h> // for ruler
#include <vtkMRMLFiducial.h>
#include <vtkMRMLBSplineTransformNode.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkProperty.h>
#include <vtkPolygonalSurfacePointPlacer.h>
// Note: The following VTK includes should be in DisplayableManager
#include <vtkLineRepresentation.h>
#include <vtkLineWidget2.h> // for ruler
#include <vtkPointHandleRepresentation3D.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkAngleWidget.h>
#include <vtkAngleRepresentation3D.h>
#include <vtkSplineWidget.h>
#include <vtkSplineRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>


// STD includes
#include <string>
#include <iostream>
#include <sstream>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//-----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkSlicerAnnotationModuleLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkSlicerAnnotationModuleLogic);

//-----------------------------------------------------------------------------
// General Functions 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class vtkSlicerAnnotationModuleLogicPrivate: public ctkPrivate<vtkSlicerAnnotationModuleLogic>
{
public:
  vtkSlicerAnnotationModuleLogicPrivate();
  ~vtkSlicerAnnotationModuleLogicPrivate();
  //char* FiducialListID;
  char* AngleNodeID;
  int numControlPoints;
  int maxControlPoints;
  int Updating3DFiducial;
  int Updating3DRuler;
  int Updating3DAngle;
 //BTX
  // Description:
  // encapsulated 3d widgets for each ruler node
  //std::map<std::string, vtkMeasurementsAngleWidgetClass *> AngleWidgets;
  // Not using it really just as dummy place holder
  //std::map<std::string, vtkSlicerFiducialListWidget *> FiducialWidgets;
 //ETX
 
};

//-----------------------------------------------------------------------------
// vtkSlicerAnnotationModuleLogicPrivate methods

//-----------------------------------------------------------------------------
vtkSlicerAnnotationModuleLogicPrivate::vtkSlicerAnnotationModuleLogicPrivate()
{
  this->AngleNodeID = 0;
  this->Updating3DFiducial = 0;
  this->Updating3DRuler = 0;
  this->Updating3DAngle = 0;
}

//-----------------------------------------------------------------------------
vtkSlicerAnnotationModuleLogicPrivate::~vtkSlicerAnnotationModuleLogicPrivate()
{
    /*
  // 3d widgets
  std::map<std::string, vtkMeasurementsAngleWidgetClass *>::iterator iter2;
  for (iter2 = this->AngleWidgets.begin();
      iter2 != this->AngleWidgets.end();
      iter2++)
  {
      iter2->second->Delete();
  }
  this->AngleWidgets.clear(); 
*/
}

//-----------------------------------------------------------------------------
// vtkSlicerAnnotationModuleLogic methods

//-----------------------------------------------------------------------------
vtkSlicerAnnotationModuleLogic::vtkSlicerAnnotationModuleLogic()
{
  CTK_INIT_PRIVATE(vtkSlicerAnnotationModuleLogic);

  this->m_RulerManager = 0;
  this->m_AngleManager = 0;
  this->m_FiducialManager = 0;
  this->m_ROIManager = 0;
  this->m_SplineManager = 0;
  this->m_BidimensionalManager = 0;

  this->m_Widget = 0;

  this->m_LastAddedAnnotationNode = 0;

}

//-----------------------------------------------------------------------------
vtkSlicerAnnotationModuleLogic::~vtkSlicerAnnotationModuleLogic()
{
  if (this->m_RulerManager)
  {
    //this->m_RulerManager->SetParent(0);
    //this->m_RulerManager->Delete();
    this->m_RulerManager = 0;
  }
  if (this->m_AngleManager)
  {
    //this->m_AngleManager->SetParent(0);
    //this->m_AngleManager->Delete();
    this->m_AngleManager = 0;
  }
  if (this->m_FiducialManager)
  {
    //this->m_FiducialManager->SetParent(0);
    //this->m_FiducialManager->Delete();
    this->m_FiducialManager = 0;
  }
  if (this->m_ROIManager)
  {
    //this->m_ROIManager->SetParent(0);
    //this->m_ROIManager->Delete();
    this->m_ROIManager = 0;
  }
  if (this->m_SplineManager)
  {
    //this->m_SplineManager->SetParent(0);
    //this->m_SplineManager->Delete();
    this->m_SplineManager = 0;
  }
  if (this->m_BidimensionalManager)
  {
    //this->m_BidimensionalManager->SetParent(0);
    //this->m_BidimensionalManager->Delete();
    this->m_BidimensionalManager = 0;
  }
  /*if (this->m_TextManager)
  {
    //this->m_TextManager->SetParent(0);
    //this->m_TextManager->Delete();

    this->m_TextManager = 0;
}*/

}

void vtkSlicerAnnotationModuleLogic::SetAndObserveWidget(qSlicerAnnotationModuleWidget* widget)
{
  if (!widget)
    {
    return;
    }

  this->m_Widget = widget;
}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{ 
  Superclass::PrintSelf(os, indent); 
};

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData )
{

  std::cout << "vtkSlicerAnnotationModuleLogic ProcessMRMLEvents" << std::endl;

  switch(event)
    {
    case vtkMRMLScene::NodeAddedEvent:
      vtkMRMLAnnotationNode* annotationNode = reinterpret_cast<vtkMRMLAnnotationNode*>(callData);
      this->OnMRMLSceneNodeAddedEvent(annotationNode);
      break;
    }


  /*
    // Check RulerNode
    {
        vtkMRMLAnnotationRulerNode *callerNode = vtkMRMLAnnotationRulerNode::SafeDownCast(caller);
        if (callerNode != 0)
        {
            if (event == vtkMRMLAnnotationControlPointsNode::ControlPointModifiedEvent)
            {
                this->TestReceivedMessage = 1;
                //vtkWarningMacro("ProcessLogicEvents: got a control point modified event");
                // need to be sure that the modifeid event isn't coming from the widget moving
                //this->Update3DRuler(callerNode);
            }
            if (event == vtkCommand::ModifiedEvent)
            {
                this->TestReceivedMessage = 1;
                //vtkWarningMacro("ProcessLogicEvents: got a control point modified event");
                // need to be sure that the modifeid event isn't coming from the widget moving
                //this->Update3DRuler(callerNode);
            }
            if (event == vtkMRMLTransformableNode::TransformModifiedEvent)
            {
                vtkDebugMacro("ProcessLogicEvents: got a transform modified event");
                //this->Update3DRuler(callerNode);
            }
            // check for a node added event
            if (event == vtkMRMLScene::NodeAddedEvent)
            {
                vtkDebugMacro("ProcessMRMLEvents: got a node added event on scene");
                //check to see if it was a angle node    
                vtkMRMLAnnotationAngleNode *addNode = reinterpret_cast<vtkMRMLAnnotationAngleNode*>(callData);
                if (addNode != 0 && addNode->IsA("vtkMRMLAnnotationAngleNode"))
                {
                    vtkDebugMacro("Got a node added event with a angle node " << addNode->GetID());
                    return;
                }
            }
        }
    else
    {
      vtkMRMLAnnotationTextDisplayNode *callerNode = vtkMRMLAnnotationTextDisplayNode::SafeDownCast(caller);
      if (callerNode != 0)
      {
        if (event == vtkCommand::ModifiedEvent)
        {
          this->TestReceivedMessage = 1;
          //vtkWarningMacro("ProcessLogicEvents: got a control point modified event");
          // need to be sure that the modifeid event isn't coming from the widget moving
          //this->Update3DRuler(callerNode);
        }
      }
    }
    }

    // Check for fiducials

    {
        vtkMRMLFiducialListNode* callerNode = vtkMRMLFiducialListNode::SafeDownCast(caller);
        if (callerNode != 0)
        {
            if ( event == vtkMRMLScene::NodeAddedEvent ) 
            {
                return;
            }
            if ( event == vtkMRMLFiducialListNode::FiducialModifiedEvent ) 
            {
                return;
            }

            if ( event == vtkMRMLScene::NodeRemovedEvent ) 
            {
                return;
            }
        }
    }*/


}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::OnMRMLSceneNodeAddedEvent(vtkMRMLNode* node)
{
  vtkDebugMacro("OnMRMLSceneNodeAddedEvent");
  vtkMRMLAnnotationNode * annotationNode = vtkMRMLAnnotationNode::SafeDownCast(node);
  if (!annotationNode)
    {
    return;
    }

  if (annotationNode->IsA("vtkMRMLAnnotationTextNode")) {
    this->AddNodeCompleted(annotationNode);
  }
  if (annotationNode->IsA("vtkMRMLAnnotationAngleNode")) {
    this->AddNodeCompleted(annotationNode);
  }
}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::SaveMRMLScene()
{
  /*
  vtkSlicerApplicationGUI *appGUI = GetApplicationGUI();
  if ( !appGUI )
   {
     return;
   }
  appGUI->ProcessSaveSceneAsCommand();
  */
  qSlicerApplication::application()->ioManager()->openSaveDataDialog();
}

//---------------------------------------------------------------------------
vtkSlicerViewerWidget* vtkSlicerAnnotationModuleLogic::GetViewerWidget()
{
  //return this->GetApplicationGUI()->GetActiveViewerWidget();
  return 0;
}
/*
//---------------------------------------------------------------------------
vtkSlicerApplicationGUI* vtkSlicerAnnotationModuleLogic::GetApplicationGUI()
{
  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast (vtkSlicerApplication::GetInstance()); 
  if ( !app )
    {
     std::cerr << "GetApplicationGUI: got 0 SlicerApplication"  << std::endl;
    return 0;
    }
  vtkSlicerApplicationGUI *appGUI = app->GetApplicationGUI();
  if ( !appGUI )
   {
     std::cerr << "GetApplicationGUI: got 0 SlicerApplicationGUI"  << std::endl;
    return 0;
   }
  return appGUI;
 
}
*/

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::RemoveAnnotationByID(const char* id)
{
    vtkMRMLNode* node = this->GetMRMLScene()->GetNodeByID(id);

    if (node->IsA("vtkMRMLAnnotationFiducialNode"))
    {
        this->RemoveFiducial(id);
    }
    else if (node->IsA("vtkMRMLAnnotationRulerNode"))
    {
        this->RemoveRuler(id);
    }
    else if (node->IsA("vtkMRMLAnnotationAngleNode"))
    {
        this->RemoveAngle(id);
    }

}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::ModifyPropertiesAndWidget(vtkMRMLNode* node, int type, void* data)
{
    if (!node)
    {
        return;
    }
  if ( vtkMRMLAnnotationLinesNode::SafeDownCast(node) == 0 )
  {
    this->SetAnnotationControlPointsProperties(vtkMRMLAnnotationControlPointsNode::SafeDownCast(node), type, data);
  } 
  else
  {
    this->SetAnnotationLinesProperties(vtkMRMLAnnotationLinesNode::SafeDownCast(node), type, data);
  }
  
  //this->GetViewerWidget()->Render();
}

//-----------------------------------------------------------------------------
/*vtkImageData* vtkSlicerAnnotationModuleLogic::SaveScreenShot()
{
  return 0;

    vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast (vtkSlicerApplication::GetInstance()); 
    if ( !app )
    {
        std::cerr << "selectLayout: got 0 SlicerApplication"  << std::endl;
        return 0;
    }

    vtkSlicerApplicationGUI *appGUI = app->GetApplicationGUI();
    if ( !appGUI )
    {
        std::cerr << "selectLayout: got 0 SlicerApplicationGUI"  << std::endl;
        return 0;
    }

    vtkSlicerViewerWidget* viewerWidget = appGUI->GetActiveViewerWidget();
    vtkKWRenderWidget* mainViewer = viewerWidget->GetMainViewer();
    vtkRenderWindow* win = mainViewer->GetRenderWindow();

    vtkWindowToImageFilter* renderLargeImage = vtkWindowToImageFilter::New();
    renderLargeImage->SetInput( win );
    renderLargeImage->SetMagnification( 1 );
    
    mainViewer->Render();
    renderLargeImage->Update();

    return renderLargeImage->GetOutput();

}*/

//-----------------------------------------------------------------------------
// Angle Widget     
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class vtkAnnotationAngleWidgetCallback : public vtkCommand
{
public:
    static vtkAnnotationAngleWidgetCallback *New()
    { return new vtkAnnotationAngleWidgetCallback; }
         
    void SetCoordinates(vtkAngleWidget *angleWidget) 
    {
        /*
        if (!angleWidget || !this->AngleNode)
        {
            return;

        } 
        vtkAngleRepresentation3D *rep = vtkAngleRepresentation3D::SafeDownCast(angleWidget->GetRepresentation());
        if (!rep)
        {
            return;
        }
        double p1[3], p2[3], p3[3];
        rep->GetPoint1WorldPosition(p1);
        rep->GetPoint2WorldPosition(p2);
        rep->GetCenterWorldPosition(p3);
        // does the ruler node have a transform?
        vtkMRMLTransformNode* tnode = this->AngleNode->GetParentTransformNode();
        vtkMatrix4x4* transformToWorld = vtkMatrix4x4::New();
        transformToWorld->Identity();
        if (tnode != 0 && tnode->IsLinear())
        {
            vtkMRMLLinearTransformNode *lnode = vtkMRMLLinearTransformNode::SafeDownCast(tnode);
            lnode->GetMatrixTransformToWorld(transformToWorld);
        }
        // convert by the inverted parent transform
        double  xyzw[4];
        xyzw[0] = p1[0];
        xyzw[1] = p1[1];
        xyzw[2] = p1[2];
        xyzw[3] = 1.0;
        double worldxyz[4], *worldp = &worldxyz[0];
        transformToWorld->Invert();
        transformToWorld->MultiplyPoint(xyzw, worldp);
        this->AngleNode->SetPosition1(worldxyz[0], worldxyz[1], worldxyz[2]);

        vtkAngleRepresentation3D* angleRep = vtkAngleRepresentation3D::SafeDownCast(angleWidget->GetRepresentation());
        angleRep->SetPoint1WorldPosition(worldxyz);
        // second point
        xyzw[0] = p2[0];
        xyzw[1] = p2[1];
        xyzw[2] = p2[2];
        xyzw[3] = 1.0;
        transformToWorld->MultiplyPoint(xyzw, worldp);
        this->AngleNode->SetPosition2(worldxyz[0], worldxyz[1], worldxyz[2]);
        angleRep->SetPoint2WorldPosition(worldxyz);
        // center point
        xyzw[0] = p3[0];
        xyzw[1] = p3[1];
        xyzw[2] = p3[2];
        xyzw[3] = 1.0;
        transformToWorld->MultiplyPoint(xyzw, worldp);
        this->AngleNode->SetPositionCenter(worldxyz[0], worldxyz[1], worldxyz[2]);
        angleRep->SetCenterWorldPosition(worldxyz);
        transformToWorld->Delete();
        transformToWorld = 0;
        tnode = 0;
*/
    }
        
    virtual void Execute (vtkObject *caller, unsigned long event, void*)
    {
        // save node for undo if it's the start of an interaction event

        if (event == vtkCommand::PlacePointEvent)
        {
            count++;
            cout << "point placed\n";
            if ( count == 3 )
            {
                cout << "Angle widget placed\n";
                LogicPointer->AddAngleCompleted();
            }
        }

        if (event == vtkCommand::StartInteractionEvent)
        {
            if (this->AngleNode && this->AngleNode->GetScene())
            {
                this->AngleNode->GetScene()->SaveStateForUndo(this->AngleNode);
            }
        }
        else if (event == vtkCommand::InteractionEvent)
        {
          this->SetCoordinates(reinterpret_cast<vtkAngleWidget*>(caller));
        }                    
         
    }
    //,DistanceRepresentation(0)
    //vtkAnnotationAngleWidgetCallback():AngleNode(0),AngleWidgetClass(0) { count = 0; }
    vtkMRMLAnnotationAngleNode *AngleNode;
    vtkSlicerAnnotationModuleLogic* LogicPointer;
    //vtkMeasurementsAngleWidgetClass* AngleWidgetClass;
    int count;
    
    //  std::string RulerID;
    //  vtkLineRepresentation *DistanceRepresentation;
};

//-----------------------------------------------------------------------------
vtkMRMLAnnotationAngleNode* vtkSlicerAnnotationModuleLogic::GetAngleNodeByID(const char* id)
{/*
    vtkMRMLAnnotationAngleNode* anglenode = 0;
    anglenode = vtkMRMLAnnotationAngleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID( id ));
    return anglenode;
*/
return 0;
}


//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::RemoveAngle(const char* id)
{
/*
    vtkMRMLAnnotationAngleNode *angleNode = vtkMRMLAnnotationAngleNode::SafeDownCast(
        this->GetMRMLScene()->GetNodeByID( id ));

    if (angleNode)
    {
        this->GetMRMLScene()->RemoveNode(angleNode);
    }
    this->GetViewerWidget()->Render();
*/
}
/*
//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::AddAngleCompleted()
{
    this->InvokeEvent(vtkSlicerAnnotationModuleLogic::AddAngleCompletedEvent, 0);
}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::SetAnnotationSelectedByIDs(std::vector<const char*> selectedIDs, std::vector<const char*> allIDs)
{
    for (unsigned int i=0; i<allIDs.size(); ++i)
    {
        if ( allIDs[i] == 0 )
        {
            continue;
        }
        this->GetMRMLScene()->GetNodeByID( allIDs[i] )->SetSelected(0);
    }

    for (unsigned int i=0; i<selectedIDs.size(); ++i)
    {
        if ( selectedIDs[i] == 0 )
        {
            continue;
        }
        this->GetMRMLScene()->GetNodeByID( selectedIDs[i] )->SetSelected(1);
    }
    this->GetViewerWidget()->Render();
}

//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddAngle()
{
    if (m_AngleManager == 0)
    {
        this->m_AngleManager = vtkSlicerAnnotationAngleManager::New();
        this->m_AngleManager->SetMRMLScene( this->GetMRMLScene() );
        if (this->GetApplicationGUI()->GetActiveViewerWidget())
        {
            this->m_AngleManager->SetViewerWidget(this->GetApplicationGUI()->GetActiveViewerWidget());
        }
        this->m_AngleManager->AddMRMLObservers();
        this->m_AngleManager->SetParent ( this->GetApplicationGUI()->GetActiveViewerWidget()->GetParent() );
        this->m_AngleManager->Create();
    }

    vtkMRMLAnnotationAngleNode *angleNode = vtkMRMLAnnotationAngleNode::New();
    angleNode->Initialize(this->GetMRMLScene());

    if (angleNode->GetScene())
    {
        angleNode->SetName(angleNode->GetScene()->GetUniqueNameByString("AnnotationAngle"));
    }
    else
    {
        angleNode->SetName("AnnotationAngle");
    }

    angleNode->Delete();

    vtkAngleRepresentation3D* rep = vtkAngleRepresentation3D::New();
    this->m_AngleManager->GetAngleWidget( angleNode->GetID() )->GetWidget()->SetRepresentation( rep );
    this->m_AngleManager->GetAngleWidget( angleNode->GetID() )->GetWidget()->SetInteractor(this->GetViewerWidget()->GetMainViewer()->GetRenderWindowInteractor());
    this->m_AngleManager->GetAngleWidget( angleNode->GetID() )->GetWidget()->On();

    //vtkAngleRepresentation3D* rep = vtkAngleRepresentation3D::New();
    //this->GetAngleWidget( angleNode->GetID() )->GetWidget()->SetRepresentation( rep );
    //this->GetAngleWidget( angleNode->GetID() )->GetWidget()->SetInteractor(this->GetViewerWidget()->GetMainViewer()->GetRenderWindowInteractor());
    //this->GetAngleWidget( angleNode->GetID() )->GetWidget()->On();

    vtkAnnotationAngleWidgetCallback *myCallback = vtkAnnotationAngleWidgetCallback::New();
    myCallback->AngleNode = angleNode;
    myCallback->AngleWidgetClass = this->m_AngleManager->GetAngleWidget( angleNode->GetID() );
    myCallback->LogicPointer = this;
    this->m_AngleManager->GetAngleWidget(angleNode->GetID() )->GetWidget()->AddObserver(vtkCommand::InteractionEvent,myCallback);
    this->m_AngleManager->GetAngleWidget(angleNode->GetID() )->GetWidget()->AddObserver(vtkCommand::StartInteractionEvent, myCallback);
    this->m_AngleManager->GetAngleWidget(angleNode->GetID() )->GetWidget()->AddObserver(vtkCommand::PlacePointEvent, myCallback);
    myCallback->Delete();

    return angleNode->GetID();
}

//-----------------------------------------------------------------------------
// Ruler Widget     
//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddRuler()
{    
    if (m_RulerManager == 0)
    {
        this->m_RulerManager = vtkSlicerAnnotationRulerManager::New();
        this->m_RulerManager->SetMRMLScene( this->GetMRMLScene() );
        if (this->GetApplicationGUI()->GetActiveViewerWidget())
        {
            this->m_RulerManager->SetViewerWidget(this->GetApplicationGUI()->GetActiveViewerWidget());
        }
        this->m_RulerManager->AddMRMLObservers();
        this->m_RulerManager->SetParent ( this->GetApplicationGUI()->GetActiveViewerWidget()->GetParent() );
        this->m_RulerManager->Create();
    }
    
    vtkMRMLAnnotationRulerNode *rulerNode = vtkMRMLAnnotationRulerNode::New();
    rulerNode->Initialize(this->GetMRMLScene());

    // need a unique name since the storage node will be named from it
    if (rulerNode->GetScene())
    {
        rulerNode->SetName(rulerNode->GetScene()->GetUniqueNameByString("AnnotationRuler"));
    }
    else
    {
        rulerNode->SetName("AnnotationRuler");
    }
    rulerNode->Delete();

  this->GetViewerWidget()->Render();

    return rulerNode->GetID();
}
*/
//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::RemoveRuler(const char* id)
{/*
    vtkMRMLAnnotationRulerNode *rulerNode = vtkMRMLAnnotationRulerNode::SafeDownCast(
        this->GetMRMLScene()->GetNodeByID( id ));

    if (rulerNode)
    {
        this->GetMRMLScene()->RemoveNode(rulerNode);
    }
    this->GetViewerWidget()->Render();
*/}
/*
//-----------------------------------------------------------------------------
vtkMRMLAnnotationRulerNode* vtkSlicerAnnotationModuleLogic::GetRulerNodeByID(const char* id)
{
    vtkMRMLAnnotationRulerNode* rulernode = 0;
    rulernode = vtkMRMLAnnotationRulerNode::SafeDownCast(
        this->GetMRMLScene()->GetNodeByID( id ));

    return rulernode;
}

//-----------------------------------------------------------------------------
// Fiducial  Widget     
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class vtkAnnotationFiducialWidgetCallback : public vtkCommand
{
public:
  static vtkAnnotationFiducialWidgetCallback *New()
  { return new vtkAnnotationFiducialWidgetCallback; }

  virtual void Execute (vtkObject *caller, unsigned long event, void*)
  {
    if (event == vtkCommand::PlacePointEvent)
    {
      cout << "Fiducial point placed\n";
      LogicPointer->AddFiducialPicked();
    }
  }
  vtkAnnotationFiducialWidgetCallback(){}
  vtkSlicerAnnotationModuleLogic* LogicPointer;
};*/

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::StartAddingFiducials()
{/*
  vtkMRMLInteractionNode *interactionNode = this->GetApplicationLogic()->GetInteractionNode();
  if (interactionNode == 0)
    {
      std::cerr << "no interaction node in the scene, not updating the interaction mode!" << endl;
      return ;
    }

  interactionNode->SetLastInteractionMode ( interactionNode->GetCurrentInteractionMode() );
  interactionNode->SetCurrentInteractionMode ( vtkMRMLInteractionNode::Place );
*/}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::StopAddingFiducials()
{
  vtkMRMLInteractionNode *interactionNode = this->GetApplicationLogic()->GetInteractionNode();
  if (interactionNode == 0)
    {
      std::cerr << "no interaction node in the scene, not updating the interaction mode!" << endl;
      return ;
    }
  
  interactionNode->SetLastInteractionMode ( interactionNode->GetCurrentInteractionMode() );
  interactionNode->SetCurrentInteractionMode ( vtkMRMLInteractionNode::PickManipulate );
}
/*
//-----------------------------------------------------------------------------
vtkMRMLAnnotationFiducialNode* vtkSlicerAnnotationModuleLogic::GetFiducialNodeByID(const char* id)
{
    return vtkMRMLAnnotationFiducialNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID( id ));
}
*/
//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddFiducial()
{/*
  if (m_FiducialManager == 0)
  {
    this->m_FiducialManager = vtkMRMLAnnotationFiducialDisplayableManager::New();
    this->m_FiducialManager->SetMRMLScene( this->GetMRMLScene() );
    if (this->GetApplicationGUI()->GetActiveViewerWidget())
    {
      this->m_FiducialManager->SetViewerWidget(this->GetApplicationGUI()->GetActiveViewerWidget());
    }
    this->m_FiducialManager->AddMRMLObservers();
    this->m_FiducialManager->SetParent ( this->GetApplicationGUI()->GetActiveViewerWidget()->GetParent() );
    this->m_FiducialManager->Create();
  }*/
/*
  vtkMRMLAnnotationFiducialNode *fiducialNode = vtkMRMLAnnotationFiducialNode::New();
  fiducialNode->Initialize(this->GetMRMLScene());

  vtkMRMLFiducialListNode* fiducialNode = vtkMRMLFiducialListNode::New();
  this->GetMRMLScene()->AddNode(fiducialNode);
  fiducialNode->Delete();
  
  
  vtkAnnotationFiducialWidgetCallback *myCallback = vtkAnnotationFiducialWidgetCallback::New();
  myCallback->LogicPointer = this;

  this->m_FiducialManager->GetSeedWidget(fiducialNode->GetID() )->GetWidget()->AddObserver(vtkCommand::PlacePointEvent, myCallback);
  myCallback->Delete();


  //return fiducialNode->GetID();
 // return "";
*/ return 0;
}

//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddFiducialPicked()
{  
  /*  
  if (m_FiducialManager == 0)
  {
    this->m_FiducialManager = vtkMRMLAnnotationFiducialDisplayableManager::New();
    this->m_FiducialManager->SetMRMLScene( this->GetMRMLScene() );
    if (this->GetApplicationGUI()->GetActiveViewerWidget())
    {
      this->m_FiducialManager->SetViewerWidget(this->GetApplicationGUI()->GetActiveViewerWidget());
    }
    this->m_FiducialManager->AddMRMLObservers();
    this->m_FiducialManager->SetParent ( this->GetApplicationGUI()->GetActiveViewerWidget()->GetParent() );
    this->m_FiducialManager->Create();
  }

  vtkMRMLAnnotationFiducialNode *fiducialNode = vtkMRMLAnnotationFiducialNode::New();
  fiducialNode->Initialize(this->GetMRMLScene());

  // need a unique name since the storage node will be named from it
  if (fiducialNode->GetScene())
  {
    fiducialNode->SetName(fiducialNode->GetScene()->GetUniqueNameByString("AnnotationFiducial"));
  }
  else
  {
    fiducialNode->SetName("AnnotationFiducial");
  }
  fiducialNode->Delete();

  return fiducialNode->GetID();*/return 0;
}


//-----------------------------------------------------------------------------
double vtkSlicerAnnotationModuleLogic::GetFiducialValue(const char* cpID)
{

    if (!cpID)
    {
        std::cerr << "GetFiducialValue: no angle with ID " << cpID << " defined !" << endl;
        return 0;
    }
    //this->GetViewerWidget()->Render();

    return 0;

}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::RemoveFiducial(const char* id)
{
    vtkMRMLAnnotationFiducialNode *cpNode = vtkMRMLAnnotationFiducialNode::SafeDownCast(
        this->GetMRMLScene()->GetNodeByID( id ));

    this->RemoveFiducialWidget( cpNode);
    this->GetMRMLScene()->RemoveNode(cpNode);
    //this->GetViewerWidget()->Render();

}

//-----------------------------------------------------------------------------
std::vector<double> vtkSlicerAnnotationModuleLogic::GetFiducialPositionsByNodeID(const char* cpID)
{
    std::vector<double> pvector;

    if (!cpID)
    {
        std::cerr << "GetFiducial: no Fiducial with ID " << cpID << " defined !" << endl;
        return pvector;
    }

    // vtkSlicerFiducialListWidget *cpWidgetClass = this->GetFiducialWidget(cpID);
    // Kilian: This is not working right now - as it is only a place holder i removed it without debugging the code 
    // double *pos = cpWidgetClass->GetFiducialCoordinates();

    pvector.push_back(0);
    pvector.push_back(0);
    pvector.push_back(0);
    return pvector;

}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::SetFiducialPositionsByNodeID(const char* cpID, std::vector<double> positions)
{
    if (!cpID)
    {
        std::cerr << "GetFiducial: no fiducial with ID " << cpID << " defined !" << endl;
        return;
    }

    if ( positions.size() < 3)
    {
        return;
    }

    double pos[3];

    pos[0] = positions[0];
    pos[1] = positions[1];
    pos[2] = positions[2];
    //this->GetViewerWidget()->Render();
}

//---------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::Update3DFiducial(vtkMRMLAnnotationFiducialNode *activeCPNode)
{
    CTK_D(vtkSlicerAnnotationModuleLogic);

    if (activeCPNode == 0)
    {
        vtkDebugMacro("Update3DWidget: passed in fiducial node is 0, returning");
        return;
    }

    if (d->Updating3DFiducial)
    {
        vtkDebugMacro("Already updating 3d widget");
        return;
    }



    d->Updating3DFiducial = 1;

    if ( activeCPNode->GetVisible() )
    {

     //   if ( this->GetViewerWidget() && this->GetViewerWidget()->GetMainViewer())
     //   {
            // Initialize point
     //   }
    }
    else
    {
    }

    vtkMRMLTransformNode* tnode = activeCPNode->GetParentTransformNode();
    vtkMatrix4x4* transformToWorld = vtkMatrix4x4::New();
    transformToWorld->Identity();
    if (tnode != 0 && tnode->IsLinear())
    {
        vtkMRMLLinearTransformNode *lnode = vtkMRMLLinearTransformNode::SafeDownCast(tnode);
        lnode->GetMatrixTransformToWorld(transformToWorld);
    }


    tnode = 0;
    transformToWorld->Delete();
    transformToWorld = 0;

    /*if (this->GetViewerWidget())
    {
        this->GetViewerWidget()->RequestRender();
    }*/
    // reset the flag
    d->Updating3DFiducial= 0;
}

//---------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::AddFiducialWidget(vtkMRMLAnnotationFiducialNode *cpNode)
{
/*
    CTK_D(vtkSlicerAnnotationModuleLogic);
    if (!cpNode)
    {
        return;
    }
    if (this->GetFiducialWidget(cpNode->GetID()) != 0)
    {
        vtkDebugMacro("Already have widgets for ruler node " << cpNode->GetID());
        return;
    }

    // Just do this as a hack right now - should be the widget class 
    vtkSlicerFiducialListWidget  *c =  vtkSlicerFiducialListWidget::New();

    d->FiducialWidgets[cpNode->GetID()] = c;
    // watch for control point modified events
    if (cpNode->HasObserver(vtkMRMLAnnotationControlPointsNode::ControlPointModifiedEvent, (vtkCommand *)this->LogicCallbackCommand) != 1)
    {
        cpNode->AddObserver(vtkMRMLAnnotationControlPointsNode::ControlPointModifiedEvent, (vtkCommand *)this->LogicCallbackCommand);
    }
    // make sure we're observing the node for transform changes
    if (cpNode->HasObserver(vtkMRMLTransformableNode::TransformModifiedEvent, (vtkCommand *)this->LogicCallbackCommand) != 1)
    {
        cpNode->AddObserver(vtkMRMLTransformableNode::TransformModifiedEvent, (vtkCommand *)this->LogicCallbackCommand);
    }
    if (cpNode->HasObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->LogicCallbackCommand) != 1)
    {
        cpNode->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->LogicCallbackCommand);
    }
    */
}

//---------------------------------------------------------------------------
vtkSlicerFiducialListWidget* vtkSlicerAnnotationModuleLogic::GetFiducialWidget(const char *nodeID)
{
/*    CTK_D(vtkSlicerAnnotationModuleLogic);
    // This hsould be the widget not mrml class - just do it right now bc widget does not extist

    std::map<std::string, vtkSlicerFiducialListWidget *>::iterator iter;
    for (iter = d->FiducialWidgets.begin();
        iter != d->FiducialWidgets.end();
        iter++)
    {
        if (iter->first.c_str() && !strcmp(iter->first.c_str(), nodeID))
        {
            return iter->second;
        }
    }
    */
    return 0;
}

//---------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::RemoveFiducialWidget(vtkMRMLAnnotationFiducialNode *cpNode)
{/*
    CTK_D(vtkSlicerAnnotationModuleLogic);
    if (!cpNode)
    {
        return;
    }
    if (this->GetFiducialWidget(cpNode->GetID()) != 0)
    {
        d->FiducialWidgets[cpNode->GetID()]->Delete();
        d->FiducialWidgets.erase(cpNode->GetID());
    }*/
}

//-----------------------------------------------------------------------------
// MRML Related Set/Get Functions     
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::SetAnnotationLinesProperties(vtkMRMLAnnotationLinesNode* node, int type, void* data)
{
    if (!node) 
    {
        return;
    }
    this->SetAnnotationControlPointsProperties(node, type, data);

    // Line Display Properties
    if (node->GetAnnotationLineDisplayNode()==0)
    {
    }
    node->CreateAnnotationLineDisplayNode();
    this->SetAnnotationLineDisplayProperties(node->GetAnnotationLineDisplayNode(), type, data);
}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::SetAnnotationProperties(vtkMRMLAnnotationNode* node, int type, void* data)
{
    if (!node) 
    {
        return;
    }

    switch(type)
    {
    case TEXT:
        {
            node->SetText(0, (char*)data, 0, 1);
            break;
        }
    default:
        {
            if (node->GetAnnotationTextDisplayNode()==0)
            {
            }
            node->CreateAnnotationTextDisplayNode();
            this->SetAnnotationTextDisplayProperties( node->GetAnnotationTextDisplayNode(), type, data);
            break;
        }
    }

}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::SetAnnotationTextDisplayProperties(vtkMRMLAnnotationTextDisplayNode* node, int type, void* data)
{
    if ((type < TEXT_COLOR) || (type > TEXT_SCALE))
    {
        return 0;
    }

    if (!node) 
    {
        return 0;
    }
  
    switch(type)
    {
    case TEXT_SCALE:
        {
            node->SetTextScale( *(double*)data );
            return 1;
        }
    default:
        { 
            return this->SetAnnotationDisplayProperties(node, type, data);
        }
    }
}

//-----------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::SetAnnotationControlPointsProperties(vtkMRMLAnnotationControlPointsNode* node, int type, void* data)
{
    if (!node) 
    {
        return;
    }

    this->SetAnnotationProperties(node, type, data);
    if (node->GetAnnotationPointDisplayNode()==0)
    {
    }
    node->CreateAnnotationPointDisplayNode();
    this->SetAnnotationPointDisplayProperties(node->GetAnnotationPointDisplayNode(), type, data);
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::SetAnnotationPointDisplayProperties(vtkMRMLAnnotationPointDisplayNode* node, int type, void* data)
{
    if ((type < POINT_COLOR) || (type > POINT_SIZE))
    {
        return 0;
    }


    if (!node) 
    {
        return 0;
    }

    switch(type)
    {
    case POINT_SIZE:
        {
            //ToDo: set Point Size
            return 1;
        }
    default:
        return this->SetAnnotationDisplayProperties(node,type,data);  
    }
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::SetAnnotationLineDisplayProperties(vtkMRMLAnnotationLineDisplayNode* node, int type, void* data)
{
  if ((type < LINE_COLOR) || (type > LINE_WIDTH))
    {
      return 0;
    }


  if (!node) 
    {
      return 0;
    }

  switch(type)
    {
    case LINE_WIDTH:
      {
    node->SetLineThickness( *(double*)data );
    return 1;
      }
    default:
      return this->SetAnnotationDisplayProperties(node,type,data);
    }
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::SetAnnotationDisplayProperties(vtkMRMLAnnotationDisplayNode* node, int type, void* data)
{
    if (!node)
    {
        return 0;
    }

    switch(type)
    {
    case TEXT_OPACITY:
    case POINT_OPACITY:
    case LINE_OPACITY:
        node->SetOpacity( *(double*)data );
        return 1;
    case TEXT_AMBIENT:
    case POINT_AMBIENT:
    case LINE_AMBIENT:
        node->SetAmbient( *(double*)data );
        return 1;
    case TEXT_DIFFUSE:
    case POINT_DIFFUSE:
    case LINE_DIFFUSE:
        node->SetDiffuse( *(double*)data );
        return 1;
    case TEXT_SPECULAR:
    case POINT_SPECULAR:
    case LINE_SPECULAR:
        node->SetSpecular( *(double*)data );
        return 1;
    case TEXT_COLOR:
    case LINE_COLOR:
    case POINT_COLOR:
        {
            double* color = (double*)data;
            node->SetColor(color[0], color[1], color[2]);
            return 1;
        }
    case TEXT_SELECTED_COLOR:
    case POINT_SELECTED_COLOR:
    case LINE_SELECTED_COLOR:
        {
            double* color = (double*)data;
            node->SetSelectedColor(color[0], color[1], color[2]);
            return 1;
        }
    default:
        {
            std::cerr << "Property Type not found!" << endl;
            return 0;
        }
    }

}


//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::GetNumberOfControlPoints(vtkMRMLNode* mrmlnode)
{
  vtkMRMLAnnotationControlPointsNode* node = vtkMRMLAnnotationControlPointsNode::SafeDownCast(mrmlnode);
  if (!node) 
    {
      return -1;
    }
  return node->GetNumberOfControlPoints();
}

//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::GetIconName(vtkMRMLNode* node, bool isEdit)
{
    if (node->IsA("vtkMRMLAnnotationFiducialNode"))
    {
        if (isEdit)
        {
            return ":/Icons/AnnotationEditPoint.png";
        } 
        else
        {
            return ":/Icons/AnnotationPoint.png";
        }
    }
  else if (node->IsA("vtkMRMLFiducialListNode"))
  {
    if (isEdit)
    {
      return ":/Icons/AnnotationEditPoint.png";
    } 
    else
    {
      return ":/Icons/AnnotationPoint.png";
    }
  }
    else if (node->IsA("vtkMRMLAnnotationRulerNode"))
    {
        if (isEdit)
        {
            return ":/Icons/AnnotationEditDistance.png";
        } 
        else
        {
            return ":/Icons/AnnotationDistance.png";
        }
    }
    else if (node->IsA("vtkMRMLAnnotationAngleNode"))
    {
        if (isEdit)
        {
            return ":/Icons/AnnotationEditAngle.png";
        } 
        else
        {
            return ":/Icons/AnnotationAngle.png";
        }
    }
    else if (node->IsA("vtkMRMLAnnotationStickyNode"))
    {
        if (isEdit)
        {
            return ":/Icons/AnnotationEditNote.png";
        } 
        else
        {
            return ":/Icons/AnnotationNote.png";
        }
    }
  else if (node->IsA("vtkMRMLAnnotationTextNode"))
  {
    if (isEdit)
    {
      return ":/Icons/AnnotationEditText.png";
    } 
    else
    {
      return ":/Icons/AnnotationText.png";
    }
  }
  else if (node->IsA("vtkMRMLAnnotationROINode"))
  {
    if (isEdit)
    {
      return ":/Icons/AnnotationEditROI.png";
    } 
    else
    {
      return ":/Icons/AnnotationROI.png";
    }
  }
  else if (node->IsA("vtkMRMLAnnotationBidimensionalNode"))
  {
    if (isEdit)
    {
      return ":/Icons/AnnotationEditPolyline.png";
    } 
    else
    {
      return ":/Icons/AnnotationPolyline.png";
    }
  }
  else if (node->IsA("vtkMRMLAnnotationSplineNode"))
  {
    if (isEdit)
    {
      return ":/Icons/AnnotationEditSpline.png";
    } 
    else
    {
      return ":/Icons/AnnotationSpline.png";
    }
  }
  

  
    return 0;
} 

//-----------------------------------------------------------------------------
vtkStdString vtkSlicerAnnotationModuleLogic::GetAnnotationTextProperty(vtkMRMLNode* node)
{
    vtkMRMLAnnotationNode *aNode = vtkMRMLAnnotationNode::SafeDownCast(node);
    if (!aNode)
    {
        return "";
    } 
    return aNode->GetText(0);
}

const char* vtkSlicerAnnotationModuleLogic::GetAnnotationTextFormatProperty(vtkMRMLNode* node)
{
    if (node->IsA("vtkMRMLAnnotationFiducialNode"))
    {
    return " ";
    }
  if (node->IsA("vtkMRMLFiducialListNode"))
  {
    return " ";
  }
    else if (node->IsA("vtkMRMLAnnotationRulerNode"))
    {
        return vtkMRMLAnnotationRulerNode::SafeDownCast(node)->GetDistanceAnnotationFormat();
    }
    else if (node->IsA("vtkMRMLAnnotationAngleNode"))
    {
        return vtkMRMLAnnotationAngleNode::SafeDownCast(node)->GetLabelFormat();
    }
    else if (node->IsA("vtkMRMLAnnotationStickyNode"))
    {
        return " ";
    }
  else if (node->IsA("vtkMRMLAnnotationTextNode"))
  {
    return " ";
  }
  else if (node->IsA("vtkMRMLAnnotationROINode"))
  {
    return vtkMRMLAnnotationROINode::SafeDownCast(node)->GetROIAnnotationFormat();
  }
  else if (node->IsA("vtkMRMLAnnotationBidimensionalNode"))
  {
    return vtkMRMLAnnotationBidimensionalNode::SafeDownCast(node)->GetAnnotationFormat();
  }
  else if (node->IsA("vtkMRMLAnnotationSplineNode"))
  {
    return vtkMRMLAnnotationSplineNode::SafeDownCast(node)->GetDistanceAnnotationFormat();
  }

    return 0;
}

//-----------------------------------------------------------------------------
std::vector<double> vtkSlicerAnnotationModuleLogic::GetAnnotationMeasurement(vtkMRMLNode* node)
{
  std::vector<double> measurements;

  if (node->IsA("vtkMRMLAnnotationFiducialNode"))
  {
    measurements.push_back(0);
  }
  if (node->IsA("vtkMRMLFiducialListNode"))
  {
    measurements.push_back(0);
  }
  else if (node->IsA("vtkMRMLAnnotationRulerNode"))
  {
    measurements.push_back(vtkMRMLAnnotationRulerNode::SafeDownCast(node)->GetDistanceMeasurement());
  }
  else if (node->IsA("vtkMRMLAnnotationAngleNode"))
  {
    measurements.push_back(vtkMRMLAnnotationAngleNode::SafeDownCast(node)->GetAngleMeasurement());
  }
  else if (node->IsA("vtkMRMLAnnotationStickyNode"))
  {
    measurements.push_back(0);
  }
  else if (node->IsA("vtkMRMLAnnotationTextNode"))
  {
    measurements.push_back(0);
  }
  else if (node->IsA("vtkMRMLAnnotationROINode"))
  {
    measurements = vtkMRMLAnnotationROINode::SafeDownCast(node)->GetROIMeasurement();
  }
  else if (node->IsA("vtkMRMLAnnotationBidimensionalNode"))
  {
    measurements = vtkMRMLAnnotationBidimensionalNode::SafeDownCast(node)->GetBidimensionalMeasurement();
  }
  else if (node->IsA("vtkMRMLAnnotationSplineNode"))
  {
    measurements.push_back(vtkMRMLAnnotationSplineNode::SafeDownCast(node)->GetSplineMeasurement());
  }

    return measurements;
}

//-----------------------------------------------------------------------------
double* vtkSlicerAnnotationModuleLogic::GetAnnotationControlPointsCoordinate(vtkMRMLNode* mrmlnode, vtkIdType coordId)
{
    vtkMRMLAnnotationControlPointsNode* node = vtkMRMLAnnotationControlPointsNode::SafeDownCast(mrmlnode);

    if (!node || (coordId >= node->GetNumberOfControlPoints()))
    {
        return 0;
    }

    return node->GetControlPointCoordinates(coordId);
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::SetAnnotationControlPointsCoordinate(vtkMRMLNode* mrmlnode, double* pos, vtkIdType coordId)
{
    vtkMRMLAnnotationControlPointsNode* node = vtkMRMLAnnotationControlPointsNode::SafeDownCast(mrmlnode);

    if (!node || (coordId >= node->GetNumberOfControlPoints()))
    {
        return 0;
    }

    node->SetControlPoint(coordId, pos, 0, 1);

    return 1;
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::GetAnnotationLinesPropertiesDouble(vtkMRMLNode* node, int type, double &result)
{
    if (!node)
    {
        return 0;
    }
    vtkMRMLAnnotationLinesNode* lnode = vtkMRMLAnnotationLinesNode::SafeDownCast(node);
    if (!lnode)
    {
        return 0;
    }

     if (this->GetAnnotationControlPointsPropertiesDouble((vtkMRMLAnnotationControlPointsNode*)lnode, type, result) )
    {
        return 1;
    } 

    lnode->CreateAnnotationLineDisplayNode();
    if (this->GetAnnotationLineDisplayPropertiesDouble(lnode->GetAnnotationLineDisplayNode(), type, result) )
    {
        return 1;
    } 
    return 0;
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::GetAnnotationControlPointsPropertiesDouble(vtkMRMLAnnotationControlPointsNode* node, int type, double &result)
{
    if (!node)
    {
        return 0;
    }

    if ( this->GetAnnotationPropertiesDouble(node, type, result) )
    {
        return 1;
    }

    node->CreateAnnotationPointDisplayNode();
    if (this->GetAnnotationPointDisplayPropertiesDouble(node->GetAnnotationPointDisplayNode(), type, result) )
    {
        return 1;
    } 
    return 0;

}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::GetAnnotationLineDisplayPropertiesDouble(vtkMRMLAnnotationLineDisplayNode* node, int type, double& result)
{
    if ((type < LINE_COLOR) || (type > LINE_WIDTH))
    {
        return 0;
    }

    if (!node) 
    {
        return 0;
    }

    switch(type)
    {
    case LINE_WIDTH:
        {
            result = node->GetLineThickness();
            return 1;
        }
    default: 
        return this->GetAnnotationDisplayPropertiesDouble(node, type, result);
    }
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::GetAnnotationDisplayPropertiesDouble(vtkMRMLAnnotationDisplayNode* node, int type, double& result)
{
    if (!node)
    {
        return 0;
    }

    switch(type)
    {
    case POINT_OPACITY:
    case LINE_OPACITY:
    case TEXT_OPACITY:
        result = node->GetOpacity();
        return 1;
    case POINT_AMBIENT:
    case LINE_AMBIENT:
    case TEXT_AMBIENT:
        result = node->GetAmbient();
        return 1;
    case POINT_DIFFUSE:
    case LINE_DIFFUSE:
    case TEXT_DIFFUSE:
        result = node->GetDiffuse();
        return 1;
    case POINT_SPECULAR:
    case LINE_SPECULAR:
    case TEXT_SPECULAR:
        result = node->GetSpecular();
        return 1;
    default:
        {
            std::cerr << "Property Type not found!" << endl;
            return 0;
        }
    }
}


//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::GetAnnotationPropertiesDouble(vtkMRMLAnnotationNode* node, int type, double& result)
{
    if (!node) 
    {
        return 0;
    }

    node->CreateAnnotationTextDisplayNode();
    if ( this->GetAnnotationTextDisplayPropertiesDouble(node->GetAnnotationTextDisplayNode(), type, result) )
    {
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::GetAnnotationTextDisplayPropertiesDouble(vtkMRMLAnnotationTextDisplayNode* node, int type, double& result)
{
 if ((type < TEXT_COLOR) || (type > TEXT_SCALE))
    {
      return 0;
    }

 if (!node) 
   {
     return 0;
   }
 
 switch(type)
   {
   case TEXT_SCALE:
     result = node->GetTextScale();
     return 1;
   default:
     return this->GetAnnotationDisplayPropertiesDouble(node, type, result);    
   }
}

//-----------------------------------------------------------------------------
int vtkSlicerAnnotationModuleLogic::GetAnnotationPointDisplayPropertiesDouble(vtkMRMLAnnotationPointDisplayNode* node, int type, double& result)
{
    if ((type < POINT_COLOR) || (type > POINT_SIZE))
    {
        return 0;
    }

    if (!node) 
    {
        return 0;
    }

    switch(type)
    {
    case POINT_SIZE:
        {
            //ToDo: get Point Size
            return 1;
        }
    default:
        return this->GetAnnotationDisplayPropertiesDouble(node,type,result);
    }
}

//-----------------------------------------------------------------------------
double* vtkSlicerAnnotationModuleLogic::GetAnnotationLinesPropertiesColor(vtkMRMLNode* mrmlnode, int type)
{
    vtkMRMLAnnotationLinesNode* lnode = vtkMRMLAnnotationLinesNode::SafeDownCast(mrmlnode);
    if (!lnode) 
    {
        return 0;
    }

    switch(type)
    {
    case LINE_COLOR:
    case LINE_SELECTED_COLOR:
        {
            lnode->CreateAnnotationLineDisplayNode();
            return this->GetAnnotationLineDisplayPropertiesColor(lnode->GetAnnotationLineDisplayNode(), type);
        }
    default:
        {
            return this->GetAnnotationControlPointsPropertiesColor((vtkMRMLAnnotationControlPointsNode*)lnode, type);
        }
    }
}

//-----------------------------------------------------------------------------
double* vtkSlicerAnnotationModuleLogic::GetAnnotationLineDisplayPropertiesColor(vtkMRMLAnnotationLineDisplayNode* node, int type)
{
    if ((type != LINE_COLOR) && (type !=LINE_SELECTED_COLOR))
    {
        return 0;
    }

    if (!node) 
    {
        return 0;
    }
    return this->GetAnnotationDisplayPropertiesColor(node, type);
}

//-----------------------------------------------------------------------------
double* vtkSlicerAnnotationModuleLogic::GetAnnotationControlPointsPropertiesColor(vtkMRMLAnnotationControlPointsNode* node, int type)
{
    if (!node)
    {
        return 0;
    }

    switch(type)
    {
    case POINT_COLOR:
    case POINT_SELECTED_COLOR:
        {
            node->CreateAnnotationPointDisplayNode();
            return this->GetAnnotationPointDisplayPropertiesColor(node->GetAnnotationPointDisplayNode(), type);
        }
    default:
        {
          return this->GetAnnotationPropertiesColor(node, type);
        }
    }
}

//-----------------------------------------------------------------------------
double* vtkSlicerAnnotationModuleLogic::GetAnnotationPropertiesColor(vtkMRMLAnnotationNode* node, int type)
{
    if (!node) 
    {
        return 0;
    }
    node->CreateAnnotationTextDisplayNode();
    return this->GetAnnotationTextDisplayPropertiesColor(node->GetAnnotationTextDisplayNode(), type);
}

//-----------------------------------------------------------------------------
double* vtkSlicerAnnotationModuleLogic::GetAnnotationTextDisplayPropertiesColor(vtkMRMLAnnotationTextDisplayNode* node, int type)
{
    if ((type != TEXT_COLOR) && (type !=TEXT_SELECTED_COLOR))
    {
        return 0;
    }
    if (!node) 
    {
        return 0;
    }
    return this->GetAnnotationDisplayPropertiesColor(node, type);
}

//-----------------------------------------------------------------------------
double* vtkSlicerAnnotationModuleLogic::GetAnnotationPointDisplayPropertiesColor(vtkMRMLAnnotationPointDisplayNode* node, int type)
{
    if ((type != POINT_COLOR) && (type !=POINT_SELECTED_COLOR))
    {
        return 0;
    }
    if (!node) 
    {
        return 0;
    }
    return this->GetAnnotationDisplayPropertiesColor(node, type);
}

//-----------------------------------------------------------------------------
double* vtkSlicerAnnotationModuleLogic::GetAnnotationDisplayPropertiesColor(vtkMRMLAnnotationDisplayNode* node, int type)
{
    if (!node) 
    {
        return 0;
    }
    switch(type)
    {
    case TEXT_COLOR:
    case POINT_COLOR:
    case LINE_COLOR:
        {
            return node->GetColor();
        }
    case TEXT_SELECTED_COLOR:
    case POINT_SELECTED_COLOR:
    case LINE_SELECTED_COLOR:
        {
            return node->GetSelectedColor();
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Sticky Node
//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddStickyNode()
{    
    vtkMRMLAnnotationStickyNode *stickyNode = vtkMRMLAnnotationStickyNode::New();
    stickyNode->Initialize(this->GetMRMLScene());

    // need a unique name since the storage node will be named from it
    if (stickyNode->GetScene())
    {
        stickyNode->SetName(stickyNode->GetScene()->GetUniqueNameByString("AnnotationStickyNode"));
    }
    else
    {
        stickyNode->SetName("AnnotationRuler");
    }
    stickyNode->Delete();

    return stickyNode->GetID();
}

//-----------------------------------------------------------------------------
// Text Node
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddTextNode()
{   

  vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
  if (!selectionNode)
    {
    vtkErrorMacro("AddTextNode: No selection node in the scene.");
    return 0;
    }

  selectionNode->SetActiveAnnotationID("vtkMRMLAnnotationTextNode");

  this->StartPlaceMode();

  return 0;

}

//-----------------------------------------------------------------------------
// Angle Node
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddAngleNode()
{

  vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
  if (!selectionNode)
    {
    vtkErrorMacro("AddTextNode: No selection node in the scene.");
    return 0;
    }

  selectionNode->SetActiveAnnotationID("vtkMRMLAnnotationAngleNode");

  this->StartPlaceMode();

  return 0;

}

//---------------------------------------------------------------------------
// Start the place mouse mode
//---------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::StartPlaceMode()
{
  vtkIntArray *events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(),events);
  events->Delete();

  vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast(
      this->GetMRMLScene()->GetNthNodeByClass( 0, "vtkMRMLInteractionNode"));
  if (!interactionNode)
    {
    vtkErrorMacro ( "StartPlaceMode: No interaction node in the scene." );
    return;
    }

  interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);
  interactionNode->SetPlaceModePersistence(1);

  if (interactionNode->GetCurrentInteractionMode()!=vtkMRMLInteractionNode::Place) {

    vtkErrorMacro("StartPlaceMode: Could not set place mode!");
    return;

  }
}

//---------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::AddNodeCompleted(vtkMRMLAnnotationNode * node)
{

  std::cout << "AddNodeCompleted" << std::endl;

  if (!node)
    {
    return;
    }

  if (!this->m_Widget)
    {
    return;
    }

  this->m_Widget->addNodeToTable(node->GetID());
  this->m_LastAddedAnnotationNode = node;

}

//---------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::StopPlaceMode()
{

  vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast(
      this->GetMRMLScene()->GetNthNodeByClass( 0, "vtkMRMLInteractionNode"));
  if ( interactionNode == NULL )
    {
    vtkErrorMacro ( "StopPlaceMode: No interaction node in the scene." );
    return;
    }

  interactionNode->SetPlaceModePersistence(0);
  interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);

  if (interactionNode->GetCurrentInteractionMode()!=vtkMRMLInteractionNode::ViewTransform) {

    vtkErrorMacro("AddTextNode: Could not set transform mode!");

  }

}

//---------------------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::CancelCurrentOrRemoveLastAddedAnnotationNode()
{

  if (!this->m_LastAddedAnnotationNode)
    {
    return;
    }

  if (this->m_LastAddedAnnotationNode->IsA("vtkMRMLAnnotationTextNode")) {
    // for text annotations, just remove the last node
    this->GetMRMLScene()->RemoveNode(this->m_LastAddedAnnotationNode);
    this->m_LastAddedAnnotationNode = 0;
  }

}

//-----------------------------------------------------------------------------
vtkMRMLAnnotationTextNode* vtkSlicerAnnotationModuleLogic::GetTextNodeByID(const char* id)
{
  vtkMRMLAnnotationTextNode* textNode = 0;
  textNode = vtkMRMLAnnotationTextNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID( id ));
  return textNode;
}


//-----------------------------------------------------------------------------
// ROI Node
//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddROINode()
{   
  //vtkSlicerROIGUI* roi = vtkSlicerROIGUI::SafeDownCast(vtkSlicerApplication::GetInstance()->GetModuleGUIByName("ROI"));
  //vtkSlicerROIDisplayWidget* roiWidget = vtkSlicerROIDisplayWidget::SafeDownCast(roi->GetApplicationGUI()->GetActiveViewerWidget());
  //vtkMRMLROINode *roiNode = vtkMRMLROINode::New();
  /*
  if (m_ROIManager == 0)
  {
    this->m_ROIManager = vtkSlicerAnnotationROIManager::New();
    this->m_ROIManager->SetMRMLScene( this->GetMRMLScene() );
    if (this->GetApplicationGUI()->GetActiveViewerWidget())
    {
      this->m_ROIManager->SetViewerWidget(this->GetApplicationGUI()->GetActiveViewerWidget());
    }
    this->m_ROIManager->AddMRMLObservers();
    this->m_ROIManager->SetParent ( this->GetApplicationGUI()->GetActiveViewerWidget()->GetParent() );
    this->m_ROIManager->Create();
  }
  vtkMRMLAnnotationROINode *roiNode = vtkMRMLAnnotationROINode::New();
  roiNode->Initialize(this->GetMRMLScene());

  // need a unique name since the storage node will be named from it
  if (roiNode->GetScene())
  {
    roiNode->SetName(roiNode->GetScene()->GetUniqueNameByString("AnnotationROINode"));
  }
  else
  {
    roiNode->SetName("AnnotationROI");
  }
  //roiNode->SetLabelText("");
  roiNode->Delete();
  //roiWidget->SetROINode(roiNode);

  //vtkSlicerROIDisplayWidget* ROIWidget = vtkSlicerROIDisplayWidget::New();
  //ROIWidget->SetROINode(roiNode);
  //roiWidget->SetParent(this->GetApplicationGUI()->GetActiveViewerWidget()->GetParent());
  //roiWidget->Create();

  return roiNode->GetID();
}

//-----------------------------------------------------------------------------
// BiDimentional Line Node
//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddBidLineNode()
{

  vtkBiDimensionalRepresentation2D* bidRep = vtkBiDimensionalRepresentation2D::New();
  vtkBiDimensionalWidget* bidWidget = vtkBiDimensionalWidget::New();
  bidWidget->SetRepresentation(bidRep);
  bidWidget->SetInteractor(this->GetViewerWidget()->GetMainViewer()->GetRenderWindowInteractor());
  bidWidget->On();
  vtkMRMLOrthogonalLinePlotNode *node = vtkMRMLOrthogonalLinePlotNode::New();
  this->GetMRMLScene()->AddNode(node);
  

  if (m_BidimensionalManager == 0)
  {
    this->m_BidimensionalManager = vtkSlicerAnnotationBidimensionalManager::New();
    this->m_BidimensionalManager->SetMRMLScene( this->GetMRMLScene() );
    if (this->GetApplicationGUI()->GetActiveViewerWidget())
    {
      this->m_BidimensionalManager->SetViewerWidget(this->GetApplicationGUI()->GetActiveViewerWidget());
    }
    this->m_BidimensionalManager->AddMRMLObservers();
    this->m_BidimensionalManager->SetParent ( this->GetApplicationGUI()->GetActiveViewerWidget()->GetParent() );
    this->m_BidimensionalManager->Create();
  }

  vtkMRMLAnnotationBidimensionalNode *node = vtkMRMLAnnotationBidimensionalNode::New();
  node->Initialize(this->GetMRMLScene());

  // need a unique name since the storage node will be named from it
  if (node->GetScene())
  {
    node->SetName(node->GetScene()->GetUniqueNameByString("AnnotationPolylineNode"));
  }
  else
  {
    node->SetName("AnnotationROI");
  }
  node->Delete();

  return node->GetID();
  */
  return 0;

}

//-----------------------------------------------------------------------------
// Spline Node
//-----------------------------------------------------------------------------
const char* vtkSlicerAnnotationModuleLogic::AddSplineNode()
{   
  /*
  vtkSplineRepresentation* sRep = vtkSplineRepresentation::New();
  vtkSplineWidget* sWidget = vtkSplineWidget::New();
  sWidget->PlaceWidget();
  sWidget->ProjectToPlaneOn();
  sWidget->SetInteractor(this->GetViewerWidget()->GetMainViewer()->GetRenderWindowInteractor());
  sWidget->On();
  vtkMRMLBSplineTransformNode *node = vtkMRMLBSplineTransformNode::New();
  this->GetMRMLScene()->AddNode(node);
  */

  /*if (m_SplineManager == 0)
  {
    this->m_SplineManager = vtkSlicerAnnotationSplineManager::New();
    this->m_SplineManager->SetMRMLScene( this->GetMRMLScene() );
    if (this->GetApplicationGUI()->GetActiveViewerWidget())
    {
      this->m_SplineManager->SetViewerWidget(this->GetApplicationGUI()->GetActiveViewerWidget());
    }
    this->m_SplineManager->AddMRMLObservers();
    this->m_SplineManager->SetParent ( this->GetApplicationGUI()->GetActiveViewerWidget()->GetParent() );
    this->m_SplineManager->Create();
  }

  vtkMRMLAnnotationSplineNode *node = vtkMRMLAnnotationSplineNode::New();
  node->Initialize(this->GetMRMLScene());

  // need a unique name since the storage node will be named from it
  if (node->GetScene())
  {
    node->SetName(node->GetScene()->GetUniqueNameByString("AnnotationSpline"));
  }
  else
  {
    node->SetName("AnnotationSpline");
  }
  node->Delete();

  return node->GetID();
  */
  return 0;
}


//-----------------------------------------------------------------------------
// This is for testing right now 
//------------
//-----------------------------------------------------------------
void vtkSlicerAnnotationModuleLogic::AddRulerNodeObserver(vtkMRMLAnnotationRulerNode* rnode)
{
    if (rnode->HasObserver(vtkMRMLAnnotationControlPointsNode::ControlPointModifiedEvent, (vtkCommand *)this->LogicCallbackCommand) != 1)
    {
        rnode->AddObserver(vtkMRMLAnnotationControlPointsNode::ControlPointModifiedEvent, (vtkCommand *)this->LogicCallbackCommand);
    }
    if (rnode->GetAnnotationTextDisplayNode()==0)
    {
        rnode->CreateAnnotationTextDisplayNode();
    }
    if (rnode->GetAnnotationTextDisplayNode()->HasObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->LogicCallbackCommand) != 1)
    {
    rnode->GetAnnotationTextDisplayNode()->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->LogicCallbackCommand);
    }
}