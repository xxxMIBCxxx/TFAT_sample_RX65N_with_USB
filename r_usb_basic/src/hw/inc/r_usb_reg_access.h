/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_reg_access.h
 * Description  : USB IP Register control code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 04.01.2014 1.00 First Release
 *         : 30.01.2015 1.01    Added RX71M.
 ***********************************************************************************************************************/
#ifndef __HW_USB_REG_ACCESS_H__
    #define __HW_USB_REG_ACCESS_H__

/************/
/*  SYSCFG  */
/************/
uint16_t HW_USB_ReadSyscfg (usb_utr_t *ptr, uint16_t port);
void HW_USB_WriteSyscfg (usb_utr_t *ptr, uint16_t port, uint16_t data);
void HW_USB_ClearCnen (usb_utr_t *ptr);
void HW_USB_SetHse (usb_utr_t *ptr, uint16_t port);
void HW_USB_ClearHse (usb_utr_t *ptr, uint16_t port);
void HW_USB_SetDcfm (void);
void HW_USB_ClearDcfm (usb_utr_t *ptr);
void HW_USB_ClearDrpd (usb_utr_t *ptr, uint16_t port);
void HW_USB_SetUsbe (usb_utr_t *ptr);
void HW_USB_ClearUsbe (usb_utr_t *ptr);
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
void HW_USB_SetCnen (void);
void HW_USB_PSetDprpu (void);
void HW_USB_PClearDprpu (void);
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/************/
/*  BUSWAIT */
/************/
void HW_USB_SetBusWait (usb_utr_t *ptr);

/************/
/*  SYSSTS0 */
/************/
uint16_t HW_USB_ReadSyssts (usb_utr_t *ptr, uint16_t port);

/**************/
/*  DVSTCTR0  */
/**************/
uint16_t HW_USB_ReadDvstctr (usb_utr_t *ptr, uint16_t port);
void HW_USB_WriteDvstctr (usb_utr_t *ptr, uint16_t port, uint16_t data);
void HW_USB_RmwDvstctr (usb_utr_t *ptr, uint16_t port, uint16_t data, uint16_t width);
void HW_USB_ClearDvstctr (usb_utr_t *ptr, uint16_t port, uint16_t data);
void HW_USB_SetVbout (usb_utr_t *ptr, uint16_t port);
void HW_USB_ClearVbout (usb_utr_t *ptr, uint16_t port);
    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
