
# 3dVis

This is a fork of Carsten Sinz' 3D SAT formula visualization tool, with the intent of making it a bit handier to use.

## Building

Build:
```bash
make
```

## Usage

```bash
cnf3d2 -f $cnf # basic usage
cat $cnf | cnf3d2 # read data from stdin
cnf3d2 --space-grid 0 --random-seed 42 -f $cnf # disable space grid (more expensive!), seed random initial layout
cnf3d2 --color-scene 000 --color-vertex f008 # black background, red semi-transparent vertices
cnf3d2 -h  # show all program options
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

You can use the mouse to
- **rotate** (with the left mouse button pressed)
- **zoom**   (with the scrolling wheel OR with ctrl + left mouse)
- **drag**   (with shift + left mouse)

## About

This version uses freely licensed [arcball code by Brad Smith](http://rainwarrior.ca/dragon/arcball.html) to make rotations more natural.

(C) Carsten Sinz, JKU Linz, 2006
(C) Dominik Schreiber 2023
