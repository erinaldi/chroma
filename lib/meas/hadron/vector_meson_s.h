#ifndef VECTOR_MESON_S_H
#define VECTOR_MESON_S_H

#define NUM_STAG_PROPS   8
#define NUM_STAG_PIONS   16


#include "meas/hadron/hadron_corr_s.h"

namespace Chroma {

  class staggered_hadron_corr ; 

  class vector_meson  : public staggered_hadron_corr
  {


  public :

    void
    compute(multi1d<LatticeStaggeredPropagator>& quark_props,
	    int j_decay);


    vector_meson(int t_len, multi1d<LatticeColorMatrix> & uin)  
      : staggered_hadron_corr(t_len,no_vector,uin)
      {
	outer_tag = "Vector_meson"  ; 
	inner_tag = "Vec" ; 

	tag_names.resize(no_vector) ; 

      }

    virtual ~vector_meson()
      {
      }


  protected:

  private :
    static const int no_vector = 4 ; 


  } ; 

}  // end namespace Chroma

#endif
