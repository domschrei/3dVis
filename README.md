
# 3dVis

This is a fork of Carsten Sinz' 3D SAT formula visualization tool, with the intent of making it a bit handier to use.

## Building

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make -j
cd ..
```

## Usage

```bash
cnf3d2 -f $cnf # basic usage
cat $cnf | cnf3d2 # read data from stdin
cnf3d2 --space-grid 0 --random-seed 42 -f $cnf # disable space grid (more expensive!), seed random initial layout
cnf3d2 --color-scene 000 --color-vertex f008 # black background, red semi-transparent vertices
cnf3d2 -h  # show all program options
```

OpenMP is used to parallelize the force-directed layout computation. You can overwrite `OMP_NUM_THREADS` to control the degree of parallelism. For example: 

```bash
OMP_NUM_THREADS=8 cnf3d2 -f $cnf
```

When the graphics display appears (and has the focus)
the following commands may be used:

- 'n': display next (finer) graph
- 'e': toggle edge display
- 'a': toggle node size adjustment
- '2': toggle 2-clauses (only for graph with L=0)
- 'g': go; compute all graphs down to level L=0
- 'r': reset display (zoom, orientation, translation)
- 'x': show x, y, and z axes
- 'p': save picture of current view to `out-%i.png`

You can use the mouse to
- **rotate** (with the left mouse button pressed)
- **zoom**   (with the scrolling wheel OR with ctrl + left mouse)
- **drag**   (with shift + left mouse)

## About

This version uses the following freely licensed dependencies:

* [arcball code by Brad Smith](http://rainwarrior.ca/dragon/arcball.html) to make rotations more natural
* [CLI11](https://github.com/CLIUtils/CLI11) for program options
* [stb_image_write](https://github.com/nothings/stb/blob/master/stb_image_write.h) for saving images

(C) Carsten Sinz, JKU Linz, 2006
(C) Dominik Schreiber 2023
