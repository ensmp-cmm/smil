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


#ifndef _D_IMAGE_IO_HPP
#define _D_IMAGE_IO_HPP


#include <string>
#include <algorithm>



#include "DImageIO_BMP.hpp"
#include "DImageIO_RAW.hpp"

#ifdef USE_PNG
#include "DImageIO_PNG.hpp"
#endif // USE_PNG


extern const char *getFileExtension(const char *fileName);

#ifdef USE_CURL
extern RES_T getHttpFile(const char *url, const char *outfilename);
#endif // USE_CURL

template <class T>
RES_T read(const char* filename, Image<T> &image)
{
    string fileExt = getFileExtension(filename);
    string filePrefix = (string(filename).substr(0, 7));
    string tmpFileName;
    
    RES_T res;
    
    if (filePrefix=="http://")
    {
#ifdef USE_CURL
	tmpFileName = "_smilTmpIO." + fileExt;
	if (getHttpFile(filename, tmpFileName.c_str())!=RES_OK)
	{
	    cout << "Error reading file " << filename << endl;
	    return RES_ERR;
	}
	res = read(tmpFileName.c_str(), image);
	remove(tmpFileName.c_str());
	return res;
#else // USE_CURL
	cout << "Error: to use this functionality you must compile smil with the Curl option" << endl;
	return RES_ERR;
#endif // USE_CURL
    }

    if (fileExt=="BMP")
        res = readBMP(filename, image);

#ifdef USE_PNG
    else if (fileExt=="PNG")
        res = readPNG(filename, image);
#endif // USE_PNG

//     else if (fileExt=="RAW")
//         res = readRAW(filename, image);
    
    else
    {
        cout << "File type not supported" << endl;
	res = RES_ERR;
    }
    
    if (res==RES_OK)
    {
// 	if (!image.getName())
// 	{
// 	    string fName = filename;
// 	    int pos = fName.find_last_of("/\\");
// 	    fName = fName.substr(pos+1);
// 	    image.setName(fName.c_str());
// 	}
// 	image.modified();
    }
    
    return res;
}


template <class T>
RES_T write(Image<T> &image, const char *filename)
{
    string fileExt = getFileExtension(filename);
    RES_T res;

    if (fileExt=="BMP")
        res = writeBMP(image, filename);

#ifdef USE_PNG
    else if (fileExt=="PNG")
        res = writePNG(image, filename);
#endif // USE_PNG

    else if (fileExt=="RAW")
        res = writeRAW(image, filename);
    
    else
    {
        cout << "File type not supported" << endl;
	res = RES_ERR;
    }
    
    return res;
}





#endif // _D_IMAGE_IO_HPP
