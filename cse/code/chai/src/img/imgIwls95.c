/**CFile***********************************************************************

  FileName    [imgIwls95.c]

  PackageName [img]

  Synopsis    [Routines for image computation using component transition
               relation approach described in the proceedings of IWLS'95,
               (henceforth referred to Iwls95 technique).]

  Description [<p>The initialization process performs following steps:
  <UL>
  <LI> Create the relation of the roots at the bit level
  in terms of the quantify and domain variables.
  <LI> Order the bit level relations.
  <LI> Group the relations of bits together, making a cluster
  whenever the BDD size reaches a threshold.
  <LI> For each cluster, quantify out the quantify variables which
  are local to that particular cluster.
  <LI> Order the clusters using the algorithm given in
  "Efficient BDD Algorithms for FSM Synthesis and
  Verification", by R. K. Ranjan et. al. in the proceedings of
  IWLS'95{1}. 
  <LI> The orders of the clusters for forward and backward image are
  calculated and stored. Also stored is the schedule of
  variables for early quantification. 
  </UL>
  For forward and backward image computation the corresponding
  routines are called with appropriate ordering of clusters and
  early quantification schedule. <p>
  Note that, a cubic implementation of the above algorithm is
  straightforward. However, to obtain quadratic complexity requires
  significant amount of book-keeping. This is the reason for the complexity
  of the code in this file.]
            
  SeeAlso     []

  Author      [Rajeev K. Ranjan]

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

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define domainVar_c 0
#define rangeVar_c 1
#define quantifyVar_c 2
#define IMG_IWLS95_DEBUG
/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef struct Iwls95InfoStruct Iwls95Info_t;
typedef struct Iwls95OptionStruct Iwls95Option_t;
typedef struct CtrInfoStruct CtrInfo_t;
typedef struct VarInfoStruct VarInfo_t;
typedef struct CtrItemStruct CtrItem_t;
typedef struct VarItemStruct VarItem_t;


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Struct**********************************************************************

  Synopsis    [This structure stores the information needed for iterative image
               computation by IWLS'95 method.] 

  Description [A step of image computation is computed by taking the product of
               "From" set with the BDD's of fwdClusteredRelationArray in order.
               "fwdArraySmoothVarBddArray[i]" is an array of Bdd variables,
               which represents the set of variables which could be quantified
               out once fwdClusteredRelationArray[i] is multiplied in the
               product. Similar analysis holds good for
               bwdClusteredRelationArray. The "option" struct contains
               option values for computing the above mentioned arrays and also
               controls the verbosity.]  

******************************************************************************/

struct Iwls95InfoStruct {
  array_t *fwdClusteredRelationArray;
  array_t *bwdClusteredRelationArray;
  array_t *fwdArraySmoothVarBddArray;
  array_t *bwdArraySmoothVarBddArray;
  bdd_t *toCareSet;
  array_t *bwdClusteredCofactoredRelationArray;
  Iwls95Option_t *option;
};

/**Struct**********************************************************************

  Synopsis    [This structure contains the required options for filling in
               the data structure.]

  Description ["clusterSize" is the threshold value used to
               create clusters. "W1", "W2" etc are the weights used in the
               heuristic algorithms to order the cluster processing for early
               quantification. VerbosityLevel  controls the amount of
               information printed during image computation.]


******************************************************************************/
struct Iwls95OptionStruct {
  int clusterSize;
  int verbosity;
  int W1;
  int W2; /* Weights attached to various parameters. */
  int W3; /* For details please refer to {1} */
  int W4;
};

/**Struct**********************************************************************

  Synopsis    [This structure contains the information about a particular
               cluster.]

  Description [Corresponding to each cluster one ctrInfoStruct is created, The
               various fields of this structure contain the information for  
               ordering the clusters for image computation
               purposes. "varItemList" is the linked list of "varItemStruct" 
               corresponding to all the support variables of the cluster.]

******************************************************************************/
struct CtrInfoStruct {
  int ctrId; /* Unique id of the cluster */
  int numLocalSmoothVars; /*
                           * Number of smooth variables in the support of the
                           * cluster which can be quantified out if the cluster
                           * is multiplied in the current product.
                           */
  int numSmoothVars; /* Number of smooth variables in the support */
  int numIntroducedVars; /*
                          * Number of range variables introduced if the cluster
                          * is multiplied in the current product.
                          */
  int maxSmoothVarIndex; /* Max. index of a smooth variable in the support */ 
  int rankMaxSmoothVarIndex; /* Rank of maximum index amongst all ctr's */
  lsList varItemList; /* List of varItemStruct of the support variables */
  lsHandle ctrInfoListHandle; /*
                               * Handle to the list of info struct of ctr's
                               * which are yet to be ordered.
                               */
};



/**Struct**********************************************************************

  Synopsis    [This structure contains information about a particular variable
               (domain, range or quantify).]

  Description [Corresponding to each variable a structure is created. Like the
               varItemList field of ctrInfoStruct, ctrItemList contains the
               list of ctrInfoStruct corresponding to various clusters which
               depend on this variable.] 

******************************************************************************/

struct VarInfoStruct {
  int bddId;  /* BDD id of the variable */
  int numCtr; /* Number of components which depend on this variable. */
  int varType; /* Domain, range or quantify variable ?*/
  lsList ctrItemList; /* List of ctrItemStruct corresponding to the clusters
                         that depend on it */ 
};

  
  
/**Struct**********************************************************************
  
  Synopsis    [This structure is the wrapper around a particular variable which
               is in the support of a particular cluster.]

  Description [Every cluster has a linked list of varItemStruct's. Each of
               these structs is a wrapper around varInfo. The "ctrItemStruct"
               corresponding to the cluster itself is present in ctrItemList,
               the linked list of varInfo. "ctrItemHandle" is the handle to
               the ctrItemStruct in "ctrItemList" of varInfo.] 

******************************************************************************/
struct VarItemStruct {
  VarInfo_t *varInfo; /* The pointer to the corresponding variable info
                       * structure */
  lsHandle ctrItemHandle; /*
                           * The list handle to the ctrItemStruct
                           * corresponding to the ctrInfo (the one which 
                           * contains this varItem in the varItemList) in the
                           * ctrItemList field of the varInfo.
                           */ 
};


/**Struct**********************************************************************

  Synopsis    [This structure is the wrapper around a particular ctr which
               depend on a particular variable.]

  Description [Every variable has a linked list of ctrItemStruct's. Each of
               these structs is a wrapper around ctrInfo. The "varItemStruct"
               corresponding to the variable itself is present in varItemList,
               the linked list of ctrInfo. "varItemHandle" is the handle to
               the varItemStruct in "varItemList" of ctrInfo.] 

******************************************************************************/
struct CtrItemStruct {
  CtrInfo_t *ctrInfo;     /* Pointer to the corresponding cluster
                           * info structure. */
  lsHandle varItemHandle; /*
                           * The list handle to the varItemStruct
                           * corresponding to the varInfo (the one which 
                           * contains this ctrItem in the ctrItemList) in the
                           * varItemList field of the ctrInfo.
                           */ 
};
  
  

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

static Iwls95Option_t * Iwls95GetOptions(void);
static array_t * CreateClusters(bdd_manager *bddManager, array_t *relationArray, Iwls95Option_t *option);
static void OrderRelationArray(mdd_manager *mddManager, array_t *relationArray, array_t *domainVarBddArray, array_t *quantifyVarBddArray, array_t *rangeVarBddArray, Iwls95Option_t *option, array_t **orderedRelationArrayPtr, array_t **arraySmoothVarBddArrayPtr);
static array_t * RelationArraySmoothLocalVars(array_t *relationArray, array_t *ctrInfoArray, array_t *varInfoArray, st_table *bddIdToBddTable);
static void OrderRelationArrayAux(array_t *relationArray, lsList remainingCtrInfoList, array_t *ctrInfoArray, array_t *varInfoArray, int *sortedMaxIndexVector, int numSmoothVarsRemaining, int numIntroducedVarsRemaining, st_table *bddIdToBddTable, Iwls95Option_t *option, array_t *domainAndQuantifyVarBddArray, array_t **orderedRelationArrayPtr, array_t **arraySmoothVarBddArrayPtr);
static array_t * UpdateInfoArrays(CtrInfo_t *ctrInfo, st_table *bddIdToBddTable, int *numSmoothVarsRemainingPtr, int *numIntroducedVarsRemainingPtr);
static float CalculateBenefit(CtrInfo_t *ctrInfo, int maxNumLocalSmoothVars, int maxNumSmoothVars, int maxIndex, int maxNumIntroducedVars, Iwls95Option_t *option);
static void PrintOption(Iwls95Option_t *option, FILE *fp);
static Iwls95Info_t * Iwls95InfoStructAlloc();
static CtrInfo_t * CtrInfoStructAlloc(void);
static void CtrInfoStructFree(CtrInfo_t *ctrInfo);
static VarInfo_t * VarInfoStructAlloc(void);
static void VarInfoStructFree(VarInfo_t *varInfo);
static void CtrItemStructFree(CtrItem_t *ctrItem);
static void VarItemStructFree(VarItem_t *varItem);
static int CtrInfoMaxIndexCompare(CtrInfo_t** infoCtr1, CtrInfo_t **infoCtr2);
static void PrintCtrInfoStruct(CtrInfo_t *ctrInfo);
static void PrintVarInfoStruct(VarInfo_t *varInfo);
static int CheckQuantificationSchedule(array_t *relationArray, array_t *arraySmoothVarBddArray);
static int CheckCtrInfoArray(array_t *ctrInfoArray, int numDomainVars, int numQuantifyVars, int numRangeVars);
static int CheckCtrInfo(CtrInfo_t *ctrInfo, int numDomainVars, int numQuantifyVars, int numRangeVars);
static int CheckVarInfoArray(array_t *varInfoArray, int numRelation);
static array_t * MddIdToBddIdArray(mdd_manager *mddManager, int mddId);
static array_t * MddIdToBddArray(mdd_manager *mddManager, int mddId);
static array_t * MddIdArrayToBddArray(mdd_manager *mddManager, array_t *mddIdArray);
static void MddArrayFree(array_t *mddArray);
static array_t * BddArrayDup(array_t *bddArray);
static void BddArrayFree(array_t *bddArray);
static void ArrayBddArrayFree(array_t *arrayBddArray);
static array_t * MddFnArrayToBddRelArray(mdd_manager *mddManager, int mddId, array_t *mddFnArray);
static bdd_t * BddLinearAndSmooth(bdd_manager *mddManager, bdd_t *fromSet, array_t *relationArray, array_t *arraySmoothVarBddArray, int verbosity);
static void HashIdToBddTable(st_table *table, array_t *idArray, array_t *bddArray);
static void PrintSmoothIntroducedCount(array_t *clusterArray, array_t **arraySmoothVarBddArrayPtr, array_t *psBddIdArray, array_t *nsBddIdArray);
static void PrintIntegerArray(array_t *idArray);
static void PrintBddIdFromBddArray(array_t *bddArray);
static void PrintBddIdTable(st_table *idTable);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis    [Returns the result after existentially quantifying a
  set of variables after taking the product of the array of relations.]

  Description ["relationArray" is an array of mdd's which need to be
  multiplied and the variables in the "smoothVarMddIdArray" need to
  be quantified out from the product. "introducedVarMddIdArray" is the
  array of mddIds of the variables (other than the variables to be
  quantified out) in the support of the relations. This array is used
  to compute the product order such that the number of new variables
  introduced in the product is minimized. However passing an empty
  array or an array of mddIds of partial support will not result in any
  error (some optimality will be lost though). The computation consists of 2
  phases. In phase 1, an ordering of the relations and a schedule of
  quantifying variables is found (based on IWLS95) heuristic. In phase
  2, the relations are multiplied in order and the quantifying
  variables are quantified according to the schedule.]

  SideEffects [None]

