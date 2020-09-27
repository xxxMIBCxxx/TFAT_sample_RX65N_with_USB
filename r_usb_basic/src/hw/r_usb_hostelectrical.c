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
 * File Name    : r_usb_hostelectrical.c
 * Description  : USB Host Electrical Test code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 04.01.2014 1.00 First Release
 *         : 30.01.2015 1.01    Added RX71M.
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_if.h"
#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if defined(BSP_MCU_RX71M)
    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : usb_hstd_test_stop
 Description     : Host electrical test stop
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t port   : port number.
 Return value    : none
 ******************************************************************************/
void usb_hstd_test_stop(usb_utr_t *ptr, uint16_t port)
{
    /* USBRST=0, RESUME=0, UACT=1 */
    usb_hstd_set_uact(ptr, port);
}/* eof usb_hstd_test_stop() */

/******************************************************************************
 Function Name   : usb_hstd_test_signal
 Description     : Host electrical test signal control.
 Arguments       : usb_utr_t *ptr   : USB internal structure. Selects USB channel.
 : uint16_t port    : port number
 : uint16_t command          : command
 Return value    : none
 ******************************************************************************/
void usb_hstd_test_signal(usb_utr_t *ptr, uint16_t port, uint16_t command)
{
    uint16_t buff;

    switch (command)
    {
        case 1: buff = USB_H_TST_J; break;
        case 2: buff = USB_H_TST_K; break;
        case 3: buff = USB_H_TST_SE0_NAK; break;
        case 4: buff = USB_H_TST_PACKET; break;

        default:
        buff = USB_H_TST_NORMAL;
        HW_USB_SetUtst(ptr, buff);
        usb_hstd_sw_reset(ptr);
        break;
    }

    usb_hstd_test_uact_ctrl(ptr, port, (uint16_t)USB_UACTOFF);
    HW_USB_SetUtst(ptr, buff);
    usb_hstd_test_uact_ctrl(ptr, port, (uint16_t)USB_UACTON);
}/* eof usb_hstd_test_signal() */

/******************************************************************************
 Function Name   : usb_hstd_test_uact_ctrl
 Description     : Host electrical test SOF control.
 Arguments       : usb_utr_t *ptr   : USB internal structure. Selects USB channel.
 : uint16_t port    : port number
 : uint16_t command          : USB_UACTON / OFF
 Return value    : none
 ******************************************************************************/
void usb_hstd_test_uact_ctrl(usb_utr_t *ptr, uint16_t port, uint16_t command)
{

    if (command == USB_UACTON)
    {
        /* SOF out disable */
        HW_USB_HSetUact(ptr, port);
    }
    else
    {
        /* SOF out disable */
        HW_USB_HClearUact(ptr, port);
    }
    /* Wait 1ms */
    usb_cpu_delay_xms((uint16_t)1);
}/* eof usb_hstd_test_uact_ctrl() */

/******************************************************************************
 Function Name   : usb_hstd_test_vbus_ctrl
 Description     : Host electrical test VBUS control.
 Arguments       : usb_utr_t *ptr   : USB internal structure. Selects USB channel.
 : uint16_t port    : port number
 : uint16_t command          : USB_UACTON / OFF
 Return value    : none
 ******************************************************************************/
void usb_hstd_test_vbus_ctrl(usb_utr_t *ptr, uint16_t port, uint16_t command)
{
    if (command == USB_VBON)
    {
        /* VBUS on */
        HW_USB_SetVbout(ptr, port);
    }
    else
    {
        /* VBUS off */
        HW_USB_ClearVbout(ptr, port);
    }
    /* Wait 1ms */
    usb_cpu_delay_xms((uint16_t)1);
}/* eof usb_hstd_test_vbus_ctrl() */

/******************************************************************************
 Function Name   : usb_hstd_test_bus_reset
 Description     : Host electrical test USB-reset signal control.
 Arguments       : usb_utr_t *ptr   : USB internal structure. Selects USB channel.
 : uint16_t port    : port number
 Return value    : none
 ******************************************************************************/
void usb_hstd_test_bus_reset(usb_utr_t *ptr, uint16_t port)
{
    /* USBRST=1, UACT=0 */
    HW_USB_RmwDvstctr(ptr, port, USB_USBRST, (USB_USBRST | USB_UACT));

    /* Wait 50ms */
    usb_cpu_delay_xms((uint16_t)50);
    /* USBRST=0 */
    HW_USB_ClearDvstctr(ptr, USB_PORT0, USB_USBRST); //for UTMI
    usb_cpu_delay_1us(300);//for UTMI

    /* USBRST=0, RESUME=0, UACT=1 */
    usb_hstd_set_uact(ptr, port);
    /* Wait 10ms or more (USB reset recovery) */
    usb_cpu_delay_xms((uint16_t)20);
}/* eof usb_hstd_test_bus_reset() */

/******************************************************************************
 Function Name   : usb_hstd_test_suspend
 Description     : Host electrical test suspend control.
 Arguments       : usb_utr_t *ptr   : USB internal structure. Selects USB channel.
 : uint16_t port    : port number
 Return value    : none
 ******************************************************************************/
void usb_hstd_test_suspend(usb_utr_t *ptr, uint16_t port)
{
    /* SOF out disable */
    HW_USB_HClearUact(ptr, port);
    /* Wait 1ms */
    usb_cpu_delay_xms((uint16_t)1);
}/* eof usb_hstd_test_suspend() */

/******************************************************************************
 Function Name   : usb_hstd_test_resume
 Description     : Host electrical test resume control.
 Arguments       : usb_utr_t *ptr   : USB internal structure. Selects USB channel.
 : uint16_t port    : port number
 Return value    : none
 ******************************************************************************/
void usb_hstd_test_resume(usb_utr_t *ptr, uint16_t port)
{
    /* RESUME bit on */
    HW_USB_HsetResume(ptr, port);
    /* Wait */
    usb_cpu_delay_xms((uint16_t)20);
    /* RESUME bit off */
    HW_USB_HClearResume(ptr, port);
    /* SOF on */
    HW_USB_HSetUact(ptr, port);
}/* eof usb_hstd_test_resume() */
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
#endif  /* defined(BSP_MCU_RX71M) */

/******************************************************************************
 End of file
 ******************************************************************************/
