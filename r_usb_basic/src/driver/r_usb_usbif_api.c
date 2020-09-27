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
* File Name    : r_usb_usbif_api.c
* Description  : USB Host and Peripheral Driver API code. HCD(Host Control Driver) PCD (Peripheral Control Driver)
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

#if defined(USB_CFG_HCDC_USE)
#include "r_usb_hcdc_if.h"
#endif /* defined(USB_CFG_HCDC_USE) */

#if defined(USB_CFG_HHID_USE)
#include "r_usb_hhid_if.h"
#endif /* defined(USB_CFG_HHID_USE) */

#if defined(USB_CFG_HMSC_USE)
#include "r_usb_hmsc_if.h"
#endif /* defined(USB_CFG_HMSC_USE) */

#if defined(USB_CFG_HVND_USE)
#include "r_usb_vendor.h"
#endif /* defined(USB_CFG_HVND_USE) */

#if defined(USB_CFG_PHID_USE)
#include "r_usb_phid_if.h"
#endif /* defined(USB_CFG_PHID_USE) */

#if defined(USB_CFG_PCDC_USE)
#include "r_usb_pcdc_if.h"
#endif /* defined(USB_CFG_PCDC_USE) */

/******************************************************************************
External variables and functions
******************************************************************************/
extern void         usb_cstd_usb_task(void);
extern void         HW_USB_hmodule_init(usb_ctrl_t *p_ctrl);
extern void         usb_hdriver_init( usb_utr_t *ptr, usb_cfg_t *cfg );
extern void         HW_USB_pmodule_init( void );
extern void         usb_pdriver_init( usb_ctrl_t *ctrl, usb_cfg_t *cfg  );

#if defined(USB_CFG_HMSC_USE)
extern void         usb_hmsc_strg_cmd_complete(usb_utr_t *mess, uint16_t devadr, uint16_t data2);
#endif /* defined(USB_CFG_HMSC_USE) */

#if defined(USB_CFG_PVND_USE)
extern void         usb_pvnd_read_complete(usb_utr_t *mess, uint16_t data1, uint16_t data2);
extern void         usb_pvnd_write_complete(usb_utr_t *mess, uint16_t data1, uint16_t data2);
#endif /* defined(USB_CFG_PVND_USE) */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
extern uint16_t     g_usb_hstd_use_pipe[];
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
Private global variables and functions
******************************************************************************/
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
const uint8_t g_usb_pipe_peri[] =
{
    /* OUT pipe */          /* IN pipe */
#if defined(USB_CFG_PCDC_USE)
    USB_CFG_PCDC_BULK_OUT,  USB_CFG_PCDC_BULK_IN,   /* USB_PCDC */
    USB_NULL,               USB_CFG_PCDC_INT_IN,    /* USB_PCDCC */
#else
    USB_NULL,               USB_NULL,
    USB_NULL,               USB_NULL,
#endif

#if defined(USB_CFG_PHID_USE)
    USB_CFG_PHID_INT_OUT,   USB_CFG_PHID_INT_IN,    /* USB_PHID */
#else
    USB_NULL,               USB_NULL,
#endif
};
#endif

uint32_t        g_usb_read_request_size[USB_MAXPIPE_NUM+1];
usb_event_t     g_usb_cstd_event;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
usb_utr_t       g_usb_hdata[USB_NUM_USBIP][USB_MAXPIPE_NUM + 1];
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
usb_utr_t       g_usb_pdata[USB_MAXPIPE_NUM + 1];
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */

uint16_t        g_usb_usbmode;
uint16_t        g_usb_cstd_driver_open;

#if (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI
uint16_t        g_usb_bc_detect;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */

/******************************************************************************
Renesas Abstracted Peripheral Driver API functions
******************************************************************************/

/*****************************************************************************
* Function Name: R_USB_GetVersion
* Description  : Returns the version of this module. The version number is
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.
* Arguments    : none
* Return Value : version number
******************************************************************************/
#pragma inline(R_USB_GetVersion)
uint32_t R_USB_GetVersion(void)
{
    uint32_t version = 0;

    version = (USB_VERSION_MAJOR << 16) | USB_VERSION_MINOR;

    return version;
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : R_USB_GetEvent
Description     : Get event.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : event code.
******************************************************************************/
uint16_t R_USB_GetEvent(usb_ctrl_t *p_ctrl)
{
    uint16_t    event = USB_STS_NONE;

    usb_cstd_usb_task();
    if ( g_usb_cstd_event.write_pointer != g_usb_cstd_event.read_pointer )
    {
        *p_ctrl = g_usb_cstd_event.ctrl[g_usb_cstd_event.read_pointer];
        event = g_usb_cstd_event.code[g_usb_cstd_event.read_pointer];
        g_usb_cstd_event.read_pointer++;
        if( g_usb_cstd_event.read_pointer >= USB_EVENT_MAX )
        {
            g_usb_cstd_event.read_pointer = 0;
        }
    }
    return event;
}  /* End of function R_USB_GetEvent() */

/******************************************************************************
Function Name   : R_USB_Open
Description     : Start of USB Driver.
Arguments       : usb_ctrl_t *p_ctrl    : control structure for USB API.
                : usb_cfg_t *cfg        : configuration structure for USB API.
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_Open( usb_ctrl_t *ctrl, usb_cfg_t *cfg )
{
    usb_err_t   err;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    /* Argument Checking */
    if ( ( USB_IP0 != ctrl->module ) && ( USB_IP1 != ctrl->module ) )
    {
        return USB_ERR_PARA;
    }

    if ( ( USB_HS != cfg->usb_speed ) && ( USB_FS != cfg->usb_speed ) && ( USB_LS != cfg->usb_speed ))
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == ctrl->module )
    {
        return USB_ERR_PARA;
    }
