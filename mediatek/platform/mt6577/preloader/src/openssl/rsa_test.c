/* test vectors from p1ovect1.txt */


#include "include/e_os.h"
#include "include/crypto.h"
#include "include/rand.h"
#include "include/bn.h"
//#include "include/bget.h"
#include "include/kal_release.h"
#include "include/rsa.h"

#ifdef _WIN32_DEBUG_		
#define MSG printf
#endif
//===============================================================================

//#define _MTK_DEF_KEY_FEATURE_		1		//128
#define _SBC_KEY_FEATURE_		1			//256


//#define _DEV01_KEY_FEATURE_		1


#define _MTK_DEF_KEY_FEATURE_INT_	1
//#define _DEV01_KEY_FEATURE_INT_		1
//#define _SBC_KEY_FEATURE_INT_		1
//===============================================================================



#ifdef _MTK_DEF_KEY_FEATURE_
unsigned char MTK_DEF_IMGKEY_RSA_N[] = "5FFF0B70D5DE3FC5BF41CB824B4BFD14820571CE57EDD3E7C668CC570E718DB07DCC7A6CACD0E80DADC38AA33DB37816839D97980DF3E577A6E0B1169D708071E17DD259CFE538DBDA804A2FC07D795841F2F59DEE023A9919360D0A3F4647FDF5657D9FC5944C8BFA2802336BA23AFDCDE8D546E8806EB532AA7F95A01D8DD1";
unsigned char MTK_DEF_IMGKEY_RSA_D[] = "4BD992E9A2230CD2ABEF49E4F6A7E11D7E2ADD24847787B320239829C560D5EAB94B8304317C938E9358E94758AE60D9B13F2913DD1A749A9941FACAFAB574D70EBBFBCC0133A4BE2134CBA3CE7EE18A6D3CC98D33DAB06AEEE512F405A3248EA316ABC31A2758D4C5A7B9DFCC02C2508A492EF3760A0D4CDA827CFFCADD11ED";
unsigned char MTK_DEF_IMGKEY_RSA_E[] = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010001";

//unsigned char cust_key_store_n[] = "\x5F\xFF\x0B\x70\xD5\xDE\x3F\xC5\xBF\x41\xCB\x82\x4B\x4B\xFD\x14\x82\x05\x71\xCE\x57\xED\xD3\xE7\xC6\x68\xCC\x57\x0E\x71\x8D\xB0\x7D\xCC\x7A\x6C\xAC\xD0\xE8\x0D\xAD\xC3\x8A\xA3\x3D\xB3\x78\x16\x83\x9D\x97\x98\x0D\xF3\xE5\x77\xA6\xE0\xB1\x16\x9D\x70\x80\x71\xE1\x7D\xD2\x59\xCF\xE5\x38\xDB\xDA\x80\x4A\x2F\xC0\x7D\x79\x58\x41\xF2\xF5\x9D\xEE\x02\x3A\x99\x19\x36\x0D\x0A\x3F\x46\x47\xFD\xF5\x65\x7D\x9F\xC5\x94\x4C\x8B\xFA\x28\x02\x33\x6B\xA2\x3A\xFD\xCD\xE8\xD5\x46\xE8\x80\x6E\xB5\x32\xAA\x7F\x95\xA0\x1D\x8D\xD1";
//unsigned char cust_key_store_d[]= "\x4B\xD9\x92\xE9\xA2\x23\x0C\xD2\xAB\xEF\x49\xE4\xF6\xA7\xE1\x1D\x7E\x2A\xDD\x24\x84\x77\x87\xB3\x20\x23\x98\x29\xC5\x60\xD5\xEA\xB9\x4B\x83\x04\x31\x7C\x93\x8E\x93\x58\xE9\x47\x58\xAE\x60\xD9\xB1\x3F\x29\x13\xDD\x1A\x74\x9A\x99\x41\xFA\xCA\xFA\xB5\x74\xD7\x0E\xBB\xFB\xCC\x01\x33\xA4\xBE\x21\x34\xCB\xA3\xCE\x7E\xE1\x8A\x6D\x3C\xC9\x8D\x33\xDA\xB0\x6A\xEE\xE5\x12\xF4\x05\xA3\x24\x8E\xA3\x16\xAB\xC3\x1A\x27\x58\xD4\xC5\xA7\xB9\xDF\xCC\x02\xC2\x50\x8A\x49\x2E\xF3\x76\x0A\x0D\x4C\xDA\x82\x7C\xFF\xCA\xDD\x11\xED";
//unsigned char cust_key_store_e[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01";
#endif 

#ifdef _DEV01_KEY_FEATURE_
unsigned char MTK_DEF_IMGKEY_RSA_N[] = "BA9B3E946DCB73E0DBF08FDBD44828FB893DAEA0C430C3B2D49E993880648807320347CBEA596341BFDB133D2995DBAF3346DE505FF13F8CF7F677A67567B28B8277C5D267D3287176B186DF5EFF1B8BE9D5446F719FC464D88F779B5795752D2015E2A2109DBCF198E8F2E9BDB72F97AD0AC9DC3F4C4BE5B8AEA6678C713AE1";
unsigned char MTK_DEF_IMGKEY_RSA_D[] = "83D676CA2B2018C87EDB60E7F0FCE2678438DE0D9B4ED094A1FF43455600B0D2549558BE4ADDC8708579E407CC3FA22C0FD03BF51133C2B077E7DBF4F0F1C883EE09677F006701AA996B19B1D9DBCBF2FFA9BE08EE8DB337C28402854BE46D399C24B3A2370F2CEE5715ACE248B98ACC2257CFDE6622CF7269DFADFCA53AF001";
unsigned char MTK_DEF_IMGKEY_RSA_E[] = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010001";

