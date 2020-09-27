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
 * File Name    : r_usb_creg_abs.c
 * Description  : Call USB register access function
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

/******************************************************************************
 Constant macro definitions
 ******************************************************************************/
#define USB_BUFSIZE_BIT   (10u)
#if USB_CFG_LPW == USB_CFG_ENABLE
    #define USB_SUSPEND_MODE  (1u)
    #define USB_NORMAL_MODE   (0)
#endif /* USB_CFG_LPW == USB_CFG_ENABLE */

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
#if USB_CFG_LPW == USB_CFG_ENABLE
uint16_t g_usb_cstd_suspend_mode = USB_NORMAL_MODE;
#endif /* USB_CFG_LPW == USB_CFG_ENABLE */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
uint16_t g_usb_hstd_use_pipe[USB_NUM_USBIP];
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : usb_cstd_get_buf_size
 Description     : Return buffer size, or max packet size, of specified pipe.
 Arguments       : usb_utr_t *ptr    : USB system internal structure. Selects channel.
 : uint16_t pipe     : Pipe number.
 Return value    : uint16_t          : FIFO buffer size or max packet size.
 ******************************************************************************/
uint16_t usb_cstd_get_buf_size (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t size;
    uint16_t buffer;

    if (USB_PIPE0 == pipe)
    {
        buffer = HW_USB_ReadDcpcfg(ptr);
        if ((buffer & USB_CNTMDFIELD) == USB_CFG_CNTMDON)
        {
            /* Continuation transmit */
            /* Buffer Size */
            size = USB_PIPE0BUF;
        }
        else
        {
            /* Not continuation transmit */
            buffer = HW_USB_ReadDcpmaxp(ptr);

            /* Max Packet Size */
            size = (uint16_t) (buffer & USB_MAXP);
        }
    }
    else
    {
        /* Pipe select */
        HW_USB_WritePipesel(ptr, pipe);

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        /* Read CNTMD */
        buffer = HW_USB_ReadPipecfg(ptr);
        if ((buffer & USB_CNTMDFIELD) == USB_CFG_CNTMDON)
        {
            buffer = HW_USB_ReadPipebuf(ptr);
            /* Buffer Size */
            size = (uint16_t)((uint16_t)((buffer >> USB_BUFSIZE_BIT) + 1) * USB_PIPEXBUF);
        }
        else
        {
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        buffer = HW_USB_ReadPipemaxp(ptr);

        /* Max Packet Size */
        size = (uint16_t) (buffer & USB_MXPS);
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    }
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
    return size;
}/* End of function usb_cstd_get_buf_size() */

/******************************************************************************
 Function Name   : usb_cstd_pipe_init
 Description     : Initialization of registers associated with specified pipe.
 Arguments       : usb_utr_t *ptr    : USB system internal structure. Selects channel.
 : uint16_t pipe     : Pipe Number
 : uint16_t *tbl     : ep table
 : uint16_t ofs      : ep table offset
 Return value    : none
 ******************************************************************************/
void usb_cstd_pipe_init (usb_utr_t *ptr, uint16_t pipe, uint16_t *tbl, uint16_t ofs)
{
    uint16_t    useport = USB_CUSE;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        g_usb_pstd_pipe[pipe] = (usb_utr_t *)USB_NULL;
        useport = usb_pstd_pipe2fport(pipe);
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        g_usb_hstd_pipe[ptr->ip][pipe] = (usb_utr_t*) USB_NULL;
        useport = usb_hstd_pipe2fport(ptr, pipe);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    /* Interrupt Disable */
    /* Ready         Int Disable */
    HW_USB_ClearBrdyenb(ptr, pipe);

    /* NotReady      Int Disable */
    HW_USB_ClearNrdyenb(ptr, pipe);

    /* Empty/SizeErr Int Disable */
    HW_USB_ClearBempenb(ptr, pipe);

    /* PID=NAK & clear STALL */
    usb_cstd_clr_stall(ptr, pipe);

    /* PIPE Configuration */
    HW_USB_WritePipesel(ptr, pipe);

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    /* Update use pipe no info */
    if (USB_NULL != tbl[ofs + 1])
    {
        g_usb_hstd_use_pipe[ptr->ip] |= ((uint16_t) 1 << pipe);
    }
    else
    {
        g_usb_hstd_use_pipe[ptr->ip] &= ~((uint16_t) 1 << pipe);
    }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

    if ((USB_D0DMA == useport) || (USB_D1DMA == useport))
    {
        tbl[ofs + 1] |= USB_BFREON;
    }

    HW_USB_WritePipecfg(ptr, tbl[ofs + 1]);

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    HW_USB_WritePipebuf(ptr, tbl[ofs + 2]);
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    HW_USB_WritePipemaxp(ptr, tbl[ofs + 3]);
    HW_USB_WritePipeperi(ptr, tbl[ofs + 4]);

    /* FIFO buffer DATA-PID initialized */
    HW_USB_WritePipesel(ptr, USB_PIPE0);

    /* SQCLR */
    HW_USB_SetSqclr(ptr, pipe);

    /* ACLRM */
    usb_cstd_do_aclrm(ptr, pipe);

    /* CSSTS */
    HW_USB_SetCsclr(ptr, pipe);

    /* Interrupt status clear */
    /* Ready         Int Clear */
    HW_USB_ClearStsBrdy(ptr, pipe);

    /* NotReady      Int Clear */
    HW_USB_ClearStatusNrdy(ptr, pipe);

    /* Empty/SizeErr Int Clear */
    HW_USB_ClearStatusBemp(ptr, pipe);
}/* End of function usb_cstd_pipe_init() */

/******************************************************************************
 Function Name   : usb_cstd_clr_pipe_cnfg
 Description     : Clear specified pipe configuration register.
 Arguments       : usb_utr_t *ptr    : USB system internal structure. Selects channel.
 : uint16_t pipe_no  : pipe number
 Return value    : none
 ******************************************************************************/
void usb_cstd_clr_pipe_cnfg (usb_utr_t *ptr, uint16_t pipe_no)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        g_usb_pstd_pipe[pipe_no] = (usb_utr_t *)USB_NULL;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        g_usb_hstd_pipe[ptr->ip][pipe_no] = (usb_utr_t*) USB_NULL;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    /* PID=NAK & clear STALL */
    usb_cstd_clr_stall(ptr, pipe_no);

    /* Interrupt disable */
    /* Ready         Int Disable */
    HW_USB_ClearBrdyenb(ptr, pipe_no);

    /* NotReady      Int Disable */
    HW_USB_ClearNrdyenb(ptr, pipe_no);

    /* Empty/SizeErr Int Disable */
    HW_USB_ClearBempenb(ptr, pipe_no);

    /* PIPE Configuration */
    usb_cstd_chg_curpipe(ptr, (uint16_t) USB_PIPE0, (uint16_t) USB_CUSE, USB_FALSE);
    HW_USB_WritePipesel(ptr, pipe_no);
    HW_USB_WritePipecfg(ptr, 0);

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    HW_USB_WritePipebuf(ptr, 0);
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    HW_USB_WritePipemaxp(ptr, 0);
    HW_USB_WritePipeperi(ptr, 0);
    HW_USB_WritePipesel(ptr, 0);

    /* FIFO buffer DATA-PID initialized */
    /* SQCLR */
    HW_USB_SetSqclr(ptr, pipe_no);

    /* ACLRM */
    usb_cstd_do_aclrm(ptr, pipe_no);

    /* CSSTS */
    HW_USB_SetCsclr(ptr, pipe_no);
    usb_cstd_clr_transaction_counter(ptr, pipe_no);

    /* Interrupt status clear */
    /* Ready         Int Clear */
    HW_USB_ClearStsBrdy(ptr, pipe_no);

    /* NotReady      Int Clear */
    HW_USB_ClearStatusNrdy(ptr, pipe_no);

    /* Empty/SizeErr Int Clear */
    HW_USB_ClearStatusBemp(ptr, pipe_no);
}/* End of function usb_cstd_clr_pipe_cnfg() */

