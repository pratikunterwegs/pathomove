% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/fun_process_networks.R
\name{get_transmission_chain}
\alias{get_transmission_chain}
\title{Get pathogen transmission chains}
\usage{
get_transmission_chain(df)
}
\arguments{
\item{df}{A dataframe with individual data, typically part of a
\code{pathomove_output} object.}
}
\value{
A \code{tidygraph} object with a directed network showing which individual
was infected by which other individual, for all infected individuals, except
the index cases (initial infections).
}
\description{
Get pathogen transmission chains
}