//unsigned char cust_key_store_n[129] = "\xBA\x9B\x3E\x94\x6D\xCB\x73\xE0\xDB\xF0\x8F\xDB\xD4\x48\x28\xFB\x89\x3D\xAE\xA0\xC4\x30\xC3\xB2\xD4\x9E\x99\x38\x80\x64\x88\x07\x32\x03\x47\xCB\xEA\x59\x63\x41\xBF\xDB\x13\x3D\x29\x95\xDB\xAF\x33\x46\xDE\x50\x5F\xF1\x3F\x8C\xF7\xF6\x77\xA6\x75\x67\xB2\x8B\x82\x77\xC5\xD2\x67\xD3\x28\x71\x76\xB1\x86\xDF\x5E\xFF\x1B\x8B\xE9\xD5\x44\x6F\x71\x9F\xC4\x64\xD8\x8F\x77\x9B\x57\x95\x75\x2D\x20\x15\xE2\xA2\x10\x9D\xBC\xF1\x98\xE8\xF2\xE9\xBD\xB7\x2F\x97\xAD\x0A\xC9\xDC\x3F\x4C\x4B\xE5\xB8\xAE\xA6\x67\x8C\x71\x3A\xE1";
//unsigned char cust_key_store_d[129] = "\x83\xD6\x76\xCA\x2B\x20\x18\xC8\x7E\xDB\x60\xE7\xF0\xFC\xE2\x67\x84\x38\xDE\x0D\x9B\x4E\xD0\x94\xA1\xFF\x43\x45\x56\x00\xB0\xD2\x54\x95\x58\xBE\x4A\xDD\xC8\x70\x85\x79\xE4\x07\xCC\x3F\xA2\x2C\x0F\xD0\x3B\xF5\x11\x33\xC2\xB0\x77\xE7\xDB\xF4\xF0\xF1\xC8\x83\xEE\x09\x67\x7F\x00\x67\x01\xAA\x99\x6B\x19\xB1\xD9\xDB\xCB\xF2\xFF\xA9\xBE\x08\xEE\x8D\xB3\x37\xC2\x84\x02\x85\x4B\xE4\x6D\x39\x9C\x24\xB3\xA2\x37\x0F\x2C\xEE\x57\x15\xAC\xE2\x48\xB9\x8A\xCC\x22\x57\xCF\xDE\x66\x22\xCF\x72\x69\xDF\xAD\xFC\xA5\x3A\xF0\x01";
//unsigned char cust_key_store_e[129] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01";
#endif

#ifdef _SBC_KEY_FEATURE_
unsigned char SBC_IMGKEY_RSA_N[] =  "01C6B6A1DDF05E818E3DFE16101C5DF65939F352EAB8AACA91CB5BFEC15A1989DD7553343683BC30BB38E45F15BF17BCCAB16A41D695A4318F26504675FE83E92EE21C991C0FBA705395B4A34C331842D8A6F69846B58CC67306E3DE27B05666A6C4372E3FC0D92F314805EDD5B1CB7D25BF3CF9CA9C33C36D97B0B37DA8A44A7A1CA651679D8D680557740C7C1CA25D84BDD12136C2930432808F28265D1E33E667389E4806D865F3CC06329534F7A11861EB688545DCCCEF0B04E96735A08368FA31A1F3260073B31299B216192E620B8D1EA468925ADBD627C49EFC3623658F3CF8AD6D8556272E48FA7711E650287DA19196610F036B6C0D394E42C121D1";
unsigned char SBC_IMGKEY_RSA_D[] =  "00D57BCF5934CE1A035F4598B42DAD4BC8AE7577FB6D81FA232317E8EE7C4FBB33772EFE378DF7DFE5369BB9ACAB1008FA1CFBA737890012A883B372B15932335B689B46A32F1B383B75F0DE2E1B5B0B9F4E1C3E780C2AB0CD3671EB4E34F30BB4C630A60D168CA124810D0F91A1ACC8EFDCEE52D4762BB35813BCC93878E1D15B750561B78006B4C13A8F76B5F10C941E5776C21192357A9B9D7E02C0FC812D2B154671863DE97CE3ED07F90624A0CADD04079E145168A3558A64786820192F5B638354DA69520288B976296961C337FB18A90120F2B6B365C0E1A57CE4119AE8BC718E08FDA33F1F42AA1C91AED090EC6B5656A66C246F89FDE5FD41A76671";
unsigned char SBC_IMGKEY_RSA_E[] =  "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010001";
//[sbc-256]	
//unsigned char cust_key_store_e[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01";
//unsigned char cust_key_store_d[] = "\x00\xD5\x7B\xCF\x59\x34\xCE\x1A\x03\x5F\x45\x98\xB4\x2D\xAD\x4B\xC8\xAE\x75\x77\xFB\x6D\x81\xFA\x23\x23\x17\xE8\xEE\x7C\x4F\xBB\x33\x77\x2E\xFE\x37\x8D\xF7\xDF\xE5\x36\x9B\xB9\xAC\xAB\x10\x08\xFA\x1C\xFB\xA7\x37\x89\x00\x12\xA8\x83\xB3\x72\xB1\x59\x32\x33\x5B\x68\x9B\x46\xA3\x2F\x1B\x38\x3B\x75\xF0\xDE\x2E\x1B\x5B\x0B\x9F\x4E\x1C\x3E\x78\x0C\x2A\xB0\xCD\x36\x71\xEB\x4E\x34\xF3\x0B\xB4\xC6\x30\xA6\x0D\x16\x8C\xA1\x24\x81\x0D\x0F\x91\xA1\xAC\xC8\xEF\xDC\xEE\x52\xD4\x76\x2B\xB3\x58\x13\xBC\xC9\x38\x78\xE1\xD1\x5B\x75\x05\x61\xB7\x80\x06\xB4\xC1\x3A\x8F\x76\xB5\xF1\x0C\x94\x1E\x57\x76\xC2\x11\x92\x35\x7A\x9B\x9D\x7E\x02\xC0\xFC\x81\x2D\x2B\x15\x46\x71\x86\x3D\xE9\x7C\xE3\xED\x07\xF9\x06\x24\xA0\xCA\xDD\x04\x07\x9E\x14\x51\x68\xA3\x55\x8A\x64\x78\x68\x20\x19\x2F\x5B\x63\x83\x54\xDA\x69\x52\x02\x88\xB9\x76\x29\x69\x61\xC3\x37\xFB\x18\xA9\x01\x20\xF2\xB6\xB3\x65\xC0\xE1\xA5\x7C\xE4\x11\x9A\xE8\xBC\x71\x8E\x08\xFD\xA3\x3F\x1F\x42\xAA\x1C\x91\xAE\xD0\x90\xEC\x6B\x56\x56\xA6\x6C\x24\x6F\x89\xFD\xE5\xFD\x41\xA7\x66\x71";
//unsigned char cust_key_store_n[] = "\x01\xC6\xB6\xA1\xDD\xF0\x5E\x81\x8E\x3D\xFE\x16\x10\x1C\x5D\xF6\x59\x39\xF3\x52\xEA\xB8\xAA\xCA\x91\xCB\x5B\xFE\xC1\x5A\x19\x89\xDD\x75\x53\x34\x36\x83\xBC\x30\xBB\x38\xE4\x5F\x15\xBF\x17\xBC\xCA\xB1\x6A\x41\xD6\x95\xA4\x31\x8F\x26\x50\x46\x75\xFE\x83\xE9\x2E\xE2\x1C\x99\x1C\x0F\xBA\x70\x53\x95\xB4\xA3\x4C\x33\x18\x42\xD8\xA6\xF6\x98\x46\xB5\x8C\xC6\x73\x06\xE3\xDE\x27\xB0\x56\x66\xA6\xC4\x37\x2E\x3F\xC0\xD9\x2F\x31\x48\x05\xED\xD5\xB1\xCB\x7D\x25\xBF\x3C\xF9\xCA\x9C\x33\xC3\x6D\x97\xB0\xB3\x7D\xA8\xA4\x4A\x7A\x1C\xA6\x51\x67\x9D\x8D\x68\x05\x57\x74\x0C\x7C\x1C\xA2\x5D\x84\xBD\xD1\x21\x36\xC2\x93\x04\x32\x80\x8F\x28\x26\x5D\x1E\x33\xE6\x67\x38\x9E\x48\x06\xD8\x65\xF3\xCC\x06\x32\x95\x34\xF7\xA1\x18\x61\xEB\x68\x85\x45\xDC\xCC\xEF\x0B\x04\xE9\x67\x35\xA0\x83\x68\xFA\x31\xA1\xF3\x26\x00\x73\xB3\x12\x99\xB2\x16\x19\x2E\x62\x0B\x8D\x1E\xA4\x68\x92\x5A\xDB\xD6\x27\xC4\x9E\xFC\x36\x23\x65\x8F\x3C\xF8\xAD\x6D\x85\x56\x27\x2E\x48\xFA\x77\x11\xE6\x50\x28\x7D\xA1\x91\x96\x61\x0F\x03\x6B\x6C\x0D\x39\x4E\x42\xC1\x21\xD1";
#endif