******************************************************************************/
mdd_t*
Img_MultiwayLinearAndSmooth(mdd_manager *mddManager, array_t *relationArray,
                            array_t *smoothVarMddIdArray, array_t
                            *introducedVarMddIdArray) 
{
  mdd_t *product;
  array_t *domainVarBddArray = array_alloc(bdd_t*, 0);
  array_t *quantifyVarBddArray = MddIdArrayToBddArray(mddManager,
                                                      smoothVarMddIdArray);
  array_t *rangeVarBddArray = MddIdArrayToBddArray(mddManager,
                                                   introducedVarMddIdArray);
  Iwls95Option_t *option = Iwls95GetOptions();
  array_t *orderedRelationArray = NIL(array_t);
  array_t  *arraySmoothVarBddArray = NIL(array_t);
  mdd_t *dummyFromSet = mdd_one(mddManager);
                               
  OrderRelationArray(mddManager, relationArray, domainVarBddArray,
                     quantifyVarBddArray, rangeVarBddArray, option,
                     &orderedRelationArray, &arraySmoothVarBddArray);

  product = BddLinearAndSmooth(mddManager, dummyFromSet, 
                               orderedRelationArray,
                               arraySmoothVarBddArray, option->verbosity); 
  mdd_free(dummyFromSet);
  array_free(domainVarBddArray);
  FREE(option);
  mdd_array_free(rangeVarBddArray);
  mdd_array_free(quantifyVarBddArray);
  mdd_array_free(orderedRelationArray);
  ArrayBddArrayFree(arraySmoothVarBddArray);
  return product;
}



    
/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************
  
  Synopsis    [Initializes an image data structure for image
  computation using the Iwls95 technique.] 

  Description [This process consists of following steps.
  1. The transition functions are built.
  2. An array of bit level relations are built using the function
  MddFnArrayToBddRelArray.
  3. The relations are clustered using the threshold value.
  4. The clustered relations are ordered.]

  SideEffects []

******************************************************************************/
void *
ImgImageInfoInitializeIwls95(void *methodData, 
			     array_t *bddRelationArray,
			     ImgFunctionData_t *functionData,
                             Img_DirectionType  directionType)  
{
  Iwls95Info_t *info = (Iwls95Info_t *) methodData;
  if (info &&
      (((info->fwdClusteredRelationArray) &&
       (info->bwdClusteredRelationArray)) || 
      ((directionType == Img_Forward_c) &&
       (info->fwdClusteredRelationArray)) ||
      ((directionType == Img_Backward_c) &&
       (info->bwdClusteredRelationArray)))){
    /* Nothing needs to be done. Return */
    return (void *) info;
  }
  else{
    array_t *rangeVarBddArray, *domainVarBddArray, *quantifyVarBddArray;
    array_t *orderedRelationArray;
    array_t *clusteredRelationArray;
    int i;
    array_t     *domainVarMddIdArray  = functionData->domainVars;
    array_t     *rangeVarMddIdArray  = functionData->rangeVars;
    array_t     *quantifyVarMddIdArray = array_dup(functionData->quantifyVars);
    mdd_manager *mddManager = functionData->mddManager;


    if (info == NIL(Iwls95Info_t)){
      info = Iwls95InfoStructAlloc();
    }

    /* There was some code here whose purpose was to generate the bddRelationArray
       from the partition data structure. I am removing this code because the
       bddRelationArray is being created in PVS using foreign function calls to the
       mdd package. I am also going to remove all references to the partition data
       structure in this file.
    */

    /* Get the Bdds from Mdd Ids */
    rangeVarBddArray  = MddIdArrayToBddArray(mddManager, rangeVarMddIdArray);
    domainVarBddArray = MddIdArrayToBddArray(mddManager, domainVarMddIdArray);
    quantifyVarBddArray = MddIdArrayToBddArray(mddManager,
                                               quantifyVarMddIdArray);
    array_free(quantifyVarMddIdArray);
    
    
    if (((directionType == Img_Forward_c) ||
         (directionType == Img_Both_c)) &&
        (info->fwdClusteredRelationArray == NIL(array_t))){
      /*
       * Since clusters are formed by multiplying the latches starting
       * from one end we need to order the latches using some heuristics
       * first. Right now, we order the latches using the same heuristic
       * as the one used for ordering the clusters for early quantifiction.
       * However, since we are not interested in the quantification
       * schedule at this stage, we will pass a NIL(array_t*) as the last
       * argument. 
       */
      
      OrderRelationArray(mddManager, bddRelationArray, domainVarBddArray,
                         quantifyVarBddArray, rangeVarBddArray,
                         info->option, &orderedRelationArray, NIL(array_t *));
      
      /* Create the clusters */
      
      clusteredRelationArray = CreateClusters((bdd_manager *)mddManager,
                                              orderedRelationArray,
                                              info->option);
      
      /* Free the orderedRelationArray */
      
      BddArrayFree(orderedRelationArray);
      
      /* Order the clusters for image and pre-image computation. */
      
      OrderRelationArray((bdd_manager *)mddManager, clusteredRelationArray,
                         domainVarBddArray, quantifyVarBddArray,
                         rangeVarBddArray, info->option,
                         &info->fwdClusteredRelationArray, 
                         &info->fwdArraySmoothVarBddArray);
      
      /* Free the clusteredRelationArray. */
      BddArrayFree(clusteredRelationArray);

#ifndef NDEBUG
      assert(CheckQuantificationSchedule(info->fwdClusteredRelationArray,
                                         info->fwdArraySmoothVarBddArray));
#endif    
    }
    
    if (((directionType == Img_Backward_c) ||
         (directionType == Img_Both_c)) &&
        (info->bwdClusteredRelationArray == NIL(array_t))){
      
      /*
       * Since clusters are formed by multiplying the latches starting
       * from one end we need to order the latches using some heuristics
       * first. Right now, we order the latches using the same heuristic
       * as the one used for ordering the clusters for early quantifiction.
       * However, since we are not interested in the quantification
       * schedule at this stage, we will pass a NIL(array_t*) as the last
       * argument. 
       */
      
      OrderRelationArray(mddManager, bddRelationArray, rangeVarBddArray,
                         quantifyVarBddArray, domainVarBddArray,
                         info->option, &orderedRelationArray, NIL(array_t *));
      
      /* Create the clusters */
      
      clusteredRelationArray = CreateClusters((bdd_manager *)mddManager,
                                              orderedRelationArray,
                                              info->option);
      
      /* Free the orderedRelationArray */
      
      BddArrayFree(orderedRelationArray);
      
      /* Order the clusters for image and pre-image computation. */
      OrderRelationArray((bdd_manager *)mddManager, clusteredRelationArray,
                         rangeVarBddArray, quantifyVarBddArray,
                         domainVarBddArray, info->option, 
                         &info->bwdClusteredRelationArray,
                         &info->bwdArraySmoothVarBddArray); 
      
      /* Free the clusteredRelationArray. */
      BddArrayFree(clusteredRelationArray);

      /* Initialize the cofactored relation array */
      info->toCareSet = bdd_one(mddManager);
      info->bwdClusteredCofactoredRelationArray = array_alloc(bdd_t*,
                                                              0);
      
      for (i=0; i<array_n(info->bwdClusteredRelationArray); i++){
        bdd_t *relation = array_fetch(bdd_t*,
                                      info->bwdClusteredRelationArray,
                                      i);
        array_insert_last(bdd_t*,
                          info->bwdClusteredCofactoredRelationArray,
                          bdd_dup(relation));
      }

#ifndef NDEBUG
    assert(CheckQuantificationSchedule(info->bwdClusteredRelationArray,
                                       info->bwdArraySmoothVarBddArray));
#endif
    }
    
    
    /*
      Free the bddRelationArray
      I am commenting this out sicne the conjuncts will be needed later - eg. ref
    BddArrayFree(bddRelationArray);
    */
    
    if (info->option->verbosity > 0){
      fprintf(stdout,"Computing Image Using IWLS95 technique.\n");
      fprintf(stdout,"Total # of domain binary variables = %3d\n",
              array_n(domainVarBddArray));
      fprintf(stdout,"Total # of range binary variables = %3d\n",
              array_n(rangeVarBddArray));
      fprintf(stdout,"Total # of quantify binary variables = %3d\n",
              array_n(quantifyVarBddArray));
      if ((directionType == Img_Forward_c) || (directionType ==
                                               Img_Both_c)){
        fprintf(stdout, "Shared size of transition relation for forward image computation is %10ld BDD nodes (%-4d components)\n", 
                bdd_size_multiple(info->fwdClusteredRelationArray),
                array_n(info->fwdClusteredRelationArray));
      }
      if ((directionType == Img_Backward_c) || (directionType ==
                                                Img_Both_c)){ 
        fprintf(stdout, "Shared size of transition relation for backward image computation is %10ld BDD nodes (%-4d components)\n", 
                bdd_size_multiple(info->bwdClusteredRelationArray),
                array_n(info->bwdClusteredRelationArray));
      }
    }
    
    /* Free the Bdd arrays */
    BddArrayFree(domainVarBddArray);
    BddArrayFree(quantifyVarBddArray);
    BddArrayFree(rangeVarBddArray);
    return (void *)info;
  }
}





/**Function********************************************************************

  Synopsis    [Computes the forward image of a set of states.]

  Description [First bdd_cofactor is used to compute the simplified set of
  states to compute the image of. Next, this simplified set is multiplied with
  the relation arrays given in the fwdClusteredRelationArray in order and the
  variables are quantified according to the schedule in
  fwdArraySmoothVarBddArray.]

  SideEffects []

  SeeAlso     [ImgImageInfoComputeFwdWithDomainVarsIwls95]

******************************************************************************/
mdd_t *
ImgImageInfoComputeFwdIwls95(ImgFunctionData_t *functionData,
                             void *methodData,
                             mdd_t *fromLowerBound,
                             mdd_t *fromUpperBound,
                             mdd_t *toCareSet)
{
  mdd_t *image, *domainSubset;
  mdd_manager *mddManager;

  Iwls95Info_t *info = (Iwls95Info_t *)methodData;
  if (info->fwdClusteredRelationArray == NIL(array_t)){
    fprintf(stderr, "The data structure has not been initialized\
for forward image computation\n");
    return NIL(mdd_t);
  }
  
  mddManager = functionData->mddManager;
   
  domainSubset = bdd_between(fromLowerBound, fromUpperBound);
  /*
   * We do not want to worry about using toCareSet in forward image computation
   */
  image = (mdd_t *) BddLinearAndSmooth((bdd_manager *)mddManager,
                                       (bdd_t *) domainSubset, 
                                       info->fwdClusteredRelationArray,
                                       info->fwdArraySmoothVarBddArray,
                                       info->option->verbosity);  
  mdd_free(domainSubset);
  return image;
}


/**Function********************************************************************

  Synopsis    [Computes the forward image of a set of states in terms
  of domain variables.]

  Description [First the forward image computation function is called
  which returns an image on range vars. Later, variable substitution
  is used to obtain image on domain vars.]

  SideEffects []

  SeeAlso     [ImgImageInfoComputeFwdIwls95]

******************************************************************************/
mdd_t *
ImgImageInfoComputeFwdWithDomainVarsIwls95(ImgFunctionData_t *functionData,
                                           void *methodData,
                                           mdd_t *fromLowerBound,
                                           mdd_t *fromUpperBound,
                                           mdd_t *toCareSet)
{
  mdd_manager *mddManager = functionData->mddManager;

  /*
   * Here I am passing NIL(mdd_t) as toCareSet to the
   * ImgImageInfoComputeFwdIwls95. However, if
   * ImgImageInfoComputeFwdIwls95 is changed to exploit the toCareSet,
   * we would need to pass in toCareSet after substituting the domain
   * variables with range variables.
   */

  mdd_t *imageRV = ImgImageInfoComputeFwdIwls95(functionData,
                                                methodData,
                                                fromLowerBound,
                                                fromUpperBound,
                                                NIL(mdd_t));
  

  mdd_t *imageDV = mdd_substitute(mddManager, imageRV, functionData->rangeVars,
                                  functionData->domainVars);
  mdd_free(imageRV);
  return imageDV;
}

