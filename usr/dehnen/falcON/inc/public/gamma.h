// -*- C++ -*-                                                                 |
//-----------------------------------------------------------------------------+
//                                                                             |
// gamma.h                                                                     |
//                                                                             |
// Copyright (C) 1994, 1995, 2004, 2005  Walter Dehnen                         |
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
#ifndef falcON_included_gamma_h
#define falcON_included_gamma_h

#ifndef falcON_included_iostream
#  include <iostream>
#  define falcON_included_iostream
#endif
#ifndef falcON_included_Pi_h
#  include <public/Pi.h>
#endif
#ifndef falcON_included_inline_h
#  include <public/inline.h>
#endif
#ifndef falcON_included_basic_h
#  include <public/basic.h>
#endif
#ifndef falcON_included_sample_h
#  include <public/sample.h>
#endif
#ifndef falcON_included_numerics_h
#  include <public/numerics.h>
#endif
#ifndef falcON_included_cmath
#  include <cmath>
#  define falcON_included_cmath
#endif
////////////////////////////////////////////////////////////////////////////////
namespace falcON {
  //////////////////////////////////////////////////////////////////////////////
  //                                                                          //
  // class falcON::DehnenModel                                                //
  //                                                                          //
  // units used: G=M=a=1                                                      //
  //                                                                          //
  //////////////////////////////////////////////////////////////////////////////
  class DehnenModel {
  private:
    DehnenModel           (DehnenModel const&);    // not implemented           
    DehnenModel& operator=(DehnenModel const&);    // not implemented           
    //--------------------------------------------------------------------------
    // data of DehnenModel                                                      
    //--------------------------------------------------------------------------
    const double g,g1,g2,ig2,g3,ig3,g3f,g4;
    const double eps;
    //--------------------------------------------------------------------------
    // private methods                                                          
    //--------------------------------------------------------------------------
    double YcofE  (double) const;                  // non-inline in gamma.cc    
    double YcofLq (double) const;                  // non-inline in gamma.cc    
    double SigIsoQ(double) const;                  // non-inline in gamma.cc    
    //--------------------------------------------------------------------------
    // static methods                                                           
    //--------------------------------------------------------------------------
  public:
    static double YofX(double x) { return x/(x+1); }
    static double XofY(double y) { return y/(1-y); }
    //--------------------------------------------------------------------------
    // construction                                                             
    //--------------------------------------------------------------------------
    explicit
    DehnenModel(double,                            // I: gamma                  
		double=1.e-7);                     //[I: numerical precision]   
    //--------------------------------------------------------------------------
    // const data access                                                        
    //--------------------------------------------------------------------------
    double const& gamma() const { return g; }
    //--------------------------------------------------------------------------
    // potential, density, and cumulative mass                                  
    //                                                                          
    //   note:  y   := x/(x+1)                                                  
    //          Psi := -Phi                                                     
    //--------------------------------------------------------------------------
    enum mass {
      ym,           // argument is y
      xm,           // argument is x or r (for ScaledDehnenModel)
      ps,           // argument is Psi
      mm            // argument is M(<r)
    };
    template<mass> double Y (double) const;
    template<mass> double X (double) const;
    template<mass> double M (double) const;
    template<mass> double Ps(double) const;
    template<mass> double Rh(double) const;
    //--------------------------------------------------------------------------
    // circular orbits                                                          
    //--------------------------------------------------------------------------
    enum circ {
      yc,           // argument is y_circ
      xc,           // argument is x_circ or r_circ (for ScaledDehnenModel)
      lc,           // argument is L_circ
      lq,           // argument is L_circ^2
      ec            // argument is E_circ
    };
    template<circ> double Yc(double) const;
    template<circ> double Xc(double) const;
    template<circ> double Vc(double) const;
    template<circ> double Vq(double) const;
    template<circ> double Lc(double) const;
    template<circ> double Lq(double) const;
    template<circ> double Ec(double) const;
    template<circ> double Oq(double) const;
    template<circ> double Kq(double) const;
    template<circ> double Gm(double) const;
    //--------------------------------------------------------------------------
    // non-circular orbits                                                      
    //--------------------------------------------------------------------------
    double XofELC (double, double, double) const;  // X(E,L,cos(eta))           
    //--------------------------------------------------------------------------
    // isotropic velocity dispersion                                            
    //--------------------------------------------------------------------------
    template<mass> double sigq(double) const;
    //--------------------------------------------------------------------------
    // projected quantities                                                     
    //--------------------------------------------------------------------------
    double SurfaceDensity    (double) const;
    double DSurfaceDensityDR (double) const;
    double CumSurfaceDensity (double) const;
    double EffectiveRadius   ()       const;
    double SigIsotropicProj  (double) const;
    double SigCircProj       (double) const;
    //--------------------------------------------------------------------------
    // distributions                                                            
    //                                                                          
    // note: asymptotic behaviour of f(E):                                      
    //       at E~=0:      f propto E^(5/2)                                     
    //       at E~=Psi(0): f propto Y(E)^((g-6)/2)   for g != 0                 
    //                              Y(E)^(-2)        for g == 0                 
    //--------------------------------------------------------------------------
    double Fsub(double,                            // f(E): isotropic DF        
		double = -1.) const;               // I: gamma_rho < gamma      
    double F(double E) const { return Fsub(E); }   // f(E): isotropic DF        
    double G(double) const;                        // g(E): density of states   
    double F(double, double) const;                // f(Q,ra): OM DF            
  };
  //////////////////////////////////////////////////////////////////////////////
  //                                                                          //
  // class falcON::ScaledDehnenModel                                          //
  //                                                                          //
  // uses given scale radius and total mass to scale a DehnenModel.           //
  //                                                                          //
  //////////////////////////////////////////////////////////////////////////////
  class ScaledDehnenModel : protected DehnenModel {
  private:
    ScaledDehnenModel           (ScaledDehnenModel const&);
    ScaledDehnenModel& operator=(ScaledDehnenModel const&);
    //--------------------------------------------------------------------------
    // data of ScaledDehnenModel                                                
    //--------------------------------------------------------------------------
    const double sR,iR;                            // scale radius, inverse     
    const double sM,iM;                            // scale mass, inverse       
    const double sE,iE;                            // scale energy, inverse     
    const double sV,iV;                            // scale velocity, inverse   
    const double sL,iL;                            // scale ang. mom., inverse  
    const double sQ,iQ;                            // scale (a. m.)^2, inverse  
    const double sF,iF;                            // scale DF, inverse         
    const double sD,iD;                            // scale density, inverse    
    const double sO,iO;                            // scale frequency^2, inv.   
    //--------------------------------------------------------------------------
    // construction and such                                                    
    //--------------------------------------------------------------------------
  public:
    ScaledDehnenModel(double,                      // I: gamma                  
		      double,                      // I: scale radius           
		      double,                      // I: total mass             
		      double=1.e-7);               //[I: numerical precision]   
    //--------------------------------------------------------------------------
    // const data access                                                        
    //--------------------------------------------------------------------------
    DehnenModel::gamma;
    double const&scale_radius() const { return sR; }
    double const&total_mass  () const { return sM; }
    //--------------------------------------------------------------------------
    // potential, density, and cumulative mass                                  
    //                                                                          
    //   note:  y   := x/(x+1); x=r/a                                           
    //          Psi := -Phi                                                     
    //--------------------------------------------------------------------------
    template<mass> double Y (double) const;
    template<mass> double R (double) const;
    template<mass> double M (double) const;
    template<mass> double Ps(double) const;
    template<mass> double Rh(double) const;
    //--------------------------------------------------------------------------
    // circular orbits                                                          
    //--------------------------------------------------------------------------
    template<circ> double Yc(double) const;
    template<circ> double Rc(double) const;
    template<circ> double Vc(double) const;
    template<circ> double Vq(double) const;
    template<circ> double Lc(double) const;
    template<circ> double Lq(double) const;
    template<circ> double Ec(double) const;
    template<circ> double Oq(double) const;
    template<circ> double Kq(double) const;
    template<circ> double Gm(double) const;
    //--------------------------------------------------------------------------
    // non-circular orbits                                                      
    //--------------------------------------------------------------------------
    double RofELC (double, double, double) const;  // R(E,L,cos(eta))           
    //--------------------------------------------------------------------------
    // isotropic velocity dispersion                                            
    //--------------------------------------------------------------------------
    template<mass> double sigq(double) const;
    //--------------------------------------------------------------------------
    // projected quantities                                                     
    //--------------------------------------------------------------------------
    double SurfaceDensity    (double) const;
    double DSurfaceDensityDR (double) const;
    double CumSurfaceDensity (double) const;
    double EffectiveRadius   ()       const;
    double SigIsotropicProj  (double) const;
    double SigCircProj       (double) const;
    //--------------------------------------------------------------------------
    // distributions                                                            
    //--------------------------------------------------------------------------
    double Fsub(double, double) const;
    double F(double) const;                        // f(E): isotropic DF        
    double G(double) const;                        // g(E): density of states   
    double F(double, double) const;                // f(Q,ra): OM DF            
  };
  //////////////////////////////////////////////////////////////////////////////
  //                                                                          //
  // class DehnenModelSampler                                                 //
  //                                                                          //
  // implements a SphericalSampler using a ScaledDehnenModel                  //
  //                                                                          //
  //////////////////////////////////////////////////////////////////////////////
  class DehnenModelSampler : 
    public ScaledDehnenModel,
    public SphericalSampler
  {
    //--------------------------------------------------------------------------
    // data                                                                     
    //--------------------------------------------------------------------------
    const int     n;
    double        *y, *f;
    const double  fi;                              // at y<<1: f=c* y_E^fi      
    const double  fo;                              // at y~=1: f=c* (1-y_E)^fo  
    //--------------------------------------------------------------------------
    // public methods                                                           
    //--------------------------------------------------------------------------
  public:
    DehnenModelSampler(double const&,              // I: gamma                  
		       double const&,              // I: scale radius           
		       double const&,              // I: GM (untruncated)       
#ifdef falcON_PROPER
		       double const& = 0.,         //[I: anisotropy radius]     
#endif
		       double const& = 0.,         //[I: maximum radius]        
		       int    const& = 9999,       //[I: # points on table f(y)]
		       double const& = 1.e-8       //[I: numerical precision]   
#ifdef falcON_PROPER
		      ,const double* = 0,          //[I: mass adaption: radii]  
		       int    const& = 0,          //[I: mass adaption: # --]   
		       double const& = 1.2,        //[I: mass adaption: factor] 
		       bool   const& = 0           //[I: mass adaption: R_-/Re] 
#endif
		       );
    ~DehnenModelSampler(); 
    //--------------------------------------------------------------------------
    // provide the abstract functions and more                                  
    //--------------------------------------------------------------------------
    double DF (double) const;                      // f(Q)                      
    double Ra (double, double) const;              // R_apo (Eps,L)             
    double Rp (double, double) const;              // R_peri(Eps,L)             
    double Re (double) const;                      // R_circ(Eps)               
    double Ps (double) const;                      // Psi(r)                    
    double Mr (double) const;                      // M(<r)                     
    double rM (double) const;                      // r(M)                      
    double Psy(double) const;                      // Psi(y)                    
    //--------------------------------------------------------------------------
  };
  //////////////////////////////////////////////////////////////////////////////
  //                                                                          //
  // inlined member methods of DehnenModel                                    //
  //                                                                          //
  //////////////////////////////////////////////////////////////////////////////
  inline DehnenModel::DehnenModel(double gamma, double e) :
    g   (gamma),
    g1  (1-g),
    g2  (2-g),
    ig2 (g2? 1/g2 : 0),
    g3  (3-g),
    ig3 (1/g3),
    g3f (g3/12.56637061435917295385057353312),
    g4  (4-g),
    eps (e)
  {
    if(gamma<0. || gamma>3.) error("DehnenModel: gamma out of range");
  }
  //----------------------------------------------------------------------------
  // potential, density, and cumulative mass                                    
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Y <DehnenModel::ym>(double y) const
  {
    return y;
  }
  template<> inline double DehnenModel::Y <DehnenModel::xm>(double x) const
  {
    return x/(1+x);
  }
  template<> inline double DehnenModel::Y <DehnenModel::ps>(double p) const
  { 
    if(p==0.) return 1.;
    if(g==2.) return exp(-p);
    register double pg2=p*g2;
    if(g2>0. && pg2>1.) error("DehnenModel: psi out of range");
    return pow(1.-pg2, ig2);
  }
  template<> inline double DehnenModel::Y <DehnenModel::mm>(double m) const
  {
    return pow(m,ig3);
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::X <DehnenModel::xm>(double x) const
  {
    return x;
  }
  template<> inline double DehnenModel::X <DehnenModel::ym>(double y) const
  {
    return y/(1-y);
  }
  template<DehnenModel::mass __M> inline double DehnenModel::X(double a) const
  {
    return X <ym>(Y<__M>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Ps<DehnenModel::ps>(double p) const
  {
    return p;
  }
  template<> inline double DehnenModel::Ps<DehnenModel::ym>(double y) const { 
    if(y==0. && g>=2.) error("DehnenModel: potential diverges at r=0");
    if(g==2.) return -std::log(y);
    return (y==0.) ?  ig2 : ig2*(1.-pow(y,g2));
  }
  template<DehnenModel::mass __M> inline double DehnenModel::Ps(double a) const
  {
    return Ps<ym>(Y<__M>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::M <DehnenModel::mm>(double m) const
  {
    return m;
  }
  template<> inline double DehnenModel::M <DehnenModel::ym>(double y) const
  {
    return pow(y,g3);
  }
  template<DehnenModel::mass __M> inline double DehnenModel::M (double a) const
  {
    return M <ym>(Y<__M>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Rh<DehnenModel::ym>(double y) const
  { 
    if(g==0.) return iFPit*power<4>(1-y);
    if(y==0.) error("DehnenModel: density diverges at r=0");
    return g3f*pow(y,-g)*power<4>(1-y);
  }
  template<DehnenModel::mass __M> inline double DehnenModel::Rh(double a) const
  {
    return Rh<ym>(Y<__M>(a));
  }
  //----------------------------------------------------------------------------
  // falcON::DehnenModel: circular orbits                                       
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Yc<DehnenModel::yc>(double y) const
  {
    return y;
  }
  template<> inline double DehnenModel::Yc<DehnenModel::xc>(double x) const
  {
    return x/(1+x);
  }
  template<> inline double DehnenModel::Yc<DehnenModel::lc>(double l) const
  {
    return YcofLq(l*l);
  }
  template<> inline double DehnenModel::Yc<DehnenModel::lq>(double q) const
  {
    return YcofLq(q);
  }
  template<> inline double DehnenModel::Yc<DehnenModel::ec>(double e) const
  {
    return YcofE (e);
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Xc<DehnenModel::xc>(double x) const
  {
    return x;
  }
  template<> inline double DehnenModel::Xc<DehnenModel::yc>(double y) const
  {
    return y/(1-y);
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Xc(double a) const
  {
    return Xc<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Vq<DehnenModel::yc>(double y) const
  {
    return g2==0? 1-y : pow(y,g2)*(1-y);
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Vq(double a) const
  {
    return Vq<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Vc<DehnenModel::yc>(double y) const
  {
    return sqrt(Vq<yc>(y));
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Vc(double a) const
  {
    return Vc<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Lq<DehnenModel::lc>(double l) const
  {
    return l*l;
  }
  template<> inline double DehnenModel::Lq<DehnenModel::lq>(double q) const
  {
    return q;
  }
  template<> inline double DehnenModel::Lq<DehnenModel::yc>(double y) const
  {
    return pow(y,g4)/(1-y);
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Lq(double a) const
  {
    return Lq<yc>(Yc<__C>(a)); 
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Lc<DehnenModel::lc>(double l) const
  {
    return l;
  }
  template<> inline double DehnenModel::Lc<DehnenModel::yc>(double y) const
  {
    return sqrt(Lq<yc>(y));
  }
  template<> inline double DehnenModel::Lc<DehnenModel::lq>(double q) const
  {
    return sqrt(q);
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Lc(double a) const
  {
    return Lc<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Ec<DehnenModel::ec>(double e) const
  {
    return e;
  }
  template<> inline double DehnenModel::Ec<DehnenModel::yc>(double y) const
  {
    return g2==0?
      log(y) - 0.5*(1-y) :
      ig2 + pow(y,g2)*(0.5*(y-1)-ig2);
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Ec(double a) const
  {
    return Ec<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Oq<DehnenModel::yc>(double y) const
  {
    return pow(y,-g)*power<3>(1-y);
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Oq(double a) const
  {
    return Oq<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Kq<DehnenModel::yc>(double y) const
  {
    return Oq<yc>(y)*(g4-g3*y);
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Kq(double a) const
  {
    return Kq<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline double DehnenModel::Gm<DehnenModel::yc>(double y) const
  {
    return 2./sqrt(g4-g3*y);
  }
  template<DehnenModel::circ __C> inline double DehnenModel::Gm(double a) const
  {
    return Gm<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  // non-circular orbits                                                        
  //----------------------------------------------------------------------------
  inline double DehnenModel::XofELC(double e, double l, double cet) const
  {
    register double ye = Yc<ec>(e);
    return Xc<yc>(ye) * pow(1 + cet*sqrt(1-l*l/Lq<yc>(ye)),0.5*Gm<yc>(ye));
  }
  //----------------------------------------------------------------------------
  // isotropic velocity dispersion                                              
  //----------------------------------------------------------------------------
  template<DehnenModel::mass __M> inline
  double DehnenModel::sigq(double a) const
  {
    return SigIsoQ(X<__M>(a));
  }
  //////////////////////////////////////////////////////////////////////////////
  //                                                                          //
  // inlined member methods of ScaledDehnenModel                              //
  //                                                                          //
  //////////////////////////////////////////////////////////////////////////////
  inline ScaledDehnenModel::ScaledDehnenModel(double g,
					      double a,
					      double m,
					      double e) :
    DehnenModel ( g, e ), 
    sR ( a             ), iR ( 1./sR ),
    sM ( m             ), iM ( 1./sM ),
    sE ( sM/sR         ), iE ( 1./sE ),
    sV ( sqrt(sE)      ), iV ( 1./sV ),
    sL ( sR*sV         ), iL ( 1./sL ),
    sQ ( sL*sL         ), iQ ( 1./sQ ),
    sF ( sM/(sL*sL*sL) ), iF ( 1./sF ),
    sD ( sM/(sR*sR*sR) ), iD ( 1./sD ),
    sO ( sV*sV/(sR*sR) ), iO ( 1./sO ) {}
  //----------------------------------------------------------------------------
  // potential, density, and cumulative mass                                    
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Y <DehnenModel::ym>(double y) const
  {
    return y;
  }
  template<> inline
  double ScaledDehnenModel::Y <DehnenModel::xm>(double r) const
  {
    return DehnenModel::Y<xm>(r*iR);
  }
  template<> inline
  double ScaledDehnenModel::Y <DehnenModel::ps>(double p) const
  {
    return DehnenModel::Y<ps>(p*iE);
  }
  template<> inline
  double ScaledDehnenModel::Y <DehnenModel::mm>(double m) const
  {
    return DehnenModel::Y<mm>(m*iM);
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::R <DehnenModel::xm>(double r) const
  {
    return r;
  }
  template<> inline
  double ScaledDehnenModel::R <DehnenModel::ym>(double y) const
  {
    return sR*DehnenModel::X <ym>(y);
  }
  template<DehnenModel::mass __M> inline
  double ScaledDehnenModel::R(double a) const
  {
    return R <ym>(Y<__M>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::M <DehnenModel::mm>(double m) const
  {
    return m;
  }
  template<> inline
  double ScaledDehnenModel::M <DehnenModel::ym>(double y) const
  {
    return sM*DehnenModel::M <ym>(y);
  }
  template<DehnenModel::mass __M> inline
  double ScaledDehnenModel::M(double a) const
  {
    return M <ym>(Y<__M>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Ps<DehnenModel::ps>(double p) const
  {
    return p;
  }
  template<> inline
  double ScaledDehnenModel::Ps<DehnenModel::ym>(double y) const
  {
    return sE*DehnenModel::Ps<ym>(y);
  }
  template<DehnenModel::mass __M> inline
  double ScaledDehnenModel::Ps(double a) const
  {
    return Ps<ym>(Y<__M>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Rh<DehnenModel::ym>(double y) const
  {
    return sD*DehnenModel::Rh<ym>(y);
  }
  template<DehnenModel::mass __M> inline
  double ScaledDehnenModel::Rh(double a) const
  {
    return Rh<ym>(Y<__M>(a));
  }
  //----------------------------------------------------------------------------
  // circular orbits                                                            
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Yc<DehnenModel::yc>(double y) const
  {
    return y;
  }
  template<> inline
  double ScaledDehnenModel::Yc<DehnenModel::xc>(double r) const
  {
    return DehnenModel::Y<xm>(r*iR);
  }
  template<> inline
  double ScaledDehnenModel::Yc<DehnenModel::lc>(double l) const
  {
    return DehnenModel::Yc<lc>(l*iL);
  }
  template<> inline
  double ScaledDehnenModel::Yc<DehnenModel::lq>(double q) const
  {
    return DehnenModel::Yc<lq>(q*iQ);
  }
  template<> inline
  double ScaledDehnenModel::Yc<DehnenModel::ec>(double e) const
  {
    return DehnenModel::Yc<ec>(e*iE);
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Rc<DehnenModel::xc>(double r) const
  {
    return r;
  }
  template<> inline
  double ScaledDehnenModel::Rc<DehnenModel::yc>(double y) const
  {
    return sR*DehnenModel::Xc<yc>(y);
  }
  template<DehnenModel::circ __C> inline
  double ScaledDehnenModel::Rc(double a) const
  {
    return Rc<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Vc<DehnenModel::yc>(double y) const
  {
    return sV*DehnenModel::Vc<yc>(y);
  }
  template<DehnenModel::circ __C> inline
  double ScaledDehnenModel::Vc(double a) const
  {
    return Vc<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Vq<DehnenModel::yc>(double y) const
  {
    return sE*DehnenModel::Vq<yc>(y);
  }
  template<DehnenModel::circ __C> inline
  double ScaledDehnenModel::Vq(double a) const
  {
    return Vq<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Lc<DehnenModel::lc>(double l) const
  {
    return l;
  }
  template<> inline
  double ScaledDehnenModel::Lc<DehnenModel::lq>(double q) const
  {
    return sqrt(q);
  }
  template<> inline
  double ScaledDehnenModel::Lc<DehnenModel::yc>(double y) const
  {
    return sL*DehnenModel::Lc<yc>(y);
  }
  template<DehnenModel::circ __C> inline 
  double ScaledDehnenModel::Lc(double a) const
  {
    return Lc<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Lq<DehnenModel::lc>(double l) const
  {
    return l*l;
  }
  template<> inline
  double ScaledDehnenModel::Lq<DehnenModel::lq>(double q) const
  {
    return q;
  }
  template<> inline 
  double ScaledDehnenModel::Lq<DehnenModel::yc>(double y) const
  {
    return sQ*DehnenModel::Lq<yc>(y);
  }
  template<DehnenModel::circ __C> inline
  double ScaledDehnenModel::Lq(double a) const
  {
    return Lq<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Ec<DehnenModel::ec>(double e) const
  {
    return e;
  }
  template<> inline
  double ScaledDehnenModel::Ec<DehnenModel::yc>(double y) const
  {
    return sE*DehnenModel::Ec<yc>(y);
  }
  template<DehnenModel::circ __C> inline
  double ScaledDehnenModel::Ec(double a) const
  {
    return Ec<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Oq<DehnenModel::yc>(double y) const
  {
    return sO*DehnenModel::Oq<yc>(y);
  }
  template<DehnenModel::circ __C> inline
  double ScaledDehnenModel::Oq(double a) const
  {
    return Oq<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Kq<DehnenModel::yc>(double y) const
  {
    return sO*DehnenModel::Kq<yc>(y);
  }
  template<DehnenModel::circ __C> inline
  double ScaledDehnenModel::Kq(double a) const
  {
    return Kq<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::Gm<DehnenModel::yc>(double y) const
  {
    return    DehnenModel::Gm<yc>(y);
  }
  template<DehnenModel::circ __C> inline
  double ScaledDehnenModel::Gm(double a) const
  {
    return Gm<yc>(Yc<__C>(a));
  }
  //----------------------------------------------------------------------------
  // non-circular orbits                                                        
  //----------------------------------------------------------------------------
  inline double ScaledDehnenModel::RofELC(double e, double l, double c) const
  {
    return sR * DehnenModel::XofELC(e*iE,l*iL,c);
  }
  //----------------------------------------------------------------------------
  // isotropic velocity dispersion                                              
  //----------------------------------------------------------------------------
  template<> inline
  double ScaledDehnenModel::sigq<DehnenModel::ym>(double y) const
  {
    return sE*DehnenModel::sigq<ym>(y);
  }
  template<> inline
  double ScaledDehnenModel::sigq<DehnenModel::xm>(double r) const
  {
    return sE*DehnenModel::sigq<xm>(r*iR);
  }
  template<> inline
  double ScaledDehnenModel::sigq<DehnenModel::ps>(double p) const
  {
    return sE*DehnenModel::sigq<ps>(p*iE);
  }
  template<> inline
  double ScaledDehnenModel::sigq<DehnenModel::mm>(double m) const
  {
    return sE*DehnenModel::sigq<mm>(m*iM);
  }
  //----------------------------------------------------------------------------
  // projected quantities                                                       
  //----------------------------------------------------------------------------
  inline double ScaledDehnenModel::SurfaceDensity(double r) const
  {
    return sR*sD*DehnenModel::SurfaceDensity(r*iR);
  }
  //----------------------------------------------------------------------------
  inline double ScaledDehnenModel::DSurfaceDensityDR(double r) const
  {
    return sD*DehnenModel::DSurfaceDensityDR(r*iR);
  }
  //----------------------------------------------------------------------------
  inline double ScaledDehnenModel::CumSurfaceDensity(double r) const
  {
    return sM*DehnenModel::CumSurfaceDensity(r*iR);
  }
  //----------------------------------------------------------------------------
  inline double ScaledDehnenModel::EffectiveRadius() const
  {
    return sR*DehnenModel::EffectiveRadius();
  }
  //----------------------------------------------------------------------------
  inline double ScaledDehnenModel::SigIsotropicProj(double r) const
  {
    return sV*DehnenModel::SigIsotropicProj(r*iR);
  }
  //----------------------------------------------------------------------------
  inline double ScaledDehnenModel::SigCircProj(double r) const
  {
    return sV*DehnenModel::SigCircProj(r*iR);
  }
  //----------------------------------------------------------------------------
  // distributions                                                              
  //----------------------------------------------------------------------------
  inline double ScaledDehnenModel::F(double e) const
  {
    return sF*DehnenModel::F(e*iE);
  }
  inline double ScaledDehnenModel::Fsub(double e, double G) const
  {
    return sF*DehnenModel::Fsub(e*iE,G);
  }
  inline double ScaledDehnenModel::G(double e) const
  {
    return sR*sR*sL*DehnenModel::G(e*iE);
  }
  inline double ScaledDehnenModel::F(double e, double r) const
  {
    return sF*DehnenModel::F(e*iE,r*iR);
  }
  //////////////////////////////////////////////////////////////////////////////
  //                                                                          //
  // inlined member methods of DehnenModelSampler                             //
  //                                                                          //
  //////////////////////////////////////////////////////////////////////////////
  inline DehnenModelSampler::~DehnenModelSampler() { 
    delete[] y;
    delete[] f;
  } 
  //----------------------------------------------------------------------------
  // provide the abstract functions and more                                    
  //----------------------------------------------------------------------------
  inline double DehnenModelSampler::DF (double e) const {
    register double ye = ScaledDehnenModel::Y<DehnenModel::ps>(e);
    if(ye >= 1.   ) return 0.;
    if(ye < y[0]  ) return exp(f[0]   + fi*log(ye/y[0]) );
    if(ye > y[n-1]) return exp(f[n-1] + fo*log((1-ye)/(1-y[n-1])) );
    return exp(polev(ye,y,f,n));
  }
  //----------------------------------------------------------------------------
  inline double DehnenModelSampler::Ra(double e, double l) const
  {
    return ScaledDehnenModel::RofELC(e,l,+1.);
  }
  //----------------------------------------------------------------------------
  inline double DehnenModelSampler::Rp(double e, double l) const
  {
    return ScaledDehnenModel::RofELC(e,l,-1.);
  }
  //----------------------------------------------------------------------------
  inline double DehnenModelSampler::Re(double e) const
  {
    return ScaledDehnenModel::Rc<DehnenModel::ec>(e);
  }
  //----------------------------------------------------------------------------
  inline double DehnenModelSampler::Ps(double r) const
  {
    return ScaledDehnenModel::Ps<DehnenModel::xm>(r);
  }
  //----------------------------------------------------------------------------
  inline double DehnenModelSampler::Psy(double y) const
  {
    return ScaledDehnenModel::Ps<DehnenModel::ym>(y);
  }
  //----------------------------------------------------------------------------
  inline double DehnenModelSampler::Mr(double r) const
  {
    return ScaledDehnenModel::M <DehnenModel::xm>(r);
  }
  //----------------------------------------------------------------------------
  inline double DehnenModelSampler::rM(double m) const
  {
    return ScaledDehnenModel::R <DehnenModel::mm>(m);
  }
  //////////////////////////////////////////////////////////////////////////////
} // namespace falcON {
////////////////////////////////////////////////////////////////////////////////
#endif // falcON_included_gamma_h
