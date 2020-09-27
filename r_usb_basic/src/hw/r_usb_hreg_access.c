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
 * File Name    : r_usb_hreg_access.c
 * Description  : USB IP Host control register access code
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
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 External variables and functions
 ******************************************************************************/
extern uint16_t usb_chattaring (uint16_t *syssts);
extern void usb_cpu_delay_xms (uint16_t time);

/******************************************************************************
 Function Name   : HW_USB_HSetRwupe
 Description     : Set the RWUPE-bit specified port's DVSTCTR0 reg-
 : ister. When host. To allow detection of remote wake-up from 
 : a USB Function.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetRwupe (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->DVSTCTR0.WORD |= USB_RWUPE;
    }
} /* End of function HW_USB_HSetRwupe() */

/******************************************************************************
 Function Name   : HW_USB_HClearRwupe
 Description     : Clear the RWUPE-bit specified port's DVSTCTR0 reg-
 : ister. When host. To prohibit detection of remote wake-up from 
 : a USB Function.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearRwupe (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->DVSTCTR0.WORD &= (~USB_RWUPE);
    }
} /* End of function HW_USB_HClearRwupe() */

/******************************************************************************
 Function Name   : HW_USB_HsetResume
 Description     : Set the RESUME-bit specified port's DVSTCTR0 register 
 : When host. To allow output of resume signal to a USB Function.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HsetResume (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->DVSTCTR0.WORD |= USB_RESUME;
    }
} /* End of function HW_USB_HsetResume() */

/******************************************************************************
 Function Name   : HW_USB_HClearResume
 Description     : Clear the RESUME-bit specified port's DVSTCTR0 register 
 : When host. To prohibit output of resume signal to a USB Func-
 : tion.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearResume (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->DVSTCTR0.WORD &= (~USB_RESUME);
    }
} /* End of function HW_USB_HClearResume() */

/******************************************************************************
 Function Name   : HW_USB_HSetUact
 Description     : Set UACT-bit (USB Bus Enable) specified port's DVSTCTR0 
 : register. When Host, to output SOF.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetUact (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->DVSTCTR0.WORD |= USB_UACT;
    }
} /* End of function HW_USB_HSetUact() */

/******************************************************************************
 Function Name   : HW_USB_HClearUact
 Description     : Clear UACT-bit (USB Bus Enable) specified port's DVSTCTR0 
 : register. When Host, to prohibit output SOF.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearUact (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->DVSTCTR0.WORD &= (~USB_UACT);
    }
} /* End of function HW_USB_HClearUact() */

/******************************************************************************
 Function Name   : HW_USB_HWriteIntenb
 Description     : Write the specified data to the specified port's INTENB register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HWriteIntenb (usb_utr_t *ptr, uint16_t port, uint16_t data)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD = data;
    }
} /* End of function HW_USB_HWriteIntenb() */

/******************************************************************************
 Function Name   : HW_USB_HSetEnbOvrcre
 Description     : Set specified port's OVRCRE-bit (Overcurrent Input Change Int-
 : errupt Status Enable) in the INTENB1 register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetEnbOvrcre (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD |= USB_OVRCRE;
    }
} /* End of function HW_USB_HSetEnbOvrcre() */

/******************************************************************************
 Function Name   : HW_USB_HClearEnbOvrcre
 Description     : Clear the OVRCRE-bit of the specified port's INTENB1 register,
 : to prohibit VBUS interrupts.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearEnbOvrcre (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD &= (USB_OVRCRE);
    }
} /* End of function HW_USB_HClearEnbOvrcre() */

/******************************************************************************
 Function Name   : HW_USB_HSetEnbBchge
 Description     : The BCHGE-bit (USB Bus Change Interrupt Enable) is set in the 
 : specified port's INTENB1 register. This will cause a BCHG 
 : interrupt when a change of USB bus state has been detected.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetEnbBchge (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD |= USB_BCHGE;
    }
} /* End of function HW_USB_HSetEnbBchge() */

/******************************************************************************
 Function Name   : HW_USB_HClearEnbBchge
 Description     : The BCHGE-bit (USB Bus Change Interrupt Enable) is cleared in 
 : the specified port's INTENB1 register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearEnbBchge (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD &= (~USB_BCHGE);
    }
} /* End of function HW_USB_HClearEnbBchge() */

