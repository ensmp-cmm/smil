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


#ifndef _BASE_IMAGE_OPERATIONS_HXX
#define _BASE_IMAGE_OPERATIONS_HXX

#include "DImage.hpp"
#include "DMemory.hpp"

#ifdef USE_OPEN_MP
#include <omp.h>
#endif // USE_OPEN_MP

template <class T>
struct fillLine;

template <class T>
inline typename Image<T>::lineType *imageFunctionBase<T>::createAlignedBuffers(UINT8 nbr, UINT32 len)
{
    if (alignedBuffers)
    {
        if (nbr==bufferNumber && len==bufferLength)
            return alignedBuffers;

        deleteAlignedBuffers();
    }


    bufferNumber = nbr;
    bufferLength = len;
    bufferSize = bufferLength * sizeof(T);

    alignedBuffers = new lineType[bufferNumber];
    for (int i=0;i<bufferNumber;i++)
        alignedBuffers[i] = ImDtTypes<T>::createLine(len);

    return alignedBuffers;
}


template <class T>
inline void imageFunctionBase<T>::deleteAlignedBuffers()
{
    if (!alignedBuffers) return;

    for (UINT i=0;i<bufferNumber;i++)
      ImDtTypes<T>::deleteLine(alignedBuffers[i]);
}

template <class T>
inline void imageFunctionBase<T>::copyLineToBuffer(T *line, UINT32 bufIndex)
{
    memcpy(alignedBuffers[bufIndex], line, bufferSize);
}

template <class T>
inline void imageFunctionBase<T>::copyBufferToLine(UINT32 bufIndex, T *line)
{
    memcpy(line, alignedBuffers[bufIndex], bufferSize);
}




template <class T, class lineFunction_T>
inline RES_T unaryImageFunction<T, lineFunction_T>::_exec(const imageType &imIn, imageType &imOut)
{
    if (!areAllocated(&imIn, &imOut, NULL))
        return RES_ERR_BAD_ALLOCATION;

    int lineLen = imIn.getWidth();
    int lineCount = imIn.getLineCount();

    lineType *srcLines = imIn.getLines();
    lineType *destLines = imOut.getLines();

#pragma omp parallel for
    for (int i=0;i<lineCount;i++)
        lineFunction(srcLines[i], lineLen, destLines[i]);

    imOut.modified();

    return RES_OK;
}


template <class T, class lineFunction_T>
inline RES_T unaryImageFunction<T, lineFunction_T>::_exec(imageType &imOut, const T &value)
{
    if (!areAllocated(&imOut, NULL))
        return RES_ERR_BAD_ALLOCATION;

    int lineLen = imOut.getWidth();
    int lineCount = imOut.getLineCount();

    sliceType destLines = imOut.getLines();
    lineType constBuf = ImDtTypes<T>::createLine(lineLen);

    // Fill the first aligned buffer with the constant value
    fillLine<T>::_exec(constBuf, lineLen, value);

    // Use it for operations on lines

#pragma omp parallel for
    for (int i=0;i<lineCount;i++)
        lineFunction._exec_aligned(constBuf, lineLen, destLines[i]);

    ImDtTypes<T>::deleteLine(constBuf);
    imOut.modified();
}


// Binary image function
template <class T, class lineFunction_T>
inline RES_T binaryImageFunction<T, lineFunction_T>::_exec(const imageType &imIn1, const imageType &imIn2, imageType &imOut)
{
    if (!areAllocated(&imIn1, &imIn2, &imOut, NULL))
        return RES_ERR_BAD_ALLOCATION;

    UINT lineLen = imIn1.getWidth();
    UINT lineCount = imIn1.getLineCount();

    lineType *srcLines1 = imIn1.getLines();
    lineType *srcLines2 = imIn2.getLines();
    lineType *destLines = imOut.getLines();

    UINT i, chunk = 100;
    #pragma omp parallel shared(srcLines1,srcLines2,destLines,chunk) private(i)
    {
	#pragma omp for schedule(dynamic,chunk) nowait
	for (i=0;i<lineCount;i++)
	    lineFunction(srcLines1[i], srcLines2[i], lineLen, destLines[i]);
    }
    imOut.modified();

    return RES_OK;
}

// Binary image function
template <class T, class lineFunction_T>
inline RES_T binaryImageFunction<T, lineFunction_T>::_exec(const imageType &imIn, imageType &imInOut)
{
    if (!areAllocated(&imIn, &imInOut, NULL))
        return RES_ERR_BAD_ALLOCATION;

    int lineLen = imIn.getWidth();
    int lineCount = imIn.getLineCount();

    sliceType srcLines1 = imIn.getLines();
    sliceType srcLines2 = imInOut.getLines();

    lineType tmpBuf = ImDtTypes<T>::createLine(lineLen);

#pragma omp parallel for
    for (int i=0;i<lineCount;i++)
        lineFunction(srcLines1[i], srcLines2[i], lineLen, tmpBuf);

    ImDtTypes<T>::deleteLine(tmpBuf);
    imInOut.modified();

    return RES_OK;
}


