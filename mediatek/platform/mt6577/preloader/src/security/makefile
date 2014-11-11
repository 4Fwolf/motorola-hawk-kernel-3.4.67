###################################################################
# flag
###################################################################

MOD_SRC  = 
MOD_OBJ  = $(MTK_ROOT_OUT)/PRELOADER_OBJ/obj

###################################################################
# source files
# - add your file here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
###################################################################

MOD_SRC  := \
sec.c \
sec_boot.c \
sec_ctrl.c \
sec_key.c \
sec_rom_info.c \
sec_secroimg.c \
sec_util.c \
sec_region.c \
sec_flashtool_cfg.c \

###################################################################
# target
###################################################################

include $(COMMON_FILE_MK)

###################################################################
# include sub dir
###################################################################

#//<20121009-14989-Eric Lin, [Hawk4.0][secu] Include the customer auth lib in the make file.
#ifeq ($(CUSTOM_SEC_AUTH_SUPPORT), yes)
SUBDIRS += $(MTK_ROOT_CUSTOM)/common/security/preloader
#else
#SUBDIRS += auth
#endif
#//>20121009-14989-Eric Lin

include $(COMMON_DIR_MK)
