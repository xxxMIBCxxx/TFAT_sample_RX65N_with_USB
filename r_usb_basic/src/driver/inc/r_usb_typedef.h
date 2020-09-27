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
/**********************************************************************************************************************/
/** @file r_usb_typedef.h
 *  @brief  User macro define file@n
 *  This file is the macrodefinition header file which a user can operate. @n
 *  @author Renesas Electronics Corporation.
 *  @date 09.31.2015 0.01 First Release.
 */
/**********************************************************************************************************************/
#ifndef __R_USB_TYPEDEF_H__
    #define __R_USB_TYPEDEF_H__

/******************************************************************************
 Typedef definitions
 ******************************************************************************/
typedef struct st_usb usb_stnbyint_t;

typedef void (*usb_cb_check_t) (struct usb_utr *, uint16_t**);

/* Class request processing function type. */
typedef void (*usb_cb_trn_t) (usb_setup_t* preq, uint16_t ctsq);

    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
typedef struct USB_HCDREG
{
    uint16_t       rootport; /* Root port */
    uint16_t       devaddr; /* Device address */
    uint16_t       devstate; /* Device state */
    uint16_t       ifclass; /* Interface Class */
    uint16_t       *tpl; /* Target peripheral list
     (Vendor ID, Product ID) */
    uint16_t       *pipetbl; /* Pipe Define Table address */
    usb_cb_t       classinit; /* Driver init */
    usb_cb_check_t classcheck; /* Driver check */
    usb_cb_t       devconfig; /* Device configured */
    usb_cb_t       devdetach; /* Device detach */
    usb_cb_t       devsuspend; /* Device suspend */
    usb_cb_t       devresume; /* Device resume */
} usb_hcdreg_t;
    #endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
typedef struct USB_PCDREG
{
    uint16_t     *pipetbl; /* Pipe Define Table address */
    uint8_t      *devicetbl; /* Device descriptor Table address */
    uint8_t      *qualitbl; /* Qualifier descriptor Table address */
    uint8_t      *configtbl; /* Configuration descriptor Table address */
    uint8_t      *othertbl; /* Other configuration descriptor Table address */
    uint8_t      **stringtbl; /* String descriptor Table address */
    usb_cb_t     devdefault; /* Device default */
    usb_cb_t     devconfig; /* Device configured */
    usb_cb_t     devdetach; /* Device detach */
    usb_cb_t     devsuspend; /* Device suspend */
    usb_cb_t     devresume; /* Device resume */
    usb_cb_t     interface; /* Interface changed */
    usb_cb_trn_t ctrltrans; /* Control Transfer */
} usb_pcdreg_t;
    #endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */

typedef struct usb_utr usb_hcdinfo_t;
typedef struct usb_utr usb_mgrinfo_t;
typedef struct usb_utr usb_pcdinfo_t;
typedef struct usb_utr usb_clsinfo_t;

    #if USB_CFG_BC == USB_CFG_ENABLE
typedef struct
{
    uint8_t dcpmode; /* DCP Mode Flag */
    uint8_t state; /* BC State */
    uint8_t pd_detect; /* PD Detect Flag */
} usb_bc_status_t;
    #endif  /* USB_CFG_BC == USB_CFG_ENABLE */

typedef struct usb_ctrl_trans
{
    usb_setup_t setup; /* Request command */
    uint16_t    address; /* Device address setting */
} usb_ctrl_trans_t;

    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
typedef struct
{
    struct
    {
        uint16_t type;
        uint16_t status;
    } buf[USB_INT_BUFSIZE];
    uint8_t                                                                 wp;
    uint8_t                                                                 rp;
} usb_int_t;
    #endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
typedef struct
{
    struct {
        uint16_t    ip;
        uint16_t    fifo_type;
    } buf[USB_INT_BUFSIZE];
    uint8_t         wp;
    uint8_t         rp;
} usb_dma_int_t;
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

/***************************************************************************************/
typedef struct USB_EVENT /* Structure for USB Event */
{
    uint8_t    write_pointer; /* Write pointer */
    uint8_t    read_pointer; /* Read pointer */
    uint8_t    code[USB_EVENT_MAX]; /* Event code */
    usb_ctrl_t ctrl[USB_EVENT_MAX]; /* Control Information */
} usb_event_t;

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/

#endif /* __R_USB_TYPEDEF_H__ */