/**Function********************************************************************

  Synopsis    [Computes the backward image of domainSubset.]

  Description [First bdd_cofactor is used to compute the simplified set of
  states to compute the image of. Next, this simplified set is multiplied with
  the relation arrays given in the bwdClusteredRelationArray in order and the
  variables are quantified according to the schedule in
  bwdArraySmoothVarBddArray.]

  SideEffects []

  SeeAlso     [ImgImageInfoComputeBwdWithDomainVarsIwls95]

******************************************************************************/
mdd_t *
ImgImageInfoComputeBwdIwls95(ImgFunctionData_t *functionData,
                             void *methodData,
                             mdd_t *fromLowerBound,
                             mdd_t *fromUpperBound,
                             mdd_t *toCareSet)
{
  mdd_t *image, *domainSubset, *simplifiedImage;
  mdd_manager *mddManager;
  Iwls95Info_t *info = (Iwls95Info_t *)methodData;
  int i;

  if (info->bwdClusteredRelationArray == NIL(array_t)){
    fprintf(stderr, "The data structure has not been initialized for\
backward image computation\n");
    return NIL(mdd_t);
  }
  mddManager = functionData->mddManager;
  domainSubset = bdd_between(fromLowerBound, fromUpperBound);

  /* Check if we can use the stored simplified relations */

  if (bdd_equal(info->toCareSet, toCareSet) == 0){ 
  /* We cannot. Hence create a new set of relations */
    bdd_free(info->toCareSet);
    info->toCareSet = bdd_dup(toCareSet);
    for (i=0; i<array_n(info->bwdClusteredRelationArray); i++){
      bdd_t *relation;
      relation = array_fetch(bdd_t*,
                             info->bwdClusteredCofactoredRelationArray, i); 
      bdd_free(relation);
      relation = array_fetch(bdd_t*, info->bwdClusteredRelationArray, i);
      array_insert(bdd_t*,
                   info->bwdClusteredCofactoredRelationArray, i,
                   bdd_minimize(relation, toCareSet));
    }
  }

  image = (mdd_t *)
      BddLinearAndSmooth((bdd_manager *)mddManager, domainSubset,
                         info->bwdClusteredCofactoredRelationArray, 
                         info->bwdArraySmoothVarBddArray,
                         info->option->verbosity);
  mdd_free(domainSubset);
  simplifiedImage = bdd_minimize(image, toCareSet);
  bdd_free(image);
  return simplifiedImage;
}

/**Function********************************************************************

  Synopsis    [Computes the backward image of a set of states.]

  Description [Identical to ImgImageInfoComputeBwdIwls95 except in the
  fromLowerBound and fromUpperBound, domainVars are substituted by
  rangeVars.]
  
  SideEffects []

  SeeAlso     [ImgImageInfoComputeBwdIwls95]

******************************************************************************/
mdd_t *
ImgImageInfoComputeBwdWithDomainVarsIwls95(ImgFunctionData_t *functionData,
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
  
  mdd_t *image =  ImgImageInfoComputeBwdIwls95(functionData,
                                               methodData,
                                               fromLowerBoundRV,
                                               fromUpperBoundRV,
                                               toCareSet);
  mdd_free(fromLowerBoundRV);
  mdd_free(fromUpperBoundRV);
  return image;
}

/**Function********************************************************************

  Synopsis    [Frees the memory associated with imageInfo.]

  Description [Frees the memory associated with imageInfo.]

  SideEffects []

******************************************************************************/
void
ImgImageInfoFreeIwls95(void *methodData)
{
  Iwls95Info_t *info = (Iwls95Info_t *)methodData;
  
  if (info == NIL(Iwls95Info_t)){
    fprintf(stderr, "Trying to free the NULL image info\n");
    return;
  }
  if (info->fwdClusteredRelationArray != NIL(array_t)){
    BddArrayFree(info->fwdClusteredRelationArray);
    ArrayBddArrayFree(info->fwdArraySmoothVarBddArray);
  }
  if (info->bwdClusteredRelationArray != NIL(array_t)){
    BddArrayFree(info->bwdClusteredRelationArray);
    ArrayBddArrayFree(info->bwdArraySmoothVarBddArray);
    bdd_free(info->toCareSet);
  }
  if (info->bwdClusteredCofactoredRelationArray != NIL(array_t)){
    BddArrayFree(info->bwdClusteredCofactoredRelationArray);
  }
  FREE(info->option);
  FREE(info);
}

/**Function********************************************************************

  Synopsis    [Prints information about the IWLS95 method.]

  Description [This function is used to obtain the information about
  the parameters used to initialize the imageInfo.]

  SideEffects []

******************************************************************************/
void
ImgImageInfoPrintMethodParamsIwls95(void *methodData, FILE *fp)
{
  Iwls95Info_t *info = (Iwls95Info_t *)methodData;
  PrintOption(info->option, fp);
  if (info->fwdClusteredRelationArray != NIL(array_t)){
    (void) fprintf(stdout, "Shared size of transition relation for forward image computation is %10ld BDD nodes (%-4d components)\n", 
                   bdd_size_multiple(info->fwdClusteredRelationArray),
                   array_n(info->fwdClusteredRelationArray));
  }
  if (info->bwdClusteredRelationArray != NIL(array_t)){
    (void) fprintf(stdout, "Shared size of transition relation for backward image computation is %10ld BDD nodes (%-4d components)\n", 
                   bdd_size_multiple(info->bwdClusteredRelationArray),
                   array_n(info->bwdClusteredRelationArray));
  }
}

/**Function********************************************************************

  Synopsis    [Returns the st_table containing the bdd id of the support
  variables of the function.]

  Description [Returns the st_table containing the bdd id of the support
  variables of the function.]

  SideEffects []

******************************************************************************/

st_table *
ImgBddGetSupportIdTable(bdd_t *function)
{
  st_table *supportTable;
  var_set_t *supportVarSet;
  int i;
  
  supportTable = st_init_table(st_numcmp, st_numhash);
  supportVarSet = bdd_get_support(function);
  for(i=0; i<supportVarSet->n_elts; i++){
    if (var_set_get_elt(supportVarSet, i) == 1){
      st_insert(supportTable, (char *)(long) i, (char *)NULL);
    }
  }
  var_set_free(supportVarSet);
  return supportTable;
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Gets the necessary options for computing the image and returns
               in the option structure.]

  Description [Gets the necessary options for computing the image and returns
               in the option structure.]

  SideEffects []

******************************************************************************/

static Iwls95Option_t *
Iwls95GetOptions(void)
{
  char *flagValue;
  Iwls95Option_t *option;
  
  option = ALLOC(Iwls95Option_t, 1);

  /* flagValue = Cmd_FlagReadByName("image_cluster_size"); */
  flagValue = NIL(char);

  if (flagValue == NIL(char)){
    option->clusterSize = 1000; /* the default value */
  }
  else {
    option->clusterSize = atoi(flagValue);
  }

  /* flagValue = Cmd_FlagReadByName("image_verbosity"); */
  flagValue = NIL(char);

  if (flagValue == NIL(char)){
    option->verbosity = 0; /* the default value */
  }
  else {
    option->verbosity = atoi(flagValue);
  }

  /*flagValue = Cmd_FlagReadByName("image_W1"); */
  flagValue = NIL(char);

  if (flagValue == NIL(char)){
    option->W1 = 6; /* the default value */
  }
  else {
    option->W1 = atoi(flagValue);
  }

  /*  flagValue = Cmd_FlagReadByName("image_W2"); */
  flagValue = NIL(char);

  if (flagValue == NIL(char)){
    option->W2 = 1; /* the default value */
  }
  else {
    option->W2 = atoi(flagValue);
  }

  /* flagValue = Cmd_FlagReadByName("image_W3"); */
  flagValue = NIL(char);

  if (flagValue == NIL(char)){
    option->W3 = 1; /* the default value */
  }
  else {
    option->W3 = atoi(flagValue);
  }

  /* flagValue = Cmd_FlagReadByName("image_W4"); */
  flagValue = NIL(char);

  if (flagValue == NIL(char)){
    option->W4 = 2; /* the default value */
  }
  else {
    option->W4 = atoi(flagValue);
  }
  return option;
}


/**Function********************************************************************

  Synopsis    [Forms the clusters of relations based on BDD size heuristic.]


  Description [The clusters are formed by taking the product in order. Once the
               BDD size of the cluster reaches a threshold, a new cluster is
               started.]
  SideEffects []            
******************************************************************************/

static array_t *
CreateClusters(bdd_manager *bddManager, array_t *relationArray,
               Iwls95Option_t *option)
{
  array_t *clusterArray;
  int i;
  bdd_t *cluster, *relation, *tempCluster;
  int flag;
  
  clusterArray = array_alloc(bdd_t *, 0);
  
  for (i=0; i<array_n(relationArray);){
    cluster = bdd_one(bddManager);
    flag = 0;
    do{
      relation = array_fetch(bdd_t *,relationArray, i);
      i++;
      tempCluster = bdd_and(cluster, relation, 1, 1);
      if ((bdd_size(tempCluster) <= option->clusterSize) || (flag == 0)){
        bdd_free(cluster);
        cluster = tempCluster;
        flag = 1;
      }
      else{
        bdd_free(tempCluster);
        i--;
        break;
      }
    } while (i < array_n(relationArray));
    array_insert_last(bdd_t *, clusterArray, cluster);
  }
  return clusterArray;
}


