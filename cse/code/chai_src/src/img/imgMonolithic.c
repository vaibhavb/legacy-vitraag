/**CFile***********************************************************************

  FileName    [imgMonolithic.c]

  PackageName [img]

  Synopsis    [Routines for image computation using a monolithic transition
  relation.] 

  Author      [Rajeev K. Ranjan, Tom Shiple, Abelardo Pardo]

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
  using a monolithic transition relation.]

  Description [This function computes the monolithic transition relation
  characterizing the behavior of the system from the individual functions.]

  SideEffects []

  SeeAlso     [Img_MultiwayLinearAndSmooth]
******************************************************************************/

void *
ImgImageInfoInitializeMono(
void *methodData,
mdd_t *transRelation,
ImgFunctionData_t *functionData)
{
  mdd_t *newTransRelation;
  
  if (!methodData){
    if(array_n(functionData->quantifyVars) > 0) {
      newTransRelation =
	mdd_smooth(functionData->mddManager, (mdd_t *) transRelation,
		   functionData->quantifyVars);
      mdd_free(transRelation);
      return (void *) newTransRelation;
    }
    else
      return (void *) transRelation;
  }
  else
    return methodData;
}


/**Function********************************************************************

  Synopsis    [Computes the forward image of a set of states between
  fromLowerBound and fromUpperBound.]

  Description [Computes the forward image of a set of states between
  fromLowerBound and fromUpperBound.  First a set of states between
  fromLowerBound and fromUpperBound is computed.  Then, the transition
  relation is simplified by cofactoring it wrt to the set of states found in
  the first step, and wrt the toCareSet.]

  SideEffects []

  SeeAlso     [ImgImageInfoComputeBwdMono, ImgImageInfoComputeFwdWithDomainVarsMono]
******************************************************************************/
mdd_t *
ImgImageInfoComputeFwdMono(
  ImgFunctionData_t * functionData,
  void * methodData,
  mdd_t *fromLowerBound,
  mdd_t *fromUpperBound,
  mdd_t * toCareSet)
{
  return  ImageComputeMonolithic(methodData, fromLowerBound,
                                 fromUpperBound, toCareSet, 
                                 functionData->domainVars);  
}

  
/**Function********************************************************************

  Synopsis    [Computes the forward image on domain variables of a set
  of states between fromLowerBound and fromUpperBound.]

  Description [Identical to ImgImageInfoComputeFwdMono except
  1. toCareSet is in terms of domain vars and hence range vars are
  substituted first. 2. Before returning the image, range vars are
  substituted with domain vars.]

  SideEffects []

  SeeAlso     [ImgImageInfoComputeFwdMono]
******************************************************************************/
mdd_t *
ImgImageInfoComputeFwdWithDomainVarsMono(
  ImgFunctionData_t * functionData,
  void * methodData,
  mdd_t *fromLowerBound,
  mdd_t *fromUpperBound,
  mdd_t * toCareSet)
{
  mdd_manager *mddManager = functionData->mddManager; 
  mdd_t *toCareSetRV = mdd_substitute(mddManager, toCareSet,
                                      functionData->domainVars,
                                      functionData->rangeVars);
  mdd_t *imageRV = ImageComputeMonolithic(methodData, fromLowerBound,
                                          fromUpperBound, toCareSetRV,  
                                          functionData->domainVars);  
  mdd_t *imageDV = mdd_substitute(mddManager, imageRV,
                                  functionData->rangeVars,
                                  functionData->domainVars);

  mdd_free(toCareSetRV);
  mdd_free(imageRV);
  return imageDV;
}

/**Function********************************************************************

  Synopsis    [Computes the backward image of a set of states between
  fromLowerBound and fromUpperBound.]

  Description [Computes the backward image of a set of states between
  fromLowerBound and fromUpperBound.  First a set of states between
  fromLowerBound and fromUpperBound is computed.  Then, the transition
  relation is simplified by cofactoring it wrt to the set of states found in
  the first step, and wrt the toCareSet.]

  SideEffects []
  
  SeeAlso     [ImgImageInfoComputeFwdMono,ImgImageInfoComputeBwdWithDomainVarsMono]

******************************************************************************/
mdd_t *
ImgImageInfoComputeBwdMono(
  ImgFunctionData_t * functionData,
  void *methodData,
  mdd_t *fromLowerBound,
  mdd_t *fromUpperBound,
  mdd_t *toCareSet)
{
  return ImageComputeMonolithic((mdd_t*)methodData, fromLowerBound,
                                fromUpperBound, toCareSet,  
                                functionData->rangeVars);  
}

