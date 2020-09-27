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
 * File Name    : r_usb_preg_access.c
 * Description  : USB IP Peripheral control register access code
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
#include "r_usb_reg_access.h"
#include "r_usb_bitdefine.h"


/******************************************************************************
External variables and functions
******************************************************************************/
extern void usb_cpu_delay_xms (uint16_t time);
extern void usb_cpu_delay_1us (uint16_t time);


#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : HW_USB_PSetDprpu
 Description     : Set DPRPU-bit SYSCFG0 register.
 : (Enable D+Line pullup when PeripheralController function is selected)
 Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_PSetDprpu(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG.WORD |= USB_DPRPU;
    #else
    USB_M1.SYSCFG.WORD |= USB_DPRPU;
    #endif
} /* eof HW_USB_PSetDprpu() */

/******************************************************************************
 Function Name   : HW_USB_PClearDprpu
 Description     : Clear DPRPU-bit of the SYSCFG0 register.
 : (Disable D+Line pullup when PeripheralController function is
 : selected.)
 Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_PClearDprpu(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG.WORD &= (~USB_DPRPU);
    #else
    USB_M1.SYSCFG.WORD &= (~USB_DPRPU);
    #endif
} /* eof HW_USB_PClearDprpu() */

/******************************************************************************
 Function Name   : HW_USB_PSetWkup
 Description     : Set WKUP-bit DVSTCTR register.
 : (Output Remote wakeup signal when PeripheralController function is selected)
 Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_PSetWkup(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.DVSTCTR0.WORD |= USB_WKUP;
    #else
    USB_M1.DVSTCTR0.WORD |= USB_WKUP;
    #endif
} /* eof HW_USB_PSetWkup() */

/******************************************************************************
 Function Name   : HW_USB_PSetEnbRsme
 Description     : Enable interrupt from RESUME
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void HW_USB_PSetEnbRsme(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTENB0.WORD |= USB_RSME;
    #else
    USB_M1.INTENB0.WORD |= USB_RSME;
    #endif
} /* eof HW_USB_PSetEnbRsme() */

/******************************************************************************
 Function Name   : HW_USB_PClearEnbRsme
 Description     : Disable interrupt from RESUME
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void HW_USB_PClearEnbRsme(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTENB0.WORD &= (~USB_RSME);
    #else
    USB_M1.INTENB0.WORD &= (~USB_RSME);
    #endif
} /* eof HW_USB_PClearEnbRsme() */

/******************************************************************************
 Function Name   : HW_USB_PClearStsResm
 Description     : Clear interrupt status of RESUME.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void HW_USB_PClearStsResm(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTSTS0.WORD = (uint16_t)~USB_RESM;
    #else
    USB_M1.INTSTS0.WORD = (uint16_t)~USB_RESM;
    #endif
} /* eof HW_USB_PClearStsResm() */

/******************************************************************************
 Function Name   : HW_USB_PClearStsValid
 Description     : Clear the Setup Packet Reception interrupt flag.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void HW_USB_PClearStsValid(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTSTS0.WORD = (uint16_t)~USB_VALID;
    #else
    USB_M1.INTSTS0.WORD = (uint16_t)~USB_VALID;
    #endif
} /* eof HW_USB_PClearStsValid() */

/******************************************************************************
 Function Name   : HW_USB_PSetCcpl
 Description     : Enable termination of control transfer status stage.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void HW_USB_PSetCcpl(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.DCPCTR.WORD |= USB_CCPL;
    #else
    USB_M1.DCPCTR.WORD |= USB_CCPL;
    #endif
} /* eof HW_USB_PSetCcpl() */

/******************************************************************************
 Function Name   : HW_USB_pmodule_init
 Description     : 
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void HW_USB_pmodule_init( void )
{

    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG.WORD |= USB_SCKE;
    while( USB_SCKE != (USB_M0.SYSCFG.WORD & USB_SCKE) );

    USB_M0.SYSCFG.WORD |= USB_USBE;
    USB_M0.CFIFOSEL.WORD  = USB0_CFIFO_MBW;
    USB_M0.D0FIFOSEL.WORD = USB0_D0FIFO_MBW;
    USB_M0.D1FIFOSEL.WORD = USB0_D1FIFO_MBW;
#if USB_CFG_ENDIAN == USB_CFG_BIG
    USB_M0.CFIFOSEL.WORD  |= USB_BIGEND;
    USB_M0.D0FIFOSEL.WORD |= USB_BIGEND;
    USB_M0.D1FIFOSEL.WORD |= USB_BIGEND;
#endif  /* USB_CFG_ENDIAN == USB_CFG_BIG */
    USB_M0.INTENB0.WORD = (USB_BEMPE | USB_NRDYE | USB_BRDYE | USB_VBSE | USB_DVSE | USB_CTRE);
    #else   /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            #if USB_CFG_CLKSEL == USB_CFG_20MHZ
    USB_M1.PHYSET.WORD &= ~USB_CLKSEL;
    USB_M1.PHYSET.WORD |= USB_CLKSEL_20;
            #endif
    usb_cpu_delay_1us((uint16_t) 1);                    /* wait 1usec */
    USB_M1.PHYSET.WORD &= ~USB_DIRPD;
    usb_cpu_delay_xms(1); /* wait 1msec */
    USB_M1.PHYSET.WORD &= (~USB_PLLRESET);
    USB_M1.LPSTS.WORD |= USB_SUSPENDM;
    while( USB_PLLLOCK != (USB_M1.PLLSTA.WORD & USB_PLLLOCK ) );
    USB_M1.PHYSET.WORD |= USB_REPSEL_16;
    USB_M1.BUSWAIT.WORD = (USB_CFG_BUSWAIT | 0xF0);
    USB_M1.SYSCFG.WORD |= USB_USBE;

    USB_M1.CFIFOSEL.WORD  = USB1_CFIFO_MBW;
    USB_M1.D0FIFOSEL.WORD = USB1_D0FIFO_MBW;
    USB_M1.D1FIFOSEL.WORD = USB1_D1FIFO_MBW;

#if USB_CFG_ENDIAN == USB_CFG_BIG
    USB_M1.CFIFOSEL.WORD  |= USB_BIGEND;
    USB_M1.D0FIFOSEL.WORD |= USB_BIGEND;
    USB_M1.D1FIFOSEL.WORD |= USB_BIGEND;
#endif  /* USB_CFG_ENDIAN == USB_CFG_BIG */

    USB_M1.INTENB0.WORD = (USB_BEMPE | USB_NRDYE | USB_BRDYE | USB_VBSE | USB_DVSE | USB_CTRE);
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        #if defined(BSP_MCU_RX63N)
    USB_M1.SYSCFG.WORD |= USB_SCKE;
    while( USB_SCKE != (USB_M1.SYSCFG.WORD & USB_SCKE) );

    USB_M1.SYSCFG.WORD |= USB_USBE;
    USB_M1.INTENB0.WORD = (USB_BEMPE | USB_NRDYE | USB_BRDYE | USB_VBSE | USB_DVSE | USB_CTRE);
    HW_USB_SetMbw(USB_NULL, USB_CUSE, USB1_CFIFO_MBW);
        #endif  /* defined(BSP_MCU_RX63N) */
    #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
} /* eof HW_USB_pmodule_init() */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 End of file
 ******************************************************************************/