/**Function********************************************************************

  Synopsis    [This function returns an array of ordered relations and an array
               of BDD cubes (array of BDDs).  
               This consists of following steps:
               a. Initialize the array of ctrInfoStructs and varInfoStructs.
               b. Fill in the list of varItemStruct's of ctrInfo's and
                  ctrItemStruct's of varInfo's.
               c. Simplify the relations by quantifying out the quantify
                  variables local to a particular relation.
               d. Order the relations according to the cost function described
                  in "CalculateRelationBenefit".]
  Description [This function returns an array of ordered relations and an array
               of BDD cubes (array of BDDs).  
               This consists of following steps:
               a. Initialize the array of ctrInfoStructs and varInfoStructs.
               b. Fill in the list of varItemStruct's of ctrInfo's and
                  ctrItemStruct's of varInfo's.
               c. Simplify the relations by quantifying out the quantify
                  variables local to a particular relation.
               d. Order the relations according to the cost function described
                  in "CalculateRelationBenefit".]
  SideEffects []            

******************************************************************************/
static void
OrderRelationArray(mdd_manager *mddManager,
                   array_t *relationArray,
                   array_t *domainVarBddArray,
                   array_t *quantifyVarBddArray,
                   array_t *rangeVarBddArray,
                   Iwls95Option_t *option,
                   array_t **orderedRelationArrayPtr,
                   array_t **arraySmoothVarBddArrayPtr)
{
  array_t *quantifyVarBddIdArray, *domainVarBddIdArray, *rangeVarBddIdArray;
  array_t *domainAndQuantifyVarBddArray;
  array_t *ctrInfoArray, *varInfoArray, *simplifiedRelationArray;
  array_t *sortedMaxIndexArray;
  array_t *arrayDomainQuantifyVarsWithZeroNumCtr;
  int numRelation, numDomainVars, numQuantifyVars, numRangeVars;
  int numSmoothVars, numVars;
  int varId, bddId;
  int i;
  int *sortedMaxIndexVector;
  int numSmoothVarsRemaining, numIntroducedVarsRemaining;

  st_table  *bddIdToVarInfoTable, *bddIdToBddTable;

  bdd_t *relation, *bdd;

  CtrInfo_t *ctrInfo;
  VarInfo_t *varInfo;

  lsList remainingCtrInfoList;
  

  numRelation = array_n(relationArray);
  numDomainVars = array_n(domainVarBddArray);
  numRangeVars = array_n(rangeVarBddArray);
  numQuantifyVars = array_n(quantifyVarBddArray);
  numSmoothVars = numDomainVars + numQuantifyVars;
  numVars = numSmoothVars + numRangeVars;
  
  domainVarBddIdArray = bdd_get_varids(domainVarBddArray);
  rangeVarBddIdArray = bdd_get_varids(rangeVarBddArray);
  quantifyVarBddIdArray = bdd_get_varids(quantifyVarBddArray);
  domainAndQuantifyVarBddArray = array_join(domainVarBddArray,
                                            quantifyVarBddArray);  

  bddIdToBddTable = st_init_table(st_numcmp, st_numhash);
  HashIdToBddTable(bddIdToBddTable, domainVarBddIdArray,
                        domainVarBddArray);
  HashIdToBddTable(bddIdToBddTable, quantifyVarBddIdArray,
                        quantifyVarBddArray);
  HashIdToBddTable(bddIdToBddTable, rangeVarBddIdArray,
                        rangeVarBddArray);
  
  bddIdToVarInfoTable = st_init_table(st_numcmp, st_numhash);
  
      /*
       * Create the array of ctrInfo's for each component
       */

  ctrInfoArray = array_alloc(CtrInfo_t*, 0);
  varInfoArray = array_alloc(VarInfo_t*, 0);
  
      /*
       * Create the array of varInfo for each variable
       */
  
  for (i=0; i<numVars; i++){
    varInfo = VarInfoStructAlloc();
    if (i<numDomainVars){
      bddId = array_fetch(int, domainVarBddIdArray, i);
      varInfo->varType = domainVar_c;
    }
    else if (i < (numDomainVars+numQuantifyVars)){
      bddId = array_fetch(int, quantifyVarBddIdArray,
                          i-numDomainVars);
      varInfo->varType = quantifyVar_c;
    }
    else{
      bddId = array_fetch(int, rangeVarBddIdArray,
                          (i-(numDomainVars+numQuantifyVars)));
      varInfo->varType = rangeVar_c;
    }
    array_insert_last(VarInfo_t*, varInfoArray, varInfo);
    varInfo->bddId = bddId;
    st_insert(bddIdToVarInfoTable, (char *)(long) bddId, (char *)varInfo);
  }
  
      /*
       * Fill in the data structure of the ctrInfo and varInfo
       */

  for (i=0; i<numRelation; i++){
    st_table *supportTable;
    st_generator *stGen;
    VarItem_t *varItem;
    CtrItem_t *ctrItem;
    lsHandle varItemHandle, ctrItemHandle;
    
    ctrInfo = CtrInfoStructAlloc();
    array_insert_last(CtrInfo_t*, ctrInfoArray, ctrInfo);
    ctrInfo->ctrId = i;
    relation = array_fetch(bdd_t*, relationArray, i);
    supportTable = ImgBddGetSupportIdTable(relation);
    st_foreach_item(supportTable, stGen, (char **)&varId,
                    NIL(char *)){ 
      varInfo = NIL(VarInfo_t);
      if (st_lookup(bddIdToVarInfoTable, (char *)(long)varId,
                    (char **)&varInfo) == 0){
        /* This variable is of no interest, because it is not present
         * in the bddIdToVarInfoTable.
         */
        continue;
      }
      varItem = ALLOC(VarItem_t,1);
      varItem->varInfo = varInfo;
      (void) lsNewBegin(ctrInfo->varItemList, (lsGeneric)varItem,
                       (lsHandle *)&varItemHandle);
      ctrItem = ALLOC(CtrItem_t,1);
      ctrItem->ctrInfo = ctrInfo;
      (void) lsNewBegin(varInfo->ctrItemList, (lsGeneric)ctrItem,
                        (lsHandle *)&ctrItemHandle);
      varItem->ctrItemHandle = ctrItemHandle;
      ctrItem->varItemHandle = varItemHandle;
      varInfo->numCtr++;
    }
    st_free_table(supportTable);
  }/* Initialization of ctrInfoArray and varInfoArray complete */

  
  /*
   * Smooth out the quantify variables which are local to a particular cluster.
   */

  simplifiedRelationArray = RelationArraySmoothLocalVars(relationArray,
                                                         ctrInfoArray,
                                                         varInfoArray,
                                                         bddIdToBddTable);
  
  assert(CheckCtrInfoArray(ctrInfoArray, numDomainVars, numQuantifyVars,
                           numRangeVars));
  assert(CheckVarInfoArray(varInfoArray, numRelation));

  /*
   * In the ordering scheme of clusters, the number of variables yet to be
   * quatified out and number of variables which are yet to be introduced is
   * taken into account.
   * The number of smooth variables which are yet to be quantified out could
   * have changed. Also some of the range variables may not be in the
   * support of any of the clusters. This can happen while doing the
   * ordering for clusters for backward image (when a present state
   * variable does not appear in the support of any of the next state
   * functions. Recalculate these numbers.
   */
  
  numSmoothVarsRemaining = numSmoothVars;
  numIntroducedVarsRemaining = numRangeVars;
  
  /*
   * Find out which variables do not appear in the support of any cluster.
   * Update the numSmoothVarsRemaining and numIntroducedVarsRemaining
   * accordingly.
   * Also, these domainVars and quantifyVars can be quantified out as soon as
   * possible.
   */
  arrayDomainQuantifyVarsWithZeroNumCtr = array_alloc(bdd_t*, 0);
  for (i=0; i<numVars; i++){
    varInfo = array_fetch(VarInfo_t*, varInfoArray, i);
    if (varInfo->numCtr == 0){
      if ((varInfo->varType == domainVar_c) ||
          (varInfo->varType == quantifyVar_c)){
        numSmoothVarsRemaining--;
        st_lookup(bddIdToBddTable, (char *)(long)varInfo->bddId, (char
                                                                  **)&bdd);
        array_insert_last(bdd_t*, arrayDomainQuantifyVarsWithZeroNumCtr, bdd_dup(bdd));
      }
      else numIntroducedVarsRemaining--;
    }
  }

  /*
   * The ordering scheme also depends on the value of the maximum index of a
   * smooth variable which is yet to be quantified. For efficiency reasons, we
   * maintain a vector indicating the maximum index of the clusters. This
   * vector is sorted in the decreasing order of index. The rank of a cluster
   * is stored in its "rankMaxSmoothVarIndex" field.
   */
  sortedMaxIndexArray = array_dup(ctrInfoArray);
  array_sort(sortedMaxIndexArray, CtrInfoMaxIndexCompare);
  sortedMaxIndexVector = ALLOC(int, numRelation);
  for (i=0; i<numRelation; i++){
    ctrInfo = array_fetch(CtrInfo_t*, sortedMaxIndexArray, i);
    ctrInfo->rankMaxSmoothVarIndex = i;
    sortedMaxIndexVector[i] = ctrInfo->maxSmoothVarIndex;
  }
  array_free(sortedMaxIndexArray);
  
  
  /*
   * Create a list of clusters which are yet to be ordered. Right now
   * put all the clusters. Later on the list will contain only those
   * clusters which have not yet been ordered.
   */
  remainingCtrInfoList = lsCreate();
  for(i=0; i<numRelation; i++){
    lsHandle ctrHandle;
    ctrInfo = array_fetch(CtrInfo_t*, ctrInfoArray, i);
    lsNewBegin(remainingCtrInfoList, (lsGeneric)ctrInfo, &ctrHandle);
    ctrInfo->ctrInfoListHandle = ctrHandle;
  }

  
  /* Call the auxiliary routine to do the ordering. */
  OrderRelationArrayAux(simplifiedRelationArray, remainingCtrInfoList,
                        ctrInfoArray, varInfoArray, sortedMaxIndexVector,
                        numSmoothVarsRemaining, numIntroducedVarsRemaining,
                        bddIdToBddTable, option, domainAndQuantifyVarBddArray,
                        orderedRelationArrayPtr, arraySmoothVarBddArrayPtr);

  lsDestroy(remainingCtrInfoList,0);
  
  if (arraySmoothVarBddArrayPtr != NIL(array_t *)){
    /*
     * The first smoothVarBddArray is appended with
     * arrayDomainQuantifyVarsWithZeroNumCtr, because we would like to quantify
     * out the variables which do not appear in the support of any cluster as
     * soon as possible.
     */
    array_append(array_fetch(array_t*, *arraySmoothVarBddArrayPtr, 0),
                 arrayDomainQuantifyVarsWithZeroNumCtr);
  }
  else {
    /*
     * Need to free the elements of arrayDomainQuantifyVarsWithZeroNumCtr
     */
    for (i=0; i<array_n(arrayDomainQuantifyVarsWithZeroNumCtr); i++){
      bdd_free(array_fetch(bdd_t*, arrayDomainQuantifyVarsWithZeroNumCtr, i));
    }
  }
  
  array_free(arrayDomainQuantifyVarsWithZeroNumCtr);

/* Free the info arrays */

  for (i=0; i<numRelation; i++){
    ctrInfo = array_fetch(CtrInfo_t*, ctrInfoArray, i);
    CtrInfoStructFree(ctrInfo);
  }
  array_free(ctrInfoArray);
  
  for (i=0; i<numVars; i++){
    varInfo = array_fetch(VarInfo_t*, varInfoArray, i);
    assert(varInfo->numCtr == 0);
    VarInfoStructFree(varInfo);
  }
  array_free(varInfoArray);
  
  if (option->verbosity >= 3){
    int numRelation = array_n(*orderedRelationArrayPtr);
    
    if (arraySmoothVarBddArrayPtr != NIL(array_t*)){
      PrintSmoothIntroducedCount(*orderedRelationArrayPtr,
                                 arraySmoothVarBddArrayPtr,
                                 domainVarBddIdArray,
                                 rangeVarBddIdArray);
    }
    if (option->verbosity >= 4){
      for (i= 0; i <numRelation; i++){
        st_table *supportTable;
        (void) fprintf(stdout, "Cluster # %d\n",i);
        supportTable = ImgBddGetSupportIdTable(
          array_fetch(bdd_t*, *orderedRelationArrayPtr, i));   
        PrintBddIdTable(supportTable);
        if (arraySmoothVarBddArrayPtr != NIL(array_t*)){
          (void) fprintf(stdout, "Exist cube # %d\n",i);
          PrintBddIdFromBddArray(array_fetch(array_t*,
                                      *arraySmoothVarBddArrayPtr, i)); 
        }
      }
    }
  }
  FREE(sortedMaxIndexVector);
  array_free(simplifiedRelationArray);
  
  /* Free BDD Id arrays */
  array_free(domainVarBddIdArray);
  array_free(quantifyVarBddIdArray);
  array_free(rangeVarBddIdArray);
  array_free(domainAndQuantifyVarBddArray);
  /* Free the st_tables */
  st_free_table(bddIdToBddTable);
  st_free_table(bddIdToVarInfoTable);
}

/**Function********************************************************************

  Synopsis    [This function takes an array of relations and quantifies out the
               quantify variables which are local to a particular relation.]

  Description [This function takes an array of relations and quantifies out the
               quantify variables which are local to a particular relation.]

  SideEffects [This function fills in the "numSmoothVars",
               "numLocalSmoothVars", "numIntroducedVars", "maxSmoothVarIndex"
               fields of ctrInfoStruct corresponding to each relation. It also
               alters the "numCtr" and "ctrItemList" field of those quantify
               variables which are quantified out in this function.]

******************************************************************************/

