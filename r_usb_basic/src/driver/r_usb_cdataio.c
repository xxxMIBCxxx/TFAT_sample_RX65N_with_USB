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
 * File Name    : r_usb_cdataio.c
 * Description  : USB Host and Peripheral low level data I/O code
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
    #include "r_usb_hcdc_config.h"
#endif /* defined(USB_CFG_HCDC_USE) */

#if defined(USB_CFG_PCDC_USE)
    #include "r_usb_pcdc_config.h"
#endif /* defined(USB_CFG_PCDC_USE) */

#if defined(USB_CFG_HHID_USE)
    #include "r_usb_hhid_config.h"
#endif /* defined(USB_CFG_HHID_USE) */

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
    #include "r_usb_dmac.h"
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

/******************************************************************************
 Constant macro definitions
 ******************************************************************************/

/******************************************************************************
 External variables and functions
 ******************************************************************************/
extern uint16_t g_usb_usbmode; /* USB mode HOST/PERI */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
extern usb_utr_t g_usb_hdata[USB_NUM_USBIP][USB_MAXPIPE_NUM + 1];
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
extern usb_utr_t g_usb_pdata[USB_MAXPIPE_NUM + 1];
extern const uint8_t g_usb_pipe_peri[];
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */

#if defined(USB_CFG_PCDC_USE)
extern void pcdc_read_complete( usb_utr_t *mess, uint16_t data1, uint16_t data2);
extern void pcdc_write_complete( usb_utr_t *mess, uint16_t data1, uint16_t data2);
#endif /* defined(USB_CFG_PCDC_USE) */

#if defined(USB_CFG_HCDC_USE)
extern void hcdc_read_complete (usb_utr_t *mess, uint16_t devadr, uint16_t data2);
extern void hcdc_write_complete (usb_utr_t *mess, uint16_t devadr, uint16_t data2);
#endif /* defined(USB_CFG_HCDC_USE) */

#if defined(USB_CFG_HHID_USE)
extern void hhid_read_complete(usb_utr_t *mess, uint16_t devadr, uint16_t data2);
extern void hhid_write_complete(usb_utr_t *mess, uint16_t devadr, uint16_t data2);
#endif /* defined(USB_CFG_HHID_USE) */

#if defined(USB_CFG_PHID_USE)
extern void phid_read_complete(usb_utr_t *mess, uint16_t data1, uint16_t data2);
extern void phid_write_complete(usb_utr_t *mess, uint16_t data1, uint16_t data2);
#endif /* defined(USB_CFG_PHID_USE) */

#if defined(USB_CFG_PCDC_USE)
extern uint8_t g_usb_pcdc_serialstate_table[];
#endif /* defined(USB_CFG_PCDC_USE) */

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
extern uint16_t g_usb_cstd_dma_dir[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 direction */
extern uint32_t g_usb_cstd_dma_size[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 buffer size */
extern uint16_t g_usb_cstd_dma_fifo[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 FIFO buffer size */
extern uint16_t g_usb_cstd_dma_pipe[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 pipe number */

extern void usb_cstd_dxfifo2buf_start_dma(usb_utr_t *ptr, uint16_t pipe, uint16_t useport, uint32_t length);
extern void usb_cstd_buf2dxfifo_start_dma(usb_utr_t *ptr, uint16_t pipe, uint16_t useport);

#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

/******************************************************************************
 Static variables and functions
 ******************************************************************************/
static void usb_cstd_select_nak (usb_utr_t *ptr, uint16_t pipe);

/*uint8_t usb_get_usepipe (usb_ctrl_t *p_ctrl, uint8_t dir);*/
void usb_class_request_complete (usb_utr_t *mess, uint16_t devadr, uint16_t data2);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )

/* USB data transfer */
/* PIPEn Buffer counter */
uint32_t g_usb_hstd_data_cnt[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];

/* PIPEn Buffer pointer(8bit) */
uint8_t *g_usb_hstd_data_ptr[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];

/* Message pipe */
usb_utr_t *g_usb_hstd_pipe[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];

/* Hi-speed enable */
uint16_t g_usb_cstd_hs_enable[USB_NUM_USBIP];
usb_ctrl_trans_t g_usb_ctrl_request[USB_NUM_USBIP][USB_MAXDEVADDR + 1];

#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/* USB data transfer */
/* PIPEn Buffer counter */
uint32_t g_usb_pstd_data_cnt[USB_MAX_PIPE_NO + 1u];
/* PIPEn Buffer pointer(8bit) */
uint8_t *g_usb_pstd_data_ptr[USB_MAX_PIPE_NO + 1u];
/* Message pipe */
usb_utr_t *g_usb_pstd_pipe[USB_MAX_PIPE_NO + 1u];
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */

/* Callback function of USB Read/USB Write */
void (*g_usb_callback[]) (usb_utr_t *, uint16_t, uint16_t) =
{
    /* PCDC, PCDCC */
#if defined(USB_CFG_PCDC_USE)
        pcdc_read_complete, pcdc_write_complete, /* USB_PCDC  (0) */
        USB_NULL, pcdc_write_complete, /* USB_PCDCC (1) */
#else
        USB_NULL, USB_NULL, /* USB_PCDC  (0) */
        USB_NULL, USB_NULL, /* USB_PCDCC (1) */
#endif

        /* PHID */
#if defined(USB_CFG_PHID_USE)
        phid_read_complete, phid_write_complete, /* USB_PHID (2) */
#else
        USB_NULL, USB_NULL, /* USB_PHID (2) */
#endif

        /* PVNDR */
#if defined(USB_CFG_PVNDR_USE)
        pvndr_read_complete, pnvdr_write_complete, /* USB_PVND  (3) */
#else
        USB_NULL, USB_NULL, /* USB_PVND  (3) */
#endif

        /* HCDC, HCDCC */
#if defined(USB_CFG_HCDC_USE)
        hcdc_read_complete, hcdc_write_complete, /* USB_HCDC  (4) */
        hcdc_read_complete, USB_NULL, /* USB_HCDCC (5) */
#else
        USB_NULL, USB_NULL, /* USB_HCDC  (4) */
        USB_NULL, USB_NULL, /* USB_HCDCC (5) */
#endif

        /* HHID */
#if defined(USB_CFG_HHID_USE)
        hhid_read_complete, hhid_write_complete, /* USB_HHID  (6) */
#else
        USB_NULL, USB_NULL, /* USB_HHID  (6) */
#endif

        /* HVNDR */
#if defined(USB_CFG_HVNDR_USE)
        hvndr_read_complete, hnvdr_write_complete, /* USB_HVND  (7) */
#else
        USB_NULL, USB_NULL, /* USB_HVND  (7) */
#endif

        /* HMSC */
        USB_NULL, USB_NULL, /* USB_HMSC  (8) */

        /* PMSC */
        USB_NULL, USB_NULL, /* USB_PMSC  (9) */
}; /* const void (g_usb_callback[])(usb_utr_t *, uint16_t, uint16_t) */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )

/* Pipe number of USB Host transfer.(Read pipe/Write pipe) */
const uint8_t g_usb_pipe_host[] =
{
/* READ pipe *//* WRITE pipe */
/* IN pipe *//* OUT pipe */
    #if defined(USB_CFG_HCDC_USE)
        USB_CFG_HCDC_BULK_IN, USB_CFG_HCDC_BULK_OUT, /* HCDC: Address 1 */
        USB_CFG_HCDC_BULK_IN, USB_CFG_HCDC_BULK_OUT, /* HCDC: Address 2 using Hub */
        USB_CFG_HCDC_BULK_IN2, USB_CFG_HCDC_BULK_OUT2, /* HCDC: Address 3 using Hub */
        USB_NULL, USB_NULL,
    #else
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
    #endif

    #if defined(USB_CFG_HCDC_USE)
        USB_CFG_HCDC_INT_IN, USB_NULL, /* HCDCC: Address 1 */
        USB_CFG_HCDC_INT_IN, USB_NULL, /* HCDCC: Address 2 using Hub */
        USB_CFG_HCDC_INT_IN2, USB_NULL, /* HCDCC: Address 3 using Hub */
        USB_NULL, USB_NULL,
    #else
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
    #endif

    #if defined(USB_CFG_HHID_USE)
        USB_CFG_HHID_INT_IN, USB_NULL, /* HCDC: Address 1 */
        USB_CFG_HHID_INT_IN, USB_NULL, /* HCDC: Address 2 using Hub */
        USB_CFG_HHID_INT_IN2, USB_NULL, /* HCDC: Address 3 using Hub */
        USB_CFG_HHID_INT_IN3, USB_NULL, /* HCDC: Address 4 using Hub */
    #else
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
        USB_NULL, USB_NULL,
    #endif

        };

#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

/******************************************************************************
 Renesas Abstracted common data I/O functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_select_nak
 Description     : Set the specified pipe PID to send a NAK if the transfer type 
 : is BULK/INT. 
 Arguments       : uint16_t pipe     : Pipe number.
 Return value    : none
 ******************************************************************************/
static void usb_cstd_select_nak (usb_utr_t *ptr, uint16_t pipe)
{
    /* Check PIPE TYPE */
    if (usb_cstd_get_pipe_type(ptr, pipe) != USB_TYPFIELD_ISO)
    {
        usb_cstd_set_nak(ptr, pipe);
    }
}
/******************************************************************************
 End of function usb_cstd_select_nak
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_debug_hook
 Description     : Debug hook
 Arguments       : uint16_t error_code          : error code
 Return value    : none
 ******************************************************************************/
void usb_cstd_debug_hook (uint16_t error_code)
{
    while (1)
    {
        /* Non */
    }
} /* End of function usb_cstd_debug_hook() */

/******************************************************************************
 Function Name   : usb_ctrl_read
 Description     : Receive process for Control transfer
 Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
 : uint8_t *buf      : transfer data address
 : uint32_t size     : transfer length
 Return value    : usb_er_t          : USB_SUCCESS(USB_OK) / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_ctrl_read (usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_er_t err;

    if (USB_HOST == g_usb_usbmode)
    {
        g_usb_read_request_size[p_ctrl->address] = size;
        g_usb_hdata[p_ctrl->module][USB_PIPE0].keyword = USB_PIPE0; /* Pipe No */
        g_usb_hdata[p_ctrl->module][USB_PIPE0].tranadr = buf; /* Data address */
        g_usb_hdata[p_ctrl->module][USB_PIPE0].tranlen = size; /* Data Size */

        /* Callback function */
        g_usb_hdata[p_ctrl->module][USB_PIPE0].complete = usb_class_request_complete;
        g_usb_ctrl_request[p_ctrl->module][p_ctrl->address].address = p_ctrl->address;
        g_usb_ctrl_request[p_ctrl->module][p_ctrl->address].setup = p_ctrl->setup;

        /* Setup message address set */
        g_usb_hdata[p_ctrl->module][USB_PIPE0].setup =
                (uint16_t *) &g_usb_ctrl_request[p_ctrl->module][p_ctrl->address];
        g_usb_hdata[p_ctrl->module][USB_PIPE0].segment = USB_TRAN_END;
        g_usb_hdata[p_ctrl->module][USB_PIPE0].ip = p_ctrl->module;
        g_usb_hdata[p_ctrl->module][USB_PIPE0].ipp = usb_hstd_get_usb_ip_adr((uint8_t) p_ctrl->module);
        err = usb_hstd_transfer_start(&g_usb_hdata[p_ctrl->module][USB_PIPE0]);
        return err;
    }
#endif
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    if (USB_PERI == g_usb_usbmode)
    {
        usb_pstd_ctrl_write(size, buf);
    }
#endif
    return USB_SUCCESS;
} /* End of function usb_ctrl_read() */