#endif

#if defined(BSP_MCU_RX71M)
#else
    if ( USB_HS == cfg->usb_speed )
    {
        return USB_ERR_PARA;
    }
#endif

    switch (ctrl->type)
    {
        case USB_PCDC:
        case USB_PCDCC:
        case USB_PHID:
        case USB_PVND:
        case USB_PMSC:
            g_usb_usbmode = USB_PERI;
        break;

        /* Host */
        case USB_HCDC:
        case USB_HCDCC:
        case USB_HHID:
        case USB_HVND:
        case USB_HMSC:
            g_usb_usbmode = USB_HOST;
        break;

        default:
            return USB_ERR_PARA;
        break;
    }

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip  = ctrl->module;
        utr.ipp = usb_hstd_get_usb_ip_adr( (uint8_t)ctrl->module  );    /* Get the USB IP base address. */
        /* USB driver initialization */
        usb_hdriver_init( &utr, cfg );
        err = usb_module_start( (uint8_t)utr.ip );
        if( USB_SUCCESS == err )
        {
            /* Setting MCU(USB interrupt init) register */
            usb_cpu_usbint_init((uint8_t)utr.ip);

            /* Setting USB relation register  */
            HW_USB_hmodule_init(ctrl); /* MCU */
            usb_hstd_vbus_control(&utr, USB_PORT0, (uint16_t)USB_VBON);
#if USB_CFG_BC == USB_CFG_DISABLE
            usb_cpu_delay_xms((uint16_t)100);    /* 100ms wait */
#endif /*  USB_CFG_BC == USB_CFG_DISABLE */
            g_usb_cstd_driver_open = USB_OK;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        if ( (USB_LS == cfg->usb_speed) || (USB_NULL == cfg->usb_reg) )
        {
            return USB_ERR_PARA;
        }

        /* USB driver initialization */
        usb_pdriver_init( ctrl, cfg );
        /* USB module start setting */
        err = usb_module_start(USB_CFG_USE_USBIP);
        if( USB_SUCCESS == err )
        {
            /* Setting USB relation register  */
            HW_USB_pmodule_init();
            /* Setting MCU(USB interrupt init) register */
            usb_cpu_usbint_init(USB_CFG_USE_USBIP);
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            if( USB_HS == cfg->usb_speed )
            {
                HW_USB_SetHse(USB_NULL, USB_NULL);
            }
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
            if ( USB_ATTACH == usb_pstd_chk_vbsts() )
            {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                HW_USB_SetCnen();
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                usb_cpu_delay_xms((uint16_t)10);
                HW_USB_PSetDprpu();
            }
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    return err;
} /* End of function R_USB_Open() */


/******************************************************************************
Function Name   : R_USB_Close
Description     : End of USB Driver.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_Close(usb_ctrl_t *p_ctrl)
{
    usb_err_t   ret_code;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    ret_code = usb_module_stop(p_ctrl->module);
    if (USB_SUCCESS == ret_code)
    {
        if (USB_HOST == g_usb_usbmode)
        {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
            utr.ip = p_ctrl->module;
            utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
            switch(p_ctrl->type)
            {
            case USB_HCDC:
                usb_hstd_driver_release(&utr, (uint8_t)USB_IFCLS_CDC);
                break;
            case USB_HHID:
                usb_hstd_driver_release(&utr, (uint8_t)USB_IFCLS_HID);
                break;
            case USB_HMSC:
                usb_hstd_driver_release(&utr, (uint8_t)USB_IFCLS_MAS);
                break;
            case USB_HVND:
                usb_hstd_driver_release(&utr, (uint8_t)USB_IFCLS_VEN);
                break;
            default:
                ret_code = USB_ERR_PARA;
                break;
            }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
        }
        else
        {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )

        usb_pstd_driver_release();   /* Clear the information registered in the structure usb_pcdreg_t. */

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
        }
    }

    return ret_code;
} /* End of function R_USB_Close() */


