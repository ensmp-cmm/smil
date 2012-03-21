/*
 * Copyright (c) 2011, Matthieu FAESSEL and ARMINES
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _IMAGE_BIT_H
#define _IMAGE_BIT_H


#include "DImage.hpp"
#include "DTypes.h"


template <>
struct ImDtTypes<Bit>
{
    typedef Bit pixelType;
    typedef BitArray lineType;
    typedef lineType* sliceType;
    typedef sliceType* volType;

    static inline pixelType min() { return Bit(0); }
    static inline pixelType max() { return Bit(1); }
    static inline lineType createLine(UINT lineLen) 
    { 
	BitArray ba(lineLen);
	ba.createIntArray();
	return ba; 
    }
    static inline void deleteLine(lineType line) 
    { 
	line.deleteIntArray();
    }
    static inline unsigned long ptrOffset(lineType p, unsigned long n=SIMD_VEC_SIZE) { return ((unsigned long)(p.intArray)) & (n-1); }
};


template <>
void Image<Bit>::init();

template <>
void* Image<Bit>::getVoidPointer(void);

template <>
RES_T Image<Bit>::restruct(void);

template <>
RES_T Image<Bit>::allocate(void);

template <>
RES_T Image<Bit>::deallocate(void);


template <>
Image<Bit>& Image<Bit>::clone(const Image<Bit> &rhs);


template <>
void Image<Bit>::updateViewerData();


#endif // _IMAGE_BIN_HXX