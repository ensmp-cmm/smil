/*
 * Smil
 * Copyright (c) 2010 Matthieu Faessel
 *
 * This file is part of Smil.
 *
 * Smil is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Smil is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Smil.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */


#ifndef _BASE_LINE_OPERATIONS_HPP
#define _BASE_LINE_OPERATIONS_HPP


#include "DImage.hpp"

struct stat;

template <class T> class Image;



// Base abstract struct of line unary function
template <class T>
struct _SMIL unaryLineFunctionBase
{
    virtual void _exec(T *lineIn, int size, T *lineOut) {}
    virtual void _exec_aligned(T *lineIn, int size, T *lineOut) { _exec(lineIn, size, lineOut); }
    virtual void _exec(T *lInOut, int size, T value) {}
    virtual void _exec_aligned(T *lineIn, int size, T value) { _exec(lineIn, size, value); }
    inline void operator()(T *lineIn, int size, T *lineOut)
    { 
	unsigned long ptrOffset = PTR_OFFSET(lineIn);
	unsigned long misAlignSize = ptrOffset==0 ? 0 : SIMD_VEC_SIZE - ptrOffset;
	if (misAlignSize)
	  _exec(lineIn, misAlignSize, lineOut); 
	_exec_aligned(lineIn+misAlignSize, size-misAlignSize, lineOut+misAlignSize); 
    }
    inline void operator()(T *lineIn, int size, T value)
    { 
	unsigned long ptrOffset = PTR_OFFSET(lineIn);
	unsigned long misAlignSize = ptrOffset==0 ? 0 : SIMD_VEC_SIZE - ptrOffset;
	if (misAlignSize)
	  _exec(lineIn, misAlignSize, value); 
	_exec_aligned(lineIn+misAlignSize, size-misAlignSize, value); 
    }
};


// Base abstract struct of line binary function
template <class T>
struct _SMIL binaryLineFunctionBase
{
    virtual void _exec(T *lineIn1, T *lineIn2, int size, T *lineOut) {}
    virtual void _exec_aligned(T *lineIn1, T *lineIn2, int size, T *lineOut) { _exec(lineIn1, lineIn2, size, lineOut); }
    inline void operator()(T *lineIn1, T *lineIn2, int size, T *lineOut)
    { 
	unsigned long ptrOffset = PTR_OFFSET(lineIn1);
	unsigned long misAlignSize = ptrOffset==0 ? 0 : SIMD_VEC_SIZE - ptrOffset;
	if (misAlignSize)
	  _exec(lineIn1, lineIn2, misAlignSize, lineOut); 
	_exec_aligned(lineIn1+misAlignSize, lineIn2+misAlignSize, size-misAlignSize, lineOut+misAlignSize); 
    }
};

// Base abstract struct of line binary function
template <class T>
struct _SMIL tertiaryLineFunctionBase
{
    virtual void _exec(T *lineIn1, T *lineIn2, T *lineIn3, int size, T *lineOut) {}
    virtual void _exec_aligned(T *lineIn1, T *lineIn2, T *lineIn3, int size, T *lineOut) { _exec(lineIn1, lineIn2, lineIn3, size, lineOut); }
    inline void operator()(T *lineIn1, T *lineIn2, T *lineIn3, int size, T *lineOut)
    { 
	unsigned long ptrOffset = PTR_OFFSET(lineIn1);
	unsigned long misAlignSize = ptrOffset==0 ? 0 : SIMD_VEC_SIZE - ptrOffset;
	if (misAlignSize)
	  _exec(lineIn1, lineIn2, lineIn3, misAlignSize, lineOut); 
	_exec_aligned(lineIn1+misAlignSize, lineIn2+misAlignSize, lineIn3+misAlignSize, size-misAlignSize, lineOut+misAlignSize); 
    }
};



#endif