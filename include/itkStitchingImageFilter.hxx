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

namespace itk
{
    template< typename TImage >
    StitchingImageFilter< TImage >::StitchingImageFilter()
    {

    }

    template< typename TImage >
    void StitchingImageFilter< TImage >::PrintSelf( std::ostream& os, Indent indent ) const
    {
        Superclass::PrintSelf( os, indent );
    }

    template< typename TImage >
    void StitchingImageFilter< TImage >::GenerateData()
    {
        ImagePointer pOutput( this->GetOutput() );

        // Create the actual output image
        ImagePointer pOutputStitched( TImage::New() );
        pOutputStitched->SetRegions( pOutput->GetLargestPossibleRegion() );
        pOutputStitched->Allocate( true );
        pOutputStitched->FillBuffer( 100.0 );

        this->GetOutput()->Graft( pOutputStitched );
    }

    template< typename TImage >
    void StitchingImageFilter< TImage >::GenerateInputRequestedRegion()
    {
        // Get pointer to the input
        ImagePointer pInput(  const_cast< TImage * >( this->GetInput() ) );
        const RegionType & regionLargestInput( pInput->GetLargestPossibleRegion() );
        pInput->SetRequestedRegion( regionLargestInput );
    }

    template< typename TImage >
    void StitchingImageFilter< TImage >::GenerateOutputInformation()
    {
        Superclass::GenerateOutputInformation();

        const TImage *pInput( this->GetInput() );
        TImage *pOutput( this->GetOutput() );

        if( !pInput || !pOutput )
          return;

        int intNumInputs( this->GetNumberOfInputs() );

        if( intNumInputs == 1 )
            // Nothing to do if there is only one input
            return;

        // Calculate the size of the overlap in pixels

        RegionType regionOutput( pInput->GetLargestPossibleRegion() );

        // Set size of first input
        SizeType sizeOutput( regionOutput.GetSize() );

        IndexType indexOverlapPixels;
        pOutput->TransformPhysicalPointToIndex( this->GetOverlap(), indexOverlapPixels );
        SizeType sizeOverlapPixels;

        for( unsigned int j = 0; j < ImageDimension; j++ )
            sizeOverlapPixels[j] = indexOverlapPixels[j];

        // Iterate through each input
        for( int intInputIdx = 1; intInputIdx < intNumInputs; intInputIdx++ )
        {
            const TImage *pInputN( this->GetInput( intInputIdx ) );
            SizeType sizeInputN( pInputN->GetLargestPossibleRegion().GetSize() );

            // Add the difference between the n-th input and the overlap to the output image size
            sizeOutput += ( sizeInputN - sizeOverlapPixels );
        }

        // Update the size for the output region
        regionOutput.SetSize( sizeOutput );

        // Set the output largest possible region to the total size of the stitched image
        pOutput->SetLargestPossibleRegion( regionOutput );
    }
}

#endif // itkStitchingImageFilter_hxx
