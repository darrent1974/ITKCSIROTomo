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
#include "itkMeanProjectionImageFilter.h"
#include "itkComposeImageFilter.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageScanlineConstIterator.h"
#include "itkExtractImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{
    template< typename TImage, typename TWeighting >
    VerticalStitchingImageFilter< TImage, TWeighting >::VerticalStitchingImageFilter()
        : m_ComputeWeighting( true )
        , m_Rescale( true )
        , m_VerticalShift( 0.0 )
        , m_WeightingAlpha( NULL )
        , m_WeightingBeta( NULL )
    {
        m_TrimPointMin.Fill( 0.0 );
        m_TrimPointMax.Fill( 0.0 );
    }

    template< typename TImage, typename TWeighting >
    void VerticalStitchingImageFilter< TImage, TWeighting >::PrintSelf( std::ostream& os, Indent indent ) const
    {
        Superclass::PrintSelf( os, indent );
    }


    template< typename TImage, typename TWeighting >
    typename TImage::Pointer VerticalStitchingImageFilter< TImage, TWeighting >::CreateRegionCopy( typename TImage::ConstPointer pImage, typename TImage::RegionType region )
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

    template< typename TImage, typename TWeighting >
    typename TImage::RegionType VerticalStitchingImageFilter< TImage, TWeighting >::ComputeTrimRegion( typename TImage::ConstPointer pImage )
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

    template< typename TImage, typename TWeighting >
    void VerticalStitchingImageFilter< TImage, TWeighting >::CreateWeightingVectorImages( std::vector<typename TImage::Pointer> & vecImages )
    {
        typedef itk::ExtractImageFilter< TImage, TImage > ExtractImageFilterType;
        typedef itk::MeanProjectionImageFilter< TImage, TImage > MeanProjectionImageFilterType;

        if( vecImages.size() == 1 )
          return;

        unsigned int uintNumOverlap( vecImages.size() - 1 );

        // Set initial weighting value to 1.0
        typename WeightingImageType::PixelType valInitial( uintNumOverlap );
        valInitial.Fill( 1.0 );

        // Create VectorImages for Alpha & Beta weightings
        WeightingImageTypePointer pWeightingAlpha( WeightingImageType::New() );
        pWeightingAlpha->SetRegions( m_RegionWeighting );
        pWeightingAlpha->SetVectorLength( uintNumOverlap );
        pWeightingAlpha->Allocate();
        pWeightingAlpha->FillBuffer( valInitial );

        WeightingImageTypePointer pWeightingBeta( WeightingImageType::New() );
        pWeightingBeta->SetRegions( m_RegionWeighting );
        pWeightingBeta->SetVectorLength( uintNumOverlap );
        pWeightingBeta->Allocate();
        pWeightingBeta->FillBuffer( valInitial );

        // Create a vector column-wise mean images
        std::vector< typename TImage::Pointer > vecColumnWiseMeans;
        for( typename std::vector< typename TImage::Pointer >::const_iterator itVec = vecImages.begin(); itVec != vecImages.end(); itVec++ )
        {
            // Extract non-overlap region
            typename ExtractImageFilterType::Pointer pExtractNonOverlapImageFilter( ExtractImageFilterType::New() );
            pExtractNonOverlapImageFilter->SetInput( *itVec );
            pExtractNonOverlapImageFilter->SetExtractionRegion( m_RegionNonOverlap );

            // Compute mean projection along vertical axis
            typename MeanProjectionImageFilterType::Pointer pMeanProjectionImageFilter( MeanProjectionImageFilterType::New() );
            pMeanProjectionImageFilter->SetInput( pExtractNonOverlapImageFilter->GetOutput() );
            pMeanProjectionImageFilter->SetProjectionDimension( 1 );
            pMeanProjectionImageFilter->Update();

            vecColumnWiseMeans.push_back( pMeanProjectionImageFilter->GetOutput() );

            pMeanProjectionImageFilter->GetOutput()->DisconnectPipeline();
        }

        RegionType regionColumnUpper( m_RegionOverlapUpper );
        regionColumnUpper.SetSize( 0, 1 );

        RegionType regionColumnLower( m_RegionOverlapLower );
        regionColumnLower.SetSize( 0, 1 );

        RegionType regionColumnAlphaBeta( m_RegionWeighting );
        regionColumnAlphaBeta.SetSize( 0, 1 );

        for( unsigned int i = 0; i < uintNumOverlap; i++ )
        {
            itk::ImageScanlineConstIterator< TImage > itMeanColumn( vecColumnWiseMeans[i], vecColumnWiseMeans[i]->GetLargestPossibleRegion() );

            while( !itMeanColumn.IsAtEnd() )
            {
                IndexType indexMean( itMeanColumn.GetIndex() );

                double dblN(  0.5 * ( itMeanColumn.Get() + vecColumnWiseMeans[i+1]->GetPixel( indexMean ) ) );

                if( dblN > 0.0 )
                {
                    regionColumnUpper.SetIndex( 0, indexMean[0] );
                    regionColumnLower.SetIndex( 0, indexMean[0] );
                    regionColumnAlphaBeta.SetIndex( 0, indexMean[0] );

                    // Iterate over the overlapped region computing alpha and beta weights
                    itk::ImageLinearConstIteratorWithIndex< TImage > itColumnInputN( vecImages[i], regionColumnLower );
                    itColumnInputN.SetDirection( 1 );

                    itk::ImageLinearConstIteratorWithIndex< TImage > itColumnInputN1( vecImages[i + 1], regionColumnUpper );
                    itColumnInputN1.SetDirection( 1 );

                    itk::ImageLinearIteratorWithIndex< WeightingImageType > itAlpha( pWeightingAlpha, regionColumnAlphaBeta );
                    itAlpha.SetDirection( 1 );

                    itk::ImageLinearIteratorWithIndex< WeightingImageType > itBeta( pWeightingBeta, regionColumnAlphaBeta );
                    itBeta.SetDirection( 1 );

                    while( !itColumnInputN.IsAtEndOfLine() )
                    {
                        IndexType indexColumnN( itColumnInputN.GetIndex() );
                        IndexType indexColumnN1( itColumnInputN1.GetIndex() );

                        //std::cout << "Index n: " << indexColumnN[0] << " " << indexColumnN[1] << ", Index n1: " << indexColumnN1[0] << " " << indexColumnN1[1] << std::endl;

                        double dblN1( itColumnInputN.Get() );
                        double dblN2( itColumnInputN1.Get() );

                        if( dblN1 > 0 && dblN2 > 0 && dblN1 + dblN2 > dblN )
                        {
                            double dblN1N( std::max( dblN1, dblN2 ) / dblN );
                            double dblN2N( std::min( dblN1, dblN2 ) / dblN );

                            double dblB( dblN2N == 1.0 ? ( 1.0 - dblN1N ) / 2.0 : ( 1.0 - sqrt( ( dblN1N + dblN2N - 1.0 ) / ( dblN1N * dblN2N ) ) ) / ( 1.0 - dblN2N ) * dblN1N );
                            double dblAlpha( 1.0 / ( dblN1N + dblB * dblN2N ) );
                            double dblBeta( dblB * dblAlpha );

                            typename itk::ImageLinearIteratorWithIndex< WeightingImageType >::PixelType valAlpha( itAlpha.Get() );
                            typename itk::ImageLinearIteratorWithIndex< WeightingImageType >::PixelType valBeta( itBeta.Get() );

                            if( dblN1 > dblN2 )
                            {
                                valAlpha[i] = dblAlpha;
                                valBeta[i] = dblBeta;
                            }
                            else
                            {
                                valAlpha[i] = dblBeta;
                                valBeta[i] = dblAlpha;
                            }

                            itAlpha.Set( valAlpha );
                            itBeta.Set( valBeta );
                        }

                        ++itColumnInputN;
                        ++itColumnInputN1;
                        ++itAlpha;
                        ++itBeta;
                    }
                }

                ++itMeanColumn;
            }
        }

        this->SetWeightingAlpha( pWeightingAlpha );
        this->SetWeightingBeta( pWeightingBeta );
    }


    template< typename TImage, typename TWeighting >
    void VerticalStitchingImageFilter< TImage, TWeighting >::GenerateData()
    {
        // Get a reference to the current output image
        typename TImage::Pointer pOutput( this->GetOutput() );
        typename TImage::ConstPointer pInputImage( this->GetInput() );

        if( !pInputImage || !pOutput )
          return;

        RegionType regionTrimmed( ComputeTrimRegion( pInputImage ) );

        if( this->GetNumberOfInputs() == 1 )
        {
            this->GraftOutput( CreateRegionCopy( pInputImage, regionTrimmed ) );
            return;
        }

        RegionType regionOutput( pOutput->GetLargestPossibleRegion() );

        typename TImage::Pointer pImageOutput( TImage::New() );
        pImageOutput->SetRegions( regionOutput );
        pImageOutput->SetSpacing( pInputImage->GetSpacing() );
        pImageOutput->Allocate();
        pImageOutput->FillBuffer( 0 );

        // Create a vector of trimmed input images to be used in subsequent operations
        std::vector<typename TImage::Pointer> vecRescaledImages;
        for( unsigned int i = 0; i < this->GetNumberOfInputs(); i++ )
            vecRescaledImages.push_back( CreateRegionCopy( this->GetInput( i ), regionTrimmed ) );

        if( this->GetComputeWeighting() )
            CreateWeightingVectorImages( vecRescaledImages );

        itk::ImageRegionIterator< WeightingImageType > itAlpha( this->GetWeightingAlpha(), m_RegionWeighting );
        itk::ImageRegionIterator< WeightingImageType > itBeta( this->GetWeightingBeta(), m_RegionWeighting );

        // Apply scaling to trimmed images prior to stitching
        for( unsigned int i = 0; i < this->GetNumberOfInputs() - 1; i++ )
        {
          itAlpha.GoToBegin();
          itBeta.GoToBegin();

           itk::ImageRegionIterator< TImage > itOverlapLowerN( vecRescaledImages[i], m_RegionOverlapLower );
           itk::ImageRegionIterator< TImage > itOverlapUpperN1( vecRescaledImages[i+1], m_RegionOverlapUpper );

           IndexType indexIterator;

           while( !itOverlapLowerN.IsAtEnd() )
           {
               indexIterator = itAlpha.GetIndex();

               typename itk::ImageRegionIterator< WeightingImageType >::PixelType valAlpha( itAlpha.Get() );
               typename itk::ImageRegionIterator< WeightingImageType >::PixelType valBeta( itBeta.Get() );

              // double dblN( itOverlapLowerN.Value() );
               //double dblN1( itOverlapUpperN1.Value() );

               //double dblAlpha( valAlpha[i] );
               //double dblBeta( valBeta[i] );

               // Apply alpha scaling to lower overlap on N image
               itOverlapLowerN.Value() *= valAlpha[i];

                // Apply beta scaling to upper overlap on N+1 image
               itOverlapUpperN1.Value() *= valBeta[i];

               //double dblNpost( itOverlapLowerN.Value() );
               //double dblN1post( itOverlapUpperN1.Value() );

               ++itAlpha;
               ++itBeta;
               ++itOverlapLowerN;
               ++itOverlapUpperN1;
           }
        }


        RegionType regionOutputN( vecRescaledImages[0]->GetLargestPossibleRegion() );

        // Finally, add trimed, rescaled images to stitched output image
        for( unsigned int i = 0; i < this->GetNumberOfInputs(); i++ )
        {
            itk::ImageRegionIterator< TImage > itInputScaled( vecRescaledImages[i], vecRescaledImages[i]->GetLargestPossibleRegion() );
            itk::ImageRegionIterator< TImage > itOutput( pImageOutput, regionOutputN );

            while( !itOutput.IsAtEnd() )
            {
                itOutput.Value() += itInputScaled.Value();
                ++itInputScaled;
                ++itOutput;
            }

            // Shift regionOutputN for the next iteration
            regionOutputN.GetModifiableIndex()[1] += m_VerticalShiftPixels;
        }

        this->GraftOutput( pImageOutput );
    }

    template< typename TImage, typename TWeighting >
    void VerticalStitchingImageFilter< TImage, TWeighting >::GenerateInputRequestedRegion()
    {
        // Get pointer to the input
        //typename TWeightingVectorImage::Pointer pInput(  const_cast< TWeightingVectorImage * >( this->GetInput() ) );
        //const InputRegionType & regionLargestInput( pInput->GetLargestPossibleRegion() );
        //pInput->SetRequestedRegion( regionLargestInput );
    }

    template< typename TImage, typename TWeighting >
    void VerticalStitchingImageFilter< TImage, TWeighting >::GenerateOutputInformation()
    {
        Superclass::GenerateOutputInformation();

        typename TImage::ConstPointer pInputImage( this->GetInput() );
        typename TImage::Pointer pOutput( this->GetOutput() );

        if( !pInputImage || !pOutput )
          return;

        unsigned int uintNumInputs( this->GetNumberOfInputs() );

        RegionType regionTrimmed( ComputeTrimRegion( pInputImage ) );

        if( uintNumInputs == 1 )
        {
            // Set the output size to the trimmed input size
            pOutput->SetLargestPossibleRegion( regionTrimmed );
            return;
        }

        // Initialise output region
        RegionType regionOutput( regionTrimmed );

        // Initialize output size to be updated
        SizeType sizeOutput( regionOutput.GetSize() );

        // Get the index position of the shift
        IndexType indexVerticalShift;
        PointType pointVerticalShift;
        pointVerticalShift.Fill( 0.0 );
        pointVerticalShift[1] = m_VerticalShift;

        pInputImage->TransformPhysicalPointToIndex( pointVerticalShift, indexVerticalShift );

        m_VerticalShiftPixels = indexVerticalShift[1];

        // Compute the size of the vertical overlap in pixels
        unsigned int uintVerticalOverlap( sizeOutput[1] - m_VerticalShiftPixels );

        sizeOutput[1] = ( sizeOutput[1] * uintNumInputs ) - ( uintVerticalOverlap * ( uintNumInputs - 1  ) );

        // Update the size for the output region
        regionOutput.SetSize( sizeOutput );

        // Reset the vertical index to 0
        regionOutput.SetIndex( 1, 0 );

        // Set the output largest possible region to the total size of the stitched image
        pOutput->SetLargestPossibleRegion( regionOutput );

        // Set regions for the overlapped and non-overlapped areas
        m_RegionNonOverlap = regionTrimmed;
        m_RegionNonOverlap.SetIndex( 1, uintVerticalOverlap );
        m_RegionNonOverlap.SetSize( 1, m_VerticalShiftPixels - uintVerticalOverlap );

        m_RegionOverlapLower = regionTrimmed;
        m_RegionOverlapLower.SetIndex( 1, m_VerticalShiftPixels );
        m_RegionOverlapLower.SetSize( 1, uintVerticalOverlap );

        m_RegionOverlapUpper = regionTrimmed;
        m_RegionOverlapUpper.SetIndex( 1, 0 );
        m_RegionOverlapUpper.SetSize( 1, uintVerticalOverlap );

        m_RegionWeighting = regionTrimmed;
        m_RegionWeighting.SetIndex( 1, 0 );
        m_RegionWeighting.SetSize( 1, uintVerticalOverlap );
    }
}

#endif // itkVerticalStitchingImageFilter_hxx
