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

//                           triple_shift_rotate()
//
// Author: Stew Forster (stew675@gmail.com)            Copyright (C) 2025
//
// I believe this to be a new variant of a block rotate algorithm.  Until I find
// otherwise, I'm going to name it the "Triple Shift Block Rotation" algorithm
//
// It ultimately has its roots in the successive swap Gries-Mills variant, but
// adds an improvement of a 3-way block swap.  When the blocks are close in
// size, it'll work similarly to the successive swap Gries-Mills, but instead
// of reducing the rotation space by the smaller array size per loop, it will
// instead collapse the rotation space by the larger array size.  This nets a
// small, but measurable speed boost that becomes more significant the larger
// the difference in sizes is.
//
// For blocks starting out with vastly different sizes it will collapse the
// rotation space by twice the size of the smaller array per loop.  This nets
// a significant speed boost over the regular successive swap Gries-Mills as 
// it quickly collapses the rotation space with every cycle.
//
// To work around the degenerate case of the two arrays differening by only a
// small amount, which collapses the rotation space by the smallest amount per
// cycle, the (optional) rotate_small() function is used.  rotate_small() will
// allocate space for a small number of items on the stack to copy the items
// out, and shift the memory over with memmove().  rotate_small() limits its
// stack use to 256 bytes however (or the size of 1 element, whichever is the
// larger), but rotate_small() can be disabled entirely in stack-restricted
// scenarios and this rotation algorithm will still run just fine, albeit with
// a ~20% speed penalty.

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

// I've discovered that the performance "roughness" at certain sizes is highly 
// influenced by this value.  I need to do more research on what exactly is
// going on to cause the slowness.  A higher SMALL_ROTATE_SIZE means that the
// bulk memmove() operations kick in earlier, so it's clear that the algorithm
// bogs down on the small fiddly stuff while excelling at quickly collapsing
// the operational space.  There must exist a different solution to address
// bogging down and I need to find it.  For now though, I'm raising this value
// from 16 to 64, as that almost completely solves the issue in the problematic
// 2000-8000 item ranges.
// TODO - Find a solution that doesn't require as much stack space (even if it
// is just 256 bytes).
#define SMALL_ROTATE_SIZE      64

// This is done to prevent compiler complaints if SMALL_ROTATE_SIZE is set to 0
#if (SMALL_ROTATE_SIZE > 0)
#define SMALL_BUF_SIZE SMALL_ROTATE_SIZE
#else
#define SMALL_BUF_SIZE 1
#endif

// Function prototypes
static void two_way_swap_block(int32_t * restrict pa, int32_t * restrict pe, int32_t * restrict pb);


// Completely optional function to handle degenerate scenario of rotating a
// tiny block with a (typically MUCH) larger block
static void
rotate_small(int32_t *pa, int32_t *pb, int32_t *pe)
{
	size_t	na = pb - pa, nb = pe - pb;

	int32_t	buf[SMALL_BUF_SIZE];
	int32_t	*pc = pa + nb;

	// Steps are:
	// 1.  Copy out the smaller of the two arrays into the buffer entirely
	// 2.  Move the larger of the arrays over to where the smaller was
	// 3.  Copy the smaller array data back to the hole created by the move
	if (na < nb) {
		memcpy(buf, pa, na * sizeof(*pa));
		memmove(pa, pb, nb * sizeof(*pa));
		memcpy(pc, buf, na * sizeof(*pa));
	} else {
		memcpy(buf, pb, nb * sizeof(*pa));
		memmove(pc, pa, na * sizeof(*pa));
		memcpy(pa, buf, nb * sizeof(*pa));
	}
} // rotate_small


// Uses a limited amount of stack space to rotate two blocks that overlap by
// only a small amount.  It's basically a special variant of rotate_small()
static void
rotate_overlap(int32_t *pa, int32_t *pb, int32_t *pe)
{
	size_t	na = pb - pa, nb = pe - pb;
	size_t	nc = (na < nb) ? (nb - na) : (na - nb);
	int32_t	buf[SMALL_BUF_SIZE], *pc = pa + nb, *pd = pe - nc;

	assert(nc <= SMALL_BUF_SIZE);

	if (na < nb) {
		// Steps are:
		// 1.  Copy out the overlapping amount from the end of B into the buffer
		// 2.  Swap A with B, while moving B over to the end of the array
		// 3.  Copy the buffer back to the end of where B is now
		memcpy(buf, pd, nc * sizeof(*pa));
		for (int32_t *ta = pb, *tb = pd, *tc = pe; ta > pa; *--tc = *--ta, *ta = *--tb);
//		for (pc = pb; pc > pa; *--pe = *--pc, *pc = *--pd);
		memcpy(pb, buf, nc * sizeof(*pa));
	} else {
		// Steps are:
		// 1.  Copy out the overlapping amount from the end of A into the buffer
		// 2.  Swap non-overlapping portion of A with B, and move B back to PC
		// 3.  Copy the buffer back to the end of where A now is
		memcpy(buf, pc, nc * sizeof(*pa));
		for (int32_t *ta = pa, *tb = pc, *tc = pb; tc < pe; *tb++ = *ta, *ta++ = *tc++);
//		for (pc = pc; pc < pd; *pc++ = *pa, *pa++ = *pb++);
		memcpy(pd, buf, nc * sizeof(*pa));
	}
} // rotate_overlap


