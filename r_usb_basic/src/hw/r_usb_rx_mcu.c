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
 * File Name    : r_usb_rx_mcu.c
 * Description  : RX MCU processing
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

/******************************************************************************
 External variables and functions
 ******************************************************************************/
extern uint16_t g_usb_usbmode;
#if USB_CFG_DTCDMA == USB_CFG_ENABLE
extern usb_dma_int_t    g_usb_cstd_dma_int;             /* DMA Interrupt Info */
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

extern void             usb_cstd_dxfifo_handler(usb_utr_t *ptr, uint16_t useport);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N)
        #pragma interrupt usbfs_usbr_isr(vect = VECT(USB0, USBR0))
    #endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N) */

    #if defined (BSP_MCU_RX63N)
        #pragma interrupt usbfs_usbr_isr(vect = VECT(USB, USBR0))
    #endif  /* defined (BSP_MCU_RX63N) */

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

#pragma interrupt usbfs_usbi_isr(vect = VECT(USB0, USBI0))
#if USB_NUM_USBIP == 2
    #if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        #pragma interrupt usbhs_usbir_isr(vect = VECT(USBA, USBAR))
    #endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    #if defined (BSP_MCU_RX63N)
        #pragma interrupt usbhs_usbir_isr(vect = VECT(USB1, USBI1))
    #endif  /* defined (BSP_MCU_RX63N) */
    #if defined (BSP_MCU_RX63N)
        #pragma interrupt usb1_usbr_isr(vect = VECT(USB, USBR1))
    #endif  /* defined (BSP_MCU_RX63N) */

#endif /* USB_NUM_USBIP == 2 */

static bool g_usb_is_opened[2] =
{ false, false };

/*=== Interrupt =============================================================*/
void usb_cpu_usbint_init (uint8_t ip_type);
void usbfs_usbi_isr (void);
void usbhs_usbir_isr (void);
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
void usb_cpu_int_enable (usb_utr_t *ptr);
void usb_cpu_int_disable (usb_utr_t *ptr);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
/*=== TIMER =================================================================*/
void usb_cpu_delay_1us (uint16_t time);
void usb_cpu_delay_xms (uint16_t time);

/******************************************************************************
 Renesas Abstracted RSK functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_module_start
 Description     : USB module start
 Arguments       : uint8_t ip_type  : USB_IP0/USB_IP1
 Return value    : none
 ******************************************************************************/
usb_err_t usb_module_start (uint8_t ip_type)
{

    if (USB_IP0 == ip_type)
    {
        if ( false == R_BSP_HardwareLock(BSP_LOCK_USB0))
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USB0);
            return USB_ERR_BUSY;
        }
        if ( true == g_usb_is_opened[USB_IP0])
        {
            return USB_ERR_NG; //USB_ERR_OPENED;
        }
        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Enable power for USB0 */
        MSTP(USB0) = 0;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        g_usb_is_opened[USB_IP0] = true;

        R_BSP_HardwareUnlock(BSP_LOCK_USB0);
    }
#if USB_NUM_USBIP == 2
    else
    {
    #if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N)
        if( false == R_BSP_HardwareLock(BSP_LOCK_USBA) )
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USBA);
            return USB_ERR_BUSY;
        }
        if( true == g_usb_is_opened[USB_IP1] )
        {
            return USB_ERR_NG; //USB_ERR_OPENED;
        }

        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Enable power for USBA */
        MSTP(USBA) = 0;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        g_usb_is_opened[USB_IP1] = true;

        R_BSP_HardwareUnlock(BSP_LOCK_USBA);
    #endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N) */

    #if defined (BSP_MCU_RX63N)
        if( false == R_BSP_HardwareLock(BSP_LOCK_USB1) )
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USB1);
            return USB_ERR_BUSY;
        }
        if( true == g_usb_is_opened[USB_IP1] )
        {
            return USB_ERR_NG; //USB_ERR_OPENED;
        }

        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Enable power for USB1 */
        MSTP(USB1) = 0;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        g_usb_is_opened[USB_IP1] = true;

        R_BSP_HardwareUnlock(BSP_LOCK_USB1);
    #endif  /* defined (BSP_MCU_RX63N) */

    }
#endif /* USB_NUM_USBIP == 2 */
    return USB_SUCCESS;
} /* eof usb_module_start() */

