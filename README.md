# Individual-based Model for the Evolution of Movement Rules and the Structure of Emergent Social Networks

This is a small `Rcpp` model tying together a number of interesting tools to examine the evolution of animal movement rules in the context of exploitation competition and infectious pathogen transmission, and the consequences of evolved movement rules on the structure of emergent proximity-based animal social networks.

## Running the model

The model is bundled as an R package, which means it can be installed and run out-of-the-box on most systems with minimal effort.
The actual model code is written in C++; users need not interact with this code.

### Pre-requisites

The package depends on the following R packages, which are installed by default alongside it.

1. `Rcpp` to link R and C++, and to export simulation data directly as R lists and data.frames.

2. `RcppParallel` for multi-threading using Intel's Thread Building Blocks (TBB).

3. `BH` to make the Boost.Geometry headers available to the Rcpp package. The package _does not_ explicitly link to system Boost installations (though this is possible).

4. `igraph` to calculate social network metrics within the C++ simulation.

### RcppParallel on Windows

The contents of the `src/Makevars.win` script need to be copied to the `Makevars.win` script for your local installation of R on Windows, usually `Documents/.R/`.
This helps the package find the TBB libraries provided by RcppParallel.
This is not necessary on Linux systems.

### Installation

1. Clone the repository using SSH by running `git clone git@github.com:pratikunterwegs/snevo.git`

2. In `R`, build the package using `devtools::build()`

3. In `R`, install the package using `devtools::install()`

4. Try out the model using the script `scripts/chk_pkg_install.R`

## Fast distance calculations using Boost Rtrees

Distances between agents and between agents and food items are calculated using [`boost` Rtrees](https://www.boost.org/doc/libs/1_76_0/libs/geometry/doc/html/geometry/spatial_indexes.html).


## Simulations launched from, and data returned to, `R`

The simulations are disguised as easy-to-use `R` functions, using `Rcpp` to obscure the scary `C++` code that lies beneath. The simulation data are also returned to `R`, as well known objects (lists and data.frames).
