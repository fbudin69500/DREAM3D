/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "VtkGrainIdWriter.h"

#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkGrainIdWriter::VtkGrainIdWriter() :
  FileWriter(),
  m_WriteBinaryFiles(true),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkGrainIdWriter::~VtkGrainIdWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkGrainIdWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameterWidgetType::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void VtkGrainIdWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->closeFilterGroup();
  return index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkGrainIdWriter::dataCheck()
{
  setErrorCondition(0);

  VoxelDataContainer* m = getVoxelDataContainer();


  m_GrainIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_GrainIdsArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GrainIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GrainIds = m_GrainIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkGrainIdWriter::preflight()
void VtkGrainIdWriter::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdWriter::writeHeader()
{
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdWriter::writeFile()
{

  // Copy all the variables into the helper class from above
  if (NULL == getVoxelDataContainer())
  {

    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "(" << __LINE__ << ")";
    setErrorMessage(ss.str());
    setErrorCondition(-1);
    return -1;
  }
  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {

    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "(" << __LINE__ << ")";
    setErrorMessage(ss.str());
    setErrorCondition(-1);
    return -1;
  }

  int64_t totalPoints = m->getTotalPoints();
  dataCheck();
  if (getErrorCondition() < 0)
  {
    return -40;
  }

  VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<VoxelDataContainer>(m));
  QVector<VtkScalarWriter*> scalarsToWrite;
  w0->m_WriteBinaryFiles = m_WriteBinaryFiles;
  scalarsToWrite.push_back(w0);
  VTKRectilinearGridFileWriter writer;
  writer.setWriteBinaryFiles(m_WriteBinaryFiles);
  int err = writer.write<VoxelDataContainer>(getOutputFile(), m, scalarsToWrite);
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage(getHumanLabel(), "Error Writing GrainId Vtk RectilinearGrid File", getErrorCondition());
  }
  return err;
}



