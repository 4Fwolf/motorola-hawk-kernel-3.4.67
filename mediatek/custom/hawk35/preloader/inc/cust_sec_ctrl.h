#ifndef CUST_SEC_CTRL_H
#define CUST_SEC_CTRL_H

#include "typedefs.h"
#include "proj_cfg.h"
#include "cust_kpd.h"

/**************************************************************************
 * [ROM INFO]
 **************************************************************************/
//<20120927-14381-Eric Lin, [Hawk4.0][secu] To modify the PROJECT NAME to DEV settings.
/*
MOTDEVARIMA000005	DEV	DEV Hawk family
MOTPRDARIMA000030	PRD	Hawk 3.5: SLA & SBC & IMG/SML keys
MOTPRDARIMA000031	PRD	Hawk 4.0: SLA & SBC & IMG/SML keys
MOTPRDARIMA000032	PRD	Hawk 4.3: SLA & SBC & IMG/SML keys
MOTPRDARIMA000033	PRD	Hawk Prime: SLA & SBC & IMG/SML keys
*/
#if defined(KEY_TYPE_MTK) 
//#error #1
#define PROJECT_NAME                        "MOTDEVARIMA000005"
#elif defined(KEY_TYPE_DEV)
//#error #2
#define PROJECT_NAME                        "MOTDEVARIMA000005"
#elif defined(KEY_TYPE_MP) && defined(ARIMA_PROJECT_HAWK35)
//#error #3
#define PROJECT_NAME                        "MOTPRDARIMA000030"
#elif defined(KEY_TYPE_MP) && defined(ARIMA_PROJECT_HAWK40)
//#error #4
#define PROJECT_NAME                        "MOTPRDARIMA000031"
#else
#error "Unknow key type!"
#endif
//>20120927-14381-Eric Lin
#define PLATFORM_NAME                       "MT6575"

/**************************************************************************
 * [SEC ENV CONTROL]
 **************************************************************************/
#define SEC_ENV_ENABLE                      TRUE

/**************************************************************************
 * [CRYPTO SEED]
 **************************************************************************/
#define CUSTOM_CRYPTO_SEED_SIZE             (16)
#define CUSTOM_CRYPTO_SEED                  "1A52A367CB12C458"

/**************************************************************************
 * [SML AES KEY CONTROL]
 **************************************************************************/
/* It can be enabled only if SUSBDL is turned on */
/* Please make sure SUSBDL is on before enabling this flag */
//#define SML_AES_KEY_ANTICLONE_EN

/**************************************************************************
 * [S-USBDL]
 **************************************************************************/
/* S-USBDL Attribute */
#define ATTR_SUSBDL_DISABLE                 0x00
#define ATTR_SUSBDL_ENABLE                  0x11
#define ATTR_SUSBDL_ONLY_ENABLE_ON_SCHIP    0x22
/* S-USBDL Control */
#define SEC_USBDL_CFG                       CUSTOM_SUSBDL_CFG

/**************************************************************************
 * [FLASHTOOL SECURE CONFIG]
 **************************************************************************/
//<20121030-15946-Eric Lin, [secu] Add PROD_INFO into bypass check list.
//<20120927-14396-Eric Lin, [Hawk4.0][secu] To add the BYPASS CHECK image list files.
#define FLASHTOOL_SEC_CFG
#define BYPASS_CHECK_IMAGE_0_NAME           "PRO_INFO"
#define BYPASS_CHECK_IMAGE_0_OFFSET         0xa60000
#define BYPASS_CHECK_IMAGE_0_LENGTH         0x300000
#define BYPASS_CHECK_IMAGE_1_NAME           "FLEX"
#define BYPASS_CHECK_IMAGE_1_OFFSET         0x26e4000
#define BYPASS_CHECK_IMAGE_1_LENGTH         0x10000000
#define BYPASS_CHECK_IMAGE_2_NAME           ""
#define BYPASS_CHECK_IMAGE_2_OFFSET         0x0
#define BYPASS_CHECK_IMAGE_2_LENGTH         0x0

/**************************************************************************
 * [FLASHTOOL FORBIT DOWNLOAD CONFIG (for NSLA mode only)] , 32 bits
 * It's not recommended to use 32 bits (v3) mode now. Please use FLASHTOOL_FORBID_DL_NSLA_CFG_64 instead
 **************************************************************************/