/******************************************************************************
Function Name   : R_USB_Read
Description     : USB Data Receive process.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : uint8_t *buf      : transfer data address
                : uint32_t size     : transfer length
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_Read(usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
#if defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE)
    return USB_ERR_NG;
#else /* defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE) */
    usb_info_t  info;
    usb_er_t    err;

    if ((USB_NULL == p_ctrl) || (USB_NULL == buf) || (USB_NULL == size))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    if ((USB_PCDCC == p_ctrl->type) || (USB_PVND == p_ctrl->type) ||
            (USB_PMSC == p_ctrl->type) || (USB_HMSC == p_ctrl->type) || (USB_HVND == p_ctrl->type))
    {
        return USB_ERR_PARA;
    }

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    if( USB_REQUEST == p_ctrl->type )
    {
        err = usb_ctrl_read( p_ctrl, buf, size );
    }
    else
    {
        err = usb_data_read( p_ctrl, buf, size );
    }

    if( USB_OK == err )
    {
        return USB_SUCCESS;
    }
    else if( USB_QOVR == err )
    {
        return USB_ERR_BUSY;
    }
    else
    {
        /* Noting */
    }

    return USB_ERR_NG;
#endif /* defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE) */
} /* End of function R_USB_Read() */

/******************************************************************************
Function Name   : R_USB_Write
Description     : USB Data send process.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : uint8_t *buf      : transfer data address
                : uint32_t size     : transfer length
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_Write(usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
#if defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE)
    return USB_ERR_NG;
#else /* defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE) */
    usb_info_t  info;
    usb_er_t    err;


    if ((USB_NULL == p_ctrl) || ((USB_NULL == buf) && (0 != size)))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    if ((USB_HCDCC == p_ctrl->type) || (USB_PVND == p_ctrl->type) ||
            (USB_PMSC == p_ctrl->type) || (USB_HMSC == p_ctrl->type) || (USB_HVND == p_ctrl->type))
    {
        return USB_ERR_PARA;
    }

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    if( USB_REQUEST == p_ctrl->type )
    {
        err = usb_ctrl_write( p_ctrl, buf, size );
    }
    else
    {
        err = usb_data_write( p_ctrl, buf, size );
    }

    if (USB_OK == err)
    {
        return USB_SUCCESS;
    }
    else if(USB_QOVR == err)
    {
        return USB_ERR_BUSY;
    }
    else
    {
        /* error */
    }

    return USB_ERR_NG;
#endif /* defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE) */
} /* End of function R_USB_Write() */

/******************************************************************************
Function Name   : R_USB_Stop
Description     : USB Transfer stop
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : uint16_t type     : Read(0)/Write(1)
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_Stop(usb_ctrl_t *p_ctrl, uint16_t type)
{
#if defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE)
    return USB_ERR_NG;
#else /* defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE) */
    usb_info_t  info;
    usb_er_t    err;

    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    if ((USB_WRITE != type) && (USB_READ != type))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    if ((USB_PVND == p_ctrl->type ) || (USB_HVND == p_ctrl->type))
    {
        return USB_ERR_NG;
    }

    if ((USB_PMSC == p_ctrl->type ) || (USB_HMSC == p_ctrl->type))
    {
        return USB_ERR_PARA;
    }

    if (((USB_HCDCC == p_ctrl->type) && (USB_WRITE == type))   ||
        ((USB_PCDCC == p_ctrl->type ) && (USB_READ == type)))
    {
        return USB_ERR_PARA;
    }

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    if( USB_REQUEST == p_ctrl->type )
    {
        err = usb_ctrl_stop( p_ctrl);
    }
    else
    {
        err = usb_data_stop( p_ctrl, type);
    }

    if (USB_OK == err)
    {
        return USB_SUCCESS;
    }
    else if(USB_QOVR == err)
    {
        return USB_ERR_BUSY;
    }
    else
    {
        /* error */
    }

    return USB_ERR_NG;
#endif /* defined(USB_CFG_HMSC_USE) || defined(USB_CFG_PMSC_USE) */
} /* End of function R_USB_Stop() */

/******************************************************************************
Function Name   : R_USB_Suspend
Description     : USB Suspend process for USB Host.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_Suspend(usb_ctrl_t *p_ctrl)
{
    usb_err_t   ret_code = USB_SUCCESS;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
    usb_info_t  info;
#if defined(USB_CFG_HHID_USE)
    usb_er_t    err;
#else   /* defined(USB_CFG_HHID_USE) */
    uint16_t    j;
