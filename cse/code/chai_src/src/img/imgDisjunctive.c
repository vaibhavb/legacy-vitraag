/**CFile***********************************************************************

  FileName    [imgDisjunctive.c]

  PackageName [img]

  Synopsis    [Routines for image computation using a disjunctive transition
  relation.] 

  Author      [Shaz Qadeer, Sriram Rajamani]

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

#include  "imgInt.h"


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

  Synopsis    [Initializes an image structure for image computation
  using a disjunctively partitioned transition relation.]

  Description []

  SideEffects []

  SeeAlso     []
******************************************************************************/

void *
ImgImageInfoInitializeDisjunctive(
void *methodData,
array_t *disjunctionArray,
ImgFunctionData_t *functionData)
{
  if(!methodData) {
    if(array_n(functionData->quantifyVars) > 0) {
      int i;
      mdd_t *disjunct;
      
      for(i = 0; i < array_n(disjunctionArray); i++) {
	disjunct = array_fetch(mdd_t *, disjunctionArray, i);
	array_insert(mdd_t *, disjunctionArray, i,
		     mdd_smooth(functionData->mddManager, disjunct, functionData->quantifyVars));
	mdd_free(disjunct);
      }
    }
    
    return (void *) disjunctionArray;
  }
  else
    return methodData;
}


/**Function********************************************************************

  Synopsis    [Computes the forward image of a set of states between
  fromLowerBound and fromUpperBound.]

  Description [Computes the forward image of a set of states between
  fromLowerBound and fromUpperBound.]

  SideEffects []

  SeeAlso     []
******************************************************************************/
mdd_t *
ImgImageInfoComputeFwdDisjunctive(
  ImgFunctionData_t *functionData,
  void *methodData,
  mdd_t *fromLowerBound,
  mdd_t *fromUpperBound,
  mdd_t *toCareSet)
{
  int i;
  mdd_t *disjunct;
  mdd_t *image2, *partialImage;
  mdd_t *image1 = mdd_zero(functionData->mddManager);

  for(i = 0; i < array_n((array_t *) methodData); i++) {
    disjunct = array_fetch(mdd_t *, (array_t *) methodData, i);
    partialImage = ImageComputeMonolithic(disjunct, fromLowerBound,
					  fromUpperBound, toCareSet, 
					  functionData->domainVars);
    image2 = mdd_or(image1, partialImage, 1, 1);

    mdd_free(image1);
    mdd_free(partialImage);
    image1 = image2;
  }

  return image1;
}



/**Function********************************************************************

  Synopsis    [Computes the backward image of a set of states between
  fromLowerBound and fromUpperBound.]

  Description [Computes the backward image of a set of states between
  fromLowerBound and fromUpperBound.]

  SideEffects []

  SeeAlso     []
******************************************************************************/
mdd_t *
ImgImageInfoComputeBwdDisjunctive(
  ImgFunctionData_t *functionData,
  void *methodData,
  mdd_t *fromLowerBound,
  mdd_t *fromUpperBound,
  mdd_t *toCareSet)
{
  int i;
  mdd_t *disjunct;
  mdd_t *image2, *partialImage;
  mdd_t *image1 = mdd_zero(functionData->mddManager);
    
  for(i = 0; i < array_n((array_t *) methodData); i++) {
    disjunct = array_fetch(mdd_t *, (array_t *) methodData, i);
    partialImage = ImageComputeMonolithic(disjunct, fromLowerBound,
					  fromUpperBound, toCareSet, 
					  functionData->rangeVars);
    image2 = mdd_or(image1, partialImage, 1, 1);

    mdd_free(image1);
    mdd_free(partialImage);
    image1 = image2;
  }

  return image1;
}