/******************************************************************************
 Function Name   : HW_USB_HSetEnbDtche
 Description     : Enable the specified port's DTCHE-interrupt "Disconnection 
 : Detection" by setting the DTCHE-bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetEnbDtche (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD |= USB_DTCHE;
    }
} /* End of function HW_USB_HSetEnbDtche() */

/******************************************************************************
 Function Name   : HW_USB_HClearEnbDtche
 Description     : Disable the specified port's DTCHE-interrupt "Disconnection 
 : Detection" by clearing the DTCHE-bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearEnbDtche (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD &= (~USB_DTCHE);
    }
} /* End of function HW_USB_HClearEnbBchge() */

/******************************************************************************
 Function Name   : HW_USB_HSetEnbAttche
 Description     : Enable the specified port's ATTCHE-interrupt "Connection 
 : Detection" by setting the ATTCHE-bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetEnbAttche (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD |= USB_ATTCHE;
    }
} /* End of function HW_USB_HSetEnbAttche() */

/******************************************************************************
 Function Name   : HW_USB_HClearEnbAttche
 Description     : Disable the specified port's ATTCHE-interrupt "Disconnection 
 : Detection" by clearing the ATTCHE-bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearEnbAttche (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTENB1.WORD &= (~USB_ATTCHE);
    }
} /* End of function HW_USB_HClearEnbAttche() */

/******************************************************************************
 Function Name   : HW_USB_HSetEnbSigne
 Description     : Enable the SIGNE-interrupt "Setup Transaction
 : Error" by setting the SIGNE-bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetEnbSigne (usb_utr_t *ptr)
{
    ptr->ipp->INTENB1.WORD |= USB_SIGNE;
} /* End of function HW_USB_HSetEnbSigne() */

/******************************************************************************
 Function Name   : HW_USB_HSetEnbSacke
 Description     : Enable the SACKE-interrupt "Setup Transaction 
 : Normal Response" by setting the SACKE-bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetEnbSacke (usb_utr_t *ptr)
{
    ptr->ipp->INTENB1.WORD |= USB_SACKE;
} /* End of function HW_USB_HSetEnbSacke() */

/******************************************************************************
 Function Name   : HW_USB_HSetEnbPddetinte
 Description     : Enable the PDDETINT-interrupt "Connection Detection for 
 : Battery Charging Supporting Device" by setting 
 : the PDDETINTE-bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetEnbPddetinte (usb_utr_t *ptr)
{
    #if USB_NUM_USBIP == 2
    if (ptr->ip == USB_USBIP_1)
    {
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        ptr->ipp1->INTENB1.WORD |= USB_PDDETINTE;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
    #endif /* USB_NUM_USBIP == 2 */
} /* End of function HW_USB_HSetEnbPddetinte() */

/******************************************************************************
 Function Name   : HW_USB_HWriteIntsts
 Description     : Write the specified data to the specified port's INTSTS1 reg-
 : ister.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HWriteIntsts (usb_utr_t *ptr, uint16_t port, uint16_t data)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTSTS1.WORD = data;
    }
} /* End of function HW_USB_HWriteIntsts() */

/******************************************************************************
 Function Name   : HW_USB_HClearStsOvrcr
 Description     : Clear the specified port's OVRCR-bit; "Overcurrent 
 : Input Change Interrupt Status".
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearStsOvrcr (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t) (~USB_OVRCR & INTSTS1_MASK);
    }
} /* End of function HW_USB_HClearStsOvrcr() */

/******************************************************************************
 Function Name   : HW_USB_HClearStsBchg
 Description     : Clear the specified port's BCHG-bit; "USB Bus Change Interrupt 
 : Status".
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearStsBchg (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t) (~USB_BCHG & INTSTS1_MASK);
    }
} /* End of function HW_USB_HClearStsBchg() */

/******************************************************************************
 Function Name   : HW_USB_HClearStsDtch
 Description     : Clear the specified port's DTCH-bit; "USB Disconnection Detec-
 : tion Interrupt Status".
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearStsDtch (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t) (~USB_DTCH & INTSTS1_MASK);
    }
} /* End of function HW_USB_HClearStsDtch() */

/******************************************************************************
 Function Name   : HW_USB_HClearStsAttch
 Description     : Clear the specified port's ATTCH-bit; "ATTCH Interrupt Status".
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearStsAttch (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t) (~USB_ATTCH & INTSTS1_MASK);
    }
} /* End of function HW_USB_HClearStsAttch() */

