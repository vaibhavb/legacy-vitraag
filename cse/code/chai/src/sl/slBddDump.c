/**CFile***********************************************************************

  FileName    [slBddDump.c]

  PackageName [sl]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [Freddy Mang]

  Copyright   [Copyright (c) 1994-1996 The Regents of the Univ. of California.
  All rights reserved.

  Permission is hereby granted, without written agreement and without license
  or royalty fees, to use, copy, modify, and distribute this software and its
  documentation for any purpose, provided that the above copyright notice and
  the following two paragraphs appear in all copies of this software.

  IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
  CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
  "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.]

******************************************************************************/

#include "slInt.h"
#include "dddmp.h"
#include "cuPortInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Dumping a BDD into a file.]

  Description        []

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlDumpBddCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv
  )
{
  char usage[] = "Usage: sl_dump_bdd [-ha] [-f <filename>] <bdd>";
  char c, *fname = NIL(char);
  array_t * ddArray; 
  boolean isBinaryMode = TRUE; /* the default is always binary */
  int i;
  FILE * fp;
  char * ddname;
  int nroots;
  int * auxids;
  int mode;
  int varinfo;
  DdNode ** f;
  Sl_GSTEntry_t* entry;
  char ** rootnames, **varnames;
  DdManager * manager;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "haf:")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaPrint("%s\n", usage);
          return TCL_OK;
          break;
        case 'a':
          isBinaryMode = FALSE;
          break;
        case 'f':
          fname = util_optarg;
          break;
        default:
          Main_MochaPrint("%s\n", usage);
          return TCL_ERROR;
          break;
    }
  }

  if (util_optind == argc) {
    Main_MochaPrint("%s\n", usage);
    return TCL_ERROR;
  }
  
  entry = SlGstGetEntry(argv[util_optind]);
  if (!GSTEntryIsBdd(entry)){
    Main_MochaErrorPrint("Error: BDD \"%s\" does not exists.\n",
                         argv[util_optind]);
    return TCL_OK;
  }
  
  if (!fname) {

    Main_MochaPrint(
      "No output file specified. Output to stdout in ASCII mode.\n");
    
    isBinaryMode = FALSE;
    fp = stdout;
  } else {
    if (isBinaryMode)
      fp = fopen(fname, "wb");
    else
      fp = fopen(fname, "w");
  }
  
  ddArray   = entry -> obj.bddArray;
  ddname    = entry -> name;
  nroots    = array_n(ddArray);
  rootnames = NIL(char *);
  varnames  = NIL(char *);
  auxids    = NIL(int);
  mode      = isBinaryMode? DDDMP_MODE_BINARY: DDDMP_MODE_TEXT;
  varinfo   = DDDMP_MODE_DEFAULT;
  fname     = NIL(char);
  f         = ALLOC(DdNode *, nroots);
  
  for (i=0; i<nroots; i++) {
    mdd_t * mdd = array_fetch(mdd_t *, ddArray, i);
    manager = mdd -> mgr;
    *(f+i) = mdd -> node;
  }

  Dddmp_cuddBddArrayStore(manager,
                          ddname,
                          nroots,
                          f,
                          rootnames,
                          varnames,
                          auxids,
                          mode,
                          varinfo,
                          fname,
                          fp);

  if (fp != stdout)
    fclose (fp);

  return TCL_OK;
}