/******************************************************************************
 Function Name   : usb_cstd_set_nak
 Description     : Set up to NAK the specified pipe.
 Arguments       : usb_utr_t *ptr    : USB system internal structure. Selects channel.
 : uint16_t pipe     : Pipe Number
 Return value    : none
 ******************************************************************************/
void usb_cstd_set_nak (usb_utr_t *ptr, uint16_t pipe)
{
    uint16_t buf;
    uint16_t n;

    /* Set NAK */
    HW_USB_ClearPid(ptr, pipe, (uint16_t) USB_PID_BUF);

    /* The state of PBUSY continues while transmitting the packet when it is a detach. */
    /* 1ms comes off when leaving because the packet duration might not exceed 1ms.  */
    /* Whether it is PBUSY release or 1ms passage can be judged. */
    for (n = 0; n < 0xFFFFu; ++n)
    {
        /* PIPE control reg read */
        buf = HW_USB_ReadPipectr(ptr, pipe);
        if ((uint16_t) (buf & USB_PBUSY) == 0)
        {
            n = 0xFFFEu;
        }
    }
}/* End of function usb_cstd_set_nak() */

/******************************************************************************
 Function Name   : usb_cstd_is_set_frdy
 Description     : Changes the specified FIFO port by the specified pipe.
 : Please change the wait time for your MCU.
 Arguments       : usb_utr_t *ptr    : USB system internal structure. Selects channel.
 : uint16_t pipe     : Pipe Number
 : uint16_t fifosel  : FIFO select
 : uint16_t isel     : ISEL bit status
 Return value    : FRDY status
 ******************************************************************************/
