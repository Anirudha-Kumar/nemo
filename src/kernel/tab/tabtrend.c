/*
 * TABTREND: difference rows from previous rows
 *          
 *   12-may-05   Created quick & dirty for CARMA               PJT
 *   15-jun-2016 cumul= option
 *   23-may-2020 orig=T/F option
 *   17-mar-2021 add first= option
 *                        
 * 
 * TODO:
 *     option to do dual-pass to subtract the mean before computing
 *     the higher order moments - needed for accuracy
 */

/**************** INCLUDE FILES ********************************/ 

#include <stdinc.h>
#include <strlib.h>
#include <getparam.h>
#include <moment.h>
#include <yapp.h>
#include <table.h>
#include <axis.h>
#include <mdarray.h>

/**************** COMMAND LINE PARAMETERS **********************/

string defv[] = {
    "in=???\n                     Input file name",
    "xcol=1\n			  Column(s) to use",
    "nmax=100000\n                max size if a pipe",
    "cumul=f\n                    cumulative instead?",
    "orig=f\n                     show original column as well?",
    "first=f\n                    add first row?",
    "VERSION=0.4\n		  17-mar-2021 PJT",
    NULL
};

string usage = "difference rows from previous rows, or cumulate them";

string cvsid = "$Id$";

/**************** SOME GLOBAL VARIABLES ************************/

#ifndef MAXHIST
#define MAXHIST	1024
#endif

#ifndef MAXCOL
#define MAXCOL 256
#endif

#define MAXCOORD 16

local string input;			/* filename */
local stream instr;			/* input file */

local int ncol;                         /* number of columns used */
local int col[MAXCOL];			/* histogram column number(s) */

real *coldat[MAXCOL];
local int    nmax;			/* lines to use at all */
local int    npt;			/* actual number of points */

local bool   Qcumul;
local bool   Qorig;
local bool   Qfirst;


local void setparams(void);
local void read_data(void); 
local void trend_data(void);
local void cumul_data(void);



/****************************** START OF PROGRAM **********************/

void nemo_main(void)
{
    setparams();			/* read the parameters */
    read_data();
    if (Qcumul)
      cumul_data();
    else
      trend_data();
}

local void setparams()
{
    input = getparam("in");
    ncol = nemoinpi(getparam("xcol"),col,MAXCOL);
    if (ncol < 0) error("parsing error col=%s",getparam("col"));
    
    nmax = nemo_file_lines(input,getiparam("nmax"));
    if (nmax<1) error("Problem reading from %s",input);

    instr = stropen (input,"r");

    Qcumul = getbparam("cumul");
    Qorig = getbparam("orig");
    Qfirst = getbparam("first");
}



local void read_data()
{
    int   i,j,k;
    
    dprintf(0,"Reading %d column(s)\n",ncol);
    for (i=0; i<ncol; i++)
      coldat[i] = (real *) allocate(sizeof(real)*nmax);
    npt = get_atable(instr,ncol,col,coldat,nmax);        /* read it */
    if (npt == -nmax) {
    	warning("Could only read %d data",nmax);
    	npt = nmax;
    }
}


local void trend_data(void)
{
  int i,j;

  for (j=0; j<ncol;  j++) {  
    if (Qfirst) {
      if (Qorig)
	printf("%g %g ",coldat[j][0],coldat[j][0]);
      else
	printf("%g ",coldat[j][0]);
      printf("\n");       
    }
  }

  for (i=1; i<npt; i++) {
    for (j=0; j<ncol;  j++) {
      if (Qorig)
	printf("%g %g ",coldat[j][i]-coldat[j][i-1],coldat[j][i-1]);
      else
	printf("%g ",coldat[j][i]-coldat[j][i-1]);
      printf("\n");
    }
  }
}

local void cumul_data(void)
{
  int i,j;
  real sum[MAXCOL];

  for (i=0; i<npt; i++) {
    if (i==0)
      for (j=0; j<ncol;  j++)
	sum[j] = -coldat[j][0];
	  
    for (j=0; j<ncol;  j++) {
	sum[j] += coldat[j][i];
	printf("%g ",sum[j]);
    }
    printf("\n");
  }
}