// Binary image function
template <class T, class lineFunction_T>
inline RES_T binaryImageFunction<T, lineFunction_T>::_exec(const imageType &imIn, const T &value, imageType &imOut)
{
    if (!areAllocated(&imIn, &imOut, NULL))
        return RES_ERR_BAD_ALLOCATION;

    int lineLen = imIn.getWidth();
    int lineCount = imIn.getLineCount();

    sliceType srcLines = imIn.getLines();
    sliceType destLines = imOut.getLines();

    lineType constBuf = ImDtTypes<T>::createLine(lineLen);

    // Fill the const buffer with the value
    fillLine<T> f;
    f(constBuf, lineLen, value);

#pragma omp parallel for
    for (int i=0;i<lineCount;i++)
        lineFunction(srcLines[i], constBuf, lineLen, destLines[i]);

    ImDtTypes<T>::deleteLine(constBuf);
    imOut.modified();

    return RES_OK;
}



// Tertiary image function
template <class T, class lineFunction_T>
inline RES_T tertiaryImageFunction<T, lineFunction_T>::_exec(const imageType &imIn1, const imageType &imIn2, const imageType &imIn3, imageType &imOut)
{
    if (!areAllocated(&imIn1, &imIn2, &imIn3, &imOut, NULL))
        return RES_ERR_BAD_ALLOCATION;

    int lineLen = imIn1.getWidth();
    int lineCount = imIn1.getLineCount();

    sliceType srcLines1 = imIn1.getLines();
    sliceType srcLines2 = imIn2.getLines();
    sliceType srcLines3 = imIn3.getLines();
    sliceType destLines = imOut.getLines();

#pragma omp parallel for
    for (int i=0;i<lineCount;i++)
        lineFunction(srcLines1[i], srcLines2[i], srcLines3[i], lineLen, destLines[i]);

    imOut.modified();

    return RES_OK;
}

// Tertiary image function
template <class T, class lineFunction_T>
inline RES_T tertiaryImageFunction<T, lineFunction_T>::_exec(const imageType &imIn1, const imageType &imIn2, const T &value, imageType &imOut)
{
    if (!areAllocated(&imIn1, &imIn2, &imOut, NULL))
        return RES_ERR_BAD_ALLOCATION;

    int lineLen = imIn1.getWidth();
    int lineCount = imIn1.getLineCount();

    sliceType srcLines1 = imIn2.getLines();
    sliceType srcLines2 = imIn2.getLines();
    sliceType destLines = imOut.getLines();

    lineType constBuf = ImDtTypes<T>::createLine(lineLen);

    // Fill the const buffer with the value
    fillLine<T> f;
    f(constBuf, lineLen, value);

#pragma omp parallel for
    for (int i=0;i<lineCount;i++)
        lineFunction(srcLines1[i], srcLines2[i], constBuf, lineLen, destLines[i]);

    ImDtTypes<T>::deleteLine(constBuf);
    imOut.modified();

    return RES_OK;
}

template <class T, class lineFunction_T>
inline RES_T tertiaryImageFunction<T, lineFunction_T>::_exec(const imageType &imIn1, const T &value, const imageType &imIn2, imageType &imOut)
{
    return tertiaryImageFunction<T, lineFunction_T>::_exec(imIn1, imIn2, value, imOut);
}


template <class T, class lineFunction_T>
inline RES_T tertiaryImageFunction<T, lineFunction_T>::_exec(const imageType &imIn, const T &value1, const T &value2, imageType &imOut)
{
    if (!areAllocated(&imIn, &imOut, NULL))
        return RES_ERR_BAD_ALLOCATION;

    int lineLen = imIn.getWidth();
    int lineCount = imIn.getLineCount();

    sliceType srcLines = imIn.getLines();
    sliceType destLines = imOut.getLines();

    lineType constBuf1 = ImDtTypes<T>::createLine(lineLen);
    lineType constBuf2 = ImDtTypes<T>::createLine(lineLen);

    // Fill the const buffers with the values
    fillLine<T> f;
    f(constBuf1, lineLen, value1);
    f(constBuf2, lineLen, value2);

#pragma omp parallel for
    for (int i=0;i<lineCount;i++)
        lineFunction(srcLines[i], constBuf1, constBuf2, lineLen, destLines[i]);

    ImDtTypes<T>::deleteLine(constBuf1);
    ImDtTypes<T>::deleteLine(constBuf2);
    imOut.modified();

    return RES_OK;
}



#endif