/******************************************************************************
 Function Name   : usb_ctrl_write
 Description     : Send process for Control transfer
 Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
 : uint8_t *buf      : transfer data address
 : uint32_t size     : transfer length
 Return value    : usb_er_t          : USB_SUCCESS(USB_OK) / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_ctrl_write (usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_er_t err;

    if (USB_HOST == g_usb_usbmode)
    {
        g_usb_read_request_size[p_ctrl->address] = size;
        g_usb_hdata[p_ctrl->module][USB_PIPE0].keyword = USB_PIPE0; /* Pipe No */
        g_usb_hdata[p_ctrl->module][USB_PIPE0].tranadr = buf; /* Data address */
        g_usb_hdata[p_ctrl->module][USB_PIPE0].tranlen = size; /* Data Size */

        /* Callback function */
        g_usb_hdata[p_ctrl->module][USB_PIPE0].complete = usb_class_request_complete;
        g_usb_ctrl_request[p_ctrl->module][p_ctrl->address].address = p_ctrl->address;
        g_usb_ctrl_request[p_ctrl->module][p_ctrl->address].setup = p_ctrl->setup;

        /* Setup message address set */
        g_usb_hdata[p_ctrl->module][USB_PIPE0].setup =
                (uint16_t *) &g_usb_ctrl_request[p_ctrl->module][p_ctrl->address];
        g_usb_hdata[p_ctrl->module][USB_PIPE0].segment = USB_TRAN_END;
        g_usb_hdata[p_ctrl->module][USB_PIPE0].ip = p_ctrl->module;
        g_usb_hdata[p_ctrl->module][USB_PIPE0].ipp = usb_hstd_get_usb_ip_adr((uint8_t) p_ctrl->module);
        err = usb_hstd_transfer_start(&g_usb_hdata[p_ctrl->module][USB_PIPE0]);
        return err;
    }
#endif
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    if (USB_PERI == g_usb_usbmode)
    {
        usb_pstd_ctrl_read(size, buf);
    }
#endif
    return USB_SUCCESS;
} /* End of function usb_ctrl_write() */

/******************************************************************************
 Function Name   : usb_ctrl_stop
 Description     : Stop of USB Control transfer.
 Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
 Return value    : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_ctrl_stop (usb_ctrl_t *p_ctrl)
{
    usb_er_t err;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t utr;
#endif

    if ((p_ctrl->type) > USB_PVND)
    {
        /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip = p_ctrl->module;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
        err = usb_hstd_transfer_end(&utr, USB_PIPE0, (uint16_t) USB_DATA_STOP);
#endif
    }
    else
    {
        /* Peripheral only */
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        err = usb_pstd_transfer_end(USB_PIPE0);
#endif
    }
    return err;
} /* End of function usb_ctrl_stop() */

/******************************************************************************
 Function Name   : usb_data_read
 Description     : 
 Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
 : uint8_t *buf      : transfer data address
 : uint32_t size     : transfer length
 Return value    : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_data_read (usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    uint8_t pipe;
    usb_er_t err;

    pipe = usb_get_usepipe(p_ctrl, USB_READ);
    g_usb_read_request_size[pipe] = size;

    if ((p_ctrl->type) > USB_PVND)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        g_usb_hdata[p_ctrl->module][pipe].keyword = pipe; /* Pipe No */
        g_usb_hdata[p_ctrl->module][pipe].tranadr = buf; /* Data address */
        g_usb_hdata[p_ctrl->module][pipe].tranlen = size; /* Data Size */
        g_usb_hdata[p_ctrl->module][pipe].complete = g_usb_callback[p_ctrl->type * 2]; /* Callback function */
        g_usb_hdata[p_ctrl->module][pipe].segment = USB_TRAN_END;
        g_usb_hdata[p_ctrl->module][pipe].ip = p_ctrl->module;
        g_usb_hdata[p_ctrl->module][pipe].ipp = usb_hstd_get_usb_ip_adr((uint8_t) p_ctrl->module);
        err = usb_hstd_transfer_start(&g_usb_hdata[p_ctrl->module][pipe]);
#endif
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        g_usb_pdata[pipe].keyword = pipe; /* Pipe No */
        g_usb_pdata[pipe].tranadr = buf; /* Data address */
        g_usb_pdata[pipe].tranlen = size; /* Data Size */
        g_usb_pdata[pipe].complete = (usb_cb_t)g_usb_callback[p_ctrl->type*2]; /* Callback function */
        err = usb_pstd_transfer_start(&g_usb_pdata[pipe]);
#endif
    }
    return err;
} /* End of function usb_data_read() */

/******************************************************************************
 Function Name   : usb_data_write
 Description     : 
 Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
 : uint8_t *buf      : transfer data address
 : uint32_t size     : transfer length
 Return value    : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_data_write (usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    uint8_t pipe;
    usb_er_t err;

    pipe = usb_get_usepipe(p_ctrl, USB_WRITE);

    if ((p_ctrl->type) > USB_PVND)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        g_usb_hdata[p_ctrl->module][pipe].keyword = pipe; /* Pipe No */
        g_usb_hdata[p_ctrl->module][pipe].tranadr = buf; /* Data address */
        g_usb_hdata[p_ctrl->module][pipe].tranlen = size; /* Data Size */
        g_usb_hdata[p_ctrl->module][pipe].complete = g_usb_callback[(p_ctrl->type * 2) + 1]; /* Callback function */
        g_usb_hdata[p_ctrl->module][pipe].segment = USB_TRAN_END;
        g_usb_hdata[p_ctrl->module][pipe].ip = p_ctrl->module;
        g_usb_hdata[p_ctrl->module][pipe].ipp = usb_hstd_get_usb_ip_adr((uint8_t) p_ctrl->module);
        err = usb_hstd_transfer_start(&g_usb_hdata[p_ctrl->module][pipe]);
#endif
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if defined(USB_CFG_PCDC_USE)
        if (USB_CFG_PCDC_INT_IN != pipe)
        {
            g_usb_pdata[pipe].tranadr = buf; /* Data address */
            g_usb_pdata[pipe].tranlen = size; /* Data Size */
        }
        else
        {
            g_usb_pcdc_serialstate_table[8] = buf[0];
            g_usb_pcdc_serialstate_table[9] = buf[1];
            g_usb_pdata[pipe].tranadr = g_usb_pcdc_serialstate_table; /* Data address */
            g_usb_pdata[pipe].tranlen = 10; /* Data Size */
        }
    #else   /* defined(USB_CFG_PCDC_USE) */
        g_usb_pdata[pipe].tranadr = buf; /* Data address */
        g_usb_pdata[pipe].tranlen = size; /* Data Size */
    #endif  /* defined(USB_CFG_PCDC_USE) */
        g_usb_pdata[pipe].keyword = pipe; /* Pipe No */
        g_usb_pdata[pipe].complete = (usb_cb_t)g_usb_callback[p_ctrl->type*2+1]; /* Callback function */
        err = usb_pstd_transfer_start(&g_usb_pdata[pipe]);
#endif
    }
    return err;
} /* End of function usb_data_write() */

/******************************************************************************
 Function Name   : usb_data_stop
 Description     : 
 Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
 : uint16_t type     : Read(0)/Write(1)
 Return value    : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_data_stop (usb_ctrl_t *p_ctrl, uint16_t type)
{
    uint8_t pipe;
    usb_er_t err = USB_ERROR;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t utr;
#endif

    pipe = usb_get_usepipe(p_ctrl, type);

    if (USB_NULL == pipe)
    {
        return USB_ERROR;
    }

    if ((p_ctrl->type) > USB_PVND)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip = p_ctrl->module;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
        err = usb_hstd_transfer_end(&utr, pipe, (uint16_t) USB_DATA_STOP);
#endif
    }
    else
    {
        /* Peripheral only */
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        err = usb_pstd_transfer_end(pipe);
#endif
    }
    return err;
} /* End of function usb_data_stop() */

/******************************************************************************
 Function Name   : usb_get_usepipe
 Description     : Get pipe number for USB Read/USB Write
 Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
 : uint8_t dir       : Read(0)/Write(1)
 Return value    : Bitmap of Use pipe
 ******************************************************************************/
uint8_t usb_get_usepipe (usb_ctrl_t *p_ctrl, uint8_t dir)
{
    uint8_t pipe = USB_NULL;
    uint8_t idx;

    if ((p_ctrl->type) > USB_PVND)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        /* Host */
        idx = ((p_ctrl->type - USB_HCDC) * 8) + ((p_ctrl->address - 1) * 2) + dir;
        pipe = g_usb_pipe_host[idx];
#endif
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        /* Peripheral */
        idx = (p_ctrl->type * 2) + dir;
        pipe = g_usb_pipe_peri[idx];
#endif
    }
    return pipe;
} /* End of function usb_get_usepipe() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : usb_hstd_send_start
 Description     : Start data transmission using CPU/DMA transfer to USB host/
 : /device.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe no.
 Return value    : none
 ******************************************************************************/
