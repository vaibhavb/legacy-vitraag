/**CFile***********************************************************************

  FileName    [mainMain.c]

  PackageName [main]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [Freddy Mang]

  Copyright   [Copyright (c) 1994-2001 The Regents of the Univ. of California.
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

#include "mainInt.h" 


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
/**Function********************************************************************

  Synopsis           [Main function called when Mocha is invoked.]

  Description        [This routine parses the command line arguments when
  Mocha is called. Then it either invokes Tcl alone (only command line input is
  activated), or with Tk (graphical user interface is enabled).]

  SideEffects        [Mocha Packages are initialized.]

  SeeAlso            [TclAppInit, TkAppInit]

******************************************************************************/
int main(
  int argc,
  char **argv)
{
  char c;
  int argcForTclTk = 0;
  char *MainSrcFile = NIL(char);
  char *argvForTclTk[3];
  
  util_getopt_reset();

  /* by default, mocha uses the text interface. */
  MainTkEnabled = 0;		/* Luca changed it. */

  /* By default, we use the text interface */ 
  while ((c=util_getopt(argc, argv, "t")) != -1) {
    switch (c) {
        case 't':
          MainTkEnabled = 0;
          break;
        default:
          fprintf(stderr, "Usage: mocha [-t] <filename>\n");
          exit(1);
          break;
    }
  }

  if (argc - util_optind != 0) {
    MainSrcFile = argv[util_optind];
  }
  
  /* form the argc and argv and call tcl/tk */
  argvForTclTk[0] = argv[0];
  if (MainSrcFile) {
    argvForTclTk[1] = MainSrcFile;
    argvForTclTk[2] = NIL(char);
    argcForTclTk = 2;
  } else {
    argvForTclTk[1] = NIL(char);
    argcForTclTk = 1;
  }

  if (MainTkEnabled == 1){
    Tk_Main(argcForTclTk, argvForTclTk, MainTkAppInit);
  }
  else {
    Tcl_Main(argcForTclTk, argvForTclTk, MainTclAppInit);
  }

  return 1;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/




