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
#ifndef itkStitchingImageFilter_hxx
#define itkStitchingImageFilter_hxx

#include "itkStitchingImageFilter.h"
#include "itkImageAlgorithm.h"

namespace itk
{
    template< typename TImage >
    StitchingImageFilter< TImage >::StitchingImageFilter()
    {
        m_Shift.Fill( 0.0 );
        m_TrimPointMin.Fill( 0.0 );
        m_TrimPointMax.Fill( 0.0 );

        // Create group to store image spatial objects
        m_GroupImageSpatialObjects = GroupSpatialObjectType::New();
    }

    template< typename TImage >
    void StitchingImageFilter< TImage >::PrintSelf( std::ostream& os, Indent indent ) const
    {
        Superclass::PrintSelf( os, indent );
    }

    template< typename TImage >
    typename TImage::Pointer StitchingImageFilter< TImage >::CreateRegionCopy( typename TImage::ConstPointer pImage, RegionType region )
    {
        // Create a new image with zero-based indexes containing
        // a copy of the contents of the passed image/region
        RegionType regionCopy( region.GetSize() );
        typename TImage::Pointer pImageCopy( TImage::New() );
        pImageCopy->SetRegions( regionCopy );
        pImageCopy->SetSpacing( pImage->GetSpacing() );
        pImageCopy->Allocate();

        // Copy region
        ImageAlgorithm::Copy( pImage.GetPointer(), pImageCopy.GetPointer(), region, regionCopy );

        return pImageCopy;
    }

    template< typename TImage >
    typename TImage::RegionType StitchingImageFilter< TImage >::ComputeTrimRegion( typename TImage::ConstPointer pImage )
    {
        // Special case, no trim points set (zero)
        if( m_TrimPointMax.EuclideanDistanceTo( m_TrimPointMin ) == 0.0 )
            return pImage->GetLargestPossibleRegion();

        RegionType regionTrim( pImage->GetLargestPossibleRegion() );
        IndexType indexBoundMin( regionTrim.GetIndex() );
        IndexType indexBoundMax;

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
    void StitchingImageFilter< TImage >::GenerateData()
    {
        // Get a reference to the current output image
        typename TImage::Pointer pOutput( this->GetOutput() );
        typename TImage::ConstPointer pInput( this->GetInput() );

        if( !pInput || !pOutput )
          return;

        if( this->GetNumberOfInputs() == 1 )
        {
            this->GraftOutput( CreateRegionCopy( pInput, ComputeTrimRegion( pInput ) ) );
            return;
        }

        RegionType regionOutput( pOutput->GetLargestPossibleRegion() );

        typename SpatialObjectToBlendedImageFilterType::Pointer pSpatialObjectToBlendedImage( SpatialObjectToBlendedImageFilterType::New() );

        pSpatialObjectToBlendedImage->SetInput( m_GroupImageSpatialObjects );
        pSpatialObjectToBlendedImage->SetSize( regionOutput.GetSize() );
        pSpatialObjectToBlendedImage->SetSpacing( pInput->GetSpacing() );
        pSpatialObjectToBlendedImage->Update();

        this->GraftOutput( pSpatialObjectToBlendedImage->GetOutput() );
    }

    template< typename TImage >
    void StitchingImageFilter< TImage >::GenerateInputRequestedRegion()
    {
        // Get pointer to the input
        typename TImage::Pointer pInput(  const_cast< TImage * >( this->GetInput() ) );
        const RegionType & regionLargestInput( pInput->GetLargestPossibleRegion() );
        pInput->SetRequestedRegion( regionLargestInput );
    }

    template< typename TImage >
    void StitchingImageFilter< TImage >::GenerateOutputInformation()
    {
        Superclass::GenerateOutputInformation();

        typename TImage::ConstPointer pInput( this->GetInput() );
        typename TImage::Pointer pOutput( this->GetOutput() );

        if( !pInput || !pOutput )
          return;

        int intNumInputs( this->GetNumberOfInputs() );

        if( intNumInputs == 1 )
        {
            // Set the output size to the trimmed input size
            pOutput->SetLargestPossibleRegion( ComputeTrimRegion( pInput ) );
            return;
        }

        // Initialise output region
        RegionType regionOutput( pInput->GetRequestedRegion() );

        // Initialize output size to be updated
        SizeType sizeOutput( regionOutput.GetSize() );

        // Set the spacing on the spatial object group based on the spacing of
        // the first image (assuming spacing is consistant over all images)
        typename GroupSpatialObjectType::VectorType vectorSpacing( pInput->GetSpacing() );
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

        typename ImageSpatialObjectType::VectorType vectorShift;
        vectorShift.Fill( 0 );

        // Iterate through each input
        for( int intInputIdx = 0; intInputIdx < intNumInputs; intInputIdx++ )
        {
            typename TImage::ConstPointer pInputN( this->GetInput( intInputIdx ) );

            // Create new spatial object
            typename ImageSpatialObjectType::Pointer pImageSpatialObject( ImageSpatialObjectType::New() );

            pImageSpatialObject->SetImage( CreateRegionCopy( pInputN, ComputeTrimRegion( pInputN ) ) );

            // Shift the image relative to the group
            pImageSpatialObject->GetObjectToParentTransform()->SetOffset( vectorShift );
            pImageSpatialObject->ComputeObjectToWorldTransform();

            // Add it to the group
            m_GroupImageSpatialObjects->AddSpatialObject( pImageSpatialObject );


            // Increment the shift for the next image
            vectorShift += m_Shift;
        }

        // Compute the bounding-box of all image objects
        m_GroupImageSpatialObjects->ComputeBoundingBox();

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
}

#endif // itkStitchingImageFilter_hxx
