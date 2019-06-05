;;;
;;; ext.el
;;; 
;;; Add bindings to the C mode keymap that insert various structured comment
;;; templates for the ext system
;;;
;;; This file was generated automatically from ext.m4 -- do not edit
;;;


(defun ext-insert-cfile ()
  "Insert a CFile template."
  (interactive)
  (save-excursion (insert "/**CFile***********************************************************************

  FileName    [required]

  PackageName [required]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [optional]

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

#include \"Int.h\" 

static char rcsid[] = \"$Id: $\";
USE(rcsid);


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


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



")))


(defun ext-insert-cheaderfile ()
  "Insert a CHeaderFile template."
  (interactive)
  (save-excursion (insert "/**CHeaderFile*****************************************************************

  FileName    [required]

  PackageName [required]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [optional]

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

  Revision    [$Id: $]

******************************************************************************/

#ifndef _
#define _

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
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

/**AutomaticEnd***************************************************************/

#endif /* _ */
")))


(defun ext-insert-enum ()
  "Insert a Enum template."
  (interactive)
  (save-excursion (insert "/**Enum************************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
")))


(defun ext-insert-function ()
  "Insert a Function template."
  (interactive)
  (save-excursion (insert "/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [optional] 	   

  CommandSynopsis    [optional]  

  CommandArguments   [optional]  

  CommandDescription [optional]  

******************************************************************************/
")))


(defun ext-insert-macro ()
  "Insert a Macro template."
  (interactive)
  (save-excursion (insert "/**Macro***********************************************************************

  Synopsis     [required]

  Description  [optional]

  SideEffects  [required]

  SeeAlso      [optional]

******************************************************************************/
#define 

")))


(defun ext-insert-struct ()
  "Insert a Struct template."
  (interactive)
  (save-excursion (insert "/**Struct**********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
")))


(defun ext-insert-variable ()
  "Insert a Variable template."
  (interactive)
  (save-excursion (insert "/**Variable********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
")))



(define-key c-mode-map "\M-1" 'ext-insert-cfile)
(define-key c-mode-map "\M-2" 'ext-insert-cheaderfile)
(define-key c-mode-map "\M-3" 'ext-insert-cheaderfile)
(define-key c-mode-map "\M-4" 'ext-insert-function)
(define-key c-mode-map "\M-5" 'ext-insert-macro)
(define-key c-mode-map "\M-6" 'ext-insert-struct)
(define-key c-mode-map "\M-7" 'ext-insert-enum)
(define-key c-mode-map "\M-8" 'ext-insert-variable)


;; Local Variables:
;; mode: lisp
;; End:
