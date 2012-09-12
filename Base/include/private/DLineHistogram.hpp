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
 *     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
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


#ifndef _D_LINE_HISTOGRAM_HPP
#define _D_LINE_HISTOGRAM_HPP


#include "DBaseLineOperations.hpp"

//! \ingroup Base
//! \defgroup Histogram
//! @{

template <class T>
struct threshLine : public unaryLineFunctionBase<T>
{
    T minVal, maxVal, trueVal, falseVal;
    
    inline void _exec(T* lIn, int size, T* lOut)
    {
	for(int i=0;i<size;i++)
	    lOut[i] = lIn[i] >= minVal && lIn[i] <= maxVal  ? trueVal : falseVal;
    }
};

template <class T>
struct stretchHistLine : public unaryLineFunctionBase<T>
{
    T inOrig, outOrig;
    double coeff;
    
    inline void _exec(T* lIn, int size, T* lOut)
    {
	double newVal;
	
	for(int i=0;i<size;i++)
	{
	    newVal = outOrig + (lIn[i]-inOrig)*coeff;
	    if (newVal > numeric_limits<T>::max())
		newVal = numeric_limits<T>::max();
	    else if (newVal < numeric_limits<T>::min())
		newVal = numeric_limits<T>::min();
	    lOut[i] = T(newVal);
	    
	}
    }
};


//! @}

#endif // _D_LINE_HISTOGRAM_HPP