// $Id: ischiral_w.h,v 1.3 2005-01-14 18:42:35 edwards Exp $
#ifndef IS_CHIRAL_VECTOR_H
#define IS_CHIRAL_VECTOR_H

#include <chromabase.h>

namespace Chroma {

enum Chirality { CH_MINUS=-1, CH_NONE=0, CH_PLUS=1};

Chirality isChiralVector( const LatticeFermion& chi);

}  // end namespace Chroma


#endif
