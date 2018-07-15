itk_module_test()

set(RingArtifactTests
  itkNegLogCheckedImageFilterTest.cxx
  )

CreateTestDriver(RingArtifact "${RingArtifact-Test_LIBRARIES}" "${RingArtifactTests}")

  itk_add_test(NAME itkNegLogCheckedImageFilterTest
    COMMAND RingArtifactTestDriver itkNegLogCheckedImageFilterTest)
