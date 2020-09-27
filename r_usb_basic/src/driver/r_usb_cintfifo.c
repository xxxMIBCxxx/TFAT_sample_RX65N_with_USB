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
 * File Name    : r_usb_cintfifo.c
 * Description  : USB Host and Peripheral interrupt code
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
#include "r_usb_reg_access.h"
#include "r_usb_bitdefine.h"

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
    #include "r_usb_dmac.h"
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

/******************************************************************************
 External variables and functions
 ******************************************************************************/
#if USB_CFG_DTCDMA == USB_CFG_ENABLE
extern uint16_t g_usb_usbmode; /* USB mode HOST/PERI */

extern uint16_t g_usb_cstd_dma_dir[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 direction */
extern uint32_t g_usb_cstd_dma_size[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 buffer size */
extern uint16_t g_usb_cstd_dma_fifo[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 FIFO buffer size */
extern uint16_t g_usb_cstd_dma_pipe[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 pipe number */

extern void usb_cpu_clear_dxfifo_dreqe(usb_utr_t *ptr, uint16_t useport);
extern uint16_t usb_cpu_get_dma_crtb(uint16_t use_port);
extern void usb_cstd_dxfifo_stop_usb(usb_utr_t *ptr, uint8_t channel);
extern void usb_cpu_dxfifo_stop_dma(uint8_t ip_type, uint16_t fifo_mode);

#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : usb_pstd_brdy_pipe_process
 Description     : Search for the PIPE No. that BRDY interrupt occurred, and
 request data transmission/reception from the PIPE
 Arguments       : uint16_t bitsts       ; BRDYSTS Register & BRDYENB Register
 Return value    : none
 ******************************************************************************/
void usb_pstd_brdy_pipe_process(uint16_t bitsts)
{
    uint16_t useport;
    uint16_t i;
    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
    uint16_t buffer;
    uint16_t maxps;
    uint16_t set_dtc_brock_cnt;
    uint16_t trans_dtc_block_cnt;
    uint16_t dma_ch;
    uint16_t status;
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    for (i = USB_PIPE1; i <= USB_MAX_PIPE_NO; i++)
    {
        if ((bitsts & USB_BITSET(i)) != 0u)
        {
            /* Interrupt check */
            HW_USB_ClearStatusBemp(USB_NULL, i);

            if (USB_NULL != g_usb_pstd_pipe[i])
            {
                /* Pipe number to FIFO port select */
                useport = usb_pstd_pipe2fport(i);

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
                if ((USB_D0DMA == useport) || (USB_D1DMA == useport))
                {
                    if (USB_D0DMA == useport)
                    {
                        dma_ch = USB_DMA_CH;
                    }
                    else
                    {
                        dma_ch = USB_DMA_CH2;
                    }

                    maxps = g_usb_cstd_dma_fifo[USB_CFG_USE_USBIP][dma_ch];

                    /* DMA Transfer request disable */
                    usb_cpu_clear_dxfifo_dreqe( USB_NULL, useport );

                    /* DMA stop */
                    usb_cpu_dxfifo_stop_dma(USB_CFG_USE_USBIP, useport);

                    /* Changes FIFO port by the pipe. */
                    buffer = usb_cstd_is_set_frdy(USB_NULL, i, useport, USB_FALSE);

                    set_dtc_brock_cnt = (uint16_t)((g_usb_pstd_data_cnt[g_usb_cstd_dma_pipe[USB_CFG_USE_USBIP][dma_ch]] -1)
                            / g_usb_cstd_dma_fifo[USB_CFG_USE_USBIP][dma_ch]) +1;

                    trans_dtc_block_cnt = usb_cpu_get_dma_crtb(useport);

                    /* Get D0fifo Receive Data Length */
                    g_usb_cstd_dma_size[USB_CFG_USE_USBIP][dma_ch]
                    = (uint32_t)(buffer & USB_DTLN) + (set_dtc_brock_cnt - (trans_dtc_block_cnt + 1)) * maxps;

                    /* Check data count */
                    if( g_usb_cstd_dma_size[USB_CFG_USE_USBIP][dma_ch] == g_usb_pstd_data_cnt[i] )
                    {
                        status = USB_DATA_OK;
                    }
                    else if( g_usb_cstd_dma_size[USB_CFG_USE_USBIP][dma_ch] > g_usb_pstd_data_cnt[i] )
                    {
                        status = USB_DATA_OVR;
                    }
                    else
                    {
                        status = USB_DATA_SHT;
                    }
                    /* D0FIFO access DMA stop */
                    usb_cstd_dxfifo_stop_usb(USB_NULL, dma_ch);
                    /* End of data transfer */
                    usb_pstd_data_end(i, status);
                    /* Set BCLR */
                    HW_USB_SetBclr(USB_NULL, useport );
                }

    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

                if (USB_CUSE == useport)
                {
                    if(usb_cstd_get_pipe_dir(USB_NULL, i) == USB_BUF2FIFO)
                    {
                        /* Buffer to FIFO data write */
                        usb_pstd_buf2fifo(i, useport);
                    }
                    else
                    {
                        /* FIFO to Buffer data read */
                        usb_pstd_fifo_to_buf(i, useport);
                    }
                }
            }
        }
    }
}/* End of function usb_pstd_brdy_pipe_process() */

/******************************************************************************
 Function Name   : usb_pstd_nrdy_pipe_process
 Description     : Search for PIPE No. that occurred NRDY interrupt, and execute
 the process for PIPE when NRDY interrupt occurred
 Arguments       :
 : uint16_t bitsts       ; NRDYSTS Register & NRDYENB Register
 Return value    : none
 ******************************************************************************/
void usb_pstd_nrdy_pipe_process(uint16_t bitsts)
{
    uint16_t buffer;
    uint16_t i;

    for (i = USB_MIN_PIPE_NO; i <= USB_MAX_PIPE_NO; i++)
    {
        if ((bitsts & USB_BITSET(i)) != 0)
        {
            /* Interrupt check */
            if (USB_NULL != g_usb_pstd_pipe[i])
            {
                if (usb_cstd_get_pipe_type(USB_NULL, i) == USB_ISO)
                {
                    /* Wait for About 60ns */
                    buffer = HW_USB_ReadFrmnum(USB_NULL);
                    if ((buffer & USB_OVRN) == USB_OVRN)
                    {
                        /* @1 */
                        /* End of data transfer */
                        usb_pstd_forced_termination(i, (uint16_t)USB_DATA_OVR);
                        USB_PRINTF1("###ISO OVRN %d\n", g_usb_pstd_data_cnt[i]);
                    }
                    else
                    {
                        /* @2 */
                        /* End of data transfer */
                        usb_pstd_forced_termination(i, (uint16_t)USB_DATA_ERR);
                    }
                }
                else
                {
                    /* Non processing. */
                }
            }
        }
    }
}
/******************************************************************************
 End of function usb_pstd_nrdy_pipe_process
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_bemp_pipe_process
 Description     : Search for PIPE No. that BEMP interrupt occurred, and complete data transmission for the PIPE
 Arguments       : uint16_t bitsts       ; BEMPSTS Register & BEMPENB Register
 Return value    : none
 ******************************************************************************/
void usb_pstd_bemp_pipe_process(uint16_t bitsts)
{
    uint16_t buffer;
    uint16_t i;

    for (i = USB_MIN_PIPE_NO; i <= USB_PIPE5; i++)
    {
        if ((bitsts & USB_BITSET(i)) != 0)
        {
            /* Interrupt check */
            if (USB_NULL != g_usb_pstd_pipe[i])
            {
                buffer = usb_cstd_get_pid(USB_NULL, i);

                /* MAX packet size error ? */
                if ((buffer & USB_PID_STALL) == USB_PID_STALL)
                {
                    USB_PRINTF1("### STALL Pipe %d\n", i);
                    usb_pstd_forced_termination(i, (uint16_t)USB_DATA_STALL);
                }
                else
                {
                    if ((HW_USB_ReadPipectr(USB_NULL, i) & USB_INBUFM) != USB_INBUFM)
                    {
                        usb_pstd_data_end(i, (uint16_t)USB_DATA_NONE);
                    }
                    else
                    {
                        /* set BEMP enable */
                        HW_USB_SetBempenb(USB_NULL, i);
                    }
                }
            }
        }
    }
    for (i = USB_PIPE6; i <= USB_MAX_PIPE_NO; i++)
    {
        /* Interrupt check */
        if ((bitsts & USB_BITSET(i)) != 0)
        {
            if (USB_NULL != g_usb_pstd_pipe[i])
            {
                buffer = usb_cstd_get_pid(USB_NULL, i);

                /* MAX packet size error ? */
                if ((buffer & USB_PID_STALL) == USB_PID_STALL)
                {
                    USB_PRINTF1("### STALL Pipe %d\n", i);
                    usb_pstd_forced_termination(i, (uint16_t)USB_DATA_STALL);
                }
                else
                {
                    /* End of data transfer */
                    usb_pstd_data_end(i, (uint16_t)USB_DATA_NONE);
                }
            }
        }
    }
}
/******************************************************************************
 End of function usb_pstd_bemp_pipe_process
 ******************************************************************************/
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : usb_hstd_brdy_pipe_process
 Description     : Search for the PIPE No. that BRDY interrupt occurred, and 
 request data transmission/reception from the PIPE
 Arguments       : usb_utr_t *ptr
 : uint16_t bitsts       ; BRDYSTS Register & BRDYENB Register
 Return value    : none
 ******************************************************************************/
void usb_hstd_brdy_pipe_process (usb_utr_t *ptr, uint16_t bitsts)
{
    uint16_t useport;
    uint16_t i;
    uint16_t ip;

    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
    uint16_t buffer;
    uint16_t maxps;
    uint16_t set_dtc_brock_cnt;
    uint16_t trans_dtc_block_cnt;
    uint16_t dma_ch;
    uint16_t status;

    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

    ip = ptr->ip;
    for (i = USB_PIPE1; i <= USB_MAX_PIPE_NO; i++)
    {
        if ((bitsts & USB_BITSET(i)) != 0)
        {
            /* Interrupt check */
            HW_USB_ClearStatusBemp(ptr, i);

            if (USB_NULL != g_usb_hstd_pipe[ip][i])
            {
                /* Pipe number to FIFO port select */
                useport = usb_hstd_pipe2fport(ptr, i);
                if ((USB_D0DMA == useport) || (USB_D1DMA == useport))
                {
    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
                    if (USB_D0DMA == useport)
                    {
                        dma_ch = USB_DMA_CH;
                    }
                    else
                    {
                        dma_ch = USB_DMA_CH2;
                    }

                    maxps = g_usb_cstd_dma_fifo[ip][dma_ch];

                    /* DMA Transfer request disable */
                    usb_cpu_clear_dxfifo_dreqe( ptr, useport );

                    /* DMA stop */
                    usb_cpu_dxfifo_stop_dma(ptr->ip, useport);

                    /* Changes FIFO port by the pipe. */
                    buffer = usb_cstd_is_set_frdy(ptr, i, useport, USB_FALSE);

                    set_dtc_brock_cnt = (uint16_t)((g_usb_hstd_data_cnt[ip][g_usb_cstd_dma_pipe[ip][dma_ch]] -1)
                            / g_usb_cstd_dma_fifo[ip][dma_ch]) +1;

                    trans_dtc_block_cnt = usb_cpu_get_dma_crtb(useport);

                    /* Get D0fifo Receive Data Length */
                    g_usb_cstd_dma_size[ip][dma_ch]
                    = (uint32_t)(buffer & USB_DTLN) + (set_dtc_brock_cnt - (trans_dtc_block_cnt + 1)) * maxps;

                    /* Check data count */
                    if( g_usb_cstd_dma_size[ip][dma_ch] == g_usb_hstd_data_cnt[ptr->ip][i] )
                    {
                        status = USB_DATA_OK;
                    }
                    else if( g_usb_cstd_dma_size[ip][dma_ch] > g_usb_hstd_data_cnt[ip][i] )
                    {
                        status = USB_DATA_OVR;
                    }
                    else
                    {
                        status = USB_DATA_SHT;
                    }
                    /* D0FIFO access DMA stop */
                    usb_cstd_dxfifo_stop_usb(ptr, dma_ch);
                    /* End of data transfer */
                    usb_hstd_data_end(ptr, i, status);
                    /* Set BCLR */
                    HW_USB_SetBclr( ptr, useport );

    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */
                }
                else
                {
                    if (usb_cstd_get_pipe_dir(ptr, i) == USB_BUF2FIFO)
                    {
                        /* Buffer to FIFO data write */
                        usb_hstd_buf2fifo(ptr, i, useport);
                    }
                    else
                    {
                        /* FIFO to Buffer data read */
                        usb_hstd_fifo_to_buf(ptr, i, useport);
                    }
                }
            }
        }
    }
}/* End of function usb_hstd_brdy_pipe_process() */

/******************************************************************************
 Function Name   : usb_hstd_nrdy_pipe_process
 Description     : Search for PIPE No. that occurred NRDY interrupt, and execute 
 the process for PIPE when NRDY interrupt occurred
 Arguments       : usb_utr_t *ptr
 : uint16_t bitsts       ; NRDYSTS Register & NRDYENB Register
 Return value    : none
 ******************************************************************************/
void usb_hstd_nrdy_pipe_process (usb_utr_t *ptr, uint16_t bitsts)
{
    uint16_t buffer;
    uint16_t i;

    for (i = USB_MIN_PIPE_NO; i <= USB_MAX_PIPE_NO; i++)
    {
        if ((bitsts & USB_BITSET(i)) != 0)
        {
            /* Interrupt check */
            if (USB_NULL != g_usb_hstd_pipe[ptr->ip][i])
            {
                if (usb_cstd_get_pipe_type(ptr, i) == USB_TYPFIELD_ISO)
                {
                    /* Wait for About 60ns */
                    buffer = HW_USB_ReadFrmnum(ptr);
                    if ((buffer & USB_OVRN) == USB_OVRN)
                    {
                        /* @1 */
                        /* End of data transfer */
                        usb_hstd_forced_termination(ptr, i, (uint16_t) USB_DATA_OVR);
                        USB_PRINTF1("###ISO OVRN %d\n", g_usb_hstd_data_cnt[ptr->ip][i]);
                    }
                    else
                    {
                        /* @2 */
                        /* End of data transfer */
                        usb_hstd_forced_termination(ptr, i, (uint16_t) USB_DATA_ERR);
                    }
                }
                else
                {
                    usb_hstd_nrdy_endprocess(ptr, i);
                }
            }
        }
    }
}
/******************************************************************************
 End of function usb_hstd_nrdy_pipe_process
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_bemp_pipe_process
 Description     : Search for PIPE No. that BEMP interrupt occurred, and complete data transmission for the PIPE
 Arguments       : usb_utr_t *ptr
 : uint16_t bitsts       ; BEMPSTS Register & BEMPENB Register
 Return value    : none
 ******************************************************************************/
void usb_hstd_bemp_pipe_process (usb_utr_t *ptr, uint16_t bitsts)
{
    uint16_t buffer;
    uint16_t i;
    uint16_t useport;

    for (i = USB_MIN_PIPE_NO; i <= USB_PIPE5; i++)
    {
        if ((bitsts & USB_BITSET(i)) != 0)
        {
            /* Interrupt check */
            if (USB_NULL != g_usb_hstd_pipe[ptr->ip][i])
            {
                buffer = usb_cstd_get_pid(ptr, i);

                /* MAX packet size error ? */
                if ((buffer & USB_PID_STALL) == USB_PID_STALL)
                {
                    USB_PRINTF1("### STALL Pipe %d\n", i);
                    usb_hstd_forced_termination(ptr, i, (uint16_t) USB_DATA_STALL);
                }
                else
                {
                    if ((HW_USB_ReadPipectr(ptr, i) & USB_INBUFM) != USB_INBUFM)
                    {
                        /* Pipe number to FIFO port select */
                        useport = usb_hstd_pipe2fport(ptr, i);

                        if (USB_D0DMA == useport)
                        {
    #if USB_CFG_DTCDMA == USB_CFG_ENABLE
                            HW_USB_ClearStatusBemp( ptr, i );
    #endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */
                        }

                        /* End of data transfer */
                        usb_hstd_data_end(ptr, i, (uint16_t) USB_DATA_NONE);
                    }
                }
            }
        }
    }
    for (i = USB_PIPE6; i <= USB_MAX_PIPE_NO; i++)
    {
        /* Interrupt check */
        if ((bitsts & USB_BITSET(i)) != 0)
        {
            if (USB_NULL != g_usb_hstd_pipe[ptr->ip][i])
            {
                buffer = usb_cstd_get_pid(ptr, i);

                /* MAX packet size error ? */
                if ((buffer & USB_PID_STALL) == USB_PID_STALL)
                {
                    /*USB_PRINTF1("### STALL Pipe %d\n", i);*/
                    usb_hstd_forced_termination(ptr, i, (uint16_t) USB_DATA_STALL);
                }
                else
                {
                    /* End of data transfer */
                    usb_hstd_data_end(ptr, i, (uint16_t) USB_DATA_NONE);
                }
            }
        }
    }
}
/******************************************************************************
 End of function usb_hstd_bemp_pipe_process
 ******************************************************************************/
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */
/******************************************************************************
 End  Of File
 ******************************************************************************/
