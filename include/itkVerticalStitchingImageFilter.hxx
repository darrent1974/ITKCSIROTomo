/*=========================================================================
 *
 *  Copyright
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkVerticalStitchingImageFilter_hxx
#define itkVerticalStitchingImageFilter_hxx

#include "itkVerticalStitchingImageFilter.h"
#include "itkImageAlgorithm.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include "itkImageFileWriter.h"
#include <limits>
#include <strstream>
#include "itkImageScanlineConstIterator.h"
#include "itkExtractImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkBoxMeanImageFilter.h"
#include "itkBoxSigmaImageFilter.h"
#include "itkDivideImageFilter.h"

namespace itk
{
	template< typename TImage >
	VerticalStitchingImageFilter< TImage >::VerticalStitchingImageFilter()
    {
		m_VerticalShift.Fill( 0.0 );
        m_TrimPointMin.Fill( 0.0 );
        m_TrimPointMax.Fill( 0.0 );
    }

	template< typename TImage >
	void VerticalStitchingImageFilter< TImage >::PrintSelf( std::ostream& os, Indent indent ) const
    {
        Superclass::PrintSelf( os, indent );
    }


	template< typename TImage >
	typename TImage::Pointer VerticalStitchingImageFilter< TImage >::CreateRegionCopy( typename TImage::ConstPointer pImage, typename TImage::RegionType region )
    {
        // Create a new image with zero-based indexes containing
        // a copy of the contents of the passed image/region
        OutputRegionType regionCopy( region.GetSize() );
        typename TImage::Pointer pImageCopy( TImage::New() );
        pImageCopy->SetRegions( regionCopy );
        pImageCopy->SetSpacing( pImage->GetSpacing() );
        pImageCopy->Allocate();

        // Copy region
        ImageAlgorithm::Copy( pImage.GetPointer(), pImageCopy.GetPointer(), region, regionCopy );

        return pImageCopy;
    }

	template< typename TImage >
	typename TImage::RegionType VerticalStitchingImageFilter< TImage >::ComputeTrimRegion( typename TImage::ConstPointer pImage )
    {
        // Special case, no trim points set (zero)
        if( m_TrimPointMax.EuclideanDistanceTo( m_TrimPointMin ) == 0.0 )
            return pImage->GetLargestPossibleRegion();

        OutputRegionType regionTrim( pImage->GetLargestPossibleRegion() );
        OutputIndexType indexBoundMin( regionTrim.GetIndex() );
        OutputIndexType indexBoundMax;

        /*if( !*/pImage->TransformPhysicalPointToIndex( m_TrimPointMin, indexBoundMin );// )
        //    itkExceptionMacro ("ComputeTrimRegion min index transform failed");

        regionTrim.SetIndex( indexBoundMin );

        /*if( !*/pImage->TransformPhysicalPointToIndex( m_TrimPointMax, indexBoundMax );// )
        //    itkExceptionMacro ("ComputeTrimRegion max index transform failed");

        for( unsigned int j = 0; j < ImageDimension; j++ )
            regionTrim.SetSize( j, indexBoundMax[j] - indexBoundMin[j] );

        return regionTrim;
    }

	template< typename TImage >
	void VerticalStitchingImageFilter< TImage >::GenerateData()
    {
        // Get a reference to the current output image
        typename TImage::Pointer pOutput( this->GetOutput() );
		typename TImage::ConstPointer pInputImage( this->GetInput() );

        if( !pInputImage || !pOutput )
          return;

        if( this->GetNumberOfInputs() == 1 )
        {
            this->GraftOutput( CreateRegionCopy( pInputImage, ComputeTrimRegion( pInputImage ) ) );
            return;
        }

        OutputRegionType regionOutput( pOutput->GetLargestPossibleRegion() );

		//this->GraftOutput( pSpatialObjectToBlendedImage->GetOutput() );
    }

	template< typename TImage >
	void VerticalStitchingImageFilter< TImage >::GenerateInputRequestedRegion()
    {
        // Get pointer to the input
		//typename TWeightingVectorImage::Pointer pInput(  const_cast< TWeightingVectorImage * >( this->GetInput() ) );
		//const InputRegionType & regionLargestInput( pInput->GetLargestPossibleRegion() );
		//pInput->SetRequestedRegion( regionLargestInput );
    }

	template< typename TImage >
	void VerticalStitchingImageFilter< TImage >::GenerateOutputInformation()
    {
        Superclass::GenerateOutputInformation();

		typename TImage::ConstPointer pInputImage( this->GetInput() );
        typename TImage::Pointer pOutput( this->GetOutput() );

        if( !pInputImage || !pOutput )
          return;

        int intNumInputs( this->GetNumberOfInputs() );

        if( intNumInputs == 1 )
        {
            // Set the output size to the trimmed input size
            pOutput->SetLargestPossibleRegion( ComputeTrimRegion( pInputImage ) );
            return;
        }

#ifdef TEMP_REMOVED

        // Initialise output region
        InputRegionType regionOutput( pInputImage->GetRequestedRegion() );

        // Initialize output size to be updated
        InputSizeType sizeOutput( regionOutput.GetSize() );

        // Set the spacing on the spatial object group based on the spacing of
        // the first image (assuming spacing is consistant over all images)
        typename GroupSpatialObjectType::VectorType vectorSpacing( pInputImage->GetSpacing() );
        m_GroupImageSpatialObjects->SetSpacing( &vectorSpacing[0] );

        typename GroupSpatialObjectType::VectorType vectorTrimTop;
        typename GroupSpatialObjectType::VectorType vectorTrimBottom;

        if( m_TrimPointMax.EuclideanDistanceTo( m_TrimPointMin ) == 0.0 )
        {
            vectorTrimTop.Fill( 0.0 );
            vectorTrimBottom.Fill( 0.0 );
        }
        else
            for( unsigned int j = 0; j < ImageDimension; j++ )
            {
                vectorTrimTop[j] = m_TrimPointMin[j];
                vectorTrimBottom[j] = ( static_cast<double>( sizeOutput[j] ) * vectorSpacing[j] ) - m_TrimPointMax[j];
            }

        typename CheckedImageSpatialObjectType::VectorType vectorShift;
        vectorShift.Fill( 0 );

        typedef ImageFileWriter< TImage > ImageFileWriterType;
        typename ImageFileWriterType::Pointer pImageFileWriter( ImageFileWriterType::New() );

        // Iterate through each input
        for( int intInputIdx = 0; intInputIdx < intNumInputs; intInputIdx++ )
        {
            std::ostringstream ss;

            typename TImage::ConstPointer pInputImageN( this->GetScalarInput( intInputIdx, 0 ) );
            typename TImage::ConstPointer pInputWeightingImageN( this->GetScalarInput( intInputIdx, 1 ) );

            InputRegionType regionTrim( ComputeTrimRegion( pInputImageN ) );

            // Create new image spatial objects for the image and weighting image
            typename CheckedImageSpatialObjectType::Pointer pImageSpatialObject( CheckedImageSpatialObjectType::New() );
            pImageSpatialObject->SetImage( CreateRegionCopy( pInputImageN, regionTrim ) );

            pImageFileWriter->SetInput( pImageSpatialObject->GetImage() );

#ifdef TEMP_REMOVED
            ss << "trimmed_" << intInputIdx << ".mhd";
            pImageFileWriter->SetFileName( ss.str() );
            pImageFileWriter->Update();

            std::string strSuffix( std::to_string( intInputIdx ) );
            createMeanSigmaSNRImages( pImageSpatialObject->GetImage(), 3, strSuffix );

            //computeMaxLineSNR( pImageSpatialObject->GetImage() );
#endif

            typename CheckedImageSpatialObjectType::Pointer pWeightingImageSpatialObject( CheckedImageSpatialObjectType::New() );
            pWeightingImageSpatialObject->SetImage( CreateRegionCopy( pInputWeightingImageN, regionTrim ) );

            // Shift the image relative to the group
            pImageSpatialObject->GetObjectToParentTransform()->SetOffset( vectorShift );
            pImageSpatialObject->ComputeObjectToWorldTransform();
            pWeightingImageSpatialObject->GetObjectToParentTransform()->SetOffset( vectorShift );
            pWeightingImageSpatialObject->ComputeObjectToWorldTransform();

            // Add it to the group
            m_GroupImageSpatialObjects->AddSpatialObject( pImageSpatialObject );
            m_GroupWeightImageSpatialObjects->AddSpatialObject( pWeightingImageSpatialObject );

            // Increment the shift for the next image
            vectorShift += m_Shift;
        }

        // Compute the bounding-box of all image objects
        m_GroupImageSpatialObjects->ComputeBoundingBox();
        m_GroupWeightImageSpatialObjects->ComputeBoundingBox();

        typename GroupSpatialObjectType::BoundingBoxType::Pointer pBoundingBox( m_GroupImageSpatialObjects->GetBoundingBox() );
        typename GroupSpatialObjectType::BoundingBoxType::BoundsArrayType bounds( pBoundingBox->GetBounds() );

        std::cout << "bounds: " << pBoundingBox->GetBounds() << std::endl;

        // Compute the size of the "stitched" image based on the bounding box of all spatial objects
        for( unsigned int j = 0, k = 0; j < ImageDimension; j++, k += 2 )
            sizeOutput[j] = static_cast<unsigned int>( ceil( ( bounds[k+1] - bounds[k] ) / vectorSpacing[j] ) );

        // Update the size for the output region
        regionOutput.SetSize( sizeOutput );

        // Set the output largest possible region to the total size of the stitched image
        pOutput->SetLargestPossibleRegion( regionOutput );
    }
#endif
}

#endif // itkVerticalStitchingImageFilter_hxx