/******************************************************************************
 Function Name   : HW_USB_HClearStsSign
 Description     : Clear the SIGN-bit; "Setup Transaction Error
 : Interrupt Status".
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearStsSign (usb_utr_t *ptr)
{
    ptr->ipp->INTSTS1.WORD = (uint16_t) (~USB_SIGN & INTSTS1_MASK);
} /* End of function HW_USB_HClearStsSign() */

/******************************************************************************
 Function Name   : HW_USB_HClearStsSack
 Description     : Clear the SACK-bit; "Setup Transaction Normal
 : Response Interrupt Status".
 : Interrupt Status".
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearStsSack (usb_utr_t *ptr)
{
    ptr->ipp->INTSTS1.WORD = (uint16_t) (~USB_SACK & INTSTS1_MASK);
} /* End of function HW_USB_HClearStsSack() */

/******************************************************************************
 Function Name   : HW_USB_HClearStsPddetint
 Description     : Clear the PDDETINT-bit; "
 : ".
 : Interrupt Status".
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HClearStsPddetint (usb_utr_t *ptr, uint16_t port)
{
    #if USB_NUM_USBIP == 2
    if (port == USB_PORT0)
    {
        if (ptr->ip == USB_USBIP_1)
        {
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            ptr->ipp1->INTSTS1.WORD = (uint16_t) (~USB_PDDETINT & INTSTS1_MASK);
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        }
    }
    #endif /* USB_NUM_USBIP == 2 */
} /* End of function HW_USB_HClearStsPddetint() */

/******************************************************************************
 Function Name   : HW_USB_HWriteUsbreq
 Description     : Write bRequest and bmRequestType to USBREQ register.
 : When Host, the values of bRequest and bmRequestType 
 : to be transmitted are written. (When Function, the received 
 : values of bRequest and bmRequestType are stored in USBREQ).
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HWriteUsbreq (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->USBREQ.WORD = data;
} /* End of function HW_USB_HWriteUsbreq() */

/******************************************************************************
 Function Name   : HW_USB_HSetUsbval
 Description     : Write the specified 'wValue' to USBVAL register,
 : to write the USB request. When Host, the value of 
 : wValue to be transmitted is set. (When Function, the value of 
 : wValue that has been received is stored in USBREQ.)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetUsbval (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->USBVAL = data;
} /* End of function HW_USB_HSetUsbval() */

/******************************************************************************
 Function Name   : HW_USB_HSetUsbindx
 Description     : Write the specified 'wIndex', the USB request, to USBINDX
 : register, for host setup requests for control 
 : transfers.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetUsbindx (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->USBINDX = data;
} /* End of function HW_USB_HSetUsbindx() */

/******************************************************************************
 Function Name   : HW_USB_HSetUsbleng
 Description     : Write the specified 'wLength' value to USBINDX register, 
 : for host setup requests for control.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetUsbleng (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->USBLENG = data;
} /* End of function HW_USB_HSetUsbleng() */

/******************************************************************************
 Function Name   : HW_USB_HWriteDcpctr
 Description     : Write the specified data value to the DCPCTR register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HWriteDcpctr (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->DCPCTR.WORD = data;
} /* End of function HW_USB_HWriteDcpctr() */

/******************************************************************************
 Function Name   : HW_USB_HSetSureq
 Description     : Set te SUREQ-bit in the DCPCTR register 
 : (Set SETUP packet send when HostController function is selected)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetSureq (usb_utr_t *ptr)
{
    ptr->ipp->DCPCTR.WORD |= USB_SUREQ;
} /* End of function HW_USB_HSetSureq() */

/******************************************************************************
 Function Name   : HW_USB_HReadDevadd
 Description     : Return the DEVADD register value for the specified USB device 
 ; address.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  devsel      ; USB device address value 
 Return value    : DEVADDx content
 ******************************************************************************/
uint16_t HW_USB_HReadDevadd (usb_utr_t *ptr, uint16_t devsel)
{
    volatile uint16_t *preg;
    uint16_t devadr;
    uint16_t return_value;

    devadr = devsel >> USB_DEVADDRBIT;

    if (devadr > USB_MAXDEVADDR)
    {
        return USB_ERROR;
    }
    else
    {
        preg = (uint16_t *) &(ptr->ipp->DEVADD0) + devadr;
        return_value = ((*preg) & (USB_UPPHUB | USB_HUBPORT | USB_USBSPD));
        return return_value;
    }
} /* End of function HW_USB_HReadDevadd() */

