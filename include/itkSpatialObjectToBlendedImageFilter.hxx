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
#ifndef itkSpatialObjectToBlendedImageFilter_hxx
#define itkSpatialObjectToBlendedImageFilter_hxx

#include "itkSpatialObjectToBlendedImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkProgressReporter.h"
#include "itkMath.h"
#include <limits>
#include <typeinfo>
#include <cmath>
#include "itkMinimumMaximumImageCalculator.h"

namespace itk
{
    template< typename TOutputImage >
    SpatialObjectToBlendedImageFilter< TOutputImage >::SpatialObjectToBlendedImageFilter()
        : SpatialObjectToImageFilter< GroupSpatialObjectType, TOutputImage >()
    {
    }

    template< typename TOutputImage >
    void SpatialObjectToBlendedImageFilter< TOutputImage >::AllocateOutputs()
    {
        // Get the input and output pointers
        const GroupSpatialObjectType *pInputSpatialObject( this->GetInput() );
        OutputImagePointer pOutput( this->GetOutput() );

        // Generate the image
        SizeType sizeOutput;

        pInputSpatialObject->ComputeBoundingBox();

        for( unsigned int i = 0; i < OutputImageDimension; i++ )
            sizeOutput[i] = static_cast<SizeValueType>( pInputSpatialObject->GetBoundingBox()->GetMaximum()[i] - pInputSpatialObject->GetBoundingBox()->GetMinimum()[i] );

        typename OutputImageType::IndexType index;
        index.Fill(0);
        typename OutputImageType::RegionType region;

        // If the size of the output has been explicitly specified, the filter
        // will set the output size to the explicit size, otherwise the size from the
        // spatial
        // object's bounding box will be used as default.
        bool bSpecifiedSize( false );
        for( unsigned int i = 0; i < OutputImageDimension; i++ )
        {
            if ( Superclass::m_Size[i] != 0 )
            {
                bSpecifiedSize = true;
                break;
            }
        }

        region.SetSize( bSpecifiedSize ? Superclass::m_Size : sizeOutput );
        region.SetIndex(index);

        pOutput->SetLargestPossibleRegion( region );
        pOutput->SetBufferedRegion( region );
        pOutput->SetRequestedRegion( region );

        // If the spacing has been explicitly specified, the filter
        // will set the output spacing to that explicit spacing, otherwise the spacing
        // from the spatial object is used as default.
        bool bSpecifiedSpacing( false );
        for( unsigned int i = 0; i < OutputImageDimension; i++ )
        {
            if ( Math::NotExactlyEquals( Superclass::m_Spacing[i], 0 ) )
            {
                bSpecifiedSpacing = true;
                break;
            }
        }

        // set spacing
        pOutput->SetSpacing( bSpecifiedSpacing ? this->m_Spacing : pInputSpatialObject->GetIndexToObjectTransform()->GetScaleComponent() );

        pOutput->SetOrigin( Superclass::m_Origin );     //   and origin
        pOutput->SetDirection( Superclass::m_Direction );
        pOutput->Allocate();   // allocate the image
        pOutput->FillBuffer( 0 );
    }

    template< typename TOutputImage >
    void SpatialObjectToBlendedImageFilter< TOutputImage >::GenerateData()
    {
        // Call ImageSource base class method
        Superclass::Superclass::GenerateData();
    }

    template< typename TOutputImage >
    void SpatialObjectToBlendedImageFilter< TOutputImage >::BeforeThreadedGenerateData()
    {
        const GroupSpatialObjectType *pInputSpatialObject( this->GetInput() );
        typename GroupSpatialObjectType::ChildrenListType* pChildList( pInputSpatialObject->GetChildren() );

        //typedef itk::MinimumMaximumImageCalculator< OutputImageType > MinMaxImageCalcType;
        using  MinMaxImageCalcType = itk::MinimumMaximumImageCalculator< OutputImageType >;
        typename MinMaxImageCalcType::Pointer pMinMax( MinMaxImageCalcType::New() );

        m_ImageMin = std::numeric_limits< typename TOutputImage::PixelType >::max();
        m_ImageMax = std::numeric_limits< typename TOutputImage::PixelType >::min();

        // Determine the min and max image values for all child ImageSpatialObjects
        for( typename ImageSpatialObjectType::ChildrenListType::const_iterator iterator = pChildList->begin(); iterator != pChildList->end(); ++iterator )
        {
            if( typeid( **iterator ) == typeid( ImageSpatialObjectType ) )
            {
                ImageSpatialObjectType* pImageSpatialObject( reinterpret_cast< ImageSpatialObjectType* >( ( *iterator ).GetPointer() ) );

                pMinMax->SetImage( pImageSpatialObject->GetImage() );
                pMinMax->Compute();

                if( pMinMax->GetMinimum() < m_ImageMin )
                    m_ImageMin = pMinMax->GetMinimum();

                if( pMinMax->GetMaximum() > m_ImageMax )
                    m_ImageMax = pMinMax->GetMaximum();
            }
        }
    }

    template< typename TOutputImage >
    void SpatialObjectToBlendedImageFilter< TOutputImage >::ThreadedGenerateData( const RegionType & outputRegionForThread, ThreadIdType threadId )
    {
        // Get the input and output pointers
        const GroupSpatialObjectType *pInputSpatialObject( this->GetInput() );
        OutputImagePointer pOutput( this->GetOutput() );

        typename GroupSpatialObjectType::ChildrenListType* pChildList( pInputSpatialObject->GetChildren() );

        ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

        typedef itk::ImageRegionIteratorWithIndex< OutputImageType > iteratorRegion;

        iteratorRegion itRegion( pOutput, outputRegionForThread );

        itk::Point< double, OutputImageDimension >      objectPoint;
        itk::Point< double, OutputImageDimension >      imagePoint;
        double dblValObject;

        while( !itRegion.IsAtEnd() )
        {
            // ValueAt requires the point to be in physical coordinate i.e
            pOutput->TransformIndexToPhysicalPoint( itRegion.GetIndex(), imagePoint );

            for( unsigned int i = 0; i < OutputImageDimension; i++ )
                objectPoint[i] = imagePoint[i];

            if( pInputSpatialObject->IsEvaluableAt( objectPoint, Superclass::m_ChildrenDepth ) )
            {
                double dblValPoint( 0 );
                int intDivisor( 0 );

                // Iterate through each child spatial object
                for( typename ImageSpatialObjectType::ChildrenListType::const_iterator iterator = pChildList->begin(); iterator != pChildList->end(); ++iterator )
                {
                    // Only include, inside, evaluable and finite values into the blended calculation
                    if( (*iterator)->IsInside( objectPoint ) && (*iterator)->ValueAt( objectPoint, dblValObject ) && std::isfinite( dblValObject ) )
                    {
                        if( typeid( **iterator ) == typeid( ImageSpatialObjectType ) )
                        {
                            // Special check for ImageSpatialObjects to ensure that evaluated pixel
                            // values fall within the range of image pixel values of the contained
                            // ImageSpatialObjects, if this condition fails then ignore this value
                            if( dblValObject < m_ImageMin || dblValObject > m_ImageMax  )
                                continue;
                        }

                        dblValPoint += dblValObject;
                        intDivisor++;
                    }
                }

                itRegion.Set( intDivisor == 1 ? dblValPoint : dblValPoint / static_cast<double>( intDivisor ) );
            }

            ++itRegion;

            progress.CompletedPixel();
        }
    }
}

#endif // itkSpatialObjectToBlendedImageFilter_hxx