#endif  /* defined(USB_CFG_HHID_USE) */

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    if (USB_PERI == g_usb_usbmode)
    {
        return USB_ERR_NG;              /* Support Host only. */
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    if ((0 == p_ctrl->address) || (USB_MAXDEVADDR < p_ctrl->address))
    {
        return USB_ERR_PARA;
    }

    ret_code = R_USB_GetInformation(p_ctrl, &info);
    if (USB_SUCCESS != ret_code)
    {
        return USB_ERR_NG;
    }
    if (USB_STS_CONFIGURED == info.status)
    {
    utr.ip = p_ctrl->module;
    utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);

#if defined(USB_CFG_HHID_USE)
    err = usb_hstd_ChangeDeviceState(&utr, (usb_cb_t)&usb_hstd_dummy_function, USB_DO_GLOBAL_SUSPEND, p_ctrl->address);
    if (USB_OK != err)
    {
        ret_code = USB_ERR_NG;
    }
#else   /* defined(USB_CFG_HHID_USE) */
        usb_hstd_suspend_process(&utr, USB_PORT0);
        /* PIPE suspend */
        for( j = USB_MIN_PIPE_NO; j <= USB_MAX_PIPE_NO; j++ )
        {
            /* PID=BUF ? */
            if( usb_cstd_get_pid(&utr, j) == USB_PID_BUF )
            {
                usb_cstd_set_nak(&utr, j);
                g_usb_hstd_suspend_pipe[utr.ip][j] = USB_SUSPENDED;
            }
        }
        g_usb_hstd_device_info[utr.ip][p_ctrl->address][1] = USB_SUSPENDED;
#endif  /* defined(USB_CFG_HHID_USE) */
    }
    else if (USB_STS_SUSPEND == info.status)
    {
        return USB_ERR_BUSY;
    }
    else
    {
        return USB_ERR_NG;
    }

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    return ret_code;
} /* End of function R_USB_Suspend() */

/******************************************************************************
Function Name   : R_USB_Resume
Description     : USB Resume process(Host) and Remote wakeup process(Peri).
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_Resume(usb_ctrl_t *p_ctrl)
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#if defined(USB_CFG_HHID_USE)
    usb_er_t    err;
#else   /* defined(USB_CFG_HHID_USE) */
    uint16_t    j;
#endif  /* defined(USB_CFG_HHID_USE) */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    usb_err_t   ret_code = USB_SUCCESS;
    usb_info_t  info;

    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    if (USB_HOST == g_usb_usbmode)
    {
        if ((0 == p_ctrl->address) || (USB_MAXDEVADDR < p_ctrl->address))
        {
            return USB_ERR_PARA;
        }
    }
    else
    {
        if (USB_PHID != p_ctrl->type)
        {
            return USB_ERR_PARA;
        }
    }

    ret_code = R_USB_GetInformation(p_ctrl, &info);
    if (USB_SUCCESS != ret_code)
    {
        return USB_ERR_NG;
    }

    if (USB_STS_SUSPEND != info.status)
    {
        return USB_ERR_NOT_SUSPEND;
    }

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip = p_ctrl->module;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);

#if defined(USB_CFG_HHID_USE)
        err = usb_hstd_ChangeDeviceState(&utr, (usb_cb_t)&usb_hstd_dummy_function,
                                         USB_DO_GLOBAL_RESUME, p_ctrl->address);
        if (USB_OK != err)
        {
            ret_code = USB_ERR_NG;
        }
#else   /* defined(USB_CFG_HHID_USE) */
        usb_hstd_resume_process(&utr, USB_PORT0);
        /* PIPE resume */
        for( j = USB_MIN_PIPE_NO; j <= USB_MAX_PIPE_NO; j++ )
        {
            if(g_usb_hstd_suspend_pipe[utr.ip][j] == USB_SUSPENDED)
            {
                usb_cstd_set_buf(&utr, j);
                g_usb_hstd_suspend_pipe[utr.ip][j] = USB_NULL;
            }
        }
        g_usb_hstd_device_info[utr.ip][p_ctrl->address][1] = USB_CONFIGURED;
#endif  /* defined(USB_CFG_HHID_USE) */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        usb_pstd_change_device_state(USB_DO_REMOTEWAKEUP, USB_NULL, (usb_cb_t)usb_pstd_dummy_function);
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }

    return ret_code;
} /* End of function R_USB_Resume() */

/******************************************************************************
Function Name   : R_USB_VbusSetting
Description     : Contol of USB VBUS.(USB Host only)
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t   R_USB_VbusSetting( usb_ctrl_t *p_ctrl, uint16_t state )
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    usb_err_t   ret_code = USB_SUCCESS;

    if (USB_PERI == g_usb_usbmode)
    {
        return USB_ERR_NG;              /* Support Host only. */
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    utr.ip = p_ctrl->module;
    utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);

    if (USB_ON == state)
    {
        usb_hstd_vbus_control(&utr, USB_PORT0, (uint16_t)USB_VBON);  /* VBUS out */
    }
    else if (USB_OFF == state)
    {
        usb_hstd_vbus_control(&utr, USB_PORT0, (uint16_t)USB_VBOFF); /* VBUS out */
    }
    else
    {
        ret_code = USB_ERR_PARA;
    }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    return ret_code;
} /* End of function R_USB_VbusSetting() */

