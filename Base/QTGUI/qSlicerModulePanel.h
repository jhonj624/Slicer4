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

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerModulePanel_h
#define __qSlicerModulePanel_h

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerAbstractModulePanel.h"

#include "qSlicerBaseQTGUIExport.h"

class qSlicerAbstractModule;
class qSlicerModulePanelPrivate;

class Q_SLICER_BASE_QTGUI_EXPORT qSlicerModulePanel: public qSlicerAbstractModulePanel
{
  Q_OBJECT
public:
  qSlicerModulePanel(QWidget* parent = 0, Qt::WindowFlags f = 0);
  virtual ~qSlicerModulePanel();

  void clear();

public slots:
  void setModule(const QString& moduleName);

protected slots:
  virtual void addModule(const QString& moduleName);
  virtual void removeModule(const QString& moduleName);
  virtual void removeAllModule();

protected:
  QScopedPointer<qSlicerModulePanelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerModulePanel);
  Q_DISABLE_COPY(qSlicerModulePanel);
};

#endif
