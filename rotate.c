/*
               Copyright (C) 2025 Stew Forster stew675@gmail.com
*/

/*
        Permission is hereby granted, free of charge, to any person obtaining
        a copy of this software and associated documentation files (the
        "Software"), to deal in the Software without restriction, including
        without limitation the rights to use, copy, modify, merge, publish,
        distribute, sublicense, and/or sell copies of the Software, and to
        permit persons to whom the Software is furnished to do so, subject to
        the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
        EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
        MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
        IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
        CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
        TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
        SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

#include "rotate.h"
#include "triple-shift-rotate.h"

typedef void rotate_function(int *array, size_t left, size_t right);

typedef struct {
	rotate_function		*rotate;
	char			*name;
} rotate_function_t;

rotate_function_t rotations[] = {
//	{juggling_rotation,       "Juggling Rotation"},
//	{griesmills_rotation,     "Gries-Mills Rotation"},
//	{piston_rotation,         "Piston Rotation"},
//	{grail_rotation,          "Grail Rotation"},
//	{old_forsort_rotate,      "Old Forsort Rotation"},
//	{helix_rotation,          "Helix Rotation"},
//	{drill_rotation,          "Drill Rotation"},
//	{reversal_rotation,       "Triple-Reverse Rotate"},
	{contrev_rotation,        "ContRev Rotation"},
	{trinity_rotation,        "Trinity Rotation"},
//	{half_reverse_rotate,     "Half Reverse Rotate"},
	{triple_shift_rotate,     "Triple Shift Rotate"},
	{triple_shift_rotate_v2,  "Triple Shift Rotate V2"},
//	{auxiliary_rotation,      "Aux Rotation (N/2 Aux)"},
//	{bridge_rotation,         "Bridge Rotate (N/3 Aux)"},
	{NULL,                    "End Of List"}
};


// Simple function to safely return an entry from the rotations[] table
rotate_function_t *
get_function(int i)
{
	if (i < 0)
		return NULL;

	if (i > (sizeof(rotations) / sizeof(*rotations)))
		return NULL;

	if (rotations[i].rotate == NULL)
		return NULL;

	return rotations + i;
} // get_function 


// Feel free to exit this to set whatever sizes you want to test
size_t	test_steps[] = {10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000};
//size_t	test_steps[] = {2500, 3750, 5000, 6250, 7500, 8750, 10000, 12500};

#define MAX_TIME	50000000000ULL
#define	MAX_VALS	2000000

int
main()
{
	struct	timespec start, end;
	int32_t	*a;

	a = malloc(sizeof(*a) * MAX_VALS);
	if (!a) {
		printf("malloc() failure\n");
		exit(1);
	}
	for (size_t i = 0; i < MAX_VALS; i++)
		a[i] = i;

	for (size_t step = 0; step < (sizeof(test_steps) / sizeof(*test_steps)); step++) {
		size_t	SZ = test_steps[step];

		if (SZ > MAX_VALS)
			continue;

		printf("\n");
		printf("=======================================================\n");
		printf("         NAME                 ITEMS         TIME/ROTATE\n");
		printf("=======================================================\n");

		for (int fno = 0; ; fno++) {
			rotate_function_t *f = get_function(fno);
			if (f == NULL)
				break;

			// Determine the number of loops we will do
			size_t	stop = MAX_TIME / (SZ * SZ);

			// The following applies a fudge factor to speed up the
			// testing of very small arrays
			size_t	div = 400 / SZ;
			if (div < 1)
				div = 1;

			// Apply the fudge factor, and apply a minimum of 1
			stop /= div;
			if (stop < 1)
				stop = 1;

			// Let's run this thing!
			size_t	runs = 0;
			clock_gettime(CLOCK_MONOTONIC, &start);

			// Very large sets take a long time to do every single
			// rotation size.  The following starts skipping sizes
			// for tests over 100,000 elements in size
			size_t gap = 1;
			if (SZ > (100 * 1000))
				gap = (SZ - 1) / (100 * 1000);

			for (size_t j = 0; j < stop; j++) {
				for (size_t i = 1; i < SZ; i += gap) {
					f->rotate(a, i, SZ - i);
					runs++;
				}
			}

			clock_gettime(CLOCK_MONOTONIC, &end);

			double tim = ((end.tv_sec - start.tv_sec) * 1000000000) + (end.tv_nsec - start.tv_nsec);
			printf("%-24s    %7lu        %10.3fns\n", f->name, SZ, tim/runs);
		}
	}

	free(a);
} // main
