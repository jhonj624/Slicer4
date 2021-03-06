/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerCorePythonManager_h
#define __qSlicerCorePythonManager_h

// CTK includes
# include <ctkAbstractPythonManager.h>

#include "qSlicerBaseQTCoreExport.h"

class PythonQtObjectPtr;
class vtkObject;

class Q_SLICER_BASE_QTCORE_EXPORT qSlicerCorePythonManager : public ctkAbstractPythonManager
{
  Q_OBJECT

public:
  typedef ctkAbstractPythonManager Superclass;
  qSlicerCorePythonManager(QObject* parent=0);
  ~qSlicerCorePythonManager();

  /// Convenient function allowing to add a VTK object to the interpreter main module
  void addVTKObjectToPythonMain(const QString& name, vtkObject * object);
  
protected:

  virtual QStringList pythonPaths();
  virtual void preInitialization();

};

#endif
