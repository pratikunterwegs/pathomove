# Source code for _Pathomove_, an individual-based model for the evolution of animal movement strategies under the risk of pathogen transmission

<!-- badges: start -->
  [![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)
  [![R-CMD-check](https://github.com/pratikunterwegs/pathomove/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/pratikunterwegs/pathomove/actions/workflows/R-CMD-check.yaml)
  [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.6331816.svg)](https://doi.org/10.5281/zenodo.6331816)
  [![Codecov test coverage](https://codecov.io/gh/pratikunterwegs/pathomove/branch/main/graph/badge.svg)](https://codecov.io/gh/pratikunterwegs/pathomove?branch=main)
<!-- badges: end -->


This repository holds the source code for the _Pathomove_ simulation, a spatially explicit, individual-based, evolutionary model of the evolution of animal social movement strategies under the risk of pathogen transmission.

The model is written by Pratik Gupte, in the Modelling Adaptive Response Mechanisms Group (Weissing Lab) at the Groningen Institute for Evolutionary Life Science, at the University of Groningen.

The source code for analyses of this simulation's output can be found on Github at https://github.com/pratikunterwegs/patho-move-evol, from where a link to an archived version on Zenodo should be available.

## Contact and Attribution

Please contact [Pratik Gupte](p.r.gupte@rug.nl) for questions on the model or the associated project.

```md
Name: Pratik Rajan Gupte
Email: pratikgupte16@gmail.com OR p.r.gupte@rug.nl
ORCID: https://orcid.org/0000-0001-5294-7819
```

Please cite this simulation model as Pratik Gupte. (2022). Source code for Pathomove, an individual-based model for the evolution of animal movement strategies under the risk of pathogen transmission (v1.0.2). Zenodo. https://doi.org/10.5281/zenodo.6331816

```bibtex
@software{pratik_gupte_2022_6331816,
  author       = {Pratik Gupte},
  title        = {{Source code for Pathomove, an individual-based 
                   model for the evolution of animal movement
                   strategies under the risk of pathogen transmission}},
  month        = mar,
  year         = 2022,
  publisher    = {Zenodo},
  version      = {v1.0.2},
  doi          = {10.5281/zenodo.6331816},
  url          = {https://doi.org/10.5281/zenodo.6331816}
}
```

## Simulation model

Please refer to the preprint on _biorXiv_ for a full description of the model, and the biological system it aims to simulate.

This model ties together a number of different concepts:

1. Mechanistic modelling of the evolution of animal movement decisions, following a framework from an earlier model [(Gupte et al. 2021)](https://www.biorxiv.org/content/10.1101/2021.07.19.452886v2).

2. Exploitation competition for discrete food items distributed in continuous space, rather than on a grid.

3. The introduction and spread of an infectious pathogen between agents when they are close together. The pathogen causes a chronic 'disease', which reduces net energy, and hence fitness.

<img src="inst/fig_schematic.png" height="200">

---

## Simulation methods

The model combines a number of interesting tools to implement its conceptual components:

1. **Familiarity in R, speed in C++** The simulation is written in C++, but disguised as easy-to-use `R` functions. `Rcpp` is used to link the two. The model runs with a single function, `run_pathomove`.

2. **R functions return R objects** Simulation results from `run_pathomove` are returned to `R` as well known objects (lists and data.frames).

3. **Fast and efficient distance calculations** Distances between agent pairs, and between agents and food items, are calculated many hundreds of thousands of times, using [`boost` Rtrees](https://www.boost.org/doc/libs/1_76_0/libs/geometry/doc/html/geometry/spatial_indexes.html).

4. **Speed boosts using TBB multi-threading** Internal simulation functions are sped up using Intel's Thread Building Blocks (TBB) library, which is conveniently included with `RcppParallel`, making it cross-platform.

5. **Testing Rcpp functions** The internal C++ functions underlying the main simulation code (`run_pathomove`) are tested (some!) using Catch testing, which is integrated with the R package `testthat`.

---

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

**Copy** this to Documents/.R/Makevars.win **on Windows systems only**

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

### Usage on different systems

- Linux and Windows: This package is confirmed to work on both Linux (Ubuntu 20.04+) and Windows (10) systems. This functionality is checked weekly using a Github Actions 'job', the details of which can be found in `.github/workflows/R-CMD-check.yaml`.

- Multi-threading: This package uses Intel's TBB library for multi-threading, which substantially improves the speed of the underlying C++ code. This is especially noticeable when running large population sizes, or many generations. This functionality is confirmed to work on both Windows and Linux systems, as above.

  Multi-threading can be turned on for the lone function that uses it, `pathomove::run_pathomove`, by setting the `multithreaded` argument to `TRUE`.

- High-performance computing clusters: The installation of this package on an Ubuntu-based HPC cluster can be automated by running the shell script provided in the `bash/` folder. The example below shows how to install it on the University of Groningen's HPC cluster.

  ```sh
  #!/bin/bash
  # script to install pathomove on the peregrine cluster

  ml load R/4.1.0-foss-2021a
  ml load Boost/1.76.0-GCC-10.3.0
  ml load tbb/4.4.2.152

  # here working in R
  Rscript --slave -e 'devtools::build()'
  Rscript --slave -e 'sink("install_log.log"); devtools::install(upgrade = "never"); sink()'
  ```

  An example of a template job script is provided as `bash/main_job_maker.sh`.

- Mass job submission to an HPC cluster: The function `use_cluster` in `R/fun_use_cluster.R` can be used to run multiple replicates of this simulation, or multiple parameter combinations; *please use this advanced functionality carefully.*

- Multi-threading caveat for high-performance computing clusters: When using (an Ubuntu-based) HPC cluster, multi-threading may not work, even when the cluster has TBB available and loaded. It is not entirely clear why. When using an HPC cluster, set `multithreaded = FALSE`, to use single-threaded alternatives of multi-threaded functions.

- MacOS: This package likely does _not_ work on MacOS. This is related to using Intel's TBB library for multi-threading. Users can try to use the single-threaded option at their own risk.

---

## Package documentation

Each function in the package is documented, and this can be accessed through R help, once the package is installed.

```r
?pathomove::run_pathomove()
```

Alternatively, build the package manual --- a PDF version of the documentation --- after installing the package. A pre-built version of the documentation is provided among the supplementary files in the associated _biorXiv_ submission.

```r
devtools::build_manual(pkg = "pathomove")
```

---

## Workflow

The workflow to run this model to replicate the results presented in our _biorXiv_ manuscript are described more thoroughly in the Readme of a dedicated repository, https://github.com/pratikunterwegs/patho-move-evol.

A basic working example of how to use this package can be found in the script in the `vignettes` directory, `vignettes/basic_usage.Rmd`.

The basic workflow for the package is:

### Local use

1. Install the package.

2. Run the following commands.

```r
# run a single replicate with a single combination of parameters
pathomove::run_pathomove(..., multithreaded = TRUE)
```

Here, '`...`' indicates the many function arguments, such as population size, landscape size, the number of generations, and when the pathogen is introduced.

`multithreaded` controls multi-threading to speed up the simulation, `TRUE` results in automatic use of as many threads as TBB decides internally.

### HPC cluster use

**Warnings**

Please note: This is an advanced workflow, and should _not_ be attempted lightly.

This workflow describes how to prepare a combination of parameters, and create a job array on an HPC cluster, so that a separate simulation is run for each parameter combination and replicate.

Please note: If _any_ part of this sounds unfamiliar, please stop now, and consider using the simulation locally.

**Warning** This workflow currently needs to be run from a Linux system, due to issues converting between line-ending types on Windows and Linux systems.

**Workflow**

1. Install the package locally.

2. Install the package on the cluster.

3. Prepare a directory structure to store the output. A template directory structure can be found at https://github.com/pratikunterwegs/patho-move-evol.

There should be at least the following paths:

```md
yourFolder
├───bash
├───data
│   ├───output
│   ├───parameters
└───scripts
```

4. Prepare an `R` script to actually run the `run_pathomove` command on the cluster, and to save the output. An example can be found in `scripts/do_sim_pathomove.R`. 

  - You should **prepare this locally*, it will be uploaded to the cluster.

  - Make sure that the output path to save the simulation results is in the directory structure shown above; e.g. `yourFolder/data/output`.

5. Prepare a template job. An example is found in `bash\main_job_maker.sh`. This script is written for an Ubuntu-based, SLURM-scheduler HPC cluster.

6. Run the following commands locally from `R`.

```r
# this should be your R terminal
# be careful about working directories etc.
# load the package locally
library(pathomove)

# make a parameter file with all the combinations required
# or with multiple replicates
pathomove::make_parameter_file(
  ...,
  replicates = N,
  which_file = "some parameter file name.csv"
)

# above, ... indicates the simulation parameters

# use the use_cluster function to send in jobs
pathomove::use_cluster(
  ssh_con = "ssh connection to your HPC cluster",
  password = "your HPC password", 
  script = "your simulation run script", # e.g. scripts/do_sim_pathomove.R
  folder = "yourFolder", # folder for the output
  template_job = "template job shell script",  # the shell script from (5)
  parameter_file = "some parameter file name.csv" # the parameter data
)
```

7. Simulation output should be returned as `Rds` files into the `data/output` folder specified above _on the cluster_, or your custom equivalent. Move these `Rds` files to your local system for further analysis.

Please note (again): This is advanced functionality. It is brittle, i.e., it is not tested to work across a range of systems. Please do _not_ attempt this lightly.

---

## Reference dataset

A reference dataset is available on Zenodo at https://zenodo.org/record/6331757.

Please cite the simulation data as:

```bibtex
@dataset{pratik_rajan_gupte_2022_6331757,
  author       = {Pratik Rajan Gupte},
  title        = {{Reference data from the Pathomove simulation, for 
                   the manuscript "Novel pathogen introduction
                   rapidly alters the evolution of movement,
                   restructuring animal societies"}},
  month        = mar,
  year         = 2022,
  publisher    = {Zenodo},
  version      = {v1.0},
  doi          = {10.5281/zenodo.6331757},
  url          = {https://doi.org/10.5281/zenodo.6331757}
}
```
