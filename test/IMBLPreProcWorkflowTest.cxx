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

#include "itkMaskedMedianImageFilter.h"
#include "itkThresholdedMedianMaskImageFilter.h"

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
#include "itkRegularExpressionSeriesFileNames.h"
#include "itkMeanProjectionImageFilter.h"
#include "itkChangeInformationImageFilter.h"
#include "itkVerticalStitchingImageFilter.h"

#include "itkTestingMacros.h"

using ImageType = itk::Image< float, 2 >;
using VolumeType = itk::Image< float, 3 >;
using MaskImageType = itk::Image< unsigned char, 2 >;

using ImageReader = itk::ImageFileReader< ImageType >;
using ImageSeriesReader = itk::ImageSeriesReader< VolumeType >;
using RegexSeriesFileNames = itk::RegularExpressionSeriesFileNames;
using FileNamesContainer = ImageSeriesReader::FileNamesContainer;
using ImageWriter = itk::ImageFileWriter< ImageType >;
using MaskImageWriter = itk::ImageFileWriter< MaskImageType >;
using ChangeInformationImageType = itk::ChangeInformationImageFilter< ImageType >;
using MeanProjectionImageFilter = itk::MeanProjectionImageFilter< VolumeType, ImageType >;
using VerticalStitchingImageFilter = itk::VerticalStitchingImageFilter< ImageType, ImageType >;
using VerticalStitchingVolumeFilter = itk::VerticalStitchingImageFilter< VolumeType, ImageType >;

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

FileNamesContainer GetRegexFiles( const std::string& strDir, const std::string& strRegex )
{
    RegexSeriesFileNames::Pointer pRegexFileNames( RegexSeriesFileNames::New() );
    pRegexFileNames->SetDirectory( strDir );
    pRegexFileNames->SetRegularExpression( strRegex );
    pRegexFileNames->NumericSortOn();

    return pRegexFileNames->GetFileNames();
}

FileNamesContainer GetDarkFiles( const std::string& strDir, unsigned int uintStack )
{
    std::stringstream ssRegex;
    ssRegex << "DF_Y" << uintStack << "_[A-Z]+_([0-9]+).tif";

    return GetRegexFiles( strDir, ssRegex.str() );
}

FileNamesContainer GetFlatFiles( const std::string& strDir, unsigned int uintStack )
{
    std::stringstream ssRegex;
    ssRegex << "BG_Y" << uintStack << "_[A-Z]+_([0-9]+).tif";

    return GetRegexFiles( strDir, ssRegex.str() );
}

FileNamesContainer GetProjectionFiles( const std::string& strDir, unsigned int uintStack )
{
    std::stringstream ssRegex;
    ssRegex << "SAMPLE_Y" << uintStack << "_T[A-Z]+_([0-9]+).tif";

    return GetRegexFiles( strDir, ssRegex.str() );
}

VolumeType::Pointer ReadImageSeries( const FileNamesContainer& vecFileNames )
{
    ImageSeriesReader::Pointer pSeriesReader( ImageSeriesReader::New() );
    pSeriesReader->SetFileNames( vecFileNames );
    pSeriesReader->Update();
    return pSeriesReader->GetOutput();
}

ImageType::Pointer ChangeImageSpacing( ImageType::Pointer pImage, double dblSpacing )
{
    ImageType::SpacingType spacing;
    spacing.Fill( dblSpacing );

    ChangeInformationImageType::Pointer pChangeImageInfoFilter( ChangeInformationImageType::New() );
    pChangeImageInfoFilter->SetInput( pImage );
    pChangeImageInfoFilter->SetOutputSpacing( spacing );
    pChangeImageInfoFilter->ChangeSpacingOn();
    pChangeImageInfoFilter->Update();

    return pChangeImageInfoFilter->GetOutput();
}

