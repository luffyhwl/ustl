// This file is part of the ustl library, an STL implementation.
// Copyright (c) 2003 by Mike Sharov <msharov@talentg.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the 
// Free Software Foundation, Inc., 59 Temple Place - Suite 330, 
// Boston, MA  02111-1307  USA.
//
// ulaalgo.h
//

#ifndef ULAALGO_H_2E403D182E83FB596AFB800E68B255A1
#define ULAALGO_H_2E403D182E83FB596AFB800E68B255A1

#include "umatrix.h"

namespace ustl {

template <size_t NX, size_t NY, typename T>
void load_identity (matrix<NX,NY,T>& m)
{
    fill_n (m.begin(), NX * NY, 0);
    for (typename matrix<NX,NY,T>::iterator i = m.begin(); i < m.end(); i += NX + 1)
	*i = 1;
}

template <size_t NX, size_t NY, typename T>
matrix<NY,NY,T> operator* (const matrix<NX,NY,T>& m1, const matrix<NY,NX,T>& m2)
{
    matrix<NY,NY,T> mr;
    for (uoff_t ry = 0; ry < NY; ++ ry) {
	for (uoff_t rx = 0; rx < NY; ++ rx) {
	    T dpv (0);
	    for (uoff_t x = 0; x < NX; ++ x)
		dpv += m1[ry][x] * m2[x][rx];
	    mr[ry][rx] = dpv;
	}
    }
    return (mr);
}

template <size_t NX, size_t NY, typename T>
tuple<NX,T> operator* (const tuple<NY,T>& t, const matrix<NX,NY,T>& m)
{
    tuple<NX,T> tr;
    for (uoff_t x = 0; x < NX; ++ x) {
	T dpv (0);
	for (uoff_t y = 0; y < NY; ++ y)
	    dpv += t[y] * m[y][x];
	tr[x] = dpv;
    }
    return (tr);
}

template <size_t N, typename T>
void transpose (matrix<N,N,T>& m)
{
    for (uoff_t x = 0; x < N; ++ x)
	for (uoff_t y = x; y < N; ++ y)
	    swap (m[x][y], m[y][x]);
}

#if WANT_UNROLLED_COPY

#if CPU_HAS_3DNOW

/// Specialization for 4-component vector transform, the slow part of 3D graphics.
template <>
inline tuple<4,float> operator* (const tuple<4,float>& t, const matrix<4,4,float>& m)
{
    tuple<4,float> result;
    // This is taken from "AMD Athlon Code Optimization Guide" from AMD. 18 cycles!
    // If you are writing a 3D engine, you may want to copy it instead of calling it
    // because of the femms instruction at the end, which takes 2 cycles.
    asm volatile (
	"movq	   (%0), %%mm0		\n\t"	//            y | x
	"movq	   8(%0), %%mm1		\n\t"	//            w | z
	"movq	   %%mm0, %%mm2		\n\t"	//            y | x
	"movq	   (%1), %%mm3		\n\t"	//      m[0][1] | m[0][0]
	"punpckldq  %%mm0, %%mm0	\n\t"	//            x | x
	"movq	   16(%1), %%mm4	\n\t"	//      m[1][1] | m[1][0]
	"pfmul	   %%mm0, %%mm3		\n\t"	//    x*m[0][1] | x*m[0][0]
	"punpckhdq  %%mm2, %%mm2	\n\t"	//            y | y
	"pfmul	   %%mm2, %%mm4		\n\t"	//    y*m[1][1] | y*m[1][0]
	"movq	   8(%1), %%mm5		\n\t"	//      m[0][3] | m[0][2]
	"movq	   24(%1), %%mm7	\n\t"	//      m[1][3] | m[1][2]
	"movq	   %%mm1, %%mm6		\n\t"	//            w | z
	"pfmul	   %%mm0, %%mm5		\n\t"	//    x*m[0][3] | v0>x*m[0][2]
	"movq	   32(%1), %%mm0	\n\t"	//      m[2][1] | m[2][0]
	"punpckldq  %%mm1, %%mm1	\n\t"	//            z | z
	"pfmul	   %%mm2, %%mm7		\n\t"	//    y*m[1][3] | y*m[1][2]
	"movq	   40(%1), %%mm2	\n\t"	//      m[2][3] | m[2][2]
	"pfmul	   %%mm1, %%mm0		\n\t"	//    z*m[2][1] | z*m[2][0]
	"pfadd	   %%mm4, %%mm3		\n\t"	// x*m[0][1]+y*m[1][1] | x*m[0][0]+y*m[1][0]
	"movq	   48(%1), %%mm4	\n\t"	//      m[3][1] | m[3][0]
	"pfmul	   %%mm1, %%mm2		\n\t"	//    z*m[2][3] | z*m[2][2]
	"pfadd	   %%mm7, %%mm5		\n\t"	// x*m[0][3]+y*m[1][3] | x*m[0][2]+y*m[1][2]
	"movq	   56(%1), %%mm1	\n\t"	//      m[3][3] | m[3][2]
	"punpckhdq  %%mm6, %%mm6	\n\t"	//            w | w
	"pfadd	   %%mm0, %%mm3		\n\t"	// x*m[0][1]+y*m[1][1]+z*m[2][1] | x*m[0][0]+y*m[1][0]+z*m[2][0]
	"pfmul	   %%mm6, %%mm4		\n\t"	//    w*m[3][1] | w*m[3][0]
	"pfmul	   %%mm6, %%mm1		\n\t"	//    w*m[3][3] | w*m[3][2]
	"pfadd	   %%mm2, %%mm5		\n\t"	// x*m[0][3]+y*m[1][3]+z*m[2][3] | x*m[0][2]+y*m[1][2]+z*m[2][2]
	"pfadd	   %%mm4, %%mm3		\n\t"	// x*m[0][1]+y*m[1][1]+z*m[2][1]+w*m[3][1] | x*m[0][0]+y*m[1][0]+z*m[2][0]+w*m[3][0]
	"movq	   %%mm3, (%2)		\n\t"	// store result->y | result->x
	"pfadd	   %%mm1, %%mm5		\n\t"	// x*m[0][3]+y*m[1][3]+z*m[2][3]+w*m[3][3] | x*m[0][2]+y*m[1][2]+z*m[2][2]+w*m[3][2]
	"movq	   %%mm5, 8(%2)		\n\t"	// store result->w | result->z
	"femms				    "	// clear mmx state
	:
	: "r"(t.begin()), "r"(m.begin()), "r"(result.begin())
	: "memory"
    );
    return (result);
}

#else	// If no processor extensions, just unroll the multiplication

/// Specialization for 4-component vector transform, the slow part of 3D graphics.
template <>
inline tuple<4,float> operator* (const tuple<4,float>& t, const matrix<4,4,float>& m)
{
    tuple<4,float> tr;
    tr[0] = t[0] * m[0][0] + t[1] * m[1][0] + t[2] * m[2][0] + t[3] * m[3][0];
    tr[1] = t[0] * m[0][1] + t[1] * m[1][1] + t[2] * m[2][1] + t[3] * m[3][1];
    tr[2] = t[0] * m[0][2] + t[1] * m[1][2] + t[2] * m[2][2] + t[3] * m[3][2];
    tr[3] = t[0] * m[0][3] + t[1] * m[1][3] + t[2] * m[2][3] + t[3] * m[3][3];
    return (tr);
}

#endif	// CPU_HAS_3DNOW
#endif	// WANT_UNROLLED_COPY

} // namespace ustl

#endif
