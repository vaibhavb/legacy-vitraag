CSRC_cu += cuddAPI.c cuddAddAbs.c cuddAddApply.c cuddAddFind.c cuddAddIte.c \
        cuddAddInv.c cuddAddNeg.c cuddAddWalsh.c cuddAndAbs.c \
        cuddAnneal.c cuddApa.c cuddApprox.c cuddBddAbs.c cuddBddCorr.c \
	cuddBddIte.c cuddBridge.c cuddCache.c cuddCheck.c cuddClip.c \
	cuddCof.c cuddCompose.c cuddDecomp.c cuddEssent.c cuddExact.c \
	cuddExport.c cuddGenCof.c cuddGenetic.c \
        cuddGroup.c cuddHarwell.c cuddInit.c cuddInteract.c \
	cuddLCache.c cuddLevelQ.c \
        cuddLinear.c cuddLiteral.c cuddMatMult.c cuddPriority.c \
        cuddRead.c cuddRef.c cuddReorder.c cuddSat.c cuddSign.c \
        cuddSolve.c cuddSplit.c cuddSubsetHB.c cuddSubsetSP.c cuddSymmetry.c \
	cuddTable.c cuddUtil.c cuddWindow.c cuddZddCount.c cuddZddFuncs.c \
	cuddZddGroup.c cuddZddIsop.c cuddZddLin.c cuddZddMisc.c cuddZddPort.c \
	cuddZddReord.c cuddZddSetop.c cuddZddSymm.c cuddZddUtil.c 
HEADERS_cu += cudd.h cuddInt.h
MISC += testcudd.c r7x8.1.mat doc/cudd.ps doc/cuddAllAbs.html doc/cuddAllDet.html \
	doc/cuddExtAbs.html doc/cuddExtDet.html doc/cuddIntro.html \
	doc/footnode.html doc/img1.gif doc/img10.gif doc/img2.gif \
	doc/img3.gif doc/img4.gif doc/img5.gif doc/img6.gif doc/img7.gif \
	doc/img8.gif doc/img9.gif doc/index.html doc/node1.html \
	doc/node10.html doc/node11.html doc/node12.html \
	doc/node13.html doc/node14.html doc/node15.html \
	doc/node16.html doc/node17.html doc/node18.html \
	doc/node19.html doc/node2.html doc/node20.html doc/node21.html \
	doc/node22.html doc/node23.html doc/node24.html doc/node25.html \
	doc/node26.html doc/node27.html doc/node28.html doc/node29.html \
	doc/node3.html doc/node30.html doc/node31.html doc/node32.html \
	doc/node33.html doc/node4.html doc/node5.html doc/node6.html \
	doc/node7.html doc/node8.html doc/node9.html \
	doc/icons/change_begin.gif \
	doc/icons/change_delete.gif \
	doc/icons/change_end.gif \
	doc/icons/contents_motif.gif \
	doc/icons/cross_ref_motif.gif \
	doc/icons/foot_motif.gif \
	doc/icons/image.gif \
	doc/icons/index_motif.gif \
	doc/icons/next_group_motif.gif \
	doc/icons/next_group_motif_gr.gif \
	doc/icons/next_motif.gif \
	doc/icons/next_motif_gr.gif \
	doc/icons/previous_group_motif.gif \
	doc/icons/previous_group_motif_gr.gif \
	doc/icons/previous_motif.gif \
	doc/icons/previous_motif_gr.gif \
	doc/icons/up_motif.gif \
	doc/icons/up_motif_gr.gif

DEPENDENCYFILES = $(CSRC_cu)