static array_t *
RelationArraySmoothLocalVars(array_t *relationArray, array_t *ctrInfoArray,
                             array_t *varInfoArray, st_table *bddIdToBddTable)
{
  array_t *arraySmoothVarBddArray, *smoothVarBddArray ;
  array_t *simplifiedRelationArray;
  bdd_t *simplifiedRelation, *relation, *varBdd;
  int maxSmoothVarIndexForAllCtr, i, numRelation;
  
  
  numRelation = array_n(relationArray);
  
  /*
   * Initialize the array of cubes (of quantified variables which occur in
   * only one relation), to be smoothed out.
   */
  arraySmoothVarBddArray = array_alloc(array_t*, 0);
  for (i=0; i<numRelation; i++){
    smoothVarBddArray = array_alloc(bdd_t*, 0);
    array_insert_last(array_t*, arraySmoothVarBddArray,
                      smoothVarBddArray);
  }
  
  maxSmoothVarIndexForAllCtr = -1;
  for (i=0; i<numRelation; i++){
    VarItem_t *varItem;
    lsHandle varItemHandle;
    CtrInfo_t *ctrInfo = array_fetch(CtrInfo_t*, ctrInfoArray, i);
    int maxSmoothVarIndex = -1;
    array_t *smoothVarBddArray = array_fetch(array_t *, arraySmoothVarBddArray,
                                             i);  
    lsGen varItemListGen = lsStart(ctrInfo->varItemList);
    while (lsNext(varItemListGen, (lsGeneric *)&varItem, &varItemHandle) ==
           LS_OK){
      int  varBddId;
      VarInfo_t *varInfo  = varItem->varInfo;
      if (varInfo->varType == rangeVar_c){
        ctrInfo->numIntroducedVars++;
      }
      else if (varInfo->numCtr == 1){
        assert(lsLength(varInfo->ctrItemList) == 1);
        if (varInfo->varType == quantifyVar_c){
/*
 * The variable can be smoothed out. Put it in an array of variables to be
 * smoothed out from the relation.
 */
          CtrItem_t *ctrItem;
          varBddId = varInfo->bddId;
          st_lookup(bddIdToBddTable, (char *)(long)varBddId,(char **)&varBdd); 
          array_insert_last(bdd_t*, smoothVarBddArray, bdd_dup(varBdd));
          varInfo->numCtr = 0;
          /* Remove the cluster from the ctrItemList of varInfo */
          lsRemoveItem(varItem->ctrItemHandle, (lsGeneric *)&ctrItem);
          CtrItemStructFree(ctrItem);
          /* Remove the variable from the varItemList of ctrInfo.*/
          lsRemoveItem(varItemHandle, (lsGeneric *) &varItem);
          VarItemStructFree(varItem);
          continue;
        }
        else { /* Increase the numLocalSmoothVars count of the corresponding
                  ctr. */ 
          ctrInfo->numLocalSmoothVars++;
          ctrInfo->numSmoothVars++;
          if (maxSmoothVarIndex < varInfo->bddId){
            maxSmoothVarIndex = varInfo->bddId;
          }
        }
      }
      else{ /* varInfo->numCtr > 1 */
        assert(varInfo->numCtr > 1);
        ctrInfo->numSmoothVars++;
        if (maxSmoothVarIndex < varInfo->bddId){
          maxSmoothVarIndex = varInfo->bddId;
        }
      }
    }
    lsFinish(varItemListGen);
    if (maxSmoothVarIndex >= 0){
      ctrInfo->maxSmoothVarIndex = maxSmoothVarIndex;
    }
    else{
      ctrInfo->maxSmoothVarIndex = 0;
    }
    if (maxSmoothVarIndexForAllCtr < maxSmoothVarIndex){
      maxSmoothVarIndexForAllCtr = maxSmoothVarIndex;
    }
  }
  
  /*
   * Initialization Finished. We need to smooth out those quantify
   * variables which appear in only one ctr.
   */
  simplifiedRelationArray = array_alloc(bdd_t*, 0);
  for (i=0; i<numRelation; i++){
    relation = array_fetch(bdd_t*, relationArray, i);
    smoothVarBddArray = array_fetch(array_t*, arraySmoothVarBddArray, i);
    if (array_n(smoothVarBddArray) != 0){
      simplifiedRelation = bdd_smooth(relation, smoothVarBddArray);
    }
    else
      simplifiedRelation = bdd_dup(relation);
    array_insert_last(bdd_t*, simplifiedRelationArray, simplifiedRelation);
  }
  ArrayBddArrayFree(arraySmoothVarBddArray);
  return simplifiedRelationArray;
}

/**Function********************************************************************

  Synopsis    [An auxiliary routine for orderRelationArray to order the
  clusters.]

  Description [This routine is called by orderRelationArray after the
  initialization is complete and the cluster relations are simplified wrt to
  the quantify variable local to the cluster. The algorithm is:
      While (there exists a cluster to be ordered){
         Calculate benefit for each unordered cluster.
         Choose the cluster with the maximum benefit.
         Update the cost function parameters.
      }
  ]

  SideEffects [ctrInfo and varInfo structures are modified.]

******************************************************************************/
static void
OrderRelationArrayAux(array_t *relationArray,
                      lsList remainingCtrInfoList,
                      array_t *ctrInfoArray,
                      array_t *varInfoArray,
                      int *sortedMaxIndexVector,
                      int numSmoothVarsRemaining,
                      int numIntroducedVarsRemaining,
                      st_table *bddIdToBddTable,
                      Iwls95Option_t *option,
                      array_t *domainAndQuantifyVarBddArray,
                      array_t **orderedRelationArrayPtr,
                      array_t **arraySmoothVarBddArrayPtr) 
{
  int numRelation, ctrCount, currentRankMaxSmoothVarIndex;
  int maxIndex = 0;
  array_t *orderedRelationArray, *arraySmoothVarBddArray;
  array_t *smoothVarBddArray;
  int *bestCtrIdVector;
  int maxNumLocalSmoothVars, maxNumSmoothVars, maxNumIntroducedVars;
  lsGen lsgen;
  
  ctrCount = 0;
  currentRankMaxSmoothVarIndex = 0;

  numRelation = array_n(relationArray);

  arraySmoothVarBddArray  = NIL(array_t);
  orderedRelationArray = array_alloc(bdd_t*, 0);
  *orderedRelationArrayPtr = orderedRelationArray;
  if (arraySmoothVarBddArrayPtr != NIL(array_t *)){
    arraySmoothVarBddArray = array_alloc(array_t*, 0);
    *arraySmoothVarBddArrayPtr = arraySmoothVarBddArray;
  }
  bestCtrIdVector = ALLOC(int, numRelation);
  
  while (ctrCount < numRelation){
    float bestBenefit, benefit;
    int bestCtrId;
    bdd_t *bestRelation;
    lsGen ctrInfoListGen;
    CtrInfo_t *ctrInfo, *ctrInfoAux;
    
    bestBenefit = -9999;
    bestCtrId = -1;
    /* Find the maximum index of the remaining clusters */
    while
      ((currentRankMaxSmoothVarIndex < numRelation) &&
       ((maxIndex =
         sortedMaxIndexVector[currentRankMaxSmoothVarIndex++]) ==
        -1));
    
    /* Calculate the maximum values of local smooth variables etc. */
    maxNumLocalSmoothVars = 0;
    maxNumSmoothVars = 0;
    maxNumIntroducedVars = 0;
    lsForEachItem(remainingCtrInfoList, lsgen, ctrInfo){
      if (ctrInfo->numLocalSmoothVars > maxNumLocalSmoothVars){
        maxNumLocalSmoothVars = ctrInfo->numLocalSmoothVars;
      }
      if (ctrInfo->numSmoothVars > maxNumSmoothVars){
        maxNumSmoothVars = ctrInfo->numSmoothVars;
      }
      if (ctrInfo->numIntroducedVars > maxNumIntroducedVars){
        maxNumIntroducedVars = ctrInfo->numIntroducedVars;
      }
    }
    
    if (option->verbosity >= 4){
      fprintf(stdout, "maxNumLocalSmoothVars = %3d maxNumSmoothVars = %3d maxNumIntroducedVars = %3d\n",
              maxNumLocalSmoothVars, maxNumSmoothVars, maxNumIntroducedVars);
    }
    /* Calculate the cost function of each of the cluster */
    ctrInfoListGen = lsStart(remainingCtrInfoList);
    while (lsNext(ctrInfoListGen, (lsGeneric *)&ctrInfo, NIL(lsHandle)) ==
           LS_OK){ 
      /*
      benefit = CalculateBenefit(ctrInfo, numSmoothVarsRemaining,
                                 numIntroducedVarsRemaining, maxIndex, option);
      */
      benefit = CalculateBenefit(ctrInfo, maxNumLocalSmoothVars,
                                 maxNumSmoothVars, maxIndex,
                                 maxNumIntroducedVars, option);

      if (option->verbosity >= 4){
        fprintf(stdout, "numLocalSmoothVars = %3d numSmoothVars = %3d benefit = %4.2f\n",
                ctrInfo->numLocalSmoothVars, ctrInfo->numSmoothVars, benefit);
      }
      if (benefit > bestBenefit){
        bestBenefit = benefit;
        bestCtrId = ctrInfo->ctrId;
      }
    }
    lsFinish(ctrInfoListGen);
    /*
     * Insert the relation in the ordered array of relations and put in the
     * appropriate cubes.
     */
    
    bestCtrIdVector[ctrCount] = bestCtrId;
    ctrInfo = array_fetch(CtrInfo_t*, ctrInfoArray, bestCtrId);
    lsRemoveItem(ctrInfo->ctrInfoListHandle, (lsGeneric *)&ctrInfoAux);
    assert(ctrInfo == ctrInfoAux);
    bestRelation = array_fetch(bdd_t*, relationArray, bestCtrId);
    array_insert_last(bdd_t*, orderedRelationArray, bestRelation);
    if (option->verbosity >= 4){
      fprintf(stdout, "Best benefit = %4.2f numLocalSmoothVars = %3d numSmoothVars = %3d\n",
              bestBenefit, ctrInfo->numLocalSmoothVars, ctrInfo->numSmoothVars);
    }
    /*
     * Update the remaining ctrInfo's and the varInfo's affected by choosing
     * this cluster. Also get the array of smooth variables which can be
     * quantified once this cluster is multiplied in the product.
     */
    smoothVarBddArray = UpdateInfoArrays(ctrInfo, bddIdToBddTable,
                                         &numSmoothVarsRemaining,
                                         &numIntroducedVarsRemaining);
    assert(CheckCtrInfo(ctrInfo, numSmoothVarsRemaining, 0,
                        numIntroducedVarsRemaining));
  
    if (arraySmoothVarBddArrayPtr != NIL(array_t *)){
      if (ctrCount == numRelation-1){
        /*
         * In the last element of arraySmoothVarBddArray, put all the domain
         * and quantify variables (in case some of them were not in the support
         * of any cluster).
         */
        BddArrayFree(smoothVarBddArray);
        smoothVarBddArray = BddArrayDup(domainAndQuantifyVarBddArray);
      }
      array_insert_last(array_t*, arraySmoothVarBddArray, smoothVarBddArray);
    }
    else{
      BddArrayFree(smoothVarBddArray);
    }
    sortedMaxIndexVector[ctrInfo->rankMaxSmoothVarIndex] = -1;
    ctrCount++;
  }
  assert(numIntroducedVarsRemaining == 0);
  assert(numSmoothVarsRemaining == 0);
  if (option->verbosity >= 3){
    int i;
    (void) fprintf(stdout,"Cluster Sequence = ");
    for (i=0; i<numRelation; i++){
      (void) fprintf(stdout, "%d ", bestCtrIdVector[i]);
    }
    (void) fprintf(stdout,"\n");
  }
  
  FREE(bestCtrIdVector);
}