/**Function********************************************************************

  Synopsis    [Computes the forward image on domain variables of a set
  of states between fromLowerBound and fromUpperBound.]

  Description [Identical to ImgImageInfoComputeFwdDisjunctive except
  1. toCareSet is in terms of domain vars and hence range vars are
  substituted first. 2. Before returning the image, range vars are
  substituted with domain vars.]

  SideEffects []

  SeeAlso     [ImgImageInfoComputeFwdDisjunctive]
******************************************************************************/
mdd_t *
ImgImageInfoComputeFwdWithDomainVarsDisjunctive(
  ImgFunctionData_t *functionData,
  void *methodData,
  mdd_t *fromLowerBound,
  mdd_t *fromUpperBound,
  mdd_t *toCareSet)
{
  int i;
  mdd_t *disjunct;
  mdd_t *image2, *partialImageRV, *partialImageDV;
  mdd_t *image1 = mdd_zero(functionData->mddManager);
  mdd_t *toCareSetRV = mdd_substitute(functionData->mddManager, toCareSet,
                                      functionData->domainVars,
                                      functionData->rangeVars);

  for(i = 0; i < array_n((array_t *) methodData); i++) {
    disjunct = array_fetch(mdd_t *, (array_t *) methodData, i);
    partialImageRV = ImageComputeMonolithic(disjunct, fromLowerBound,
					    fromUpperBound, toCareSetRV,  
					    functionData->domainVars);
    partialImageDV = mdd_substitute(functionData->mddManager, partialImageRV,
				    functionData->rangeVars,
				    functionData->domainVars);
    image2 = mdd_or(image1, partialImageDV, 1, 1);

    mdd_free(image1);
    mdd_free(partialImageRV);
    mdd_free(partialImageDV);
    image1 = image2;
  }

  mdd_free(toCareSetRV);
  return image1;
}

/**Function********************************************************************

  Synopsis    [Computes the backward image of a set of states between
  fromLowerBound and fromUpperBound.]

  Description [Identical to ImgImageInfoComputeBwdDisjunctive except that
  fromLowerBound and fromUpperBound and given in terms of domain
  variables, hence we need to substitute the range variables first.]
  
  SideEffects []

  SeeAlso [ImgImageInfoComputeDisjunctive]

******************************************************************************/
mdd_t *
ImgImageInfoComputeBwdWithDomainVarsDisjunctive(
  ImgFunctionData_t *functionData,
  void *methodData,
  mdd_t *fromLowerBound,
  mdd_t *fromUpperBound,
  mdd_t *toCareSet)
{
  int i;
  mdd_t *disjunct;
  mdd_t *image2, *partialImage;
  mdd_t *image1 = mdd_zero(functionData->mddManager);
  mdd_t *fromLowerBoundRV = mdd_substitute(functionData->mddManager,
					   fromLowerBound,
					   functionData->domainVars,
					   functionData->rangeVars);
  mdd_t *fromUpperBoundRV = mdd_substitute(functionData->mddManager,
					   fromUpperBound,
					   functionData->domainVars,
					   functionData->rangeVars);

  for(i = 0; i < array_n((array_t *) methodData); i++) {
    disjunct = array_fetch(mdd_t *, (array_t *) methodData, i);
    partialImage = ImageComputeMonolithic(disjunct, fromLowerBoundRV,
					  fromUpperBoundRV, toCareSet,  
					  functionData->rangeVars);
    image2 = mdd_or(image1, partialImage, 1, 1);

    mdd_free(image1);
    mdd_free(partialImage);
    image1 = image2;
  }

  mdd_free(fromLowerBoundRV);
  mdd_free(fromUpperBoundRV);
  return image1;
}



/**Function********************************************************************

  Synopsis    [Frees the method data associated with the disjunctive method.]

  SideEffects []

******************************************************************************/
void
ImgImageInfoFreeDisjunctive(
  void *methodData)
{
  int i;
  mdd_t *disjunct;
  
  for(i = 0; i < array_n((array_t *) methodData); i++) {
    disjunct = array_fetch(mdd_t *, (array_t *) methodData, i);
    mdd_free(disjunct);
  }

  array_free((array_t *) methodData);
}

/**Function********************************************************************

  Synopsis    [Prints information about the disjunctive method.]

  Description [This function is used to obtain the information about
  the parameters used to initialize the imageInfo.]

  SideEffects []

******************************************************************************/
void
ImgImageInfoPrintMethodParamsDisjunctive(
  void *methodData,
  FILE *fp)
{
  int i;
  mdd_t *disjunct;
  
  (void) fprintf(fp, "Printing information about image method: Disjunctive\n");
  (void) fprintf(fp, "No. of disjuncts = %d\n", array_n((array_t *) methodData));
  (void) fprintf(fp, "Sizes of the disjuncts:\n");

  for(i = 0; i < array_n((array_t *) methodData); i++) {
    disjunct = array_fetch(mdd_t *, (array_t *) methodData, i);
    (void) fprintf(fp, "Disjunct %d -- %101d\n", i, (long) bdd_size(disjunct));
  }
}


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