/******************************************************************************
 Function Name   : usb_module_stop
 Description     : USB module stop
 Arguments       : uint8_t ip_type  : USB_IP0/USB_IP1
 Return value    : none
 ******************************************************************************/
usb_err_t usb_module_stop (uint8_t ip_type)
{
    if (USB_IP0 == ip_type)
    {
        if (R_BSP_HardwareLock(BSP_LOCK_USB0) == false)
        {
            /* Lock has already been acquired by another task. Need to try again later */
            return USB_ERR_BUSY;
        }
        if (false == g_usb_is_opened[ip_type])
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USB0);
            return USB_ERR_NOT_OPEN;
        }
        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Disable power for USB0 */
        MSTP(USB0) = 1;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
    }
#if USB_NUM_USBIP == 2
    else if (USB_IP1 == ip_type)
    {
        #if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N)
        if(R_BSP_HardwareLock(BSP_LOCK_USBA) == false)
        {
            /* Lock has already been acquired by another task. Need to try again later */
            return USB_ERR_BUSY;
        }
        if(g_usb_is_opened[ip_type] == false)
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USBA);
            return USB_ERR_NOT_OPEN;
        }
        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Disable power for USBA */
        MSTP(USBA) = 1;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        R_BSP_HardwareUnlock(BSP_LOCK_USBA);
        #endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N) */

        #if defined (BSP_MCU_RX63N)
        if(R_BSP_HardwareLock(BSP_LOCK_USB1) == false)
        {
            /* Lock has already been acquired by another task. Need to try again later */
            return USB_ERR_BUSY;
        }
        if(g_usb_is_opened[ip_type] == false)
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USB1);
            return USB_ERR_NOT_OPEN;
        }
        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Disable power for USB1 */
        MSTP(USB1) = 1;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        R_BSP_HardwareUnlock(BSP_LOCK_USB1);
        #endif  /* defined (BSP_MCU_RX63N) */
    }
#endif /* USB_NUM_USBIP == 2 */
    else
    {
        return USB_ERR_PARA;
    }
    g_usb_is_opened[ip_type] = false;
    return USB_SUCCESS;
} /* eof usb_module_stop() */

/******************************************************************************
 Interrupt function
 ******************************************************************************/
/******************************************************************************
 Function Name   : usb_cpu_usbint_init
 Description     : USB interrupt Initialize
 Arguments       : uint8_t ip_type  : USB_IP0/USB_IP1
 Return value    : void
 ******************************************************************************/