/**Function********************************************************************

  Synopsis    [This function computes the set of variables which can be
  smoothed out once a particular cluster is chosen to be multiplied in the
  product.]

  Description [The support variable list (varItemList) of the cluster (ctrInfo)
  is traversed and depending upon the type of the variable and the number of
  unordered clustered relations which depend on that variable following actions
  are taken:

  a. If the variable is of range type:
     This implies that this variable is appearing for the first time in the
     product. Since it is already introduced in the product, the cost function
     of any other unordered relation which depends on this variable will get
     modified. The numIntroducedVariables field of each of the cluster which
     depends on this variable is decreased by 1. Also this varItemStruct
     corresponding to this variable is removed from the varItemList field of
     the cluster.
     
  b. If the variable is of domain or quantify type:
    b1. If number of clusters which depend on this variable is 1 (numCtr == 1):
           In this case, this variable can be quantified out once the chosen
           cluster is multiplied in the product. Hence the variable is put in
           the smoothVarBddArray.
    b2. If  (numCtr == 2)
           In this case, there is one more unordered cluster which depends on
           this variable. But once the current cluster is multiplied in the
           product, the "numLocalSmoothVars" field of ctrInfo corresponding to
           the other dependent cluster needs to be increased by 1.
     b3. If (numCtr > 2)
           In this case, we just need to decrease the numCtr of the variable by
           1.

  In any case, for each varInfo in the support variable list (varItemList) of
  the cluster (ctrInfo) the following invariant is maintained:
  varInfo->numCtr == lsLength(varInfo->ctrItemList)
  ]
  
  SideEffects [The fields of ctrInfo and varInfo are changed as mentioned
  above.]

******************************************************************************/
static array_t *
UpdateInfoArrays(CtrInfo_t *ctrInfo, st_table *bddIdToBddTable,
                 int *numSmoothVarsRemainingPtr,
                 int *numIntroducedVarsRemainingPtr)
{
  array_t *smoothVarBddArray;
  lsGen varItemListGen;
  int numSmoothVarsRemaining = *numSmoothVarsRemainingPtr;
  int numIntroducedVarsRemaining = *numIntroducedVarsRemainingPtr;
  VarItem_t *varItem;
  lsHandle varItemHandle;
  
  smoothVarBddArray = array_alloc(bdd_t*, 0);
  varItemListGen = lsStart(ctrInfo->varItemList);
  while (lsNext(varItemListGen, (lsGeneric *)&varItem, &varItemHandle) ==
         LS_OK){ 
    VarInfo_t *varInfo = varItem->varInfo;
    CtrItem_t *ctrItem;
    assert(varInfo->numCtr == lsLength(varInfo->ctrItemList));
    lsRemoveItem(varItem->ctrItemHandle, (lsGeneric *) &ctrItem);
    CtrItemStructFree(ctrItem);
    varInfo->numCtr--;
    lsRemoveItem(varItemHandle, (lsGeneric *)&varItem);
    VarItemStructFree(varItem);
    
    /*
     * If this variable is to be smoothed (domain or quantify type) and
     * the numCtr is 1, then it should be added to the smoothVarBddArray,
     * otherwise, the numCtr should be decreased by 1.
     * If the variable is of the range type then the number of introduced
     * vars remaining and the number of introduced vars should be
     * appropriately modified.
     */
    if ((varInfo->varType == domainVar_c) ||
        (varInfo->varType == quantifyVar_c)){
      if (varInfo->numCtr == 0){
        bdd_t *varBdd;
        st_lookup(bddIdToBddTable, (char *)(long)(varInfo->bddId), (char **)&varBdd);
        array_insert_last(bdd_t*, smoothVarBddArray, bdd_dup(varBdd));
        numSmoothVarsRemaining--;
        ctrInfo->numLocalSmoothVars--;
        ctrInfo->numSmoothVars--;
      }
      else{
        if (varInfo->numCtr == 1){
          /*
           * We need to update the numLocalSmoothVars of the ctr
           * which depends on it.
           */
          lsFirstItem(varInfo->ctrItemList, (lsGeneric *)&ctrItem,
                      LS_NH); 
          ctrItem->ctrInfo->numLocalSmoothVars++;
        }
        /*
         * else varInfo->numCtr > 1 : Nothing to be done because neither it
         * can be quantified out, nor it is effecting any cost function.
         */
        ctrInfo->numSmoothVars--;
      }
    }
    else{/* The variable is of range type, so need to appropriately modify the
          * numIntroducedVars of those clusters which contain this range
          * variable in their support.
          */
      lsHandle ctrItemHandle;
      lsGen ctrItemListGen = lsStart(varInfo->ctrItemList);
      ctrInfo->numIntroducedVars--;
      while (lsNext(ctrItemListGen, (lsGeneric *)&ctrItem,
                    &ctrItemHandle) == LS_OK){
        ctrItem->ctrInfo->numIntroducedVars--;
        lsRemoveItem(ctrItem->varItemHandle,(lsGeneric *)&varItem);
        lsRemoveItem(ctrItemHandle,(lsGeneric *)&ctrItem);
        VarItemStructFree(varItem);
        CtrItemStructFree(ctrItem);
      }
      lsFinish(ctrItemListGen);
      numIntroducedVarsRemaining--;
      varInfo->numCtr = 0;
    }
    assert(varInfo->numCtr == lsLength(varInfo->ctrItemList));
  }
  lsFinish(varItemListGen);
  *numIntroducedVarsRemainingPtr = numIntroducedVarsRemaining;
  *numSmoothVarsRemainingPtr = numSmoothVarsRemaining;
  return smoothVarBddArray;
}

        
            
    
/**Function********************************************************************

  Synopsis    [Gets the necessary options for computing the image and returns
               in the option structure.]

  Description [
       The strategy
       Choose the cost function:
       The objective function attached with each Ti is
       Ci =  W1 C1i + W2 C2i + W3 C3i - W4C4i
       where:
       W1 = weight attached with variable getting smoothed
       W2 = weight attached with the support count of the Ti
       W3 = weight attached with the max id of the Ti
       W4 = weight attached with variable getting introduced
       C1i = Ui/Vi
       C2i = Vi/Wi
       C3i = Mi/Max
       C4i = Xi/Yi
       Ui = number of variables getting smoothed
       Vi = number of ps support variables of Ti
       Wi = total number of ps variables remaining which have not been
            smoothed out
       Mi = value of Max id of Ti
       Max = value of Max id across all the Ti's remaining to be multiplied
       Xi = number of ns variables introduced
       Yi = total number of ns variables which have not been introduced so
            far.
       Get the weights from the global option]

  SideEffects []

******************************************************************************/
static float
CalculateBenefit(CtrInfo_t *ctrInfo, int maxNumLocalSmoothVars, int
                 maxNumSmoothVars, int maxIndex, int
                 maxNumIntroducedVars, Iwls95Option_t *option)   
{
  int W1, W2, W3, W4;
  float benefit;
  
  W1 = option->W1;
  W2 = option->W2;
  W3 = option->W3;
  W4 = option->W4;
  
  benefit = 0;
  benefit += (maxNumLocalSmoothVars ?
              (W1  *
               ((float)ctrInfo->numLocalSmoothVars/maxNumLocalSmoothVars))
              : 0);

  benefit += (maxNumSmoothVars ?
              (W2  * ((float)ctrInfo->numSmoothVars/maxNumSmoothVars))
              : 0);
  
  benefit += (maxIndex ? (W3  * ((float)ctrInfo->maxSmoothVarIndex/maxIndex))
              :  0);

  benefit -= (maxNumIntroducedVars ?
              (W4  *
               ((float)ctrInfo->numIntroducedVars/maxNumIntroducedVars)) : 0);
  
  return benefit;
}

/**Function********************************************************************

  Synopsis    [Prints the option values used in IWLS95 techinique for
  image computation.]

  Description [Prints the option values used in IWLS95 techinique for
  image computation.]

  SideEffects []

******************************************************************************/
static void
PrintOption(Iwls95Option_t *option, FILE *fp)
{
  (void) fprintf(fp,"Printing Information about Image method: IWLS95\n"); 
  (void) fprintf(fp,"\tThreshold Value of Bdd Size For Creating Clusters = %d\n", option->clusterSize);
  (void)fprintf(fp,"\t\t(Use \"set image_cluster_size value \" to set this to desired value) \n"); 
  (void) fprintf(fp,"\tVerbosity = %d\n\t\t(Use \"set image_verbosity value \" to set this to desired value) \n", option->verbosity);
  (void) fprintf(fp,"\tW1 =%3d W2 =%2d W3 =%2d W4 =%2d\n\t\t(Use \"set image_W? value \" to set these to desired values) \n", option->W1,
  option->W2, option->W3, option->W4);
}

/**Function********************************************************************

  Synopsis    [Allocates and initializes the info structure for IWLS95
  technique.] 

  Description [Allocates and initializes the info structure for IWLS95
  technique.]

  SideEffects []

******************************************************************************/
static Iwls95Info_t *
Iwls95InfoStructAlloc()
{
  Iwls95Info_t *info;
  info = ALLOC(Iwls95Info_t, 1);
  info->fwdClusteredRelationArray = NIL(array_t);
  info->fwdArraySmoothVarBddArray = NIL(array_t);
  info->bwdClusteredRelationArray = NIL(array_t);
  info->bwdArraySmoothVarBddArray = NIL(array_t);
  info->toCareSet = NIL(bdd_t);
  info->bwdClusteredCofactoredRelationArray = NIL(array_t);
  info->option = Iwls95GetOptions();
  return info;
}

/**Function********************************************************************

  Synopsis    [Allocates and initializes memory for ctrInfoStruct.]

  Description [Allocates and initializes memory for ctrInfoStruct.]

  SideEffects []

******************************************************************************/
static CtrInfo_t *
CtrInfoStructAlloc(void)
{
  CtrInfo_t *ctrInfo;
  ctrInfo = ALLOC(CtrInfo_t, 1);
  ctrInfo->ctrId = -1;
  ctrInfo->numLocalSmoothVars = 0;
  ctrInfo->numSmoothVars = 0;
  ctrInfo->maxSmoothVarIndex = -1;
  ctrInfo->numIntroducedVars = 0;
  ctrInfo->varItemList = lsCreate();
  ctrInfo->ctrInfoListHandle = NULL;
  return ctrInfo;
}


/**Function********************************************************************

  Synopsis    [Frees the memory associated with ctrInfoStruct.]

  Description [Frees the memory associated with ctrInfoStruct.]

  SideEffects []

******************************************************************************/
static void
CtrInfoStructFree(CtrInfo_t *ctrInfo)
{
  lsDestroy(ctrInfo->varItemList, 0);
  FREE(ctrInfo);
}

/**Function********************************************************************

  Synopsis    [Allocates and initializes memory for varInfoStruct.]

  Description [Allocates and initializes memory for varInfoStruct.]

  SideEffects []

******************************************************************************/
static VarInfo_t *
VarInfoStructAlloc(void)
{
  VarInfo_t *varInfo;
  varInfo = ALLOC(VarInfo_t, 1);
  varInfo->bddId = -1;
  varInfo->numCtr = 0;
  varInfo->varType = -1;
  varInfo->ctrItemList = lsCreate();
  return varInfo;
}

/**Function********************************************************************

  Synopsis    [Frees the memory associated with varInfoStruct.]

  Description [Frees the memory associated with varInfoStruct.]

  SideEffects []

******************************************************************************/
static void
VarInfoStructFree(VarInfo_t *varInfo)
{
  lsDestroy(varInfo->ctrItemList,0);
  FREE(varInfo);
}

/**Function********************************************************************

  Synopsis    [Frees the memory associated with CtrItemStruct]

  Description [Frees the memory associated with CtrItemStruct]

  SideEffects []

******************************************************************************/
static void
CtrItemStructFree(CtrItem_t *ctrItem)
{
  FREE(ctrItem);
}

/**Function********************************************************************

  Synopsis    [Frees the memory associated with VarItemStruct]

  Description [Frees the memory associated with VarItemStruct]

  SideEffects []

******************************************************************************/
static void
VarItemStructFree(VarItem_t *varItem)
{
  FREE(varItem);
}

/**Function********************************************************************

  Synopsis    [Compare function used to sort the ctrInfoStruct based on the
  maxSmoothVarIndex field.]

  Description [This function is used to sort the array of clusters based on the
  maximum index of the support variable.]

  SideEffects []

******************************************************************************/
static int
CtrInfoMaxIndexCompare(CtrInfo_t** infoCtr1, CtrInfo_t **infoCtr2)
{
  return ((*infoCtr1)->maxSmoothVarIndex > (*infoCtr2)->maxSmoothVarIndex);
}

