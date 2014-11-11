/* MTK Proprietary Customization File */

#include "sec_platform.h"
#include "kal_release.h"
#include "sec_auth.h"
#include "sha1.h"
#include "sha2.h"
#include "rsa.h"
#include "sec_key.h"
#include "KEY_IMAGE_AUTH.h"
#include "sec_error.h"

#define RSA2048_KEY_LENGTH          (256)
#define RSA1024_KEY_LENGTH          (128)
#define SHA256_LENGTH_IN_BYTE       (32)
#define SHA1_LENGTH_IN_BYTE         (20)

//#error 1

// ***************************************************************
// 			defination start
// ***************************************************************
#if defined(KEY_TYPE_DEV)
//<20121105-16269-Eric Lin, [secu] Reduce PL code size for FB functions.
//#define RSA_DUMP_DETAILS_MSG	1			//remove due to FB patch.
//>20121105-15969-Eric Lin
//#define DECRYPTED_256_UNIT_TEST	1
#endif
//#define _RSA_UNITTEST_FOR_REDUCE_CODESIZE	1

// ***************************************************************
// 			defination end
// ***************************************************************


#define SHA1_HASH_LEN       (20)
#define SHA256_HASH_LEN   (32)
#define RSA1024_SIG_LEN     (128)
#define RSA2048_SIG_LEN     (256)

#define CUST_RETURN_TRUE	0
#define MSG                            dbg_print



#define CUST_RSA_KEY_STORE_SIZE			128
#define CUST_RSA_KEY_STORE_SIZE_256		256

__attribute__((section(".mem.region"))) static unsigned char rsa_key_store_e[CUST_RSA_KEY_STORE_SIZE] ;
__attribute__((section(".mem.region"))) static unsigned char rsa_key_store_d[CUST_RSA_KEY_STORE_SIZE] ;
__attribute__((section(".mem.region"))) static unsigned char rsa_key_store_n[CUST_RSA_KEY_STORE_SIZE] ;
__attribute__((section(".mem.region"))) static unsigned char rsa_key_store_256_e[CUST_RSA_KEY_STORE_SIZE_256+1] ;
__attribute__((section(".mem.region"))) static unsigned char rsa_key_store_256_d[CUST_RSA_KEY_STORE_SIZE_256+1] ;
__attribute__((section(".mem.region"))) static unsigned char rsa_key_store_256_n[CUST_RSA_KEY_STORE_SIZE_256+1] ;
__attribute__((section(".mem.region"))) static SHA1Context  cust_sha;                /* SHA-1 context                 */
__attribute__((section(".mem.region"))) static unsigned char cust_out[256];

static RSA			*custom_test_key;
extern int RSA_eay_public_decrypt_ex(int flen, const unsigned char *from,unsigned char *to, RSA *rsa, int padding);
extern RSA_PUBK g_SBC_PUBK;

#ifdef DECRYPTED_256_UNIT_TEST 