void usb_cpu_usbint_init (uint8_t ip_type)
{
    if ( USB_IP0 == ip_type)
    {
        /* Deep standby USB monitor register
         b0      SRPC0    USB0 single end control
         b3-b1   Reserved 0
         b4      FIXPHY0  USB0 transceiver output fix
         b7-b5   Reserved 0
         b8      SRPC1    USB1 single end control
         b11-b9  Reserved 0
         b12     FIXPHY1  USB1 transceiver output fix
         b15-b13 Reserved 0
         b16     DP0      USB0 DP input
         b17     DM0      USB0 DM input
         b19-b18 Reserved 0
         b20     DOVCA0   USB0 OVRCURA input
         b21     DOVCB0   USB0 OVRCURB input
         b22     Reserved 0
         b23     DVBSTS0  USB1 VBUS input
         b24     DP1      USB1 DP input
         b25     DM1      USB1 DM input
         b27-b26 Reserved 0
         b28     DOVCA1   USB1 OVRCURA input
         b29     DOVCB1   USB1 OVRCURB input
         b30     Reserved 0
         b31     DVBSTS1  USB1 VBUS input
         */
        USB.DPUSR0R.BIT.FIXPHY0 = 0u; /* USB0 Transceiver Output fixed */

        /* Interrupt enable register
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
#if USB_CFG_DTCDMA == USB_CFG_ENABLE
        IEN( USB0, D0FIFO0 ) = 1u;    /* D0FIFO0 Enable */
        IEN( USB0, D1FIFO0 ) = 1u;    /* D1FIFO0 Enable */
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

#if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N)
        IEN( USB0, USBR0 )= 1u; /* USBR0 enable */
#endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N) */

#if defined (BSP_MCU_RX63N)
        IEN( USB, USBR0 ) = 1u; /* USBR0 enable */
#endif  /* defined (BSP_MCU_RX63N) */

        /* Interrupt priority register
         b3-b0 IPR      Interrupt priority
         b7-b4 Reserved 0
         */
#if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N)
        IPR( USB0, USBR0 )= 0x00; /* USBR0 */
#endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N) */

#if defined (BSP_MCU_RX63N)
        IPR( USB, USBR0 ) = 0x00; /* USBR0 */
#endif  /* defined (BSP_MCU_RX63N) */

        IPR( USB0, USBI0 )= 0x03; /* USBI0 in vector 128 */
        IEN( USB0, USBI0 )= 1u; /* USBI0 enable in vector 128 */
    }

#if USB_NUM_USBIP == 2
    if( USB_IP1 == ip_type )
    {
    #if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N)
        /* Interrupt enable register
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
#if USB_CFG_DTCDMA == USB_CFG_ENABLE
        IEN( USBA, D0FIFO2 ) = 1u;   /* Enable D0FIF2 interrupt */
        IEN( USBA, D1FIFO2 ) = 1u;   /* Enable D1FIF2 interrupt */
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        IEN( USBA, USBAR ) = 1u; /* Enable  USBA  interrupt */

        /* Priority D0FIFO0=0(Disable)
         b3-b0 IPR      Interrupt priority
         b7-b4 Reserved 0
         */
        IPR( USBA, USBAR ) = 0x03; /* USBA */
    #endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N) */

    #if defined (BSP_MCU_RX63N)
        /* Interrupt enable register
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
#if USB_CFG_DTCDMA == USB_CFG_ENABLE
        IEN( USB1, D0FIFO1 ) = 1u;   /* Enable D0FIF1 interrupt */
        IEN( USB1, D1FIFO1 ) = 1u;   /* Enable D1FIF1 interrupt */
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

        IEN( USB1, USBI1 ) = 1u; /* Enable  USB1  interrupt */
        IEN( USB, USBR1 ) = 1u; /* Enable  USB1  interrupt */

        /* Priority D0FIFO0=0(Disable)
         b3-b0 IPR      Interrupt priority
         b7-b4 Reserved 0
         */
        IPR( USB1, USBI1 ) = 0x03; /* USB1 */
        IPR( USB, USBR1 ) = 0x03; /* USB1 */
    #endif  /* defined (BSP_MCU_RX63N) */

    }
#endif /* USB_NUM_USBIP == 2 */
}
/******************************************************************************
 End of function usb_cpu_usbint_init
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usb_cpu_usb_int_hand
 * Description  : Interrupt service routine of USBF
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usbfs_usbi_isr (void)
{
    /* Call USB interrupt routine */
    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        usb_hstd_usb_handler(); /* Call interrupt routine */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        usb_pstd_usb_handler(); /* Call interrupt routine */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    ICU.PIBR7.BYTE |= 0x40; /* Flag clear */
#endif /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
}
/******************************************************************************
 End of function usbfs_usbi_isr
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usbhs_usbir_isr
 * Description  : Interrupt service routine of USBF
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usbhs_usbir_isr (void)
{
    /* Condition compilation by the difference of USB function */
    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    #if USB_NUM_USBIP == 2
        usb2_hstd_usb_handler(); /* Call interrupt routine */
    #endif /* USB_NUM_USBIP == 2 */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        usb_pstd_usb_handler();
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
}
/******************************************************************************
 End of function usbhs_usbir_isr
 ******************************************************************************/

/******************************************************************************
 TIMER function
 ******************************************************************************/
/******************************************************************************
 Function Name   : usb_cpu_delay_1us
 Description     : 1us Delay timer
 Arguments       : uint16_t  time        ; Delay time(*1us)
 Return value    : none
 Note            : Please change for your MCU
 ******************************************************************************/
void usb_cpu_delay_1us (uint16_t time)
{
    volatile register uint16_t i;

    /* Wait 1us (Please change for your MCU) */
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65N)
    for (i = 0; i < (9 * time); ++i)
#endif /* defined(BSP_MCU_RX64M) */

#if defined(BSP_MCU_RX71M)
    for( i = 0; i < (20 * time); ++i )
#endif /* defined(BSP_MCU_RX71M) */

#if defined(BSP_MCU_RX63N)
    for( i = 0; i < (9 * time); ++i )
