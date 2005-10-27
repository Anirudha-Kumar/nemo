// -*- C++ -*-                                                                 |
//-----------------------------------------------------------------------------+
//                                                                             |
// add_plummer.cc                                                              |
//                                                                             |
// Copyright (C) 2004, 2005 Walter Dehnen                                      |
//                                                                             |
// This program is free software; you can redistribute it and/or modify        |
// it under the terms of the GNU General Public License as published by        |
// the Free Software Foundation; either version 2 of the License, or (at       |
// your option) any later version.                                             |
//                                                                             |
// This program is distributed in the hope that it will be useful, but         |
// WITHOUT ANY WARRANTY; without even the implied warranty of                  |
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
// General Public License for more details.                                    |
//                                                                             |
// You should have received a copy of the GNU General Public License           |
// along with this program; if not, write to the Free Software                 |
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                   |
//                                                                             |
//-----------------------------------------------------------------------------+
#include <public/defman.h>
#include <public/random.h>
#include <public/Pi.h>
#include <ctime>
#include <cmath>

namespace {
  using namespace falcON;
  //////////////////////////////////////////////////////////////////////////////
  //                                                                          //
  // class add_plummer                                                        //
  //                                                                          //
  //////////////////////////////////////////////////////////////////////////////
  class add_plummer : public manipulator {
  private:
    const   Random3 Ran;
    const   int     N;
    const   double  GM,R,V,m,e;
    mutable int     K;
    //--------------------------------------------------------------------------
  public:
    const char* name    () const { return "add_plummer"; }
    const char* describe() const {
      return message("add a %d new bodies drawn from a Plummer "
		     "sphere, one each time step",N);
    }
    //--------------------------------------------------------------------------
    void draw(double&r, double&vr, double&vt) const
    {
      // 1. get radius from cumulative mass
      register double x = std::pow(Ran(),0.66666666666666666667), P = 1-x;
      r = std::sqrt(x/P);
      // 2. get velocity using rejection method
      register double ve= std::sqrt(2*P), f0= std::pow(P,3.5),f,v;
      do {
	v = ve * std::pow(Ran(),0.333333333333333333333);
	f = std::pow(P-0.5*v*v,3.5);
      } while(f0 * Ran() > f);
      r *= R;
      v *= V;
      register double c = Ran(-1.,1.);
      vr = v * c;
      vt = v * std::sqrt(1.-c*c);
    }
    //--------------------------------------------------------------------------
    void sample(vect&pos, vect&vel) const
    {
      // 1. get r,vr,vt
      double r,vr,vt;
      draw(r,vr,vt);
      // 2. set position
      register double
	cth = Ran(-1.,1.),
	sth = std::sqrt(1.-cth*cth),
	phi = Ran(0.,TPi),
	cph = std::cos(phi),
	sph = std::sin(phi);
      pos[0] = r * sth * cph;
      pos[1] = r * sth * sph;
      pos[2] = r * cth;
      // 2. set velocity
      register double
	psi = Ran(0.,TPi),
	vth = vt * std::cos(psi),
	vph = vt * std::sin(psi),
	vm  = vr * sth + vth * cth;
      vel[0] = vm * cph - vph * sph;
      vel[1] = vm * sph + vph * cph;
      vel[2] = vr * cth - vth * sth;
    }
    //--------------------------------------------------------------------------
    fieldset need   () const { return fieldset::o; }
    fieldset provide() const { return fieldset::o; }
    fieldset change () const { return fieldset::o; }
    //--------------------------------------------------------------------------
    bool manipulate(const snapshot*) const falcON_THROWING;
    //--------------------------------------------------------------------------
    add_plummer(const double*pars,
		int          npar,
		const char  *file) :
      N   (npar>0? int (pars[0]) : 0 ),
      GM  (npar>1?      pars[1]  : 1.),
      R   (npar>2?      pars[2]  : 1.),
      V   ( sqrt(GM/R ) ),
      Ran (npar>3? long(pars[3]) : long(time(0)) ),
      m   (npar>4?      pars[4]  : N? GM/N : 0.),
      e   (npar>5?      pars[5]  : 0.1 )
    {
      if(npar<6 && nemo_debug(1) || nemo_debug(2))
	std::cerr<<
	  "\n Manipulator \"add_plummer\":\n"
	  " adds N new bodies drawn from a Plummer sphere, one per time step;\n"
	  " meaning of parameters:\n"
	  " par[0] : N (default: 0)\n"
	  " par[1] : GM of Plummer sphere (default: 1)\n"
	  " par[2] : scale radius R of Plummer sphere (default: 1)\n"
	  " par[3] : random seed (default time)\n"
	  " par[4] : mass per new body (default: GM/N)\n"
	  " par[5] : individual softening length (if needed, default: 0.1)\n\n";
      if(npar>6 && nemo_debug(1))
	warning(" Manipulator \"add_plummer\":"
		" skipping parameters beyond 6\n");
      if(N <= 0)
	warning("Manipulator \"add_plummer\": N=%d: nothing to be done\n");
    }
    //--------------------------------------------------------------------------
    ~add_plummer() {}
  };
  //////////////////////////////////////////////////////////////////////////////
  bool add_plummer::manipulate(const snapshot*S) const falcON_THROWING {
    if(K >= N) return false;
    body B = const_cast<snapshot*>(S)->new_body(bodytype::std, N-K);
    if(! B.is_valid() )
      error("Manipulator \"add_plummer\": bodies::new_body() is invalid\n");
    ++K;
    if( S->have(fieldbit::x) )
      if( S->have(fieldbit::v) ) sample(B.pos(), B.vel());
      else { vect v; sample(B.pos(), v); }
    else if( S->have(fieldbit::v) ) { vect x; sample(x, B.vel()); }
    if( S->have(fieldbit::m) ) B.mass() = m;
    if( S->have(fieldbit::e) ) B.eps () = e;
    if( S->have(fieldbit::a) ) B.acc () = zero;
    if( S->have(fieldbit::p) ) B.pot () = zero;
    if( S->have(fieldbit::q) ) B.pex () = zero;
    if( S->have(fieldbit::l) ) {
      indx lmax = 0;
      LoopAllBodies(S,b) if(level(b) > lmax) lmax = level(b);
      B.level() = lmax;
    }
    debug_info(5,"Manipulator \"add_plummer\": "
	       "added new body with block No %d and sub-index %d\n",
	       block_No(B), subindex(B));
    return false;
  }
  //////////////////////////////////////////////////////////////////////////////
} // namespace {

__DEF__MAN(add_plummer)
