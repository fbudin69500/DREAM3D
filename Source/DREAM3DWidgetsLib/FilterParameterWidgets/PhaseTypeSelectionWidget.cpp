/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "PhaseTypeSelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtGui/QComboBox>
#include <QtGui/QLabel>

#include "DREAM3DLib/Common/PhaseType.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::~PhaseTypeSelectionWidget()
{}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::setupGui()
{
  if(getFilter() == NULL)
  {
    return;
  }
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  if (getFilterParameter() == NULL)
  {
    return;
  }
  QString units = getFilterParameter()->getUnits();
  if(units.isEmpty() == false)
  {
    label->setText(getFilterParameter()->getHumanLabel() + " (" + units + ")");
  }
  else
  {
    label->setText(getFilterParameter()->getHumanLabel() );
  }

  updateComboBoxes();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::updateComboBoxes()
{
  bool ok = false;
  // setup the list of choices for the widget
  PhaseTypesFilterParameter* shapeType = dynamic_cast<PhaseTypesFilterParameter*>(getFilterParameter());
  QString countProp = shapeType->getPhaseTypeCountProperty();
  int size = getFilter()->property(countProp.toLatin1().constData()).toInt(&ok);

  UInt32Vector_t vectorWrapper = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<UInt32Vector_t>();
  QVector<quint32> dataFromFilter = vectorWrapper.d;

  // Get our list of predefined Phase Type Strings
  QVector<QString> phaseTypestrings;
  PhaseType::getPhaseTypeStrings(phaseTypestrings);
  // Get our list of predefined enumeration values
  QVector<unsigned int> shapeTypeEnums;
  PhaseType::getPhaseTypeEnums(shapeTypeEnums);

  // Remove all the items from the GUI and from the internal tracking Lists
  QLayoutItem* child;
  while ((formLayout_2->count() > 0) && (child = formLayout_2->takeAt(0)) != 0)
  {
    delete child;
  }
  m_PhaseTypeLabels.clear();
  m_PhaseTypeCombos.clear();

  // Create a whole new QWidget to hold everything
  m_PhaseTypeScrollContents = new QWidget();
  m_PhaseTypeScrollContents->setObjectName(QString::fromUtf8("m_PhaseTypeScrollContents"));
  formLayout_2 = new QFormLayout(m_PhaseTypeScrollContents);
  formLayout_2->setContentsMargins(4, 4, 4, 4);
  formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
  formLayout_2->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
  formLayout_2->setHorizontalSpacing(6);
  formLayout_2->setVerticalSpacing(6);
  m_PhaseTypeScrollArea->setWidget(m_PhaseTypeScrollContents);

  // We skip the first Ensemble as it is always a dummy
  for (int i = 0; i < size; i++)
  //for (int i = 1; i < size; i++)
  {
    QLabel* shapeTypeLabel = new QLabel(m_PhaseTypeScrollContents);
    QString str("Phase ");
    str.append(QString::number(i, 10));
    str.append(":");
    shapeTypeLabel->setText(str);
    shapeTypeLabel->setObjectName(str);
    m_PhaseTypeLabels << shapeTypeLabel;

    formLayout_2->setWidget(i, QFormLayout::LabelRole, shapeTypeLabel);

    QComboBox* cb = new QComboBox(m_PhaseTypeScrollContents);
    str.append(" ComboBox");
    cb->setObjectName(str);
    for (qint32 s = 0; s < phaseTypestrings.size(); ++s)
    {
      cb->addItem((phaseTypestrings[s]), shapeTypeEnums[s]);
      cb->setItemData(static_cast<int>(s), shapeTypeEnums[s], Qt::UserRole);
    }
    m_PhaseTypeCombos << cb;
    formLayout_2->setWidget(i, QFormLayout::FieldRole, cb);
    if (i < dataFromFilter.size())
    {
      cb->setCurrentIndex(dataFromFilter[i]);
    }
    connect(cb, SIGNAL(currentIndexChanged(int)),
            this, SLOT(comboboxChanged(int)) );

  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::comboboxChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::beforePreflight()
{
    std::cout << "PhaseTypeSelectionWidget::beforePreflight()" << std::endl;
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    //  std::cout << "***  PhaseTypeSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }
      updateComboBoxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::afterPreflight()
{
   std::cout << "PhaseTypeSelectionWidget::afterPreflight()" << std::endl;

   //updateComboBoxes();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  int count = m_PhaseTypeCombos.count();
 // if(count == 0)
  {
    std::cout << "PhaseTypeSelectionWidget::filterNeedsInputParameters Count = " << count << std::endl;
  }
  QVector<uint32_t> phaseTypes(count, DREAM3D::PhaseType::UnknownPhaseType);
 // QVector<uint32_t> phaseTypes(count+1, DREAM3D::PhaseType::UnknownPhaseType);
  bool ok = false;
 // phaseTypes[0] = DREAM3D::PhaseType::UnknownPhaseType;
  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = m_PhaseTypeCombos.at(i);
    unsigned int sType = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    //phaseTypes[i+1] = sType;
    phaseTypes[i] = sType;
  }

  UInt32Vector_t data;
  data.d = phaseTypes;

  QVariant var;
  var.setValue(data);
  ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}