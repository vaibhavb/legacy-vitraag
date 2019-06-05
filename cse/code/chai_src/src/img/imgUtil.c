/**CFile***********************************************************************

  FileName    [imgUtil.c]

  PackageName [img]

  Synopsis    [High-level routines to perform image computations.]

  Description [This file provides the exported interface to the
  method-specific image computation routines.]

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

******************************************************************************/
#include  "imgInt.h"

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int CheckImageValidity(mdd_manager *mddManager, mdd_t *image, array_t *domainVarMddIdArray, array_t *quantifyVarMddIdArray);
static void PrintVarIdTable(st_table *table);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/**Function********************************************************************

  Synopsis    [Initialize the image package.]

  SideEffects []

  SeeAlso     [Img_Reinit, Img_End]

******************************************************************************/
int
Img_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Reinitializes the img package.]

  SideEffects        []

  SeeAlso            [Img_Init, Img_End]

******************************************************************************/
int
Img_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager) 
{
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis    [End the image package.]

  SideEffects []

  SeeAlso     [Img_Init, Img_Reinit]

******************************************************************************/
int
Img_End()
{
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis    [Print information about the image package]

  SideEffects []

  SeeAlso     [Img_Init, Img_Reinit]

******************************************************************************/
void
ImageInfoPrint(
Img_ImageInfo_t *imageInfo)
{
  int i;

  printf("No. of domain vars = %d\n", array_n(imageInfo->functionData.domainVars));
  fflush(stdout);
  for(i=0; i<array_n(imageInfo->functionData.domainVars); i++) {
    printf("%d ", array_fetch(int, imageInfo->functionData.domainVars, i));
    fflush(stdout);
  }
  printf("\nNo. of range vars = %d\n", array_n(imageInfo->functionData.rangeVars));
  fflush(stdout);
  for(i=0; i<array_n(imageInfo->functionData.rangeVars); i++) {
    printf("%d ", array_fetch(int, imageInfo->functionData.rangeVars, i));
    fflush(stdout);
  }
  printf("\nNo. of quantify vars = %d\n", array_n(imageInfo->functionData.quantifyVars));
  fflush(stdout);
  for(i=0; i<array_n(imageInfo->functionData.quantifyVars); i++) {
    printf("%d ", array_fetch(int, imageInfo->functionData.quantifyVars, i));
    fflush(stdout);
  }
  printf("\n");
}

/**Function********************************************************************

  Synopsis    [Initializes the image info structure for use in Mocha]

  Description [The function initializes the image info data structure for use
               in Mocha. Note that domainVars and rangeVars are arrays of mddIds
	       of present state variables and next state variables
	       respectively.]
	       
  SideEffects [None.]

  See Also    [Img_ImageInfoInitialize]
  
******************************************************************************/
Img_ImageInfo_t *
Img_ImageInfoInitializeMocha(
  mdd_manager *mddManager,
  array_t *domainVars,
  array_t *rangeVars,
  array_t *quantifyVars,
  unsigned int methodType,
  Img_DirectionType  directionType,
  void *methodInfo)
{
  switch (methodType) {
  case 0 :
    return Img_ImageInfoInitialize(NIL(Img_ImageInfo_t), mddManager, domainVars, 
				   rangeVars, quantifyVars, Img_Monolithic_c, 
				   methodInfo, directionType);
  case 1 :
    return Img_ImageInfoInitialize(NIL(Img_ImageInfo_t), mddManager, domainVars, 
				   rangeVars, quantifyVars, Img_Iwls95_c, 
				   methodInfo, directionType);
  case 2 :
    return Img_ImageInfoInitialize(NIL(Img_ImageInfo_t), mddManager, domainVars, 
				   rangeVars, quantifyVars, Img_Disjunctive_c, 
				   methodInfo, directionType);
  }
}

/**Function********************************************************************

  Synopsis    [Initializes an imageInfo structure for the given method and
  direction.] 

  Description [Initializes an imageInfo structure.  MethodType specifies which
  image computation method to use.  If methodType is Img_Default_c, then if
  the user-settable flag "image_method" has been set, then this method is
  used, otherwise some default is used. DirectionType specifies which types of
  image computations will be performed (forward, backward, or both).
  Method-specific initialization takes into account the value of relevant
  parameters in the global flag table.<p>

  MddNetwork is a graph representing the functions to be used.  Each vertex of
  the graph contains a multi-valued function (MVF) and an MDD id.  The MVF
  gives the function of the vertex in terms of the MDD ids of the immediate
  fanins of the vertex.<p>

  Roots is an array of char* specifying the vertices of the graph which
  represent those functions for which we want to compute the image (it must
  not be empty); for example, for an FSM, roots represent the next state
  functions.  DomainVars is an array of mddIds; for an FSM, these are the
  present state variables.  Subsets of the domain are defined over these
  variables. RangeVars is an array of mddIds over which the range is
  expressed; for an FSM, these are the next state variables.  This array must
  be in one-to-one correspondence with the array of roots.  QuantifyVars is an
  array of mddIds, representing variables to be quantified from results of
  backward images; for an FSM, these are the input variables.  This array may
  be empty. No copies are made of any of the input parameters, and thus it is
  the application's responsibility to free this data *after* the returned
  Img_ImageInfo_t is freed.]

  Comment     [To add a new method, see the instructions at the top of imgInt.h.]
  
  SideEffects []

  SeeAlso     [Img_ImageInfoComputeFwd Img_ImageInfoComputeBwd Img_ImageInfoFree]

******************************************************************************/
Img_ImageInfo_t *
Img_ImageInfoInitialize(
  Img_ImageInfo_t *imageInfo,
  mdd_manager *mddManager,
  array_t * domainVars,
  array_t * rangeVars,
  array_t * quantifyVars,
  Img_MethodType methodType,
  void *methodInfo,
  Img_DirectionType  directionType)
{
  char *userSpecifiedMethod;
  

  if (imageInfo == NIL(Img_ImageInfo_t)){ /* If it does not exist,
                                             create a new one and
                                             initialize fields
                                             appropriately */
    imageInfo = ALLOC(Img_ImageInfo_t, 1);
    imageInfo->directionType = directionType;

    /*
     * Initialization of the function structure inside ImageInfo .
     * Since no duplication is needed, this process is not encapsulated
     * inside a static procedure.
     */

    imageInfo->functionData.mddManager = mddManager;
    imageInfo->functionData.domainVars   = domainVars;
    imageInfo->functionData.rangeVars    = rangeVars;
    imageInfo->functionData.quantifyVars = quantifyVars;
    imageInfo->methodData                = NIL(void);
    /*
     * If methodType is default, use user-specified method if set.
     */
    if (methodType == Img_Default_c) {
      
      /* userSpecifiedMethod = Cmd_FlagReadByName("image_method"); */
      userSpecifiedMethod = NIL(char);

      if (userSpecifiedMethod == NIL(char)) {
        methodType = Img_Iwls95_c;
      }
      else {
        if (strcmp(userSpecifiedMethod, "monolithic") == 0) {
          methodType = Img_Monolithic_c;
        }
        else if (strcmp(userSpecifiedMethod, "iwls95") == 0) {
          methodType = Img_Iwls95_c;
        }
        else {
          (void) fprintf(stderr, "Unrecognized image_method %s: using Iwls95 method.\n",
                         userSpecifiedMethod);
          methodType = Img_Iwls95_c;
        }
      }
    }
    imageInfo->methodType = methodType;
    switch (methodType) {
    case Img_Monolithic_c:
      imageInfo->imageComputeFwdProc = ImgImageInfoComputeFwdMono;
      imageInfo->imageComputeBwdProc = ImgImageInfoComputeBwdMono;
      imageInfo->imageComputeFwdWithDomainVarsProc = ImgImageInfoComputeFwdWithDomainVarsMono;
      imageInfo->imageComputeBwdWithDomainVarsProc = ImgImageInfoComputeBwdWithDomainVarsMono;
      imageInfo->imageFreeProc       = ImgImageInfoFreeMono;
      imageInfo->imagePrintMethodParamsProc =
	ImgImageInfoPrintMethodParamsMono;
      break;
      
    case Img_Iwls95_c:
      imageInfo->imageComputeFwdProc = ImgImageInfoComputeFwdIwls95;
      imageInfo->imageComputeBwdProc = ImgImageInfoComputeBwdIwls95;
      imageInfo->imageComputeFwdWithDomainVarsProc = ImgImageInfoComputeFwdWithDomainVarsIwls95;
      imageInfo->imageComputeBwdWithDomainVarsProc = ImgImageInfoComputeBwdWithDomainVarsIwls95;
      imageInfo->imageFreeProc       = ImgImageInfoFreeIwls95;
      imageInfo->imagePrintMethodParamsProc =
	ImgImageInfoPrintMethodParamsIwls95; 
      break;
	  
    case Img_Disjunctive_c :
      imageInfo->imageComputeFwdProc = ImgImageInfoComputeFwdDisjunctive;
      imageInfo->imageComputeBwdProc = ImgImageInfoComputeBwdDisjunctive;
      imageInfo->imageComputeFwdWithDomainVarsProc = ImgImageInfoComputeFwdWithDomainVarsDisjunctive;
      imageInfo->imageComputeBwdWithDomainVarsProc = ImgImageInfoComputeBwdWithDomainVarsDisjunctive;
      imageInfo->imageFreeProc       = ImgImageInfoFreeDisjunctive;
      imageInfo->imagePrintMethodParamsProc =
	ImgImageInfoPrintMethodParamsDisjunctive;
      break;
      
    default:
          fail("Unexpected type when initalizing image method");
    }
  }
  
  /*
   * Perform method-specific initialization of methodData. For the monolithic method,
   * methodInfo is the transition relation. For the IWLS95 method, methodInfo is the 
   * array containing the conjuncts of the transition relation. For the disjunctive
   * method, methodInfo is the array containing the disjuncts of the transition
   * relation.
   */
  switch (imageInfo->methodType) {
  case Img_Monolithic_c:
    imageInfo->methodData =
      ImgImageInfoInitializeMono(imageInfo->methodData, (mdd_t *) methodInfo,
				 &(imageInfo->functionData));
    break;
    
  case Img_Iwls95_c:
    imageInfo->methodData =
      ImgImageInfoInitializeIwls95(imageInfo->methodData, (array_t *) methodInfo,
				   &(imageInfo->functionData), 
				   directionType);
    break;
    
  case Img_Disjunctive_c :
    imageInfo->methodData =
      ImgImageInfoInitializeDisjunctive(imageInfo->methodData, (array_t *) methodInfo,
					&(imageInfo->functionData));
    break;

  default:
    fail("Unexpected type when initalizing image method");
  }
  return imageInfo;
}


/**Function********************************************************************

  Synopsis [Computes the forward image of a set and expresses it in terms of
  domain variables.]

  Description [Computes the forward image of a set and expresses it in terms
  of domain variables. FromLowerBound, fromUpperBound, and toCareSet are
  expressed in terms of domain variables. See Img_ImageInfoComputeFwd for more
  information.]

  SideEffects []
  
  SeeAlso     [Img_ImageInfoComputeFwd]

******************************************************************************/
mdd_t *
Img_ImageInfoComputeFwdWithDomainVars(
  Img_ImageInfo_t * imageInfo,
  mdd_t           * fromLowerBound,
  mdd_t           * fromUpperBound,
  mdd_t           * toCareSet)
{
  mdd_t       *image;

  if (mdd_is_tautology(fromLowerBound, 0)){
    mdd_manager *mddManager = imageInfo->functionData.mddManager;
    return (mdd_zero(mddManager));
  }
  image  = ((*imageInfo->imageComputeFwdWithDomainVarsProc)
            (&(imageInfo->functionData), imageInfo->methodData,
             fromLowerBound, fromUpperBound, toCareSet));
  
#ifndef NDEBUG
  assert(CheckImageValidity(
    imageInfo->functionData.mddManager, 
    image,
    imageInfo->functionData.rangeVars, 
    imageInfo->functionData.quantifyVars));
#endif
  return(image);
}


/**Function********************************************************************

  Synopsis    [Computes the forward image of a set.]

  Description [Computes the forward image of a set, under the function vector
  in imageInfo, using the image computation method specified in imageInfo.  The
  set for which the forward image is computed is some set containing
  fromLowerBound and contained in fromUpperBound. The exact set used is chosen
  to simplify the computation.  ToCareSet specifies those forward image points
  of interest; any points not in this set may or may not belong to the returned
  set.  The MDDs fromLowerBound and fromUpperBound are defined over the domain
  variables.  The MDD toCareSet and the returned MDD are defined over the range
  variables. If fromLowerBound is zero, then zero will be returned. ]

  SideEffects []
  
  SeeAlso     [Img_ImageInfoInitialize Img_ImageInfoComputeBwd
  Img_ImageInfoFree Img_ImageInfoComputeFwdWithDomainVars]

******************************************************************************/
mdd_t *
Img_ImageInfoComputeFwd(Img_ImageInfo_t * imageInfo,
                        mdd_t * fromLowerBound,
                        mdd_t * fromUpperBound,
                        mdd_t * toCareSet)
{
  mdd_t *image;
  if (mdd_is_tautology(fromLowerBound, 0)){
    mdd_manager *mddManager = imageInfo->functionData.mddManager;
    return (mdd_zero(mddManager));
  }
  image =  ((*imageInfo->imageComputeFwdProc) (&(imageInfo->functionData),
                                               imageInfo->methodData,
                                               fromLowerBound,
                                               fromUpperBound,
                                               toCareSet));
#ifndef NDEBUG
  assert(CheckImageValidity(
    imageInfo->functionData.mddManager, 
    image,
    imageInfo->functionData.domainVars, 
    imageInfo->functionData.quantifyVars));
#endif
  return image;
}


/**Function********************************************************************

  Synopsis [Computes the backward image of a set expressed in terms of domain
  variables.]

  Description [Computes the backward image of a set expressed in terms of
  domain variables. FromLowerBound, fromUpperBound, and toCareSet are
  expressed in terms of domain variables. See Img_ImageInfoComputeBwd for more
  information.]

  SideEffects []
  
  SeeAlso     [Img_ImageInfoComputeBwd]

******************************************************************************/
mdd_t *
Img_ImageInfoComputeBwdWithDomainVars(
  Img_ImageInfo_t * imageInfo,
  mdd_t           * fromLowerBound,
  mdd_t           * fromUpperBound,
  mdd_t           * toCareSet)
{
  mdd_t *image;
  if (mdd_is_tautology(fromLowerBound, 0)){
    mdd_manager *mddManager = imageInfo->functionData.mddManager;
    return (mdd_zero(mddManager));
  }
  image =  ((*imageInfo->imageComputeBwdWithDomainVarsProc)
            (&(imageInfo->functionData), imageInfo->methodData,
             fromLowerBound, fromUpperBound, toCareSet));
#ifndef NDEBUG
  assert(CheckImageValidity(
    imageInfo->functionData.mddManager, 
    image,
    imageInfo->functionData.rangeVars,
    imageInfo->functionData.quantifyVars));
#endif
  return image;
}


/**Function********************************************************************

  Synopsis    [Computes the backward image of a set.]

  Description [Computes the backward image of a set, under the function vector
  in imageInfo, using the image computation method specified in imageInfo.  The
  set for which the backward image is computed is some set containing
  fromLowerBound and contained in fromUpperBound.  The exact set used is chosen
  to simplify the computation.  ToCareSet specifies those backward image points
  of interest; any points not in this set may or may not belong to the returned
  set.  The MDDs fromLowerBound and fromUpperBound are defined over the range
  variables.  The MDD toCareSet and the returned MDD are defined over the
  domain variables. If fromLowerBound is zero, then zero will be returned.]

  SideEffects []

  SeeAlso     [Img_ImageInfoInitialize Img_ImageInfoComputeFwd
  Img_ImageInfoFree Img_ImageInfoComputeBwdWithDomainVars]   

******************************************************************************/
mdd_t *
Img_ImageInfoComputeBwd(Img_ImageInfo_t * imageInfo,
                        mdd_t * fromLowerBound,
                        mdd_t * fromUpperBound,
                        mdd_t * toCareSet)
{
  mdd_t *image;
  if (mdd_is_tautology(fromLowerBound, 0)){
    mdd_manager *mddManager =
        imageInfo->functionData.mddManager;
    return (mdd_zero(mddManager));
  }
  image =  ((*imageInfo->imageComputeBwdProc) (&(imageInfo->functionData),
                                             imageInfo->methodData,
                                             fromLowerBound,
                                             fromUpperBound,
                                             toCareSet));
#ifndef NDEBUG
  assert(CheckImageValidity(
    imageInfo->functionData.mddManager, 
    image,
    imageInfo->functionData.rangeVars, 
    imageInfo->functionData.quantifyVars));
#endif
  return image;
}


/**Function********************************************************************

  Synopsis    [Frees the memory associated with imageInfo.]

  SideEffects []

  SeeAlso     [Img_ImageInfoInitialize Img_ImageInfoComputeFwd
  Img_ImageInfoComputeBwd] 

******************************************************************************/
void
Img_ImageInfoFree(Img_ImageInfo_t * imageInfo)
{
  (*imageInfo->imageFreeProc) (imageInfo->methodData);
  FREE(imageInfo);
}


/**Function********************************************************************

  Synopsis    [Returns a string giving the method type of an imageInfo.]

  Description [Returns a string giving the method type of an imageInfo. It is
  the user's responsibility to free this string.]

  SideEffects []

  SeeAlso     [Img_ImageInfoInitialize]

******************************************************************************/
char *
Img_ImageInfoObtainMethodTypeAsString(Img_ImageInfo_t * imageInfo)
{
  char *methodString;
  Img_MethodType methodType = imageInfo->methodType;
  
  switch (methodType) {
  case Img_Monolithic_c:
    methodString = util_strsav("monolithic");
    break;
  case Img_Domain_c:
    methodString = util_strsav("domain");
    break;
  case Img_Iwls95_c:
    methodString = util_strsav("iwls95");
    break;
  case Img_Disjunctive_c:
    methodString = util_strsav("disjunctive");
    break;
  case Img_Default_c:
    methodString = util_strsav("default");
    break;
  default:
    fail("Unexpected type when initalizing image method");
  }
  return methodString;
}
  
  
/**Function********************************************************************

  Synopsis    [Prints information about the image technique currently used.]

  Description [Prints information about the image technique currently used.]

  SideEffects [None.]

******************************************************************************/
void
Img_ImageInfoPrintMethodParams(Img_ImageInfo_t *imageInfo, FILE *fp)
{
  (*imageInfo->imagePrintMethodParamsProc)(imageInfo->methodData, fp);
}


/**Function********************************************************************

  Synopsis    [Returns the mdd manager.]

  SideEffects [None.]

******************************************************************************/
mdd_manager *
Img_ImageInfoReadMddManager(
Img_ImageInfo_t *imageInfo)
{
  return imageInfo->functionData.mddManager;
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [This function checks the validity of an array of array
  of BDD nodes.]

  SideEffects []

  SeeAlso     [ImgBddCheckValidity]
  
******************************************************************************/
int
ImgArrayBddArrayCheckValidity(array_t *arrayBddArray)
{
  int i;
  for(i=0; i<array_n(arrayBddArray); i++) {
    ImgBddArrayCheckValidity(array_fetch(array_t*, arrayBddArray, i));
  }
  return 1;
}


/**Function********************************************************************

  Synopsis    [This function checks the validity of array of BDD nodes.]

  SideEffects []

  SeeAlso     [ImgBddCheckValidity]

******************************************************************************/
int
ImgBddArrayCheckValidity(array_t *bddArray)
{
  int i;
  for(i=0; i<array_n(bddArray); i++) {
    ImgBddCheckValidity(array_fetch(bdd_t*, bddArray, i));
  }
  return 1;
}


/**Function********************************************************************

  Synopsis    [This function checks the validity of a BDD.]

  Description [[This function checks the validity of a BDD. Three checks are done:
  1. The BDD should not be freed. "free" field should be 0.
  2. The node pointer of the BDD should be a valid pointer.
  3. The manager pointer of the BDD should be a valid pointer.
  The assumption for 2 and 3 is that, the value of a valid pointer
  should be > 0xf.]

  SideEffects []

******************************************************************************/
int
ImgBddCheckValidity(bdd_t *bdd)
{
  int i;
  assert(bdd_get_free(bdd) == 0); /* Bdd should not have been freed */
  assert(((unsigned long)bdd_get_node(bdd, &i)) & ~0xf); /* Valid node pointer */
  assert(((unsigned long)bdd_get_manager(bdd)) & ~0xf); /* Valid manager pointer */
  return 1;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Checks the validity of image]

  Description [In a properly formed image, there should not be any
  domain or quantify variables in its support. This function checks
  for that fact.]

  SideEffects []

******************************************************************************/
static int
CheckImageValidity(mdd_manager *mddManager, mdd_t *image, array_t
                   *domainVarMddIdArray, array_t *quantifyVarMddIdArray)
{
  int i;
  array_t *imageSupportArray = mdd_get_support(mddManager, image);
  st_table *imageSupportTable = st_init_table(st_numcmp, st_numhash);
  for (i=0; i<array_n(imageSupportArray); i++){
    int mddId = array_fetch(int, imageSupportArray, i);  
    (void) st_insert(imageSupportTable, (char *) (long) mddId, NIL(char));
  }
  for (i=0; i<array_n(domainVarMddIdArray); i++){
    int domainVarId;
    domainVarId = array_fetch(int, domainVarMddIdArray, i);
    assert(st_is_member(imageSupportTable, (char *)(long)domainVarId) == 0);
  }
  for (i=0; i<array_n(quantifyVarMddIdArray); i++){
    int quantifyVarId;
    quantifyVarId = array_fetch(int, quantifyVarMddIdArray, i);
    assert(st_is_member(imageSupportTable, (char *)(long)quantifyVarId) == 0);
  }
  st_free_table(imageSupportTable);
  array_free(imageSupportArray);
  return 1;
}

/**Function********************************************************************

  Synopsis    [Prints the content of a table containing integers.]

  Description [Prints the content of a table containing integers.]

  SideEffects []

******************************************************************************/
static void
PrintVarIdTable(st_table *table)
{
  st_generator *stgen;
  int varId;
  st_foreach_item(table, stgen, (char **)&varId, NIL(char*)){
    (void) fprintf(stdout, "%d ", varId);
  }
  (void) fprintf(stdout, "\n");
}










