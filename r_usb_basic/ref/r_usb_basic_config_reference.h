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
 * File Name    : r_usb_basic_config_reference.h
 * Description  : USB User definition
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

#ifndef __R_USB_BASIC_CONFIG_H__
    #define __R_USB_BASIC_CONFIG_H__

/** [Device class setting] */
//#define USB_CFG_HCDC_USE    /* Host Communication Device Class */
//#define USB_CFG_HHID_USE    /* Host Human Interface Device Class */
//#define USB_CFG_HMSC_USE    /* Host Mass Storage Class */
//#define USB_CFG_HVND_USE    /* Host VENDER Class */
//#define USB_CFG_PCDC_USE    /* Peripheral Communication Device Class */
//#define USB_CFG_PHID_USE    /* Peripheral Human Interface Device Class */
//#define USB_CFG_PMSC_USE    /* Peripheral Mass Storage Class */
//#define USB_CFG_PVND_USE    /* Peripheral VENDER Class */

/**  [DTC/CMA setting]
 * USB_CFG_ENABLE  : DTC/DMA use
 * USB_CFG_DISABLE : DTC/DMA none
 */
    #define    USB_CFG_DTCDMA    USB_CFG_DISABLE    /* DTC/DMA none   */

/**  [Endian setting]
 * USB_CFG_LITTLE : Little Endian
 * USB_CFG_BIG    : Big Endian
 */
    #define    USB_CFG_ENDIAN    USB_CFG_LITTLE     /* Little Endian  */

/** [USB Host/Peri mode]
 *  USB_CFG_HOST : USB Host mode.
 *  USB_CFG_PERI : USB Peri mode.
 */
    #define USB_CFG_MODE          (USB_CFG_HOST | USB_CFG_PERI)

/** [USB Host VBUS Polarity]
 * USB_CFG_HIGH : High assert
 * USB_CFG_LOW  : Low assert
 */
        #define USB_CFG_VBUS          (USB_CFG_HIGH)

/** [USE USB IP select]
 *  USB_CFG_IP0 : USB0
 *  USB_CFG_IP1 : USBA(RX64M,RX71M only.)
 */
        #define USB_CFG_USE_USBIP   USB_CFG_IP0

/** [USBA PHY Setting Register (PHYSET CLKSEL[1:0])]
 * USB_CFG_24MHZ : 24MHz
 * USB_CFG_20MHZ : 20MHz
 */
    #define USB_CFG_CLKSEL        (USB_CFG_24MHZ)     /* 24MHz */

/** [CPU Bus Access Wait Select(CPU Bus Wait Register (BUSWAIT)BWAIT[3:0])]
 *  0 : 2 access cycles  waits
 *  1 : 3 access cycles  waits
 *  2 : 4 access cycles  waits
 *  3 : 5 access cycles  waits
 *  4 : 6 access cycles  waits
 *  5 : 7 access cycles  waits
 *  6 : 8 access cycles  waits
 *  7 : 9 access cycles  waits
 *  8 : 10 access cycles waits
 *  9 : 11 access cycles waits
 *  10: 12 access cycles waits
 *  11: 13 access cycles waits
 *  12: 14 access cycles waits
 *  13: 15 access cycles waits
 *  14: 16 access cycles waits
 *  15: 17 access cycles waits
 */
        #define USB_CFG_BUSWAIT       (7)

/** [MCU Low power support for USB suspended/detach]
 * USB_CFG_ENABLE  : USB suspended/detach MCU Low power support
 * USB_CFG_DISABLE : USB suspended/detach MCU Low power not support
 */
        #define USB_CFG_LPW           (USB_CFG_DISABLE)

/** [Battery Charging setting]
 * USB_CFG_ENABLE  : BC function using.
 * USB_CFG_DISABLE : BC function don't use.
 */
    #define USB_CFG_BC            (USB_CFG_DISABLE)

/** [Dedicated Charging Port(DCP) function setting.]
 * USB_CFG_ENABLE  : Enable.
 * USB_CFG_DISABLE : Disable.
 */
        #define USB_CFG_DCP           (USB_CFG_DISABLE)

/** [Compliance Test mode setting]
 * USB_CFG_ENABLE  : It supports a compliance test.
 * USB_CFG_DISABLE : It not  supports a compliance test.
 */
        #define USB_CFG_COMPLIANCE    (USB_CFG_DISABLE)

/** [Hi-speed(HS) Electrical Test setting.]
 * USB_CFG_ENABLE  : It supports an HS electrical test.
 * USB_CFG_DISABLE : It not supports an HS electrical test.
 */
        #define USB_CFG_ELECTRICAL    (USB_CFG_DISABLE)

/** [Target Peripheral List (TPL) setting]
 * USB_CFG_TPLCNT : Setting the number of the USB devices to connect.
 * USB_CFG_TPL    : Setting VID and PID of the USB device.
 * */
        #define USB_CFG_TPLCNT        (1)
        #define USB_CFG_TPL           USB_NOVENDOR, USB_NOPRODUCT

/** [Target Peripheral List(TPL) setting (USB Hub)]
 * USB_CFG_HUB_TPLCNT : Setting the number of the USB Hub to connect.
 * USB_CFG_HUB_TPL    : Setting VID and PID of the USB Hub.
 */
        #define USB_CFG_HUB_TPLCNT    (1)
        #define USB_CFG_HUB_TPL       USB_NOVENDOR, USB_NOPRODUCT

/******************************************************************************/
/** OTHER DEFINITIONS SETTING                                                 */
/******************************************************************************/
/** [DBLB bit setting]
 * USB_CFG_DBLBON  : DBLB bit setting.
 * USB_CFG_DBLBOFF : DBLB bit clear.
 */
    #define USB_CFG_DBLB          (USB_CFG_DBLBON)

/** [SHTNAK bit setting]
 * USB_CFG_SHTNAKON  : SHTNAK bit setting.
 * USB_CFG_SHTNAKOFF : SHTNAK bit clear.
 */
    #define USB_CFG_SHTNAK        (USB_CFG_SHTNAKON)

/** [CNTMD bit setting]
 * USB_CFG_CNTMDON  : CNTMD bit setting.
 * USB_CFG_CNTMDOFF : CNTMD bit clear.
 */
    #define USB_CFG_CNTMD         (USB_CFG_CNTMDOFF)

#endif  /* __R_USB_BASIC_CONFIG_H__ */
/******************************************************************************
 End  Of File
 ******************************************************************************/