const unsigned char NSLA_PADDING[256]={
	0xc2,0x06,0xb7,0x1b,0x64,0x20,0xd7,0x04,0x20,0xc4,0x1e,0x0f,0xf9,0x23,0x44,0xc8,0x1b,0x71,0xe5,0xa3,0x14,
	0x48,0x8d,0x4c,0xb7,0x70,0x60,0x37,0x11,0x8f,0xca,0x3d,0x11,0xf8,0x5b,0xf6,0x2a,0x6f,0xa2,0xce,0xda,
	0x48,0x20,0x67,0x97,0xa6,0xbe,0x88,0x55,0xd0,0x1c,0xc7,0xd8,0xf1,0xa4,0xfb,0xad,0x47,0x0e,0x4b,0x5d,
	0x6c,0x6b,0x4d,0xe0,0xf4,0xb0,0xfb,0x88,0xdd,0x47,0xfd,0x58,0xcf,0xe4,0x3e,0xbf,0x3c,0x57,0xa7,0x56,
	0x46,0x0c,0xc8,0xb7,0x3b,0x5f,0xb4,0x2f,0x86,0xbb,0x9e,0xde,0x90,0x7b,0x38,0x44,0x84,0x9a,0x7e,0xb3,
	0xcb,0x1b,0xee,0x4d,0xeb,0x7d,0xf5,0x50,0xaa,0x09,0x80,0x91,0xa9,0xa0,0x27,0xdb,0xc4,0x74,0x64,0xdd,
	0x93,0xcb,0x75,0xf2,0xea,0xbc,0xf7,0xb9,0xa3,0xd7,0xe3,0x47,0x09,0x27,0x18,0xf9,0x24,0x59,0xb4,0x1b,
	0xdb,0x52,0x09,0x91,0x57,0x67,0xb7,0xc7,0xd3,0x48,0xe1,0x48,0xd0,0xd3,0xc6,0x5a,0x80,0x46,0x6c,0xfd,
	0xe9,0xa6,0x53,0x7f,0x27,0x76,0x03,0xbc,0x32,0x76,0x28,0xae,0xd1,0x69,0x1e,0xba,0x09,0x7e,0x05,0xeb,
	0xb9,0x32,0x71,0x3b,0xa2,0xec,0xd5,0x47,0x61,0x88,0x9d,0xea,0x45,0x90,0xb1,0x03,0xc4,0xa8,0x88,0x34,
	0x6b,0x79,0xc8,0x9b,0x99,0x00,0xd7,0xfd,0x99,0xec,0xa9,0x83,0x44,0x1d,0x9f,0x98,0x1c,0x49,0xb7,0x3b,
	0x8e,0x4c,0xc2,0xce,0xe7,0xfb,0xa7,0xc7,0xfb,0x3f,0x4b,0x4e,0x27,0x5c,0xa4,0xa6,0x39,0x90,0x48,0x35,
	0x63,0x58,0x02,0x37,0xe4,0xa8,0xd6,0xa8,0x57,0x0d,0x1f,0xe6,0xb3,0x72,0xb5};
#endif

void do_sha1_proc(unsigned char* data_buf,  unsigned int data_len)
{
	char        c;                  /* Character read from file      */
	unsigned int i,x;
	/*
	 *  Reset the SHA-1 context and process input
	 */
	SHA1Reset(&cust_sha);
	for(i=0;i<data_len;i++)
		SHA1Input(&cust_sha, &data_buf[i], 1);
	x = SHA1Result(&cust_sha);
	
#ifdef RSA_DUMP_DETAILS_MSG_ADV	
	if (!x)
	{
		MSG("sha: could not compute message digest.\n");
	}
	else
	{
		MSG( "%08X %08X %08X %08X %08X" ,
				cust_sha.Message_Digest[0],
				cust_sha.Message_Digest[1],
				cust_sha.Message_Digest[2],
				cust_sha.Message_Digest[3],
				cust_sha.Message_Digest[4]);
	}
	MSG( "\n");
#endif	
}

// only support 2 digit
int cust_string2hex (unsigned char *instring)
{
	unsigned char *p;
	int i=0,result=0;
	const unsigned char baseTable[]={16,1};

	p = instring;
	for(i=0;i<2;i++)
	{
		if(*p >='0' && *p <='9')
			result |= (*p-'0')*baseTable[i];
		else if(*p >='A' && *p <='Z')
			result |= (*p-'A'+10)*baseTable[i]; 
		else if(*p >='a' && *p <='z')
			result |= (*p-'a'+10)*baseTable[i];
		p++;
	}

	return result;
}


/**************************************************************************
 *  AUTH KEY INIT
 **************************************************************************/
