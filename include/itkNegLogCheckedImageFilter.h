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
#ifndef itkNegLogCheckedImageFilter_h
#define itkNegLogCheckedImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkProgressReporter.h"

namespace itk
{
/** \class NegLogCheckedImageFilter
 *
 * \brief stub.
 *
 *
 * \note This class has not yet been implemented
 *
 * \ingroup CSIROTo
 */

template< typename TImage >
class ITK_TEMPLATE_EXPORT NegLogCheckedImageFilter : public ImageToImageFilter< TImage, TImage >
{
public:
#ifdef TEMP_REMOVED
    using Self = NegLogCheckedImageFilter;
    using Superclass = ImageToImageFilter< TImage, TImage >;
    using Pointer = SmartPointer<Self>;
    using ConstPointer = SmartPointer<const Self>;
#else
    typedef NegLogCheckedImageFilter                    Self;
    typedef ImageToImageFilter< TImage, TImage >        Superclass;
    typedef SmartPointer< Self >                        Pointer;
    typedef SmartPointer< const Self >                  ConstPointer;
#endif

    itkStaticConstMacro( ImageDimension, unsigned int, TImage::ImageDimension );

    itkNewMacro(Self);
    itkTypeMacro(NegLogCheckedImageFilter, ImageToImageFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
    itkConceptMacro( FloatingPointPixel, ( itk::Concept::IsFloatingPoint< typename TImage::PixelType > ) );
#endif

protected:
    NegLogCheckedImageFilter();
    virtual ~NegLogCheckedImageFilter() {}

    void PrintSelf( std::ostream& os, Indent indent ) const ITK_OVERRIDE;
    virtual void GenerateData() ITK_OVERRIDE;
private:
    ITK_DISALLOW_COPY_AND_ASSIGN(NegLogCheckedImageFilter);

};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNegLogCheckedImageFilter.hxx"
#endif

#endif // itkNegLogCheckedImageFilter_h
