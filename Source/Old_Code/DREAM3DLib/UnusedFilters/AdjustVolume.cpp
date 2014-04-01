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

#include "AdjustVolume.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataContainers/DataContainerMacros.h"
#include "DREAM3DLib/SyntheticBuildingFilters/PackPrimaryPhases.h"


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdjustVolume::AdjustVolume() :
  AbstractFilter(),
  m_MaxIterations(1),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_GrainIds(NULL),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_EquivalentDiameters(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdjustVolume::~AdjustVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void AdjustVolume::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Max Iterations");
    option->setPropertyName("MaxIterations");
    option->setWidgetType(FilterParameterWidgetType::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void AdjustVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AdjustVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MaxIterations", getMaxIterations() );
  writer->closeFilterGroup();
  return index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolume::dataCheck()
{
  setErrorCondition(0);

  VoxelDataContainer* m = getVoxelDataContainer();

  m_GrainIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_GrainIdsArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GrainIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GrainIds = m_GrainIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_EquivalentDiametersPtr = attrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_FeatureAttributeMatrixName,  m_EquivalentDiametersArrayName, 0, features, 1); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolume::preflight()
void AdjustVolume::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolume::execute()
{
  setErrorCondition(0);
  DREAM3D_RANDOMNG_NEW()
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The DataContainer Object was NULL", getErrorCondition());
    return;
  }
  int64_t totalPoints = m->getTotalPoints();
  int totalFeatures = m->getNumFeatureTuples();

  // Check to make sure we have all of our data arrays available or make them available.
  dataCheck();
  if (getErrorCondition() < 0)
  {
    return;
  }

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];
  int iterations = 0;
  int selectedgrain = 0;
  int good = 0;
  int growth = 1;
  int nucleus;
  int bad = 0;
  float random, oldsizedisterror = 0.0f, currentsizedisterror = 0.0f, diam;
  DimType x, y, z;
  int neighpoint, index;
  DimType count, affectedcount;
  int vListSize = 1000;

  float voxtovol = m->getXRes() * m->getYRes() * m->getZRes() * (0.75f) * (1.0f / m_pi);

  gsizes.resize(m->getNumFeatureTuples());

  QVector<int> voxellist(vListSize, -1);
  QVector<int> affectedvoxellist(vListSize, -1);
  for(size_t i = 1; i < m->getNumFeatureTuples(); i++)
  {
    gsizes[i] = 0;
  }
  NEW_SHARED_ARRAY(reassigned, int, totalPoints)

  for(int i = 0; i < totalPoints; i++)
  {
    reassigned[i] = 0;
    gsizes[m_GrainIds[i]]++;
  }
  PackPrimaryPhases::Pointer packGrains = PackPrimaryPhases::New();
  packGrains->setVoxelDataContainer(getVoxelDataContainer());
  connect(packGrains, SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
          this, SLOT(emitFilterGeneratedMessage(const PipelineMessage&)));
//  Feature feature;
//  oldsizedisterror = packGrains->check_sizedisterror(&feature);
  while(iterations < m_MaxIterations)
  {

    ss << "Adjusting Grain Boundaries - " << ((float)iterations / m_MaxIterations) * 100 << "Percent Complete";
    notifyStatusMessage(getHumanLabel(), ss.str());
    iterations++;
    good = 0;
    while (good == 0)
    {
      good = 1;
      selectedgrain = int(rg.genrand_res53() * m->getNumFeatureTuples());
      if (selectedgrain >= static_cast<int>(m->getNumFeatureTuples())) { selectedgrain = m->getNumFeatureTuples() - 1;}
      if (selectedgrain == 0) { selectedgrain = 1; }
    }
    growth = 1;
    random = static_cast<float>( rg.genrand_res53() );
    if(random < 0.5) { growth = -1; }
    nucleus = 0;
    count = 0;
    affectedcount = 0;
    while(m_GrainIds[nucleus] != selectedgrain)
    {
      nucleus++;
      if(nucleus >= totalPoints) { selectedgrain++, nucleus = 0; }
    }
    voxellist[count] = nucleus;
    count++;
    for(DimType i = 0; i < count; ++i)
    {
      index = voxellist[i];
      x = index % dims[0];
      y = (index / dims[0]) % dims[1];
      z = index / (dims[0] * dims[1]);
      for(DimType j = 0; j < 6; j++)
      {
        good = 1;
        neighpoint = static_cast<int>( index + neighpoints[j] );
        if(j == 0 && z == 0) { good = 0; }
        if(j == 5 && z == (dims[2] - 1)) { good = 0; }
        if(j == 1 && y == 0) { good = 0; }
        if(j == 4 && y == (dims[1] - 1)) { good = 0; }
        if(j == 2 && x == 0) { good = 0; }
        if(j == 3 && x == (dims[0] - 1)) { good = 0; }
        if(good == 1 && m_GrainIds[neighpoint] == selectedgrain && reassigned[neighpoint] == 0)
        {
          voxellist[count] = neighpoint;
          reassigned[neighpoint] = -1;
          count++;
          if(count >= static_cast<DimType>(voxellist.size())) { voxellist.resize(voxellist.size() + vListSize, -1); }
        }
        if(good == 1 && m_GrainIds[neighpoint] != selectedgrain && m_GrainIds[index] == selectedgrain)
        {
          if(growth == 1 && reassigned[neighpoint] <= 0)
          {
            reassigned[neighpoint] = m_GrainIds[neighpoint];
            m_GrainIds[neighpoint] = m_GrainIds[index];
            affectedvoxellist[affectedcount] = neighpoint;
            affectedcount++;
            if(affectedcount >= static_cast<DimType>(affectedvoxellist.size())) { affectedvoxellist.resize(affectedvoxellist.size() + vListSize, -1);}
          }
          if(growth == -1 && reassigned[neighpoint] <= 0)
          {
            reassigned[index] = m_GrainIds[index];
            m_GrainIds[index] = m_GrainIds[neighpoint];
            affectedvoxellist[affectedcount] = index;
            affectedcount++;
            if(affectedcount >= static_cast<DimType>(affectedvoxellist.size())) { affectedvoxellist.resize(affectedvoxellist.size() + vListSize, -1);}
          }
        }
      }
    }
    for(DimType i = 0; i < affectedcount; i++)
    {
      index = affectedvoxellist[i];
      if(reassigned[index] > 0)
      {
        gsizes[m_GrainIds[index]]++;
        gsizes[reassigned[index]] = gsizes[reassigned[index]] - 1;
      }
    }
    for(size_t i = 1; i < m->getNumFeatureTuples(); i++)
    {
      index = i;
      diam = 2.0f * powf((gsizes[index] * voxtovol), (1.0f / 3.0f));
      m_EquivalentDiameters[index] = diam;
    }
//    currentsizedisterror = packGrains->check_sizedisterror(&feature);
    if(currentsizedisterror <= oldsizedisterror)
    {
      oldsizedisterror = currentsizedisterror;
      for(size_t i = 1; i < m->getNumFeatureTuples(); i++)
      {
//        if(gsizes[i] == 0) m->m_Grains.erase(m->m_Grains.begin() + i);
      }
    }
    if(currentsizedisterror > oldsizedisterror)
    {
      bad++;
      for(DimType i = 0; i < affectedcount; i++)
      {
        index = affectedvoxellist[i];
        if(reassigned[index] > 0)
        {
          gsizes[m_GrainIds[index]] = gsizes[m_GrainIds[index]] - 1;
          m_GrainIds[index] = reassigned[index];
          gsizes[m_GrainIds[index]]++;
        }
      }
      for(size_t i = 1; i < m->getNumFeatureTuples(); i++)
      {
        index = i;
        diam = 2.0f * powf((gsizes[index] * voxtovol), (1.0f / 3.0f));
        m_EquivalentDiameters[index] = diam;
      }
    }
    for(int i = 0; i < totalPoints; i++)
    {
      reassigned[i] = 0;
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Adjusting Grain Boundaries Complete");
}