#define FLASHTOOL_FORBID_DL_NSLA_CFG
#define FORBID_DL_IMAGE_0_NAME              "PRO_INFO"
#define FORBID_DL_IMAGE_0_OFFSET            0xa60000
#define FORBID_DL_IMAGE_0_LENGTH            0x300000
#define FORBID_DL_IMAGE_1_NAME              ""
#define FORBID_DL_IMAGE_1_OFFSET            0x0
#define FORBID_DL_IMAGE_1_LENGTH            0x0

/**************************************************************************
 * [FLASHTOOL FORBIT DOWNLOAD CONFIG (for NSLA mode only)], 64 bits for v4 sign format
 **************************************************************************/
//#define FLASHTOOL_FORBID_DL_NSLA_CFG_64
#ifdef FLASHTOOL_FORBID_DL_NSLA_CFG_64
#define FORBID_DL_IMAGE_0_NAME              ""
#define FORBID_DL_IMAGE_0_OFFSET            0x0
#define FORBID_DL_IMAGE_1_NAME              ""
#define FORBID_DL_IMAGE_1_OFFSET            0x0
#endif

#define SEC_USBDL_WITHOUT_SLA_ENABLE

#ifdef SEC_USBDL_WITHOUT_SLA_ENABLE
//<2012/10/02 yaotsulin-14537, Change USB configuration VID/PID for RSD request
#define USBDL_DETECT_VIA_KEY
//>2012/10/02 yaotsulin-14537
/* if com port wait key is enabled, define the key*/
#ifdef USBDL_DETECT_VIA_KEY
#define COM_WAIT_KEY    KPD_DL_KEY3
#endif
#define USBDL_DETECT_VIA_AT_COMMAND
#endif
//>20121002-14530-Eric Lin
//>20121030-15946-Eric Lin

/**************************************************************************
 * [S-BOOT]
 **************************************************************************/
/* S-BOOT Attribute */
#define ATTR_SBOOT_DISABLE                  0x00
#define ATTR_SBOOT_ENABLE                   0x11
#define ATTR_SBOOT_ONLY_ENABLE_ON_SCHIP     0x22
/* S-BOOT Control */
#define SEC_BOOT_CFG                        CUSTOM_SBOOT_CFG

//<20120927-14395-Eric Lin, [Hawk4.0][secu] Turn on the S-BOOT related check.
/* Note : these attributes only work when S-BOOT is enabled */
#define VERIFY_PART_UBOOT                   (TRUE)
#define VERIFY_PART_LOGO                    (TRUE)
#define VERIFY_PART_BOOTIMG                 (TRUE)
#define VERIFY_PART_RECOVERY                (TRUE)
#define VERIFY_PART_ANDSYSIMG               (TRUE)
#define VERIFY_PART_SECSTATIC               (TRUE)
//>20120927-14395-Eric Lin

/**************************************************************************
 * [DEFINITION CHECK]
 **************************************************************************/
#ifdef SML_AES_KEY_ANTICLONE_EN
#ifndef SECRO_IMG_ANTICLONE_EN
#error "SML_AES_KEY_ANTICLONE_EN is defined. Should also enable SECRO_IMG_ANTICLONE_EN"
#endif
#endif

#if SEC_USBDL_CFG
#if !SEC_ENV_ENABLE
#error "SEC_USBDL_CFG is NOT disabled. Should set SEC_ENV_ENABLE as TRUE"
#endif
#endif

#if SEC_BOOT_CFG
#if !SEC_ENV_ENABLE
#error "SEC_BOOT_CFG is NOT disabled. Should set SEC_ENV_ENABLE as TRUE"
#endif
#endif

#ifdef SEC_USBDL_WITHOUT_SLA_ENABLE
#if !SEC_ENV_ENABLE
#error "SEC_USBDL_WITHOUT_SLA_ENABLE is NOT disabled. Should set SEC_ENV_ENABLE as TRUE"
#endif
#endif

#ifdef USBDL_DETECT_VIA_KEY
#ifndef SEC_USBDL_WITHOUT_SLA_ENABLE
#error "USBDL_DETECT_VIA_KEY can only be enabled when SEC_USBDL_WITHOUT_SLA_ENABLE is enabled"
#endif
#ifndef COM_WAIT_KEY
#error "COM_WAIT_KEY is not defined!!"
#endif
#endif

#ifdef USBDL_DETECT_VIA_AT_COMMAND
#ifndef SEC_USBDL_WITHOUT_SLA_ENABLE
#error "USBDL_DETECT_VIA_AT_COMMAND can only be enabled when SEC_USBDL_WITHOUT_SLA_ENABLE is enabled"
#endif
#endif

#endif   /* CUST_SEC_CTRL_H */
