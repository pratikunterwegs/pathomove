test_that("multiplication works", {
  
  # check file exists
  testthat::expect_output_file(
    export_test_landscapes(100, 1, 100, 1),
    file = "data/test_landscape/"
  )
  
})
