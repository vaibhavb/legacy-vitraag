/**CFile***********************************************************************

  FileName    [mainUtil.c]

  PackageName [main]

  Synopsis    [required]

  Description [Utility functions useful for mocha]

  SeeAlso     [optional]

  Author      [Freddy Mang]

  Copyright   [Copyright (c) 1994-2000 The Regents of the Univ. of California.
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

  Synopsis           [Convert an integer into string.]

  Description        [This function converts an integer (both positive and
  negative) into a string. It is the inverse of the library function atoi.]

  SideEffects        [User should free the string after use.]

  SeeAlso            [optional]

******************************************************************************/
char *
Main_ConvertIntToString(
  int i)
{
  char * result;
  int isNeg;
  int numdigits = 0;
  int num = i;
  
  isNeg = (i < 0);

  if (isNeg)
    num = -num;

  if (num == 0)
    numdigits = 1;
  else 
    while (num > 0) {
      numdigits ++;
      num = num/10;
    }

  result = ALLOC(char, numdigits + 1 + isNeg);
  if (isNeg)
    sprintf(result, "-%d", i);
  else
    sprintf(result, "%d", i);
  
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
char * 
Main_strConcat (
  int numArgs,
  ...
  )
{
  va_list args;
  int totalLen = 0, i, offset;
  char * s, * resultString;
  
  va_start(args, numArgs);
  
  for (i=0; i<numArgs; i++) {
    s = va_arg(args, char *);    
    totalLen += strlen(s);
  }

  va_end(args);


  resultString = ALLOC(char, totalLen + 1);
  va_start(args, numArgs);

  offset = 0;
  for (i=0; i<numArgs; i++) {
    int len;

    s = va_arg(args, char *);
    len = strlen (s);
    memcpy (resultString + offset, s, len);
    offset += len;
  }
  
  va_end(args);
  
  *(resultString + offset) = '\0';

  return resultString;
  

}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



