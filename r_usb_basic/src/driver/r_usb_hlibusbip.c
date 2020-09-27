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
 * File Name    : r_usb_hlibusbip.c
 * Description  : USB IP Host library.
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 04.01.2014 1.00 First Release
 *         : 30.01.2015 1.01    Support Multi device.
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

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Renesas Abstracted Host Lib IP functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_set_dev_addr
 Description     : Set USB speed (Full/Hi) of the connected USB Device.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t addr             : device address
 : uint16_t speed            : device speed
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_set_dev_addr (usb_utr_t *ptr, uint16_t addr, uint16_t speed, uint16_t port)
{
    if (USB_DEVICE_0 == addr)
    {
        HW_USB_WriteDcpmxps(ptr, (uint16_t) (USB_DEFPACKET + USB_DEVICE_0));
    }
    HW_USB_HSetUsbspd(ptr, addr, (speed | port));
}
/******************************************************************************
 End of function usb_hstd_set_dev_addr
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_bchg_enable
 Description     : Enable BCHG interrupt for the specified USB port.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_bchg_enable (usb_utr_t *ptr, uint16_t port)
{

    HW_USB_HClearStsBchg(ptr, port);
    HW_USB_HSetEnbBchge(ptr, port);

}
/******************************************************************************
 End of function usb_hstd_bchg_enable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_bchg_disable
 Description     : Disable BCHG interrupt for specified USB port.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_bchg_disable (usb_utr_t *ptr, uint16_t port)
{

    HW_USB_HClearStsBchg(ptr, port);
    HW_USB_HClearEnbBchge(ptr, port);

}
/******************************************************************************
 End of function usb_hstd_bchg_disable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_set_uact
 Description     : Start sending SOF to the connected USB device.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_set_uact (usb_utr_t *ptr, uint16_t port)
{
    HW_USB_RmwDvstctr(ptr, port, USB_UACT, (USB_USBRST | USB_RESUME | USB_UACT));
}
/******************************************************************************
 End of function usb_hstd_set_uact
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_ovrcr_enable
 Description     : Enable OVRCR interrupt of the specified USB port.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_ovrcr_enable (usb_utr_t *ptr, uint16_t port)
{

    HW_USB_HClearStsOvrcr(ptr, port);
    HW_USB_HSetEnbOvrcre(ptr, port);

}
/******************************************************************************
 End of function usb_hstd_ovrcr_enable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_ovrcr_disable
 Description     : Disable OVRCR interrupt of the specified USB port.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_ovrcr_disable (usb_utr_t *ptr, uint16_t port)
{
    /* OVRCR Clear(INT_N edge sense) */
    HW_USB_HClearStsOvrcr(ptr, port);

    /* Over-current disable */
    HW_USB_HClearEnbOvrcre(ptr, port);
}
/******************************************************************************
 End of function usb_hstd_ovrcr_disable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_attch_enable
 Description     : Enable ATTCH (attach) interrupt of the specified USB port.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_attch_enable (usb_utr_t *ptr, uint16_t port)
{

    /* ATTCH status Clear */
    HW_USB_HClearStsAttch(ptr, port);

    /* Attach enable */
    HW_USB_HSetEnbAttche(ptr, port);

}
/******************************************************************************
 End of function usb_hstd_attch_enable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_attch_disable
 Description     : Disable ATTCH (attach) interrupt of the specified USB port.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_attch_disable (usb_utr_t *ptr, uint16_t port)
{

    /* ATTCH Clear(INT_N edge sense) */
    HW_USB_HClearStsAttch(ptr, port);

    /* Attach disable */
    HW_USB_HClearEnbAttche(ptr, port);

}
/******************************************************************************
 End of function usb_hstd_attch_disable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_dtch_enable
 Description     : Enable DTCH (detach) interrupt of the specified USB port. 
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_dtch_enable (usb_utr_t *ptr, uint16_t port)
{

    /* DTCH Clear */
    HW_USB_HClearStsDtch(ptr, port);

    /* Detach enable */
    HW_USB_HSetEnbDtche(ptr, port);

}
/******************************************************************************
 End of function usb_hstd_dtch_enable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_dtch_disable
 Description     : Disable DTCH (detach) interrupt of the specified USB port.  
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : uint16_t port             : root port
 Return value    : none
 ******************************************************************************/
void usb_hstd_dtch_disable (usb_utr_t *ptr, uint16_t port)
{

    /* DTCH Clear(INT_N edge sense) */
    HW_USB_HClearStsDtch(ptr, port);

    /* Detach disable */
    HW_USB_HClearEnbDtche(ptr, port);

}
/******************************************************************************
 End of function usb_hstd_dtch_disable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_set_pipe_register
 Description     : Set up USB registers to use specified pipe (given in infor-
 : mation table).
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : uint16_t pipe_no          : pipe number
 : uint16_t *tbl     : pipe information table 
 Return value    : none
 ******************************************************************************/