unsigned char cust_key_store_256_e[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01";
unsigned char cust_key_store_256_d[256+1] = {0};
unsigned char cust_key_store_256_n[256+1] = {0};

unsigned char cust_key_store_128_n[128+1] = {0};
unsigned char cust_key_store_128_d[128+1] = {0};
unsigned char cust_key_store_128_e[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01";

#define MSG                            dbg_print
extern int RSA_eay_public_decrypt_ex(int flen, const unsigned char *from,unsigned char *to, RSA *rsa, int padding);

#ifdef _MTK_DEF_KEY_FEATURE_
int test_option=0;
#else
int test_option=1;
#endif

#define SetKey_256 \
  key->n = BN_bin2bn(n, sizeof(n)-1, key->n); \
  key->e = BN_bin2bn(e, sizeof(e)-1, key->e); \
  key->d = BN_bin2bn(d, sizeof(d)-1, key->d); \
  key->p = NULL;\
  key->q = NULL;\
  key->dmp1 = NULL;\
  key->dmq1 = NULL;\
  key->iqmp = NULL;\
  //memcpy(c, ctext_ex, sizeof(ctext_ex) - 1); \
  //memcpy(d_bak, d, 256); \
  return (sizeof(ctext_ex) - 1);

int key_mtk_def_img_128(RSA *key, unsigned char *c)
    {
	int chklen;

#if defined(_MTK_DEF_KEY_FEATURE_INT_)
//<20120126-2907-Eric Lin, [secu] Add the pub key settings.
#if defined(KEY_TYPE_MTK)
	static unsigned char n[] = "\x5F\xFF\x0B\x70\xD5\xDE\x3F\xC5\xBF\x41\xCB\x82\x4B\x4B\xFD\x14\x82\x05\x71\xCE\x57\xED\xD3\xE7\xC6\x68\xCC\x57\x0E\x71\x8D\xB0\x7D\xCC\x7A\x6C\xAC\xD0\xE8\x0D\xAD\xC3\x8A\xA3\x3D\xB3\x78\x16\x83\x9D\x97\x98\x0D\xF3\xE5\x77\xA6\xE0\xB1\x16\x9D\x70\x80\x71\xE1\x7D\xD2\x59\xCF\xE5\x38\xDB\xDA\x80\x4A\x2F\xC0\x7D\x79\x58\x41\xF2\xF5\x9D\xEE\x02\x3A\x99\x19\x36\x0D\x0A\x3F\x46\x47\xFD\xF5\x65\x7D\x9F\xC5\x94\x4C\x8B\xFA\x28\x02\x33\x6B\xA2\x3A\xFD\xCD\xE8\xD5\x46\xE8\x80\x6E\xB5\x32\xAA\x7F\x95\xA0\x1D\x8D\xD1";
	static unsigned char d[] = "\x4B\xD9\x92\xE9\xA2\x23\x0C\xD2\xAB\xEF\x49\xE4\xF6\xA7\xE1\x1D\x7E\x2A\xDD\x24\x84\x77\x87\xB3\x20\x23\x98\x29\xC5\x60\xD5\xEA\xB9\x4B\x83\x04\x31\x7C\x93\x8E\x93\x58\xE9\x47\x58\xAE\x60\xD9\xB1\x3F\x29\x13\xDD\x1A\x74\x9A\x99\x41\xFA\xCA\xFA\xB5\x74\xD7\x0E\xBB\xFB\xCC\x01\x33\xA4\xBE\x21\x34\xCB\xA3\xCE\x7E\xE1\x8A\x6D\x3C\xC9\x8D\x33\xDA\xB0\x6A\xEE\xE5\x12\xF4\x05\xA3\x24\x8E\xA3\x16\xAB\xC3\x1A\x27\x58\xD4\xC5\xA7\xB9\xDF\xCC\x02\xC2\x50\x8A\x49\x2E\xF3\x76\x0A\x0D\x4C\xDA\x82\x7C\xFF\xCA\xDD\x11\xED";
	static unsigned char e[] = "\x01\x00\x01";
	//#[PL][Error][Check] -KEY_TYPE_MTK
#elif defined(KEY_TYPE_DEV)	
	unsigned char n[] = "\xbd\x06\xc1\x59\x15\x21\xb3\xe1\x6e\xa6\x17\x73\x5d\xd5\xba\x81\xc0\xbf\xda\x67\xae\xd1\x7b\x6f\xba\xbc\xa4\x9f\xd3\xd8\x4e\xd9\x95\x80\x72\xb8\x9e\xe8\xd5\x6c\x6e\x16\x56\xdc\x39\xbd\x82\x47\x13\xa0\x5e\xec\x40\x82\xd7\xe7\xfc\xb6\x73\xd0\x28\x8a\xee\x15\xb6\x1e\x2f\x91\x47\xa0\xb6\x99\x10\xdd\x96\xac\x00\xcc\x1e\x14\xe7\x24\x03\x9e\x88\x13\x2c\x63\x1b\xf7\xe9\x1d\xea\x4b\x8e\xbb\x25\x65\xb2\x55\x8b\x3f\x71\xd9\x29\x2d\x25\x27\xfe\x18\xf0\x85\x20\x8d\x37\x4a\x5c\x1a\xe3\x27\x08\x71\x38\x46\x89\xa7\x4c\x3d";
	unsigned char d[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	unsigned char e[] = "\x01\x00\x01";
	//#[PL][Error][Check] -KEY_TYPE_DEV
#elif defined(KEY_TYPE_MP)	
	unsigned char n[] = "\x9d\xfc\x4b\x54\xa0\x3f\x05\x7b\xfb\x39\x7e\x39\x0e\xc1\x56\x3b\x7c\x90\xef\x56\x98\x63\xf5\x56\x7f\x76\x04\x55\xe4\xbe\x50\xa2\x1d\xb7\xba\x53\xb0\x74\xcd\xf9\x9c\xe4\x3c\x79\x14\xe7\xc6\x84\xbb\x21\x11\xbe\x5c\x57\xb8\x10\x6c\xf8\xce\xfd\xf1\x09\xea\xaf\xb2\xab\x8d\x24\x1b\xb0\xf1\xb0\xcc\x4a\xac\xe5\xe8\x76\x3b\xbb\x99\xc2\x88\xa6\x3d\x9d\x16\xe8\x80\xea\x1d\x3b\x21\x0e\x00\xea\xba\x00\x19\x92\x7d\x6f\x13\x8b\x13\xca\xe7\x47\x8e\x0e\xfa\x1d\xe8\x46\x86\x85\xd3\xfc\x0c\x4c\x2a\x38\x17\x79\x97\xe1\x67\x95";
	unsigned char d[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	unsigned char e[] = "\x01\x00\x01";
	//#[PL][Error][Check] -KEY_TYPE_MP
#endif
//>20120126-2907-Eric Lin
#endif

	static unsigned char p[64] ={0};
	static unsigned char q[64] ={0};
	static unsigned char dmp1[64] ={0};
	static unsigned char dmq1[64] ={0};
	static unsigned char iqmp[64] ={0};
    static unsigned char ctext_ex[256] ={0};

	//convert string to hex

	//set key
	//    SetKey_256;
#if defined(_MTK_DEF_KEY_FEATURE_INT_) 
	key->n = BN_bin2bn(n, sizeof(n)-1, key->n); 
	key->e = BN_bin2bn(e, sizeof(e)-1, key->e); 
	key->d = BN_bin2bn(d, sizeof(d)-1, key->d); 
#else
	//chklen= sizeof(cust_key_store_n)-1;
	//chklen= sizeof(cust_key_store_e)-1;
	//chklen= sizeof(cust_key_store_d)-1;

		key->n = BN_bin2bn(cust_key_store_128_n, sizeof(cust_key_store_128_n)-1, key->n); 
		key->e = BN_bin2bn(cust_key_store_128_e, sizeof(cust_key_store_128_e)-1, key->e); 
		key->d = BN_bin2bn(cust_key_store_128_d, sizeof(cust_key_store_128_d)-1, key->d); 



#endif
	key->p = NULL;
	key->q = NULL;
	key->dmp1 = NULL;
	key->dmq1 = NULL;
	key->iqmp = NULL;
//	memcpy(c, ctext_ex, sizeof(ctext_ex) - 1); 
    }

int key_mtk_def_img(RSA *key, unsigned char *c)
    {
	int chklen;

#if defined(_SBC_KEY_FEATURE_INT_)
	static unsigned char e[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01";
	static unsigned char d[] = "\x00\xD5\x7B\xCF\x59\x34\xCE\x1A\x03\x5F\x45\x98\xB4\x2D\xAD\x4B\xC8\xAE\x75\x77\xFB\x6D\x81\xFA\x23\x23\x17\xE8\xEE\x7C\x4F\xBB\x33\x77\x2E\xFE\x37\x8D\xF7\xDF\xE5\x36\x9B\xB9\xAC\xAB\x10\x08\xFA\x1C\xFB\xA7\x37\x89\x00\x12\xA8\x83\xB3\x72\xB1\x59\x32\x33\x5B\x68\x9B\x46\xA3\x2F\x1B\x38\x3B\x75\xF0\xDE\x2E\x1B\x5B\x0B\x9F\x4E\x1C\x3E\x78\x0C\x2A\xB0\xCD\x36\x71\xEB\x4E\x34\xF3\x0B\xB4\xC6\x30\xA6\x0D\x16\x8C\xA1\x24\x81\x0D\x0F\x91\xA1\xAC\xC8\xEF\xDC\xEE\x52\xD4\x76\x2B\xB3\x58\x13\xBC\xC9\x38\x78\xE1\xD1\x5B\x75\x05\x61\xB7\x80\x06\xB4\xC1\x3A\x8F\x76\xB5\xF1\x0C\x94\x1E\x57\x76\xC2\x11\x92\x35\x7A\x9B\x9D\x7E\x02\xC0\xFC\x81\x2D\x2B\x15\x46\x71\x86\x3D\xE9\x7C\xE3\xED\x07\xF9\x06\x24\xA0\xCA\xDD\x04\x07\x9E\x14\x51\x68\xA3\x55\x8A\x64\x78\x68\x20\x19\x2F\x5B\x63\x83\x54\xDA\x69\x52\x02\x88\xB9\x76\x29\x69\x61\xC3\x37\xFB\x18\xA9\x01\x20\xF2\xB6\xB3\x65\xC0\xE1\xA5\x7C\xE4\x11\x9A\xE8\xBC\x71\x8E\x08\xFD\xA3\x3F\x1F\x42\xAA\x1C\x91\xAE\xD0\x90\xEC\x6B\x56\x56\xA6\x6C\x24\x6F\x89\xFD\xE5\xFD\x41\xA7\x66\x71";
	static unsigned char n[] = "\x01\xC6\xB6\xA1\xDD\xF0\x5E\x81\x8E\x3D\xFE\x16\x10\x1C\x5D\xF6\x59\x39\xF3\x52\xEA\xB8\xAA\xCA\x91\xCB\x5B\xFE\xC1\x5A\x19\x89\xDD\x75\x53\x34\x36\x83\xBC\x30\xBB\x38\xE4\x5F\x15\xBF\x17\xBC\xCA\xB1\x6A\x41\xD6\x95\xA4\x31\x8F\x26\x50\x46\x75\xFE\x83\xE9\x2E\xE2\x1C\x99\x1C\x0F\xBA\x70\x53\x95\xB4\xA3\x4C\x33\x18\x42\xD8\xA6\xF6\x98\x46\xB5\x8C\xC6\x73\x06\xE3\xDE\x27\xB0\x56\x66\xA6\xC4\x37\x2E\x3F\xC0\xD9\x2F\x31\x48\x05\xED\xD5\xB1\xCB\x7D\x25\xBF\x3C\xF9\xCA\x9C\x33\xC3\x6D\x97\xB0\xB3\x7D\xA8\xA4\x4A\x7A\x1C\xA6\x51\x67\x9D\x8D\x68\x05\x57\x74\x0C\x7C\x1C\xA2\x5D\x84\xBD\xD1\x21\x36\xC2\x93\x04\x32\x80\x8F\x28\x26\x5D\x1E\x33\xE6\x67\x38\x9E\x48\x06\xD8\x65\xF3\xCC\x06\x32\x95\x34\xF7\xA1\x18\x61\xEB\x68\x85\x45\xDC\xCC\xEF\x0B\x04\xE9\x67\x35\xA0\x83\x68\xFA\x31\xA1\xF3\x26\x00\x73\xB3\x12\x99\xB2\x16\x19\x2E\x62\x0B\x8D\x1E\xA4\x68\x92\x5A\xDB\xD6\x27\xC4\x9E\xFC\x36\x23\x65\x8F\x3C\xF8\xAD\x6D\x85\x56\x27\x2E\x48\xFA\x77\x11\xE6\x50\x28\x7D\xA1\x91\x96\x61\x0F\x03\x6B\x6C\x0D\x39\x4E\x42\xC1\x21\xD1";
#endif

	static unsigned char p[64] ={0};
	static unsigned char q[64] ={0};
	static unsigned char dmp1[64] ={0};
	static unsigned char dmq1[64] ={0};
	static unsigned char iqmp[64] ={0};
    static unsigned char ctext_ex[256] ={0};

	//convert string to hex

	//set key
	//    SetKey_256;
#if defined(_SBC_KEY_FEATURE_INT_)
	key->n = BN_bin2bn(n, sizeof(n)-1, key->n); 
	key->e = BN_bin2bn(e, sizeof(e)-1, key->e); 
	key->d = BN_bin2bn(d, sizeof(d)-1, key->d); 
#else
	//chklen= sizeof(cust_key_store_n)-1;
	//chklen= sizeof(cust_key_store_e)-1;
	//chklen= sizeof(cust_key_store_d)-1;

		key->n = BN_bin2bn(cust_key_store_256_n, sizeof(cust_key_store_256_n)-1, key->n); 
		key->e = BN_bin2bn(cust_key_store_256_e, sizeof(cust_key_store_256_e)-1, key->e); 
		key->d = BN_bin2bn(cust_key_store_256_d, sizeof(cust_key_store_256_d)-1, key->d); 



#endif
	key->p = NULL;
	key->q = NULL;
	key->dmp1 = NULL;
	key->dmq1 = NULL;
	key->iqmp = NULL;
//	memcpy(c, ctext_ex, sizeof(ctext_ex) - 1); 
    }


//MTK SBC

static int key_dev_04_SBC(RSA *key, unsigned char *c)
    {
static unsigned char e[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01";
static unsigned char d[] = "\x00\xD5\x7B\xCF\x59\x34\xCE\x1A\x03\x5F\x45\x98\xB4\x2D\xAD\x4B\xC8\xAE\x75\x77\xFB\x6D\x81\xFA\x23\x23\x17\xE8\xEE\x7C\x4F\xBB\x33\x77\x2E\xFE\x37\x8D\xF7\xDF\xE5\x36\x9B\xB9\xAC\xAB\x10\x08\xFA\x1C\xFB\xA7\x37\x89\x00\x12\xA8\x83\xB3\x72\xB1\x59\x32\x33\x5B\x68\x9B\x46\xA3\x2F\x1B\x38\x3B\x75\xF0\xDE\x2E\x1B\x5B\x0B\x9F\x4E\x1C\x3E\x78\x0C\x2A\xB0\xCD\x36\x71\xEB\x4E\x34\xF3\x0B\xB4\xC6\x30\xA6\x0D\x16\x8C\xA1\x24\x81\x0D\x0F\x91\xA1\xAC\xC8\xEF\xDC\xEE\x52\xD4\x76\x2B\xB3\x58\x13\xBC\xC9\x38\x78\xE1\xD1\x5B\x75\x05\x61\xB7\x80\x06\xB4\xC1\x3A\x8F\x76\xB5\xF1\x0C\x94\x1E\x57\x76\xC2\x11\x92\x35\x7A\x9B\x9D\x7E\x02\xC0\xFC\x81\x2D\x2B\x15\x46\x71\x86\x3D\xE9\x7C\xE3\xED\x07\xF9\x06\x24\xA0\xCA\xDD\x04\x07\x9E\x14\x51\x68\xA3\x55\x8A\x64\x78\x68\x20\x19\x2F\x5B\x63\x83\x54\xDA\x69\x52\x02\x88\xB9\x76\x29\x69\x61\xC3\x37\xFB\x18\xA9\x01\x20\xF2\xB6\xB3\x65\xC0\xE1\xA5\x7C\xE4\x11\x9A\xE8\xBC\x71\x8E\x08\xFD\xA3\x3F\x1F\x42\xAA\x1C\x91\xAE\xD0\x90\xEC\x6B\x56\x56\xA6\x6C\x24\x6F\x89\xFD\xE5\xFD\x41\xA7\x66\x71";
static unsigned char n[] = "\x01\xC6\xB6\xA1\xDD\xF0\x5E\x81\x8E\x3D\xFE\x16\x10\x1C\x5D\xF6\x59\x39\xF3\x52\xEA\xB8\xAA\xCA\x91\xCB\x5B\xFE\xC1\x5A\x19\x89\xDD\x75\x53\x34\x36\x83\xBC\x30\xBB\x38\xE4\x5F\x15\xBF\x17\xBC\xCA\xB1\x6A\x41\xD6\x95\xA4\x31\x8F\x26\x50\x46\x75\xFE\x83\xE9\x2E\xE2\x1C\x99\x1C\x0F\xBA\x70\x53\x95\xB4\xA3\x4C\x33\x18\x42\xD8\xA6\xF6\x98\x46\xB5\x8C\xC6\x73\x06\xE3\xDE\x27\xB0\x56\x66\xA6\xC4\x37\x2E\x3F\xC0\xD9\x2F\x31\x48\x05\xED\xD5\xB1\xCB\x7D\x25\xBF\x3C\xF9\xCA\x9C\x33\xC3\x6D\x97\xB0\xB3\x7D\xA8\xA4\x4A\x7A\x1C\xA6\x51\x67\x9D\x8D\x68\x05\x57\x74\x0C\x7C\x1C\xA2\x5D\x84\xBD\xD1\x21\x36\xC2\x93\x04\x32\x80\x8F\x28\x26\x5D\x1E\x33\xE6\x67\x38\x9E\x48\x06\xD8\x65\xF3\xCC\x06\x32\x95\x34\xF7\xA1\x18\x61\xEB\x68\x85\x45\xDC\xCC\xEF\x0B\x04\xE9\x67\x35\xA0\x83\x68\xFA\x31\xA1\xF3\x26\x00\x73\xB3\x12\x99\xB2\x16\x19\x2E\x62\x0B\x8D\x1E\xA4\x68\x92\x5A\xDB\xD6\x27\xC4\x9E\xFC\x36\x23\x65\x8F\x3C\xF8\xAD\x6D\x85\x56\x27\x2E\x48\xFA\x77\x11\xE6\x50\x28\x7D\xA1\x91\x96\x61\x0F\x03\x6B\x6C\x0D\x39\x4E\x42\xC1\x21\xD1";

	static unsigned char p[64] ={0};
	static unsigned char q[64] ={0};
	static unsigned char dmp1[64] ={0};
	static unsigned char dmq1[64] ={0};
	static unsigned char iqmp[64] ={0};
    static unsigned char ctext_ex[256] ={0};
		SetKey_256;
    }


//Eric default 
int key_test_05_SBC(RSA *key, unsigned char *c)
    {
static unsigned char e[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01";
static unsigned char d[] = "\xb1\x12\xbc\x59\x91\xa7\x50\xd7\xaa\x13\x4b\x89\x8a\x61\x85\x15\x52\xad\xec\x3e\x50\xad\xef\xd2\xa8\x76\x22\x77\x3b\x88\xde\x26\x9d\xc3\x4d\x8c\xa8\x55\x3d\xee\x1f\x7c\x24\x31\x89\x39\x7a\x12\x64\x2d\x0b\xb4\x0f\xf8\x12\x76\xd9\x22\x44\x5a\x72\xe6\x77\xf7\x8d\x8c\x89\x1a\x52\x85\x74\x74\x3e\x1b\x49\xb0\xc0\xfd\x5c\x4a\x8f\x5b\xe0\x4a\x2d\xa0\xbd\x24\xaa\x72\x80\xf2\xf2\x99\xa2\xf8\x55\x58\x92\xd1\x02\x2e\xe5\x7b\xce\xe0\x63\x62\x51\x88\x69\x91\x10\xd1\x82\x8f\xf0\x08\xf0\xf7\x2f\x92\xb7\x86\x05\x19\x5d\x93\x5c\x29\x71\x72\x34\xb5\xbc\xfe\x5e\xfa\x78\xd7\xe2\x1b\xd3\xab\xc6\x9d\x56\xa3\x5e\x8d\xd5\xc1\x4b\x91\xbd\xe0\x12\x57\x8b\x70\xdb\x8a\x21\x06\x69\x8e\x96\x31\x45\xe3\x46\xe7\x92\x75\x2f\xa2\xa0\x65\x21\xbc\xe0\xee\x88\x37\x3e\x26\x2d\x9a\xe9\x22\xf7\xe2\x65\xf4\x7d\xcb\xbf\x3e\xc6\x9c\xf0\x60\xb9\xfd\xe1\xbb\x24\x49\xdd\x39\x53\xad\x53\x7e\xbd\x69\xdd\xb1\xa2\x87\xa6\x13\xa5\x77\xba\xc2\x3d\xdc\xbc\x5d\x45\x80\xdf\x28\x1a\x03\x50\xc8\xa3\xe6\x97\x2c\x6c\x95\xcc\xa1\x29\x88\x66\x62\xaf\xbb\x5a\x40\x61\xa1";
static unsigned char n[] = "\xc0\xf7\x90\x97\x9a\x14\x2d\xeb\x06\xc0\x49\x68\x6c\x5f\xcf\x54\xce\xac\x56\x1f\xeb\xe4\x55\x39\x48\xf8\x22\x13\xc9\x1f\x27\x7a\xcb\xb2\x40\x33\x72\x7e\xaf\x02\xf6\xe9\xb0\x4f\xad\x28\x8b\x06\xd5\x4e\x5c\xee\x47\x0b\xf1\x32\xe1\x3f\xd4\x1f\xb7\xf1\xd8\x76\x7f\xb6\x89\xf0\xf0\x15\x36\x28\x75\x85\x16\x90\xc4\xb0\xbf\x7c\xdf\x34\x03\x2d\x01\xc0\xe2\x24\xfd\x69\x9f\x1c\xdf\x3a\xb6\x58\x28\xe7\x3a\xf7\xbd\xe6\x61\x91\xa8\x19\x0a\x25\xa0\x23\xcd\xed\xe2\x7a\x2d\x1a\x4e\x2a\xcd\x5b\x7a\x0e\x11\xb3\xf3\x58\x9d\xe7\x3e\xbb\x6d\xf8\x5c\x8f\x41\x5c\xbd\x4b\xbc\x77\x2d\x13\xe4\x11\x0a\xd9\xb1\x72\xba\xaa\x9a\x97\xa6\x2c\x99\x07\x2f\x35\x7a\x59\x84\x16\xb8\xcb\x55\xa9\xeb\x6b\x4d\x1c\xbb\xa9\xbf\x8b\x7e\xf0\xc0\x5b\x07\xda\x55\x86\x5e\x4b\x3d\x8f\xc6\xae\x7d\x01\x6c\x6e\xef\xea\xd8\xf9\x8b\xfa\xb2\xfd\x5c\xb3\x1c\xe7\xce\xd3\xcc\x34\x6a\xf5\xa5\xd6\x41\x9d\x41\x3e\xf0\x0a\xe4\xa3\xf7\x11\x0a\xcb\xeb\x07\xb6\x69\xcc\xe8\x4b\xe8\xee\xfd\x6a\xf0\xf2\x79\xfb\xe4\xd6\xfd\x00\x85\x75\x7c\x3a\x90\x06\xd6\xc7\xfe\xcd\xf9\x43\xc3";

	static unsigned char p[64] ={0};
	static unsigned char q[64] ={0};
	static unsigned char dmp1[64] ={0};
	static unsigned char dmq1[64] ={0};
	static unsigned char iqmp[64] ={0};
    static unsigned char ctext_ex[256] ={0};
	    SetKey_256;
    }


	//ORG: src_addr1[]="465f09657f95d9fe4d66dda0f8abf6f2a7e4ede82fb91fdca9f46bcc6637cc15";	//SHA256
#ifdef _SBC_KEY_FEATURE_
const unsigned char RSA_private_encrypt_test_string[256]={
	0x00,0x43,0xcc,0x8c,0x3f,0x76,0x3b,0x11,0x46,0xd9,0x9b,0xdc,0x28,0xed,0xbe,0x96,
	0xab,0xaa,0xf0,0xd6,0x87,0x5a,0x47,0x93,0x74,0x98,0x9a,0x80,0x7f,0xe4,0x12,0x4d,
	0x38,0x3b,0x4f,0x25,0x58,0x57,0x6c,0x9c,0xf0,0x07,0xe3,0xce,0x24,0x5a,0x35,0xe3,
	0x18,0xfe,0x19,0xd6,0xe3,0x5c,0x31,0x1f,0x6b,0xfc,0x2f,0xcd,0x3e,0xa8,0x78,0x10,
	0x28,
	0x02,0xb9,0xc3,0x38,0xc4,0xcf,0x04,0xf5,0x02,0xfb,0xb3,0x6c,0xc9,0x04,0x55,0x0d,
	0xd3,0x8f,0x31,0x80,0xe5,0x0a,0x9c,0xd7,0x75,0x40,0xb6,0xba,0x8b,0xb7,0x19,0x83,
	0x70,0x10,0xe8,0x51,0x9f,0x09,0x42,0xc7,0xe6,0x8c,0xb5,0x94,0xc5,0x3b,0xe8,0xc4,
	0x4f,0x23,0xb9,0xbd,0x26,0x81,0xfe,0x9d,0x3e,0x8b,0xb0,0xb0,0xfe,0x5f,0x54,0x17,

	0xcd,0x99,0x1b,0xb3,0x9d,0x28,0x2f,0x97,0x5c,0x6f,0x2d,0xd3,0x17,0x98,0xdd,0xf7,
	0x97,0xd4,0xa8,0xbd,0x42,0x7e,0xe0,0xaa,0x51,0xe2,0x21,0xf3,0xbe,0xfb,0xa9,0xfa,
	0x58,0x8a,0x24,0x7b,0x45,0xfd,0xec,0x83,0x31,0x8b,0x11,0x86,0xaa,0x2f,0x61,0xd1,
	0x3c,0x7c,0x48,0x61,0xef,0x78,0xbb,0x5c,0xd4,0xdb,0x23,0x8d,0xcf,0x22,0xb4,0xb3,

	0x36,0xfd,0x54,0x82,0x5b,0x70,0x80,0x69,0xa8,0x51,0xc8,0x23,0xa9,0x4b,0xdc,0x62,
	0x9d,0x05,0x00,0xc9,0xda,0xa8,0x9a,0x60,0xd0,0xf1,0xe0,0x60,0x9e,0xf3,0x82,0x1a,
	0xc0,0xda,0x37,0x93,0x76,0x96,0x20,0xeb,0xa0,0x0a,0x7f,0x7b,0xb6,0x63,0xd7,0x9d,
	0x49,0x2a,0x62,0x51,0x66,0x95,0x1c,0x11,0xc1,0x28,0x17,0x12,0x07,0x25,0x33};
#endif
#ifdef _MTK_DEF_KEY_FEATURE_
	////ORG: src_addr1[]="465f09657f95d9fe4d66dda0f8abf6f2a7e4ede82fb91fdca9f46bcc6637cc15";	//SHA256
	/*
	const unsigned char RSA_private_encrypt_test_string_128[128]={
	0x0b,0xfc,0x17,0xcb,0x72,0xaf,0xa9,0xab,0xbd,0x8f,0x38,0x56,0xec,0x0e,0xa6,0x64,
	0xf8,0x2e,0xa7,0x21,0xc5,0xf5,0xb6,0xe0,0x09,0xaa,0x3c,0x17,0xb9,0x50,0x47,0x6a,
	0x71,0x27,0x31,0x06,0xc4,0xee,0xa2,0x93,0xb6,0xb1,0x34,0x26,0xd8,0x53,0x66,0x6f,
	0x8f,0x20,0xe4,0x18,0x74,0x0a,0x9d,0x5d,0xe1,0x6e,0xa9,0x8d,0x75,0xda,0x3c,0x58,
	0xd7,
	0x42,0xf1,0x98,0xd4,0x78,0xf1,0x38,0x7f,0x28,0x30,0x09,0xff,0x02,0x04,0xf3,0x97,
	0x5c,0xf0,0x1b,0x94,0x15,0x55,0x2e,0xbd,0x4d,0x49,0x6e,0x21,0xda,0x6e,0x42,0x11,
	0xb6,0xba,0x71,0xc8,0x9a,0xe7,0x65,0x84,0xe8,0x39,0xd4,0xb9,0x1e,0x0a,0x32,0x7d,
	0x1e,0xef,0x1c,0xd7,0xcb,0x6c,0xb1,0x79,0x1f,0x53,0x34,0x5a,0xfe,0x19,0x09};
	*/

	//SHA1(1.txt)= 44db8ad8a7cc6ebb19f0c56558aeb3f2b6b6675b
	const unsigned char RSA_private_encrypt_test_string_128[128]={
	0x1a,0xcc,0xb9,0x05,0x12,0xa0,0x93,0x2f,0x2e,0x74,0x31,0x86,0xe8,0xdc,0xba,0x66,
	0x49,0x2b,0x23,0x42,0x14,0xc8,0x3f,0x5e,0x1d,0xf2,0x68,0x52,0x51,0xf2,0x6a,0x6f,
	0xd5,0x23,0x9a,0xc7,0x4c,0x26,0xb4,0xd7,0x67,0xae,0xea,0x5f,0x8e,0x4a,0x86,0x46,
	0x9b,0x51,0xb1,0x89,0xe3,0xac,0x32,0xd8,0x74,0x0c,0xeb,0x96,0x3c,0x78,0x03,0x57,
	0xcd,
	0x4e,0x22,0xa1,0xa6,0xe8,0xec,0x90,0x26,0x8a,0x8d,0xa9,0xc0,0xcc,0x10,0x6e,0x8f,
	0xed,0x0e,0x9c,0xcd,0x03,0x60,0x25,0xb4,0x23,0xae,0xd8,0x38,0x4d,0x7e,0xcb,0xc1,
	0x0f,0x5e,0x5c,0xfd,0x14,0xbe,0x45,0x2a,0x90,0x93,0xe9,0x3d,0x51,0x8f,0x0e,0x49,
	0xd3,0xc0,0x68,0x27,0x05,0x3a,0xdc,0xf6,0x76,0x1a,0x57,0x06,0xe7,0x15,0x5f};
#endif


/*
administrator@ubuntu183:~/workspace/tools$ openssl dgst -sha256 1.txt
SHA256(1.txt)= 465f09657f95d9fe4d66dda0f8abf6f2a7e4ede82fb91fdca9f46bcc6637cc15

administrator@ubuntu183:~/workspace/tools$ openssl dgst -sha1 1.txt
SHA1(1.txt)= 44db8ad8a7cc6ebb19f0c56558aeb3f2b6b6675b
administrator@ubuntu183:~/workspace/tools$
*/
	char   		src_addrx2[]="abc";
	char   		src_addrx1[]="This is test.";
	char   		src_addr_sha256[]={ 0x46,0x5f,0x09,0x65,0x7f,0x95,0xd9,
							  0xfe,0x4d,0x66,0xdd,0xa0,0xf8,0xab,
							  0xf6,0xf2,0xa7,0xe4,0xed,0xe8,0x2f,
							  0xb9,0x1f,0xdc,0xa9,0xf4,0x6b,0xcc,
							  0x66,0x37,0xcc,0x15};	//SHA256

	char   		src_addr_sha128[]={ 0x44,0xdb,0x8a,0xd8,0xa7,0xcc,0x6e,0xbb,
									0x19,0xf0,0xc5,0x65,0x58,0xae,0xb3,0xf2,
									0xb6,0xb6,0x67,0x5b};	//SHA128

	char   		src_addrx[256]={0};
	char   		src_addr[256]={0};
extern int main_sha256(void);

int RSA_process(char *g_message,int msg_len,char *g_out,int *out_msg_len)
    {

    RSA *key;
    int err=0,v=2;    
    unsigned char ctext_ex[256];
	unsigned char ctext_out[256]={0};
    int plen,clen = 0,num,n;

#ifdef _WRITE_DATA_TO_FILE_
	FILE op_handle;
#endif

	// create bget buffer
    create_bget_buf_pool();

	{
		key = RSA_new();

		//***********************************************
		//***********RSA_PKCS1_PADDING*******************
		//***********************************************
#if 0
		//clen = key_test_05_SBC(key, ctext_ex);			
			clen = key_mtk_def_img(key, 0/*ctext_ex*/); //128
		num = RSA_private_encrypt(msg_len, g_message, g_out, key, RSA_PKCS1_PADDING);
		*out_msg_len = num;
		if ((num != 256) && (num != 128))
			{
			//dbg_print("PKCS#1 v1.5 encryption failed!\n");
			err=1;
			goto oaep;
			}

		MSG("\RSA_private_encrypt[%d]:\n",num);
		for(n=0;n<num;n++)
		{
			MSG("0x%02x,",(unsigned char)g_out[n]); 
			if((n>=64)&&(!(n%64)))
				MSG("\n");
		}	

#else
		
#ifdef _SBC_KEY_FEATURE_
		clen = key_dev_04_SBC(key, ctext_ex);			
		memcpy(g_out,RSA_private_encrypt_test_string,256);					
		*out_msg_len = num = 256;
#endif

#ifdef _MTK_DEF_KEY_FEATURE_
		//clen = key_mtk_def_img(key, ctext_ex);
		clen = key_mtk_def_img_128(key, 0/*ctext_ex*/); //128
		memcpy(g_out,RSA_private_encrypt_test_string_128,128);					
		*out_msg_len = num = 128;
#endif
	
#endif 
	
		//key->d = NULL;
		num = RSA_eay_public_decrypt_ex(num, (const unsigned char *)g_out,(unsigned char *)ctext_out, key,RSA_PKCS1_PADDING);
		//num = RSA_eay_public_decrypt_ex(128, (const unsigned char *)RSA_private_encrypt_test_string_128,(unsigned char *)ctext_out, key,RSA_PKCS1_PADDING);
		*out_msg_len = num;

#if 1//def _WIN32_DEBUG_		
		//if (num != msg_len || memcmp(g_message, ctext_out, num) != 0)
		if (memcmp(g_message, ctext_out, msg_len) != 0)
			{
			dbg_print("PKCS#1 v1.5 decryption failed!\n");
				err=-1;
			}
		else
		{
			dbg_print("PKCS #1 v1.5 encryption/decryption ok:%d\n",num);
			dbg_print("result: %s\n",ctext_out);
			for(v=0;v<num;v++)
			{
				dbg_print("0x%x,",(unsigned char)ctext_out[v]);	
			}
			err=0;
		}
#endif

oaep:
next:
		RSA_free(key);
	}
    return err;
}

//#endif //#if defined(SUSB_SIGN_BY_RSA_OPENSSL)

// Big_endian 0
// Little_endian 1
int checkCPU( )

{ 
    { 
           union w 
           {   
                  int  a; 
                  char b; 

           } c; 
           c.a = 1; 
           return(c.b ==1);
    } 
} 


int main_pl(void)
{

	int 		out_msg_len=0;
	char   		rsa_signning_buf[256];
	unsigned char		hash_buf[32];
	unsigned char		hash_buf_ver[32];
	
	memset(hash_buf,0,32);
	memset(hash_buf_ver,0,32);
	// Swithch 
#if 1		
	//if(!test_option)	
	//{
	//	memcpy(src_addr,src_addr_sha128,20);
	//	return RSA_process(( char*)src_addr,20,(char*)rsa_signning_buf,&out_msg_len);
	//}
	//else
	{
		memcpy(src_addr,src_addr_sha256,32);
		print("[arima secu] test enter.\r\n");
		return RSA_process(( char*)src_addr,32,(char*)rsa_signning_buf,&out_msg_len);
	}
#else
    MSG("\n=========================================\n");
    MSG("[Android SignTool]\n\n");
    MSG("Built at %s\n");    
    MSG("=========================================\n\n");    

#if defined(_SBC_KEY_FEATURE_) || defined(_SBC_KEY_FEATURE_INT)
		strcpy(src_addr,src_addr_sha256);
	//int sec_init_key (unsigned char *nKey, unsigned int nKey_len, 
	//	unsigned char *eKey, unsigned int eKey_len)
		sec_init_key_256(SBC_IMGKEY_RSA_N,strlen(SBC_IMGKEY_RSA_N),
					SBC_IMGKEY_RSA_E,strlen(SBC_IMGKEY_RSA_E));
	
	//int sec_hash(unsigned char *data_buf,  unsigned int data_len,
    //	unsigned char *hash_buf, unsigned int hash_len)
		sec_hash(src_addrx1,13/*strlen(src_addr)*/,hash_buf,32);

	// int sec_verify (unsigned char *data_buf,  unsigned int data_len, 
    //		unsigned char *sig_buf, unsigned int sig_len)
	
		sec_verify(src_addrx1,13,RSA_private_encrypt_test_string,256);

#endif

#ifdef _MTK_DEF_KEY_FEATURE_ // || defined(_MTK_DEF_KEY_FEATURE_INT_)
		strcpy(src_addr,src_addrx1);
		sec_init_key(MTK_DEF_IMGKEY_RSA_N,strlen(MTK_DEF_IMGKEY_RSA_N),
					MTK_DEF_IMGKEY_RSA_E,strlen(MTK_DEF_IMGKEY_RSA_E));

		sec_hash(src_addr,13,hash_buf,20);

		sec_verify(src_addr,13,RSA_private_encrypt_test_string_128,128);

 #endif 

#endif

}


