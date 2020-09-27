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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_usb_dmac.h
* Description  : DMA Difinition for USB
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/

#ifndef __R_USB_DMAC_H__
#define __R_USB_DMAC_H__

/*******************************************************************************
Includes   <System Includes>, "Project Includes"
*******************************************************************************/
#include "r_usb_if.h"
#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
#include "r_dmaca_rx_if.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
#define USB_DMA_CH          DMACA_CH0               /* Which channel to use for USB Pipe */
#define USB_DMA_CH2         DMACA_CH1               /* Which channel to use for USB Pipe */
#define USB_DMA_USE_CH_MAX  (USB_DMA_CH2 + 1)       /* MAX USE DMAC CH for USB */
#define USB_DMA_CH_PRI      (10)                    /* DMACmI interrupt priority level for USB Pipe */
#define USB_DMA_CH2_PRI     (10)                    /* DMACmI interrupt priority level for USB Pipe */

#define USB_PIPE1_DMA_CH    USB_DMA_CH

#define USB_PIPE2_DMA_CH    USB_DMA_CH2

#define USB_FIFO_TYPE_D0DMA         (0)                             /* D0FIFO Select */
#define USB_FIFO_TYPE_D1DMA         (1)                             /* D1FIFO Select */
#define USB_DMA_FIFO_TYPE_NUM       (USB_FIFO_TYPE_D1DMA + 1)       /*  */

#define USB_FIFO_ACCESS_TYPE_32BIT  (0)                             /* FIFO port 32bit access */
#define USB_FIFO_ACCESS_TYPE_16BIT  (1)                             /* FIFO port 16bit access */
#define USB_FIFO_ACCESS_TYPE_8BIT   (2)                             /* FIFO port 8bit access */

#define USB_FIFO_ACCSESS_TYPE_NUM   (USB_FIFO_ACCESS_TYPE_8BIT + 1) /*  */

#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */


#endif /* __R_USB_DMAC_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