int IMBLPreProcWorkflowTest( int argc, char * argv[] )
{
    if( argc < 1 )
    {
        std::cerr << "Usage: " << argv[0];
        std::cerr << std::endl;
        return EXIT_FAILURE;
    }

#ifdef LOCAL_TEST
    const unsigned int uintNumStacks( 4 );
    const double dblSpacing( 0.1 );
    const double dblVerticalShift( 15.7 );

    std::string strInputDir( "/mnt/csiro/civ/archive/IMBL/13393a/input/4704628_60keV_6m_2mGy" );

    try
    {
        // Create averaged dark image from the first set of dark files
        MeanProjectionImageFilter::Pointer pMeanProjectionImageFilter( MeanProjectionImageFilter::New() );
        pMeanProjectionImageFilter->SetInput( ReadImageSeries( GetDarkFiles( strInputDir, 0 ) ) );

        ImageType::Pointer pAverageDark( ChangeImageSpacing( pMeanProjectionImageFilter->GetOutput(), dblSpacing ) );
        ImageType::RegionType regionRawImage( pAverageDark->GetLargestPossibleRegion() );

        // Create a vector af averaged flats for each stack
        std::vector<ImageType::Pointer> vecFlats;

        // Stitch averaged flats, computing weigting matrices
        VerticalStitchingImageFilter::Pointer pVerticalStitchingImageFilter( VerticalStitchingImageFilter::New() );
        pVerticalStitchingImageFilter->SetVerticalShift( dblVerticalShift );

        ImageType::PointType pointTrimMin;
        pointTrimMin[0] = 0.0;
        pointTrimMin[1] = 0.2;

        pVerticalStitchingImageFilter->SetTrimPointMin( pointTrimMin );

        ImageType::PointType pointTrimMax;
        pointTrimMax[0] = static_cast<double>( regionRawImage.GetSize( 0 ) ) * dblSpacing;
        pointTrimMax[1] = ( static_cast<double>( regionRawImage.GetSize( 1 ) ) * dblSpacing ) - 1.1;

        pVerticalStitchingImageFilter->SetTrimPointMax( pointTrimMax );

        // create average flats for each stack, adding the result to a vector
        for( unsigned int uintStackIdx = 0; uintStackIdx < uintNumStacks; uintStackIdx++ )
        {
            MeanProjectionImageFilter::Pointer pMeanProjectionImageFilter( MeanProjectionImageFilter::New() );
            pMeanProjectionImageFilter->SetInput( ReadImageSeries( GetFlatFiles( strInputDir, uintStackIdx ) ) );

            pVerticalStitchingImageFilter->SetInput( uintStackIdx, ChangeImageSpacing( pMeanProjectionImageFilter->GetOutput(), dblSpacing ) );

            //vecFlats.push_back( ChangeImageSpacing( pMeanProjectionImageFilter->GetOutput(), dblSpacing ) );
        }

        pVerticalStitchingImageFilter->Update();
        ImageType::Pointer pStitchedFlat( pVerticalStitchingImageFilter->GetOutput() );

        ImageWriter::Pointer pImageWriter( ImageWriter::New() );
        pImageWriter->SetInput( pStitchedFlat );
        pImageWriter->SetFileName( "workflow_stitched_flat.mhd" );
        pImageWriter->Update();

    }
    catch( itk::ExceptionObject & error )
    {
        std::cerr << "Error: " << error << std::endl;
        return EXIT_FAILURE;
    }

#ifdef TEMP_REMOVED
    itk::ImageFileReader< ImageType >::Pointer pImageFileReader( itk::ImageFileReader< ImageType >::New() );
    pImageFileReader->SetFileName( "stitched.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage( pImageFileReader->GetOutput() );

    using ThresholdedMedianMaskImageFilterType = itk::ThresholdedMedianMaskImageFilter< ImageType, MaskImageType >;
    ThresholdedMedianMaskImageFilterType::Pointer pThresholdedMedianMaskImageFilter( ThresholdedMedianMaskImageFilterType::New() );
    pThresholdedMedianMaskImageFilter->SetInput( pImage );
    pThresholdedMedianMaskImageFilter->SetThresholdLower( 0.5 );
    pThresholdedMedianMaskImageFilter->SetThresholdUpper( 1.5 );

    ThresholdedMedianMaskImageFilterType::RadiusType radiusFilter;
    radiusFilter.Fill( 3 );
    pThresholdedMedianMaskImageFilter->SetRadius( radiusFilter );
    pThresholdedMedianMaskImageFilter->Update();
    MaskImageType::ConstPointer pMask( pThresholdedMedianMaskImageFilter->GetOutput() );

    MaskImageWriter::Pointer pMaskImageFileWriter( MaskImageWriter::New() );
    pMaskImageFileWriter->SetInput( pMask );
    pMaskImageFileWriter->SetFileName( "mask.mhd" );
    pMaskImageFileWriter->Update();

    using MaskedMedianImageFilterType = itk::MaskedMedianImageFilter< ImageType, ImageType, MaskImageType >;
    MaskedMedianImageFilterType::Pointer pMaskedMedianImageFilter( MaskedMedianImageFilterType::New() );

    EXERCISE_BASIC_OBJECT_METHODS( pMaskedMedianImageFilter, MaskedMedianImageFilter, BoxImageFilter );

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );
    pMaskedMedianImageFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pMaskedMedianImageFilter->SetInput( pImage );
    pMaskedMedianImageFilter->SetMaskImage( pMask );
    pMaskedMedianImageFilter->SetRadius( radiusFilter );

    try
    {
        pMaskedMedianImageFilter->Update();

        ImageWriter::Pointer pImageFileWriter( ImageWriter::New() );
        pImageFileWriter->SetInput( pMaskedMedianImageFilter->GetOutput() );
        pImageFileWriter->SetFileName( "median_masked.mhd" );
        pImageFileWriter->Update();

    }
    catch( itk::ExceptionObject & error )
    {
        std::cerr << "Error: " << error << std::endl;
        return EXIT_FAILURE;
    }
#endif
#endif

    return EXIT_SUCCESS;
}
