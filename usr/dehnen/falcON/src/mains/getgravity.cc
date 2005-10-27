// -*- C++ -*-                                                                 |
//-----------------------------------------------------------------------------+
//                                                                             |
// getgravity.cc                                                               |
//                                                                             |
// Copyright (C) 2002, 2003, 2005 Walter Dehnen                                |
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
//                                                                             |
// history:                                                                    |
//                                                                             |
// v 0.0    23/11/2002  WD created.                                            |
// v 0.1    04/02/2003  WD default falcON parameters automized                 |
// v 0.2    20/03/2003  WD gravity, action reporting                           |
// v 0.3    23/05/2003  WD automated NEMO history                              |
// v 1.0    20/05/2005  WD several minor updates                               |
// v 2.0    14/06/2005  WD new falcON, new body.h, new nemo I/O                |
// v 2.1    22/06/2005  WD changes in nemo I/O support                         |
// v 2.2    13/06/2005  WD changes in fieldset                                 |
//-----------------------------------------------------------------------------+
#define falcON_VERSION   "2.2"
#define falcON_VERSION_D "13-jul-2005 Walter Dehnen                          "
//-----------------------------------------------------------------------------+
#ifndef falcON_NEMO
#error You need NEMO to compile "src/mains/getgravity.cc"
#endif
#include <body.h>                                  // bodies etc..              
#include <FAlCON.h>                                // fakcIB                    
#include <public/io.h>                             // WDs C++ NEMO I/O          
#include <main.h>                                  // main & NEMO stuff         
//------------------------------------------------------------------------------
string defv[] = {
  "srce=???\n          input file: sources [m,x]       ",
  "sink=???\n          input file: sinks   [x]         ",
  "out=???\n           output file         [x,a,p]     ",
  "times=all\n         time range (for srce only)      ",
  "eps=0.05\n          softening length                ",
  "kernel="falcON_KERNEL_TEXT
  "\n                  softening kernel                ",
  "theta="falcON_THETA_TEXT
  "\n                  tolerance parameter at M=M_tot  ",
  "Ncrit="falcON_NCRIT_TEXT
  "\n                  max # bodies in un-split cells  ",
  falcON_DEFV, NULL };
//------------------------------------------------------------------------------
string usage="getgravity -- computes gravity at sink positions; using falcON";
//------------------------------------------------------------------------------
void falcON::main() falcON_THROWING
{
  nemo_in        srce(getparam("srce"));
  nemo_out       out(getparam("out"));
  const fieldset write(fieldset::x | fieldset::p | fieldset::a);
  snapshot       ssht(0,fieldset::gravity);
  FAlCON         falcon(&ssht,getrparam("eps"), getrparam("theta"),
			kern_type(getiparam("kernel")));
  const fieldset srcedata(fieldset::m|fieldset::x);
  while(srce.has_snapshot()) {
    // open snapshot with sources and check for time in range, if both given
    snap_in srce_in(srce);
    if(srce_in.has_time() && !time_in_range(srce_in.time(),getparam("times")))
      continue;
    // open snapshot with sinks and ensure we have enough bodies
    nemo_in sink   (getparam("sink"));
    snap_in sink_in(sink);
    ssht.resetN( srce_in.Nbod() + sink_in.Nbod() );
    // read sources
    const body sources(ssht.begin_all_bodies());
    fieldset read = ssht.read_nemo(srce_in, srcedata, sources);
    if(!read.contain(srcedata))
      falcON_THROW("sources must have mx data");
    // read sinks
    const body sinks(sources, srce_in.Nbod());
    read = ssht.read_nemo(sink_in, fieldset::x, sinks);
    if(!read.contain(fieldbit::x))
      falcON_THROW("sinks must have x data");
    ssht.set_time(srce_in.time());
    // loop sources, get their mass and flag them to be inactive
    real M(zero);
    for(body b(sources); b!=sinks; ++b) {
      b.unflag_active();
      M += mass(b);
    }
    // loop sinks, set their mass and flag them to be active
    M *= 1.e-10/real(srce_in.Nbod());
    for(body b(sinks); b; ++b) {
      b.flag_as_active();
      b.mass() = M;
    }
    // compute gravity
    falcon.grow(getiparam("Ncrit"));
    falcon.approximate_gravity();
    // write sink data to output
    if(out)
      ssht.write_nemo(out,fieldset(fieldset::x|fieldset::a|fieldset::p),sinks);
  }
}
