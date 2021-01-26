test_that("export landscape works", {
  
  # check function runes
  testthat::expect_silent(
    export_test_landscapes(100, 1, 100, 1)
  )
  
})
