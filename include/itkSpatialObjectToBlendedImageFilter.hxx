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

namespace itk
{
template< typename TInputSpatialObject, typename TOutputImage >
SpatialObjectToBlendedImageFilter< TInputSpatialObject, TOutputImage >::SpatialObjectToBlendedImageFilter()
    : SpatialObjectToImageFilter< TInputSpatialObject, TOutputImage >()
{
}

template< typename TInputSpatialObject, typename TOutputImage >
void SpatialObjectToBlendedImageFilter< TInputSpatialObject, TOutputImage >::GenerateData( void )
{
    itkDebugMacro(<< "SpatialObjectToBlendedImageFilter::Update() called");

    // Get the input and output pointers
    const InputSpatialObjectType *pInputSpatialObject( this->GetInput() );
    OutputImagePointer pOutput( this->GetOutput() );

    // Generate the image
    SizeType sizeOutput;

    pInputSpatialObject->ComputeBoundingBox();

    for( unsigned int i = 0; i < ObjectDimension; i++ )
    {
        sizeOutput[i] = static_cast<SizeValueType>( pInputSpatialObject->GetBoundingBox()->GetMaximum()[i] - pInputSpatialObject->GetBoundingBox()->GetMinimum()[i] );
    }

    typename OutputImageType::IndexType index;
    index.Fill(0);
    typename OutputImageType::RegionType region;

    // If the size of the output has been explicitly specified, the filter
    // will set the output size to the explicit size, otherwise the size from the
    // spatial
    // object's bounding box will be used as default.
    bool bSpecifiedSize( false );
    for( unsigned int i = 0; i < OutputImageDimension; i++ )
		if ( Superclass::m_Size[i] != 0 )
		{
            bSpecifiedSize = true;
			break;
		}

    region.SetSize( bSpecifiedSize ? Superclass::m_Size : sizeOutput );
	region.SetIndex(index);

    pOutput->SetLargestPossibleRegion( region );      //
    pOutput->SetBufferedRegion( region );             // set the region
    pOutput->SetRequestedRegion( region );            //

	// If the spacing has been explicitly specified, the filter
	// will set the output spacing to that explicit spacing, otherwise the spacing
	// from
	// the spatial object is used as default.
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

	typedef itk::ImageRegionIteratorWithIndex< OutputImageType > myIteratorType;

    myIteratorType it(pOutput, region);

	itk::Point< double, ObjectDimension >      objectPoint;
	itk::Point< double, OutputImageDimension > imagePoint;
	double dblValObject;

    ProgressReporter progress( this, 0, pOutput->GetRequestedRegion().GetNumberOfPixels() );

    typename TInputSpatialObject::ChildrenListType* pChildList( pInputSpatialObject->GetChildren() );

	while( !it.IsAtEnd() )
	{
		// ValueAt requires the point to be in physical coordinate i.e
        pOutput->TransformIndexToPhysicalPoint( it.GetIndex(), imagePoint );

        for( unsigned int i = 0; i < ObjectDimension; i++ )
			objectPoint[i] = imagePoint[i];

        if( pInputSpatialObject->IsEvaluableAt( objectPoint, Superclass::m_ChildrenDepth ) )
		{
			double dblValPoint( 0 );
			int intDivisor( 0 );

			// Iterate through each child spatial object
			for( typename TInputSpatialObject::ChildrenListType::const_iterator iterator = pChildList->begin(); iterator != pChildList->end(); ++iterator )
			{
				if( (*iterator)->ValueAt( objectPoint, dblValObject ) )
				{
					dblValPoint += dblValObject;
					intDivisor++;
				}
			}

			it.Set( intDivisor == 1 ? dblValPoint : dblValPoint / static_cast<double>( intDivisor ) );
		}

		++it;

		progress.CompletedPixel();
	}

  itkDebugMacro(<< "SpatialObjectToBlendedImageFilter::Update() finished");
}
}

#endif // itkSpatialObjectToBlendedImageFilter_hxx
