itk_module_test()

set(CSIROTomoTests
  itkNegLogCheckedImageFilterTest.cxx
  itkMaskedMedianImageFilterTest.cxx
  itkThresholdedMedianImageFilterTest.cxx
  itkThresholdedMedianMaskImageFilterTest.cxx
  itkVerticalStitchingImageFilterTest.cxx
  IMBLPreProcWorkflowTest.cxx
)

message( STATUS "test libs: " ${CSIROTomo-Test_LIBRARIES} )

CreateTestDriver(CSIROTomo "${CSIROTomo-Test_LIBRARIES}" "${CSIROTomoTests}")

itk_add_test(NAME itkNegLogCheckedImageFilterTest
	COMMAND CSIROTomoTestDriver itkNegLogCheckedImageFilterTest)

itk_add_test(NAME itkMaskedMedianImageFilterTest
	COMMAND CSIROTomoTestDriver
	--compare DATA{Baseline/resultMaskedMedianImageFilterTest.tif}
		${ITK_TEST_OUTPUT_DIR}/resultMaskedMedianImageFilterTest.tif
	itkMaskedMedianImageFilterTest
	DATA{Input/inputMaskedMedianImageFilterTest_image.tif}
	DATA{Input/inputMaskedMedianImageFilterTest_mask.tif}
	${ITK_TEST_OUTPUT_DIR}/resultMaskedMedianImageFilterTest.tif)

itk_add_test(NAME itkThresholdedMedianImageFilterTest
	COMMAND CSIROTomoTestDriver
	--compare DATA{Baseline/resultThresholdedMedianImageFilterTest.tif}
		${ITK_TEST_OUTPUT_DIR}/resultThresholdedMedianImageFilterTest.tif
	itkThresholdedMedianImageFilterTest
	DATA{Input/inputThresholdedMedianImageFilterTest.tif}
	${ITK_TEST_OUTPUT_DIR}/resultThresholdedMedianImageFilterTest.tif)

itk_add_test(NAME itkThresholdedMedianMaskImageFilterTest
	COMMAND CSIROTomoTestDriver
	--compare DATA{Baseline/resultThresholdedMedianMaskImageFilterTest.tif}
		${ITK_TEST_OUTPUT_DIR}/resultThresholdedMedianMaskImageFilterTest.tif
	itkThresholdedMedianMaskImageFilterTest
	DATA{Input/inputThresholdedMedianMaskImageFilterTest.tif}
	${ITK_TEST_OUTPUT_DIR}/resultThresholdedMedianMaskImageFilterTest.tif)

itk_add_test(NAME itkVerticalStitchingImageFilterTest
	COMMAND CSIROTomoTestDriver
	--compare DATA{Baseline/resultVerticalStitchingImageFilterTest.tif}
		${ITK_TEST_OUTPUT_DIR}/resultVerticalStitchingImageFilterTest.tif
	itkVerticalStitchingImageFilterTest
	DATA{Input/inputVerticalStitchingImageFilterTest_image1.tif}
	DATA{Input/inputVerticalStitchingImageFilterTest_image2.tif}
	${ITK_TEST_OUTPUT_DIR}/resultVerticalStitchingImageFilterTest.tif)

#itk_add_test(NAME IMBLPreProcWorkflowTest
#	COMMAND CSIROTomoTestDriver IMBLPreProcWorkflowTest)

