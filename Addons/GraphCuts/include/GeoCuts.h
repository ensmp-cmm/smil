#ifndef __D_GEOCUTS_H__
#define __D_GEOCUTS_H__

namespace smil
{
  /**
   * @ingroup AddonGraphCut
   * @defgroup GeoCutGroup Geo Cuts
   *
   * Most of these algorithms were developped in the PhD thesis of Jean
   * Stawiaski.
   *
   * @see
   * <i>Jean Stawiaski</i>,
   * <a
   * href="https://smil.cmm.minesparis.psl.eu/publis/These_Jean_Stawiaski.pdf">
   *  Mathematical Morphology and Graphs :  Applications
   * to Interactive Medical Image Segmentation</a>, PhD Thesis - 2008
   *
   * @author
   * * Jean Stawiaski
   * * José-Marcio Martins da Cruz (porting and rewriting)
   */

} // namespace smil

typedef float F_SIMPLE;
typedef double CVariant;

#include "GeoCuts_Markov.h"
#include "GeoCuts_MinSurfaces.h"
#include "GeoCuts_Watershed.h"

#endif // __D_GEOCUTS_H__