void usb_hstd_send_start (usb_utr_t *ptr, uint16_t pipe)
{
    usb_utr_t *pp;
    uint32_t length;
    uint16_t useport;
    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
    uint16_t dma_ch;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    /* Evacuation pointer */
    pp = g_usb_hstd_pipe[ptr->ip][pipe];
    length = pp->tranlen;

    /* Check transfer count */
    if (USB_TRAN_CONT == pp->segment)
    {
        /* Sequence toggle */
        usb_hstd_do_sqtgl(ptr, pipe, pp->pipectr);
    }

    usb_cstd_select_nak(ptr, pipe); /* Select NAK */
    g_usb_hstd_data_cnt[ptr->ip][pipe] = length; /* Set data count */
    g_usb_hstd_data_ptr[ptr->ip][pipe] = (uint8_t*) pp->tranadr; /* Set data pointer */
    g_usb_hstd_ignore_cnt[ptr->ip][pipe] = (uint16_t) 0; /* Ignore count clear */

    HW_USB_ClearStatusBemp(ptr, pipe); /* BEMP Status Clear */
    HW_USB_ClearStsBrdy(ptr, pipe); /* BRDY Status Clear */

    useport = usb_hstd_pipe2fport(ptr, pipe);/* Pipe number to FIFO port select */

    /* Check use FIFO access */
    switch (useport)
    {

        case USB_CUSE : /* CFIFO use */

            /* Buffer to FIFO data write */
            usb_hstd_buf2fifo(ptr, pipe, useport);
            usb_cstd_set_buf(ptr, pipe); /* Set BUF */

        break;

        case USB_D0USE : /* D0FIFO use */

            /* D0 FIFO access is NG */
            USB_PRINTF1("### USB-ITRON is not support(SND-D0USE:pipe%d)\n", pipe);
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_ERR);

        break;

            /* D1FIFO use */
        case USB_D1USE :

            /* Buffer to FIFO data write */
            usb_hstd_buf2fifo(ptr, pipe, useport);
            usb_cstd_set_buf(ptr, pipe); /* Set BUF */

        break;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
            /* D0FIFO DMA */
            case USB_D0DMA:
            /* D1FIFO DMA */
            case USB_D1DMA:
            if (USB_D0DMA == useport)
            {
                dma_ch = USB_DMA_CH;
            }
            else
            {
                dma_ch = USB_DMA_CH2;
            }
            /* Setting for use PIPE number */
            g_usb_cstd_dma_pipe[ptr->ip][dma_ch] = pipe;
            /* PIPE direction */
            g_usb_cstd_dma_dir[ptr->ip][dma_ch] = usb_cstd_get_pipe_dir(ptr, pipe);
            /* Buffer size */
            g_usb_cstd_dma_fifo[ptr->ip][dma_ch] = usb_cstd_get_buf_size(ptr, pipe);
            /* Check data count */
            if( g_usb_hstd_data_cnt[ptr->ip][pipe] <= g_usb_cstd_dma_fifo[ptr->ip][dma_ch] )
            {
                /* Transfer data size */
                g_usb_cstd_dma_size[ptr->ip][dma_ch] = (uint16_t)g_usb_hstd_data_cnt[ptr->ip][pipe];
                /* Enable Empty Interrupt */
                HW_USB_SetBempenb(ptr, pipe);
            }
            else
            {
                /* Data size == FIFO size */
                g_usb_cstd_dma_size[ptr->ip][dma_ch] = (uint16_t)(g_usb_hstd_data_cnt[ptr->ip][pipe]
                        - (g_usb_hstd_data_cnt[ptr->ip][pipe] % g_usb_cstd_dma_fifo[ptr->ip][dma_ch]));
            }

            usb_cstd_buf2dxfifo_start_dma( ptr, pipe, useport );

            /* Set BUF */
            usb_cstd_set_buf(ptr, pipe);
            break;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        default :

            /* Access is NG */
            USB_PRINTF1("### USB-ITRON is not support(SND-else:pipe%d)\n", pipe);
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_ERR);

        break;
    }
}
/******************************************************************************
 End of function usb_hstd_send_start
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_buf2fifo
 Description     : Set USB registers as required to write from data buffer to USB 
 : FIFO, to have USB FIFO to write data to bus.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe     : Pipe no.
 : uint16_t useport  : Port no.
 Return value    : none
 ******************************************************************************/