static void
reverse_block(int32_t * restrict pa, int32_t * restrict pe)
{
	for (int32_t t; pa < --pe; t = *pe, *pe = *pa, *pa++ = t);
} // reverse_block


static void
reverse_and_shift(int32_t *pa, int32_t *pc, size_t na)
{
	int32_t *pb = pa + (na - 1);
	int32_t *pd = pc + (na - 1);
	int32_t	t;

	while (pa < pb) {
		t = *pa;
		*pa++ = *pd;
		*pd-- = *pb;
		*pb-- = *pc;
		*pc++ = t;
	}
	// Handle single straggler corner case
	if (pa == pb) {
		t = *pa;
		*pa = *pc;
		*pc = t;
	}
} // reverse_and_shift


static void
half_reverse_rotate(int32_t *pa, size_t na, size_t nb)
{
	int32_t	*pb = pa + na, *pc = pa + nb, *pe = pb + nb;

	if (na && nb) {
		if (na < nb) {
			if (na <= SMALL_ROTATE_SIZE)
				return rotate_small(pa, pb, pe);

			size_t nc = nb - na;

			if (nc <= SMALL_ROTATE_SIZE)
				return rotate_overlap(pa, pb, pe);

			if (pc <= (pe - nc)) {
				reverse_and_shift(pe - nc, pb, nc);
				reverse_and_shift(pa, pc, na);
				reverse_block(pa + nc, pb);
			} else {
				reverse_block(pb, pe);
				reverse_block(pb, pc);
				reverse_and_shift(pa, pc, na);
			}
		} else if (na == nb) {
			two_way_swap_block(pb, pe, pa);
		} else {
			if (nb <= SMALL_ROTATE_SIZE)
				return rotate_small(pa, pb, pe);

			size_t nc = na - nb;

			if (nc <= SMALL_ROTATE_SIZE)
				return rotate_overlap(pa, pb, pe);

			if ((pa + nc) <= pc) {
				reverse_and_shift(pa, pc, nc);
				reverse_and_shift(pb, pa, nb);
				reverse_block(pb, pe - nc);
			} else {
				reverse_block(pa, pb);
				reverse_block(pc, pb);
				reverse_and_shift(pb, pa, nb);
			}
		}
	}
} // half_reverse_rotate


// Swaps PA with PB. Terminates when PA reaches PE
static void
two_way_swap_block(int32_t * restrict pa, int32_t * restrict pe,
                   int32_t * restrict pb)
{
	while (pa < pe) {
		int32_t	t = *pa;
		*pa++ = *pb;
		*pb++ = t;
	}
} // two_way_swap_block


// When given 3 blocks of equal size, everything in B goes to A, everything
// in C goes to B, and everything in A goes to C.
static void
three_way_swap_block_negative(int32_t * restrict pa, int32_t * restrict pe,
                     int32_t * restrict pb, int32_t * restrict pc)
{
	for (int32_t t; pa > pe; ) {
		t = *--pa;
		*pa = *--pb;
		*pb = *--pc;
		*pc = t;
	}
} // three_way_swap_block_negative


// When given 3 blocks of equal size, everything in B goes to A, everything
// in C goes to B, and everything in A goes to C.
static void
three_way_swap_block_positive(int32_t * restrict pa, int32_t * restrict pe,
                     int32_t * restrict pb, int32_t * restrict pc)
{
	for (int32_t t; pa < pe; ) {
		t = *pa;
		*pa++ = *pb;
		*pb++ = *pc;
		*pc++ = t;
	}
} // three_way_swap_block_positive


