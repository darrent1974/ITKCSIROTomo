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
    if( argc < 1 )
    {
        std::cerr << "Usage: " << argv[0];
        std::cerr << std::endl;
        return EXIT_FAILURE;
    }

    ImageType::SpacingType spacingImage;
    spacingImage.Fill( 0.1 );

    ImageFileReaderType::Pointer pImageFileReader( ImageFileReaderType::New() );
    ImageFileWriterType::Pointer pImageFileWriter( ImageFileWriterType::New() );

    using FilterType = itk::VerticalStitchingImageFilter< ImageType, ImageType >;
    FilterType::Pointer pFilter( FilterType::New() );

//#define LOCAL_TEST
#ifdef LOCAL_TEST
    pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/darkcorrectedflat_0.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage1( pImageFileReader->GetOutput() );
    pImage1->DisconnectPipeline();

    ImageType::RegionType region( pImage1->GetLargestPossibleRegion() );
    ImageType::SizeType size( region.GetSize() );

    pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/darkcorrectedflat_1.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage2( pImageFileReader->GetOutput() );
    pImage2->DisconnectPipeline();

    pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/darkcorrectedflat_2.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage3( pImageFileReader->GetOutput() );
    pImage3->DisconnectPipeline();

    pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/darkcorrectedflat_3.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage4( pImageFileReader->GetOutput() );
    pImage4->DisconnectPipeline();
#else
    ImageType::SizeType size;
    size.Fill( 120 );
    ImageType::Pointer pImage1( ImageType::New() );
    pImage1->SetRegions( size );
    pImage1->SetSpacing( spacingImage );
    pImage1->Allocate();
    pImage1->FillBuffer( 100.0f );

    ImageType::Pointer pImage2( ImageType::New() );
    pImage2->SetRegions( size );
    pImage2->SetSpacing( spacingImage );
    pImage2->Allocate();
    pImage2->FillBuffer( 200.0f );

    ImageType::Pointer pImage3( ImageType::New() );
    pImage3->SetRegions( size );
    pImage3->SetSpacing( spacingImage );
    pImage3->Allocate();
    pImage3->FillBuffer( 150.0f );

    ImageType::Pointer pImage4( ImageType::New() );
    pImage4->SetRegions( size );
    pImage4->SetSpacing( spacingImage );
    pImage4->Allocate();
    pImage4->FillBuffer( 50.0f );
#endif

    FilterType::SpacingType spacingShift;
    spacingShift[0] = 0.0;

    EXERCISE_BASIC_OBJECT_METHODS( pFilter, VerticalStitchingImageFilter, ImageToImageFilter );

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );

    pFilter->AddObserver( itk::ProgressEvent(), pShowProgress );

    pFilter->SetInput( 0, pImage1 );
    pFilter->SetInput( 1, pImage2 );
    pFilter->SetInput( 2, pImage3 );
    pFilter->SetInput( 3, pImage4 );

#ifdef LOCAL_TEST
    pFilter->SetVerticalShift( 15.7 );
#else
    spacingShift[1] = 5;
    pFilter->SetVerticalShift( 5.0 );
#endif

    // Trim each input image by 1.0mm on the top and bottom prior to stitching
    ImageType::PointType pointTrimMin;
    pointTrimMin[0] = 0.0;
#ifdef LOCAL_TEST
    pointTrimMin[1] = 0.2;
#else
    pointTrimMin[1] = 1.0;
#endif
    pFilter->SetTrimPointMin( pointTrimMin );

    ImageType::PointType pointTrimMax;
    pointTrimMax[0] = static_cast<double>( size[0] ) * spacingImage[0];
#ifdef LOCAL_TEST
    pointTrimMax[1] = ( static_cast<double>( size[1] ) * spacingImage[1] ) - 1.1;
#else
    pointTrimMax[1] = ( static_cast<double>( size[1] ) * spacingImage[1] ) - 1.0;
#endif
    pFilter->SetTrimPointMax( pointTrimMax );

    try
    {
        //pFilter->Update();

#ifdef LOCAL_TEST
        pImageFileWriter->SetFileName( "stitched.mhd" );
        pImageFileWriter->SetInput( pFilter->GetOutput() );
        TRY_EXPECT_NO_EXCEPTION( pImageFileWriter->Update() );

        pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/FFstitched.tif" );
        pImageFileReader->Update();
        ImageType::ConstPointer pImageStitched( pImageFileReader->GetOutput() );
#endif
    }
    catch( itk::ExceptionObject & error )
    {
        std::cerr << "Error: " << error << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
