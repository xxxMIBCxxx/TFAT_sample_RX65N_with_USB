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
 * File Name    : r_usb_pcontrolrw.c
 * Description  : USB Peripheral control transfer API code
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

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Renesas Abstracted Peripheral Control RW API functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_ctrl_read
 Description     : Called by R_USB_PstdCtrlRead, see it for description.
 Arguments       : uint32_t bsize    : Read size in bytes.
 : uint8_t *table    : Start address of read data buffer.
 Return value    : uint16_t          : USB_WRITESHRT/USB_WRITE_END/USB_WRITING/
 :                   : USB_FIFOERROR.
 ******************************************************************************/
uint16_t usb_pstd_ctrl_read (uint32_t bsize, uint8_t *table)
{
    uint16_t end_flag;

    g_usb_pstd_data_cnt[USB_PIPE0] = bsize;
    g_usb_pstd_data_ptr[USB_PIPE0] = table;

    usb_cstd_chg_curpipe(USB_NULL, (uint16_t) USB_PIPE0, (uint16_t) USB_CUSE, (uint16_t) USB_ISEL);

    /* Buffer clear */
    HW_USB_SetBclr(USB_NULL, USB_CUSE);

    HW_USB_ClearStatusBemp(USB_NULL, USB_PIPE0);

    /* Peripheral Control sequence */
    end_flag = usb_pstd_write_data(USB_PIPE0, USB_CUSE);

    /* Peripheral control sequence */
    switch (end_flag)
    {
        /* End of data write */
        case USB_WRITESHRT :
            /* Enable not ready interrupt */
            usb_cstd_nrdy_enable(USB_NULL, (uint16_t) USB_PIPE0);
            /* Set PID=BUF */
            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
        break;
            /* End of data write (not null) */
        case USB_WRITEEND :
            /* Continue */
            /* Continue of data write */
        case USB_WRITING :
            /* Enable empty interrupt */
            HW_USB_SetBempenb(USB_NULL, (uint16_t) USB_PIPE0);
            /* Enable not ready interrupt */
            usb_cstd_nrdy_enable(USB_NULL, (uint16_t) USB_PIPE0);
            /* Set PID=BUF */
            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
        break;
            /* FIFO access error */
        case USB_FIFOERROR :
        break;
        default :
        break;
    }
    /* End or error or continue */
    return (end_flag);
}
/******************************************************************************
 End of function usb_pstd_ctrl_read
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_ctrl_write
 Description     : Called by R_USB_PstdCtrlWrite, see it for description.
 Arguments       : uint32_t bsize    : Write size in bytes.
 : uint8_t *table    : Start address of write data buffer.
 Return value    : none
 ******************************************************************************/
void usb_pstd_ctrl_write (uint32_t bsize, uint8_t *table)
{
    g_usb_pstd_data_cnt[USB_PIPE0] = bsize;
    g_usb_pstd_data_ptr[USB_PIPE0] = table;

    usb_cstd_chg_curpipe(USB_NULL, (uint16_t) USB_PIPE0, (uint16_t) USB_CUSE, USB_FALSE);
    /* Buffer clear */
    HW_USB_SetBclr(USB_NULL, USB_CUSE);

    /* Interrupt enable */
    /* Enable ready interrupt */
    HW_USB_SetBrdyenb(USB_NULL, (uint16_t) USB_PIPE0);
    /* Enable not ready interrupt */
    usb_cstd_nrdy_enable(USB_NULL, (uint16_t) USB_PIPE0);

    /* Set PID=BUF */
    usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
}
/******************************************************************************
 End of function usb_pstd_ctrl_write
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_ctrl_end
 Description     : End control transfer
 Arguments       : USB_UTR_t *ptr    : USB system internal structure.
 : uint16_t status   : Transfer end status
 Return value    : none
 ******************************************************************************/
void usb_pstd_ctrl_end (uint16_t status)
{
    /* Interrupt disable */
    /* BEMP0 disable */
    HW_USB_ClearBempenb(USB_NULL, (uint16_t) USB_PIPE0);
    /* BRDY0 disable */
    HW_USB_ClearBrdyenb(USB_NULL, (uint16_t) USB_PIPE0);
    /* NRDY0 disable */
    HW_USB_ClearNrdyenb(USB_NULL, (uint16_t) USB_PIPE0);

    #if USB_CFG_USE_USBIP == USB_CFG_IP1
    HW_USB_SetMbw(USB_NULL, USB_CUSE, USB1_CFIFO_MBW);
    #else  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    HW_USB_SetMbw(USB_NULL, USB_CUSE, USB0_CFIFO_MBW);
    #endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */

    if ((status == USB_DATA_ERR) || (status == USB_DATA_OVR))
    {
        /* Request error */
        usb_pstd_set_stall_pipe0();
    }
    else if (status == USB_DATA_STOP)
    {
        /* Pipe stop */
        usb_cstd_set_nak(USB_NULL, (uint16_t) USB_PIPE0);
    }
    else
    {
        /* Set CCPL bit */
        HW_USB_PSetCcpl();
    }
}
/******************************************************************************
 End of function usb_pstd_ctrl_end
 ******************************************************************************/
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 End  Of File
 ******************************************************************************/
