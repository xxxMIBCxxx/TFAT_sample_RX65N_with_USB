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
 * File Name    : r_usb_hstdfunction.c
 * Description  : USB Host standard request related functions.
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
#include "r_usb_extern.h"       /* USB register access function */
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if defined(USB_CFG_HCDC_USE)
    #include "r_usb_hcdc_if.h"
#endif /* defined(USB_CFG_HCDC_USE) */

/******************************************************************************
 Constant macro definitions
 ******************************************************************************/
#if USB_CFG_BC == USB_CFG_ENABLE
    #if USB_CFG_DCP == USB_CFG_ENABLE
        #error "You can not define USB_CFG_DCP since USB_CFG_BC is not defined \
                in r_usb_basic_config.h."
    #endif
#endif

#if USB_CFG_COMPLIANCE == USB_CFG_DISABLE
    #if USB_CFG_ELECTRICAL == USB_CFG_ENABLE
        #error "You can not enable USB_CFG_ELECTRICAL in r_usb_basic_config.h \
                when USB_CFG_COMPLIANCE is disabled."
    #endif
#endif

/******************************************************************************
 External variables and functions
 ******************************************************************************/
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
extern void (*g_usb_hstd_enumaration_process[8]) (usb_utr_t *, uint16_t, uint16_t);
extern void usb_registration (usb_utr_t *ptr);
extern void class_driver_start (usb_utr_t *ptr);
extern uint16_t g_usb_cstd_driver_open;

/******************************************************************************
 Renesas Abstracted Host Standard functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_bchg0function
 Description     : Execute the process appropriate to the status of the connected 
 : USB device when a BCHG interrupt occurred.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 Return value    : none
 ******************************************************************************/
void usb_hstd_bchg0function (usb_utr_t *ptr)
{
    uint16_t buf, connect_inf;

    /* SUSPENDED check */
    if (USB_SUSPENDED == g_usb_hstd_remort_port[USB_PORT0])
    {
        /* Device State Control Register - Resume enable check */
        buf = HW_USB_ReadDvstctr(ptr, USB_PORT0);

        if ((uint16_t) (buf & USB_RESUME) == USB_RESUME)
        {
            USB_PRINTF0("remote wakeup port0\n");
            g_usb_hstd_remort_port[USB_PORT0] = USB_DEFAULT;

            /* Change device state to resume */
            usb_hstd_device_resume(ptr, (uint16_t) (USB_PORT0 + USB_DEVICEADDR));
        }
        else
        {
            /* Decide USB Line state (ATTACH) */
            connect_inf = usb_hstd_chk_attach(ptr, (uint16_t) USB_PORT0);
            if (USB_DETACH == connect_inf)
            {
                g_usb_hstd_remort_port[USB_PORT0] = USB_DEFAULT;

                /* USB detach process */
                usb_hstd_detach_process(ptr, (uint16_t) USB_PORT0);
            }
            else
            {
                /* Enable port BCHG interrupt */
                usb_hstd_bchg_enable(ptr, (uint16_t) USB_PORT0);

                /* Check clock */
                usb_hstd_chk_clk(ptr, (uint16_t) USB_PORT0, (uint16_t) USB_SUSPENDED);
            }
        }
    }
    else
    {
        /* USB detach process */
        usb_hstd_detach_process(ptr, (uint16_t) USB_PORT0);
    }
}
/******************************************************************************
 End of function usb_hstd_bchg0function
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_ls_connect_function
 Description     : Low-speed device connect.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_hstd_ls_connect_function (usb_utr_t *ptr)
{
    (*g_usb_hstd_enumaration_process[0])(ptr, (uint16_t) USB_DEVICE_0, (uint16_t) 0);
}
/******************************************************************************
 End of function usb_hstd_ls_connect_function
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_attach_function
 Description     : Device attach.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_hstd_attach_function (void)
{
    /* 100ms wait */
    usb_cpu_delay_xms((uint16_t) 100);
}
/******************************************************************************
 End of function usb_hstd_attach_function
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_ovrcr0function
 Description     : Set USB registers as required due to an OVRCR (over-current)
 : interrupt, and notify the MGR (manager) task about this.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 Return value    : none
 ******************************************************************************/