static void
triple_shift_rotate_v2(int32_t *pa, size_t na, size_t nb)
{
	for (int32_t *pb = pa + na, *pe = pb + nb; na; nb = pe - pb, na = pb - pa) {
		if (na < nb) {
			if (na <= SMALL_ROTATE_SIZE)
				return rotate_small(pa, pb, pe);

			size_t	delta = nb - na;

			if (delta <= SMALL_ROTATE_SIZE)
				return rotate_overlap(pa, pb, pe);

			// Temporary A Block, B Block and Stop Pointers
			pe -= na;
			int32_t	*ta = pa, *tb = pe, *stop = pe;

			for ( ; (pb - ta) > delta; ta += delta, tb += delta)
				three_way_swap_block_positive(pb, stop, tb, ta);

			three_way_swap_block_positive(ta, pb, pb, tb);

			pa = pb;
			pb += (pb - ta);
		} else if (na == nb) {
			return two_way_swap_block(pa, pb, pb);
		} else if (nb == 0) {
			return;
		} else {
			if (nb <= SMALL_ROTATE_SIZE)
				return rotate_small(pa, pb, pe);

			size_t	delta = na - nb;

			if (delta <= SMALL_ROTATE_SIZE)
				return rotate_overlap(pa, pb, pe);

			// Temporary A Block, B Block and Stop Pointers
			pa += nb;
			int32_t	*ta = pa, *tb = pe, *stop = pa;

			for ( ; (tb - pb) > delta; ta -= delta, tb -= delta)
				three_way_swap_block_negative(pb, stop, ta, tb);

			three_way_swap_block_negative(tb, pb, pb, ta);

			pe = pb;
			pb -= (tb - pb);
		}
	}
} // triple_shift_rotate_v2


static void
triple_shift_rotate(int32_t *pa, size_t na, size_t nb)
{
	for (int32_t *pb = pa + na, *pe = pb + nb; na; ) {
		if (na < nb) {
			if (na <= SMALL_ROTATE_SIZE)
				return rotate_small(pa, pb, pe);

			size_t  nc = nb - na;

			if (nc <= SMALL_ROTATE_SIZE)
				return rotate_overlap(pa, pb, pe);

			if (nc < na) {
				// Overflow scenario
				three_way_swap_block_positive(pb - nc, pb, pb, pe - nc);
				two_way_swap_block(pa, pb - nc, pb + nc);
				na -= nc;  pe = pb;  pb -= nc;  nb = nc;
			} else {
				// Remainder scenario
				three_way_swap_block_positive(pa, pb, pb, pe - na);
				pa = pb;  pb += na;  pe -= na;  nb -= (na << 1);
			}
		} else if (na == nb) {
			return two_way_swap_block(pa, pb, pb);
		} else if (nb == 0) {
			return;
		} else {
			if (nb <= SMALL_ROTATE_SIZE)
				return rotate_small(pa, pb, pe);

			size_t  nc = na - nb;

			if (nc <= SMALL_ROTATE_SIZE)
				return rotate_overlap(pa, pb, pe);

			if (nc < nb) {
				// Overflow scenario
				three_way_swap_block_positive(pb, pb + nc, pb - nc, pa);
				two_way_swap_block(pb + nc, pe, pa + nc);

				pa = pb;  na = nc;  pb += nc;  nb -= nc;
			} else {
				// Remainder scenario
				three_way_swap_block_positive(pb, pe, pb - nb, pa);
				pe = pb;  pb -= nb;  pa += nb;  na -= (nb << 1);
			}
		}
	}
} // triple_shift_rotate


//-----------------------------------------------------------------
//                        Old Forsort
//-----------------------------------------------------------------

// The following is the old block swap algorithm used in earlier
// versions of Forsort, adapted for use with the benchmark utility
// two_way_swap_block() has been substituted in-place for the
// inline block swap code that was in the original source code
// My original source code for this algorithm can be found here:
// https://github.com/stew675/ForSort/blob/f8c7bfb2aa5ea9e7ec1e21c79a3139c5c8573b9e/include/forsort-basic.h#L44-L66
// It's remarkably short and sweet, and while it doesn't excel on
// small set sizes, it does get progressively better on larger
// set sizes.

static void
old_forsort_rotate(int32_t *pa, size_t na, size_t nb)
{
	int32_t	*pb = pa + na, *pe = pb + nb;

	while (na && nb) {
		if (na < nb) {
			two_way_swap_block(pa + nb, pe, pa);
			pe -= na;
			nb -= na;
		} else {
			two_way_swap_block(pb, pe, pa);
			pa += nb;
			na -= nb;
		}
	}
} // old_forsort_rotate


//-----------------------------------------------------------------
//                     #define cleanup
//-----------------------------------------------------------------

#undef SMALL_ROTATE_SIZE
#undef SMALL_BUF_SIZE
#undef SWAP
