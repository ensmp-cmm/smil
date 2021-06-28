// Copyright (c) 2011-2015, Matthieu FAESSEL and ARMINES
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.




%include smilCommon.i
%include <std_string.i>
SMIL_MODULE(smilDendrogram)

//////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////




%{
/* Includes needed header(s)/definitions in the wrapped code */
#include "DendroNode.hpp"
#include "Dendrogram.hpp"
%}

%import "DendroNode.hpp"
%import "Dendrogram.hpp"


// Import smilCore to have correct function signatures (arguments with Image_UINT8 instead of Image<unsigned char>)
%import smilCore.i
%import smilMorpho.i

TEMPLATE_WRAP_CLASS(DendroNode, DendroNode)
TEMPLATE_WRAP_CLASS_3T_CROSS(Dendrogram,Dendrogram)
TEMPLATE_WRAP_FUNC_3T_CROSS(getLevelHierarchySeg)
TEMPLATE_WRAP_FUNC_3T_CROSS(getThreshHierarchySeg)