/******************************************************************************
Function Name   : R_USB_GetInformation
Description     : Get USB Informatio.(Device class,Connect speed,Device status)
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t   R_USB_GetInformation( usb_ctrl_t *p_ctrl, usb_info_t *p_info)
{
    usb_err_t   ret_code = USB_SUCCESS;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    uint16_t    status;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (USB_NULL == p_ctrl)
        {
            return USB_ERR_PARA;
        }
        utr.ip = p_ctrl->module;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);

        /* Set USB Device class */
        switch (g_usb_hstd_device_info[utr.ip][p_ctrl->address][3])
        {
            case USB_IFCLS_CDC:
                p_info->type   = USB_PCDC;
                break;
            case USB_IFCLS_HID:
                p_info->type   = USB_PHID;
                break;
            case USB_IFCLS_MAS:
                p_info->type   = USB_PMSC;
                break;
            case USB_IFCLS_VEN:
                p_info->type   = USB_PVND;
                break;
            default:
                p_info->type   = USB_NULL;
                break;
        }
        /* Set USB connect speed */
        switch(g_usb_hstd_device_info[utr.ip][p_ctrl->address][4])
        {
            case USB_NOCONNECT:
                p_info->speed  = USB_NULL;
                break;
            case USB_HSCONNECT:
                p_info->speed  = USB_HS;
                break;
            case USB_FSCONNECT:
                p_info->speed  = USB_FS;
                break;
            case USB_LSCONNECT:
                p_info->speed  = USB_LS;
                break;
            default:
                p_info->speed  = USB_NULL;
                break;
        }
        /* Set USB device state */
        switch (g_usb_hstd_device_info[utr.ip][p_ctrl->address][1])
        {
            case USB_POWERED:                           /* Power state  */
                p_info->status = USB_STS_POWERED;
                break;
            case USB_DEFAULT:                           /* Default state  */
                p_info->status = USB_STS_DEFAULT;
                break;
            case USB_ADDRESS:                           /* Address state  */
                p_info->status = USB_STS_ADDRESS;
                break;
            case USB_CONFIGURED:                        /* Configured state  */
                p_info->status = USB_STS_CONFIGURED;
                break;
            case USB_SUSPENDED:                         /* Suspend state */
                p_info->status = USB_STS_SUSPEND;
                break;
            case USB_DETACHED:                          /* Disconnect(VBUSon) state */
                p_info->status = USB_STS_DETACH;
                break;
            default:                                    /* Error */
                p_info->status = USB_NULL;
                break;
        }
        /* Set USB Peri BC port state */
#if USB_CFG_BC == USB_CFG_ENABLE
        if (USB_DEVICEADDR == p_ctrl->address)          /* Check Root port address */
        {
            p_info->port   = g_usb_hstd_bc[utr.ip].state;  /* USB_SDP/USB_CDP/USB_DCP */
        }
        else
        {
            p_info->port   = USB_SDP;  /* USB_SDP/USB_CDP/USB_DCP */
        }
#else   /* #if USB_CFG_BC == USB_CFG_ENABLE */
        p_info->port   = USB_SDP;  /* USB_SDP/USB_CDP/USB_DCP */
#endif  /* #if USB_CFG_BC == USB_CFG_ENABLE */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        if (USB_NULL == p_info)
        {
            return USB_ERR_PARA;
        }
        p_info->type   = USB_NULL;              /* Device class */
        switch(usb_cstd_port_speed(USB_NULL, USB_NULL))
        {
        case USB_NOCONNECT:
            p_info->speed  = USB_NULL;
            break;
        case USB_HSCONNECT:
            p_info->speed  = USB_HS;
            break;
        case USB_FSCONNECT:
            p_info->speed  = USB_FS;
            break;
        case USB_LSCONNECT:
            p_info->speed  = USB_LS;
            break;
        default:
            p_info->speed  = USB_NULL;
            break;
        }
        status = HW_USB_ReadIntsts();
        switch ((uint16_t)(status & USB_DVSQ))
        {
        case USB_DS_POWR:                           /* Power state  */
            p_info->status = USB_STS_POWERED;
            break;
        case USB_DS_DFLT:                           /* Default state  */
            p_info->status = USB_STS_DEFAULT;
            break;
        case USB_DS_ADDS:                           /* Address state  */
            p_info->status = USB_STS_ADDRESS;
            break;
        case USB_DS_CNFG:                           /* Configured state  */
            p_info->status = USB_STS_CONFIGURED;
            break;
        case USB_DS_SPD_POWR:                       /* Power suspend state */
        case USB_DS_SPD_DFLT:                       /* Default suspend state */
        case USB_DS_SPD_ADDR:                       /* Address suspend state */
        case USB_DS_SPD_CNFG:                       /* Configured Suspend state */
            p_info->status = USB_STS_SUSPEND;
            break;
        default:                                    /* Error */
/*            p_info->status = USB_STS_ERROR; */
            break;
        }
        p_info->port   = g_usb_bc_detect;           /* USB_SDP/USB_CDP/USB_DCP */
        ret_code = USB_SUCCESS;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }
    return ret_code;
}   /* End of function R_USB_GetInformation() */