void usb_hstd_buf2fifo (usb_utr_t *ptr, uint16_t pipe, uint16_t useport)
{
    uint16_t end_flag;

    /* Disable Ready Interrupt */
    HW_USB_ClearBrdyenb(ptr, pipe);

    /* Ignore count clear */
    g_usb_hstd_ignore_cnt[ptr->ip][pipe] = (uint16_t) 0;

    end_flag = usb_hstd_write_data(ptr, pipe, useport);

    /* Check FIFO access sequence */
    switch (end_flag)
    {
        case USB_WRITING :

            /* Continue of data write */
            /* Enable Ready Interrupt */
            HW_USB_SetBrdyenb(ptr, pipe);

            /* Enable Not Ready Interrupt */
            usb_cstd_nrdy_enable(ptr, pipe);

        break;

        case USB_WRITEEND :

            /* End of data write */
            /* continue */

        case USB_WRITESHRT :

            /* End of data write */
            /* Enable Empty Interrupt */
            HW_USB_SetBempenb(ptr, pipe);

            /* Enable Not Ready Interrupt */
            usb_cstd_nrdy_enable(ptr, pipe);

        break;
        case USB_FIFOERROR :

            /* FIFO access error */
            USB_PRINTF0("### FIFO access error \n");
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_ERR);

        break;
        default :
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
 End of function usb_hstd_buf2fifo
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_write_data
 Description     : Switch PIPE, request the USB FIFO to write data, and manage 
 : the size of written data.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe         : Pipe no.
 : uint16_t pipemode     : CUSE/D0DMA/D1DMA
 Return value    : uint16_t end_flag
 ******************************************************************************/
uint16_t usb_hstd_write_data (usb_utr_t *ptr, uint16_t pipe, uint16_t pipemode)
{
    uint16_t size;
    uint16_t count;
    uint16_t buffer;
    uint16_t mxps;
    uint16_t end_flag;

    /* Changes FIFO port by the pipe. */
    if ((USB_CUSE == pipemode) && (USB_PIPE0 == pipe))
    {
        buffer = usb_cstd_is_set_frdy(ptr, pipe, (uint16_t) USB_CUSE, (uint16_t) USB_ISEL);
    }
    else
    {
        buffer = usb_cstd_is_set_frdy(ptr, pipe, (uint16_t) pipemode, USB_FALSE);
    }

    /* Check error */
    if (USB_FIFOERROR == buffer)
    {
        /* FIFO access error */
        return (USB_FIFOERROR);
    }

    /* Data buffer size */
    size = usb_cstd_get_buf_size(ptr, pipe);

    /* Max Packet Size */
    mxps = usb_cstd_get_maxpacket_size(ptr, pipe);

    /* Data size check */
    if (g_usb_hstd_data_cnt[ptr->ip][pipe] <= (uint32_t) size)
    {
        count = (uint16_t) g_usb_hstd_data_cnt[ptr->ip][pipe];

        /* Data count check */
        if (0 == count)
        {
            /* Null Packet is end of write */
            end_flag = USB_WRITESHRT;
        }
        else if ((count % mxps) != 0)
        {
            /* Short Packet is end of write */
            end_flag = USB_WRITESHRT;
        }
        else
        {
            if (USB_PIPE0 == pipe)
            {
                /* Just Send Size */
                end_flag = USB_WRITING;
            }
            else
            {
                /* Write continues */
                end_flag = USB_WRITEEND;
            }
        }
    }
    else
    {
        /* Write continues */
        end_flag = USB_WRITING;
        count = size;
    }

    g_usb_hstd_data_ptr[ptr->ip][pipe] = usb_hstd_write_fifo(ptr, count, pipemode, g_usb_hstd_data_ptr[ptr->ip][pipe]);

    /* Check data count to remain */
    if (g_usb_hstd_data_cnt[ptr->ip][pipe] < (uint32_t) size)
    {
        /* Clear data count */
        g_usb_hstd_data_cnt[ptr->ip][pipe] = (uint32_t) 0u;
        buffer = HW_USB_ReadFifoctr(ptr, pipemode); /* Read CFIFOCTR */

        /* Check BVAL */
        if ((buffer & USB_BVAL) == 0u)
        {
            /* Short Packet */
            HW_USB_SetBval(ptr, pipemode);
        }
    }
    else
    {
        /* Total data count - count */
        g_usb_hstd_data_cnt[ptr->ip][pipe] -= count;
    }

    /* End or Err or Continue */
    return end_flag;
}
/******************************************************************************
 End of function usb_hstd_write_data
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_receive_start
 Description     : Start data reception using CPU/DMA transfer to USB Host/USB
 : device.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe no.
 Return value    : none
 ******************************************************************************/
void usb_hstd_receive_start (usb_utr_t *ptr, uint16_t pipe)
{
    usb_utr_t *pp;
    uint32_t length;
    uint16_t mxps;
    uint16_t useport;
    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
    uint16_t dma_ch;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    /* Evacuation pointer */
    pp = g_usb_hstd_pipe[ptr->ip][pipe];
    length = pp->tranlen;

    /* Check transfer count */
    if (USB_TRAN_CONT == pp->segment)
    {
        /* Sequence toggle */
        usb_hstd_do_sqtgl(ptr, pipe, pp->pipectr);
    }

    /* Select NAK */
    usb_cstd_select_nak(ptr, pipe);
    g_usb_hstd_data_cnt[ptr->ip][pipe] = length; /* Set data count */
    g_usb_hstd_data_ptr[ptr->ip][pipe] = (uint8_t*) pp->tranadr; /* Set data pointer */

    /* Ignore count clear */
    g_usb_hstd_ignore_cnt[ptr->ip][pipe] = (uint16_t) 0u;

    /* Pipe number to FIFO port select */
    useport = usb_hstd_pipe2fport(ptr, pipe);

    /* Check use FIFO access */
    switch (useport)
    {
        /* D0FIFO use */
        case USB_D0USE :

            /* D0 FIFO access is NG */
            USB_PRINTF1("### USB-ITRON is not support(RCV-D0USE:pipe%d)\n", pipe);
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_ERR);
        break;

            /* CFIFO use */
        case USB_CUSE :

            /* continue */

            /* D1FIFO use */
        case USB_D1USE :

            /* Changes the FIFO port by the pipe. */
            usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
            mxps = usb_cstd_get_maxpacket_size(ptr, pipe); /* Max Packet Size */
            if ((uint32_t) 0u != length)
            {
                /* Data length check */
                if ((length % mxps) == (uint32_t) 0u)
                {
                    /* Set Transaction counter */
                    usb_cstd_set_transaction_counter(ptr, pipe, (uint16_t) (length / mxps));
                }
                else
                {
                    /* Set Transaction counter */
                    usb_cstd_set_transaction_counter(ptr, pipe, (uint16_t) ((length / mxps) + (uint32_t) 1u));
                }
            }

            usb_cstd_set_buf(ptr, pipe); /* Set BUF */

            /* Enable Ready Interrupt */
            HW_USB_SetBrdyenb(ptr, pipe);

            /* Enable Not Ready Interrupt */
            usb_cstd_nrdy_enable(ptr, pipe);
        break;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
            /* D1FIFO DMA */
            case USB_D1DMA:
            /* D0FIFO DMA */
            case USB_D0DMA:
            if (USB_D0DMA == useport)
            {
                dma_ch = USB_DMA_CH;
            }
            else
            {
                dma_ch = USB_DMA_CH2;
            }
            /* Setting for use PIPE number */
            g_usb_cstd_dma_pipe[ptr->ip][dma_ch] = pipe;
            /* PIPE direction */
            g_usb_cstd_dma_dir[ptr->ip][dma_ch] = usb_cstd_get_pipe_dir(ptr, pipe);
            /* Buffer size */
            g_usb_cstd_dma_fifo[ptr->ip][dma_ch] = usb_cstd_get_buf_size(ptr, pipe);

            /* Transfer data size */
            g_usb_cstd_dma_size[ptr->ip][dma_ch] = (uint16_t)g_usb_hstd_data_cnt[ptr->ip][pipe];
            usb_cstd_dxfifo2buf_start_dma( ptr, pipe, useport, length );

            break;

    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        default :
            USB_PRINTF1("### USB-ITRON is not support(RCV-else:pipe%d)\n", pipe);
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
 End of function usb_hstd_receive_start
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_fifo_to_buf
 Description     : Request readout from USB FIFO to buffer and process depending
 : on status; read complete or reading.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe  : Pipe no.
 Return value    : none
 ******************************************************************************/
void usb_hstd_fifo_to_buf (usb_utr_t *ptr, uint16_t pipe, uint16_t useport)
{
    uint16_t end_flag;

    /* Ignore count clear */
    g_usb_hstd_ignore_cnt[ptr->ip][pipe] = (uint16_t) 0;
    end_flag = USB_ERROR;

    end_flag = usb_hstd_read_data(ptr, pipe, useport);

    /* Check FIFO access sequence */
    switch (end_flag)
    {
        case USB_READING :

            /* Continue of data read */

        break;

        case USB_READEND :

            /* End of data read */
            usb_hstd_data_end(ptr, pipe, (uint16_t) USB_DATA_OK);

        break;

        case USB_READSHRT :

            /* End of data read */
            usb_hstd_data_end(ptr, pipe, (uint16_t) USB_DATA_SHT);

        break;

        case USB_READOVER :

            /* Buffer over */
            USB_PRINTF1("### Receive data over PIPE%d\n",pipe);
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_OVR);

        break;

        case USB_FIFOERROR :

            /* FIFO access error */
            USB_PRINTF0("### FIFO access error \n");
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_ERR);

        break;

        default :
            usb_hstd_forced_termination(ptr, pipe, (uint16_t) USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
 End of function usb_hstd_fifo_to_buf
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_read_data
 Description     : Request to read data from USB FIFO, and manage the size of 
 : the data read.
 Arguments       : uint16_t pipe            : Pipe no.
 Return value    : USB_READING / USB_READEND / USB_READSHRT / USB_READOVER
 ******************************************************************************/
uint16_t usb_hstd_read_data (usb_utr_t *ptr, uint16_t pipe, uint16_t pipemode)
{
    uint16_t count;
    uint16_t buffer;
    uint16_t mxps;
    uint16_t dtln;
    uint16_t end_flag;

    /* Changes FIFO port by the pipe. */
    buffer = usb_cstd_is_set_frdy(ptr, pipe, (uint16_t) pipemode, USB_FALSE);
    if (USB_FIFOERROR == buffer)
    {
        /* FIFO access error */
        return (USB_FIFOERROR);
    }
    dtln = (uint16_t) (buffer & USB_DTLN);

    /* Max Packet Size */
    mxps = usb_cstd_get_maxpacket_size(ptr, pipe);

    if (g_usb_hstd_data_cnt[ptr->ip][pipe] < dtln)
    {
        /* Buffer Over ? */
        end_flag = USB_READOVER;
        usb_cstd_set_nak(ptr, pipe); /* Set NAK */
        count = (uint16_t) g_usb_hstd_data_cnt[ptr->ip][pipe];
        g_usb_hstd_data_cnt[ptr->ip][pipe] = dtln;
    }
    else if (g_usb_hstd_data_cnt[ptr->ip][pipe] == dtln)
    {
        /* Just Receive Size */
        count = dtln;
        end_flag = USB_READEND;
        usb_cstd_select_nak(ptr, pipe); /* Set NAK */
    }
    else
    {
        /* Continus Receive data */
        count = dtln;
        end_flag = USB_READING;
        if ((0 == count) || (0 != (count % mxps)))
        {
            /* Null Packet receive */
            end_flag = USB_READSHRT;
            usb_cstd_select_nak(ptr, pipe); /* Select NAK */
        }
    }

    if (0 == dtln)
    {
        /* 0 length packet */
        /* Clear BVAL */
        HW_USB_SetBclr(ptr, pipemode);
    }
    else
    {
        g_usb_hstd_data_ptr[ptr->ip][pipe] = usb_hstd_read_fifo(ptr, count, pipemode,
                g_usb_hstd_data_ptr[ptr->ip][pipe]);
    }
    g_usb_hstd_data_cnt[ptr->ip][pipe] -= count;

    /* End or Err or Continue */
    return (end_flag);
}
/******************************************************************************
 End of function usb_hstd_read_data
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_data_end
 Description     : Set USB registers as appropriate after data transmission/re-
 : ception, and call the callback function as transmission/recep-
 : tion is complete.
 Arguments       : uint16_t pipe     : Pipe no.
 : uint16_t status   : Transfer status type.
 Return value    : none
 ******************************************************************************/
void usb_hstd_data_end (usb_utr_t *ptr, uint16_t pipe, uint16_t status)
{
    uint16_t useport;
    uint16_t ip;

    if (USB_NULL != ptr)
    {
        ip = ptr->ip;
    }
    else
    {
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
        /* USB0 */
        ip = USB_IP0;
        #else     /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        /* USB1/USBA */
        ip = USB_IP1;
        #endif    /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    #else   /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
        ip = USB_NULL;
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }

    /* PID = NAK */
    /* Set NAK */
    usb_cstd_select_nak(ptr, pipe);

    /* Pipe number to FIFO port select */
    useport = usb_hstd_pipe2fport(ptr, pipe);

    /* Disable Interrupt */
    /* Disable Ready Interrupt */
    HW_USB_ClearBrdyenb(ptr, pipe);

    /* Disable Not Ready Interrupt */
    HW_USB_ClearNrdyenb(ptr, pipe);

    /* Disable Empty Interrupt */
    HW_USB_ClearBempenb(ptr, pipe);

    /* Disable Transaction count */
    usb_cstd_clr_transaction_counter(ptr, pipe);

    /* Check use FIFO */
    switch (useport)
    {
        /* CFIFO use */
        case USB_CUSE :
        break;
            /* D0FIFO use */
        case USB_D0USE :
        break;
            /* D1FIFO use */
        case USB_D1USE :
        break;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
            /* D0FIFO DMA */
            case USB_D0DMA:
            /* DMA buffer clear mode clear */
            HW_USB_ClearDclrm( ptr, USB_D0DMA );
            if(ip == USB_USBIP_0)
            {
                HW_USB_SetMbw( ptr, USB_D0DMA, USB0_D0FIFO_MBW );
            }
            else if (ip == USB_USBIP_1)
            {
                HW_USB_SetMbw( ptr, USB_D0DMA, USB1_D0FIFO_MBW );
            }
            break;
            /* D1FIFO DMA */
            case USB_D1DMA:
            /* DMA buffer clear mode clear */
            HW_USB_ClearDclrm( ptr, USB_D1DMA );
            if(ip == USB_USBIP_0)
            {
                HW_USB_SetMbw( ptr, USB_D1DMA, USB0_D1FIFO_MBW );
            }
            else if (ip == USB_USBIP_1)
            {
                HW_USB_SetMbw( ptr, USB_D1DMA, USB1_D1FIFO_MBW );
            }
            break;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        default :
        break;
    }

    /* Call Back */
    if (USB_NULL != g_usb_hstd_pipe[ip][pipe])
    {
        /* Check PIPE TYPE */
        if (usb_cstd_get_pipe_type(ptr, pipe) != USB_TYPFIELD_ISO)
        {
            /* Transfer information set */
            g_usb_hstd_pipe[ip][pipe]->tranlen = g_usb_hstd_data_cnt[ip][pipe];
            g_usb_hstd_pipe[ip][pipe]->status = status;
            g_usb_hstd_pipe[ip][pipe]->pipectr = HW_USB_ReadPipectr(ptr, pipe);
            g_usb_hstd_pipe[ip][pipe]->errcnt = (uint8_t) g_usb_hstd_ignore_cnt[ip][pipe];
            g_usb_hstd_pipe[ip][pipe]->ipp = usb_hstd_get_usb_ip_adr(ip);
            g_usb_hstd_pipe[ip][pipe]->ip = ip;
            (g_usb_hstd_pipe[ip][pipe]->complete)(g_usb_hstd_pipe[ip][pipe], 0, 0);
            g_usb_hstd_pipe[ip][pipe] = (usb_utr_t*) USB_NULL;
        }
        else
        {
            /* Transfer information set */
            g_usb_hstd_pipe[ip][pipe]->tranlen = g_usb_hstd_data_cnt[ip][pipe];
            g_usb_hstd_pipe[ip][pipe]->pipectr = HW_USB_ReadPipectr(ptr, pipe);
            g_usb_hstd_pipe[ip][pipe]->errcnt = (uint8_t) g_usb_hstd_ignore_cnt[ip][pipe];
            g_usb_hstd_pipe[ip][pipe]->ipp = usb_hstd_get_usb_ip_adr(ip);
            g_usb_hstd_pipe[ip][pipe]->ip = ip;

            /* Data Transfer (restart) */
            if (usb_cstd_get_pipe_dir(ptr, pipe) == USB_BUF2FIFO)
            {
                /* OUT Transfer */
                g_usb_hstd_pipe[ip][pipe]->status = USB_DATA_WRITING;
                (g_usb_hstd_pipe[ip][pipe]->complete)(g_usb_hstd_pipe[ip][pipe], 0, 0);
            }
            else
            {
                /* IN Transfer */
                g_usb_hstd_pipe[ip][pipe]->status = USB_DATA_READING;
                (g_usb_hstd_pipe[ip][pipe]->complete)(g_usb_hstd_pipe[ip][pipe], 0, 0);
            }
        }
    }
}
/******************************************************************************
 End of function usb_hstd_data_end
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_class_request_complete
 Description     : Class request transfer complete
 Argument        : usb_utr_t *mess           : Transfer result message
 Return          : none
 ******************************************************************************/
void usb_class_request_complete (usb_utr_t *mess, uint16_t data1, uint16_t data2)
{
    usb_ctrl_t ctrl;

    if ( USB_CTRL_END == mess->status) /* Check usb receive status */
    {
        ctrl.status = USB_SUCCESS;
    }
    else
    {
        ctrl.status = USB_ERROR;
    }
    ctrl.type = USB_REQUEST;
    ctrl.module = mess->ip; /* Module number setting */
    ctrl.setup.type = mess->setup[0];
    ctrl.pipe = USB_PIPE0;
    ctrl.size = 0;
    ctrl.setup.value = mess->setup[1];
    ctrl.setup.index = mess->setup[2];
    ctrl.setup.length = mess->setup[3];
    ctrl.address = mess->setup[4];
    usb_set_event(USB_STS_REQUEST_COMPLETE, &ctrl); /* Set Event()  */
} /* End of function usb_class_request_complete() */

/******************************************************************************
 Function Name   : usb_hstd_set_pipe_registration
 Description     : Set pipe configuration of USB H/W. Set the content of the
 : specified pipe information table (2nd argument).
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : uint16_t *table       : DEF_EP table pointer.
 : uint16_t pipe         : pipe number.
 Return          : usb_err_t error code   : USB_OK etc.
 ******************************************************************************/
usb_er_t usb_hstd_set_pipe_registration (usb_utr_t *ptr, uint16_t *table, uint16_t pipe)
{
    usb_hstd_set_pipe_register(ptr, pipe, table);

    return USB_SUCCESS;
}/* End of function usb_hstd_set_pipe_registration() */

/******************************************************************************
 Function Name   : usb_hstd_transfer_end
 Description     : Request HCD to force termination of data transfer.
 Arguments       : usb_utr_t *ptr : USB system internal structure.
 : uint16_t *table       : DEF_EP table pointer
 : uint16_t pipe         : pipe number
 Return          : usb_err_t error code   : USB_OK etc
 ******************************************************************************/
usb_er_t usb_hstd_transfer_end (usb_utr_t *ptr, uint16_t pipe, uint16_t status)
{
    uint16_t msg;
    usb_er_t err;

    if (USB_NULL == g_usb_hstd_pipe[ptr->ip][pipe])
    {
        USB_PRINTF1("### usb_hstd_transfer_end overlaps %d\n", pipe);
        return USB_QOVR;
    }

    if (USB_DATA_TMO == status)
    {
        msg = USB_MSG_HCD_TRANSEND1;
    }
    else
    {
        msg = USB_MSG_HCD_TRANSEND2;
    }

    err = usb_hstd_hcd_snd_mbx(ptr, msg, pipe, (uint16_t*) 0, &usb_hstd_dummy_function);
    return err;
}/* End of function usb_hstd_transfer_end() */

/******************************************************************************
 Function Name   : usb_hstd_mgr_open
 Description     : Initialize global variable that contains registration status
 : of HDCD.
 : For RTOS version, start Manager(MGR) task
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 Return          : none
 ******************************************************************************/
usb_er_t usb_hstd_mgr_open (usb_utr_t *ptr)
{
    usb_er_t err = USB_OK;
    usb_hcdreg_t *driver;
    uint16_t i;

    /* Manager Mode */
    g_usb_hstd_mgr_mode[ptr->ip][0] = USB_DETACHED;
    g_usb_hstd_mgr_mode[ptr->ip][1] = USB_DETACHED;
    g_usb_hstd_device_speed[ptr->ip] = USB_NOCONNECT;

    /* Device address */
    g_usb_hstd_device_addr[ptr->ip] = USB_DEVICE_0;

    /* Device driver number */
    g_usb_hstd_device_num[ptr->ip] = 0;
    for (i = USB_PIPE0; i <= USB_MAX_PIPE_NO; i++)
    {
        g_usb_hstd_suspend_pipe[ptr->ip][i] = USB_OK;
    }

    for (i = 0; i < (USB_MAXDEVADDR + 1u); i++)
    {
        driver = &g_usb_hstd_device_drv[ptr->ip][i];

        driver->rootport = USB_NOPORT; /* Root port */
        driver->devaddr = USB_NODEVICE; /* Device address */
        driver->devstate = USB_DETACHED; /* Device state */
        driver->ifclass = (uint16_t) USB_IFCLS_NOT; /* Interface Class : NO class */
        g_usb_hstd_device_info[ptr->ip][i][0] = USB_NOPORT; /* Root port */
        g_usb_hstd_device_info[ptr->ip][i][1] = USB_DETACHED; /* Device state */
        g_usb_hstd_device_info[ptr->ip][i][2] = (uint16_t) 0; /* Not configured */
        g_usb_hstd_device_info[ptr->ip][i][3] = (uint16_t) USB_IFCLS_NOT; /* Interface Class : NO class */
        g_usb_hstd_device_info[ptr->ip][i][4] = (uint16_t) USB_NOCONNECT; /* No connect */
    }

    USB_PRINTF0("*** Install USB-MGR ***\n");

    usb_cstd_set_task_pri(USB_MGR_TSK, USB_PRI_2);

    return err;
}/* End of function usb_hstd_mgr_open() */

/******************************************************************************
 Function Name   : usb_hstd_driver_registration
 Description     : The HDCD information registered in the class driver structure
 : is registered in HCD.
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : usb_hcdreg_t *callback    : class driver structure
 Return          : none
 ******************************************************************************/
void usb_hstd_driver_registration (usb_utr_t *ptr, usb_hcdreg_t *callback)
{
    usb_hcdreg_t *driver;

    if (g_usb_hstd_device_num[ptr->ip] <= USB_MAXDEVADDR)
    {
        driver = &g_usb_hstd_device_drv[ptr->ip][g_usb_hstd_device_num[ptr->ip]];

        driver->rootport = USB_NOPORT; /* Root port */
        driver->devaddr = USB_NODEVICE; /* Device address */
        driver->devstate = USB_DETACHED; /* Device state */
        driver->ifclass = callback->ifclass; /* Interface Class */
        driver->tpl = callback->tpl; /* Target peripheral list */
        driver->pipetbl = callback->pipetbl; /* Pipe Define Table address */
        driver->classinit = callback->classinit; /* Driver init */
        driver->classcheck = callback->classcheck; /* Driver check */
        driver->devconfig = callback->devconfig; /* Device configured */
        driver->devdetach = callback->devdetach; /* Device detach */
        driver->devsuspend = callback->devsuspend; /* Device suspend */
        driver->devresume = callback->devresume; /* Device resume */

        /* Initialized device driver */
        (*driver->classinit)(ptr, driver->devaddr, (uint16_t) USB_NO_ARG);

        g_usb_hstd_device_num[ptr->ip]++;
        USB_PRINTF1("*** Registration driver 0x%02x\n",driver->ifclass);
    }
    else
    {
        USB_PRINTF0("### Registration device driver over\n");
    }
}/* End of function usb_hstd_driver_registration() */

/******************************************************************************
 Function Name   : usb_hstd_driver_release
 Description     : Release the Device Class Driver.
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : uint8_t   devclass    : Interface class
 Return          : none
 ******************************************************************************/
void usb_hstd_driver_release (usb_utr_t *ptr, uint8_t devclass)
{
    usb_hcdreg_t *driver;
    uint16_t i;
    uint16_t flg;

    if (USB_IFCLS_NOT == devclass)
    {
        /* Device driver number */
        g_usb_hstd_device_num[ptr->ip] = 0;
    }
    else
    {
        for (flg = 0u, i = 0u
        ; (i < (USB_MAXDEVADDR + 1u)) && (0u == flg); i++)
        {
            driver = &g_usb_hstd_device_drv[ptr->ip][i];
            if (driver->ifclass == devclass)
            {

                driver->rootport = USB_NOPORT; /* Root port */
                driver->devaddr = USB_NODEVICE; /* Device address */
                driver->devstate = USB_DETACHED; /* Device state */

                /* Interface Class : NO class */
                driver->ifclass = (uint16_t) USB_IFCLS_NOT;

                g_usb_hstd_device_num[ptr->ip]--;
                USB_PRINTF1("*** Release class %d driver ***\n",devclass);
                flg = 1u; /* break; */
            }
        }
    }
}/* End of function usb_hstd_driver_release() */

/******************************************************************************
 Function Name   : usb_hstd_chk_pipe_info
 Description     : Analyze descriptor information of the connected USB Device,
 and reflect it in the pipe information table.
 Argument        : uint16_t speed            : device speed
 : uint16_t *ep_tbl          : DEF_EP table pointer
 : uint8_t  *descriptor      : Endpoint Descriptor table
 Return          : uint16_t                  : DIR_H_IN / DIR_H_OUT / USB_ERROR
 ******************************************************************************/
uint16_t usb_hstd_chk_pipe_info (uint16_t speed, uint16_t *ep_tbl, uint8_t *descriptor)
{
    uint16_t pipe_cfg;
    uint16_t pipe_maxp;
    uint16_t pipe_peri;
    uint16_t retval;
    uint16_t work1;
    uint16_t work2;

    /* Check Endpoint descriptor */
    /* Descriptor Type */
    if (USB_DT_ENDPOINT == descriptor[1])
    {
        switch ((uint16_t) (descriptor[3] & USB_EP_TRNSMASK))
        {
            /* Control Endpoint */
            case USB_EP_CNTRL :
                USB_PRINTF0("###Control pipe is not support.\n");
                return USB_ERROR;
            break;

                /* Isochronous Endpoint */
            case USB_EP_ISO :
                if ((USB_PIPE1 != ep_tbl[0]) && (USB_PIPE2 != ep_tbl[0]))
                {
                    USB_PRINTF0("###Iso pipe is 1 or 2.\n");
                    return USB_ERROR;
                }
                USB_PRINTF0(" ISOCH ");
                pipe_cfg = USB_TYPFIELD_ISO;
            break;

                /* Bulk Endpoint */
            case USB_EP_BULK :
                if ((ep_tbl[0] < USB_PIPE1) || (ep_tbl[0] > USB_PIPE5))
                {
                    USB_PRINTF0("###Bulk pipe is 1 to 5.\n");
                    return USB_ERROR;
                }
                pipe_cfg = USB_TYPFIELD_BULK; /* USB_PRINTF0(" BULK "); */
            break;

                /* Interrupt Endpoint */
            case USB_EP_INT :
                if ((ep_tbl[0] < USB_PIPE6) || (ep_tbl[0] > USB_MAX_PIPE_NO))
                {
                    USB_PRINTF0("###Int pipe is 6 to 9.\n");
                    return USB_ERROR;
                }
                pipe_cfg = USB_TYPFIELD_INT; /* USB_PRINTF0(" INT "); */
            break;
            default :
                USB_PRINTF0("###Endpoint Descriptor error.\n");
                return USB_ERROR;
            break;
        }

        /* Set pipe configuration table */
        if ((descriptor[2] & USB_EP_DIRMASK) == USB_EP_IN)
        {
            /* IN(receive) */

            if ((descriptor[3] & USB_EP_TRNSMASK) != USB_EP_ISO)
            {
                /* Compulsory SHTNAK */
                pipe_cfg |= (USB_CFG_SHTNAKON | USB_DIR_H_IN);
            }
            else
            {
                pipe_cfg |= USB_DIR_H_IN;
            }
            pipe_cfg |= (uint16_t) (ep_tbl[1] & (USB_DBLBFIELD | USB_CNTMDFIELD));
            retval = USB_DIR_H_IN;
        }
        else
        {
            /* OUT(send) */
            pipe_cfg |= (uint16_t) ((uint16_t) (ep_tbl[1] & (USB_DBLBFIELD | USB_CNTMDFIELD)) | USB_DIR_H_OUT);
            retval = USB_DIR_H_OUT;
        }

        /* Endpoint number set */
        pipe_cfg |= (uint16_t) (descriptor[2] & USB_EP_NUMMASK);

        /* Max packet size set */
        pipe_maxp = (uint16_t) (descriptor[4] | (uint16_t) ((uint16_t) descriptor[5] << 8u));

        /* Buffer flash */
        pipe_peri = (uint16_t) (ep_tbl[4] & (~USB_IITVFIELD));
        if (0 != descriptor[6])
        {
            /* FS/LS interrupt */
            if (((uint16_t) (pipe_cfg & USB_TYPFIELD) == USB_TYPFIELD_INT) && (USB_HSCONNECT != speed))
            {
                work1 = descriptor[6];
                work2 = 0;
                for (; work1 != 0; work2++)
                {
                    work1 = (uint16_t) (work1 >> 1);
                }
                if (0 != work2)
                {
                    /* Interval time */
                    pipe_peri |= (uint16_t) (work2 - 1);
                }
            }
            else
            {
                if (descriptor[6] <= 8)
                {
                    /* Interval time */
                    pipe_peri |= (uint16_t) ((descriptor[6] - 1u) & (USB_IITVFIELD));
                }
                else
                {
                    /* Interval time */
                    pipe_peri |= ((uint16_t) (0x00FFu) & (USB_IITVFIELD));
                }
            }
        }
        ep_tbl[1] = pipe_cfg;
        ep_tbl[3] = pipe_maxp;
        ep_tbl[4] = pipe_peri;

        /* USB_PRINTF4(" pipe%d configuration %04X %04X %04X\n", ep_tbl[0], ep_tbl[1], ep_tbl[3], ep_tbl[4]); */
    }
    else
    {
        USB_PRINTF0("###Endpoint Descriptor error.\n");
        return USB_ERROR;
    }
    return (retval);
}/* End of function usb_hstd_chk_pipe_info */

/******************************************************************************
 Function Name   : usb_hstd_set_pipe_info
 Description     : Copy information of pipe information table from source
 (2nd argument) to destination (1st argument)
 Argument        : uint16_t *dst_ep_tbl      : DEF_EP table pointer(destination)
 : uint16_t *src_ep_tbl      : DEF_EP table pointer(source)
 : uint16_t length           : Table length
 Return          : none
 ******************************************************************************/
void usb_hstd_set_pipe_info (uint16_t *dst_ep_tbl, uint16_t *src_ep_tbl, uint16_t length)
{
    uint16_t i;

    /*USB_PRINTF3("!!! %lx %lx %d\n", dst_ep_tbl, src_ep_tbl, length);*/

    for (i = 0; i < length; i++)
    {
        dst_ep_tbl[i] = src_ep_tbl[i];
    }
} /* End of function usb_hstd_set_pipe_info() */

/******************************************************************************
 Function Name   : usb_hstd_return_enu_mgr
 Description     : Continuous enumeration is requested to MGR task (API for nonOS)
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : uint16_t cls_result   : class check result
 Return          : none
 ******************************************************************************/
void usb_hstd_return_enu_mgr (usb_utr_t *ptr, uint16_t cls_result)
{
    g_usb_hstd_check_enu_result[ptr->ip] = cls_result;
    usb_hstd_mgr_snd_mbx(ptr, USB_MSG_MGR_SUBMITRESULT, USB_PIPE0, USB_CTRL_END);
} /* End of function usb_hstd_return_enu_mgr() */

/******************************************************************************
 Function Name   : usb_hstd_enu_wait
 Description     : Request to change enumeration priority(API for nonOS)
 Argument        : usb_utr_t *ptr        : Pointer to a structure that includes
 the USB IP No. setting to change
 : uint16_t taskID       : Enumeration wait Task ID
 Return          : none
 ******************************************************************************/
void usb_hstd_enu_wait (usb_utr_t *ptr, uint8_t task_id)
{
    g_usb_hstd_enu_wait[ptr->ip] = task_id;
} /* usb_hstd_enu_wait */

/******************************************************************************
 Function Name   : usb_hstd_ChangeDeviceState
 Description     : Request to change the status of the connected USB Device
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : usb_cb_t complete          : callback function pointer
 : uint16_t msginfo           : Message Info
 : uint16_t member            : Device address/pipe number
 Return          : usb_err_t                   : USB_OK etc
 ******************************************************************************/
usb_er_t usb_hstd_ChangeDeviceState (usb_utr_t *ptr, usb_cb_t complete, uint16_t msginfo, uint16_t member)
{
    usb_mh_t p_blf;
    usb_er_t err;
    usb_er_t err2;
    usb_hcdinfo_t *hp;

    switch (msginfo)
    {
        /* USB_MSG_HCD_CLR_STALL */
        case USB_DO_CLR_STALL :
            err = usb_hstd_change_device_state(ptr, complete, USB_MSG_HCD_CLR_STALL, member);
        break;

            /* USB_MSG_HCD_SQTGLBIT */
        case USB_DO_SET_SQTGL :
            err = usb_hstd_change_device_state(ptr, complete, USB_MSG_HCD_SETSEQBIT, member);
        break;

            /* USB_MSG_HCD_CLRSEQBIT */
        case USB_DO_CLR_SQTGL :
            err = usb_hstd_change_device_state(ptr, complete, USB_MSG_HCD_CLRSEQBIT, member);
        break;

        default :

            /* Get mem pool blk */
            err = USB_PGET_BLK(USB_MGR_MPL, &p_blf);
            if (err == USB_SUCCESS)
            {
                USB_PRINTF2("*** member%d : msginfo=%d ***\n", member, msginfo);
                hp = (usb_hcdinfo_t*) p_blf;
                hp->msghead = (usb_mh_t) USB_NULL;
                hp->msginfo = msginfo;
                hp->keyword = member;
                hp->complete = complete;

                hp->ipp = ptr->ipp;
                hp->ip = ptr->ip;

                /* Send message */
                err = USB_SND_MSG(USB_MGR_MBX, (usb_msg_t* )p_blf);
                if (err != USB_SUCCESS)
                {
                    USB_PRINTF1("### hMgrChangeDeviceState snd_msg error (%ld)\n", err);
                    err2 = USB_REL_BLK(USB_MGR_MPL, (usb_mh_t )p_blf);
                    if (err2 != USB_SUCCESS)
                    {
                        USB_PRINTF1("### hMgrChangeDeviceState rel_blk error (%ld)\n", err2);
                    }
                }
            }
            else
            {
                USB_PRINTF1("### hMgrChangeDeviceState pget_blk error (%ld)\n", err);
                err = USB_ERROR;
            }
        break;
    }
    return err;
}
/* End of function usb_hstd_ChangeDeviceState() */

/******************************************************************************
 Function Name   : usb_hstd_hcd_open
 Description     : Start HCD(Host Control Driver) task
 Argument        : usb_utr_t *ptr      : Pointer to a structure that includes USB IP No. setting
 Return          : usb_err_t            : USB_OK etc
 ******************************************************************************/
usb_err_t usb_hstd_hcd_open (usb_utr_t *ptr)
{
    uint16_t i;
    uint16_t j;
    usb_err_t err = USB_SUCCESS;

    if (USB_MAXDEVADDR < USB_DEVICEADDR)
    {
        USB_PRINTF0("Device address error\n");

        /* >yes no process */
        return USB_ERR_NG;
    }

    /* Global Init */
    /* Control transfer stage management */
    g_usb_hstd_ctsq[ptr->ip] = USB_IDLEST;

    g_usb_hstd_remort_port[0] = USB_DEFAULT;
    g_usb_hstd_remort_port[1] = USB_DEFAULT;

    for (j = 0; j < USB_NUM_USBIP; ++j)
    {
        for (i = USB_PIPE0; i <= USB_MAX_PIPE_NO; i++)
        {
            g_usb_hstd_pipe[j][i] = (usb_utr_t*) USB_NULL;
        }
    }

    #if USB_CFG_BC == USB_CFG_ENABLE
    if(USB_BC_SUPPORT_IP == ptr->ip)
    {
        g_usb_hstd_bc[ptr->ip].state = USB_BC_STATE_INIT;
    }
    #endif

    USB_PRINTF0("*** Install USB-HCD ***\n");

    usb_cstd_set_task_pri(USB_HCD_TSK, USB_PRI_1);

    return err;
}/* End of function usb_hstd_hcd_open */

#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : usb_pstd_send_start
 Description     : Start data transmission using CPU/DMA transfer to USB host.
 Arguments       : uint16_t pipe  : Pipe no.
 Return value    : none
 ******************************************************************************/
void usb_pstd_send_start(uint16_t pipe)
{
    usb_utr_t *pp;
    uint32_t length;
    uint16_t useport;
    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
    uint16_t ip;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    /* Evacuation pointer */
    pp = g_usb_pstd_pipe[pipe];
    length = pp->tranlen;

    /* Select NAK */
    usb_cstd_select_nak(USB_NULL, pipe);
    /* Set data count */
    g_usb_pstd_data_cnt[pipe] = length;
    /* Set data pointer */
    g_usb_pstd_data_ptr[pipe] = (uint8_t*)pp->tranadr;

    /* BEMP Status Clear */
    HW_USB_ClearStatusBemp(USB_NULL,pipe);
    /* BRDY Status Clear */
    HW_USB_ClearStsBrdy(USB_NULL,pipe);

    /* Pipe number to FIFO port select */
    useport = usb_pstd_pipe2fport(pipe);
    /* Check use FIFO access */
    switch (useport)
    {
        /* CFIFO use */
        case USB_CUSE:
        /* Buffer to FIFO data write */
        usb_pstd_buf2fifo(pipe, useport);
        /* Set BUF */
        usb_cstd_set_buf(USB_NULL, pipe);
        break;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
        /* D0FIFO DMA */
        case USB_D0DMA:
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
        ip = USB_IP0;
        #else   /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        ip = USB_IP1;
        #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */

        /* Setting for use PIPE number */
        g_usb_cstd_dma_pipe[ip][USB_DMA_CH] = pipe;
        /* PIPE direction */
        g_usb_cstd_dma_dir[ip][USB_DMA_CH] = usb_cstd_get_pipe_dir(USB_NULL, pipe);
        /* Buffer size */
        g_usb_cstd_dma_fifo[ip][USB_DMA_CH] = usb_cstd_get_buf_size(USB_NULL, pipe);
        /* Check data count */
        if (g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH]] < g_usb_cstd_dma_fifo[ip][USB_DMA_CH])
        {
            /* Transfer data size */
            g_usb_cstd_dma_size[ip][USB_DMA_CH] = g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH]];
        }
        else
        {
            /* Transfer data size */
            g_usb_cstd_dma_size[ip][USB_DMA_CH] = g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH]]
            - g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH]] % g_usb_cstd_dma_fifo[ip][USB_DMA_CH];
        }

        usb_cstd_buf2dxfifo_start_dma(USB_NULL, pipe, useport);
        /* Set BUF */
        usb_cstd_set_buf(USB_NULL, pipe);
        break;

        /* D1FIFO DMA */
        case USB_D1DMA:
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
        ip = USB_IP0;
        #else   /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        ip = USB_IP1;
        #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */

        /* Setting for use PIPE number */
        g_usb_cstd_dma_pipe[ip][USB_DMA_CH2] = pipe;
        /* PIPE direction */
        g_usb_cstd_dma_dir[ip][USB_DMA_CH2] = usb_cstd_get_pipe_dir(USB_NULL, pipe);
        /* Buffer size */
        g_usb_cstd_dma_fifo[ip][USB_DMA_CH2] = usb_cstd_get_buf_size(USB_NULL, pipe);
        /* Check data count */
        if (g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH2]] < g_usb_cstd_dma_fifo[ip][USB_DMA_CH2])
        {
            /* Transfer data size */
            g_usb_cstd_dma_size[ip][USB_DMA_CH2] = g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH2]];
        }
        else
        {
            /* Transfer data size */
            g_usb_cstd_dma_size[ip][USB_DMA_CH2] = g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH2]]
            - g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH2]] % g_usb_cstd_dma_fifo[ip][USB_DMA_CH2];
        }

        usb_cstd_buf2dxfifo_start_dma(USB_NULL, pipe, useport);
        /* Set BUF */
        usb_cstd_set_buf(USB_NULL, pipe);
        break;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        default:
        /* Access is NG */
        USB_PRINTF0("### USB-FW is not support\n");
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
 End of function usb_pstd_send_start
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_buf2fifo
 Description     : Set USB registers as required to write from data buffer to USB
 : FIFO, to have USB FIFO to write data to bus.
 Arguments       : uint16_t pipe     : Pipe no.
 : uint16_t useport  : Port no.
 Return value    : none
 ******************************************************************************/
