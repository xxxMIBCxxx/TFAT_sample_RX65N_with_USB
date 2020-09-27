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
 * File Name    : r_usb_clibusbip.c
 * Description  : USB IP Host and Peripheral low level library
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
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

#if defined(USB_CFG_HMSC_USE)
    #include "r_usb_hmsc_if.h"
extern uint8_t drive_search_lock;
#endif /* defined(USB_CFG_HMSC_USE) */

#if defined(USB_CFG_PMSC_USE)
    #include "r_usb_pmsc_if.h"
extern void usb_pmsc_task(void);
#endif /* defined(USB_CFG_PMSC_USE) */

#if defined(USB_CFG_PCDC_USE)
    #include "r_usb_pcdc_if.h"
//extern void usb_pcdc_Task(void);
#endif /* defined(USB_CFG_PCDC_USE) */

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
    #include "r_usb_dmac.h"
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

/******************************************************************************
 External variables and functions
 ******************************************************************************/
extern uint16_t g_usb_usbmode;
extern void usb_class_task (void);

/******************************************************************************
 Renesas Abstracted Driver API functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_nrdy_enable
 Description     : Enable NRDY interrupt of the specified pipe.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : none
 ******************************************************************************/
void usb_cstd_nrdy_enable (usb_utr_t *ptr, uint16_t pipe)
{
    /* Enable NRDY */
    HW_USB_SetNrdyenb(ptr, pipe);
}
/******************************************************************************
 End of function usb_cstd_nrdy_enable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_get_pid
 Description     : Fetch specified pipe's PID.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : uint16_t PID-bit status
 ******************************************************************************/
uint16_t usb_cstd_get_pid (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t buf;

    /* PIPE control reg read */
    buf = HW_USB_ReadPipectr(ptr, pipe);
    return (uint16_t) (buf & USB_PID);
}
/******************************************************************************
 End of function usb_cstd_get_pid
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_get_maxpacket_size
 Description     : Fetch MaxPacketSize of the specified pipe.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : uint16_t MaxPacketSize
 ******************************************************************************/
uint16_t usb_cstd_get_maxpacket_size (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t size;
    uint16_t buffer;

    if (USB_PIPE0 == pipe)
    {
        buffer = HW_USB_ReadDcpmaxp(ptr);
    }
    else
    {
        /* Pipe select */
        HW_USB_WritePipesel(ptr, pipe);
        buffer = HW_USB_ReadPipemaxp(ptr);
    }

    /* Max Packet Size */
    size = (uint16_t) (buffer & USB_MXPS);

    return size;
}
/******************************************************************************
 End of function usb_cstd_get_maxpacket_size
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_get_pipe_dir
 Description     : Get PIPE DIR
 Arguments       : uint16_t pipe  : Pipe number.
 Return value    : uint16_t pipe direction.
 ******************************************************************************/
uint16_t usb_cstd_get_pipe_dir (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t buffer;

    /* Pipe select */
    HW_USB_WritePipesel(ptr, pipe);

    /* Read Pipe direction */
    buffer = HW_USB_ReadPipecfg(ptr);
    return (uint16_t) (buffer & USB_DIRFIELD);
}
/******************************************************************************
 End of function usb_cstd_get_pipe_dir
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_get_pipe_type
 Description     : Fetch and return PIPE TYPE.
 Arguments       : uint16_t pipe            : Pipe number.
 Return value    : uint16_t pipe type
 ******************************************************************************/
uint16_t usb_cstd_get_pipe_type (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t buffer;

    /* Pipe select */
    HW_USB_WritePipesel(ptr, pipe);

    /* Read Pipe direction */
    buffer = HW_USB_ReadPipecfg(ptr);
    return (uint16_t) (buffer & USB_TYPFIELD);
}
/******************************************************************************
 End of function usb_cstd_get_pipe_type
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_do_aclrm
 Description     : Set the ACLRM-bit (Auto Buffer Clear Mode) of the specified 
 : pipe.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : none
 ******************************************************************************/
void usb_cstd_do_aclrm (usb_utr_t *ptr, uint16_t pipe)
{
    /* Control ACLRM */
    HW_USB_SetAclrm(ptr, pipe);
    HW_USB_ClearAclrm(ptr, pipe);
}
/******************************************************************************
 End of function usb_cstd_do_aclrm
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_set_buf
 Description     : Set PID (packet ID) of the specified pipe to BUF.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : none
 ******************************************************************************/
void usb_cstd_set_buf (usb_utr_t *ptr, uint16_t pipe)
{
    /* PIPE control reg set */
    HW_USB_SetPid(ptr, pipe, USB_PID_BUF);
}
/******************************************************************************
 End of function usb_cstd_set_buf
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_clr_stall
 Description     : Set up to NAK the specified pipe, and clear the STALL-bit set
 : to the PID of the specified pipe.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : none
 Note            : PID is set to NAK.
 ******************************************************************************/
