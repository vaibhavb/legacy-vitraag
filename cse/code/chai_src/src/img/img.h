/**CHeaderFile*****************************************************************

  FileName    [img.h]

  PackageName [img]

  Synopsis    [Methods for performing image computations.]

  Description [The image package is used to compute the image (forward or
  backward) of a set under a vector of functions.  The functions are given by
  a graph of multi-valued functions (MVFs). This graph is built using the
  partition package. Each vertex in this graph has an MVF and an MDD id.  The
  fanins of a vertex v give those vertices upon which the MVF at v depends.
  The vector of functions to use for an image computation, the "roots", is
  specified by an array of (names of) vertices of the graph.  The domain
  variables are the variables over which "from" sets are defined for forward
  images, and "to" sets are defined for backward images.  The range variables
  are the variables over which "to" sets are defined for forward images, and
  "from" sets are defined for backward images.  The quantify variables are
  additional variables over which the functions are defined; this set is
  disjoint from domain variables.  These variables are existentially
  quantified from the results of backward image computation. <p>
  
  Computing images is fundamental to many symbolic analysis techniques, and
  methods for computing images efficiently is an area of ongoing research.
  For this reason, the image package has been designed with lots of
  flexibility to easily allow new methods to be integrated (to add a new
  method, see the instructions in imgInt.h).  Applications that use the image
  package can switch among different image methods simply by specifying the
  method type in the image initialization routine.  By using the returned
  structure (Img_ImageInfo_t) from the initialization routine, all subsequent
  (forward or backward) image computations will be done using the specified
  method.<p>
  
  VIS users can control which image method is used by appropriately setting
  the "image_method" flag.  Also, VIS users can set flags to control
  parameters for different image computation methods.  Because the user has
  the ability to change the values of these flags, Img_ImageInfo_t structs
  should be freed and re-initialized whenever the VIS user changes the value
  of these flags.<p>
  
  Following are descriptions of the methods implemented. In the descriptions,
  x=x_1,...x_n is the set of domain variables, u=u_1,...,u_k is the set of
  quantify variables, y=y_1,...,y_m is the set of range variables, and
  f=f_1(x,u),...,f_m(x,u) is the set of functions under which we wish to
  compute images.<p>
  
  <b>Monolithic:</b> This is the most naive approach possible.  A single
  relation T(x,y) is constructed during the initialization phase, using the
  computation (exists u (prod_i(y_i = f_i(x,u)))). To compute the forward
  image, where fromUpperBound=U(x), fromLowerBound=L(x), and toCareSet=C(y),
  we first compute a set A(x) between U(x) and L(x).  Then, T(x,y) is
  simplified with respect to A(x) and C(y) to get T*.  Finally, x is
  quantified from T* to produce the final answer.  Backward images are
  computed analogously.  The monolithic method does not recognize any
  user-settable flags for image computation.<p>

  <b>IWLS95:</b> This technique is based on the early quantification heuristic.
  The initialization process consists of following steps:   
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
  early quantification schedule. <p>]

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

  Revision    [$Id: img.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _IMG
#define _IMG

#include  "mdd.h"
#include  "main.h"
#include  "mdl.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Enum************************************************************************

  Synopsis    [Methods for image computation.  See the description in img.h.]

******************************************************************************/
typedef enum {
  Img_Monolithic_c,
  Img_Domain_c,
  Img_Iwls95_c,
  Img_Disjunctive_c,
  Img_Default_c
} Img_MethodType;


/**Enum************************************************************************

  Synopsis    [Type to indicate the direction of image computation.]

  Description [Type to indicate the direction of image computation.  When an
  imageInfo structure is initialized, the application must specify what
  "directions" of image computation will be performed: forward, backward, or
  both.]

******************************************************************************/
typedef enum {
  Img_Forward_c,
  Img_Backward_c,
  Img_Both_c
} Img_DirectionType;



/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct ImgImageInfoStruct Img_ImageInfo_t;


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

EXTERN mdd_t* Img_MultiwayLinearAndSmooth(mdd_manager *mddManager, array_t *relationArray, array_t *smoothVarMddIdArray, array_t *introducedVarMddIdArray);
EXTERN int Img_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Img_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Img_End();
EXTERN Img_ImageInfo_t * Img_ImageInfoInitializeMocha(mdd_manager *mddManager, array_t *domainVars, array_t *rangeVars, array_t *quantifyVars, unsigned int methodType, Img_DirectionType directionType, void *methodInfo);
EXTERN Img_ImageInfo_t * Img_ImageInfoInitialize(Img_ImageInfo_t *imageInfo, mdd_manager *mddManager, array_t * domainVars, array_t * rangeVars, array_t * quantifyVars, Img_MethodType methodType, void *methodInfo, Img_DirectionType directionType);
EXTERN mdd_t * Img_ImageInfoComputeFwdWithDomainVars(Img_ImageInfo_t * imageInfo, mdd_t * fromLowerBound, mdd_t * fromUpperBound, mdd_t * toCareSet);
EXTERN mdd_t * Img_ImageInfoComputeFwd(Img_ImageInfo_t * imageInfo, mdd_t * fromLowerBound, mdd_t * fromUpperBound, mdd_t * toCareSet);
EXTERN mdd_t * Img_ImageInfoComputeBwdWithDomainVars(Img_ImageInfo_t * imageInfo, mdd_t * fromLowerBound, mdd_t * fromUpperBound, mdd_t * toCareSet);
EXTERN mdd_t * Img_ImageInfoComputeBwd(Img_ImageInfo_t * imageInfo, mdd_t * fromLowerBound, mdd_t * fromUpperBound, mdd_t * toCareSet);
EXTERN void Img_ImageInfoFree(Img_ImageInfo_t * imageInfo);
EXTERN char * Img_ImageInfoObtainMethodTypeAsString(Img_ImageInfo_t * imageInfo);
EXTERN void Img_ImageInfoPrintMethodParams(Img_ImageInfo_t *imageInfo, FILE *fp);
EXTERN mdd_manager * Img_ImageInfoReadMddManager(Img_ImageInfo_t *imageInfo);

/**AutomaticEnd***************************************************************/

#endif /* _IMG */


