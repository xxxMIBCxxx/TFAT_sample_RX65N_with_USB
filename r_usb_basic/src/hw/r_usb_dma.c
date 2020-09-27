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
* File Name    : r_usb_dma.c
* Description  : Setting code of DMA
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_bitdefine.h"
#include "r_usb_dmac.h"

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
/******************************************************************************
Constant macro definitions
******************************************************************************/

/******************************************************************************
External variables and functions
******************************************************************************/
extern uint32_t HW_USB_GetDxfifo_adr(usb_utr_t *ptr, uint16_t fifo_mode, uint16_t bit_width);
extern uint16_t         g_usb_usbmode;      /* USB mode HOST/PERI */

/******************************************************************************
Private global variables and functions
******************************************************************************/
usb_dma_int_t   g_usb_cstd_dma_int;         /* DMA Interrupt Info */

uint16_t    g_usb_cstd_dma_dir[USB_NUM_USBIP][USB_DMA_USE_CH_MAX];  /* DMA0 and DMA1 direction */
uint32_t    g_usb_cstd_dma_size[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 buffer size */
uint16_t    g_usb_cstd_dma_fifo[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 FIFO buffer size */
uint16_t    g_usb_cstd_dma_pipe[USB_NUM_USBIP][USB_DMA_USE_CH_MAX]; /* DMA0 and DMA1 pipe number */

uint16_t    usb_cpu_get_dma_crtb(uint16_t use_port);

uint16_t    usb_cpu_get_dxfifo_ir_vect(usb_utr_t *ptr, uint16_t useport);
void        usb_cpu_set_dxfifo_ir(usb_utr_t *ptr, uint16_t useport, uint16_t value);
void        usb_cpu_set_dxfifo_dreqe(usb_utr_t *ptr, uint16_t useport);
void        usb_cpu_clear_dxfifo_dreqe(usb_utr_t *ptr, uint16_t useport);
void        usb_cpu_dxfifo2buf_start_dma(usb_utr_t *ptr, uint32_t SourceAddr, uint16_t useport, uint32_t transfer_size);
void        usb_cpu_buf2dxfifo_start_dma(usb_utr_t *ptr, uint32_t src_adr, uint16_t useport, uint32_t transfer_size);
void        usb_cpu_dxfifo_stop_dma(uint8_t ip_type, uint16_t fifo_mode);

uint32_t  fifo_address[USB_NUM_USBIP][USB_DMA_FIFO_TYPE_NUM][USB_FIFO_ACCSESS_TYPE_NUM] = 
{
  {
    /* IP0 */
    /* 32bit            16bit               8bit */
    {(uint32_t)0,                 (uint32_t)&USB_M0.D0FIFO.WORD,  (uint32_t)&USB_M0.D0FIFO.BYTE.L},   /* USB0 D0FIFO address */
    {(uint32_t)0,                 (uint32_t)&USB_M0.D1FIFO.WORD,  (uint32_t)&USB_M0.D1FIFO.BYTE.L},
  },
#if USB_NUM_USBIP == 2
  {
    /* IP1 */
  #if defined(BSP_MCU_RX63N)
    {(uint32_t)0,                 (uint32_t)&USB1.D0FIFO.WORD,  (uint32_t)&USB1.D0FIFO.BYTE.L},   /* USB0 D0FIFO address */
    {(uint32_t)0,                 (uint32_t)&USB1.D1FIFO.WORD,  (uint32_t)&USB1.D1FIFO.BYTE.L},
  #endif  /* defined(BSP_MCU_RX63N) */

  #if defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX64M)

    /* Little */
    #if USB_CFG_ENDIAN == USB_CFG_LITTLE
    {(uint32_t)&USB_M1.D0FIFO.LONG, (uint32_t)&USB_M1.D0FIFO.WORD.H,(uint32_t)&USB_M1.D0FIFO.BYTE.HH}, /* USBA D0FIFO adr Little */
    {(uint32_t)&USB_M1.D1FIFO.LONG, (uint32_t)&USB_M1.D1FIFO.WORD.H,(uint32_t)&USB_M1.D1FIFO.BYTE.HH}  /* USBA D1FIFO adr Little */
    #endif  /* USB_CFG_ENDIAN == USB_CFG_LITTLE */

    /* Big */
    #if USB_CFG_ENDIAN == USB_CFG_BIG
    {(uint32_t)&USB_M1.D0FIFO.LONG, (uint32_t)&USB_M1.D0FIFO.WORD.L,(uint32_t)&USB_M1.D0FIFO.BYTE.LL}, /* USBA D0FIFO adr Big */
    {(uint32_t)&USB_M1.D1FIFO.LONG, (uint32_t)&USB_M1.D1FIFO.WORD.L,(uint32_t)&USB_M1.D1FIFO.BYTE.LL}  /* USBA D1FIFO adr Big */
    #endif  /* USB_CFG_ENDIAN == USB_CFG_BIG */
  #endif  /* defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX64M) */
  }
#endif  /* USB_NUM_USBIP == 2 */
};

/******************************************************************************
Function Name   : usb_cstd_dxfifo_stop_usb
Description     : Setup external variables used for USB data transfer; to reg-
                : ister if you want to stop the transfer of DMA.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_dxfifo_stop_usb(usb_utr_t *ptr, uint8_t channel)
{
    uint16_t    pipe;
    uint32_t    transfer_size;
    uint32_t    *request_size;
    uint16_t    pipemode;
    uint16_t    ip;
    uint32_t    *p_data_cnt;
    uint16_t    mbw_setting;

    if (USB_NULL == ptr)
    {
#if USB_CFG_USE_USBIP == USB_CFG_IP0
        ip = USB_IP0;
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        ip = USB_IP1;
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        pipe = g_usb_cstd_dma_pipe[ip][channel];
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        p_data_cnt = &g_usb_pstd_data_cnt[pipe];
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
        ip = ptr->ip;
        pipe = g_usb_cstd_dma_pipe[ip][channel];
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        p_data_cnt = &g_usb_hstd_data_cnt[ptr->ip][pipe];
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    transfer_size = g_usb_cstd_dma_size[ip][channel];
    request_size = p_data_cnt;
    if (USB_DMA_CH == channel)
    {
        pipemode = USB_D0DMA;
        if (USB_IP0 == ip)
        {
            mbw_setting = USB0_D0FIFO_MBW;
        }
        else
        {
            mbw_setting = USB1_D0FIFO_MBW;
        }
    }
    else
    {
        pipemode = USB_D1DMA;
        if (USB_IP0 == ip)
        {
            mbw_setting = USB0_D1FIFO_MBW;
        }
        else
        {
            mbw_setting = USB1_D1FIFO_MBW;
        }
    }

    HW_USB_ClearDreqe(ptr, pipemode);
    HW_USB_SetMbw( ptr, pipemode, mbw_setting );

    /* received data size */
    *request_size = *request_size - transfer_size;
}
/******************************************************************************
End of function usb_cstd_dxfifo_stop_usb
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_buf2fifo_dma
Description     : Start transfer using DMA0. accsess size 32bytes.
Arguments       : uint32_t src       : transfer data pointer
                : uint32_t data_size : transfer data size
                : uint16_t pipe      : Pipe nr.
Return value    : none
******************************************************************************/
void usb_cstd_buf2fifo_dma(usb_utr_t *ptr, uint32_t src, uint32_t data_size, uint8_t pipe)
{
    uint16_t pipemode;

    if (USB_PIPE1_DMA_CH == pipe)
    {
        pipemode = USB_D0DMA;
    }
    else
    {
        pipemode = USB_D1DMA;
    }

    if (2 == (data_size & 0x03))
    {
        /* Change MBW setting */
        HW_USB_SetMbw(ptr, pipemode, USB_MBW_16);
    }
    else if (1 == (data_size & 0x01))
    {
        /* Change MBW setting */
        HW_USB_SetMbw(ptr, pipemode, USB_MBW_8);
    }

    /* Changes the FIFO port by the pipe. */
    usb_cstd_chg_curpipe(ptr, pipe, pipemode, USB_FALSE);

    /* dma trans setting Divisible by FIFO buffer size  */
    usb_cpu_buf2dxfifo_start_dma(ptr, src, pipemode, data_size);

    /* Enable Not Ready Interrupt */
//    usb_cstd_nrdy_enable(ptr, pipe);
    /* CPU access Buffer to FIFO start */

    /* Set DREQ enable */
//    HW_USB_SetDreqe(ptr, pipemode);

    /* Set BUF */
    usb_cstd_set_buf(ptr, pipe);
}
/******************************************************************************
End of function usb_cstd_buf2fifo_dma
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_dxfifo_int
Description     : Set end of DMA transfer. Set to restart DMA trans-
                : fer according to data size of remaining functions to be pro-
                : cessed.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_dxfifo_int(usb_utr_t *ptr, uint8_t channel)
{
    uint8_t  *src_adr;
    uint16_t pipe;
    uint32_t    *p_data_cnt;
    uint8_t     *p_data_ptr;
    uint16_t    ip;
    uint16_t    fifo_mode;

    if (USB_NULL != ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ip = ptr->ip;
        pipe = g_usb_cstd_dma_pipe[ip][channel];
        p_data_cnt = &g_usb_hstd_data_cnt[ptr->ip][pipe];
        p_data_ptr = g_usb_hstd_data_ptr[ptr->ip][pipe];
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
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
        pipe = g_usb_cstd_dma_pipe[ip][channel];
        p_data_cnt = &g_usb_pstd_data_cnt[pipe];
        p_data_ptr = g_usb_pstd_data_ptr[pipe];
#else   /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
        ip = USB_NULL;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }

    /* trans data smaller than Buffer size */
    /*  equal all data transfer end  */
    if(*p_data_cnt < g_usb_cstd_dma_fifo[ip][channel])
    {
        if (USB_DMA_CH == channel)
        {
            fifo_mode = USB_D0DMA;
        }
        else
        {
            fifo_mode = USB_D1DMA;
        }

        /* FIFO buffer empty flag clear */
        HW_USB_ClearStatusBemp(ptr, pipe);
        /* bval control for transfer enable fifo 2 usb control */
        HW_USB_SetBval(ptr, fifo_mode);
        /* FIFO bufer empty interrupt enable */
        HW_USB_SetBempenb(ptr, pipe);
    }
    else
    {
        /* update remaining transfer data size */
        *p_data_cnt -= g_usb_cstd_dma_size[ip][channel];
        /* check transfer remaining data */
        if (*p_data_cnt == 0u)
        {
            /* FIFO buffer empty flag clear */
            HW_USB_ClearStatusBemp(ptr, pipe);
            /* check FIFO_EMPTY / INBUF bit */
            if((HW_USB_ReadPipectr(ptr,  pipe ) & USB_INBUFM) != USB_INBUFM)
            {
                /* DMA transfer function end. call callback function */

                if (USB_NULL != ptr)
                {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
                    usb_hstd_data_end(ptr, pipe, (uint16_t)USB_DATA_NONE);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
                }
                else
                {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
                    usb_pstd_data_end(pipe, (uint16_t)USB_DATA_NONE);
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
                }
            }
            else
            {
                /* FIFO bufer empty interrupt enable */
                HW_USB_SetBempenb(ptr, pipe);
            }
        }
        else
        {
            /* dma trans remaining data */
            /* DMA source address update */
            src_adr = (uint8_t *)((uint32_t)p_data_ptr + g_usb_cstd_dma_size[ip][channel]);
            /* DMA Transfer size update */
            g_usb_cstd_dma_size[ip][channel] = *p_data_cnt;
            /* DMA0 1byte trans */
            usb_cstd_buf2fifo_dma(ptr, (uint32_t)src_adr, g_usb_cstd_dma_size[ip][channel], pipe);
        }
    }

}
/******************************************************************************
End of function usb_cstd_dxfifo_int
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_buf2dxfifo_start_dma
Description     : Start transfer using DMA. If transfer size is 0, write
                : more data to buffer.
Arguments       : uint16_t pipe   : Pipe nr.
                : uint16_t useport: FIFO select
Return value    : none
******************************************************************************/
void usb_cstd_buf2dxfifo_start_dma(usb_utr_t *ptr, uint16_t pipe, uint16_t useport)
{
    uint32_t DmaSize;
    uint32_t FifoSize;
    uint8_t     *p_data_ptr;
    uint16_t    ip;
    uint16_t    ch;

    if (USB_NULL != ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        p_data_ptr = g_usb_hstd_data_ptr[ptr->ip][pipe];
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
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
        p_data_ptr = g_usb_pstd_data_ptr[pipe];
#else   /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
        ip = USB_NULL;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }

    if (USB_D0DMA == useport)
    {
        ch = USB_DMA_CH;
    }
    else
    {
        ch = USB_DMA_CH2;
    }

    DmaSize = g_usb_cstd_dma_size[ip][ch];
    FifoSize = g_usb_cstd_dma_fifo[ip][ch];

    if(DmaSize != 0u)
    {
        if(DmaSize >= FifoSize)
        {
            /* Fifosize block transfer */
            DmaSize = (DmaSize - DmaSize % FifoSize);
        }
        else
        {
            if (2 == (DmaSize & 0x03))
            {
                /* Change MBW setting */
                HW_USB_SetMbw(ptr, useport, USB_MBW_16);
            }
            else if (1 == (DmaSize & 0x01))
            {
                /* Change MBW setting */
                HW_USB_SetMbw(ptr, useport, USB_MBW_8);
            }
        }
        g_usb_cstd_dma_size[ip][ch] = DmaSize;

        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);

        usb_cpu_buf2dxfifo_start_dma(ptr, (uint32_t)p_data_ptr, useport, DmaSize);
    }
    else
    {
        if (USB_HOST == g_usb_usbmode)
        {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        	usb_hstd_buf2fifo(ptr, pipe, useport);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
        }
        else
        {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        	usb_pstd_buf2fifo(pipe, useport);
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
        }
    }
}
/******************************************************************************
End of function usb_cstd_buf2dxfifo_start_dma
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_dxfifo2buf_start_dma
Description     : Start transfer using DMA. If transfer size is 0, clear DMA.
Arguments       : uint16_t pipe   : Pipe nr.
                : uint16_t useport: FIFO select
                : uint32_t length
Return value    : none
******************************************************************************/
void usb_cstd_dxfifo2buf_start_dma(usb_utr_t *ptr, uint16_t pipe, uint16_t useport, uint32_t length)
{
    uint16_t mxps;

    uint32_t dma_size;
    uint32_t transfer_size;
    uint8_t     *p_data_ptr;
    uint16_t    ip;
    uint16_t    ch;

    if (USB_D0DMA == useport)
    {
        ch = USB_DMA_CH;
    }
    else
    {
        ch = USB_DMA_CH2;
    }

    if (USB_NULL != ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        p_data_ptr = g_usb_hstd_data_ptr[ptr->ip][g_usb_cstd_dma_pipe[ptr->ip][ch]];
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
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
        p_data_ptr = g_usb_pstd_data_ptr[g_usb_cstd_dma_pipe[ip][ch]];
#else   /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
        ip = USB_NULL;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }

    dma_size = g_usb_cstd_dma_size[ip][ch];
    transfer_size = g_usb_cstd_dma_size[ip][ch];


    /* Data size check */
    if(dma_size != 0u)
    {
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
        usb_cpu_dxfifo2buf_start_dma(ptr, (uint32_t)p_data_ptr, useport, transfer_size);
        /* Max Packet Size */
        mxps = usb_cstd_get_maxpacket_size(ptr, pipe);
        /* Set Transaction counter */
        usb_cstd_set_transaction_counter(ptr, pipe, (uint16_t)(((length - (uint32_t)1u)/ mxps) + (uint32_t)1u));
        /* Set BUF */
        usb_cstd_set_buf(ptr, pipe);
        /* Enable Ready Interrupt */
        HW_USB_SetBrdyenb(ptr, pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_nrdy_enable(ptr, pipe);

        /* Read (FIFO -> MEMORY) : USB register set */
//#if 0
#if defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX64M)
        /* DMA buffer clear mode & MBW set */
        if (USB_IP1 == ip)
        {
            HW_USB_SetMbw(ptr, useport, USB_MBW_32);
        }
        else
        {
#endif  /* defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX64M) */
            HW_USB_SetMbw(ptr, useport, USB_MBW_16);
#if defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX64M)
        }
#endif  /* defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX64M) */
//#endif // 0
        /* usb fifo set automatic clear mode  */
        HW_USB_ClearDclrm(ptr, useport);
        /* Set DREQ enable */
        HW_USB_SetDreqe(ptr, useport);
    }
    else
    {
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
        /* DMA buffer clear mode set */
        HW_USB_SetDclrm(ptr, useport);
        /* Set BUF */
        usb_cstd_set_buf(ptr, pipe);
        /* Enable Ready Interrupt */
        HW_USB_SetBrdyenb(ptr, pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_nrdy_enable(ptr, pipe);
    }
}
/******************************************************************************
End of function usb_cstd_dxfifo2buf_start_dma
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_dxfifo_handler
Description     : DMA interrupt routine. Send message to PCD task.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_dxfifo_handler(usb_utr_t *ptr, uint16_t useport)
{
    uint8_t         ip;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        ip = USB_CFG_USE_USBIP;
#else   /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
        ip = USB_IP0;
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
    }
    else
    {
        ip = ptr->ip;
    }

    HW_USB_ClearDreqe(ptr, useport);       /* DMA Transfer request disable */
    usb_cpu_dxfifo_stop_dma(ip, useport);  /* Stop DMA,FIFO access */
}
/******************************************************************************
End of function usb_cstd_dxfifo_handler
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_chk_dmarecv_data_count
Description     : this functionfor brdy function
                  Return DMA Transfer Status and value
Arguments       : uint32_t crtb         ; crtb register setting
                : uint32_t data_cnt     : transfer data count
                : uint16_t fifobuf_size : fifobuffer size
                : uint32_t reg_finalbuf  : stansfer data size mod fifobuffer size
                : uint32_t *ret_data_cnt : (return) Data count value
                : uint16_t *ret_status  : (return) Status
Return value    : none
******************************************************************************/
void usb_cstd_chk_dmarecv_data_count(uint32_t crtb, uint32_t data_cnt, uint16_t fifobuf_size, uint32_t reg_finalbuf,
    uint32_t *ret_data_cnt, uint16_t *ret_status)
{
    uint32_t final_data_logic;       /* theoretical value  */
    uint32_t final_data_register;    /* register value */

    /* Check data count */
    if(crtb == 0)
    {
        /* final transfer data size */
        final_data_logic = data_cnt % fifobuf_size;
        if(final_data_logic == 0)
        {
            final_data_logic = fifobuf_size;
        }
        final_data_register = reg_finalbuf;

        if(final_data_logic == final_data_register)
        {
            /* Request Size = Transfer Size */
            *ret_data_cnt = 0;
            /* End of data transfer */
            *ret_status = USB_DATA_OK;
        }
        else if(final_data_logic < final_data_register)
        {
            /* Request Size < Transfer Size (data size over) */
            /* caution : g_usb_cstd_data_cnt[i] is minus data */
            *ret_data_cnt = final_data_logic - final_data_register;
            /* End of data transfer */
            *ret_status = USB_DATA_OVR;
        }
        else
        {
            /* Request Size > Transfer Size (data size short) */
            *ret_data_cnt = final_data_logic - final_data_register;
            /* End of data transfer */
            *ret_status = USB_DATA_SHT;
        }
    }
    else
    {
        /* Request Size > Transfer Size (data size short) */
        *ret_data_cnt = data_cnt - (((data_cnt - crtb) / fifobuf_size) * fifobuf_size + reg_finalbuf);
        /* End of data transfer */
        *ret_status = USB_DATA_SHT;
    }
}
/******************************************************************************
End of function usb_cstd_chk_dmarecv_data_count
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_get_dma_crtb
Description     : Get DMA Current Transaction Byte reg B(CRTB).
Arguments       : uint16_t use_port : FIFO Access mode
Return value    : DMA Current Transaction Byte reg B(CRTB)
******************************************************************************/
uint16_t usb_cpu_get_dma_crtb(uint16_t use_port)
{
    if( use_port == USB_D0DMA )
    {
        return DMAC0.DMCRB;
    }
    else if( use_port == USB_D1DMA )
    {
        return DMAC1.DMCRB;
    }
    else
    {
    }
    return 0;
}
/******************************************************************************
End of function usb_cpu_get_dma_crtb
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_dxfifo2buf_start_dma
Description     : FIFO to Buffer data read DMA start
Arguments       : uint32_t des_addr      : Source address
                : uint16_t useport       : FIFO Access mode
                : uint32_t transfer_size : Transfer size
Return value    : void
******************************************************************************/
void usb_cpu_dxfifo2buf_start_dma(usb_utr_t *ptr, uint32_t des_addr, uint16_t useport, uint32_t transfer_size)
{
    volatile dmaca_return_t     ret;
    dmaca_transfer_data_cfg_t   td_cfg;
    dmaca_stat_t                dmac_status;
    uint32_t                    fifo_address;
    uint8_t                     dma_ch;
    uint16_t                    vect;
    uint16_t                    fifo_mode;

    uint16_t                    ip;
    uint16_t                    transfer_count;
    uint16_t                    block_size;

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

    if (USB_D0DMA == useport)
    {
        fifo_mode = USB_FIFO_TYPE_D0DMA;
        dma_ch = USB_DMA_CH;
    }
    else
    {
        fifo_mode = USB_FIFO_TYPE_D1DMA;
        dma_ch = USB_DMA_CH2;
    }

    transfer_count  = ((transfer_size -1) / g_usb_cstd_dma_fifo[ip][dma_ch]) +1;
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    if (USB_IP1 == ip)
    {
        td_cfg.data_size    = DMACA_DATA_SIZE_LWORD;
        block_size      = ((g_usb_cstd_dma_fifo[ip][dma_ch] -1) /4) +1;
        fifo_address    = HW_USB_GetDxfifo_adr(ptr, fifo_mode, USB_FIFO_ACCESS_TYPE_32BIT);
    }
    else
    {
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        td_cfg.data_size    = DMACA_DATA_SIZE_WORD;
        block_size      = ((g_usb_cstd_dma_fifo[ip][dma_ch] -1) /2) +1;
        fifo_address    = HW_USB_GetDxfifo_adr(ptr, fifo_mode, USB_FIFO_ACCESS_TYPE_16BIT);
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    }
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

    vect = usb_cpu_get_dxfifo_ir_vect(ptr, useport);
/* DMA test code start */
    /* Operation - No Extended Repeat Area Function and No Offset Subtraction */
    /* Source address is fixed
    * Transfer data size is 32-bit (long word).
    * DMAC transfer mode is Repeat mode & Source side is repeat area
    * At the beginning of transfer, clear the interrupt flag of the activation source
    to 0.
    * Transfer Request source is software. */
    /* Set Transfer data configuration. */
    td_cfg.transfer_mode  = DMACA_TRANSFER_MODE_BLOCK;
    td_cfg.repeat_block_side = DMACA_REPEAT_BLOCK_DISABLE;
    td_cfg.act_source     = (dmaca_activation_source_t)vect;
    td_cfg.request_source = DMACA_TRANSFER_REQUEST_PERIPHERAL;
    td_cfg.dtie_request   = DMACA_TRANSFER_END_INTERRUPT_DISABLE;
    td_cfg.esie_request   = DMACA_TRANSFER_ESCAPE_END_INTERRUPT_DISABLE;
    td_cfg.rptie_request  = DMACA_REPEAT_SIZE_END_INTERRUPT_DISABLE;
    td_cfg.sarie_request  = DMACA_SRC_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
    td_cfg.darie_request  = DMACA_DES_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
    td_cfg.src_addr_mode  = DMACA_SRC_ADDR_FIXED;
    td_cfg.src_addr_repeat_area = DMACA_SRC_ADDR_EXT_REP_AREA_NONE;
    td_cfg.des_addr_mode  = DMACA_DES_ADDR_INCR;
    td_cfg.des_addr_repeat_area = DMACA_DES_ADDR_EXT_REP_AREA_NONE;
    td_cfg.offset_value   = 0x00000000;
    td_cfg.interrupt_sel  = DMACA_CLEAR_INTERRUPT_FLAG_BEGINNING_TRANSFER;
    td_cfg.p_src_addr     = (void *)fifo_address;
    td_cfg.p_des_addr     = (void *)des_addr;
    td_cfg.transfer_count = transfer_count;
    td_cfg.block_size     = block_size;
    /* Call R_DMACA_Create(). */
    ret = R_DMACA_Create(dma_ch, &td_cfg);

    /* Enable USB0 D0FIFO0 interrupt request before Calling R_DTC_Create(). */
    usb_cpu_set_dxfifo_ir(ptr, useport, 0);

    /* Call R_DMACA_Control().
    Enable DMAC transfer. */
    ret = R_DMACA_Control(dma_ch, DMACA_CMD_ENABLE, &dmac_status);

    usb_cpu_set_dxfifo_dreqe(ptr, useport);
}
/******************************************************************************
End of function usb_cpu_dxfifo2buf_start_dma
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_buf2dxfifo_start_dma
Description     : Buffer to FIFO data write DMA start
Arguments       : uint32_t DistAdr      : Destination address
Return value    : void
******************************************************************************/
void usb_cpu_buf2dxfifo_start_dma(usb_utr_t *ptr, uint32_t src_adr, uint16_t useport, uint32_t transfer_size)
{
    volatile dmaca_return_t     ret;
    dmaca_transfer_data_cfg_t   td_cfg;
    dmaca_stat_t                dmac_status;
    uint32_t                    fifo_address;
    uint16_t                    vect;
    uint16_t                    fifo_mode;
    uint16_t                    bit_width;
    uint8_t                     dma_ch;
    uint16_t                    ip;
    uint16_t                    transfer_count;
    uint16_t                    block_size;

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

    if (USB_D0DMA == useport)
    {
        fifo_mode = USB_FIFO_TYPE_D0DMA;
        dma_ch = USB_DMA_CH;
    }
    else
    {
        fifo_mode = USB_FIFO_TYPE_D1DMA;
        dma_ch = USB_DMA_CH2;
    }

    if (g_usb_cstd_dma_fifo[ip][dma_ch] > transfer_size)
    {
        block_size       = transfer_size;
    }
    else
    {
        block_size       = g_usb_cstd_dma_fifo[ip][dma_ch];
    }

    transfer_count  = ((transfer_size -1) / g_usb_cstd_dma_fifo[ip][dma_ch]) +1;
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    if ((0 == (transfer_size & 0x03)) && (USB_IP1 == ip))
    {
        block_size       = ((block_size -1) /4) +1;
        bit_width        = USB_FIFO_ACCESS_TYPE_32BIT;
        td_cfg.data_size = DMACA_DATA_SIZE_LWORD;
        fifo_address     = HW_USB_GetDxfifo_adr(ptr, fifo_mode, USB_FIFO_ACCESS_TYPE_32BIT);
    }
    else
    {
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        if(0 == (transfer_size & 0x01))
        {
            block_size       = ((block_size -1) /2) +1;
            bit_width        = USB_FIFO_ACCESS_TYPE_16BIT;
            td_cfg.data_size = DMACA_DATA_SIZE_WORD;
            fifo_address     = HW_USB_GetDxfifo_adr(ptr, fifo_mode, USB_FIFO_ACCESS_TYPE_16BIT);
        }
        else
        {
            bit_width        = USB_FIFO_ACCESS_TYPE_8BIT;
            td_cfg.data_size = DMACA_DATA_SIZE_BYTE;
            fifo_address     = HW_USB_GetDxfifo_adr(ptr, fifo_mode, USB_FIFO_ACCESS_TYPE_8BIT);
        }
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    }
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

    vect = usb_cpu_get_dxfifo_ir_vect(ptr, useport);
    fifo_address = HW_USB_GetDxfifo_adr(ptr, fifo_mode, bit_width);
/* DMA test code start */
    /* Operation - No Extended Repeat Area Function and No Offset Subtraction */
    /* Source address is fixed
    * Transfer data size is 32-bit (long word).
    * DMAC transfer mode is Repeat mode & Source side is repeat area
    * At the beginning of transfer, clear the interrupt flag of the activation source
    to 0.
    * Transfer Request source is software. */
    /* Set Transfer data configuration. */
    td_cfg.transfer_mode        = DMACA_TRANSFER_MODE_BLOCK;
    td_cfg.repeat_block_side    = DMACA_REPEAT_BLOCK_DISABLE;
    td_cfg.act_source           = (dmaca_activation_source_t)vect;
    td_cfg.request_source       = DMACA_TRANSFER_REQUEST_PERIPHERAL;
    td_cfg.dtie_request         = DMACA_TRANSFER_END_INTERRUPT_ENABLE;
    td_cfg.esie_request         = DMACA_TRANSFER_ESCAPE_END_INTERRUPT_DISABLE;
    td_cfg.rptie_request        = DMACA_REPEAT_SIZE_END_INTERRUPT_DISABLE;
    td_cfg.sarie_request        = DMACA_SRC_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
    td_cfg.darie_request        = DMACA_DES_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
    td_cfg.src_addr_mode        = DMACA_SRC_ADDR_INCR;
    td_cfg.src_addr_repeat_area = DMACA_SRC_ADDR_EXT_REP_AREA_NONE;
    td_cfg.des_addr_mode        = DMACA_DES_ADDR_FIXED;
    td_cfg.des_addr_repeat_area = DMACA_DES_ADDR_EXT_REP_AREA_NONE;
    td_cfg.offset_value         = 0x00000000;
    td_cfg.interrupt_sel        = DMACA_CLEAR_INTERRUPT_FLAG_BEGINNING_TRANSFER;
    td_cfg.p_src_addr           = (void *)src_adr;
    td_cfg.p_des_addr           = (void *)fifo_address;
    td_cfg.transfer_count       = transfer_count;
    td_cfg.block_size           = block_size;
    /* Call R_DMACA_Create(). */
    ret = R_DMACA_Create(dma_ch, &td_cfg);

    /* Enable USB0 D0FIFO0 interrupt request before Calling R_DTC_Create(). */
    usb_cpu_set_dxfifo_ir(ptr, useport, 0);

    /* Call R_DMACA_Control().
    Enable DMAC transfer. */
    ret = R_DMACA_Control(dma_ch, DMACA_CMD_ENABLE, &dmac_status);

    usb_cpu_set_dxfifo_dreqe(ptr, useport);
}
/******************************************************************************
End of function usb_cpu_buf2dxfifo_start_dma
******************************************************************************/



/******************************************************************************
Function Name   : usb_cpu_get_dxfifo_ir_vect
Description     : 
Arguments       : 
Return value    : 
******************************************************************************/
uint16_t    usb_cpu_get_dxfifo_ir_vect(usb_utr_t *ptr, uint16_t use_port)
{
    uint16_t    ip;
    uint16_t    vect;

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

    if (ip == USB_USBIP_0)
    {
        if( use_port == USB_D0DMA )
        {
            vect = IR_USB0_D0FIFO0;
        }
        else if( use_port == USB_D1DMA )
        {
            vect = IR_USB0_D1FIFO0;
        }
    }
#if USB_NUM_USBIP == 2
    else
    {
        if( use_port == USB_D0DMA )
        {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            vect = IR_USBA_D0FIFO2;
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
#if defined(BSP_MCU_RX63N)
            vect = IR_USB1_D0FIFO1;
#endif  /* defined(BSP_MCU_RX63N) */
        }
        else if( use_port == USB_D1DMA )
        {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            vect = IR_USBA_D1FIFO2;
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
#if defined(BSP_MCU_RX63N)
            vect = IR_USB1_D1FIFO1;
#endif  /* defined(BSP_MCU_RX63N) */
        }
    }
#endif /* USB_NUM_USBIP == 2 */

    return vect;
}   /* eof usb_cpu_get_dxfifo_ir_vect() */

/******************************************************************************
Function Name   : usb_cpu_set_dxfifo_ir
Description     : 
Arguments       : 
Return value    : 
******************************************************************************/
void    usb_cpu_set_dxfifo_ir(usb_utr_t *ptr, uint16_t use_port, uint16_t value)
{
    uint16_t    ip;

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

    if (ip == USB_USBIP_0)
    {
        if( use_port == USB_D0DMA )
        {
            IR(USB0,D0FIFO0) = value;
        }
        else if( use_port == USB_D1DMA )
        {
            IR(USB0,D1FIFO0) = value;
        }
    }
#if USB_NUM_USBIP == 2
    else
    {
        if( use_port == USB_D0DMA )
        {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            IR(USBA,D0FIFO2) = value;
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
#if defined(BSP_MCU_RX63N)
            IR(USB_M1,D0FIFO1) = value;
#endif  /* defined(BSP_MCU_RX63N) */
        }
        else if( use_port == USB_D1DMA )
        {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            IR(USBA,D1FIFO2) = value;
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
#if defined(BSP_MCU_RX63N)
            IR(USB_M1,D1FIFO1) = value;
#endif  /* defined(BSP_MCU_RX63N) */
        }
    }
#endif /* USB_NUM_USBIP == 2 */
}   /* eof usb_cpu_get_dxfifo_ir_vect() */


/******************************************************************************
Function Name   : usb_cpu_set_dxfifo_dreqe
Description     : 
Arguments       : 
Return value    : 
******************************************************************************/
void    usb_cpu_set_dxfifo_dreqe(usb_utr_t *ptr, uint16_t use_port)
{
    uint16_t    ip;

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

    if (ip == USB_USBIP_0)
    {
        if( use_port == USB_D0DMA )
        {
            USB_M0.D0FIFOSEL.WORD &= ~USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
            USB_M0.D0FIFOSEL.WORD |= USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
        }
        else if( use_port == USB_D1DMA )
        {
            USB_M0.D1FIFOSEL.WORD &= ~USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
            USB_M0.D1FIFOSEL.WORD |= USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
        }
    }
#if USB_NUM_USBIP == 2
    else
    {
        if( use_port == USB_D0DMA )
        {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) || defined(BSP_MCU_RX63N)
            USB_M1.D0FIFOSEL.WORD &= ~USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
            USB_M1.D0FIFOSEL.WORD |= USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) || defined(BSP_MCU_RX63N) */
        }
        else if( use_port == USB_D1DMA )
        {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) || defined(BSP_MCU_RX63N)
            USB_M1.D1FIFOSEL.WORD &= ~USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
            USB_M1.D1FIFOSEL.WORD |= USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) || defined(BSP_MCU_RX63N) */
        }
    }
