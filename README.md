# Homework 4

Return the shortest path through all vertexes using a branch-and-bound
and manager/worker shared-memory algorithm.

## Source

General interfaces are contained in:

* `graph.hh`
* `path.hh`
* `tree.hh`
* `task.hh`

An implementation of these pure virtual classes are in
`Euclidean_impl.hh` and `searchtask_impl.hh`.

The main program using these implementations is in `h4.cc`.

## Compiling

This directory includes a GNU Makefile. The 'all' or 'h4' target
will compile and link the program.

## Data

Data files for both computers are in the `data/` directory.
