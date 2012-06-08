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


#ifndef _D_IMAGE_TRANSFORM_HPP
#define _D_IMAGE_TRANSFORM_HPP

#include "DBaseImageOperations.hpp"
#include "DLineArith.hpp"

/**
 * \ingroup Core
 * \defgroup Transform
 * @{
 */


/**
 * Horizontal mirror
 * 
 * Quick implementation (needs better integration and optimization).
 */
template <class T>
RES_T vFlip(Image<T> &imIn, Image<T> &imOut)
{
    if (!imIn.isAllocated() || !imOut.isAllocated())
        return RES_ERR_BAD_ALLOCATION;
  
    typename Image<T>::sliceType *slicesIn = imIn.getSlices();
    typename Image<T>::sliceType *slicesOut = imOut.getSlices();
    typename Image<T>::sliceType linesIn;
    typename Image<T>::sliceType linesOut;
    
    UINT width = imIn.getWidth();
    UINT height = imIn.getHeight();
    UINT depth = imIn.getDepth();

    for (int k=0;k<depth;k++)
    {
	linesIn = slicesIn[k];
	linesOut = slicesOut[k];
	
	for (int j=0;j<height;j++)
	  copyLine<T>(linesIn[j], width, linesOut[height-1-j]);
    }
    
    imOut.modified();
}

/** @}*/

#endif // _D_IMAGE_TRANSFORM_HPP

