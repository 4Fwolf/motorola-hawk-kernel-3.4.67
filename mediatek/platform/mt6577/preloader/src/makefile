ifeq ($(CREATE_SEC_LIB),TRUE)
SUBDIRS += secure_lib
else
#//<20121009-14993-Eric Lin, [Hawk4.0][secu] Include OpenSSL related files into make file.
SUBDIRS += security core drivers init openssl
#//>20121009-14993-Eric Lin
endif

include $(COMMON_DIR_MK)