uint16_t usb_cstd_is_set_frdy (usb_utr_t *ptr, uint16_t pipe, uint16_t fifosel, uint16_t isel)
{
    uint16_t buffer;
    uint16_t i;

    /* Changes the FIFO port by the pipe. */
    usb_cstd_chg_curpipe(ptr, pipe, fifosel, isel);

    for (i = 0; i < 4; i++)
    {
        buffer = HW_USB_ReadFifoctr(ptr, fifosel);

        if ((uint16_t) (buffer & USB_FRDY) == USB_FRDY)
        {
            return (buffer);
        } USB_PRINTF1("*** FRDY wait pipe = %d\n", pipe);

        /* Caution!!!
         * Depending on the external bus speed of CPU, you may need to wait
         * for 100ns here.
         * For details, please look at the data sheet.   */
        /***** The example of reference. *****/
        buffer = HW_USB_ReadSyscfg(ptr, USB_PORT0);
        buffer = HW_USB_ReadSyssts(ptr, USB_PORT0);
        /*************************************/
    }
    return (USB_FIFOERROR);
}/* End of function usb_cstd_is_set_frdy() */

/******************************************************************************
 Function Name   : usb_cstd_chg_curpipe
 Description     : Switch FIFO and pipe number.
 Arguments       : usb_utr_t *ptr    : USB system internal structure. Selects channel.
 : uint16_t pipe     : Pipe number.
 : uint16_t fifosel  : FIFO selected (CPU, D0, D1..)
 : uint16_t isel     : CFIFO Port Access Direction.(Pipe1 to 9:Set to 0)
 Return value    : none
 ******************************************************************************/
void usb_cstd_chg_curpipe (usb_utr_t *ptr, uint16_t pipe, uint16_t fifosel, uint16_t isel)
{
    uint16_t buffer;

    /* Select FIFO */
    switch (fifosel)
    {
        /* CFIFO use */
        case USB_CUSE :

            /* ISEL=1, CURPIPE=0 */
            HW_USB_RmwFifosel(ptr, USB_CUSE, (USB_RCNT | isel | pipe), (USB_RCNT | USB_ISEL | USB_CURPIPE));
            do
            {
                buffer = HW_USB_ReadFifosel(ptr, USB_CUSE);
            } while ((buffer & (uint16_t) (USB_ISEL | USB_CURPIPE)) != (uint16_t) (isel | pipe));
        break;

            /* D0FIFO use */
        case USB_D0USE :

            /* continue */
        break;

            /* D1FIFO use */
        case USB_D1USE :

            /* continue */
        break;

#if USB_CFG_DTCDMA == USB_CFG_ENABLE
    /* D0FIFO DMA */
    case USB_D0DMA:
        /* D0FIFO pipe select */
    	HW_USB_SetCurpipe( ptr, USB_D0DMA, pipe );
        do
        {
            buffer = HW_USB_ReadFifosel( ptr, USB_D0DMA );
        }
        while( (uint16_t)(buffer & USB_CURPIPE) != pipe );
        break;

    /* D1FIFO DMA */
    case USB_D1DMA:
        /* D1FIFO pipe select */
    	HW_USB_SetCurpipe( ptr, USB_D1DMA, pipe );

        do
        {
            buffer = HW_USB_ReadFifosel( ptr, USB_D1DMA );
        }
        while( (uint16_t)(buffer & USB_CURPIPE) != pipe );
        break;
#endif    /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        default :
        break;
    }
}/* End of function usb_cstd_chg_curpipe() */

/******************************************************************************
 Function Name   : usb_cstd_set_transaction_counter
 Description     : Set specified Pipe Transaction Counter Register.
 Arguments       : usb_utr_t *ptr    : USB system internal structure. Selects channel.
 : uint16_t trnreg   : Pipe number
 : uint16_t trncnt       : Transaction counter
 Return value    : none
 ******************************************************************************/
void usb_cstd_set_transaction_counter (usb_utr_t *ptr, uint16_t trnreg, uint16_t trncnt)
{
    HW_USB_SetTrclr(ptr, trnreg);
    HW_USB_WritePipetrn(ptr, trnreg, trncnt);
    HW_USB_SetTrenb(ptr, trnreg);
}/* End of function usb_cstd_set_transaction_counter() */

/******************************************************************************
 Function Name   : usb_cstd_clr_transaction_counter
 Description     : Clear specified Pipe Transaction Counter Register.
 Arguments       : usb_utr_t *ptr    : USB system internal structure. Selects channel.
 : uint16_t trnreg   : Pipe Number
 Return value    : none
 ******************************************************************************/
void usb_cstd_clr_transaction_counter (usb_utr_t *ptr, uint16_t trnreg)
{
    HW_USB_ClearTrenb(ptr, trnreg);
    HW_USB_SetTrclr(ptr, trnreg);
}/* End of function usb_cstd_clr_transaction_counter() */

/******************************************************************************
 End of file
 ******************************************************************************/