#endif /* USB_NUM_USBIP == 2 */
}   /* eof usb_cpu_get_dxfifo_ir_ien() */

/******************************************************************************
Function Name   : usb_cpu_clear_dxfifo_dreqe
Description     : 
Arguments       : usb_utr_t *ptr, 
                : uint16_t use_port
Return value    : none
******************************************************************************/
void    usb_cpu_clear_dxfifo_dreqe(usb_utr_t *ptr, uint16_t use_port)
{
    uint16_t    ip;

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

    if (ip == USB_USBIP_0)
    {
        if( use_port == USB_D0DMA )
        {
            USB_M0.D0FIFOSEL.WORD &= ~USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
        }
        else if( use_port == USB_D1DMA )
        {
            USB_M0.D1FIFOSEL.WORD &= ~USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
        }
    }
#if USB_NUM_USBIP == 2
    else
    {
        if( use_port == USB_D0DMA )
        {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) || defined(BSP_MCU_RX63N)
            USB_M1.D0FIFOSEL.WORD &= ~USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) || defined(BSP_MCU_RX63N) */
        }
        else if( use_port == USB_D1DMA )
        {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) || defined(BSP_MCU_RX63N)
            USB_M1.D1FIFOSEL.WORD &= ~USB_DREQE;   /* USB_DREQE b12: DREQ output enable */
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) || defined(BSP_MCU_RX63N) */
        }
    }