#endif /* defined(BSP_MCU_RX64M) */
    {
        /* wait */
    };
}
/******************************************************************************
 End of function usb_cpu_delay_1us
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cpu_delay_xms
 Description     : xms Delay timer
 Arguments       : uint16_t  time        ; Delay time(*1ms)
 Return value    : void
 Note            : Please change for your MCU
 ******************************************************************************/
void usb_cpu_delay_xms (uint16_t time)
{
    /* Wait xms (Please change for your MCU) */
    volatile register uint32_t i;

    /* Wait 1ms */
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65N)
    for (i = 0; i < (9500 * time); ++i)
#endif /* defined(BSP_MCU_RX64M) */

#if defined(BSP_MCU_RX71M)
    for( i = 0; i < (20000 * time); ++i )
#endif /* defined(BSP_MCU_RX71M) */

#if defined(BSP_MCU_RX63N)
    for( i = 0; i < (9500 * time); ++i )
#endif /* defined(BSP_MCU_RX64M) */
    {
        /* wait */
    };
    /* When "ICLK=120MHz" is set, this code is waiting for 1ms.
     Please change this code with CPU Clock mode. */
}
/******************************************************************************
 End of function usb_cpu_delay_xms
 ******************************************************************************/

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : usb_cpu_int_enable
 Description     : USB Interrupt Enable
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : void
 ******************************************************************************/
void usb_cpu_int_enable (usb_utr_t *ptr)
{
    if (ptr->ip == USB_USBIP_0)
    {
        /* Interrupt enable register (USB0 USBIO enable)
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
        IEN( USB0, USBI0 )= 1; /* Enable USB0 interrupt */
    }

    #if USB_NUM_USBIP == 2
    if (ptr->ip == USB_USBIP_1)
    {
        #if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N)
        /* Interrupt enable register (USB1 USBIO enable)
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
        IEN( USBA, USBAR ) = 1u; /* Enable USBA interrupt */
        #endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N) */

        #if defined (BSP_MCU_RX63N)
        /* Interrupt enable register (USB1 USBIO enable)
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
        IEN( USB, USBR1 ) = 1u; /* Enable USB1 interrupt */
        #endif  /* defined (BSP_MCU_RX63N) */

    }
    #endif /* USB_NUM_USBIP == 2 */
}
/******************************************************************************
 End of function usb_cpu_int_enable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cpu_int_disable
 Description     : USB Interrupt disable
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : void
 ******************************************************************************/
void usb_cpu_int_disable (usb_utr_t *ptr)
{
    if (ptr->ip == USB_USBIP_0)
    {
        /* Interrupt enable register (USB0 USBIO disable)
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
        IEN( USB0, USBI0 )= 0; /* Disnable USB0 interrupt */
    }

    #if USB_NUM_USBIP == 2
    if (ptr->ip == USB_USBIP_1)
    {
        #if defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N)
        /* Interrupt enable register (USB1 USBIO disable)
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
        IEN( USBA, USBAR ) = 0u; /* Disnable USBA interrupt */
        #endif  /* defined (BSP_MCU_RX64M) || (BSP_MCU_RX71M) || (BSP_MCU_RX65N) */

        #if defined (BSP_MCU_RX63N)
        /* Interrupt enable register (USB1 USBIO disable)
         b0 IEN0 Interrupt enable bit
         b1 IEN1 Interrupt enable bit
         b2 IEN2 Interrupt enable bit
         b3 IEN3 Interrupt enable bit
         b4 IEN4 Interrupt enable bit
         b5 IEN5 Interrupt enable bit
         b6 IEN6 Interrupt enable bit
         b7 IEN7 Interrupt enable bit
         */
        IEN( USB, USBR1 ) = 0u; /* Disnable USB1 interrupt */
        #endif  /* defined (BSP_MCU_RX63N) */

    }
    #endif /* USB_NUM_USBIP == 2 */
}
/******************************************************************************
 End of function usb_cpu_int_disable
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_chattaring
 Description     : 
 Arguments       : 
 Return value    : 
 ******************************************************************************/
