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


#ifndef _D_MORPHO_GRAPH_HPP
#define _D_MORPHO_GRAPH_HPP

#include "DMorphImageOperations.hpp"
#include "DGraph.hpp"


namespace smil
{
    /**
    * \ingroup Morpho
    * \defgroup Graph
    * @{
    */

    template <class T1, class T2, class graphT=Graph<UINT,UINT> >
    class mosaicToGraphFunct : public unaryMorphImageFunctionBase<T1, T2>
    {
    public:
	typedef unaryMorphImageFunctionBase<T1, T2> parentClass;
	
	mosaicToGraphFunct(/*graphT *externGraph=NULL*/)
	  : graphPtr(auto_ptr<graphT>(new graphT())), graph(*graphPtr.get())
	{
	}
	mosaicToGraphFunct(graphT &externGraph)
	  : graph(externGraph)
	{
	    externGraph.clear();
	}
	virtual inline void processPixel(size_t &pointOffset, vector<int>::iterator dOffset, vector<int>::iterator dOffsetEnd)
	{
	    T1 curVal = parentClass::pixelsIn[pointOffset];
	    bool mixed = false;
	    while(dOffset!=dOffsetEnd)
	    {
		T1 val = parentClass::pixelsIn[pointOffset + *dOffset];
		if (val!=curVal)
		{
		  mixed = true;
		  // Add an edge between the two basins. 
		  // If the edge already exists, its weight will be the minimum value between the existing and the new one (pixelsOut[pointOffset]).
		  graph.addEdge(val, curVal, parentClass::pixelsOut[pointOffset]);
		}
		dOffset++;
	    }
	}
    protected:
	auto_ptr<graphT> graphPtr;
    public:
	graphT &graph;
    };
    
    /**
    */ 
    template <class T1, class T2, class graphT>
    graphT mosaicToGraph(const Image<T1> &imMosaic, const Image<T2> &imValues, const StrElt &se=DEFAULT_SE)
    {
	ASSERT(imMosaic.isAllocated() && imValues.isAllocated(), graphT());
	
	mosaicToGraphFunct<T1, T2, graphT> f;
	
	ASSERT(f._exec(imMosaic, (Image<T2>&)imValues, se)==RES_OK, graphT());
	
	return f.graph;
	
    }
    template <class T1, class T2, class graphT>
    RES_T mosaicToGraph(const Image<T1> &imMosaic, const Image<T2> &imValues, graphT &graph, const StrElt &se=DEFAULT_SE)
    {
	ASSERT(imMosaic.isAllocated() && imValues.isAllocated(), RES_ERR);
	
	mosaicToGraphFunct<T1, T2, graphT> f(graph);
	
	ASSERT(f._exec(imMosaic, (Image<T2>&)imValues, se)==RES_OK, RES_ERR);
	
	return RES_OK;
    }

    template <class T, class graphT>
    RES_T graphToMosaic(const Image<T> &imMosRef, const graphT &graph, Image<T> &imOut)
    {
	ASSERT_ALLOCATED(&imOut);
	
	map<size_t,size_t> nodeMap = graph.labelizeNodes();
	map<T,T> lut(nodeMap.begin(), nodeMap.end()); 
	
	return applyLookup<T>(imMosRef, lut, imOut);
    }

    template <class mosImT, class graphT, class imOutT>
    RES_T drawGraph(const Image<mosImT> &imMosaic, const graphT &graph, Image<imOutT> &imOut, imOutT linesValue=ImDtTypes<imOutT>::max())
    {
	ASSERT_ALLOCATED(&imMosaic, &imOut);
	ASSERT_SAME_SIZE(&imMosaic, &imOut);
	
	ImageFreezer freeze(imOut);
	
	map<UINT, vector<double> > barys = measBarycenters(imMosaic);
	
	typedef typename graphT::EdgeType EdgeType;
	typedef const vector< EdgeType > EdgeListType;
	EdgeListType &edges = graph.getEdges();
	
	for(typename EdgeListType::const_iterator it=edges.begin();it!=edges.end();it++)
	{
	    const EdgeType &edge = *it;
	    
	    if (edge.source==edge.target)
	      continue;
	    
	    vector<double> &p1 = barys[edge.source];
	    vector<double> &p2 = barys[edge.target];
	    
	    if (p1.empty() || p2.empty())
	      continue;
	    
	    drawLine(imOut, p1[0], p1[1], p2[0], p2[1], linesValue);
	}
	
	return RES_OK;
    }

/** \} */

} // namespace smil



#endif // _D_MORPHO_GRAPH_HPP