void usb_pstd_buf2fifo(uint16_t pipe, uint16_t useport)
{
    uint16_t end_flag;

    /* Disable Ready Interrupt */
    HW_USB_ClearBrdyenb(USB_NULL,pipe);

    end_flag = usb_pstd_write_data(pipe, useport);

    /* Check FIFO access sequence */
    switch (end_flag)
    {
        case USB_WRITING:
        /* Continue of data write */
        /* Enable Ready Interrupt */
        HW_USB_SetBrdyenb(USB_NULL,pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_nrdy_enable(USB_NULL, pipe);
        break;
        case USB_WRITEEND:
        /* End of data write */
        /* continue */
        case USB_WRITESHRT:
        /* End of data write */
        /* Enable Empty Interrupt */
        HW_USB_SetBempenb(USB_NULL,pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_nrdy_enable(USB_NULL, pipe);
        break;
        case USB_FIFOERROR:
        /* FIFO access error */
        USB_PRINTF0("### FIFO access error \n");
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;
        default:
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
 End of function usb_pstd_buf2fifo
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_write_data
 Description     : Switch PIPE, request the USB FIFO to write data, and manage
 : the size of written data.
 Arguments       : uint16_t pipe         : Pipe no.
 : uint16_t pipemode     : CUSE/D0DMA/D1DMA
 Return value    : uint16_t end_flag
 ******************************************************************************/
uint16_t usb_pstd_write_data(uint16_t pipe, uint16_t pipemode)
{
    uint16_t size;
    uint16_t count;
    uint16_t buffer;
    uint16_t mxps;
    uint16_t end_flag;

    /* Changes FIFO port by the pipe. */
    if ((USB_CUSE == pipemode) && (USB_PIPE0 == pipe))
    {
        buffer = usb_cstd_is_set_frdy(USB_NULL, pipe, (uint16_t)USB_CUSE, (uint16_t)USB_ISEL);
    }
    else
    {
        buffer = usb_cstd_is_set_frdy(USB_NULL, pipe, (uint16_t)pipemode, USB_FALSE);
    }

    /* Check error */
    if (USB_FIFOERROR == buffer)
    {
        /* FIFO access error */
        return (USB_FIFOERROR);
    }
    /* Data buffer size */
    size = usb_cstd_get_buf_size(USB_NULL, pipe);
    /* Max Packet Size */
    mxps = usb_cstd_get_maxpacket_size(USB_NULL, pipe);

    /* Data size check */
    if (g_usb_pstd_data_cnt[pipe] <= (uint32_t)size)
    {
        count = (uint16_t)g_usb_pstd_data_cnt[pipe];
        /* Data count check */
        if (0 == count)
        {
            /* Null Packet is end of write */
            end_flag = USB_WRITESHRT;
        }
        else if ((count % mxps) != 0)
        {
            /* Short Packet is end of write */
            end_flag = USB_WRITESHRT;
        }
        else
        {
            if (USB_PIPE0 == pipe)
            {
                /* Just Send Size */
                end_flag = USB_WRITING;
            }
            else
            {
                /* Write continues */
                end_flag = USB_WRITEEND;
            }
        }
    }
    else
    {
        /* Write continues */
        end_flag = USB_WRITING;
        count = size;
    }

    g_usb_pstd_data_ptr[pipe] = usb_pstd_write_fifo(count, pipemode, g_usb_pstd_data_ptr[pipe]);

    /* Check data count to remain */
    if (g_usb_pstd_data_cnt[pipe] < (uint32_t)size)
    {
        /* Clear data count */
        g_usb_pstd_data_cnt[pipe] = (uint32_t)0u;
        /* Read CFIFOCTR */
        buffer = HW_USB_ReadFifoctr(USB_NULL,pipemode);
        /* Check BVAL */
        if ((buffer & USB_BVAL) == 0u)
        {
            /* Short Packet */
            HW_USB_SetBval(USB_NULL,pipemode);
        }
    }
    else
    {
        /* Total data count - count */
        g_usb_pstd_data_cnt[pipe] -= count;
    }
    /* End or Err or Continue */
    return end_flag;
}
/******************************************************************************
 End of function usb_pstd_write_data
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_receive_start
 Description     : Start data reception using CPU/DMA transfer to USB Host.
 Arguments       : uint16_t pipe  : Pipe no.
 Return value    : none
 ******************************************************************************/
void usb_pstd_receive_start(uint16_t pipe)
{
    usb_utr_t *pp;
    uint32_t length;
    uint16_t mxps;
    uint16_t useport;
    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
    uint16_t ip;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    /* Evacuation pointer */
    pp = g_usb_pstd_pipe[pipe];
    length = pp->tranlen;

    /* Select NAK */
    usb_cstd_select_nak(USB_NULL, pipe);
    /* Set data count */
    g_usb_pstd_data_cnt[pipe] = length;
    /* Set data pointer */
    g_usb_pstd_data_ptr[pipe] = (uint8_t*)pp->tranadr;

    /* Pipe number to FIFO port select */
    useport = usb_pstd_pipe2fport(pipe);

    /* Check use FIFO access */
    switch (useport)
    {
        /* CFIFO use */
        case USB_CUSE:
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(USB_NULL, pipe, useport, USB_FALSE);
        /* Max Packet Size */
        mxps = usb_cstd_get_maxpacket_size(USB_NULL, pipe);
        if ((uint32_t)0u != length)
        {
            /* Data length check */
            if ((length % mxps) == (uint32_t)0u)
            {
                /* Set Transaction counter */
                usb_cstd_set_transaction_counter(USB_NULL, pipe, (uint16_t)(length / mxps));
            }
            else
            {
                /* Set Transaction counter */
                usb_cstd_set_transaction_counter(USB_NULL, pipe, (uint16_t)((length / mxps) + (uint32_t)1u));
            }
        }
        /* Set BUF */
        usb_cstd_set_buf(USB_NULL, pipe);
        /* Enable Ready Interrupt */
        HW_USB_SetBrdyenb(USB_NULL,pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_nrdy_enable(USB_NULL, pipe);
        break;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
        /* D0FIFO DMA */
        case USB_D0DMA:
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
        ip = USB_IP0;
        #else   /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        ip = USB_IP1;
        #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */

        /* Setting for use PIPE number */
        g_usb_cstd_dma_pipe[ip][USB_DMA_CH] = pipe;
        /* PIPE direction */
        g_usb_cstd_dma_dir[ip][USB_DMA_CH] = usb_cstd_get_pipe_dir(USB_NULL, pipe);
        /* Buffer size */
        g_usb_cstd_dma_fifo[ip][USB_DMA_CH] = usb_cstd_get_buf_size(USB_NULL, pipe);
        /* Transfer data size */
        g_usb_cstd_dma_size[ip][USB_DMA_CH]
        = g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH]];
        usb_cstd_dxfifo2buf_start_dma(USB_NULL, pipe, useport, length);
        break;

        /* D1FIFOB DMA */
        case USB_D1DMA:
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
        ip = USB_IP0;
        #else   /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        ip = USB_IP1;
        #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */

        /* Setting for use PIPE number */
        g_usb_cstd_dma_pipe[ip][USB_DMA_CH2] = pipe;
        /* PIPE direction */
        g_usb_cstd_dma_dir[ip][USB_DMA_CH2] = usb_cstd_get_pipe_dir(USB_NULL, pipe);
        /* Buffer size */
        g_usb_cstd_dma_fifo[ip][USB_DMA_CH2] = usb_cstd_get_buf_size(USB_NULL, pipe);
        /* Check data count */
        g_usb_cstd_dma_size[ip][USB_DMA_CH2]
        = g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[ip][USB_DMA_CH2]];
        usb_cstd_dxfifo2buf_start_dma(USB_NULL, pipe, useport, length);
        break;

    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        default:
        USB_PRINTF0("### USB-FW is not support\n");
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
 End of function usb_pstd_receive_start
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_fifo_to_buf
 Description     : Request readout from USB FIFO to buffer and process depending
 : on status; read complete or reading.
 Arguments       : uint16_t pipe     : Pipe no.
 : uint16_t useport  : FIFO select(USB_CUSE,USB_DMA0,....)
 Return value    : none
 ******************************************************************************/
void usb_pstd_fifo_to_buf(uint16_t pipe, uint16_t useport)
{
    uint16_t end_flag;

    end_flag = USB_ERROR;

    end_flag = usb_pstd_read_data(pipe, useport);

    /* Check FIFO access sequence */
    switch (end_flag)
    {
        case USB_READING:
        /* Continue of data read */
        break;

        case USB_READEND:
        /* End of data read */
        usb_pstd_data_end(pipe, (uint16_t)USB_DATA_OK);
        break;

        case USB_READSHRT:
        /* End of data read */
        usb_pstd_data_end(pipe, (uint16_t)USB_DATA_SHT);
        break;

        case USB_READOVER:
        /* Buffer over */
        USB_PRINTF1("### Receive data over PIPE%d\n", pipe);
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_OVR);
        break;

        case USB_FIFOERROR:
        /* FIFO access error */
        USB_PRINTF0("### FIFO access error \n");
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;

        default:
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
 End of function usb_pstd_fifo_to_buf
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_read_data
 Description     : Request to read data from USB FIFO, and manage the size of
 : the data read.
 Arguments       : uint16_t pipe         : Pipe no.
 : uint16_t pipemode     : FIFO select(USB_CUSE,USB_DMA0,....)
 Return value    : uint16_t end_flag
 ******************************************************************************/
uint16_t usb_pstd_read_data(uint16_t pipe, uint16_t pipemode)
{
    uint16_t count;
    uint16_t buffer;
    uint16_t mxps;
    uint16_t dtln;
    uint16_t end_flag;

    /* Changes FIFO port by the pipe. */
    buffer = usb_cstd_is_set_frdy(USB_NULL, pipe, (uint16_t)pipemode, USB_FALSE);
    if (USB_FIFOERROR == buffer)
    {
        /* FIFO access error */
        return (USB_FIFOERROR);
    }
    dtln = (uint16_t)(buffer & USB_DTLN);
    /* Max Packet Size */
    mxps = usb_cstd_get_maxpacket_size(USB_NULL, pipe);

    if (g_usb_pstd_data_cnt[pipe] < dtln)
    {
        /* Buffer Over ? */
        end_flag = USB_READOVER;
        /* Set NAK */
        usb_cstd_set_nak(USB_NULL, pipe);
        count = (uint16_t)g_usb_pstd_data_cnt[pipe];
        g_usb_pstd_data_cnt[pipe] = dtln;
    }
    else if (g_usb_pstd_data_cnt[pipe] == dtln)
    {
        /* Just Receive Size */
        count = dtln;
        if ((USB_PIPE0 == pipe) && ((dtln % mxps) == 0))
        {
            /* Just Receive Size */
            /* Peripheral Function */
            end_flag = USB_READING;
        }
        else
        {
            end_flag = USB_READEND;
            /* Set NAK */
            usb_cstd_select_nak(USB_NULL, pipe);
        }
    }
    else
    {
        /* Continus Receive data */
        count = dtln;
        end_flag = USB_READING;
        if (0 == count)
        {
            /* Null Packet receive */
            end_flag = USB_READSHRT;
            /* Select NAK */
            usb_cstd_select_nak(USB_NULL, pipe);
        }
        if ((count % mxps) != 0)
        {
            /* Null Packet receive */
            end_flag = USB_READSHRT;
            /* Select NAK */
            usb_cstd_select_nak(USB_NULL, pipe);
        }
    }

    if (0 == dtln)
    {
        /* 0 length packet */
        /* Clear BVAL */
        HW_USB_SetBclr(USB_NULL,pipemode);
    }
    else
    {
        g_usb_pstd_data_ptr[pipe] = usb_pstd_read_fifo(count, pipemode, g_usb_pstd_data_ptr[pipe]);
    }
    g_usb_pstd_data_cnt[pipe] -= count;

    /* End or Err or Continue */
    return (end_flag);
}
/******************************************************************************
 End of function usb_pstd_read_data
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_data_end
 Description     : Set USB registers as appropriate after data transmission/re-
 : ception, and call the callback function as transmission/recep-
 : tion is complete.
 Arguments       : uint16_t pipe     : Pipe no.
 : uint16_t status   : Transfer status type.
 Return value    : none
 ******************************************************************************/
void usb_pstd_data_end(uint16_t pipe, uint16_t status)
{
    uint16_t useport;
    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
    uint16_t ip;

        #if USB_CFG_USE_USBIP == USB_CFG_IP0
    ip = USB_IP0;
        #else   /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    ip = USB_IP1;
        #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    /* PID = NAK */
    /* Set NAK */
    usb_cstd_select_nak(USB_NULL, pipe);
    /* Pipe number to FIFO port select */
    useport = usb_pstd_pipe2fport(pipe);

    /* Disable Interrupt */
    /* Disable Ready Interrupt */
    HW_USB_ClearBrdyenb(USB_NULL,pipe);
    /* Disable Not Ready Interrupt */
    HW_USB_ClearNrdyenb(USB_NULL,pipe);
    /* Disable Empty Interrupt */
    HW_USB_ClearBempenb(USB_NULL,pipe);

    /* Disable Transaction count */
    usb_cstd_clr_transaction_counter(USB_NULL, pipe);

    /* Check use FIFO */
    switch (useport)
    {
        /* CFIFO use */
        case USB_CUSE:
        break;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
        /* D0FIFO DMA */
        case USB_D0DMA:
        /* DMA buffer clear mode clear */
        HW_USB_ClearDclrm(USB_NULL, useport);
        if(ip == USB_USBIP_0)
        {
            HW_USB_SetMbw( USB_NULL, USB_D0DMA, USB0_D0FIFO_MBW );
        }
        else if (ip == USB_USBIP_1)
        {
            HW_USB_SetMbw( USB_NULL, USB_D0DMA, USB1_D0FIFO_MBW );
        }
        break;

        /* D1FIFO DMA */
        case USB_D1DMA:
        /* DMA buffer clear mode clear */
        HW_USB_ClearDclrm(USB_NULL, useport);
        if(ip == USB_USBIP_0)
        {
            HW_USB_SetMbw( USB_NULL, USB_D1DMA, USB0_D1FIFO_MBW );
        }
        else if (ip == USB_USBIP_1)
        {
            HW_USB_SetMbw( USB_NULL, USB_D1DMA, USB1_D1FIFO_MBW );
        }
        break;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        default:
        break;
    }

    /* Call Back */
    if (USB_NULL != g_usb_pstd_pipe[pipe])
    {
        /* Check PIPE TYPE */
        if (usb_cstd_get_pipe_type(USB_NULL, pipe) != USB_ISO)
        {
            /* Transfer information set */
            g_usb_pstd_pipe[pipe]->tranlen = g_usb_pstd_data_cnt[pipe];
            g_usb_pstd_pipe[pipe]->status = status;
            g_usb_pstd_pipe[pipe]->pipectr = HW_USB_ReadPipectr(USB_NULL,pipe);
            g_usb_pstd_pipe[pipe]->keyword = pipe;
            ((usb_cb_t)g_usb_pstd_pipe[pipe]->complete)(g_usb_pstd_pipe[pipe], USB_NULL, USB_NULL);
            g_usb_pstd_pipe[pipe] = (usb_utr_t*)USB_NULL;
        }
        else
        {
            /* Transfer information set */
            g_usb_pstd_pipe[pipe]->tranlen = g_usb_pstd_data_cnt[pipe];
            g_usb_pstd_pipe[pipe]->pipectr = HW_USB_ReadPipectr(USB_NULL,pipe);

            /* Data Transfer (restart) */
            if (usb_cstd_get_pipe_dir(USB_NULL, pipe) == USB_BUF2FIFO)
            {
                /* OUT Transfer */
                g_usb_pstd_pipe[pipe]->status = USB_DATA_WRITING;
                ((usb_cb_t)g_usb_pstd_pipe[pipe]->complete)(g_usb_pstd_pipe[pipe], USB_NULL, USB_NULL);
            }
            else
            {
                /* IN Transfer */
                g_usb_pstd_pipe[pipe]->status = USB_DATA_READING;
                ((usb_cb_t)g_usb_pstd_pipe[pipe]->complete)(g_usb_pstd_pipe[pipe], USB_NULL, USB_NULL);
            }
        }
    }
}
/******************************************************************************
 End of function usb_pstd_data_end
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_transfer_start
 Description     : Transfer the data of each pipe
 : Request PCD to transfer data, and the PCD transfers the data
 based on the transfer information stored in ptr
 Arguments       : usb_utr_t *ptr : keyword, msghead and msginfo are used for...
 Return value    : usb_er_t       : Error info.
 ******************************************************************************/
usb_er_t usb_pstd_transfer_start(usb_utr_t * ptr)
{
    usb_er_t err;
    uint16_t pipenum;

    pipenum = ptr->keyword;
    if (USB_NULL != g_usb_pstd_pipe[pipenum])
    {
        /* Get PIPE TYPE */
        if (usb_cstd_get_pipe_type(USB_NULL, pipenum) != USB_ISO)
        {
            USB_PRINTF1("### usb_pstd_transfer_start overlaps %d\n", pipenum);
            return USB_QOVR;
        }
    }

    /* Check state (Configured) */
    if (usb_pstd_chk_configured() != USB_TRUE)
    {
        USB_PRINTF0("### usb_pstd_transfer_start not configured\n");
        return USB_ERROR;
    }

    if (USB_PIPE0 == pipenum)
    {
        USB_PRINTF0("### usb_pstd_transfer_start PIPE0 is not support\n");
        return USB_ERROR;
    }

    err = usb_pstd_set_submitutr(ptr);
    return err;
}
/******************************************************************************
 End of function usb_pstd_transfer_start
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_transfer_end
 Description     : Force termination of data transfer of specified pipe. Request
 : PCD to force termination
 data transfer, and the PCD forced-terminates data transfer.
 Arguments       : uint16_t pipe     : Pipe number.
 Return value    : usb_er_t          : Error info.
 ******************************************************************************/
usb_er_t usb_pstd_transfer_end(uint16_t pipe)
{
    if (USB_NULL == g_usb_pstd_pipe[pipe])
    {
        USB_PRINTF0("### usb_pstd_transfer_end overlaps\n");
        return USB_ERROR;
    }
    else
    {
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_STOP);
    }

    return USB_OK;
}
/******************************************************************************
 End of function usb_pstd_transfer_end
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_change_device_state
 Description     : Change USB Device to the status specified by argument
 Arguments       : uint16_t state            : New device status.
 : uint16_t keyword          : Pipe number etc.
 : USB_CB_t complete         : Callback function.
 Return value    : none
 ******************************************************************************/
void usb_pstd_change_device_state(uint16_t state, uint16_t keyword, usb_cb_t complete)
{
    uint16_t pipenum;

    pipenum = keyword;
    switch(state)
    {
        case USB_DO_STALL:
        usb_pstd_set_stall(pipenum);
        g_usb_pstd_stall_pipe[pipenum] = USB_TRUE;
        g_usb_pstd_stall_cb = complete;
        break;

        case USB_DO_REMOTEWAKEUP:
        usb_pstd_self_clock();
        usb_pstd_remote_wakeup();
        break;

        case USB_MSG_PCD_DP_ENABLE:
        usb_pstd_dp_enable();
        break;

        case USB_MSG_PCD_DP_DISABLE:
        usb_pstd_dp_disable();
        break;
        default:
        break;
    }
}
/******************************************************************************
 End of function usb_pstd_change_device_state
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_driver_registration
 Description     : Register pipe information table, descriptor information table,
 : callback function, etc. This info is specified by the data in
 : the structure usb_pcdreg_t.
 Arguments       : usb_pcdreg_t *registinfo : Class driver structure.
 Return value    : none
 ******************************************************************************/
void usb_pstd_driver_registration(usb_pcdreg_t * registinfo)
{
    usb_pcdreg_t *driver;

    driver = &g_usb_pstd_driver;

    driver->pipetbl = registinfo->pipetbl;       /* Pipe define table address */
    driver->devicetbl = registinfo->devicetbl;   /* Device descriptor table address */
    driver->qualitbl = registinfo->qualitbl;     /* Qualifier descriptor table address */
    driver->configtbl = registinfo->configtbl;   /* Configuration descriptor table address */
    driver->othertbl = registinfo->othertbl;     /* Other configuration descriptor table address */
    driver->stringtbl = registinfo->stringtbl;   /* String descriptor table address */
    driver->devdefault = registinfo->devdefault; /* Device default */
    driver->devconfig = registinfo->devconfig;   /* Device configured */
    driver->devdetach = registinfo->devdetach;   /* Device detach */
    driver->devsuspend = registinfo->devsuspend; /* Device suspend */
    driver->devresume = registinfo->devresume;   /* Device resume */
    driver->interface = registinfo->interface;   /* Interfaced change */
    driver->ctrltrans = registinfo->ctrltrans;   /* Control transfer */
}
/******************************************************************************
 End of function  usb_pstd_driver_registration
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_driver_release
 Description     : Clear the information registered in the structure usb_pcdreg_t.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_driver_release(void)
{
    usb_pcdreg_t *driver;

    driver = &g_usb_pstd_driver;
    /* Pipe define table address */
    driver->pipetbl = (uint16_t*)0u;
    /* Device descriptor table address */
    driver->devicetbl = (uint8_t*)0u;
    /* Qualifier descriptor table address */
    driver->qualitbl = (uint8_t*)0u;
    /* Configuration descriptor table address */
    driver->configtbl = (uint8_t*)0u;
    /* Other configuration descriptor table address */
    driver->othertbl = (uint8_t*)0u;
    /* String descriptor table address */
    driver->stringtbl = (uint8_t**)0u;
    /* Device default */
    driver->devdefault = &usb_pstd_dummy_function;
    /* Device configured */
    driver->devconfig = &usb_pstd_dummy_function;
    /* Device detach */
    driver->devdetach = &usb_pstd_dummy_function;
    /* Device suspend */
    driver->devsuspend = &usb_pstd_dummy_function;
    /* Device resume */
    driver->devresume = &usb_pstd_dummy_function;
    /* Interfaced change */
    driver->interface = &usb_pstd_dummy_function;
    /* Control transfer */
    driver->ctrltrans = &usb_pstd_dummy_trn;
}/* End of function usb_pstd_driver_release() */

#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */

/******************************************************************************
 End  Of File
 ******************************************************************************/
