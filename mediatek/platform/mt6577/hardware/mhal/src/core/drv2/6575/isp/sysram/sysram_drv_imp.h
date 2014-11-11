/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifndef _SYSRAM_DRV_IMP_H_
#define _SYSRAM_DRV_IMP_H_

namespace NSIspSysram
{


/*******************************************************************************
*   Isp Sysram Driver Implementation.
*******************************************************************************/
class IspSysramDrvImp : public IspSysramDrv
                      , public UsrInfo
{
    friend class IspSysramDrv;

protected:  ////
    IspSysramDrvImp();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////
    virtual void    destroyInstance();

public:     ////    

    virtual MERROR_ENUM     init();
    virtual MERROR_ENUM     uninit();

    virtual MERROR_ENUM     alloc(
        EUser_T const eUsr, 
        MUINT32 const u4BytesToAllocate, 
        MUINT32&      ru4BytesAllocated
    );

    virtual MERROR_ENUM     alloc(
        EUser_T const eUsr, 
        MUINT32 const u4BytesToAllocate, 
        MUINT32&      ru4BytesAllocated,
        MUINT32 const TimeoutMS 
    );

    virtual MERROR_ENUM     free(
        EUser_T const eUsr
    );

    virtual MVOID*          getPhyAddr(EUser_T const eUsr);
    virtual MVOID*          getVirAddr(EUser_T const eUsr);

public:     ////    

    MVOID   dump()  const;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Link List Node.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    IspSysramDrvImp*    m_pDrvNext;
};


/*******************************************************************************
*
*******************************************************************************/
};  //  NSIspSysram
#endif // _SYSRAM_DRV_IMP_H_

