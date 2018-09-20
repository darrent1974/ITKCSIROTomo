/*=========================================================================
 *
 *  Copyright Insight Software Consortium
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

#include "itkVerticalStitchingImageFilter.h"

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkVectorImage.h"
#include "itkComposeImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageSpatialObject.h"
#include "itkMaximumProjectionImageFilter.h"
#include "itkMeanProjectionImageFilter.h"
#include "itkImageScanlineIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkExtractImageFilter.h"
#include "itkStatisticsImageFilter.h"

using PixelType = float;
using ImageType = itk::Image< PixelType, 2 >;
using WeightVectorImageType = itk::VectorImage< PixelType, 2 >;
using ImageDuplicatorType = itk::ImageDuplicator< ImageType >;
using ComposeVectorImageType = itk::ComposeImageFilter< ImageType >;
using ImageFileReaderType = itk::ImageFileReader< ImageType >;
using ImageFileWriterType = itk::ImageFileWriter< ImageType >;
using LineType = itk::Image< PixelType, 1 >;
using MaximumProjectionType = itk::MaximumProjectionImageFilter< ImageType, LineType >;
using MeanProjectionType = itk::MeanProjectionImageFilter< ImageType, LineType >;
using ExtractImageFilterType = itk::ExtractImageFilter< ImageType, ImageType >;
using StatisticsImageFilterType = itk::StatisticsImageFilter< ImageType >;

namespace
{
    class ShowProgress : public itk::Command
    {
    public:
        itkNewMacro( ShowProgress )

        void Execute( itk::Object* caller, const itk::EventObject& event ) override
        {
            Execute( dynamic_cast< const itk::Object* >( caller ), event );
        }

        void Execute( const itk::Object* caller, const itk::EventObject& event ) override
        {
            if ( !itk::ProgressEvent().CheckEvent( &event ) )
                return;

            const auto* pProcessObject( dynamic_cast< const itk::ProcessObject* >( caller ) );

            if ( !pProcessObject )
                return;

            std::cout << " " << pProcessObject->GetProgress();
        }
    };
}


int itkVerticalStitchingImageFilterTest( int argc, char * argv[] )
{
    if( argc < 3 )
    {
        std::cerr << "Missing parameters." << std::endl;
        std::cerr << "Usage: " << argv[0] << " inputImageFile1 inputImageFile2 outputImageFile" << std::endl;
        return EXIT_FAILURE;
    }

    ImageType::SpacingType spacingImage;
    spacingImage.Fill( 1.0 );

    ImageFileReaderType::Pointer pImageFileReaderInput1( ImageFileReaderType::New() );
    pImageFileReaderInput1->SetFileName( argv[1] );
    TRY_EXPECT_NO_EXCEPTION( pImageFileReaderInput1->Update() );

    ImageFileReaderType::Pointer pImageFileReaderInput2( ImageFileReaderType::New() );
    pImageFileReaderInput2->SetFileName( argv[2] );
    TRY_EXPECT_NO_EXCEPTION( pImageFileReaderInput2->Update() );

    ImageFileWriterType::Pointer pImageFileWriter( ImageFileWriterType::New() );

    using FilterType = itk::VerticalStitchingImageFilter< ImageType, ImageType >;
    FilterType::Pointer pFilter( FilterType::New() );

    ImageType::RegionType regionImage( pImageFileReaderInput1->GetOutput()->GetLargestPossibleRegion() );
    ImageType::SizeType size( regionImage.GetSize() );

    EXERCISE_BASIC_OBJECT_METHODS( pFilter, VerticalStitchingImageFilter, ImageToImageFilter );

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );

    pFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pFilter->SetInput( 0, pImageFileReaderInput1->GetOutput() );
    pFilter->SetInput( 1, pImageFileReaderInput2->GetOutput() );

    pFilter->SetVerticalShift( 100.0 );

    // Don't do any trimming
    ImageType::PointType pointTrimMin;
    pointTrimMin[0] = 0.0;
    pointTrimMin[1] = 0.0;

    pFilter->SetTrimPointMin( pointTrimMin );

    ImageType::PointType pointTrimMax;
    pointTrimMax[0] = static_cast<double>( size[0] ) * spacingImage[0];
    pointTrimMax[1] = ( static_cast<double>( size[1] ) * spacingImage[1] );

    pFilter->SetTrimPointMax( pointTrimMax );

    TRY_EXPECT_NO_EXCEPTION( pFilter->Update() );

    pImageFileWriter->SetFileName( argv[3] );
    pImageFileWriter->SetInput( pFilter->GetOutput() );
    TRY_EXPECT_NO_EXCEPTION( pImageFileWriter->Update() );

    std::cout << "Test finished." << std::endl;

    return EXIT_SUCCESS;
}
