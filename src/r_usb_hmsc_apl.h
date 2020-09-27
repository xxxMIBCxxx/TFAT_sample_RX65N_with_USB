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
 * File Name    : r_usb_hmsc_apl.h
 * Description  : USB Host MSC application code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 01.09.2014 1.00 First Release
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "r_usb_if.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
#include "r_dmaca_rx_if.h"
#include "r_usb_dmac.h"
#endif /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

#include "r_tfat_lib.h"             /* TFAT header */

/******************************************************************************
 Constant macro definitions
 ******************************************************************************/
#define WRITE_YES                   (1)
#define WRITE_NO                    (0)

#define FILE_SIZE                   (512)
#define FILE_DATA                   (0x61)

/* Support Strage Drive number */
#define MAX_DEVICE_NUM                     (1)
/* #define MAX_DEVICE_NUM                     (2) */
/* #define MAX_DEVICE_NUM                     (3) */
/* #define MAX_DEVICE_NUM                     (4) */

/*****************************************************************************
 Enumerated Types
 ******************************************************************************/
typedef enum
{
    STATE_ATTACH, STATE_DATA_READY, STATE_DATA_WRITE, STATE_DATA_READ, STATE_DETACH, STATE_ERROR,
} state_t;

/******************************************************************************
External variables and functions
******************************************************************************/
extern      void usb_usb0_d0fifo_isr (void);
extern      void usb_usb0_d1fifo_isr (void);
extern      void usb_usb2_d0fifo_isr (void);
extern      void usb_usb2_d1fifo_isr (void);

/*****************************************************************************
 Function
 ******************************************************************************/
void usb_main (void);
void usb_hmsc_driver (void);
void msc_detach_device (uint8_t adr);
void msc_connect (usb_ctrl_t  *p_ctrl);
void msc_data_ready (uint8_t adr);
void msc_data_write (usb_ctrl_t  *p_ctrl);
void msc_data_read (usb_ctrl_t  *p_ctrl);
void usb_mcu_init (void);
void usb_board_init (usb_ctrl_t  *p_ctrl);
void apl_init (void);
void usb_hmsc_DummyFunction (usb_utr_t *ptr, uint16_t data1, uint16_t data2);

/******************************************************************************
 End  Of File
 ******************************************************************************/

