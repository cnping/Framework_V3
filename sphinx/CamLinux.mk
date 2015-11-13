LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libcutils libutils
	
LOCAL_SRC_FILES:= \
	src/sphinxbase/libsphinxbase/util/bio.c \
	src/sphinxbase/libsphinxbase/util/bitvec.c \
	src/sphinxbase/libsphinxbase/util/case.c \
	src/sphinxbase/libsphinxbase/util/ckd_alloc.c \
	src/sphinxbase/libsphinxbase/util/cmd_ln.c \
	src/sphinxbase/libsphinxbase/util/dtoa.c \
	src/sphinxbase/libsphinxbase/util/err.c \
	src/sphinxbase/libsphinxbase/util/errno.c \
	src/sphinxbase/libsphinxbase/util/f2c_lite.c \
	src/sphinxbase/libsphinxbase/util/filename.c \
	src/sphinxbase/libsphinxbase/util/genrand.c \
	src/sphinxbase/libsphinxbase/util/glist.c \
	src/sphinxbase/libsphinxbase/util/hash_table.c \
	src/sphinxbase/libsphinxbase/util/heap.c \
	src/sphinxbase/libsphinxbase/util/huff_code.c \
	src/sphinxbase/libsphinxbase/util/info.c \
	src/sphinxbase/libsphinxbase/util/listelem_alloc.c \
	src/sphinxbase/libsphinxbase/util/logmath.c \
	src/sphinxbase/libsphinxbase/util/matrix.c \
	src/sphinxbase/libsphinxbase/util/mmio.c \
	src/sphinxbase/libsphinxbase/util/pio.c \
	src/sphinxbase/libsphinxbase/util/profile.c \
	src/sphinxbase/libsphinxbase/util/sbthread.c \
	src/sphinxbase/libsphinxbase/util/strfuncs.c \
	src/sphinxbase/libsphinxbase/util/utf8.c \
	src/sphinxbase/libsphinxbase/fe/fe_interface.c \
	src/sphinxbase/libsphinxbase/fe/fe_sigproc.c \
	src/sphinxbase/libsphinxbase/fe/fe_warp_affine.c \
	src/sphinxbase/libsphinxbase/fe/fe_warp.c \
	src/sphinxbase/libsphinxbase/fe/fe_warp_inverse_linear.c \
	src/sphinxbase/libsphinxbase/fe/fe_warp_piecewise_linear.c \
	src/sphinxbase/libsphinxbase/fe/fixlog.c \
	src/sphinxbase/libsphinxbase/feat/agc.c \
	src/sphinxbase/libsphinxbase/feat/cmn.c \
	src/sphinxbase/libsphinxbase/feat/cmn_prior.c \
	src/sphinxbase/libsphinxbase/feat/feat.c \
	src/sphinxbase/libsphinxbase/feat/lda.c \
	src/sphinxbase/libsphinxbase/lm/fsg_model.c \
	src/sphinxbase/libsphinxbase/lm/jsgf.c \
	src/sphinxbase/libsphinxbase/lm/jsgf_parser.c \
	src/sphinxbase/libsphinxbase/lm/jsgf_scanner.c \
	src/sphinxbase/libsphinxbase/lm/lm3g_model.c \
	src/sphinxbase/libsphinxbase/lm/ngram_model_arpa.c \
	src/sphinxbase/libsphinxbase/lm/ngram_model_dmp.c \
	src/sphinxbase/libsphinxbase/lm/ngram_model_set.c \
	src/sphinxbase/libsphinxbase/lm/ngram_model.c \
	src/pocketsphinx/libpocketsphinx/acmod.c     \
	src/pocketsphinx/libpocketsphinx/bin_mdef.c    \
	src/pocketsphinx/libpocketsphinx/blkarray_list.c   \
	src/pocketsphinx/libpocketsphinx/dict.c     \
	src/pocketsphinx/libpocketsphinx/dict2pid.c    \
	src/pocketsphinx/libpocketsphinx/fsg_history.c   \
	src/pocketsphinx/libpocketsphinx/fsg_lextree.c   \
	src/pocketsphinx/libpocketsphinx/fsg_search.c   \
	src/pocketsphinx/libpocketsphinx/hmm.c     \
	src/pocketsphinx/libpocketsphinx/mdef.c     \
	src/pocketsphinx/libpocketsphinx/ms_gauden.c    \
	src/pocketsphinx/libpocketsphinx/ms_mgau.c    \
	src/pocketsphinx/libpocketsphinx/ms_senone.c    \
	src/pocketsphinx/libpocketsphinx/ngram_search.c   \
	src/pocketsphinx/libpocketsphinx/ngram_search_fwdtree.c \
	src/pocketsphinx/libpocketsphinx/ngram_search_fwdflat.c \
	src/pocketsphinx/libpocketsphinx/phone_loop_search.c  \
	src/pocketsphinx/libpocketsphinx/pocketsphinx.c \
	src/pocketsphinx/libpocketsphinx/ps_lattice.c   \
	src/pocketsphinx/libpocketsphinx/ps_mllr.c    \
	src/pocketsphinx/libpocketsphinx/ptm_mgau.c    \
	src/pocketsphinx/libpocketsphinx/s2_semi_mgau.c   \
	src/pocketsphinx/libpocketsphinx/tmat.c     \
	src/pocketsphinx/libpocketsphinx/vector.c \
	src/pcm.c \
	src/CdrSphinx.cpp \
	src/CdrTinyCap.cpp \
	
LOCAL_C_INCLUDES := \
	$(TOP)/frameworks/sphinx/include/sphinxbase/android \
	$(TOP)/frameworks/sphinx/include/sphinxbase/sphinxbase \
	$(TOP)/frameworks/sphinx/include/sphinxbase \
	$(TOP)/frameworks/sphinx/include/pocketsphinx \
	$(TOP)/frameworks/sphinx/include/tinyalsa \
	$(TOP)/frameworks/sphinx/include \

LOCAL_ARM_MODE := arm
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= libsphinx

include $(BUILD_SHARED_LIBRARY)