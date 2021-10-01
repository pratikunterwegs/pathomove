#!/bin/bash

ml load R/4.0.0-foss-2020a
ml load Boost/1.72.0-foss-2019a
ml load tbb/2020.3-GCCcore-10.2.0
Rscript --slave -e 'devtools::build()'
Rscript --slave -e 'sink("install_log.log"); devtools::install(); sink()'
