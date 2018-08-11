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
#ifndef itkMaskedMedianImageFilter_hxx
#define itkMaskedMedianImageFilter_hxx

#include "itkMaskedMedianImageFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

#include <vector>
#include <algorithm>

#define DEFAULT_FILTER_RADIUS 3

namespace itk
{
    template< typename TInputImage, typename TOutputImage, typename TMaskImage >
    MaskedMedianImageFilter< TInputImage, TOutputImage, TMaskImage >::MaskedMedianImageFilter()
    {
        // Set default filter radius
        typename TOutputImage::SizeType sizeRadius;
        sizeRadius.Fill( DEFAULT_FILTER_RADIUS );
        this->SetRadius( sizeRadius );
    }

    template< typename TInputImage, typename TOutputImage, typename TMaskImage >
    void MaskedMedianImageFilter< TInputImage, TOutputImage, TMaskImage >::ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread, ThreadIdType threadId )
    {
        // Allocate output
        typename OutputImageType::Pointer pOutput( this->GetOutput() );
        typename InputImageType::ConstPointer pInput( this->GetInput() );
        typename MaskImageType::ConstPointer pMask( this->GetMaskImage() );

        // Find the data-set boundary "faces"
        NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType > bC;
        typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType >::FaceListType
        faceList = bC( pInput, outputRegionForThread, this->GetRadius() );

        // support progress methods/callbacks
        ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

        // All of our neighborhoods have an odd number of pixels, so there is
        // always a median index (if there where an even number of pixels
        // in the neighborhood we have to average the middle two values).
        ZeroFluxNeumannBoundaryCondition< InputImageType > nbc;
        std::vector< InputPixelType >                      pixels;

        // Process each of the boundary faces.  These are N-d regions which border
        // the edge of the buffer.
        for( typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType >::FaceListType::iterator fit = faceList.begin(); fit != faceList.end(); ++fit )
        {
            ImageRegionConstIterator< InputImageType > itInput( ImageRegionConstIterator< InputImageType >( pInput, *fit ) );
            ImageRegionConstIterator< MaskImageType > itMask( ImageRegionConstIterator< MaskImageType >( pMask, *fit ) );
            ImageRegionIterator< OutputImageType > itOutput( ImageRegionIterator< OutputImageType >( pOutput, *fit ) );

            ConstNeighborhoodIterator< InputImageType > bit( ConstNeighborhoodIterator< InputImageType >( this->GetRadius(), pInput, *fit ) );
            bit.OverrideBoundaryCondition( &nbc );
            bit.GoToBegin();

            const unsigned int neighborhoodSize( bit.Size() );
            const unsigned int medianPosition( neighborhoodSize / 2 );

            while( !bit.IsAtEnd() )
            {
                // collect all the pixels in the neighborhood, note that we use
                // GetPixel on the NeighborhoodIterator to honor the boundary conditions
                pixels.resize( neighborhoodSize );
                for( unsigned int i = 0; i < neighborhoodSize; ++i )
                    pixels[i] = ( bit.GetPixel( i ) );

                // get the median value
                const typename std::vector< InputPixelType >::iterator medianIterator( pixels.begin() + medianPosition );
                std::nth_element( pixels.begin(), medianIterator, pixels.end() );

                double dblMedianValue( static_cast< double >( *medianIterator ) );

                // Apply median filter only to pixels with a non-zero mask value
                itOutput.Set( itMask.Value() ? static_cast< typename OutputImageType::PixelType >(  dblMedianValue ) : itInput.Value() );

                ++bit;
                ++itOutput;
                ++itInput;
                ++itMask;

                progress.CompletedPixel();
            }
        }
    }
}

#endif // itkMaskedMedianImageFilter_hxx
