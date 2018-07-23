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
#ifndef itkThresholdedMedianImageFilter_hxx
#define itkThresholdedMedianImageFilter_hxx

#include "itkThresholdedMedianImageFilter.h"


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
    template< typename TInputImage, typename TOutputImage >
    ThresholdedMedianImageFilter< TInputImage, TOutputImage >::ThresholdedMedianImageFilter()
        : m_ThresholdLower( 0.0 )
        , m_ThresholdUpper( 1.0 )
        , m_Iterations( 1 )
    {
        // Set default filter radius
        typename TOutputImage::SizeType sizeRadius;
        sizeRadius.Fill( DEFAULT_FILTER_RADIUS );
        this->SetRadius( sizeRadius );
    }

    template< typename TInputImage, typename TOutputImage >
    void ThresholdedMedianImageFilter< TInputImage, TOutputImage >::ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread, ThreadIdType threadId )
    {
        // Allocate output
        typename OutputImageType::Pointer output( this->GetOutput() );
        typename  InputImageType::ConstPointer input( this->GetInput() );

        // Find the data-set boundary "faces"
        NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType > bC;
        typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType >::FaceListType
        faceList = bC( input, outputRegionForThread, this->GetRadius() );

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
            ImageRegionConstIterator< InputImageType > itInput( ImageRegionConstIterator< InputImageType >( input, *fit ) );
            ImageRegionIterator< OutputImageType > itOutput( ImageRegionIterator< OutputImageType >( output, *fit ) );

            ConstNeighborhoodIterator< InputImageType > bit( ConstNeighborhoodIterator< InputImageType >( this->GetRadius(), input, *fit ) );
            bit.OverrideBoundaryCondition( &nbc );
            bit.GoToBegin();

            const unsigned int neighborhoodSize( bit.Size() );
            const unsigned int medianPosition( neighborhoodSize / 2 );

            while( !bit.IsAtEnd() )
            {
                // Apply the median filter the given number of iterations
                for( unsigned int uintIteration = 0; uintIteration < m_Iterations; uintIteration++ )
                {
                    // collect all the pixels in the neighborhood, note that we use
                    // GetPixel on the NeighborhoodIterator to honor the boundary conditions
                    pixels.resize( neighborhoodSize );
                    for( unsigned int i = 0; i < neighborhoodSize; ++i )
                        pixels[i] = ( bit.GetPixel( i ) );

                    // get the median value
                    const typename std::vector< InputPixelType >::iterator medianIterator( pixels.begin() + medianPosition );
                    std::nth_element( pixels.begin(), medianIterator, pixels.end() );

                    double dblPixelValue( static_cast< double >( itInput.Get() ) );

                    // Apply median filter only to pixels that fall outside the threshold range
                    itOutput.Set( dblPixelValue > m_ThresholdLower && dblPixelValue <= m_ThresholdUpper ? itInput.Get() : static_cast< typename OutputImageType::PixelType >(  *medianIterator ) );
                }

                ++bit;
                ++itOutput;
                ++itInput;

                progress.CompletedPixel();
            }
        }
    }
}

#endif // itkThresholdedMedianImageFilter_hxx
