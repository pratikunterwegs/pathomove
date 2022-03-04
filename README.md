# Source code for _Pathomove_, an individual-based model for the evolution of animal movement strategies under the risk of pathogen transmission

<!-- badges: start -->
  [![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)
  [![R build status](https://github.com/pratikunterwegs/pathomove/workflows/R-CMD-check/badge.svg)](https://github.com/pratikunterwegs/pathomove/actions)
  [![codecov.io](https://codecov.io/github/pratikunterwegs/pathomove/coverage.svg?branch=master)](https://codecov.io/github/pratikunterwegs/pathomove/branch/master)
  <!-- [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4314905.svg)](https://doi.org/10.5281/zenodo.4033154) -->
<!-- badges: end -->

This repository holds the source code for the _Pathomove_ simulation, a spatially explicit, individual-based, evolutionary model of the evolution of animal social movement strategies under the risk of pathogen transmission.

The model is written by Pratik Gupte, in the Modelling Adaptive Response Mechanisms Group (Weissing Lab) at the Groningen Institute for Evolutionary Life Science, at the University of Groningen.

The source code for analyses of this simulation's output can be found on Github at https://github.com/pratikunterwegs/patho-move-evol, or archived on Zenodo: **Zenodo link to supplementary material here**.

## Contact and Attribution

Please contact [Pratik Gupte](p.r.gupte@rug.nl) for questions on the model or the associated project.

Please cite this simulation as

**WORK IN PROGRESS**

```bibtex
@software{gupte_2022_pathomove,
  author       = {Pratik Rajan Gupte},
  title        = {{Pathomove: Source code for an individual-based model for the 
                  evolution of animal movement strategies under the risk of 
                  pathogen transmission}},
  month        = mar,
  year         = 2022
}
```

## Simulation model

Please refer to the preprint on _biorXiv_ for a full description of the model, and the biological system it aims to simulate.

This model ties together a number of different concepts:

1. Mechanistic modelling of the evolution of animal movement decisions, following a framework from an earlier model [(Gupte et al. 2021)](https://www.biorxiv.org/content/10.1101/2021.07.19.452886v2).

2. Exploitation competition for discrete food items distributed in continuous space, rather than on a grid.

3. The introduction and spread of an infectious pathogen between agents when they are close together. The pathogen causes a chronic 'disease', which reduces net energy, and hence fitness.

<img src="inst/fig_schematic.png" height="200">

## Simulation methods and data

The model combines a number of interesting tools to implement its conceptual components:

1. **Familiarity in R, speed in C++** The simulation is written in C++, but disguised as easy-to-use `R` functions. `Rcpp` is used to link the two. The model runs with a single function, `run_pathomove`.

2. **R functions return R objects** Simulation results from `run_pathomove` are returned to `R` as well known objects (lists and data.frames).

3. **Fast and efficient distance calculations** Distances between agent pairs, and between agents and food items, are calculated many hundreds of thousands of times, using [`boost` Rtrees](https://www.boost.org/doc/libs/1_76_0/libs/geometry/doc/html/geometry/spatial_indexes.html).

4. **Speed boosts using TBB multi-threading** Internal simulation functions are sped up using Intel's Thread Building Blocks (TBB) library, which is conveniently included with `RcppParallel`, making it cross-platform.

5. **Testing Rcpp functions** The internal C++ functions underlying the main simulation code (`run_pathomove`) are tested (some!) using Catch testing, which is integrated with the R package `testthat`.

## Running the model

The model is bundled as an R package, which means it can be installed and run out-of-the-box on most systems with minimal effort.
The actual model code is written in C++; users need not interact with this code.

### Pre-requisites

The package depends on the following R packages, which are installed by default alongside it.

1. `Rcpp` to link R and C++, and to export simulation data directly as R lists and data.frames.

2. `RcppParallel` for multi-threading using Intel's Thread Building Blocks (TBB).

3. `BH` to make the Boost.Geometry headers available to the Rcpp package. The package _does not_ explicitly link to system Boost installations (though this is possible).

### RcppParallel on Windows: An Important Note

The contents of the `src/Makevars.win` script need to be copied to the `Makevars.win` script for your local installation of R on Windows, usually `Documents/.R/`.
This helps the package find the TBB libraries provided by RcppParallel.
This is not necessary on Linux systems.

**Copy** this to Documents/.R/Makevars.win **on Windows systems**

```cmake
CXX_STD = CXX14
PKG_CXXFLAGS += -DRCPP_PARALLEL_USE_TBB=1
PKG_LIBS += $(shell "${R_HOME}/bin${R_ARCH_BIN}/Rscript.exe" -e "RcppParallel::RcppParallelLibs()")
```

### Installation

1. Clone the repository using SSH by running `git clone git@github.com:pratikunterwegs/pathomove.git`.

2. In `R`, build the package using `devtools::build()`.

3. In `R`, install the package using `devtools::install()`.

Alternatively, install the model as an R package directly from `R` using the code:

```r
devtools::install_github("pratikunterwegs/pathomove")
```

4. Try out the model using the script `scripts/chk_pkg_install.R`. Be warned that specifying a large number of individuals, generations, or timesteps within generations, _will_ take a long time, and may crash on lower-capacity hardware.

5. Alternatively, run simulation replicates using the R scripts provided in the `scripts` folder on https://github.com/pratikunterwegs/patho-move-evol.

## Package documentation

Each function in the package is documented, and this can be accessed through R help, once the package is installed.

```r
?pathomove::run_pathomove()
```

Alternatively, build the package manual --- a PDF version of the documentation --- after installing the package. A pre-built version of the documentation is provided among the supplementary files in the associated biorXiv submission.

```r
devtools::build_manual(pkg = "../pathomove")
```

## Workflow

The workflow to run this model to replicate the results presented in our biorXiv manuscript are described more thoroughly in the Readme of a dedicated repository https://github.com/pratikunterwegs/patho-move-evol.
