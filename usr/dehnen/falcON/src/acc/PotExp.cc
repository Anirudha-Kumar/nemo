//-----------------------------------------------------------------------------+
//                                                                             |
// PotExp.cc                                                                   |
//                                                                             |
// Copyright (C) 2004 Walter Dehnen                                            |
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
#include <defacc.h>
#include <ctime>
#include "proper/PotExp.cc"                        // falcON's PotExp           
extern "C" {
#  include <stdinc.h>                              // general NEMO stuff
#  include <filestruct.h>                          // NEMO file stuff
#  include <snapshot/snapshot.h>                   // NEMO snapshot stuff
#  include <history.h>                             // NEMO history stuff
}
////////////////////////////////////////////////////////////////////////////////
namespace MyPotExp {
  using falcON::PotExp;
  typedef falcON::tupel<3,float>  vectf;
  typedef falcON::tupel<3,double> vectd;
  //----------------------------------------------------------------------------
  const int    Nexp = 10;                          // max # of pot allowed
  const double a_def=1, r_def=1;                   // default parameter
  const int    n_def=8, l_def=8, s_def=1;          //   values
  //----------------------------------------------------------------------------
  struct PwithC : public PotExp {
    PotExp::Anlm Coef;
    PwithC(scalar a, scalar r, int n, int l, PotExp::symmetry s) :
      PotExp(a,r,n,l,s), Coef(*this) {}
  };
  //----------------------------------------------------------------------------
  inline PotExp::symmetry sym(int s) {
    return
      s==4? PotExp::spherical :
      s==3? PotExp::cylindrical : 
      s==2? PotExp::triaxial :
      s==1? PotExp::reflexion : 
            PotExp::none;
  }
  //----------------------------------------------------------------------------
  class PotExpansion {
    PwithC *P;
  public:
    static const char* name() {
      return "PotExp";
    }
    //--------------------------------------------------------------------------
    PotExpansion() : P(0)
    {}
    //--------------------------------------------------------------------------
    void init(const double*, int, const char*);
    //--------------------------------------------------------------------------
    bool is_init() const {
      return P!=0;
    }
    //--------------------------------------------------------------------------
    ~PotExpansion() {
      delete P;
    }
    //--------------------------------------------------------------------------
    void acc(int        n,
	     const void*x,
	     const int *f,
	     void      *p,
	     void      *a,
	     int        d,
	     char       t) const
    {
      clock_t cpu0 = clock();
      switch(t) {
      case 'f':
	P->SetGravity(P->Coef, n,
		      static_cast<const vectf*>(x),
		      static_cast<float*>(p),
		      static_cast<vectf*>(a),
		      f,d);
	if(P->has_error  ()) error  (const_cast<char*>(P->error_msg()));
	if(P->has_warning()) warning(const_cast<char*>(P->warning_msg()));
	break;
      case 'd':
	P->SetGravity(P->Coef, n,
		      static_cast<const vectd*>(x),
		      static_cast<double*>(p),
		      static_cast<vectd*>(a),
		      f,d);
	if(P->has_error  ()) error  (const_cast<char*>(P->error_msg()));
	if(P->has_warning()) warning(const_cast<char*>(P->warning_msg()));
	break;
      default:
	error("%s unknown type : '%c'",t);
      }
      clock_t cpu1 = clock();
      nemo_dprintf(2,"PotExp: gravity computed in %f sec CPU time\n",
		   (cpu1 - cpu0)/double(CLOCKS_PER_SEC));
    }
  } // class PotExpansion
  Pexp[Nexp];                                      // array of Nexp PotExpansion
  //----------------------------------------------------------------------------
  void PotExpansion::init(const double*pars,
			  int          npar,
			  const char  *file)
  {
    // 0 checking consistency of arguments
    if(npar < 5)
      warning("%s: recognizing 6 parameters and one data file.\n"
	      "Parameters:\n"
	      " omega (real)           pattern speed (ignored)              [0]\n"
	      " alpha (real)           shape parameter of expansion basis   [%f]\n"
	      " r0    (real)           scale radius of expansion basis      [%f]\n"
	      " nmax  (integer > 0)    max n in radial expansion            [%d]\n"
	      " lmax  (integer, even)  max l in angular expansion           [%d]\n"
	      " symm  (integer)        symmetry assumed (see below)         [%d]\n"
	      " G     (real)           constant of gravity                  [1]\n\n"
	      "The potential is given by the expansion\n\n"
	      "    Phi(x) =  Sum  C_nlm Phi     (x)\n"
	      "             n,l,m          n,l,m\n\n"
	      "with the basis functions\n\n"
	      "    Phi_nlm = - Psi_nl(r) * Y_lm(theta,phi).\n\n"
	      "The lowest order radial basis function is given by\n\n"
              "                     (1/a)     -a\n"
	      "    Psi_00 = ( [r/r0]      + 1)\n\n"
	      "which gives a Hernquist sphere for a=alpha=1 and a Plummer sphere for a=1/2.\n"
	      "The coefficients are such that potential approximates that of the first\n"
	      "snapshot found in the data file.\n"
	      "The last parameter, symm, allows to symmetrize the potential by constraining\n"
	      "the coefficients:\n"
	      " symm=0:   no symmetry: all coefficients used\n"
	      " symm=1:   reflexion wrt origin: C_nlm=0 for odd (l,m)\n"
	      " symm=2:   triaxial wrt xyz axes: C_nlm=0 for odd (l,m) and C_nlm = C_nl[-m]\n"
	      " symm=3:   cylindrical: C_nlm=0 for odd l or m!=0\n"
	      " symm=4:   spherical: C_nlm=0 for (l,m) != 0\n",
	      name(),a_def,r_def,n_def,l_def,s_def);
    if(file==0 || file[0]==0)
      error("%s: data file required\n","PotExp");
    // 1 reading in parameters and initializing potential expansion
    double
      o = npar>0? pars[0] : 0,
      a = npar>1? pars[1] : a_def,
      r = npar>2? pars[2] : r_def;
    int
      n = npar>3? int(pars[3]) : n_def,
      l = npar>4? int(pars[4]) : l_def,
      s = npar>5? int(pars[5]) : s_def;
    double
      G = npar>6? pars[6] : 1.;
    if(s < 0 || s > 4) {
      warning("%s: symm out of range, defaulting to %d (%s symmetry)\n",
	      name(),s_def,PotExp::name_of_sym(sym(s_def)));
      s = s_def;
    }
    if(npar>6) warning("%s: skipped parameters beyond 5",name());
    P = new PwithC(a,r,n,l,sym(s));
    if(P->has_error  ()) error  (const_cast<char*>(P->error_msg()));
    if(P->has_warning()) warning(const_cast<char*>(P->warning_msg()));
    nemo_dprintf(2,
		 "PotExp: initialized expansion with\n"
		 " alpha = %f\n"
		 " r0    = %f\n"
		 " nmax  = %d\n"
		 " lmax  = %d\n"
		 " assuming %s symmetry\n",
		 P->alpha(), P->scale(), P->Nmax(), P->Lmax(), 
		 P->symmetry_name());
    // 2 reading in positions and masses from snapshot
    int N;
    ::stream input = stropen(file,"r");            // open data file
    get_history(input);
    nemo_dprintf(5,"PotExp: opened file %s for snapshot input\n",file);
    get_set (input,SnapShotTag);                   //  open snapshot
    nemo_dprintf(5,"PotExp: opened snapshot\n");
    get_set (input,ParametersTag);                 //   open parameter set
    get_data(input,NobjTag,IntType,&N,0);          //    read N
    get_tes (input,ParametersTag);                 //   close parameter set
    nemo_dprintf(5,"PotExp: read N=%d\n",N);
    get_set (input,ParticlesTag);                  //   open particle set
    // 2.1 read positions:
    vectf *x = new vectf[N];                       //   allocate positions
    if(get_tag_ok(input,PhaseSpaceTag)) {          //   IF phases available
      nemo_dprintf(5,"PotExp: found phases rather than positions\n");
      float *p = new float[6*N];                   //     get memory for them
      get_data_coerced(input,PhaseTag,FloatType,p,N,2,3,0);  // read them
      nemo_dprintf(5,"PotExp: read %d phases\n",N);
      for(int i=0,ip=0; i!=N; ++i,ip+=6)           //     loop bodies
	x[i].copy(p+ip);                           //       copy positions
      delete[] p;                                  //     free mem for phases
      nemo_dprintf(5,"PotExp: copied phases to positions\n");
    } else if(get_tag_ok(input,PosTag)) {          //   ELIF positions
      get_data_coerced(input,PosTag,FloatType,
		       static_cast<float*>(static_cast<void*>(x)),
		       N,3,0);
      nemo_dprintf(5,"PotExp: read %d positions\n",N);
    } else                                         //   ENDIF
      error("%s: no positions found in snapshot\n",name());
    // 2.2 read masses:
    float *m = new float[N];
    if(get_tag_ok(input,MassTag))
      get_data_coerced(input,MassTag,FloatType,m,N,0);
    else
      error("%s: no masses found in snapshot\n",name());
    nemo_dprintf(5,"PotExp: read %d masses\n",N);
    get_tes (input,ParticlesTag);                  //   close particle set
    get_tes (input,SnapShotTag);                   //  close snapshot
    strclose(input);                               // close file
    nemo_dprintf(2,"PotExp: read %d masses and positions from file %s\n",
		 N,file);
    // 3 initializing coefficients
    clock_t cpu0 = clock();
    P->Coef.reset();
    P->AddCoeffs(P->Coef,N,m,x,0);
    if(P->has_error  ()) error  (const_cast<char*>(P->error_msg()));
    if(P->has_warning()) warning(const_cast<char*>(P->warning_msg()));
    P->Normalize(P->Coef,G);
    if(P->has_error  ()) error  (const_cast<char*>(P->error_msg()));
    if(P->has_warning()) warning(const_cast<char*>(P->warning_msg()));
    clock_t cpu1 = clock();
    nemo_dprintf(2,"PotExp: coefficients computed in %f sec CPU time\n",
		 (cpu1 - cpu0)/double(CLOCKS_PER_SEC));
    if(nemo_debug(2)) {
      std::cerr<<"PotExp: coefficients:\n";
      P->Coef.table_print(P->Symmetry(),std::cerr);
    }
    // 4 clearing up
    delete[] x;
    delete[] m;
  } // PotExpansion::init()
  //----------------------------------------------------------------------------
#define ACCFUNC(NUM)							\
  void accel##NUM(int        ndim,					\
		  double     time,					\
		  int        n,						\
		  const void*m,						\
		  const void*x,						\
		  const void*v,						\
		  const int *f,						\
		  void      *p,						\
		  void      *a,						\
		  int        d,						\
		  char       type)					\
  {									\
    if(ndim != 3)							\
      error("%s: ndim=%d not supported\n",PotExpansion::name(),ndim);	\
    Pexp[NUM].acc(n,x,f,p,a,d,type);					\
  }
  ACCFUNC(0);                           // accel0() to accel(9)
  ACCFUNC(1);                           // use Pexp[i].acc()
  ACCFUNC(2);  
  ACCFUNC(3);  
  ACCFUNC(4);  
  ACCFUNC(5);  
  ACCFUNC(6);  
  ACCFUNC(7);  
  ACCFUNC(8);  
  ACCFUNC(9);  

  // array of Nexp acc_pter: accel0 to accel9
  acc_pter ACCS[Nexp] = {accel0, accel1, accel2, accel3, accel4,
			 accel5, accel6, accel7, accel8, accel9};
  int Iexp = 0;
} // namespace MyPotExp {

void iniacceleration(
		     const double*pars,
		     int          npar,
		     const char  *file,
		     acc_pter    *accf,
		     bool        *needm,
		     bool        *needv)
{
  if(MyPotExp::Iexp == MyPotExp::Nexp)
    error("iniacceleration(): cannot have more than %d instances of '%s'\n",
	  MyPotExp::Nexp,MyPotExp::PotExpansion::name());
  if(needm) *needm = 0;
  if(needv) *needv = 0;
  MyPotExp::Pexp[MyPotExp::Iexp].init(pars,npar,file);
  *accf = MyPotExp::ACCS[MyPotExp::Iexp++];
}
//------------------------------------------------------------------------------