void usb_cstd_clr_stall (usb_utr_t *ptr, uint16_t pipe)
{
    /* Set NAK */
    usb_cstd_set_nak(ptr, pipe);

    /* Clear STALL */
    HW_USB_ClearPid(ptr, pipe, USB_PID_STALL);
}
/******************************************************************************
 End of function usb_cstd_clr_stall
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_port_speed
 Description     : Get USB-speed of the specified port.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t port  : Root port
 Return value    : uint16_t       : HSCONNECT : Hi-Speed
 :                : FSCONNECT : Full-Speed
 :                : LSCONNECT : Low-Speed
 :                : NOCONNECT : not connect
 ******************************************************************************/
uint16_t usb_cstd_port_speed (usb_utr_t *ptr, uint16_t port)
{
    uint16_t buf;
    uint16_t conn_inf;

    buf = HW_USB_ReadDvstctr(ptr, port);

    /* Reset handshake status get */
    buf = (uint16_t) (buf & USB_RHST);

    switch (buf)
    {
        /* Get port speed */
        case USB_HSMODE :
            conn_inf = USB_HSCONNECT;
        break;
        case USB_FSMODE :
            conn_inf = USB_FSCONNECT;
        break;
        case USB_LSMODE :
            conn_inf = USB_LSCONNECT;
        break;
        case USB_HSPROC :
            conn_inf = USB_NOCONNECT;
        break;
        default :
            conn_inf = USB_NOCONNECT;
        break;
    }

    return (conn_inf);
}
/******************************************************************************
 End of function usb_cstd_port_speed
 ******************************************************************************/

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : usb_hstd_pipe_to_epadr
 Description     : Get the associated endpoint value of the specified pipe.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : uint8_t        : OK    : Endpoint nr + direction.
 :                : ERROR : Error.
 ******************************************************************************/
uint8_t usb_hstd_pipe_to_epadr (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t buffer;
    uint16_t direp;

    /* Pipe select */
    HW_USB_WritePipesel(ptr, pipe);

    /* Read Pipe direction */
    buffer = HW_USB_ReadPipecfg(ptr);
    direp = (uint16_t) ((((buffer & USB_DIRFIELD) ^ USB_DIRFIELD) << 3) + (buffer & USB_EPNUMFIELD));
    return (uint8_t) (direp);
}
/******************************************************************************
 End of function usb_hstd_pipe_to_epadr
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_pipe2fport
 Description     : Get port No. from the specified pipe No. by argument
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : uint16_t       : FIFO port selector.
 ******************************************************************************/
uint16_t usb_hstd_pipe2fport (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t fifo_mode = USB_CUSE;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE

    if (USB_PIPE1 == pipe)
    {
        fifo_mode = USB_D0DMA;
    }
    if (USB_PIPE2 == pipe)
    {
        fifo_mode = USB_D1DMA;
    }

    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    return fifo_mode;
}
/******************************************************************************
 End of function usb_hstd_pipe2fport
 ******************************************************************************/

    #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/******************************************************************************
 Function Name   : usb_hstd_set_hse
 Description     : Set/clear the HSE-bit of the specified port.
 Arguments       : uint16_t port     : Root port.
 : uint16_t speed    : HS_ENABLE/HS_DISABLE.
 Return value    : none
 ******************************************************************************/
void usb_hstd_set_hse(usb_utr_t *ptr, uint16_t port, uint16_t speed)
{
    if (speed == USB_HS_DISABLE)
    {
        /* HSE = disable */
        HW_USB_ClearHse(ptr, port);
    }
    else
    {
        /* HSE = enable */
        HW_USB_SetHse(ptr, port);
    }
}
/******************************************************************************
 End of function usb_hstd_set_hse
 ******************************************************************************/
    #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

/******************************************************************************
 Function Name   : usb_hstd_dummy_function
 Description     : dummy function
 Arguments       : uint16_t data1    : Not used.
 : uint16_t data2    : Not used.
 Return value    : none
 ******************************************************************************/
void usb_hstd_dummy_function (usb_utr_t *ptr, uint16_t data1, uint16_t data2)
{
    /* Non */
}
/******************************************************************************
 End of function usb_hstd_dummy_function
 ******************************************************************************/
/******************************************************************************
 Function Name   : usb_hstd_berne_enable
 Description     : Enable BRDY/NRDY/BEMP interrupt.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 Return value    : none
 ******************************************************************************/