/******************************************************************************
Function Name   : R_USB_PipeRead
Description     : USB Data receive process for Vendor class.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : uint8_t *buf      : transfer data address
                : uint32_t size     : transfer length
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t  R_USB_PipeRead(usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    usb_er_t    err;
    usb_err_t   ret_code;
    usb_info_t  info;

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].ip = p_ctrl->module;
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].ipp = usb_hstd_get_usb_ip_adr(g_usb_hdata[p_ctrl->module][p_ctrl->pipe].ip);

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    if ((USB_NULL == p_ctrl) || (USB_PIPE0 == p_ctrl->pipe) || (USB_MAXPIPE_NUM < p_ctrl->pipe))
    {
        return USB_ERR_PARA;
    }

    if ((USB_NULL == buf) || (USB_NULL == size))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    ret_code = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != ret_code) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    /* PIPE Transfer set */
    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].keyword     = p_ctrl->pipe;             /* Pipe No */
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].tranadr     = buf;                      /* Data address */
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].tranlen     = size;                     /* Data Size */
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].setup       = 0;

#if defined(USB_CFG_HVND_USE)
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].complete    = usb_hvendor_read_complete;   /* Callback function */
#endif /* defined(USB_CFG_HVND_USE) */
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].segment     = USB_TRAN_END;
        *(uint32_t *)g_usb_hdata[p_ctrl->module][p_ctrl->pipe].usr_data = p_ctrl->address;

        g_usb_read_request_size[p_ctrl->pipe] = size;

        err = usb_hstd_transfer_start(&g_usb_hdata[p_ctrl->module][p_ctrl->pipe]);        /* USB Transfer Start */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        g_usb_pdata[p_ctrl->pipe].keyword    = p_ctrl->pipe;        /* Pipe No */
        g_usb_pdata[p_ctrl->pipe].tranadr    = buf;                 /* Data address */
        g_usb_pdata[p_ctrl->pipe].tranlen    = size;                /* Data Size */
#if defined(USB_CFG_PVND_USE)
        g_usb_pdata[p_ctrl->pipe].complete   = (usb_cb_t)&usb_pvnd_read_complete;   /* Callback function */
#endif  /* defined(USB_CFG_PCDC_USE) */

        g_usb_read_request_size[p_ctrl->pipe] = size;

        err = usb_pstd_transfer_start(&g_usb_pdata[p_ctrl->pipe]);   /* USB Transfer Start */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }

    if (USB_OK == err)
    {
        ret_code = USB_SUCCESS;
    }
    else if (USB_QOVR == err)
    {
        ret_code = USB_ERR_BUSY;
    }
    else
    {
        ret_code = USB_ERR_NG;
    }
    return ret_code;
}   /* End of function R_USB_PipeRead() */

/******************************************************************************
Function Name   : R_USB_PipeWrite
Description     : USB Data send process for Vendor class.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : uint8_t *buf      : transfer data address
                : uint32_t size     : transfer length
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t  R_USB_PipeWrite(usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    usb_er_t    err;
    usb_err_t   ret_code;
    usb_info_t  info;

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].ip = p_ctrl->module;
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].ipp = usb_hstd_get_usb_ip_adr(g_usb_hdata[p_ctrl->module][p_ctrl->pipe].ip);

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    if ((USB_NULL == p_ctrl) || (USB_PIPE0 == p_ctrl->pipe) || (USB_MAXPIPE_NUM < p_ctrl->pipe))
    {
        return USB_ERR_PARA;
    }

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    if (USB_PERI == g_usb_usbmode)
    {
        if ((USB_NULL == buf) || (USB_NULL == size))
        {
            return USB_ERR_PARA;
        }
    }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    ret_code = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != ret_code) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    /* PIPE Transfer set */
    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].keyword     = p_ctrl->pipe;             /* Pipe No */
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].tranadr     = buf;                      /* Data address */
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].tranlen     = size;                     /* Data Size */
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].setup       = 0;
#if defined(USB_CFG_HVND_USE)
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].complete    = usb_hvendor_write_complete;  /* Callback function */
#endif /* defined(USB_CFG_HVND_USE) */
        g_usb_hdata[p_ctrl->module][p_ctrl->pipe].segment     = USB_TRAN_END;
        *(uint32_t *)g_usb_hdata[p_ctrl->module][p_ctrl->pipe].usr_data = p_ctrl->address;

        err = usb_hstd_transfer_start(&g_usb_hdata[p_ctrl->module][p_ctrl->pipe]);        /* USB Transfer Start */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        g_usb_pdata[p_ctrl->pipe].keyword    = p_ctrl->pipe;        /* Pipe No */
        g_usb_pdata[p_ctrl->pipe].tranadr    = buf;                 /* Data address */
        g_usb_pdata[p_ctrl->pipe].tranlen    = size;                /* Data Size */
