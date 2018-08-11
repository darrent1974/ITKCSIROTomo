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
#ifndef itkThresholdedMaskImageFilter_h
#define itkThresholdedMaskImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkProgressReporter.h"

namespace itk
{
/** \class ThresholdedMaskImageFilter
 *
 * \brief stub.
 *
 *
 * \note This class has not yet been implemented
 *
 * \ingroup ITKCSIROTomo
 */

	template< typename TImage >
	class ITK_TEMPLATE_EXPORT ThresholdedMaskImageFilter : public ImageToImageFilter< TImage, TImage >
	{
	public:
		typedef ThresholdedMaskImageFilter                  Self;
		typedef ImageToImageFilter< TImage, TImage >        Superclass;
		typedef SmartPointer< Self >                        Pointer;
		typedef SmartPointer< const Self >                  ConstPointer;

		itkStaticConstMacro( ImageDimension, unsigned int, TImage::ImageDimension );

		itkNewMacro(Self);
		itkTypeMacro(ThresholdedMaskImageFilter, ImageToImageFilter);

	#ifdef ITK_USE_CONCEPT_CHECKING
		itkConceptMacro( FloatingPointPixel, ( itk::Concept::IsFloatingPoint< typename TImage::PixelType > ) );
	#endif

		itkSetMacro( ThresholdLower, double );
		itkGetConstMacro( ThresholdLower, double );
		itkSetMacro( ThresholdUpper, double );
		itkGetConstMacro( ThresholdUpper, double );

	protected:
		ThresholdedMaskImageFilter();
		virtual ~ThresholdedMaskImageFilter() ITK_OVERRIDE {}

		void PrintSelf( std::ostream& os, Indent indent ) const ITK_OVERRIDE;

		void GenerateData() ITK_OVERRIDE;
	private:
		ITK_DISALLOW_COPY_AND_ASSIGN(ThresholdedMaskImageFilter);

		double                      m_ThresholdLower;
		double                      m_ThresholdUpper;

	};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkThresholdedMaskImageFilter.hxx"
#endif

#endif // itkThresholdedMaskImageFilter_h