void usb_hstd_set_pipe_register (usb_utr_t *ptr, uint16_t pipe_no, uint16_t *tbl)
{
    uint16_t i;
    uint16_t pipe;
    uint16_t buf;

    /* PIPE USE check */
    if (USB_USEPIPE == pipe_no)
    {
        /* Current FIFO port Clear */
        usb_cstd_chg_curpipe(ptr, (uint16_t) USB_PIPE0, (uint16_t) USB_CUSE, USB_FALSE);
        usb_cstd_chg_curpipe(ptr, (uint16_t) USB_PIPE0, (uint16_t) USB_D0USE, USB_FALSE);
        usb_cstd_chg_curpipe(ptr, (uint16_t) USB_PIPE0, (uint16_t) USB_D1USE, USB_FALSE);

        /* EP Table loop */
        for (i = 0; tbl[i] != USB_PDTBLEND; i += USB_EPL)
        {
            /* PipeNo Number */
            pipe = (uint16_t) (tbl[i + 0] & USB_CURPIPE);

            /* PIPE Setting */
            usb_cstd_pipe_init(ptr, pipe, tbl, i);
        }
    }
    else
    {
        /* Current FIFO port Clear */
        usb_cstd_chg_curpipe(ptr, (uint16_t) USB_PIPE0, (uint16_t) USB_CUSE, USB_FALSE);
        buf = HW_USB_ReadFifosel(ptr, USB_D0USE);
        if ((buf & USB_CURPIPE) == pipe_no)
        {
            usb_cstd_chg_curpipe(ptr, (uint16_t) USB_PIPE0, (uint16_t) USB_D0USE, USB_FALSE);
        }
        buf = HW_USB_ReadFifosel(ptr, USB_D1USE);
        if ((buf & USB_CURPIPE) == pipe_no)
        {
            usb_cstd_chg_curpipe(ptr, (uint16_t) USB_PIPE0, (uint16_t) USB_D1USE, USB_FALSE);
        }

        /* EP Table loop */
        for (i = 0; tbl[i] != USB_PDTBLEND; i += USB_EPL)
        {
            /* PipeNo Number */
            pipe = (uint16_t) (tbl[i + 0] & USB_CURPIPE);
            if (pipe == pipe_no)
            {
                /* PIPE Setting */
                usb_cstd_pipe_init(ptr, pipe, tbl, i);
            }
        }
    }
}
/******************************************************************************
 End of function usb_hstd_set_pipe_register
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_get_rootport
 Description     : Get USB port no. set in the USB register based on the speci-
 : fied USB Device address.
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : uint16_t addr             : device address
 Return value    : uint16_t                  : root port number
 ******************************************************************************/
uint16_t usb_hstd_get_rootport (usb_utr_t *ptr, uint16_t addr)
{
    uint16_t buffer;

    /* Get device address configuration register from device address */
    buffer = HW_USB_HReadDevadd(ptr, addr);
    if (USB_ERROR != buffer)
    {
        /* Return root port number */
        return (uint16_t) (buffer & USB_RTPORT);
    }
    return USB_ERROR;
}
/******************************************************************************
 End of function usb_hstd_get_rootport
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_chk_dev_addr
 Description     : Get USB speed set in USB register based on the specified USB 
 : Device address and USB port no.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t addr             : device address
 : uint16_t rootport         : root port
 Return value    : uint16_t                  : USB speed etc
 ******************************************************************************/
uint16_t usb_hstd_chk_dev_addr (usb_utr_t *ptr, uint16_t addr, uint16_t rootport)
{
    uint16_t buffer;

    /* Get device address configuration register from device address */
    buffer = HW_USB_HReadDevadd(ptr, addr);
    if (USB_ERROR != buffer)
    {
        if ((uint16_t) (buffer & USB_RTPORT) == rootport)
        {
            /* Return Address check result */
            return (uint16_t) (buffer & USB_USBSPD);
        }
    }
    return USB_NOCONNECT;
}
/******************************************************************************
 End of function usb_hstd_chk_dev_addr
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_get_dev_speed
 Description     : Get USB speed set in USB register based on the specified USB 
 : Device address.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t addr             : device address
 Return value    : uint16_t                  : device speed
 Note            : Use also to a connection check is possible
 ******************************************************************************/
uint16_t usb_hstd_get_dev_speed (usb_utr_t *ptr, uint16_t addr)
{
    uint16_t buffer;

    /* Get device address configuration register from device address */
    buffer = HW_USB_HReadDevadd(ptr, addr);
    if (USB_ERROR != buffer)
    {
        /* Return device speed */
        return (uint16_t) (buffer & USB_USBSPD);
    }
    return USB_NOCONNECT;
}
/******************************************************************************
 End of function usb_hstd_get_dev_speed
 ******************************************************************************/
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 End  Of File
 ******************************************************************************/
