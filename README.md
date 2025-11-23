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


# The Algorithm

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


# Visualisations

## Even-Three

[![Even Three Rotation](https://img.youtube.com/vi/VLeCVCcSNPU/0.jpg)](https://youtu.be/VLeCVCcSNPU?si=IKMUrxyoeJwP5l0m)

## OverLapping

[![Overlapping Rotation](https://img.youtube.com/vi/svkgX6XqhFc/0.jpg)](https://youtu.be/svkgX6XqhFc?si=_wNxaKBUncX4OiLV)

## No Overlap

[![No Overlap Rotation](https://img.youtube.com/vi/9LZZ1DgFdK0/0.jpg)](https://youtu.be/9LZZ1DgFdK0?si=raTo8O9zOXQXEgdB)

# RESULTS

Below are the results for all of the algorithms included.  Item size used is 32-bit integers,

The tests were performed on an AMD 9800X3D CPU, running Fedora 43, with 96GB of DDR5 memory at 6000MHz

`gcc version 15.2.1` was the C compiler, and the test utility was compiled with: `cc -O3 -o rotate rotate.c`

Triple Shift Rotate is seen to hold a strong and clear lead over all other rotation algorithms that do not allocate an auxiliary buffer space,
and it even manages to outperform even those for small set sizes, and closes the gap on them for very large set sizes.

With the smallest array sizes it is a touch slower than the best, which is `Drill Rotation`.  From about 18 items and up
it starts to pull a clear lead over the rest of the algorithms.

In my own testing I also observed that in the ranges from around 2000-8000 items the algorithm hits something of a rough patch where
it will perform a little worse than usual, and this can be seen in the 5000 item result listed below.

It does still post the best, or close to the best, of all other non-auxiliary completion times while in the "rough" range though.
I suspect that this is an CPU architecture specific anomaly from crossing a cache boundary somewhere on the 9800X3D which is
exacerbated by the algorithm's three-way swapping.  Once past this "rough" range however it returns to holding a significant lead.

```
=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation                10             5.023ns
Gries-Mills Rotation             10             6.461ns
Piston Rotation                  10             5.547ns
Grail Rotation                   10             6.007ns
Old Forsort Rotation             10             5.348ns *
Helix Rotation                   10             4.486ns
Drill Rotation                   10             4.345ns
Triple-Reverse Rotate            10             7.166ns
ContRev Rotation                 10             5.022ns
Trinity Rotation                 10             4.791ns
Triple Shift Rotate              10             5.084ns **
Aux Rotation (N/2 Aux)           10             6.881ns
Bridge Rotate (N/3 Aux)          10             7.452ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation                50            17.573ns
Gries-Mills Rotation             50            16.610ns
Piston Rotation                  50            14.318ns
Grail Rotation                   50            14.462ns
Old Forsort Rotation             50            14.181ns *
Helix Rotation                   50            16.539ns
Drill Rotation                   50            16.196ns
Triple-Reverse Rotate            50             9.494ns
ContRev Rotation                 50            11.659ns
Trinity Rotation                 50             8.766ns
Triple Shift Rotate              50             6.793ns **
Aux Rotation (N/2 Aux)           50             8.527ns
Bridge Rotate (N/3 Aux)          50             8.427ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation               100            31.202ns
Gries-Mills Rotation            100            22.535ns
Piston Rotation                 100            19.453ns
Grail Rotation                  100            20.833ns
Old Forsort Rotation            100            19.055ns *
Helix Rotation                  100            24.140ns
Drill Rotation                  100            23.934ns
Triple-Reverse Rotate           100            11.694ns
ContRev Rotation                100            15.234ns
Trinity Rotation                100            12.824ns
Triple Shift Rotate             100             9.177ns **
Aux Rotation (N/2 Aux)          100            13.676ns
Bridge Rotate (N/3 Aux)         100            11.857ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation               500           163.998ns
Gries-Mills Rotation            500            66.037ns
Piston Rotation                 500            56.985ns
Grail Rotation                  500            62.583ns
Old Forsort Rotation            500            51.677ns *
Helix Rotation                  500            67.274ns
Drill Rotation                  500            63.436ns
Triple-Reverse Rotate           500            46.994ns
ContRev Rotation                500            35.708ns
Trinity Rotation                500            34.125ns
Triple Shift Rotate             500            27.760ns **
Aux Rotation (N/2 Aux)          500            22.452ns
Bridge Rotate (N/3 Aux)         500            21.437ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation              1000           339.952ns
Gries-Mills Rotation           1000           128.337ns
Piston Rotation                1000           118.691ns
Grail Rotation                 1000           130.115ns
Old Forsort Rotation           1000           110.647ns *
Helix Rotation                 1000           122.555ns
Drill Rotation                 1000           115.409ns
Triple-Reverse Rotate          1000            86.461ns
ContRev Rotation               1000            79.050ns
Trinity Rotation               1000            74.528ns
Triple Shift Rotate            1000            62.657ns **
Aux Rotation (N/2 Aux)         1000            31.905ns
Bridge Rotate (N/3 Aux)        1000            33.901ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation              5000          1578.675ns
Gries-Mills Rotation           5000           494.151ns
Piston Rotation                5000           477.446ns
Grail Rotation                 5000           484.842ns
Old Forsort Rotation           5000           458.682ns *
Helix Rotation                 5000           429.145ns
Drill Rotation                 5000           420.709ns
Triple-Reverse Rotate          5000           389.436ns
ContRev Rotation               5000           343.692ns
Trinity Rotation               5000           332.919ns
Triple Shift Rotate            5000           343.589ns **
Aux Rotation (N/2 Aux)         5000           107.252ns
Bridge Rotate (N/3 Aux)        5000           125.921ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation             10000          3243.701ns
Gries-Mills Rotation          10000           899.104ns
Piston Rotation               10000           874.534ns
Grail Rotation                10000           878.997ns
Old Forsort Rotation          10000           837.042ns *
Helix Rotation                10000           789.884ns
Drill Rotation                10000           771.447ns
Triple-Reverse Rotate         10000           744.597ns
ContRev Rotation              10000           652.705ns
Trinity Rotation              10000           634.697ns
Triple Shift Rotate           10000           626.014ns **
Aux Rotation (N/2 Aux)        10000           237.388ns
Bridge Rotate (N/3 Aux)       10000           252.574ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation             50000         18319.853ns
Gries-Mills Rotation          50000          3973.161ns
Piston Rotation               50000          3878.596ns
Grail Rotation                50000          3870.903ns
Old Forsort Rotation          50000          3748.275ns *
Helix Rotation                50000          3618.986ns
Drill Rotation                50000          3635.094ns
Triple-Reverse Rotate         50000          3660.954ns
ContRev Rotation              50000          3199.648ns
Trinity Rotation              50000          3178.132ns
Triple Shift Rotate           50000          2950.690ns **
Aux Rotation (N/2 Aux)        50000          2071.971ns
Bridge Rotate (N/3 Aux)       50000          1905.802ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation            100000         37221.580ns
Gries-Mills Rotation         100000          7854.443ns
Piston Rotation              100000          7695.984ns
Grail Rotation               100000          7610.874ns
Old Forsort Rotation         100000          7390.227ns *
Helix Rotation               100000          7153.446ns
Drill Rotation               100000          7141.109ns
Triple-Reverse Rotate        100000          7382.198ns
ContRev Rotation             100000          6467.977ns
Trinity Rotation             100000          6426.485ns
Triple Shift Rotate          100000          5910.248ns **
Aux Rotation (N/2 Aux)       100000          3600.972ns
Bridge Rotate (N/3 Aux)      100000          3559.232ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation            500000        203656.559ns
Gries-Mills Rotation         500000         40106.277ns
Piston Rotation              500000         40076.753ns
Grail Rotation               500000         39835.602ns
Old Forsort Rotation         500000         39621.104ns *
Helix Rotation               500000         37820.957ns
Drill Rotation               500000         37642.651ns
Triple-Reverse Rotate        500000         39881.265ns
ContRev Rotation             500000         33779.788ns
Trinity Rotation             500000         33824.463ns
Triple Shift Rotate          500000         32142.761ns **
Aux Rotation (N/2 Aux)       500000         32639.573ns
Bridge Rotate (N/3 Aux)      500000         28253.924ns

=======================================================
         NAME                 ITEMS     TIME/ROTATE (s)
=======================================================
Juggling Rotation           1000000        434340.810ns
Gries-Mills Rotation        1000000         78918.004ns
Piston Rotation             1000000         78353.930ns
Grail Rotation              1000000         78512.982ns
Old Forsort Rotation        1000000         77734.205ns *
Helix Rotation              1000000         75833.244ns
Drill Rotation              1000000         75174.838ns
Triple-Reverse Rotate       1000000         79787.879ns
ContRev Rotation            1000000         68071.345ns
Trinity Rotation            1000000         68369.475ns
Triple Shift Rotate         1000000         63237.759ns **
Aux Rotation (N/2 Aux)      1000000         65060.416ns
Bridge Rotate (N/3 Aux)     1000000         57945.938ns
```
