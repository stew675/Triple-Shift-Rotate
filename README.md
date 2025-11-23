# Triple-Shift-Rotation

I present a very fast block rotation algorithm for general use

This algorithm came about as a result of work on my [ForSort](https://github.com/stew675/ForSort) Sorting Algorithm.

Included is a copy of [Scandum's](https://github.com/scandum) library of rotation algorithms which
can be found [here](https://github.com/scandum/rotate).

Provided is a test harness utility that can be compiled like so:

```cc -O3 -o rotate rotate.c```

and then run via:

```./rotate```

This utility will run multiple iterations of rotations upon arrays of varying sizes across a selection of rotation algorithms.

Unlike Scandum's test utility, my utility does not focus on corner cases, but instead measures the average time taken to
rotate an array using the left size from: `1..(N-1)` for an array of N items.  This, IMO, provides a clearer indication of
the general performance of each algorithm.

By all means though, do use Scandum's bench test as well, to sample corner case performances.  Scandum's bench utility is
not included here, but can be found at his repository linked above.


## The Algorithm

The Triple-Shift-Rotation algorithm is, itself, a derivation of the Gries-Mills progressive rotation algorithm that many
rotation algorithm variants are based upon, but Triple-Shift adds in the notion of 3-way swaps.  A 3-way swap is, in fact,
merely a left-rotate by 1 item across a set of 3 items.  There are two modes of operation, depending on if the lists have
any overlap, or not.

The first mode is what I call overflow mode, where the smaller array, after rotating, partially overlaps a portion of the
second array.  This occurs when two arrays are of a similar size.


Let's walk through an `overflow` path:

```
|---|---|---|---|---||---|---|---|---|---|----|----|----|
| 0 | 1 | 2 | 3 | 4 || 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
|---|---|---|---|---||---|---|---|---|---|----|----|----|

First we caclulate the size of the overlap by subtracting the number of items in the smaller block, from the number of
items in the larger block.
```


Now, let's walk through a `remainder` path:
