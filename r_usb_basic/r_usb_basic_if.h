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
/** @file r_usb_basic_if.h
 *  @brief  User macro define file@n
 *  This file is the macrodefinition header file which a user can operate. @n
 *  @author Renesas Electronics Corporation.
 *  @date 09.31.2015 0.01 First Release.
 */
/**********************************************************************************************************************/
#ifndef _R_USB_BASIC_IF_H_
    #define _R_USB_BASIC_IF_H_

/*****************************************************************************
 Typedef definitions
 ******************************************************************************/
typedef void* VP; /* Pointer to variable      */
typedef long ER; /* Error code               */
typedef short ID; /* Object ID (xxxid)        */
typedef long TMO; /* Time out                 */
typedef long VP_INT; /* Integer data             */

/*----------- msghead -----------*/
typedef struct
{
    VP msghead; /* Message header               */
} t_msg_t;

typedef t_msg_t usb_msg_t; /* ITRON message            */
typedef ER usb_er_t; /* ITRON system call err    */
typedef ID usb_id_t; /* ITRON system call define */
typedef TMO usb_tm_t; /* ITRON time out           */
typedef VP usb_mh_t; /* ITRON Message Header     */
typedef VP_INT usb_vp_int_t;

    #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
typedef volatile struct st_usba * usb_regadr1_t;
    #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

    #if defined(BSP_MCU_RX63N)
typedef volatile struct st_usb1 * usb_regadr1_t;
    #endif  /* defined(BSP_MCU_RX63N)*/

typedef volatile struct st_usb0 * usb_regadr_t;

typedef struct usb_utr usb_utr_t;
/*typedef void (*usb_utr_cb_t)(usb_utr_t *); */

/*#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )*/
typedef void (*usb_cb_t) (struct usb_utr *, uint16_t, uint16_t);
/*#endif */ /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

/*#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
  typedef void (*usb_cb_t)(uint16_t);
  #endif */ /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */

typedef struct usb_utr
{
    usb_mh_t                                                                                                                                                                                              msghead; /* Message header (for SH-solution) */
    uint16_t                                                                                                                                                                                              msginfo; /* Message Info for F/W */
    uint16_t                                                                                                                                                                                              keyword; /* Rootport / Device address / Pipe number */
    union
    {
        usb_regadr_t ipp; /* IP Address(USB0orUSB1)*/
    #if USB_NUM_USBIP == 2
    usb_regadr1_t ipp1; /* IP Address(USBHS) */
    #endif /* USB_NUM_USBIP == 2 */
};
uint16_t                                                                                                                                                                                              ip; /* IP number(0or1) */
uint16_t                                                                                                                                                                                              result; /* Result */
usb_cb_t                                                                                                                                                                                              complete; /* Call Back Function Info */
void                                                                                                                                                                                                  *tranadr; /* Transfer data Start address */
uint32_t                                                                                                                                                                                              tranlen; /* Transfer data length */
uint16_t                                                                                                                                                                                              *setup; /* Setup packet(for control only) */
uint16_t                                                                                                                                                                                              status; /* Status */
uint16_t                                                                                                                                                                                              pipectr; /* Pipe control register */
uint8_t                                                                                                                                                                                               errcnt; /* Error count */
uint8_t                                                                                                                                                                                               segment; /* Last flag */
void                                                                                                                                                                                                  *usr_data;
} usb_message_t;


    #define	USB_M0  USB0
    #if defined(BSP_MCU_RX63N)
        #define	USB_M1	USB1
    #endif  /* defined(BSP_MCU_RX63N) */

    #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        #define	USB_M1    USBA
    #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

#endif /* _R_USB_BASIC_IF_H_ */