static int set_rsa_key_pair(RSA *key, unsigned char *c)
{
	
	#ifdef RSA_DUMP_DETAILS_MSG_ADV
	#if defined(KEY_TYPE_DEV)
	int i;
	unsigned char n[] = "\xbd\x06\xc1\x59\x15\x21\xb3\xe1\x6e\xa6\x17\x73\x5d\xd5\xba\x81\xc0\xbf\xda\x67\xae\xd1\x7b\x6f\xba\xbc\xa4\x9f\xd3\xd8\x4e\xd9\x95\x80\x72\xb8\x9e\xe8\xd5\x6c\x6e\x16\x56\xdc\x39\xbd\x82\x47\x13\xa0\x5e\xec\x40\x82\xd7\xe7\xfc\xb6\x73\xd0\x28\x8a\xee\x15\xb6\x1e\x2f\x91\x47\xa0\xb6\x99\x10\xdd\x96\xac\x00\xcc\x1e\x14\xe7\x24\x03\x9e\x88\x13\x2c\x63\x1b\xf7\xe9\x1d\xea\x4b\x8e\xbb\x25\x65\xb2\x55\x8b\x3f\x71\xd9\x29\x2d\x25\x27\xfe\x18\xf0\x85\x20\x8d\x37\x4a\x5c\x1a\xe3\x27\x08\x71\x38\x46\x89\xa7\x4c\x3d";
	MSG("[SECU_PL] 128 Check KEY  pair:\n");
	for(i=0;i<128;i++)
	{
		if(n[i] != rsa_key_store_n[i])
		{	
			MSG("[SECU_PL]128 index:%d ,right:%x wrong:%x\n",i,n[i],rsa_key_store_n[i]);		
		}
	}	
	#else
	#Error "[SECU_PL] Please check the pub key vaild in PL."
	#endif	
	#endif
	
	key->n = BN_bin2bn(rsa_key_store_n, CUST_RSA_KEY_STORE_SIZE /*sizeof(rsa_key_store_n)-1*/ , key->n); 
	key->e = BN_bin2bn(rsa_key_store_e, CUST_RSA_KEY_STORE_SIZE /*sizeof(rsa_key_store_e)-1*/, key->e); 
	key->d = BN_bin2bn(rsa_key_store_d, CUST_RSA_KEY_STORE_SIZE /*sizeof(rsa_key_store_d)-1*/, key->d); 

	key->p = NULL;
	key->q = NULL;
	key->dmp1 = NULL;
	key->dmq1 = NULL;
	key->iqmp = NULL;
	return 0;
}
static int set_rsa_key_pair_256(RSA *key, unsigned char *c)
{
#if 0
	int i;
	#ifdef RSA_DUMP_DETAILS_MSG
	static unsigned char e[] = "\x01\x00\x01";
	static unsigned char d[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	static unsigned char n[] = "\xBC\xE0\xDD\x10\x74\xFC\x94\xAA\x8A\xB1\xBB\xCE\x0D\x3F\x47\xD8\x4E\x41\x3B\x50\x95\x9A\xCB\xD3\x57\x4A\xFD\xDA\xA0\x7D\xE9\xC2\x3E\x75\x08\xCB\xF5\xBD\x3B\xF3\xCB\x6B\xE4\x06\x9F\xFB\x32\x89\x38\x5C\x7B\x3E\x63\x38\x53\xD6\xFA\x0D\xE3\x45\x02\x44\xAE\xDA\xCB\x19\x3B\x17\xEF\x67\x08\x43\x7B\x1C\x9D\x78\x6C\x81\xE5\x0B\x69\x01\x06\x1D\x53\xA7\xDF\x29\x4A\xC1\x05\x95\x07\x62\x64\x0D\x5B\x3C\xE0\x69\x91\xCA\x8E\x4A\xEE\x98\x52\x81\xEA\xE2\x90\x14\xCF\xCE\xB3\x89\x6B\x62\x35\x63\x28\x45\x05\x1E\xDC\x99\xCD\xAE\x32\xAD\x60\xCA\xE7\x48\x62\x08\xE5\x96\xA0\x2B\xE0\x75\x31\x67\xCE\x82\x3D\xB2\x5F\x21\xD3\x16\x6D\x8B\x19\xF9\x6F\xAC\x7D\x78\xC3\x7A\x24\x44\xB9\x80\x15\x2B\x0C\xAC\xBB\x84\x63\x59\xDD\x50\x55\xAB\x7F\x0E\xBA\x16\x0F\xAA\x92\xE9\xED\x6C\xAD\xA3\xCA\xDC\xA7\x3D\x47\x1F\x48\xAC\xA4\x75\x2A\xA9\x86\x4B\x93\xE7\x55\xBD\x8F\xB5\x23\x4F\x0A\xC6\x12\x4D\x6E\xEE\x54\xFC\x7A\xBD\x6A\x8A\xC9\x57\x5E\x51\x7A\x58\xA6\x50\x9D\x86\x59\xAB\xB3\x04\xF5\xE9\x95\x26\x32\xD3\x2C\xA2\x13\x71\x0D\x9F\x7A\xCA\x04\x65\x4E\x25";
	
	MSG("[SECU_PL]size of e :%d\n", sizeof(e));
	MSG("[SECU_PL]size of d :%d\n", sizeof(d));
	MSG("[SECU_PL]size of n :%d\n", sizeof(n));	
	#endif
	
	#if 0
	key->n = BN_bin2bn(n, sizeof(n)-1, key->n); 
	key->e = BN_bin2bn(e, sizeof(e)-1, key->e); 
	key->d = BN_bin2bn(d, sizeof(d)-1, key->d); 
	#else
	key->n = BN_bin2bn(rsa_key_store_256_n, CUST_RSA_KEY_STORE_SIZE_256 /*sizeof(rsa_key_store_n)-1*/ , key->n); 
	key->e = BN_bin2bn(rsa_key_store_256_e, CUST_RSA_KEY_STORE_SIZE_256 /*sizeof(rsa_key_store_e)-1*/, key->e); 
	key->d = BN_bin2bn(rsa_key_store_256_d, CUST_RSA_KEY_STORE_SIZE_256 /*sizeof(rsa_key_store_d)-1*/, key->d); 
	#endif
	
	#ifdef RSA_DUMP_DETAILS_MSG
	MSG("[SECU_PL]size of rsa_key_store_256_e :%d\n", sizeof(rsa_key_store_256_e));
	MSG("[SECU_PL]size of rsa_key_store_256_n :%d\n", sizeof(rsa_key_store_256_n));
	MSG("[SECU_PL]size of rsa_key_store_256_d :%d\n", sizeof(rsa_key_store_256_d));	
	
	MSG("[SECU_PL] 256 Check KEY  pair:\n");
	for(i=0;i<256;i++)
	{
		if(n[i] != rsa_key_store_256_n[i])
		{	
			MSG("[SECU_PL]index:%d ,right:%x wrong:%x\n",i,n[i],rsa_key_store_256_n[i]);		
		}
	}	
	#endif
#else
	key->n = BN_bin2bn(rsa_key_store_256_n, CUST_RSA_KEY_STORE_SIZE_256 /*sizeof(rsa_key_store_n)-1*/ , key->n); 
	key->e = BN_bin2bn(rsa_key_store_256_e, CUST_RSA_KEY_STORE_SIZE_256 /*sizeof(rsa_key_store_e)-1*/, key->e); 
	key->d = BN_bin2bn(rsa_key_store_256_d, CUST_RSA_KEY_STORE_SIZE_256 /*sizeof(rsa_key_store_d)-1*/, key->d); 
#endif

	key->p = NULL;
	key->q = NULL;
	key->dmp1 = NULL;
	key->dmq1 = NULL;
	key->iqmp = NULL;
	return 0;
}
/***************************************************************************  
HASH (should support SHA1 and SHA256) 
**************************************************************************/
int sec_hash (U8* data_buf, U32 data_len, U8* hash_buf, U32 hash_len)
{
#ifndef _RSA_UNITTEST_FOR_REDUCE_CODESIZE
	int i,countNumber,shalen,remind_num;
	unsigned int realsize;

	#ifdef RSA_DUMP_DETAILS_MSG
	MSG("[SECU_PL]cust_hash -enter\n");
	#endif
	memset(hash_buf, 0x00, hash_len);
		
	if(SHA1_HASH_LEN == hash_len)
	{
	
	/* =============== */
	/* SHA1            */
	/* =============== */

	/* TODO : use sec_hash to generate hash value */    
	/* customer needs to customized this function */
		#ifdef RSA_DUMP_DETAILS_MSG_ADV
		MSG("[SECU_PL]sha1_custom_test [%d]-hash input:\n",data_len);
		for(i=0;i<16;i++)
			MSG("%x",data_buf[i]);
		MSG(" \n");
		#endif

		do_sha1_proc(data_buf,data_len);
		for(i = 0; i < 5 ; i++)
		{
			hash_buf[4*i+0] = (unsigned char)(cust_sha.Message_Digest[i]>>24);
			hash_buf[4*i+1] = (unsigned char)(cust_sha.Message_Digest[i]>>16);
			hash_buf[4*i+2] = (unsigned char)(cust_sha.Message_Digest[i]>>8);
			hash_buf[4*i+3] = (unsigned char)(cust_sha.Message_Digest[i]);
		}

		#ifdef RSA_DUMP_DETAILS_MSG_ADV	
		MSG("[SECU_PL]sha1_custom_test-hash output[%d]:\n",SHA1_HASH_LEN);
		for(i=0;i<20;i++)
			MSG("%x",(unsigned char)hash_buf[i]);
		MSG(" \n");
		#endif
		return CUST_RETURN_TRUE;

	}
	else if(SHA256_HASH_LEN == hash_len)
	{
	
	/* =============== */
	/* SHA256          */
	/* =============== */
	/* TODO : use sec_hash to generate hash value */    
	/* customer needs to customized this function */
		#ifdef RSA_DUMP_DETAILS_MSG_ADV
		MSG("[SECU_PL]sha256_custom_test [%d]-hash input:\n",data_len);
		for(i=0;i<16;i++)
			MSG("%x",data_buf[i]);
		MSG(" \n");
		#endif

		sha256((const unsigned char *) data_buf, data_len, hash_buf);

		#ifdef RSA_DUMP_DETAILS_MSG_ADV
		MSG("[SECU_PL]sha256_custom_test [%d]-hash output:\n",SHA256_HASH_LEN);
		for(i=0;i<SHA256_HASH_LEN;i++)
			MSG("%x",(unsigned char)hash_buf[i]);
		MSG(" \n");
		#endif
		return CUST_RETURN_TRUE;
	}
	else
	{
		#ifdef RSA_DUMP_DETAILS_MSG_ADV
		MSG("[SECU_CCCI]cust_hash other case return -1\n");  
		#endif
        ASSERT(0);
		return -1;
	}
#else
	return CUST_RETURN_TRUE;
#endif //#ifndef _RSA_UNITTEST_FOR_REDUCE_CODESIZE
}

/****************************
*********************************************
*
 *  RSA (should support RSA1024 and RSA2048)
 *****************************
*********************************************/
int sec_auth (U8* data_buf, U32 data_len, U8* sig_buf, U32 sig_len)
{
#ifndef _RSA_UNITTEST_FOR_REDUCE_CODESIZE
	int num,i,result;
	unsigned char verfy_buf[32];
	unsigned char swap_n=0;

	MSG("[SECU_PL]sec_auth sig buf[%d]:\n",sig_len);
	
	#ifdef RSA_DUMP_DETAILS_MSG_ADV
	for(i=0;i<sig_len;i++)
	{
		MSG(",0x%x",(unsigned char)sig_buf[i]);
		if(!(i%16)&&(i>=16))
			MSG("\n");
	}
	MSG(" \n");
	MSG("[SECU_PL]sec_auth data buf[%d]:\n",data_len);
	for(i=0;i<16;i++)
	{
		MSG("%x",(unsigned char)data_buf[i]);
	}
	MSG(" \n");
	#endif

	if(RSA1024_SIG_LEN == sig_len)
	{
		U8 sha1sum[SHA1_HASH_LEN] = {0};    
		/* =============== */
		/* RSA1024         */
		/* =============== */        

		/* SHA1 */        
		if( sec_hash(data_buf, data_len, sha1sum, SHA1_HASH_LEN) != 0 )
		{
			/*return verify failed */
			return -1;
		}    
		#ifdef RSA_DUMP_DETAILS_MSG
		MSG("[SECU_PL]RSA_eay_public_decrypt_ex todo\n");
		#endif
		num = RSA_eay_public_decrypt_ex(sig_len, (const unsigned char *)sig_buf,(unsigned char *)verfy_buf, custom_test_key,RSA_PKCS1_PADDING);
		//RSA_free(custom_test_key);
		#ifdef RSA_DUMP_DETAILS_MSG
		MSG("[SECU_PL]RSA_eay_public_decrypt_ex buf[%d]:\n",num );
		for(i=0;i<20;i++)
		{
			MSG(",0x%x",(unsigned char)verfy_buf[i]);
		}
		MSG(" \n");
		#endif
		if (memcmp(sha1sum, (unsigned char*)verfy_buf, 20) != 0)
		{

			MSG("[SECU_PL]cust_verify fail:%d\n",num);
			//<20120206-2929-Eric Lin, [secu] BYPASS related check in DEV key.			
			#if 0//defined(KEY_TYPE_DEV)
			MSG("[SECU_PL][WARN]1024 KEY_TYPE_DEV BYPASS AUTH.\n");
			return CUST_RETURN_TRUE;
			//>20120206-2929-Eric Lin
			#endif
			return -1;			
		}
		else
		{
			MSG("[SECU_PL]cust_verify pass\n");
			return CUST_RETURN_TRUE;				
		}		
	/* TODO : use sec_verify to verify data buffer with RSA1024 */    
	/* customer needs to customized this function */
	}
	else if(RSA2048_SIG_LEN == sig_len)
	{
		U8 sha256sum[SHA256_HASH_LEN] = {0};

		/* =============== */
		/* RSA2048         */
		/* =============== */        

		/* SHA256 */      
		if( sec_hash(data_buf, data_len, sha256sum, SHA256_HASH_LEN) != 0 )
		{
			/* return verify failed */
			return -1;
		}        

		#ifdef DECRYPTED_256_UNIT_TEST

		if (memcmp(NSLA_PADDING, (unsigned char*)sig_buf, 256) != 0)
		{
			MSG("[SECU_PL]padding fail\n");
		}
		else
		{
			MSG("[SECU_PL]padding pass\n");
		}
			
		//memcpy(cust_out,NSLA_PADDING,256);		
		memcpy(cust_out,sig_buf,256);	
		#else
		//MOTO SPEC: swap half-word
		memcpy(cust_out,sig_buf,256);	
		#endif
		
		//MOTO SPEC: swap half-word
		for(i=0;i<128;i++)
		{
			swap_n = cust_out[(i*2)+1];		
			cust_out[(i*2)+1] = cust_out[(i*2)+0];
			cust_out[(i*2)+0] = swap_n;		
		}		
		
		/* TODO : use sec_hash to generate hash value */    
		/* customer needs to customized this function */
		num = RSA_eay_public_decrypt_ex(sig_len 
						,(const unsigned char *)cust_out,(unsigned char *)verfy_buf
						, custom_test_key
						,RSA_PKCS1_PADDING);
		//RSA_free(custom_test_key);
		if (memcmp(sha256sum, (unsigned char*)verfy_buf, 32) != 0)
		{

			MSG("[SECU_PL]cust_verify fail:%d\n",num);
			//<20120206-2929-Eric Lin, [secu] BYPASS related check in DEV key.			
//<2012/12/19-18990-EricLin, [secu] Enable the DA auth in DEV5 key.
			#if  defined(KEY_TYPE_MTK)
			MSG("[SECU_PL][WARN]2048 KEY_TYPE_DEV BYPASS AUTH.\n");
			return CUST_RETURN_TRUE;
			#endif			
			//>20120206-2929-Eric Lin
//>2012/12/19-18990-EricLin
			return -1;			
		}
		else
		{
			MSG("[SECU_PL]cust_verify pass\n");
			return CUST_RETURN_TRUE;
		}		

	}
	else
	{
		ASSERT(0);
	}
#else
	return CUST_RETURN_TRUE;
#endif //#ifndef _RSA_UNITTEST_FOR_REDUCE_CODESIZE
}



 /*
	typedef struct RSA_PUBK
	{
	    unsigned short                      E_Key[256>>1];
	    unsigned short   		        N_Key[256>>1];
	} RSA_PUBK;
*/
/**************************************************************************
 *  DA AUTH INIT
 **************************************************************************/
U32 da_auth_init (void)
{
#ifndef _RSA_UNITTEST_FOR_REDUCE_CODESIZE

	unsigned char	need2cov[2];
	int		i=0;


	#ifdef RSA_DUMP_DETAILS_MSG_ADV
	MSG("\n[SECU_PL][DA]da_auth_init key N ,in string:\n");
	for(i=0;i<128;i++)
	{
		MSG("0x%x,0x%x,",(unsigned char)(g_SBC_PUBK.N_Key[i]>>8),(unsigned char)(g_SBC_PUBK.N_Key[i])); 
		if(!(i%16)&&(i>=16))
			MSG("\n");
	}	
	MSG("\n");	
	#endif

	memset(rsa_key_store_256_n,0,CUST_RSA_KEY_STORE_SIZE_256+1);
	memset(rsa_key_store_256_d,0,CUST_RSA_KEY_STORE_SIZE_256+1);
	memset(rsa_key_store_256_e,0,CUST_RSA_KEY_STORE_SIZE_256+1);

	//convert string to hex -N
	for(i=0;i<128;i++)
	{
		rsa_key_store_256_n[(i<<1)+0] = (unsigned char)(g_SBC_PUBK.N_Key[i]>>8);
		rsa_key_store_256_n[(i<<1)+1] = (unsigned char)(g_SBC_PUBK.N_Key[i]);
	}

	#ifdef RSA_DUMP_DETAILS_MSG
	MSG("\n[SECU_PL][DA]da_auth_init key N ,mem string:\n");
	for(i=0;i<256;i++)
	{
		MSG("0x%x,",rsa_key_store_256_n[i]); 
		if(!(i%16)&&(i>=16))
			MSG("\n");
	}	
	MSG("\n");	
	#endif
	
	//Set E value =0x010001
	rsa_key_store_256_e[CUST_RSA_KEY_STORE_SIZE_256-3]=0x01;
	rsa_key_store_256_e[CUST_RSA_KEY_STORE_SIZE_256-2]=0x00;
	rsa_key_store_256_e[CUST_RSA_KEY_STORE_SIZE_256-1]=0x01;
	
	create_bget_buf_pool();
	custom_test_key = RSA_new();
	set_rsa_key_pair_256(custom_test_key, 0);
	 

	MSG("[SECU_PL]da_auth_init done.\n");    	
	
	/* customer needs to customized this function */
	return SEC_OK;
#else
	MSG("[SECU_PL]da_auth_init done.\n");    	
	return SEC_OK;
#endif //#ifndef _RSA_UNITTEST_FOR_REDUCE_CODESIZE
}

/**************************************************************************
 *  IMAGE AUTH INIT  include
 marco:
	 IMG_CUSTOM_RSA_N
	 IMG_CUSTOM_RSA_E

 **************************************************************************/
U32 img_auth_init (void)
{       
#if 1//ndef _RSA_UNITTEST_FOR_REDUCE_CODESIZE
    /* TODO : the key will pass in, need to judge if the key is for RSA1024 or RSA2048 and save it for later use */    
    /*customer needs to customized this function */

	unsigned char	need2cov[2];
	int				i=0;
	memset(rsa_key_store_n,0,CUST_RSA_KEY_STORE_SIZE);
	memset(rsa_key_store_d,0,CUST_RSA_KEY_STORE_SIZE);
	memset(rsa_key_store_e,0,CUST_RSA_KEY_STORE_SIZE);

	#ifdef RSA_DUMP_DETAILS_MSG_ADV
	MSG("\n[SECU_PL][DA]img_auth_init key N ,in string:\n");
	for(i=0;i<256;i++)
	{
		MSG("%x",IMG_CUSTOM_RSA_N[i]); 
		if(!(i%16)&&(i>=16))
			MSG("\n");
	}	
	MSG("\n");	
	#endif
	for(i=0;i<128;i++)
	{
		need2cov[0] = IMG_CUSTOM_RSA_N[(i<<1)+0];
		need2cov[1] = IMG_CUSTOM_RSA_N[(i<<1)+1];
		need2cov[2] = 0;
		rsa_key_store_n[i] = cust_string2hex(need2cov);
	}

	#ifdef RSA_DUMP_DETAILS_MSG
	MSG("\n[SECU_PL][DA]img_auth_init key N ,mem string:\n");
	for(i=0;i<128;i++)
	{
		MSG("%x",rsa_key_store_n[i]); 
		if(!(i%16)&&(i>=16))
			MSG("\n");
	}	
	MSG("\n");	
	#endif
	
	//Set E value =0x010001
	rsa_key_store_e[CUST_RSA_KEY_STORE_SIZE-3]=0x01;
	rsa_key_store_e[CUST_RSA_KEY_STORE_SIZE-2]=0x00;
	rsa_key_store_e[CUST_RSA_KEY_STORE_SIZE-1]=0x01;
	
	create_bget_buf_pool();
	custom_test_key = RSA_new();
	set_rsa_key_pair(custom_test_key, 0);	 
	MSG("[SECU_PL]img_auth_init done.\n");    	

    return SEC_OK;   
#else
	return SEC_OK;
#endif //#ifndef _RSA_UNITTEST_FOR_REDUCE_CODESIZE
}

//>20120126-2878-Eric Lin
