#!/bin/bash

ml load R/4.0.0-foss-2020a
ml load Boost/1.72.0-foss-2019a
Rscript --slave -e 'devtools::build()'
Rscript --slave -e 'devtools::install()'