#endif /* USB_NUM_USBIP == 2 */
}   /* eof usb_cpu_clear_dxfifo_dreqe() */

/******************************************************************************
Function Name   : HW_USB_GetD0fifo8adr
Description     : Get 8 bits of used channel's D0FIFO register content.
Arguments       :
Return          : Address of D0FIFO
******************************************************************************/
uint32_t HW_USB_GetDxfifo_adr(usb_utr_t *ptr, uint16_t fifo_mode, uint16_t bit_width)
{
    uint32_t    address;
    uint8_t     ip;
    


    if (USB_NULL != ptr)
    {
        /* Host mode */
        if (USB_IP0 == ptr->ip)
        {
            ip = USB_IP0;
        }
        else
        {
            ip = USB_IP1;
        }
    }
    else
    {
        /* Peri mode */
#if USB_CFG_USE_USBIP == USB_CFG_IP0
        /* USB0 */
        ip = USB_IP0;
#else
        /* USB1/USBA */
        ip = USB_IP1;
#endif
    }

    address = fifo_address[ip][fifo_mode][bit_width];
    return address;
}

/******************************************************************************
End of function HW_USB_GetD0fifo8adr
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_dxfifo_stop_dma
Description     : DMA stop
Arguments       : uint8_t ip_type        : USB_IP0/USB_IP1
Return value    : void
******************************************************************************/
void usb_cpu_dxfifo_stop_dma(uint8_t ip_type, uint16_t fifo_mode)
{
    dmaca_stat_t                dmac_status;
    uint8_t                     dma_ch;
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t                   utr;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    if (USB_D0DMA == fifo_mode)
    {
        dma_ch = USB_DMA_CH;
    }
    else
    {
        dma_ch = USB_DMA_CH2;
    }
    R_DMACA_Control(dma_ch, DMACA_CMD_DTIF_STATUS_CLR, &dmac_status);

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip = ip_type;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
        usb_cpu_set_dxfifo_ir(&utr, fifo_mode, 0);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
        usb_cpu_set_dxfifo_ir(USB_NULL, fifo_mode, 0);
    }
}   /* eof usb_cpu_dxfifo_stop_dma() */



/******************************************************************************
Function Name   : usb_dma_driver
Description     : USB DMA transfer complete process.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_dma_driver(void)
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */
    uint8_t     channel;

    if (g_usb_cstd_dma_int.wp != g_usb_cstd_dma_int.rp)
    {
        if (USB_D0DMA == g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.rp].fifo_type)
        {
            channel = USB_DMA_CH;
        }
        else
        {
            channel = USB_DMA_CH2;
        }

        if (USB_HOST == g_usb_usbmode)
        {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
            utr.ip = g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.rp].ip;
            utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
            usb_cstd_dxfifo_int(&utr, channel);
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */
        }
        else
        {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
            usb_cstd_dxfifo_int(USB_NULL, channel);
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
        }

        /* Read countup */
        g_usb_cstd_dma_int.rp = ((g_usb_cstd_dma_int.rp+1) % USB_INT_BUFSIZE);
    }
}   /* eof usb_dma_driver() */

#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

/******************************************************************************
End of file
******************************************************************************/
