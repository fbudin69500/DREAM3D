/*
 * Your License or Copyright Information can go here
 */

#include "ITKFilter.h"

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkGaussianBlurImageFunction.h"
#include "itkImageRegionIterator.h"

 //define commonly used pixel types
typedef uint8_t ITK_CharPixelType;//8 bit images (dream3d ImageData)
typedef float ITK_FloatPixelType;//32 bit images (normal itk output)

//define dimensionality
const unsigned int ITK_ImageDimension = 3;

//define commonly used image types
typedef itk::Image< ITK_CharPixelType, ITK_ImageDimension > ITK_CharImageType;
typedef itk::Image< ITK_FloatPixelType, ITK_ImageDimension > ITK_FloatImageType;

//define iterator
typedef itk::ImageRegionConstIterator< ITK_CharImageType > ConstIteratorType;

//define import filter
typedef itk::ImportImageFilter<ITK_FloatPixelType, ITK_ImageDimension> ImportFilterType;
typedef itk::ImportImageFilter<ITK_CharPixelType, ITK_ImageDimension> ImportCharFilterType;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKFilter::ITKFilter() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_OverwriteArray(false),
m_RawImageData(NULL),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKFilter::~ITKFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKFilter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Array to Process");
    parameter->setPropertyName("SelectedCellArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Overwrite Array");
    parameter->setPropertyName("OverwriteArray");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Created Array Name");
    parameter->setPropertyName("NewCellArrayName");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    options.push_back(parameter);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setOverwriteArray( reader->readValue( "OverwriteArray", getOverwriteArray() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ITKFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("OverwriteArray", getOverwriteArray() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, float, FloatArrayType, 0, voxels, 1)

  if(m_SelectedCellArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    ss << "An array from the Voxel Data Container must be selected.";
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
  }
  else
  {
    m_RawImageDataArrayName=m_SelectedCellArrayName;
    GET_PREREQ_DATA(m, DREAM3D, CellData, RawImageData, ss, -300, uint8_t, UInt8ArrayType, voxels, 1)

    if(m_OverwriteArray)
    {
      //bool check = m->renameCellData(m_ProcessedImageDataArrayName, m_RawImageDataArrayName);
    }
    else
    {
      bool check = m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);
      if(check == false)
      {
        ss << "Array to be processed could not be found in DataContainer";
        addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKFilter::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", getErrorCondition());
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  /* Place all your code to execute your filter here. */

  //get size of dataset
  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
   DimType dims[3] = { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  ImportCharFilterType::Pointer importFilter = ImportCharFilterType::New();

  //set size of itkImage
  ImportCharFilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  //fill with 'black'
  ImportCharFilterType::IndexType start;
  start.Fill( 0 );

  //setup import filter
  ImportCharFilterType::RegionType region;
  region.SetIndex( start );
  region.SetSize(  size  );
  importFilter->SetRegion( region );

  //define origin
  double origin[ ITK_ImageDimension ];///this may not be zero if cropped w/o updating origin
  origin[0] = 0.0;    // X coordinate
  origin[1] = 0.0;    // Y coordinate
  origin[2] = 0.0;    // Z coordinate
  importFilter->SetOrigin( origin );

  //define spacing
  double spacing[ ITK_ImageDimension ];///this should probably also grab the actual spacing
  spacing[0] = 1.0;    // along X direction
  spacing[1] = 1.0;    // along Y direction
  spacing[2] = 1.0;    // along Z direction
  importFilter->SetSpacing( spacing );

  //import data
  const bool importImageFilterWillOwnTheBuffer = false;
  importFilter->SetImportPointer( m_RawImageData, totalPoints, importImageFilterWillOwnTheBuffer );
  importFilter->Update();

  //get image iterator
  const ITK_CharImageType * inputImage = importFilter->GetOutput();
  ITK_CharImageType::RegionType filterRegion = inputImage->GetBufferedRegion();
  ConstIteratorType it(inputImage, filterRegion);

  //create guassian blur filter
  typedef itk::GaussianBlurImageFunction< ITK_CharImageType > GFunctionType;
  GFunctionType::Pointer gaussianFunction = GFunctionType::New();
  gaussianFunction->SetInputImage( inputImage );

  //set guassian blur parameters
  GFunctionType::ErrorArrayType setError;
  setError.Fill( 0.01 );
  gaussianFunction->SetMaximumError( setError );
  gaussianFunction->SetSigma( 4 );
  gaussianFunction->SetMaximumKernelWidth( 5 );

  //loop over image running filter
  notifyStatusMessage("Blurring");
  it.GoToBegin();
  int index=0;
   while( !it.IsAtEnd() )
  {
    m_ProcessedImageData[index]=(gaussianFunction->EvaluateAtIndex(it.GetIndex() ));
    ++it;
    ++index;
  }

  //array name changing/cleanup
  if(m_OverwriteArray)
  {
    m->removeCellData(m_SelectedCellArrayName);
    bool check = m->renameCellData(m_ProcessedImageDataArrayName, m_SelectedCellArrayName);
  }
  else
  {
    bool check = m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);
  }

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