void usb_hstd_berne_enable (usb_utr_t *ptr)
{
    /* Enable BEMP, NRDY, BRDY */
    HW_USB_SetIntenb(ptr, (USB_BEMPE | USB_NRDYE | USB_BRDYE));
}
/******************************************************************************
 End of function usb_hstd_berne_enable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_sw_reset
 Description     : Request USB IP software reset
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 Return value    : none
 ******************************************************************************/
void usb_hstd_sw_reset (usb_utr_t *ptr)
{
    /* USB Enable */
    HW_USB_SetUsbe(ptr);

    /* USB Reset */
    HW_USB_ClearUsbe(ptr);

    /* USB Enable */
    HW_USB_SetUsbe(ptr);
}
/******************************************************************************
 End of function usb_hstd_sw_reset
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_do_sqtgl
 Description     : Toggle setting of the toggle-bit for the specified pipe by 
 : argument.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe     : Pipe number.
 : uint16_t toggle   : Current toggle status.
 Return value    : none
 ******************************************************************************/
void usb_hstd_do_sqtgl (usb_utr_t *ptr, uint16_t pipe, uint16_t toggle)
{
    /* Check toggle */
    if ((toggle & USB_SQMON) == USB_SQMON)
    {
        /* Do pipe SQSET */
        HW_USB_SetSqset(ptr, pipe);
    }
    else
    {
        /* Do pipe SQCLR */
        HW_USB_SetSqclr(ptr, pipe);
    }
}
/******************************************************************************
 End of function usb_hstd_do_sqtgl
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_get_devsel
 Description     : Get device address from pipe number
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : uint16_t DEVSEL-bit status
 ******************************************************************************/
uint16_t usb_hstd_get_devsel (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t devsel;
    uint16_t buffer;

    if (USB_PIPE0 == pipe)
    {
        buffer = HW_USB_ReadDcpmaxp(ptr);
    }
    else
    {
        /* Pipe select */
        HW_USB_WritePipesel(ptr, pipe);
        buffer = HW_USB_ReadPipemaxp(ptr);
    }

    /* Device address */
    devsel = (uint16_t) (buffer & USB_DEVSEL);

    return devsel;
}
/******************************************************************************
 End of function usb_hstd_get_devsel
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_get_device_address
 Description     : Get the device address associated with the specified pipe.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe number.
 Return value    : uint16_t DEVSEL-bit status
 ******************************************************************************/
uint16_t usb_hstd_get_device_address (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t buffer;
    uint16_t i;
    uint16_t md;
    usb_hcdreg_t *pdriver;

    /* Host */
    if (USB_PIPE0 == pipe)
    {
        buffer = HW_USB_ReadDcpmaxp(ptr);

        /* Device address */
        return (uint16_t) (buffer & USB_DEVSEL);
    }
    else
    {
        for (md = 0; md < g_usb_hstd_device_num[ptr->ip]; md++)
        {
            if ((USB_IFCLS_NOT != g_usb_hstd_device_drv[ptr->ip][md].ifclass)
                    && (USB_NODEVICE != g_usb_hstd_device_drv[ptr->ip][md].devaddr))
            {
                pdriver = (usb_hcdreg_t*) &g_usb_hstd_device_drv[ptr->ip][md];

                /* EP table loop */
                for (i = 0; pdriver->pipetbl[i] != USB_PDTBLEND; i += USB_EPL)
                {
                    if (pdriver->pipetbl[i] == pipe)
                    {
                        buffer = pdriver->pipetbl[i + 3];

                        /* Device address */
                        return (uint16_t) (buffer & USB_DEVSEL);
                    }
                }
            }
        }
    }

    return USB_ERROR;
}
/******************************************************************************
 End of function usb_hstd_get_device_address
 ******************************************************************************/
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : usb_pstd_epadr2pipe
 Description     : Get the associated pipe no. of the specified endpoint.
 Arguments       : uint16_t dir_ep : Direction + endpoint number.
 Return value    : uint16_t        : OK    : Pipe number.
 :                 : ERROR : Error.
 ******************************************************************************/
uint16_t usb_pstd_epadr2pipe(uint16_t dir_ep)
{
    uint16_t i;
    uint16_t direp;
    uint16_t tmp;
    uint16_t * ptable;

    /* Peripheral */
    /* Get PIPE Number from Endpoint address */
    ptable = g_usb_pstd_driver.pipetbl;
    direp = (uint16_t)(((dir_ep & 0x80) >> 3) | (dir_ep & 0x0F));

    /* EP table loop */
    for (i = 0; ptable[i] != USB_PDTBLEND; i += USB_EPL)
    {
        tmp = (uint16_t)(ptable[i + 1] & (USB_DIRFIELD | USB_EPNUMFIELD));

        /* EP table endpoint dir check */
        if (direp == tmp)
        {
            return ptable[i];
        }
    }

    return USB_ERROR;
}
/******************************************************************************
 End of function usb_pstd_epadr2pipe
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_pipe2fport
 Description     : Get port No. from the specified pipe No. by argument
 Arguments       : uint16_t pipe  : Pipe number.
 Return value    : uint16_t       : FIFO port selector.
 ******************************************************************************/