/**Function********************************************************************

  Synopsis    [Computes the backward image of a set of states between
  fromLowerBound and fromUpperBound.]

  Description [Identical to ImgImageInfoComputeBwdMono except that
  fromLowerBound and fromUpperBound and given in terms of domain
  variables, hence we need to substitute the range variables first.]
  
  SideEffects []

  SeeAlso [ImgImageInfoComputeBwdMono]

******************************************************************************/
mdd_t *
ImgImageInfoComputeBwdWithDomainVarsMono(
  ImgFunctionData_t * functionData,
  void *methodData,
  mdd_t *fromLowerBound,
  mdd_t *fromUpperBound,
  mdd_t *toCareSet)
{
  mdd_manager *mddManager = functionData->mddManager;
  mdd_t *fromLowerBoundRV = mdd_substitute(mddManager, fromLowerBound,
                                           functionData->domainVars,
                                           functionData->rangeVars);
  
  mdd_t *fromUpperBoundRV = mdd_substitute(mddManager, fromUpperBound,
                                           functionData->domainVars,
                                           functionData->rangeVars);
  mdd_t *image =  ImageComputeMonolithic((mdd_t*)methodData,
                                         fromLowerBoundRV,
                                         fromUpperBoundRV, toCareSet,
                                         functionData->rangeVars);
  
  mdd_free(fromLowerBoundRV);
  mdd_free(fromUpperBoundRV);

  return image;
}



/**Function********************************************************************

  Synopsis    [Frees the method data associated with the monolithic method.]

  SideEffects []

******************************************************************************/
void
ImgImageInfoFreeMono(void * methodData)
{
  mdd_free((mdd_t *)methodData);
}

/**Function********************************************************************

  Synopsis    [Prints information about the IWLS95 method.]

  Description [This function is used to obtain the information about
  the parameters used to initialize the imageInfo.]

  SideEffects []

******************************************************************************/
void
ImgImageInfoPrintMethodParamsMono(void *methodData, FILE *fp)
{
  mdd_t *monolithicRelation = (mdd_t *)methodData;
  (void) fprintf(fp,"Printing information about image method: Monolithic\n");  
  (void) fprintf(fp,"\tSize of monolithic relation = %10ld\n",
                 (long) bdd_size(monolithicRelation));
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis    [A generic routine for computing the image.]

  Description [This is a routine for image computation which is called
  by both forward image and backward image computation. The frontier
  set (the set of states for which the image is computed) is obtained
  by finding a small size BDD between lower and upper bounds.
  The transition relation is simplified by cofactoring
  it with the domainSubset (frontier set) and with the toCareSet. The
  order in which this simplification is unimportant from functionality
  point of view, but the order might effect the BDD size of the
  optimizedRelation.] 

  SideEffects []

******************************************************************************/

mdd_t *
ImageComputeMonolithic(mdd_t *methodData, mdd_t *fromLowerBound, mdd_t
                       *fromUpperBound, mdd_t *toCareSet, array_t *smoothVars)
{
  mdd_t       *domainSubset, *image;
  mdd_t       *optimizedRelation, *subOptimizedRelation;
  mdd_t       *monolithicT = (mdd_t *)methodData;
  mdd_manager *mddManager  = mdd_get_manager(fromUpperBound);
  
  assert(monolithicT != NIL(mdd_t));
  
  /*
   * Optimization steps:
   * Choose the domainSubset optimally.
   * Reduce the transition relation wrt to care set and domainSubset.
   */
  domainSubset = bdd_between(fromLowerBound,fromUpperBound);
  subOptimizedRelation = bdd_cofactor(monolithicT, toCareSet);
  optimizedRelation = bdd_cofactor(subOptimizedRelation, domainSubset);  
  mdd_free(domainSubset);
  mdd_free(subOptimizedRelation);
  /*optimizedRelation = bdd_and(fromLowerBound, monolithicT, 1, 1);*/
  if (array_n(smoothVars) == 0){
    image = mdd_dup(optimizedRelation);
  }
  else {
    image = mdd_smooth(mddManager, optimizedRelation, smoothVars);
  }

  mdd_free(optimizedRelation);
  return image;
}

