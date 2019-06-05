/**CFile***********************************************************************

  FileName    [slMocha.c]

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
  \"AS IS\" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.]

******************************************************************************/

#include "slInt.h" 


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

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
Mocha_SlObject_t *
Mocha_SlGetObjectByName(
  char * objName)
{
  Sl_GSTEntry_t *entry;

  if (!st_lookup(Sl_GST, objName, (char **) &entry)) {
    return NIL(Mocha_SlObject_t);
  } else {
    return (Mocha_SlObject_t *) entry;
  }

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t * 
Mocha_SlGetBDDArray(
  Mocha_SlObject_t * slObj
  )
{
  Sl_GSTEntry_t * entry = (Sl_GSTEntry_t *) slObj;

  if (GSTEntryIsBdd( entry )) {
    return (entry -> obj.bddArray);
  } else {
    return NIL(array_t);
  }

}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t *
Mocha_SlGetIdArray(
  Mocha_SlObject_t * slObj
  )
{
  Sl_GSTEntry_t * entry = (Sl_GSTEntry_t *) slObj;

  if (GSTEntryIsIdArray( entry )) {
    return (entry -> obj.idArray);
  } else {
    return NIL(array_t);
  }

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t*
Mocha_SlGetIntArray(
  Mocha_SlObject_t * slObj
  )
{
  Sl_GSTEntry_t * entry = (Sl_GSTEntry_t *) slObj;

  if (GSTEntryIsIntArray( entry )) {
    return (entry -> obj.intArray);
  } else {
    return NIL(array_t);
  }
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t *
Mocha_SlGetStringArray (
  Mocha_SlObject_t * slObj
  )
{
  Sl_GSTEntry_t * entry = (Sl_GSTEntry_t *) slObj;
  
  if (GSTEntryIsStringArray( entry )) {
    return (entry -> obj.stringArray);
  } else {
    return NIL(array_t);
  }
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Mocha_StateInMdd(
  mdd_t * mdd,
  array_t * idArray,
  array_t * valueArray) 
{
  return Sym_StateInMdd(SlGetMddManager(), mdd, idArray, valueArray);
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