uint16_t usb_pstd_pipe2fport(uint16_t pipe)
{
    uint16_t fifo_mode = USB_CUSE;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE

    if (USB_PIPE1 == pipe)
    {
        fifo_mode = USB_D0DMA;
    }
    if (USB_PIPE2 == pipe)
    {
        fifo_mode = USB_D1DMA;
    }

    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    return fifo_mode;
}
/******************************************************************************
 End of function usb_pstd_pipe2fport
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_hi_speed_enable
 Description     : Check if set to Hi-speed.
 Arguments       : none
 Return value    : uint16_t       : YES; Hi-Speed enabled.
 :                : NO; Hi-Speed disabled.
 ******************************************************************************/
uint16_t usb_pstd_hi_speed_enable(void)
{
    uint16_t buf;

    buf = HW_USB_ReadSyscfg(USB_NULL, USB_NULL);

    if ((buf & USB_HSE) == USB_HSE)
    {
        /* Hi-Speed Enable */
        return USB_TRUE;
    }
    else
    {
        /* Hi-Speed Disable */
        return USB_FALSE;
    }
}
/******************************************************************************
 End of function usb_pstd_hi_speed_enable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_dummy_function
 Description     : dummy function
 Arguments       : usb_utr_t *ptr
 Return value    : none
 ******************************************************************************/
void usb_pstd_dummy_function(usb_utr_t *ptr, uint16_t data1, uint16_t data2)
{
    /* Dummy function */
}
/******************************************************************************
 End of function usb_pstd_dummy_function
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_dummy_trn
 Description     : Class request processing for dummy
 Arguments       : usb_setup_t *preq  ; Class request information
 : uint16_t ctsq        ; Control Stage
 Return value    : none
 ******************************************************************************/
void usb_pstd_dummy_trn(usb_setup_t * preq, uint16_t ctsq)
{
    /* Dummy function */
} /* End of function usb_pstd_dummy_trn */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */

/******************************************************************************
 Function Name   : usb_set_event
 Description     : Set event.
 Arguments       : uint16_t event    : event code.
 : usb_ctrl_t *ctrl  :control structure for USB API.
 Return value    : none
 ******************************************************************************/
void usb_set_event (uint16_t event, usb_ctrl_t *ctrl)
{
    g_usb_cstd_event.code[g_usb_cstd_event.write_pointer] = event;
    g_usb_cstd_event.ctrl[g_usb_cstd_event.write_pointer] = *ctrl;
    g_usb_cstd_event.write_pointer++;
    if (g_usb_cstd_event.write_pointer >= USB_EVENT_MAX)
    {
        g_usb_cstd_event.write_pointer = 0;
    }
} /* End of function usb_set_event() */

/******************************************************************************
 Function Name   : usb_cstd_usb_task
 Description     : USB driver main loop processing.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_cstd_usb_task (void)
{
    if ( USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    #if defined(USB_CFG_HMSC_USE)
        do
        {
    #endif /* defined(USB_CFG_HMSC_USE) */
        usb_cstd_scheduler(); /* Scheduler */
        if (USB_FLGSET == usb_cstd_check_schedule()) /* Check for any task processing requests flags. */
        {
            usb_hstd_hcd_task((usb_vp_int_t) 0); /* HCD Task */
            usb_hstd_mgr_task((usb_vp_int_t) 0); /* MGR Task */
            usb_hhub_task((usb_vp_int_t) 0); /* HUB Task */
    #if defined(USB_CFG_HCDC_USE) || defined(USB_CFG_HHID_USE) || defined(USB_CFG_HMSC_USE) || defined(USB_CFG_HVND_USE)

            usb_class_task();

    #endif  /* defined(USB_CFG_HCDC_USE)||defined(USB_CFG_HHID_USE)||defined(USB_CFG_HMSC_USE)||defined(USB_CFG_HVND_USE) */
        }
    #if defined(USB_CFG_HMSC_USE)
    }
    while(USB_FALSE != drive_search_lock);
    #endif /* defined(USB_CFG_HMSC_USE) */
#endif
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        usb_pstd_pcd_task();
    #if defined(USB_CFG_PMSC_USE)
        usb_pmsc_task();
    #endif /* defined(USB_CFG_PMSC_USE) */
#endif
    }
} /* End of function usb_cstd_usb_task() */

/******************************************************************************
 End  Of File
 ******************************************************************************/
