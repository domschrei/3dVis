
# 3dVis

Build:
```bash
make
```

Usage:

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
- 'r': reset display (zoom, orientation,...)

You can use the mouse to
- rotate (with the left mouse button pressed)
- zoom   (with the middle mouse button pressed)
- drag   (with the right mouse button pressed)

(C) Carsten Sinz, JKU Linz, 2006  
Forked by D. Schreiber 2023