#if defined(USB_CFG_PVND_USE)
        g_usb_pdata[p_ctrl->pipe].complete   = (usb_cb_t)&usb_pvnd_write_complete;  /* Callback function */
#endif  /* defined(USB_CFG_PCDC_USE) */

        err = usb_pstd_transfer_start(&g_usb_pdata[p_ctrl->pipe]);   /* USB Transfer Start */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }

    if (USB_OK == err)
    {
        ret_code = USB_SUCCESS;
    }
    else if (USB_QOVR == err)
    {
        ret_code = USB_ERR_BUSY;
    }
    else
    {
        ret_code = USB_ERR_NG;
    }
    return ret_code;
}   /* End of function R_USB_PipeWrite() */

/******************************************************************************
Function Name   : R_USB_PipeStop
Description     : USB transfer stop process for Vendor class.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_PipeStop(usb_ctrl_t *p_ctrl)
{
    usb_er_t    err;
    usb_err_t   ret_code = USB_ERR_NG;
    usb_info_t  info;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;

    if (USB_HOST == g_usb_usbmode)
    {
        utr.ip = p_ctrl->module;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
    }

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    if ((USB_NULL == p_ctrl) || (USB_PIPE0 == p_ctrl->pipe) || (USB_MAXPIPE_NUM < p_ctrl->pipe))
    {
        return USB_ERR_PARA;
    }

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        err = usb_hstd_transfer_end(&utr, p_ctrl->pipe, (uint16_t)USB_DATA_STOP);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        err = usb_pstd_transfer_end(p_ctrl->pipe);
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }
    if (USB_OK == err)
    {
        ret_code = USB_SUCCESS;
    }

    return ret_code;
}   /* End of function R_USB_PipeStop() */

/******************************************************************************
Function Name   : R_USB_GetUsePipe
Description     : Get Information for Use pipe.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : uint16_t *p_pipe  : address of stored use pipe bitmap.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_GetUsePipe(usb_ctrl_t *p_ctrl, uint16_t *p_pipe)
{
    usb_err_t   err;
    usb_info_t  info;
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    uint16_t    i;
    uint16_t    * table;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;

    if ((USB_NULL == p_ctrl) && (USB_HOST == g_usb_usbmode))
    {
        return USB_ERR_PARA;
    }

    utr.ip = p_ctrl->module;
    utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    if (USB_NULL == p_pipe)
    {
        return USB_ERR_PARA;
    }

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    /* Get PIPE Number from Endpoint address */
    *p_pipe = ((uint16_t)1 << USB_PIPE0);
    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        *p_pipe |= g_usb_hstd_use_pipe[utr.ip];
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        table = g_usb_pstd_driver.pipetbl;
        for (i = 0; table[i] != USB_PDTBLEND; i += USB_EPL)                 /* EP table loop */
        {
            (*p_pipe) |= ((uint16_t)1 << table[i]);
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }

    return USB_SUCCESS;
}   /* End of function R_USB_GetUsePipe() */

