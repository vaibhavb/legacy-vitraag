/**CHeaderFile*****************************************************************

  FileName    [imgInt.h]

  PackageName [img]

  Synopsis    [Internal declarations for img package.]

  Description [To add a new method, called "foo", follow these steps. 1)
  describe the foo method in the Description field at the top of img.h. 2) Add
  Img_Foo_c to the Img_MethodType enumerated type. 3) Create a file, imgFoo.c,
  and define the following functions: ImgImageInfoInitializeFoo,
  ImgImageInfoComputeFwdFoo, ImgImageInfoComputeBwdFoo,
  ImgImageInfoFreeFoo. 4) In the function Img_ImageInfoInitialize, add the
  case "foo" for the "image_method" flag, and add the case Img_Foo_c to the
  switch for initialization.]
  
  SeeAlso     [fsm]

  Author      [Rajeev Ranjan, Tom Shiple, Abelardo Pardo]

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

  Revision    [$Id: imgInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _IMGINT
#define _IMGINT

#include  "img.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct ImgFunctionDataStruct ImgFunctionData_t;
typedef mdd_t * (*ImgImageComputeProc) ARGS((ImgFunctionData_t *, void *,
                                         mdd_t *, mdd_t *, mdd_t *));
typedef void    (*ImgImageFreeProc)    ARGS((void *));
typedef void    (*ImgImagePrintMethodParamsProc) ARGS((void *, FILE*));

typedef struct DomainCacheValueStruct ImgDomainCacheValue_t;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                     */
/*---------------------------------------------------------------------------*/
/**Struct**********************************************************************

  Synopsis    [Functions and variables to compute images.]

  Description [This data structure provides all the information about the
  functions that are to be used to do image computations.  For a thorough
  explanation of the fields of this structure, see the description of the
  function Img_ImageInfoInitialize.  None of the data structures stored in
  this structure belong to the image package, therefore only a pointer to them
  is kept.]

  SeeAlso     [Img_ImageInfoInitialize]

******************************************************************************/
struct ImgFunctionDataStruct {
  mdd_manager *mddManager;
  array_t     *domainVars;  /* array of mddIds of domain variables */
  array_t     *rangeVars;   /* array of mddIds of range variables */
  array_t     *quantifyVars;/* array of mddIds of variables to be quantified */
};


/**Struct**********************************************************************

  Synopsis    [Information for method-specific image computation.]

  Description [Information for method-specific image computation.  Along with
  user-specified state subsets, this structure contains all the information
  that an image computation method needs to perform the forward or backward
  image of a function vector.  In particular, the methodData is used to allow
  the image computation to carry information from one image computation to the
  next (for example, when image computations are done from within a fixed
  point loop).]

******************************************************************************/
struct ImgImageInfoStruct {
  Img_MethodType       methodType;         /* method initialized to */    
  Img_DirectionType    directionType;      /* types of computation Fwd/Bwd allowed */
  ImgFunctionData_t    functionData;       /* information about the functions */
  void                *methodData;         /* method-dependent data structure */
  ImgImageComputeProc  imageComputeFwdProc;/* function to compute Fwd Image */
  ImgImageComputeProc  imageComputeBwdProc;/* function to compute Bwd Image */
  ImgImageComputeProc  imageComputeFwdWithDomainVarsProc;/* function to compute Fwd Image on Domain Vars*/
  ImgImageComputeProc  imageComputeBwdWithDomainVarsProc;/* function to compute Bwd Image on Domain Vars*/
  ImgImageFreeProc     imageFreeProc;      /* function to free the
                                              method-dependent data structure */
  ImgImagePrintMethodParamsProc   imagePrintMethodParamsProc;
/* Function to print information about the method */
   
};

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN void * ImgImageInfoInitializeDisjunctive(void *methodData, array_t *disjunctionArray, ImgFunctionData_t *functionData);
EXTERN mdd_t * ImgImageInfoComputeFwdDisjunctive(ImgFunctionData_t *functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN mdd_t * ImgImageInfoComputeBwdDisjunctive(ImgFunctionData_t *functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN mdd_t * ImgImageInfoComputeFwdWithDomainVarsDisjunctive(ImgFunctionData_t *functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN mdd_t * ImgImageInfoComputeBwdWithDomainVarsDisjunctive(ImgFunctionData_t *functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN void ImgImageInfoFreeDisjunctive(void *methodData);
EXTERN void ImgImageInfoPrintMethodParamsDisjunctive(void *methodData, FILE *fp);
EXTERN void * ImgImageInfoInitializeIwls95(void *methodData, array_t *bddRelationArray, ImgFunctionData_t *functionData, Img_DirectionType directionType);
EXTERN mdd_t * ImgImageInfoComputeFwdIwls95(ImgFunctionData_t *functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN mdd_t * ImgImageInfoComputeFwdWithDomainVarsIwls95(ImgFunctionData_t *functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN mdd_t * ImgImageInfoComputeBwdIwls95(ImgFunctionData_t *functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN mdd_t * ImgImageInfoComputeBwdWithDomainVarsIwls95(ImgFunctionData_t *functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN void ImgImageInfoFreeIwls95(void *methodData);
EXTERN void ImgImageInfoPrintMethodParamsIwls95(void *methodData, FILE *fp);
EXTERN st_table * ImgBddGetSupportIdTable(bdd_t *function);
EXTERN void * ImgImageInfoInitializeMono(void *methodData, mdd_t *transRelation, ImgFunctionData_t *functionData);
EXTERN mdd_t * ImgImageInfoComputeFwdMono(ImgFunctionData_t * functionData, void * methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t * toCareSet);
EXTERN mdd_t * ImgImageInfoComputeFwdWithDomainVarsMono(ImgFunctionData_t * functionData, void * methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t * toCareSet);
EXTERN mdd_t * ImgImageInfoComputeBwdMono(ImgFunctionData_t * functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN mdd_t * ImgImageInfoComputeBwdWithDomainVarsMono(ImgFunctionData_t * functionData, void *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet);
EXTERN void ImgImageInfoFreeMono(void * methodData);
EXTERN void ImgImageInfoPrintMethodParamsMono(void *methodData, FILE *fp);
EXTERN mdd_t * ImageComputeMonolithic(mdd_t *methodData, mdd_t *fromLowerBound, mdd_t *fromUpperBound, mdd_t *toCareSet, array_t *smoothVars);
EXTERN void ImageInfoPrint(Img_ImageInfo_t *imageInfo);
EXTERN int ImgArrayBddArrayCheckValidity(array_t *arrayBddArray);
EXTERN int ImgBddArrayCheckValidity(array_t *bddArray);
EXTERN int ImgBddCheckValidity(bdd_t *bdd);

/**AutomaticEnd***************************************************************/

#endif /* _INT */