void HW_USB_HSetRwupe (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearRwupe (usb_utr_t *ptr, uint16_t port);
void HW_USB_HsetResume (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearResume (usb_utr_t *ptr, uint16_t port);
void HW_USB_HSetUact (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearUact (usb_utr_t *ptr, uint16_t port);
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
void HW_USB_PSetWkup (void);
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/**************/
/*  TESTMODE  */
/**************/
void HW_USB_SetUtst (usb_utr_t *ptr, uint16_t data);

/***************************/
/*  CFIFO, D0FIFO, D1FIFO  */
/***************************/
uint32_t HW_USB_ReadFifo32 (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_WriteFifo32 (usb_utr_t *ptr, uint16_t pipemode, uint32_t data);
uint16_t HW_USB_ReadFifo16 (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_WriteFifo16 (usb_utr_t *ptr, uint16_t pipemode, uint16_t data);
void HW_UBS_WriteFifo8 (usb_utr_t *ptr, uint16_t pipemode, uint8_t data);

/************************************/
/*  CFIFOSEL, D0FIFOSEL, D1FIFOSEL  */
/************************************/
uint16_t HW_USB_ReadFifosel (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_RmwFifosel (usb_utr_t *ptr, uint16_t pipemode, uint16_t data, uint16_t width);
void HW_USB_SetDclrm (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_ClearDclrm (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_SetDreqe (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_ClearDreqe (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_SetMbw (usb_utr_t *ptr, uint16_t pipemode, uint16_t data);
void HW_USB_SetCurpipe (usb_utr_t *ptr, uint16_t pipemode, uint16_t pipeno);

/**********************************/
/* CFIFOCTR, D0FIFOCTR, D1FIFOCTR */
/**********************************/
uint16_t HW_USB_ReadFifoctr (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_SetBval (usb_utr_t *ptr, uint16_t pipemode);
void HW_USB_SetBclr (usb_utr_t *ptr, uint16_t pipemode);

/*************/
/*  INTENB0  */
/*************/
uint16_t HW_USB_ReadIntenb (usb_utr_t *ptr);
void HW_USB_WriteIntenb (usb_utr_t *ptr, uint16_t data);
void HW_USB_SetIntenb (usb_utr_t *ptr, uint16_t data);
void HW_USB_ClearEnbVbse (usb_utr_t *ptr);
void HW_USB_ClearEnbSofe (usb_utr_t *ptr);
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
void HW_USB_PSetEnbRsme (void);
void HW_USB_PClearEnbRsme (void);
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/*************/
/*  BRDYENB  */
/*************/
void HW_USB_WriteBrdyenb (usb_utr_t *ptr, uint16_t data);
void HW_USB_SetBrdyenb (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_ClearBrdyenb (usb_utr_t *ptr, uint16_t pipeno);

/*************/
/*  NRDYENB  */
/*************/
void HW_USB_WriteNrdyenb (usb_utr_t *ptr, uint16_t data);
void HW_USB_SetNrdyenb (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_ClearNrdyenb (usb_utr_t *ptr, uint16_t pipeno);

/*************/
/*  BEMPENB  */
/*************/
void HW_USB_WriteBempenb (usb_utr_t *ptr, uint16_t data);
void HW_USB_SetBempenb (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_ClearBempenb (usb_utr_t *ptr, uint16_t pipeno);

/*************/
/*  SOFCFG   */
/*************/
void HW_USB_SetSofcfg (usb_utr_t *ptr, uint16_t data);
    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
void HW_USB_HSetTrnensel (usb_utr_t *ptr);
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/*************/
/*  INTSTS0  */
/*************/
void HW_USB_WriteIntsts (usb_utr_t *ptr, uint16_t data);
void HW_USB_ClearStsSofr (usb_utr_t *ptr);
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
uint16_t HW_USB_ReadIntsts (void);
void HW_USB_PClearStsResm (void);
void HW_USB_PClearStsValid (void);
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/************/
/* BRDYSTS  */
/************/
uint16_t HW_USB_Readbrdysts (usb_utr_t *ptr);
void HW_USB_WriteBrdysts (usb_utr_t *pt, uint16_t data);
void HW_USB_ClearStsBrdy (usb_utr_t *ptr, uint16_t pipeno);

/************/
/* NRDYSTS  */
/************/
void HW_USB_WriteNrdysts (usb_utr_t *ptr, uint16_t data);
void HW_USB_ClearStatusNrdy (usb_utr_t *ptr, uint16_t pipeno);

/************/
/* BEMPSTS  */
/************/
void HW_USB_WriteBempsts (usb_utr_t *ptr, uint16_t data);
void HW_USB_ClearStatusBemp (usb_utr_t *ptr, uint16_t pipeno);

/************/
/* FRMNUM   */
/************/
uint16_t HW_USB_ReadFrmnum (usb_utr_t *ptr);

    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/************/
/* USBREQ   */
/************/
void HW_USB_HWriteUsbreq (usb_utr_t *ptr, uint16_t data);

/************/
/* USBVAL   */
/************/
void HW_USB_HSetUsbval (usb_utr_t *ptr, uint16_t data);

/************/
/* USBINDX  */
/************/
void HW_USB_HSetUsbindx (usb_utr_t *ptr, uint16_t data);

/************/
/* USBLENG  */
/************/
void HW_USB_HSetUsbleng (usb_utr_t *ptr, uint16_t data);
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/************/
/* USBREQ   */
/************/
uint16_t HW_USB_ReadUsbreq (void);

/************/
/* USBVAL   */
/************/
uint16_t HW_USB_ReadUsbval (void);

/************/
/* USBINDX  */
/************/
uint16_t HW_USB_ReadUsbindx (void);

/************/
/* USBLENG  */
/************/
uint16_t HW_USB_ReadUsbleng (void);

    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/************/
/* DCPCFG   */
/************/
uint16_t HW_USB_ReadDcpcfg (usb_utr_t *ptr);
void HW_USB_WriteDcpcfg (usb_utr_t *ptr, uint16_t data);

/************/
/* DCPMAXP  */
/************/
uint16_t HW_USB_ReadDcpmaxp (usb_utr_t *ptr);
void HW_USB_WriteDcpmxps (usb_utr_t *ptr, uint16_t data);

/************/
/* DCPCTR   */
/************/
    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
void HW_USB_HWriteDcpctr (usb_utr_t *ptr, uint16_t data);
void HW_USB_HSetSureq (usb_utr_t *ptr);
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
uint16_t HW_USB_ReadDcpctr (void);
void HW_USB_PSetCcpl (void);
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/************/
/* PIPESEL  */
/************/
void HW_USB_WritePipesel (usb_utr_t *ptr, uint16_t data);

/************/
/* PIPECFG  */
/************/
uint16_t HW_USB_ReadPipecfg (usb_utr_t *ptr);
void HW_USB_WritePipecfg (usb_utr_t *ptr, uint16_t data);

/************/
/* PIPEBUF  */
/************/
void HW_USB_WritePipebuf (usb_utr_t *ptr, uint16_t data);
uint16_t HW_USB_ReadPipebuf (usb_utr_t *ptr);

/************/
/* PIPEMAXP */
/************/
uint16_t HW_USB_ReadPipemaxp (usb_utr_t *ptr);
void HW_USB_WritePipemaxp (usb_utr_t *ptr, uint16_t data);

/************/
/* PIPEPERI */
/************/
void HW_USB_WritePipeperi (usb_utr_t *ptr, uint16_t data);

/********************/
/* DCPCTR, PIPEnCTR */
/********************/
uint16_t HW_USB_ReadPipectr (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_WritePipectr (usb_utr_t *ptr, uint16_t pipeno, uint16_t data);
void HW_USB_SetCsclr (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_SetAclrm (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_ClearAclrm (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_SetSqclr (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_SetSqset (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_SetPid (usb_utr_t *ptr, uint16_t pipeno, uint16_t data);
void HW_USB_ClearPid (usb_utr_t *ptr, uint16_t pipeno, uint16_t data);

/************/
/* PIPEnTRE */
/************/
void HW_USB_SetTrenb (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_ClearTrenb (usb_utr_t *ptr, uint16_t pipeno);
void HW_USB_SetTrclr (usb_utr_t *ptr, uint16_t pipeno);

/************/
/* PIPEnTRN */
/************/
void HW_USB_WritePipetrn (usb_utr_t *ptr, uint16_t pipeno, uint16_t data);

/************/
/* BCCTRL   */
/************/
uint16_t HW_USB_ReadBcctrl (usb_utr_t *ptr);
void HW_USB_SetVdmsrce (usb_utr_t *ptr);
void HW_USB_ClearVdmsrce (usb_utr_t *ptr);
void HW_USB_SetIdpsinke (usb_utr_t *ptr);
void HW_USB_ClearIdpsinke (usb_utr_t *ptr);
    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
void HW_USB_HsetDcpmode (usb_utr_t *ptr);
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/**********************************/
/*  DMA0CFG, DMA1CFG  for 597ASSP */
/**********************************/
void HW_USB_WriteDmacfg (usb_utr_t *ptr, uint16_t pipemode, uint16_t data);

    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/*************/
/*  INTENB1  */
/*************/
void HW_USB_HWriteIntenb (usb_utr_t *ptr, uint16_t port, uint16_t data);
void HW_USB_HSetEnbOvrcre (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearEnbOvrcre (usb_utr_t *ptr, uint16_t port);
void HW_USB_HSetEnbBchge (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearEnbBchge (usb_utr_t *ptr, uint16_t port);
void HW_USB_HSetEnbDtche (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearEnbDtche (usb_utr_t *ptr, uint16_t port);
void HW_USB_HSetEnbAttche (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearEnbAttche (usb_utr_t *ptr, uint16_t port);
void HW_USB_HSetEnbSigne (usb_utr_t *ptr);
void HW_USB_HSetEnbSacke (usb_utr_t *ptr);
void HW_USB_HSetEnbPddetinte (usb_utr_t *ptr);

/*************/
/*  INTSTS1  */
/*************/
void HW_USB_HWriteIntsts (usb_utr_t *ptr, uint16_t port, uint16_t data);
void HW_USB_HClearStsOvrcr (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearStsBchg (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearStsDtch (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearStsAttch (usb_utr_t *ptr, uint16_t port);
void HW_USB_HClearStsSign (usb_utr_t *ptr);
void HW_USB_HClearStsSack (usb_utr_t *ptr);
void HW_USB_HClearStsPddetint (usb_utr_t *ptr, uint16_t port);

/************/
/* DEVADDn  */
/************/
uint16_t HW_USB_HReadDevadd (usb_utr_t *ptr, uint16_t devadr);
void HW_USB_HRmwDevadd (usb_utr_t *ptr, uint16_t devsel, uint16_t data, uint16_t width);
void HW_USB_HSetUsbspd (usb_utr_t *ptr, uint16_t devadr, uint8_t data);
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#endif /* __HW_USB_REG_ACCESS_H__ */
/******************************************************************************
 End of file
 ******************************************************************************/