/******************************************************************************
Function Name   : R_USB_GetPipeInfo
Description     : Get Infomation for pipe setting.(DIR,Transfer Type,MAXPS)
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : usb_pipe_t *p_info:
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_GetPipeInfo(usb_ctrl_t *p_ctrl, usb_pipe_t *p_info)
{
    usb_err_t   err;
    usb_info_t  info;
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    uint16_t    i;
    uint16_t    * table;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;

    if (USB_HOST == g_usb_usbmode)
    {
        utr.ip = p_ctrl->module;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
    }

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

    if ((USB_NULL == p_ctrl) || (0 == p_ctrl->pipe) || (USB_MAXPIPE_NUM < p_ctrl->pipe))
    {
        return USB_ERR_PARA;
    }


#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        p_info->ep = usb_hstd_pipe_to_epadr(&utr, p_ctrl->pipe);

        switch (usb_cstd_get_pipe_type(&utr, p_ctrl->pipe))
        {
        case USB_TYPFIELD_ISO:
            p_info->type = USB_EP_ISO;                             /* Set Isochronous */
            break;
        case USB_TYPFIELD_BULK:
            p_info->type = USB_EP_BULK;                             /* Set Bulk */
            break;
        case USB_TYPFIELD_INT:
            p_info->type = USB_EP_INT;                              /* Set Interrupt */
            break;
        default:
            return USB_ERR_NG;
            break;
        }
        p_info->mxps = usb_cstd_get_maxpacket_size(&utr, p_ctrl->pipe);   /* Set Max packet size */
        return USB_SUCCESS;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        /* Get PIPE Number from Endpoint address */
        table = g_usb_pstd_driver.pipetbl;
        for (i = 0; table[i] != USB_PDTBLEND; i += USB_EPL)                 /* EP table loop */
        {
            if (table[i] == p_ctrl->pipe)                                   /* EP table pipe no check */
            {
                p_info->ep   = (uint8_t)(table[i + 1] & USB_EPNUMFIELD);    /* Set EP num. */
                if (USB_DIR_P_IN == (table[i + 1] & USB_DIRFIELD))          /* Check dir */
                {
                    p_info->ep |= USB_EP_DIR_IN;                            /* Set DIR IN */
                }

                switch (usb_cstd_get_pipe_type(USB_NULL, p_ctrl->pipe))
                {
                case USB_TYPFIELD_ISO:
                    p_info->type = USB_EP_ISO;                             /* Set Isochronous */
                    break;
                case USB_TYPFIELD_BULK:
                    p_info->type = USB_EP_BULK;                             /* Set Bulk */
                    break;
                case USB_TYPFIELD_INT:
                    p_info->type = USB_EP_INT;                              /* Set Interrupt */
                    break;
                default:
                    return USB_ERR_NG;
                    break;
                }
                p_info->mxps = usb_cstd_get_maxpacket_size(USB_NULL, p_ctrl->pipe);   /* Set Max packet size */
                return USB_SUCCESS;
            }
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }
    return USB_ERR_NG;
}   /* End of function R_USB_GetPipeInfo() */

#if defined(USB_CFG_HMSC_USE)
/******************************************************************************
Function Name   : R_USB_HmscStrgCmd
Description     : Processing for MassStorage(ATAPI) command.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : uint8_t *buf      : address for tarnsfer data.
                : uint16_t command  : ATAPI command
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t   R_USB_HmscStrgCmd(usb_ctrl_t *p_ctrl, uint8_t *buf, uint16_t command)
{
    usb_err_t   err;
    usb_info_t  info;
    usb_utr_t   utr;
    uint16_t    side;

    utr.ip = p_ctrl->module;
    utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);

    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    side = R_USB_HmscGerDriveNo(p_ctrl);
    if (USB_OK != R_USB_HmscStrgUserCommand(&utr, side, command, buf, usb_hmsc_strg_cmd_complete) )
    {
        return USB_ERR_NG;
    }
    return USB_SUCCESS;
}   /* End of function R_USB_HmscStrgCmd() */


/******************************************************************************
Function Name   : R_USB_HmscGerDriveNo
Description     : Get number of Storage drive.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
uint8_t     R_USB_HmscGerDriveNo(usb_ctrl_t *p_ctrl)
{
    usb_err_t   err;
    usb_info_t  info;
    usb_utr_t   utr;
    volatile uint16_t   address;

    utr.ip = p_ctrl->module;
    utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);

    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }

    address = p_ctrl->address;
    if (USB_IP1 == p_ctrl->module)
    {
        address |= USBA_ADDRESS_OFFSET;
    }

    return (uint8_t)R_USB_HmscRefDrvno(address);
}   /* End of function R_USB_HmscGerDriveNo() */
#endif  /* defined(USB_CFG_HMSC_USE) */

#if defined(USB_CFG_HHID_USE)
/******************************************************************************
Function Name   : R_USB_HhidGetType
Description     : Get HID protocol.(USB Mouse/USB Keyboard/Protocon non.)
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                : uint8_t *p_type   : address for stored HID protocol value.
Return value    : usb_err_t error code   : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_HhidGetType(usb_ctrl_t *p_ctrl, uint8_t *p_type)
{
    usb_err_t   err;
    usb_info_t  info;
    usb_utr_t   utr;

    utr.ip = p_ctrl->module;
    utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);


    if ((USB_NULL == p_ctrl) || (USB_NULL == p_type))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if( (USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module) )
    {
        return USB_ERR_PARA;
    }

#if defined(BSP_MCU_RX65N)
    if( USB_IP1 == p_ctrl->module)
    {
        return USB_ERR_PARA;
    }
#endif

    err = R_USB_GetInformation(p_ctrl, &info);
    if ((USB_SUCCESS != err) || (USB_STS_CONFIGURED != info.status))
    {
        return USB_ERR_NG;
    }
    *p_type = R_USB_HhidGetHidProtocol( p_ctrl->module, p_ctrl->address );
    return USB_SUCCESS;
}   /* End of function R_USB_HhidGetType() */

#endif /* defined(USB_CFG_HHID_USE) */
/******************************************************************************
End  Of File
******************************************************************************/