/* **************************************************************************
 * Debugging Related Routines.
 ***************************************************************************/


/**Function********************************************************************

  Synopsis    [Prints the CtrInfo_t data structure.]

  Description [Prints the CtrInfo_t data structure.]

  SideEffects []

******************************************************************************/
static void
PrintCtrInfoStruct(CtrInfo_t *ctrInfo)
{
  lsGen lsgen;
  VarItem_t *varItem;
  
  (void) fprintf(stdout,"Ctr ID = %d\tNumLocal = %d\tNumSmooth=%d\tNumIntro=%d\tmaxSmooth=%d\trank=%d\n",
                 ctrInfo->ctrId, ctrInfo->numLocalSmoothVars,
                 ctrInfo->numSmoothVars, ctrInfo->numIntroducedVars,
                 ctrInfo->maxSmoothVarIndex, ctrInfo->rankMaxSmoothVarIndex);
  lsgen = lsStart(ctrInfo->varItemList);
  while (lsNext(lsgen, (lsGeneric *)&varItem, NIL(lsHandle)) == LS_OK){
    (void) fprintf(stdout,"%d\t", varItem->varInfo->bddId);
  }
  lsFinish(lsgen);
  fprintf(stdout,"\n");
}
/**Function********************************************************************

  Synopsis    [Prints the VarInfo_t structure.]

  Description [Prints the VarInfo_t structure.]

  SideEffects []

******************************************************************************/
static void
PrintVarInfoStruct(VarInfo_t *varInfo)
{
  lsGen lsgen;
  CtrItem_t *ctrItem;
  
  (void) fprintf(stdout,"Var ID = %d\tNumCtr = %d\tVarType=%d\n",
                 varInfo->bddId, varInfo->numCtr, varInfo->varType);
  lsgen = lsStart(varInfo->ctrItemList);
  while (lsNext(lsgen, (lsGeneric *)&ctrItem, NIL(lsHandle)) == LS_OK){
    (void) fprintf(stdout,"%d\t", ctrItem->ctrInfo->ctrId);
  }
  lsFinish(lsgen);
  fprintf(stdout,"\n");
}

/**Function********************************************************************

  Synopsis    [Given an array of BDD's representing the relations and another
  array of BDD cubes (in the form of array of bdd_t of variable) representing
  the quantification schedule, this function checks if the schedule is correct.
  ]

  Description [Assume Ci represents the ith cube in the array and the Ti
  represents the ith relation. The correction of the schedule is defined as
  follows: 
      a. For all Tj: j > i, SUP(Tj) and SUP(Ci) do not intersect, i.e., the
      variables which are quantified in Ci should not appear in the Tj for j>i.
      b. For any i, j, SUP(Ci) and SUP(Cj) do not intersect. However this is
      not true for the last cube (Cn-1). This is because the last cube contains
      all the smooth variables.
  ]      
  SideEffects []

******************************************************************************/
static int
CheckQuantificationSchedule(array_t *relationArray,
                            array_t *arraySmoothVarBddArray)
{
  int i, j, varId;
  st_table *smoothVarTable, *supportTable;
  bdd_t *relation;
  array_t *smoothVarBddArray, *smoothVarBddIdArray;
  st_generator *stgen;
  
  assert(array_n(relationArray) == array_n(arraySmoothVarBddArray));

  smoothVarTable = st_init_table(st_numcmp, st_numhash);
  
  for (i=0; i<array_n(relationArray); i++){
    relation = array_fetch(bdd_t*, relationArray, i);
    supportTable = ImgBddGetSupportIdTable(relation);
    /*
     * Check that none of the variables in the support have already been
     * quantified.
     */
    st_foreach_item(supportTable, stgen, (char **)&varId, (char
                                                           **)NIL(char*)){
      assert(st_is_member(smoothVarTable, (char *)(long)varId) == 0);
      
    }
    st_free_table(supportTable);
    if (i == (array_n(relationArray)-1)){
      /* Since last element of arraySmoothVarBddArray has all the
       * smooth variables, it will not satisfy the condition.
       */
      continue;
    }
    
    /*
     * Put each of the variables quantified at this step in the
     * smoothVarTable. And check that none of these smooth variables
     * have been introduced before.
     */
    smoothVarBddArray = array_fetch(array_t*, arraySmoothVarBddArray,
                                    i);
    smoothVarBddIdArray = bdd_get_varids(smoothVarBddArray);
    for (j=0; j<array_n(smoothVarBddIdArray); j++){
      varId = array_fetch(int, smoothVarBddIdArray, j);
      assert(st_insert(smoothVarTable, (char *)(long)varId, NIL(char))==0);
    }
    array_free(smoothVarBddIdArray);
  }
  st_free_table(smoothVarTable);
  return 1;
}

/**Function********************************************************************

  Synopsis    [Checks the validity of an array of CtrInfoStructs.]

  Description [Checks the validity of an array of CtrInfoStructs.]

  SideEffects []

******************************************************************************/
static int
CheckCtrInfoArray(array_t *ctrInfoArray, int numDomainVars, int
                  numQuantifyVars, int numRangeVars)
{
  int i;
  for (i=0; i<array_n(ctrInfoArray); i++){
    CheckCtrInfo(array_fetch(CtrInfo_t *, ctrInfoArray, i),
                 numDomainVars, numQuantifyVars, numRangeVars);
  }
  return 1;
}

/**Function********************************************************************

  Synopsis    [Checks the validity of a CtrInfoStruct.]

  Description [Checks the validity of a CtrInfoStruct.]

  SideEffects []

******************************************************************************/
static int
CheckCtrInfo(CtrInfo_t *ctrInfo, int numDomainVars, int numQuantifyVars,  int
             numRangeVars)
{
  assert(ctrInfo->numLocalSmoothVars <= (numDomainVars +
                                         numQuantifyVars));
  assert(ctrInfo->numSmoothVars <= (numDomainVars +
                                    numQuantifyVars));
  assert(ctrInfo->numIntroducedVars <= numRangeVars);
  assert(lsLength(ctrInfo->varItemList) ==
         (ctrInfo->numSmoothVars+ctrInfo->numIntroducedVars));
  return 1;
}

/**Function********************************************************************

  Synopsis    [Checks the validity of an array of VarInfoStruct.]

  Description [Checks the validity of an array of VarInfoStruct.]

  SideEffects []

******************************************************************************/
static int
CheckVarInfoArray(array_t *varInfoArray, int numRelation)
{
  int i;
  for (i=0; i<array_n(varInfoArray); i++){
    VarInfo_t *varInfo;
    varInfo = array_fetch(VarInfo_t *, varInfoArray, i);
    assert(varInfo->numCtr <= numRelation);
    assert(varInfo->varType >= 0);
    assert(varInfo->varType <= 2);
    assert(lsLength(varInfo->ctrItemList) == varInfo->numCtr);
  }
  return 1;
}

/* ****************************************************************
 * Utility Routines.
 ****************************************************************/



/**Function********************************************************************

  Synopsis    [Returns an array of BDD ids corresponding to a MDD variable.]

  Description [This function takes an MddId. It returns an array of BDD ids
  corresponding to the bits.]

  SideEffects []

******************************************************************************/
static array_t *
MddIdToBddIdArray(mdd_manager *mddManager, int mddId)
{
  array_t     *bddIdArray;
  mvar_type   mddVar;
  unsigned int    i, j;
  
  mddVar = array_fetch(mvar_type, mdd_ret_mvar_list(mddManager), mddId);
  bddIdArray = array_alloc(int, mddVar.encode_length);
  
  for (i=0; i<mddVar.encode_length; i++){
    j = mdd_ret_bvar_id(&mddVar, i);
    array_insert_last(int, bddIdArray, j);
  }
  return bddIdArray;
}

/**Function********************************************************************

  Synopsis    [Returns an array of Bdd_t's corresponding to a Mdd variable.]

  Description [This function takes an MddId. It returns an array of bdd_t's
  corresponding to the bits.]

  SideEffects []

******************************************************************************/
static array_t *
MddIdToBddArray(mdd_manager *mddManager, int mddId)
{
  array_t     *bddArray;
  mvar_type   mddVar;
  unsigned int    i, j;
  
  mddVar = array_fetch(mvar_type, mdd_ret_mvar_list(mddManager), mddId);
  bddArray = array_alloc(bdd_t*, mddVar.encode_length);
  
  for (i=0; i<mddVar.encode_length; i++){
    j = mdd_ret_bvar_id(&mddVar, i);
    array_insert_last(bdd_t*, bddArray,
                      bdd_get_variable((bdd_manager *)mddManager, j));
  }
  return bddArray;
}


/**Function********************************************************************

  Synopsis    [Returns an array of Bdd_t's corresponding to an array of Mdd
  ids.] 

  Description [This function takes an array of MddId's. For each MddId it
  returns an array of bdd_t's corresponding to the bits. These arrays of bdd_ts
  are concatenated together and returned.]

  SideEffects []

******************************************************************************/
static array_t *
MddIdArrayToBddArray(mdd_manager *mddManager, array_t *mddIdArray)
{
  array_t *bddArray;
  int i;

  bddArray = array_alloc(bdd_t*, 0);
  for (i=0; i<array_n(mddIdArray); i++){
    int mddId;
    array_t *tmpBddArray;
    
    mddId = array_fetch(int, mddIdArray, i);
    tmpBddArray = MddIdToBddArray(mddManager, mddId);
    array_append(bddArray, tmpBddArray);
    array_free(tmpBddArray);
  }
  return bddArray;
}


/**Function********************************************************************

  Synopsis    [Frees an array of MDDs.]

  Description [Frees an array of MDDs.]

  SideEffects []

******************************************************************************/
static void
MddArrayFree(array_t *mddArray)
{
  int i;

  for (i=0; i<array_n(mddArray); i++){
    mdd_t *mdd;
    mdd = array_fetch(mdd_t*, mddArray, i);
    mdd_free(mdd);
  }
  array_free(mddArray);
}


/**Function********************************************************************

  Synopsis    [Duplicates an array of BDDs.]

  Description [Duplicates an array of BDDs.]

  SideEffects []

******************************************************************************/
static array_t *
BddArrayDup(array_t *bddArray)
{
  int i;
  array_t *resultArray;
  resultArray = array_alloc(bdd_t*, 0);
  for (i=0; i<array_n(bddArray); i++){
    bdd_t *bdd;
    bdd = array_fetch(bdd_t*, bddArray, i);
    array_insert_last(bdd_t*, resultArray, bdd_dup(bdd));
  }
  return resultArray;
}


/**Function********************************************************************

  Synopsis    [Frees an array of BDDs.]

  Description [Frees an array of BDDs.]

  SideEffects []

******************************************************************************/
static void
BddArrayFree(array_t *bddArray)
{
  int i;

  for (i=0; i<array_n(bddArray); i++){
    bdd_t *bdd;
    bdd = array_fetch(bdd_t*, bddArray, i);
    bdd_free(bdd);
  }
  array_free(bddArray);
}


/**Function********************************************************************

  Synopsis    [Frees an array of array of BDDs.]

  Description [Frees an array of array of BDDs.]

  SideEffects []

******************************************************************************/
static void
ArrayBddArrayFree(array_t *arrayBddArray)
{
  int i;

  for (i=0; i<array_n(arrayBddArray); i++){
    array_t *bddArray;
    bddArray = array_fetch(array_t*, arrayBddArray, i);
    BddArrayFree(bddArray);
  }
  array_free(arrayBddArray);
}


