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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ThresholdExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdExample::ThresholdExample() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdExample::~ThresholdExample()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
#if 0
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Voxel Cell Arrays to Threshold");
    parameter->setPropertyName("CellComparisonInputs");
    parameter->setWidgetType(FilterParameterWidgetType::CellArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Voxel Feature Arrays to Threshold");
    parameter->setPropertyName("FeatureComparisonInputs");
    parameter->setWidgetType(FilterParameterWidgetType::FeatureArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Voxel Ensemble Arrays to Threshold");
    parameter->setPropertyName("EnsembleComparisonInputs");
    parameter->setWidgetType(FilterParameterWidgetType::EnsembleArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Point Arrays to Threshold");
    parameter->setPropertyName("PointComparisonInputs");
    parameter->setWidgetType(FilterParameterWidgetType::VertexArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Face Arrays to Threshold");
    parameter->setPropertyName("FaceComparisonInputs");
    parameter->setWidgetType(FilterParameterWidgetType::FaceArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Edge Arrays to Threshold");
    parameter->setPropertyName("EdgeComparisonInputs");
    parameter->setWidgetType(FilterParameterWidgetType::EdgeArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
#endif
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

  reader->openFilterGroup(this, index);
  setCellComparisonInputs(reader->readComparisonInputs("CellComparisonInputs", getCellComparisonInputs()));
  setFeatureComparisonInputs(reader->readComparisonInputs("FeatureComparisonInputs", getFeatureComparisonInputs()));
  setEnsembleComparisonInputs(reader->readComparisonInputs("EnsembleComparisonInputs", getEnsembleComparisonInputs()));
  setPointComparisonInputs(reader->readComparisonInputs("PointComparisonInputs", getPointComparisonInputs()));
  setFaceComparisonInputs(reader->readComparisonInputs("FaceComparisonInputs", getFaceComparisonInputs()));
  setEdgeComparisonInputs(reader->readComparisonInputs("EdgeComparisonInputs", getEdgeComparisonInputs()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ThresholdExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */

  /* --- CellArrayComparisonSelectionWidget --- */
  writer->writeValue( "CellComparisonInputs", getCellComparisonInputs() );

  /* --- FeatureArrayComparisonSelectionWidget --- */
  writer->writeValue( "FeatureComparisonInputs", getFeatureComparisonInputs() );

  /* --- EnsembleArrayComparisonSelectionWidget --- */
  writer->writeValue("EnsembleComparisonInputs", getEnsembleComparisonInputs() );

  /* --- PointArrayComparisonSelectionWidget --- */
  writer->writeValue("PointComparisonInputs", getPointComparisonInputs() );

  /* --- FaceArrayComparisonSelectionWidget --- */
  writer->writeValue("FaceComparisonInputs", getFaceComparisonInputs() );

  /* --- EdgeArrayComparisonSelectionWidget --- */
  writer->writeValue("EdgeComparisonInputs", getEdgeComparisonInputs() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/
  /*
  if (m_OutputFile.isEmpty() == true)
  {
    ss << "The output file must be set before executing this filter.";
    setErrorCondition(-1);
    notifyErrorMessage(getNameOfClass(), ss.str(), -1);
  }
  */


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::execute()
{
  dataCheck();
  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ThresholdExample::newFilterInstance(bool copyFilterParameters)
{
  /*
  * CellComparisonInputs
  * FeatureComparisonInputs
  * EnsembleComparisonInputs
  * PointComparisonInputs
  * FaceComparisonInputs
  * EdgeComparisonInputs
  */
  ThresholdExample::Pointer filter = ThresholdExample::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
