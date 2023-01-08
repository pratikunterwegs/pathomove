#!/bin/bash
# script to install pathomove on the peregrine cluster

ml load R/4.1.0-foss-2021a
ml load Boost/1.76.0-GCC-10.3.0
ml load tbb/4.4.2.152

# here working in R
Rscript --slave -e 'sink("install_log.log"); devtools::install_github("pratikunterwegs/pathomove", upgrade = "never"); sink()'
