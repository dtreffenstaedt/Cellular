# Cellular Automat

depends on SDL2.
It could use some parallelisation especially for slower computers.

To compile, type

```
make
```

To run, type:

```
./cellular [factor]
```

The argument determines how much each cell follows the average around itself. A 1 means that with each step the cell takes on the average value of it's surroundings, a negative value means it strays further from the average.

Values between 0.01 and 1.6 seem to be the most interesting.

The starting values are being randomly generated, to try specific patterns, the 'init' method would have to be modified.