/******************************************************************************
 Function Name   : HW_USB_HRmwDevadd
 Description     : Read-modify-write the specified devsel's DEVADD.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  devsel: Device address
 : uint16_t  data  : The value to write.
 : uint16_t  width : Bit pattern to read-modify-write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HRmwDevadd (usb_utr_t *ptr, uint16_t devsel, uint16_t data, uint16_t width)
{
    volatile uint16_t *preg;
    uint16_t buf;
    uint16_t devadr;

    devadr = devsel >> USB_DEVADDRBIT;

    preg = (uint16_t *) &(ptr->ipp->DEVADD0) + devadr;

    buf = *preg;
    buf &= (~width);
    buf |= (data & width);
    *preg = buf;
} /* End of function HW_USB_HRmwDevadd() */

/******************************************************************************
 Function Name   : HW_USB_HSetUsbspd
 Description     : Set the DEVADD register's USBSPD for the specified device add-
 : ress.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  devsel      ; USB device address value
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetUsbspd (usb_utr_t *ptr, uint16_t devsel, uint8_t data)
{
    volatile uint16_t *preg;
    uint16_t devadr;

    devadr = devsel >> USB_DEVADDRBIT;

    preg = (uint16_t *) &(ptr->ipp->DEVADD0) + devadr;

    (*preg) &= (~USB_USBSPD);
    (*preg) |= data;
} /* End of function HW_USB_HSetUsbspd() */

/******************************************************************************
 Function Name   : HW_USB_HsetDcpmode
 Description     : Set DCPMODE bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HsetDcpmode (usb_utr_t *ptr)
{
    #if USB_NUM_USBIP == 2
    if (ptr->ip == USB_USBIP_1)
    {
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        ptr->ipp1->BCCTRL.WORD |= USB_DCPMODE;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
    #endif /* USB_NUM_USBIP == 2 */
} /* End of function HW_USB_HsetDcpmode() */

/******************************************************************************
 Function Name   : HW_USB_hmodule_init
 Description     : 
 Arguments       : 
 Return value    : none
 ******************************************************************************/
