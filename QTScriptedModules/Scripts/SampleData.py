import os
from __main__ import slicer
import qt, ctk

#
# SampleData
#

class SampleData:
  def __init__(self, parent):
    parent.title = "SampleData"
    parent.category = "Informatics"
    parent.contributor = "Steve Pieper"
    parent.helpText = """
The SampleData module can be used to download data for working with in slicer.  Use of this module requires an active network connection.  
    """
    parent.acknowledgementText = """
This work is supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See <a>http://www.slicer.org</a> for details.  Module implemented by Steve Pieper.
    """
    self.parent = parent

    if slicer.mrmlScene.GetTagByClassName( "vtkMRMLScriptedModuleNode" ) != 'ScriptedModule':
      slicer.mrmlScene.RegisterNodeClass(vtkMRMLScriptedModuleNode())

    # trigger the menu to be added when application has started up
    qt.QTimer.singleShot(0, self.addMenu);
    

  def addMenu(self):
    i = qt.QIcon(':Icons/XLarge/SlicerDownloadMRHead.png')
    a = qt.QAction(i, 'Download Sample Data', slicer.util.mainWindow())
    a.setToolTip('Go to the SampleData module to download data from the network')
    a.connect('triggered()', self.select)

    menuFile = slicer.util.lookupTopLevelWidget('menuFile')
    if menuFile:
      for action in menuFile.actions():
        if action.text == 'Add Data':
          menuFile.insertAction(action,a)


  def select(self):
    m = slicer.util.mainWindow()
    m.moduleSelector().selectModuleByTitle('SampleData')


#
# SampleData widget
#

class SampleDataWidget:

  def __init__(self, parent=None):
    self.observerTags = []

    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
      self.layout = self.parent.layout()
      self.setup()
      self.parent.show()
    else:
      self.parent = parent
      self.layout = parent.layout()

  def enter(self):
    pass
    
  def exit(self):
    pass

  def updateGUIFromMRML(self, caller, event):
    pass

  # sets up the widget
  def setup(self):
    samples = (
        ( 'MRHead', self.downloadMRHead ),
        ( 'CTChest', self.downloadCTChest ),
        ( 'CTACardio', self.downloadCTACardio ),
        ( 'DTIBrain', self.downloadDTIBrain ),
        ( 'MRBrainTumor1', self.downloadMRBrainTumor1 ),
        ( 'MRBrainTumor2', self.downloadMRBrainTumor2 ),
      )
    for sample in samples:
      b = qt.QPushButton('Download %s' % sample[0] )
      self.layout.addWidget(b)
      b.connect('clicked()', sample[1])

    # Add spacer to layout
    self.layout.addStretch(1)

  def downloadMRHead(self):
    self.downloadVolume('http://www.slicer.org/slicerWiki/images/4/43/MR-head.nrrd', 'MRHead')

  def downloadCTChest(self):
    self.downloadVolume('http://www.slicer.org/slicerWiki/images/3/31/CT-chest.nrrd', 'CTChest')

  def downloadCTACardio(self):
    self.downloadVolume('http://www.slicer.org/slicerWiki/images/0/00/CTA-cardio.nrrd', 'CTACardio')

  def downloadDTIBrain(self):
    self.downloadVolume('http://www.slicer.org/slicerWiki/images/0/01/DTI-Brain.nrrd', 'DTIBrain')

  def downloadMRBrainTumor1(self):
    self.downloadVolume('http://www.slicer.org/slicerWiki/images/5/59/RegLib_C01_1.nrrd', 'MRBrainTumor1')

  def downloadMRBrainTumor2(self):
    self.downloadVolume('http://www.slicer.org/slicerWiki/images/5/59/RegLib_C01_2.nrrd', 'MRBrainTumor1')

  def downloadVolume(self, uri, name):
    #TODO: this should probably just call AddArchetypeScalarVolume directly and then 
    #set the composite nodes using the PropagateVolumeSelection api.
    # start the download
    vl = slicer.modules.volumes.logic()
    volumeNode = vl.AddArchetypeVolume(uri, name, 0)
    # automatically select the volume to display
    if volumeNode:
      mrmlLogic = slicer.app.mrmlApplicationLogic()
      selNode = mrmlLogic.GetSelectionNode()
      selNode.SetReferenceActiveVolumeID(volumeNode.GetID())
      mrmlLogic.PropagateVolumeSelection(1)