// -*- C++ -*-
// $Id: mciter.h,v 1.3 2005-01-14 18:42:37 edwards Exp $
/*! \file
 *  \brief One heatbath interation of updating the gauge field configuration
 */

#ifndef __mciter_h__
#define __mciter_h__

namespace Chroma {

//! One heatbath interation of updating the gauge field configuration
/*!
 * \ingroup heatbath
 *
 * Make one interation of updating the gauge field configuration:
 *      this consists of n_over overrelaxation sweeps followed
 *      by one heatbath sweep with nheat trials.
 * In the case of SU(3), for each link we loop over the 3 SU(2) subgroups.

 * Warning: this works only for Nc = 2 and 3 !

 * \param u        gauge field ( Modify )
 * \param n_over   number of overrelaxation sweeps ( Read )
 * \param nheat    number of heatbath trials ( Read )
 * \param ntrials  total number of individual heatbath trials ( Modify )
 * \param nfails   total number of individual heatbath failures ( Modify ) 
 */

void mciter(multi1d<LatticeColorMatrix>& u, 
	    int n_over, int nheat,
	    int& ntrials, int& nfails);

}  // end namespace Chroma

#endif