/**Function********************************************************************

  Synopsis [Given an Mvf representing the functionality of a multi-valued
  variable, it returns an array of Bdd's representing the characteristic
  function of the relation of the various bits of the multi-valued variable.] 

  Description [Suppose y is a k-valued variable and takes values
              0,1,..,k-1. Then the input to this function is an array with k
              Mdds each representing the onset of the respective value of the
              variable (the ith Mdd representing the onset when y takes the
              value (i-1). Suppose m bits are needed to encode the k values of
              y. Then internally y is represented as y_0, y_1, ...,
              y_(m-1). Now the functionality of each bit of y can be computed
              by proper boolean operation on the functions representing the
              onsets of various values of y. For instance if y is a 4-valued
              variable. To achieve that we do the following:
              For each bit b{
                relation = 0;
                For each value j of the variable{
                  Ej = Encoding function of the jth value
                  Fj = Onset function of the jth value
                  If (b appears in the positive phase in Ej) then
                     relation += b * Fj 
                  else if (b appears in the negative phase in Ej) then
                     relation += b'* Fj
                  else if (b does not appear in Ej) then
                     relation += Ej
                }
              }
              Note that the above algorithm does not handle the case when a
              bit appears in both phases in the encoding of any value of the
              variable. Hence the assumption behind the above algorithm is that
              the values are encoded as cubes.
              The case when the encoding are functions can be handled by more
              complex algorithm. In that case, we will not be able to build the
              relation for each bit separately. Something to be dealt with in
              the later work.
              ]              
  SideEffects []

******************************************************************************/
static array_t *
MddFnArrayToBddRelArray(mdd_manager *mddManager, int mddId,
                        array_t *mddFnArray)
{
  array_t *bddRelationArray, *mddLiteralArray, *valueArray, *bddArray;
  mvar_type mddVar;
  int i, j, numValues, numEncodingBits;
  
  mddVar = array_fetch(mvar_type, mdd_ret_mvar_list(mddManager), mddId);
  numValues = array_n(mddFnArray);
  assert(mddVar.values == numValues);

  /*
   * The following is to check whether each encoding is cube or not.
   * Since Berkeley MDD package always does the cube encoding this checking has
   * been turned off currently.
   */
  
  valueArray = array_alloc(int, 1);
  mddLiteralArray = array_alloc(mdd_t*, 0);
  for (i=0; i<numValues; i++){
    mdd_t *mddLiteral;
    
    array_insert(int, valueArray, 0, i);
    /* Form the Mdd corresponding to this value */
    mddLiteral = mdd_literal(mddManager, mddId, valueArray);
    /* Check if this is a cube */
    if (bdd_is_cube(mddLiteral) == FALSE){ 
       fprintf(stderr, "The encoding of the variable %s for the value %d isnot a cube.\n It can result in wrong answers.\n", mddVar.name, i); 
    } 
    array_insert_last(mdd_t*, mddLiteralArray, mddLiteral);
  }
  array_free(valueArray);

  bddRelationArray = array_alloc(bdd_t*, 0);
  numEncodingBits = mddVar.encode_length;
  bddArray = MddIdToBddArray(mddManager, mddId);
  for (i=0; i<numEncodingBits; i++){
    bdd_t *bdd, *bddRelation, *bddNot;

    bddRelation = bdd_zero((bdd_manager *)mddManager);
    bdd = array_fetch(bdd_t*, bddArray, i);
    bddNot = bdd_not(bdd);
    for (j=0; j<numValues; j++){
      bdd_t *mddFn, *posCofactor, *negCofactor, *tmpBddRelation;
      mdd_t *mddLiteral, *literalRelation;
      
      mddLiteral = array_fetch(mdd_t*, mddLiteralArray, j);
      posCofactor = bdd_cofactor(mddLiteral, bdd);
      negCofactor = bdd_cofactor(mddLiteral, bddNot);
      mddFn = array_fetch(mdd_t*, mddFnArray, j);
      if (bdd_is_tautology(posCofactor, 0)){
        literalRelation = bdd_and(bddNot, mddFn, 1, 1);
      }
      else if (bdd_is_tautology(negCofactor, 0)){
        literalRelation = bdd_and(bdd, mddFn, 1, 1);
      }
      else {
        assert(bdd_equal(posCofactor, negCofactor));
        literalRelation = bdd_dup(mddFn);
      }
      bdd_free(posCofactor);
      bdd_free(negCofactor);
      tmpBddRelation = bdd_or(bddRelation, literalRelation, 1, 1);
      bdd_free(literalRelation);
      bdd_free(bddRelation);
      bddRelation = tmpBddRelation;
    }
    array_insert_last(bdd_t*, bddRelationArray, bddRelation);
    bdd_free(bdd);
    bdd_free(bddNot);
  }
  /* Free stuff */
  MddArrayFree(mddLiteralArray);
  array_free(bddArray);
  return bddRelationArray;
}


/**Function********************************************************************

  Synopsis    [Returns a BDD after taking the product of fromSet with the BDDs
  in the relationArray and quantifying out the variables using the schedule
  given in the arraySmoothVarBddArray.]

  Description [The product is taken from the left, i.e., fromSet is multiplied
  with relationArray[0]. The array of variables given by
  arraySmoothVarBddArray[i] are quantified when the relationArray[i] is
  multiplied in the product. We notice that no simplification is used
  in the computation. It was found to be the fastest way to compute
  the image.]

  SideEffects []

******************************************************************************/

static bdd_t *
BddLinearAndSmooth(bdd_manager *mddManager,
                   bdd_t *fromSet,
                   array_t *relationArray,
                   array_t *arraySmoothVarBddArray, int verbosity)
{
  int i;
  bdd_t *product = bdd_dup(fromSet);
  
  long intermediateSize = 0;
  long maxSize = 0;

  for (i=0; i<array_n(relationArray); i++){
    bdd_t *relation, *tmpProduct;
    array_t *smoothVarBddArray;
    relation = array_fetch(bdd_t*, relationArray, i);
    smoothVarBddArray = array_fetch(array_t*, arraySmoothVarBddArray, i);
    if (array_n(smoothVarBddArray) == 0){
      tmpProduct = bdd_and(product, relation, 1, 1);
    }
    else{
      tmpProduct = bdd_and_smooth(product, relation,
                                  smoothVarBddArray); 
      
    }
    bdd_free(product);
    product = tmpProduct;
    if (verbosity >= 2){
      intermediateSize = bdd_size(product);
      if (maxSize < intermediateSize){
        maxSize = intermediateSize;
      }
    }
  }
  if (verbosity >= 2){
    (void) fprintf(stdout, "Max. BDD size for intermediate product = %10ld\n", maxSize);
  }
  return product;
}

/**Function********************************************************************

  Synopsis    [Hashes bdd id to bdd in the table.]

  Description [The elements of the input array should be in one to one 
  correpondence, i.e., idArray[i] should be the id of the variable
  bddArray[i]. Each element of idArray is hashed in the table with the value
  taken from the corresponding element from bddArray.] 

  SideEffects []

******************************************************************************/

static void
HashIdToBddTable(st_table *table, array_t *idArray,
                             array_t *bddArray)
{
  int i;
  for (i=0; i<array_n(idArray); i++){
    int id;
    bdd_t *bdd;
    id = array_fetch(int, idArray, i);
    bdd = array_fetch(bdd_t*, bddArray, i);
    st_insert(table, (char*)(long)id, (char *)bdd);
  }
}


/**Function********************************************************************

  Synopsis    [Prints information about the schedule of clusters and
  the the corresponding smooth cubes.]

  Description [This function is used to print information about the
  cluster sequence and the sequence of smooth cubes. For each cluster
  in sequence, it prints the number of variables quantified and the
  number of variables introduced.]

  SideEffects []

******************************************************************************/
static void
PrintSmoothIntroducedCount(array_t *clusterArray, array_t
                           **arraySmoothVarBddArrayPtr,
                           array_t *psBddIdArray, array_t
                           *nsBddIdArray) 
{
    int i,j;
    st_table *nsBddIdTable, *supportTable, *psBddIdTable;
    bdd_t *trans;
    int introducedCount;
    bdd_t *tmp;
    st_generator *stgen;
    long varId;
    array_t *smoothVarBddArray, *arraySmoothVarBddArray;
    array_t *smoothVarBddIdArray = NIL(array_t);
    int psCount, piNdCount;
    
    if (arraySmoothVarBddArrayPtr == NIL(array_t*))
      arraySmoothVarBddArray = NIL(array_t);
    else
      arraySmoothVarBddArray = *arraySmoothVarBddArrayPtr;
      
    (void) fprintf(stdout, "**********************************************\n");
    nsBddIdTable = st_init_table(st_numcmp, st_numhash);
    for (i=0; i<array_n(nsBddIdArray); i++)
        st_insert(nsBddIdTable, (char *)(long) array_fetch(int, nsBddIdArray, i),
                  (char *) NULL);
    psBddIdTable = st_init_table(st_numcmp, st_numhash);
    for (i=0; i<array_n(psBddIdArray); i++)
        st_insert(psBddIdTable, (char *)(long) array_fetch(int, psBddIdArray, i),
                  (char *) NULL);
    
    for (i=0; i<=array_n(clusterArray)-1; i++){
        trans = array_fetch(bdd_t*, clusterArray, i);
        supportTable = ImgBddGetSupportIdTable(trans);
        psCount = 0;
        piNdCount = 0;
        if (arraySmoothVarBddArray != NIL(array_t)){
          smoothVarBddArray = array_fetch(array_t*,
                                          arraySmoothVarBddArray, i); 
          smoothVarBddIdArray = bdd_get_varids(smoothVarBddArray);
          for (j=0; j<array_n(smoothVarBddIdArray);j++){
            if (st_is_member(psBddIdTable,
                             (char *)
                             (long)array_fetch(int,
                                               smoothVarBddIdArray,
                                               j)))  
              psCount++;
            else
                piNdCount++;
          }
        }
        introducedCount = 0;
        st_foreach_item(nsBddIdTable, stgen, (char **)&varId, (char **)&tmp){
            if (st_is_member(supportTable, (char *)varId)){
              introducedCount++;
              st_delete(nsBddIdTable,(char **)&varId, NULL);
            }
        }
        (void) fprintf(stdout,"Tr no = %3d\tSmooth PS # = %3d\tSmooth PI # = %3d\tIntroduced # = %d\n", i,
                psCount, piNdCount, introducedCount);
        st_free_table(supportTable);
        array_free(smoothVarBddIdArray);
    }
    st_free_table(nsBddIdTable);
    st_free_table(psBddIdTable);
}


/**Function********************************************************************

  Synopsis    [Prints integers from an array.]

  Description [Prints integers from an array.]

  SideEffects []

******************************************************************************/
static void
PrintIntegerArray(array_t *idArray)
{
    int i;
    fprintf(stdout,"**************** printing bdd ids from the bdd array ***********\n");
    fprintf(stdout,"%d::\t", array_n(idArray));
    for (i=0;i<array_n(idArray); i++){
        fprintf(stdout," %d ", array_fetch(int, idArray, i));
    }
    fprintf(stdout,"\n******************\n");
    return;
}

/**Function********************************************************************

  Synopsis    [Prints Ids of an array of BDDs.]

  Description [Prints Ids of an array of BDDs.]

  SideEffects []

******************************************************************************/
static void
PrintBddIdFromBddArray(array_t *bddArray)
{
    int i;
    array_t *idArray = bdd_get_varids(bddArray);
    fprintf(stdout,"**************** printing bdd ids from the bdd array ***********\n");
    fprintf(stdout,"%d::\t", array_n(idArray));
    for (i=0;i<array_n(idArray); i++){
        fprintf(stdout," %d ", (int)array_fetch(int, idArray, i));
    }
    fprintf(stdout,"\n******************\n");
    array_free(idArray);
    return;
}

/**Function********************************************************************

  Synopsis    [Prints the integers from a symbol table.]

  Description [Prints the integers from a symbol table.]

  SideEffects []

******************************************************************************/
static void
PrintBddIdTable(st_table *idTable)
    
{
    st_generator *stgen;
    int varId;
    fprintf(stdout,"**************** printing bdd ids from the id table  ***********\n");
    fprintf(stdout,"%d::\t", st_count(idTable));
    st_foreach_item(idTable, stgen, (char **)&varId, (char**)NULL){
        fprintf(stdout," %d ", varId);
    }
    fprintf(stdout,"\n******************\n");
    return;
}

