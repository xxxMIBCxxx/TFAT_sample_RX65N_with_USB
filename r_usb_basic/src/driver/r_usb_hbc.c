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
 * File Name    : r_usb_hbc.c
 * Description  : This is the USB host battery charging code.
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 04.01.2014 1.00 First Release
 *         : 30.01.2015 1.01    Added RX71M.
 ***********************************************************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_if.h"
#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    #if USB_CFG_BC == USB_CFG_ENABLE
/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* PD Detect Define */
        #define USB_BC_NODET        (0x00u)
        #define USB_BC_PDDET        (0x01u)

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/
/* variables */
usb_bc_status_t g_usb_hstd_bc[2u];

/* functions */
void usb_hstd_pddetint_process(usb_utr_t *ptr, uint16_t port);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
/* BC State change function */
void usb_hstd_bc_err(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_init_vb(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_det_at(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_cdp_dt(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_sdp_dt(usb_utr_t *ptr, uint16_t port);
/* BC State entry/exit function */
void usb_hstd_bc_det_entry(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_det_exit(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_cdp_entry(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_cdp_exit(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_sdp_entry(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_sdp_exit(usb_utr_t *ptr, uint16_t port);
void usb_hstd_bc_dcp_entry(usb_utr_t *ptr, uint16_t port);

/* BC State change function table */
void (*usb_hstd_bc_func[USB_BC_STATE_MAX][USB_BC_EVENT_MAX])(usb_utr_t *ptr, uint16_t port) =
{
    /* VBUS_ON              ATTACH               DETACH */
    {   usb_hstd_bc_init_vb , usb_hstd_bc_err , usb_hstd_bc_err},
    {   usb_hstd_bc_err , usb_hstd_bc_det_at , usb_hstd_bc_err},
    {   usb_hstd_bc_err , usb_hstd_bc_err , usb_hstd_bc_cdp_dt},
    {   usb_hstd_bc_err , usb_hstd_bc_err , usb_hstd_bc_sdp_dt},
    {   usb_hstd_bc_err , usb_hstd_bc_err , usb_hstd_bc_err}
};

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/
/* variables */

/* functions */
extern void usb_cpu_delay_1us(uint16_t time);

/******************************************************************************
 Renesas Abstracted USB host battery charging driver functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_pddetint_process
 Description     : PDDETINT process
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_pddetint_process(usb_utr_t *ptr, uint16_t port)
{
    uint16_t buf[3];

    /* PDDETSTS chattering cut */
    do
    {
        buf[0] = HW_USB_ReadBcctrl(ptr);
        usb_cpu_delay_1us(10);
        buf[1] = HW_USB_ReadBcctrl(ptr);
        usb_cpu_delay_1us(10);
        buf[2] = HW_USB_ReadBcctrl(ptr);
    }
    while (((buf[0] & USB_PDDETSTS) != (buf[1] & USB_PDDETSTS)) ||
            ((buf[1] & USB_PDDETSTS) != (buf[2] & USB_PDDETSTS)));

    if ((buf[0] & USB_PDDETSTS) == USB_PDDETSTS) /* VDPSRC Detect */
    {
        if ((buf[0] & USB_VDMSRCE) != USB_VDMSRCE)
        {
            HW_USB_SetVdmsrce(ptr);
        }
    }
    else /* VDPSRC Not detect */
    {
        if ((buf[0] & USB_VDMSRCE) == USB_VDMSRCE)
        {
            HW_USB_ClearVdmsrce(ptr);
            g_usb_hstd_bc[ptr->ip].pd_detect = USB_BC_PDDET;
        }
    }
} /* End of function usb_hstd_pddetint_process() */

/******************************************************************************
 Function Name   : usb_hstd_bc_err
 Description     : BC State change function [ERROR]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_err(usb_utr_t *ptr, uint16_t port)
{
    /* Nothing */
} /* End of function usb_hstd_bc_err() */

/******************************************************************************
 Function Name   : usb_hstd_bc_init_vb
 Description     : BC State change function [INIT] [VbusOn]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_init_vb(usb_utr_t *ptr, uint16_t port)
{
    g_usb_hstd_bc[ptr->ip].dcpmode = USB_BC_DCPMODE;

    if (g_usb_hstd_bc[ptr->ip].dcpmode)
    {
        g_usb_hstd_bc[ptr->ip].state = USB_BC_STATE_DCP;
        usb_hstd_bc_dcp_entry(ptr,port);
    }
    else
    {
        g_usb_hstd_bc[ptr->ip].state = USB_BC_STATE_DET;
        usb_hstd_bc_det_entry(ptr,port);
    }
} /* End of function usb_hstd_bc_init_vb() */

/******************************************************************************
 Function Name   : usb_hstd_bc_det_at
 Description     : BC State change function [DET] [Attach]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_det_at(usb_utr_t *ptr, uint16_t port)
{
    usb_hstd_bc_det_exit(ptr,port);

    if (g_usb_hstd_bc[ptr->ip].pd_detect)
    {
        g_usb_hstd_bc[ptr->ip].state = USB_BC_STATE_CDP;
        usb_hstd_bc_cdp_entry(ptr, port);
    }
    else
    {
        g_usb_hstd_bc[ptr->ip].state = USB_BC_STATE_SDP;
        usb_hstd_bc_sdp_entry(ptr,port);
    }
} /* End of function usb_hstd_bc_det_at() */

/******************************************************************************
 Function Name   : usb_hstd_bc_cdp_dt
 Description     : BC State change function [CDP] [Detach]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_cdp_dt(usb_utr_t *ptr, uint16_t port)
{
    usb_hstd_bc_cdp_exit(ptr, port);
    g_usb_hstd_bc[ptr->ip].state = USB_BC_STATE_DET;
    usb_hstd_bc_det_entry(ptr, port);
} /* End of function usb_hstd_bc_cdp_dt() */

/******************************************************************************
 Function Name   : usb_hstd_bc_sdp_dt
 Description     : BC State change function [SDP] [Detach]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_sdp_dt(usb_utr_t *ptr, uint16_t port)
{
    usb_hstd_bc_sdp_exit(ptr, port);
    g_usb_hstd_bc[ptr->ip].state = USB_BC_STATE_DET;
    usb_hstd_bc_det_entry(ptr, port);
} /* End of function usb_hstd_bc_sdp_dt() */

/******************************************************************************
 Function Name   : usb_hstd_bc_det_entry
 Description     : BC State entry function [DET]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_det_entry(usb_utr_t *ptr, uint16_t port)
{
    HW_USB_SetIdpsinke(ptr);
    HW_USB_HClearStsPddetint(ptr);
    HW_USB_HSetEnbPddetinte(ptr);
} /* End of function usb_hstd_bc_det_entry() */

/******************************************************************************
 Function Name   : usb_hstd_bc_det_exit
 Description     : BC State exit function [DET]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_det_exit(usb_utr_t *ptr, uint16_t port)
{
    HW_USB_HSetEnbPddetinte(ptr);
    HW_USB_HClearStsPddetint(ptr);
    HW_USB_ClearIdpsinke(ptr);
} /* End of function usb_hstd_bc_det_exit() */

/******************************************************************************
 Function Name   : usb_hstd_bc_cdp_entry
 Description     : BC State entry function [CDP]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_cdp_entry(usb_utr_t *ptr, uint16_t port)
{
} /* End of function usb_hstd_bc_cdp_entry() */

/******************************************************************************
 Function Name   : usb_hstd_bc_cdp_exit
 Description     : BC State exit function [CDP]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_cdp_exit(usb_utr_t *ptr, uint16_t port)
{
    g_usb_hstd_bc[ptr->ip].pd_detect = USB_BC_NODET;
} /* End of function usb_hstd_bc_cdp_exit() */

/******************************************************************************
 Function Name   : usb_hstd_bc_sdp_entry
 Description     : BC State entry function [SDP]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_sdp_entry(usb_utr_t *ptr, uint16_t port)
{
} /* End of function usb_hstd_bc_sdp_entry() */

/******************************************************************************
 Function Name   : usb_hstd_bc_sdp_exit
 Description     : BC State exit function [SDP]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_sdp_exit(usb_utr_t *ptr, uint16_t port)
{
    /* Nothing */
} /* End of function usb_hstd_bc_sdp_exit() */

/******************************************************************************
 Function Name   : usb_hstd_bc_dcp_entry
 Description     : BC State entry function [DCP]
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_hstd_bc_dcp_entry(usb_utr_t *ptr, uint16_t port)
{
    HW_USB_ClearDrpd(ptr, port);
    HW_USB_HsetDcpmode(ptr);
} /* End of function usb_hstd_bc_dcp_entry() */

    #endif  /* USB_CFG_BC == USB_CFG_ENABLE */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 End of file
 ******************************************************************************/