uint16_t usb_chattaring (uint16_t *syssts)
{
    uint16_t lnst[4];

    while (1)
    {
        lnst[0] = *syssts & USB_LNST;
        usb_cpu_delay_xms((uint16_t) 1); /* 1ms wait */
        lnst[1] = *syssts & USB_LNST;
        usb_cpu_delay_xms((uint16_t) 1); /* 1ms wait */
        lnst[2] = *syssts & USB_LNST;
        if ((lnst[0] == lnst[1]) && (lnst[0] == lnst[2]))
        {
            break;
        }
    }
    return lnst[0];
} /* eof usb_chattaring() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/*******************************************************************************
 * Function Name: usb_cpu_usb_int_hand
 * Description  : Interrupt service routine of USBF
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usbfs_usbr_isr (void)
{
    /* Call USB interrupt routine */
    if (USB_HOST == g_usb_usbmode)
    {
    }
    else
    {
        usb_pstd_usb_handler();
    }
}
/*******************************************************************************
 End of function usbfs_usbr_isr
 *******************************************************************************/

/*******************************************************************************
 * Function Name: usb1_usbr_isr
 * Description  : Interrupt service routine of USBF
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usb1_usbr_isr (void)
{
}
/*******************************************************************************
 End of function usb1_usbr_isr
 *******************************************************************************/

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */



#if USB_CFG_DTCDMA == USB_CFG_ENABLE
/*******************************************************************************
* Function Name: usb_usb0_d0fifo_isr
* Description  : Interrupt service routine of D0FIFO
* Arguments    : none
* Return Value : none
*******************************************************************************/
void usb_usb0_d0fifo_isr (void)
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

    g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.wp].ip = USB_IP0;
    g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.wp].fifo_type = USB_D0DMA;
    g_usb_cstd_dma_int.wp = ((g_usb_cstd_dma_int.wp+1) % USB_INT_BUFSIZE);

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip = USB_IP0;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
        usb_cstd_dxfifo_handler(&utr, USB_D0DMA);
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        usb_cstd_dxfifo_handler(USB_NULL, USB_D0DMA);
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
    }
}   /* eof usb_usb0_d0fifo_isr() */

/*******************************************************************************
* Function Name: usb_usb0_d1fifo_isr
* Description  : Interrupt service routine of D1FIFO
* Arguments    : none
* Return Value : none
*******************************************************************************/
void usb_usb0_d1fifo_isr (void)
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

    g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.wp].ip = USB_IP0;
    g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.wp].fifo_type = USB_D1DMA;
    g_usb_cstd_dma_int.wp = ((g_usb_cstd_dma_int.wp+1) % USB_INT_BUFSIZE);

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip = USB_IP0;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
        usb_cstd_dxfifo_handler(&utr, USB_D1DMA);
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        usb_cstd_dxfifo_handler(USB_NULL, USB_D1DMA);
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
    }
}   /* eof usb_usb0_d1fifo_isr() */

/*******************************************************************************
* Function Name: usb_usb2_d0fifo_isr
* Description  : Interrupt service routine of D0FIFO
* Arguments    : none
* Return Value : none
*******************************************************************************/
void usb_usb2_d0fifo_isr (void)
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

    g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.wp].ip = USB_IP1;
    g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.wp].fifo_type = USB_D0DMA;
    g_usb_cstd_dma_int.wp = ((g_usb_cstd_dma_int.wp+1) % USB_INT_BUFSIZE);

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip = USB_IP1;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
        usb_cstd_dxfifo_handler(&utr, USB_D0DMA);
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        usb_cstd_dxfifo_handler(USB_NULL, USB_D0DMA);
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
    }
}   /* eof usb_usb2_d0fifo_isr() */

/*******************************************************************************
* Function Name: usb_usb2_d1fifo_isr
* Description  : Interrupt service routine of D1FIFO
* Arguments    : none
* Return Value : none
*******************************************************************************/
void usb_usb2_d1fifo_isr (void)
{
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    usb_utr_t   utr;
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

    g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.wp].ip = USB_IP1;
    g_usb_cstd_dma_int.buf[g_usb_cstd_dma_int.wp].fifo_type = USB_D1DMA;
    g_usb_cstd_dma_int.wp = ((g_usb_cstd_dma_int.wp+1) % USB_INT_BUFSIZE);

    if (USB_HOST == g_usb_usbmode)
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        utr.ip = USB_IP1;
        utr.ipp = usb_hstd_get_usb_ip_adr(utr.ip);
        usb_cstd_dxfifo_handler(&utr, USB_D1DMA);
#endif  /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        usb_cstd_dxfifo_handler(USB_NULL, USB_D1DMA);
#endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
    }
}   /* eof usb_usb2_d1fifo_isr() */
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

/******************************************************************************
 End  Of File
 ******************************************************************************/