void usb_hstd_ovrcr0function (usb_utr_t *ptr)
{
    /* Over-current bit check */
    USB_PRINTF0(" OVCR int port0\n");

    /* OVRCR interrupt disable */
    /* Notification over current */
    usb_hstd_ovcr_notifiation(ptr, (uint16_t) USB_PORT0);
}
/******************************************************************************
 End of function usb_hstd_ovrcr0function
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_enum_function1
 Description     : Device enumeration function nr 1.
 Arguments       : none
 Return value    : uint16_t              : USB_OK
 ******************************************************************************/
uint16_t usb_hstd_enum_function1 (void)
{
    return USB_OK;
}
/******************************************************************************
 End of function usb_hstd_enum_function1
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_enum_function2
 Description     : Device enumeration function nr 2.
 Arguments       : uint16_t *enummode    : Enumeration mode.
 Return value    : uint16_t              : USB_TRUE
 ******************************************************************************/
uint16_t usb_hstd_enum_function2 (uint16_t* enummode)
{
    return USB_TRUE;
}
/******************************************************************************
 End of function usb_hstd_enum_function2
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_enum_function4
 Description     : Device enumeration function nr 4.
 Arguments       : uint16_t *reqnum      : Request number.
 : uint16_t *enummode    : 
 : uint16_t devaddr      : Device address.
 Return value    : none
 ******************************************************************************/
void usb_hstd_enum_function4 (uint16_t* reqnum, uint16_t* enummode, uint16_t devaddr)
{
    /* Non */
}
/******************************************************************************
 End of function usb_hstd_enum_function4
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_enum_function5
 Description     : Device enumeration function nr 5.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_hstd_enum_function5 (void)
{
    USB_PRINTF0(" Get_DeviceDescrip(8-2)\n");
}
/******************************************************************************
 End of function usb_hstd_enum_function5
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hdriver_init
 Description     : 
 Arguments       : 
 Return value    : none
 ******************************************************************************/
void usb_hdriver_init (usb_utr_t *ptr, usb_cfg_t *cfg)
{
    uint16_t i;

    if ( USB_OK != g_usb_cstd_driver_open)
    {
        g_usb_cstd_event.write_pointer = USB_NULL; /* Write pointer */
        g_usb_cstd_event.read_pointer = USB_NULL; /* Read pointer */
        for (i = 0; i < USB_EVENT_MAX; i++)
        {
            g_usb_cstd_event.code[i] = USB_STS_NONE;
            g_usb_cstd_event.ctrl[i].address = USB_NULL;
        }
    }

    g_usb_cstd_hs_enable[0] = USB_HS_DISABLE;
    g_usb_cstd_hs_enable[1] = USB_HS_DISABLE;
    if (USB_HS == cfg->usb_speed)
    {
        g_usb_cstd_hs_enable[1] = USB_HS_ENABLE;
    }

    usb_cstd_sche_init(); /* Scheduler init */
    usb_hstd_init_usb_message(ptr); /* USB interrupt message initialize */

    usb_hstd_mgr_open(ptr); /* Manager open */
    usb_hstd_hcd_open(ptr); /* Hcd open */
    #if defined(USB_CFG_HCDC_USE) || defined(USB_CFG_HHID_USE) || defined(USB_CFG_HMSC_USE) || defined(USB_CFG_HVND_USE)
    class_driver_start(ptr); /* Init host class driver task. */
    usb_registration(ptr); /* Class Registration */

    #endif  /* defined(USB_CFG_HCDC_USE)||defined(USB_CFG_HHID_USE)||defined(USB_CFG_HMSC_USE)||defined(USB_CFG_HVND_USE) */
} /* End of function usb_hdriver_init() */

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 End  Of File
 ******************************************************************************/
