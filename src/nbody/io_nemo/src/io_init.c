/* -------------------------------------------------------------- *\
|* $Id$
|*
|* Initialyze io_nemo data
|*
\* -------------------------------------------------------------- */

/* -------------------------------------------------------------- *\
|* Nemo include files
\* -------------------------------------------------------------- */
#include <stdinc.h>
#include <getparam.h>
#include <history.h>

/* -------------------------------------------------------------- *\
|* Local include files
\* -------------------------------------------------------------- */
#include "io_init.h"
#include "io_nemo_tools.h"

/* flags parameters (EXPORTED) */
int 
  N_io ,  /* nbody              */
  T_io ,  /* time               */
  M_io ,  /* mass               */
  X_io ,  /* position           */
  V_io ,  /* velocity           */ 
  P_io ,  /* potential          */
  A_io ,  /* acceleration       */
  K_io ,  /* key                */
  XV_io,  /* phasespace         */
  C_io ,  /* close              */
  I_io ,  /* info               */
  H_io ,  /* history            */
  ST_io,  /* selected time      */
  SP_io,  /* selected particles */
  F_dim;  /* fortran array      */

/* -------------------------------------------------------------- *\ 
|* init_io_one :
|* Initilalize the NEMO engine and some variables.
\* -------------------------------------------------------------- */
void init_io_one(bool * read_one,
		 bool * save_one,
		 bool * set_history,
		 char **history_prog,
		 int    MAXIO)
{ 
  string defv[] = { "none=none","VERSION=1.3",NULL };
  string argv[] = { "IO_NEMO",NULL };
  int i;
  string * histo;

  initparam(argv,defv); 
  
  /* initialyze files control arrays */
  for (i=0; i< MAXIO; i++) {
    read_one[i]    = FALSE;
    save_one[i]    = FALSE;
    set_history[i] = FALSE;
  }

  /* get command line history */
  histo         = (char **) ask_history();
  *history_prog = (char * ) allocate_pointer(*history_prog,
					     sizeof(char)*(strlen(histo[0])+1));
  /* save command line history */
  strcpy(*history_prog, histo[0]);

  /*fprintf(stderr,"history prog : <%s> \n", *history_prog); */
}

/* -------------------------------------------------------------- *\ 
|* init_flag_io : 
|* Setup io_nemo's flags
\* -------------------------------------------------------------- */
void init_flag_io()
{
  N_io= T_io= M_io= X_io= V_io= P_io= A_io= C_io = H_io=0;
  K_io= XV_io = F_dim= ST_io= SP_io= I_io= 0;
}
/* -------------------------------------------------------------- *\
|* End of io_init.c
\* -------------------------------------------------------------- */