void HW_USB_hmodule_init (usb_ctrl_t *p_ctrl)
{
    uint16_t sts;

    if ( USB_IP0 == p_ctrl->module)
    {
        USB_M0.SYSCFG.WORD |= USB_SCKE;
        while ( USB_SCKE != (USB_M0.SYSCFG.WORD & USB_SCKE))
            ;

    #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        USB_M0.PHYSLEW.LONG = 0xE;
    #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

        USB_M0.SYSCFG.WORD |= USB_DCFM;

        USB_M0.SYSCFG.WORD |= USB_DRPD;

        sts = usb_chattaring((uint16_t *) &USB_M0.SYSSTS0.WORD);

        USB_M0.SYSCFG.WORD |= USB_USBE;

        USB_M0.CFIFOSEL.WORD  = USB0_CFIFO_MBW;
        USB_M0.D0FIFOSEL.WORD = USB0_D0FIFO_MBW;
        USB_M0.D1FIFOSEL.WORD = USB0_D1FIFO_MBW;

    #if USB_CFG_ENDIAN == USB_CFG_BIG
        USB_M0.CFIFOSEL.WORD |= USB_BIGEND;
        USB_M0.D0FIFOSEL.WORD |= USB_BIGEND;
        USB_M0.D1FIFOSEL.WORD |= USB_BIGEND;
    #endif  /* USB_CFG_ENDIAN == USB_CFG_BIG */

        switch (sts)
        {
            case USB_FS_JSTS : /* USB device already connected */
            case USB_LS_JSTS :
                USB_M0.DVSTCTR0.WORD |= USB_USBRST;
                usb_cpu_delay_xms((uint16_t) 50); /* Need to wait greater equal 10ms in USB spec */
                while ( USB_HSPROC == (USB_M0.DVSTCTR0.WORD & USB_HSPROC))
                    ; /* HSPROC = 0100b */

                if ( USB_LSMODE == (USB_M0.DVSTCTR0.WORD & USB_RHST))
                {
                    USB_M0.SOFCFG.WORD |= USB_TRNENSEL;
                }

                USB_M0.DVSTCTR0.WORD |= USB_UACT;
                USB_M0.DVSTCTR0.WORD &= ~USB_USBRST;
            break;

            case USB_SE0 : /* USB device no connected */
                USB_M0.INTENB1.WORD = USB_ATTCH;
            break;

            default :
            break;
        }
        USB_M0.INTSTS1.WORD &= (~USB_OVRCRE & INTSTS1_MASK);
        USB_M0.INTENB0.WORD = (USB_BEMPE | USB_NRDYE | USB_BRDYE);
        USB_M0.INTENB1.WORD = (USB_OVRCRE | USB_ATTCH);
    }
    #if USB_NUM_USBIP == 2
    else
    {
        USB_M1.SYSCFG.WORD |= USB_DCFM;
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            #if USB_CFG_CLKSEL == USB_CFG_20MHZ
        USB_M1.PHYSET.WORD &= ~USB_CLKSEL;
        USB_M1.PHYSET.WORD |= USB_CLKSEL_20;
            #endif
        USB_M1.PHYSET.WORD |= USB_REPSEL_16;
        USB_M1.BUSWAIT.WORD = (USB_CFG_BUSWAIT | 0xF0);
        usb_cpu_delay_1us((uint16_t) 1);                    /* wait 1usec */
        USB_M1.PHYSET.WORD &= ~USB_DIRPD;
        usb_cpu_delay_xms(1); /* wait 1msec */
        USB_M1.PHYSET.WORD &= (~USB_PLLRESET);
        USB_M1.LPSTS.WORD |= USB_SUSPENDM;
        while( USB_PLLLOCK != (USB_M1.PLLSTA.WORD & USB_PLLLOCK ) );
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        USB_M1.SYSCFG.WORD |= USB_CNEN;
        USB_M1.SOFCFG.WORD |= USB_INTL;
        sts = usb_chattaring( (uint16_t *)&USB_M1.SYSCFG.WORD );
        USB_M1.SYSCFG.WORD |= USB_DRPD;
        USB_M1.SYSCFG.WORD |= USB_USBE;
        USB_M1.CFIFOSEL.WORD  = USB1_CFIFO_MBW;
        USB_M1.D0FIFOSEL.WORD = USB1_D0FIFO_MBW;
        USB_M1.D1FIFOSEL.WORD = USB1_D1FIFO_MBW;
        #if USB_CFG_ENDIAN ==USB_CFG_BIG
        USB_M1.CFIFOSEL.WORD |= USB_BIGEND;
        USB_M1.D0FIFOSEL.WORD |= USB_BIGEND;
        USB_M1.D1FIFOSEL.WORD |= USB_BIGEND;
        #endif
        switch( sts )
        {
            case USB_FS_JSTS: /* USB device already connected */
            case USB_LS_JSTS:
            if( USB_FS_JSTS == sts )
            {
                USB_M1.SYSCFG.WORD |= USB_HSE;
            }
            USB_M1.DVSTCTR0.WORD |= USB_USBRST;
            usb_cpu_delay_xms((uint16_t)50); /* Need to wait greater equal 10ms in USB spec */
            while( USB_HSPROC == (USB_M1.DVSTCTR0.WORD & USB_HSPROC) );
            if( USB_LSMODE == (USB_M1.DVSTCTR0.WORD & USB_RHST) )
            {
                USB_M1.SOFCFG.WORD |= USB_TRNENSEL;
            }
            USB_M1.DVSTCTR0.WORD |= USB_UACT;
            USB_M1.DVSTCTR0.WORD &= ~USB_USBRST;
            break;
            case USB_SE0: /* USB device no connected */
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            USB_M1.INTENB1.WORD = USB_ATTCH;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
            break;
            default:
            break;
        }
        USB_M1.INTSTS1.WORD &= (~USB_OVRCRE & INTSTS1_MASK);
        USB_M1.INTENB0.WORD = (USB_BEMPE | USB_NRDYE | USB_BRDYE);
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        USB_M1.INTENB1.WORD = (USB_OVRCRE | USB_ATTCH);
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
    #endif /* USB_NUM_USBIP == 2 */
} /* End of function HW_USB_hmodule_init() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_HSetTrnensel
 Description     : When host, set the TRNENSEL-bit; "Transac-
 : tion-Enabled Time Select" for low-speed USB communication.
 : This bit should be set to 0 if USB Function.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_HSetTrnensel (usb_utr_t *ptr)
{
    ptr->ipp->SOFCFG.WORD |= USB_TRNENSEL;
} /* End of function HW_USB_HSetTrnensel() */

/******************************************************************************
 End of file
 ******************************************************************************/
