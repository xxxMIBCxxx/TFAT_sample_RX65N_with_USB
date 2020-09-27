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
 * File Name    : r_usb_reg_access.c
 * Description  : USB IP register access code
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
#if USB_CFG_DTCDMA == USB_CFG_ENABLE
#include "r_usb_dmac.h"
#endif /* USB_CFG_DTCDMA == USB_CFG_ENABLE */

/******************************************************************************
 Function Name   : HW_USB_ReadSyscfg
 Description     : Returns the specified port's SYSCFG register value.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t port   : Port number (not used $REA)
 Return value    : SYSCFG content.
 ******************************************************************************/
uint16_t HW_USB_ReadSyscfg (usb_utr_t *ptr, uint16_t port)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return USB_M0.SYSCFG.WORD;
    #else
        return USB_M1.SYSCFG.WORD;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        return ptr->ipp->SYSCFG.WORD;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadSyscfg() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_WriteSyscfg
 Description     : Write specified value to the SYSCFG register of the given port.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t port   : Port number (only port 0 used $REA)
 : uint16_t data   : Value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteSyscfg (usb_utr_t *ptr, uint16_t port, uint16_t data)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->SYSCFG.WORD = data;
    }
} /* eof HW_USB_WriteSyscfg */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : HW_USB_SetCnen
 Description     : Enable single end receiver.
 Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
 Return value    : -
 ******************************************************************************/
void HW_USB_SetCnen(void)
{
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
        #else
    USB_M1.SYSCFG.WORD |= USB_CNEN;
        #endif
} /* eof HW_USB_SetCnen() */
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 Function Name   : HW_USB_ClearCnen
 Description     : Disable single end receiver.
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 Return value    : -
 ******************************************************************************/
void HW_USB_ClearCnen(usb_utr_t *ptr)
{
    #if USB_NUM_USBIP == 2
    if (USB_NULL == ptr)
    {
        #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
            #if USB_CFG_USE_USBIP == USB_CFG_IP0
            #else
        USB_M1.SYSCFG.WORD &= (~USB_CNEN);
            #endif
        #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
        #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp1->SYSCFG.WORD &= (~USB_CNEN);
        #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_ClearCnen() */

/******************************************************************************
 Function Name   : HW_USB_SetHse
 Description     : Not processed as the functionality is provided by R8A66597(ASSP).
 Arguments       : usb_utr_t *ptr  : Not used.
 : uint16_t  port  : Not used.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetHse(usb_utr_t *ptr, uint16_t port)
{
    #if USB_NUM_USBIP == 2
    if (USB_NULL == ptr)
    {
        #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
            #if USB_CFG_USE_USBIP == USB_CFG_IP0
            #else
        USB_M1.SYSCFG.WORD |= USB_HSE;
            #endif
        #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
        #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (ptr->ip == USB_USBIP_1)
        {
            ptr->ipp1->SYSCFG.WORD |= USB_HSE;
        }
        #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_SetHse() */

/******************************************************************************
 Function Name   : HW_USB_ClearHse
 Description     : Clears HSE bit of the specified port's SYSCFG register
 Arguments       : usb_utr_t *ptr : USB system internal structure. Selects channel.
 : uint16_t  port : Port number
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearHse(usb_utr_t *ptr, uint16_t port)
{
    #if USB_NUM_USBIP == 2
    if (USB_NULL == ptr)
    {
        #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
            #if USB_CFG_USE_USBIP == USB_CFG_IP0
            #else
        USB_M1.SYSCFG.WORD &= (~USB_HSE);
            #endif
        #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
        #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (ptr->ip == USB_USBIP_1)
        {
            ptr->ipp->SYSCFG.WORD &= (~USB_HSE);
        }
        #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_ClearHse() */
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : HW_USB_SetDcfm
 Description     : DCFM-bit set of register SYSCFG
 : (USB Host mode is selected.)
 Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetDcfm(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG.WORD |= USB_DCFM;
    #else
    USB_M1.SYSCFG.WORD |= USB_DCFM;
    #endif
} /* eof HW_USB_SetDcfm() */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 Function Name   : HW_USB_ClearDcfm
 Description     : DCFM-bit clear of register SYSCFG.
 : (USB Peripheral mode is selected.)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearDcfm (usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.SYSCFG.WORD &= (~USB_DCFM);
    #else
        USB_M1.SYSCFG.WORD &= (~USB_DCFM);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->SYSCFG.WORD &= (~USB_DCFM);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_ClearDcfm() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_ClearDrpd
 Description     : Clear bit of the specified port's SYSCFG DRPD register.
 : (for USB Host mode; Enable D + / D-line PullDown.)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : Port number
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearDrpd (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->SYSCFG.WORD &= (~USB_DRPD);
    }
} /* eof HW_USB_ClearDrpd() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_SetUsbe
 Description     : Enable USB operation.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetUsbe (usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.SYSCFG.WORD |= USB_USBE;
    #else
        USB_M1.SYSCFG.WORD |= USB_USBE;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->SYSCFG.WORD |= USB_USBE;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetUsbe() */

/******************************************************************************
 Function Name   : HW_USB_ClearUsbe
 Description     : Enable USB operation.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearUsbe (usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.SYSCFG.WORD &= (~USB_USBE);
    #else
        USB_M1.SYSCFG.WORD &= (~USB_USBE);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->SYSCFG.WORD &= (~USB_USBE);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_ClearUsbe() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/******************************************************************************
 Function Name   : HW_USB_SetBusWait
 Description     : Set BUSWAIT register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetBusWait(usb_utr_t *ptr)
{
        #if USB_NUM_USBIP == 2
    ptr -> ipp1 -> BUSWAIT.WORD = USB_CFG_BUSWAIT;
        #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_SetBusWait() */
    #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_ReadSyssts
 Description     : Returns the value of the specified port's SYSSTS register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number. ($REA not used.)
 Return value    : SYSSTS0 content
 ******************************************************************************/
uint16_t HW_USB_ReadSyssts (usb_utr_t *ptr, uint16_t port)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return (uint16_t)(USB_M0.SYSSTS0.WORD);
    #else
        return (uint16_t)(USB_M1.SYSSTS0.WORD);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        return (uint16_t) (ptr->ipp->SYSSTS0.WORD);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadSyssts() */

/******************************************************************************
 Function Name   : HW_USB_ReadDvstctr
 Description     : Returns the specified port's DVSTCTR register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number. ($REA not used.)
 Return value    : DVSTCTR0 content
 ******************************************************************************/
uint16_t HW_USB_ReadDvstctr (usb_utr_t *ptr, uint16_t port)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return (uint16_t)(USB_M0.DVSTCTR0.WORD);
    #else
        return (uint16_t)(USB_M1.DVSTCTR0.WORD);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        return (uint16_t) (ptr->ipp->DVSTCTR0.WORD);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadDvstctr() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_WriteDvstctr
 Description     : Write data to the specified port's DVSTCTR register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : USB port number.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteDvstctr (usb_utr_t *ptr, uint16_t port, uint16_t data)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->DVSTCTR0.WORD = data;
    }
} /* eof HW_USB_WriteDvstctr() */

/******************************************************************************
 Function Name   : HW_USB_RmwDvstctr
 Description     : Read-modify-write the specified port's DVSTCTR.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : Port number
 : uint16_t  data  : The value to write.
 : uint16_t  bitptn: Bit pattern to read-modify-write.
 Return value    : none
 ******************************************************************************/
void HW_USB_RmwDvstctr (usb_utr_t *ptr, uint16_t port, uint16_t data, uint16_t bitptn)
{
    uint16_t buf;

    if (USB_PORT0 == port)
    {
        buf = ptr->ipp->DVSTCTR0.WORD;
        buf &= (~bitptn);
        buf |= (data & bitptn);
        ptr->ipp->DVSTCTR0.WORD = buf;
    }
} /* eof HW_USB_RmwDvstctr() */

/******************************************************************************
 Function Name   : HW_USB_ClearDvstctr
 Description     : Clear the bit pattern specified in argument, of the specified 
 : port's DVSTCTR register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : Port number
 : uint16_t  bitptn: Bit pattern to read-modify-write.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearDvstctr (usb_utr_t *ptr, uint16_t port, uint16_t bitptn)
{
    if (USB_PORT0 == port)
    {
        ptr->ipp->DVSTCTR0.WORD &= (~bitptn);
    }
} /* eof HW_USB_ClearDvstctr() */

/******************************************************************************
 Function Name   : HW_USB_SetVbout
 Description     : Set specified port's VBOUT-bit in the DVSTCTR register.
 : (To output a "High" to pin VBOUT.) 
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : Port number
 Return value    : none
 ******************************************************************************/
void HW_USB_SetVbout (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
    #if USB_CFG_VBUS == USB_CFG_HIGH
        ptr->ipp->DVSTCTR0.WORD |= USB_VBUSEN;
    #else  /* USB_CFG_VBUS == USB_CFG_HIGH */
        ptr->ipp->DVSTCTR0.WORD &= (~USB_VBUSEN);
    #endif /* USB_CFG_VBUS == USB_CFG_HIGH */
    }
} /* eof HW_USB_SetVbout() */

/******************************************************************************
 Function Name   : HW_USB_ClearVbout
 Description     : Clear specified port's VBOUT-bit in the DVSTCTR register.
 : (To output a "Low" to pin VBOUT.)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  port  : Port number
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearVbout (usb_utr_t *ptr, uint16_t port)
{
    if (USB_PORT0 == port)
    {
    #if USB_CFG_VBUS == USB_CFG_HIGH
        ptr->ipp->DVSTCTR0.WORD &= (~USB_VBUSEN);
    #else  /* USB_CFG_VBUS == USB_CFG_HIGH */
        ptr->ipp->DVSTCTR0.WORD |= USB_VBUSEN;
    #endif /* USB_CFG_VBUS == USB_CFG_HIGH */
    }
} /* eof HW_USB_ClearVbout() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#if defined(BSP_MCU_RX71M)
/******************************************************************************
 Function Name   : HW_USB_SetUtst
 Description     : Not processed as the functionality is provided by R8A66597(ASSP).
 Arguments       : usb_utr_t *ptr  : 
 : uint16_t  data
 Return value    : none
 ******************************************************************************/
void HW_USB_SetUtst(usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
        #else
        USB_M1.TESTMODE.WORD = data;
        #endif
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        #if USB_NUM_USBIP == 2
        ptr->ipp1->TESTMODE.WORD = data;
        #endif /* USB_NUM_USBIP == 2 */
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetUtst() */
#endif  /* (BSP_MCU_RX71M) */

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/******************************************************************************
 Function Name   : HW_USB_ReadFifo32
 Description     : Data is read from the specified pipemode's FIFO register, 32-bits 
 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : CFIFO/D0FIFO/D1FIFO content (32-bit)
 ******************************************************************************/
uint32_t HW_USB_ReadFifo32(usb_utr_t *ptr, uint16_t pipemode)
{
    #if USB_NUM_USBIP == 2
    uint32_t data;

    if (USB_NULL == ptr)
    {
        #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        switch (pipemode)
        {
            case USB_CUSE:
            #if USB_CFG_USE_USBIP == USB_CFG_IP1
            data = USB_M1.CFIFO.LONG;
            #else  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE2);
            #endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE2);
            break;
        }
        #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
        #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        switch (pipemode)
        {
            case USB_CUSE:
            data = ptr->ipp1->CFIFO.LONG;
            break;
            case USB_D0USE:
            data = ptr->ipp1->D0FIFO.LONG;
            break;
            case USB_D1USE:
            data = ptr->ipp1->D1FIFO.LONG;
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE2);
            break;
        }
        #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return data;
    #else  /* USB_NUM_USBIP == 2 */
    return 0;
    #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_ReadFifo32() */

/******************************************************************************
 Function Name   : HW_USB_WriteFifo32
 Description     : Data is written to the specified pipemode's FIFO register, 32-bits 
 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 : uint32_t  data      : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteFifo32(usb_utr_t *ptr, uint16_t pipemode, uint32_t data)
{
    #if USB_NUM_USBIP == 2
    if (USB_NULL == ptr)
    {
        #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        switch (pipemode)
        {
            case USB_CUSE:
            #if USB_CFG_USE_USBIP == USB_CFG_IP1
            USB_M1.CFIFO.LONG = data;
            #else  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE2);
            #endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE3);
            break;
        }
        #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
        #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        switch (pipemode)
        {
            case USB_CUSE:
            ptr->ipp1->CFIFO.LONG = data;
            break;
            case USB_D0USE:
            ptr->ipp1->D0FIFO.LONG = data;
            break;
            case USB_D1USE:
            ptr->ipp1->D1FIFO.LONG = data;
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE3);
            break;
        }
        #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_WriteFifo32() */
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

/******************************************************************************
 Function Name   : HW_USB_ReadFifo16
 Description     : Data is read from the specified pipemode's FIFO register, 16-bits 
 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : CFIFO/D0FIFO/D1FIFO content (16-bit)
 ******************************************************************************/
uint16_t HW_USB_ReadFifo16 (usb_utr_t *ptr, uint16_t pipemode)
{
    uint16_t data;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        switch (pipemode)
        {

            case USB_CUSE:
    #if USB_CFG_USE_USBIP == USB_CFG_IP1
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            data = USB_M1.CFIFO.WORD.H;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        #if defined(BSP_MCU_RX63N) 
            data = USB1.CFIFO.WORD;
        #endif  /* defined(BSP_MCU_RX63N) */
    #else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            data = USB_M0.CFIFO.WORD;
    #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE5);
            break;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (ptr->ip == USB_USBIP_0)
        {
            switch (pipemode)
            {
                case USB_CUSE :
                    data = ptr->ipp->CFIFO.WORD;
                break;
                case USB_D0USE :
                    data = ptr->ipp->D0FIFO.WORD;
                break;
                case USB_D1USE :
                    data = ptr->ipp->D1FIFO.WORD;
                break;
                default :
                    USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE4);
                break;
            }
        }
    #if USB_NUM_USBIP == 2
        else if (ptr->ip == USB_USBIP_1)
        {
            switch (pipemode)
            {
                case USB_CUSE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                data = ptr->ipp1->CFIFO.WORD.H;
        #else   /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                data = ptr->ipp1->CFIFO.WORD;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                break;
                case USB_D0USE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                data = ptr->ipp1->D0FIFO.WORD.H;
        #else   /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                data = ptr->ipp1->D0FIFO.WORD;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                break;
                case USB_D1USE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                data = ptr->ipp1->D1FIFO.WORD.H;
        #else   /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                data = ptr->ipp1->D1FIFO.WORD;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                break;
                default:
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE5);
                break;
            }
        }
    #endif /* USB_NUM_USBIP == 2 */
        else
        {
            /* Non */
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    return data;
} /* eof HW_USB_ReadFifo16() */

/******************************************************************************
 Function Name   : HW_USB_WriteFifo16
 Description     : Data is written to the specified pipemode's FIFO register, 16-bits 
 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 : uint16_t  data      : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteFifo16 (usb_utr_t *ptr, uint16_t pipemode, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        switch (pipemode)
        {

            case USB_CUSE:
    #if USB_CFG_USE_USBIP == USB_CFG_IP1
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            USB_M1.CFIFO.WORD.H = data;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        #if defined(BSP_MCU_RX63N) 
            USB1.CFIFO.WORD = data;
        #endif  /* defined(BSP_MCU_RX63N) */
    #else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            USB_M0.CFIFO.WORD = data;
    #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE7);
            break;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (ptr->ip == USB_USBIP_0)
        {
            switch (pipemode)
            {
                case USB_CUSE :
                    ptr->ipp->CFIFO.WORD = data;
                break;
                case USB_D0USE :
                    ptr->ipp->D0FIFO.WORD = data;
                break;
                case USB_D1USE :
                    ptr->ipp->D1FIFO.WORD = data;
                break;
                default :
                    USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE6);
                break;
            }
        }
    #if USB_NUM_USBIP == 2
        else if (ptr->ip == USB_USBIP_1)
        {
            switch (pipemode)
            {
                case USB_CUSE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                ptr->ipp1->CFIFO.WORD.H = data;
        #else   /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                ptr->ipp1->CFIFO.WORD = data;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                break;
                case USB_D0USE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                ptr->ipp1->D0FIFO.WORD.H = data;
        #else   /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                ptr->ipp1->D0FIFO.WORD = data;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                break;
                case USB_D1USE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                ptr->ipp1->D1FIFO.WORD.H = data;
        #else   /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                ptr->ipp1->D1FIFO.WORD = data;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
                break;
                default:
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE7);
                break;
            }
        }
    #endif /* USB_NUM_USBIP == 2 */
        else
        {
            /* Non */
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

} /* eof HW_USB_WriteFifo16() */

/******************************************************************************
 Function Name   : HW_UBS_WriteFifo8
 Description     : Data is written to the specified pipemode's FIFO register, 8-bits 
 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipdemode   : CUSE/D0DMA/D1DMA
 : uint8_t   data        : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_UBS_WriteFifo8 (usb_utr_t *ptr, uint16_t pipemode, uint8_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        switch (pipemode)
        {
            case USB_CUSE:
    #if USB_CFG_USE_USBIP == USB_CFG_IP1
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
            USB_M1.CFIFO.BYTE.HH = data;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        #if defined(BSP_MCU_RX63N) 
            USB_M1.CFIFO.BYTE.L = data;
        #endif  /* defined(BSP_MCU_RX63N) */
    #else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            USB_M0.CFIFO.BYTE.L = data;
    #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE11);
            break;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (ptr->ip == USB_USBIP_0)
        {
            switch (pipemode)
            {
                case USB_CUSE :
                    ptr->ipp->CFIFO.BYTE.L = data;
                break;
                case USB_D0USE :
                    ptr->ipp->D0FIFO.BYTE.L = data;
                break;
                case USB_D1USE :
                    ptr->ipp->D1FIFO.BYTE.L = data;
                break;
                default :
                    USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE10);
                break;
            }
        }
    #if USB_NUM_USBIP == 2
        else if (ptr->ip == USB_USBIP_1)
        {
            switch (pipemode)
            {
                case USB_CUSE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                ptr->ipp1->CFIFO.BYTE.HH = data;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        #if defined(BSP_MCU_RX63N) 
                ptr->ipp1->CFIFO.BYTE.H = data;
        #endif  /* defined(BSP_MCU_RX63N) */
                break;
                case USB_D0USE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                ptr->ipp1->D0FIFO.BYTE.HH = data;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        #if defined(BSP_MCU_RX63N) 
                ptr->ipp1->D0FIFO.BYTE.H = data;
        #endif  /* defined(BSP_MCU_RX63N) */
                break;
                case USB_D1USE:
        #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
                ptr->ipp1->D1FIFO.BYTE.HH = data;
        #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        #if defined(BSP_MCU_RX63N) 
                ptr->ipp1->D1FIFO.BYTE.H = data;
        #endif  /* defined(BSP_MCU_RX63N) */
                break;
                default:
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE11);
                break;
            }
        }
    #endif /* USB_NUM_USBIP == 2 */
        else
        {
            /* Non */
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_UBS_WriteFifo8() */

/******************************************************************************
 Function Name   : HW_USB_GetFifoselAdr
 Description     : Returns the *address* of the FIFOSEL register corresponding to 
 : specified PIPEMODE.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
static void *HW_USB_GetFifoselAdr (usb_utr_t *ptr, uint16_t pipemode)
{
    void *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        switch (pipemode)
        {
            case USB_CUSE:
    #if USB_CFG_USE_USBIP == USB_CFG_IP1
            reg_p = (void *)&(USB_M1.CFIFOSEL);
    #else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            reg_p = (void *)&(USB_M0.CFIFOSEL);
    #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;

            case    USB_D0USE:
            case    USB_D0DMA:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
                reg_p = (void *)&(USB_M1.D0FIFOSEL);
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                reg_p = (void *)&(USB_M0.D0FIFOSEL);
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                break;
            case    USB_D1USE:
            case    USB_D1DMA:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
                reg_p = (void *)&(USB_M1.D1FIFOSEL);
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                reg_p = (void *)&(USB_M0.D1FIFOSEL);
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */

            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE12);
            break;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        switch (pipemode)
        {
            case USB_CUSE :
                reg_p = (void *) &(ptr->ipp->CFIFOSEL);
            break;
            case USB_D0USE :
            case    USB_D0DMA:
                reg_p = (void *) &(ptr->ipp->D0FIFOSEL);
            break;
            case USB_D1USE :
            case    USB_D1DMA:
                reg_p = (void *) &(ptr->ipp->D1FIFOSEL);
            break;
            default :
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE12);
            break;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return reg_p;
} /* eof HW_USB_GetFifoselAdr() */

/******************************************************************************
 Function Name   : HW_USB_ReadFifosel
 Description     : Returns the value of the specified pipemode's FIFOSEL register.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : FIFOSEL content
 ******************************************************************************/
uint16_t HW_USB_ReadFifosel (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *reg_p;

    reg_p = (uint16_t *) HW_USB_GetFifoselAdr(ptr, pipemode);

    return *reg_p;
} /* eof HW_USB_ReadFifosel() */

/******************************************************************************
 Function Name   : HW_USB_RmwFifosel
 Description     : Data is written to the specified pipemode's FIFOSEL register 
 : (the FIFOSEL corresponding to the specified PIPEMODE), using 
 : read-modify-write.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 : uint16_t data     : The value to write.
 : uint16_t bitptn   : bitptn: Bit pattern to read-modify-write.
 Return value    : none
 ******************************************************************************/
void HW_USB_RmwFifosel (usb_utr_t *ptr, uint16_t pipemode, uint16_t data, uint16_t bitptn)
{
    uint16_t buf;
    volatile uint16_t *reg_p;

    reg_p = (uint16_t *) HW_USB_GetFifoselAdr(ptr, pipemode);

    buf = *reg_p;
    buf &= (~bitptn);
    buf |= (data & bitptn);
    *reg_p = buf;
} /* eof HW_USB_RmwFifosel() */

/******************************************************************************
 Function Name   : HW_USB_SetDclrm
 Description     : Set DCLRM-bits (FIFO buffer auto clear) of the FIFOSEL cor-
 : responding to specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetDclrm (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *reg_p;

    reg_p = (uint16_t *) HW_USB_GetFifoselAdr(ptr, pipemode);

    (*reg_p) |= USB_DCLRM;
} /* eof HW_USB_SetDclrm() */

/******************************************************************************
 Function Name   : HW_USB_ClearDclrm
 Description     : Reset DCLRM-bits (FIFO buffer not auto-cleared) of the FIFOSEL 
 : corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearDclrm (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *reg_p;

    reg_p = HW_USB_GetFifoselAdr(ptr, pipemode);

    (*reg_p) &= (~USB_DCLRM);
} /* eof HW_USB_ClearDclrm() */

/******************************************************************************
 Function Name   : HW_USB_SetDreqe
 Description     : Set DREQE-bits (to output signal DxREQ_Na) of the FIFOSEL cor-
 : responding to specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetDreqe (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *reg_p;

    reg_p = HW_USB_GetFifoselAdr(ptr, pipemode);

    (*reg_p) &= (~USB_DREQE);
    (*reg_p) |= USB_DREQE;
} /* eof HW_USB_SetDreqe() */

/******************************************************************************
 Function Name   : HW_USB_ClearDreqe
 Description     : Clear DREQE-bits (To prohibit the output of the signal DxREQ_N)
 : of the FIFOSEL corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearDreqe (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *reg_p;

    reg_p = HW_USB_GetFifoselAdr(ptr, pipemode);

    (*reg_p) &= (~USB_DREQE);
} /* eof HW_USB_ClearDreqe() */

/******************************************************************************
 Function Name   : HW_USB_SetMbw
 Description     : Set MBW-bits (CFIFO Port Access Bit Width) of the FIFOSEL cor-
 : responding to the specified PIPEMODE, to select 8 or 16-bit 
 : wide FIFO port access.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 : uint16_t data     : Defined value of 8 (data = 0x0000) or 16 bit 
 : (data = 0x0400), 32 bit (data = 0x0800) access mode.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetMbw (usb_utr_t *ptr, uint16_t pipemode, uint16_t data)
{
    volatile uint16_t *reg_p;

    reg_p = HW_USB_GetFifoselAdr(ptr, pipemode);

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        (*reg_p) &= (~USB_MBW);
        if (data != 0)
        {
            (*reg_p) |= data;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (ptr->ip == USB_USBIP_0)
        {
            if (data != 0)
            {
                (*reg_p) |= USB_MBW_16;
            }
            else
            {
                (*reg_p) &= (~USB_MBW_16);
            }
        }
        else if (ptr->ip == USB_USBIP_1)
        {
            (*reg_p) &= (~USB_MBW);

            if (data != 0)
            {
                (*reg_p) |= data;
            }
        }
        else
        {
            /* Non */
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetMbw() */

/******************************************************************************
 Function Name   : HW_USB_SetCurpipe
 Description     : Set pipe to the number given; in the FIFOSEL corresponding 
 : to specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 : uint16_t pipeno   : Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetCurpipe (usb_utr_t *ptr, uint16_t pipemode, uint16_t pipeno)
{
    volatile uint16_t *reg_p;
    uint16_t reg;

    reg_p = HW_USB_GetFifoselAdr(ptr, pipemode);
    reg = *reg_p;

    reg &= (~USB_CURPIPE);
    reg |= (pipeno ^ 0x01);     /* Dummy change */
    *reg_p = reg;

    reg &= (~USB_CURPIPE);
    reg |= pipeno;

    *reg_p = reg;
} /* eof HW_USB_SetCurpipe() */

/******************************************************************************
 Function Name   : HW_USB_get_fifoctr_adr
 Description     : Returns the *address* of the FIFOCTR register corresponding to 
 : specified PIPEMODE.
 : (FIFO Port Control Register.)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 Return value    : none
 ******************************************************************************/
static void *HW_USB_get_fifoctr_adr (usb_utr_t *ptr, uint16_t pipemode)
{
    void *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        switch (pipemode)
        {
            case USB_CUSE:
    #if USB_CFG_USE_USBIP == USB_CFG_IP1
            reg_p = (void *)&(USB_M1.CFIFOCTR);
    #else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            reg_p = (void *)&(USB_M0.CFIFOCTR);
    #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;

            case    USB_D0USE:
	        case    USB_D0DMA:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
                reg_p = (void *)&(USB_M1.D0FIFOCTR);
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                reg_p = (void *)&(USB_M0.D0FIFOCTR);
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                break;
            case    USB_D1USE:
	        case    USB_D1DMA:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
                reg_p = (void *)&(USB_M1.D1FIFOCTR);
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                reg_p = (void *)&(USB_M0.D1FIFOCTR);
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */

            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE13);
            break;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        switch (pipemode)
        {
            case USB_CUSE :
                reg_p = (void *) &(ptr->ipp->CFIFOCTR);
            break;
            case USB_D0USE :
	        case USB_D0DMA:
                reg_p = (void *) &(ptr->ipp->D0FIFOCTR);
            break;
            case USB_D1USE :
	        case USB_D1DMA:
                reg_p = (void *) &(ptr->ipp->D1FIFOCTR);
            break;
            default :
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE13);
            break;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return reg_p;
} /* eof HW_USB_get_fifoctr_adr() */

/******************************************************************************
 Function Name   : HW_USB_ReadFifoctr
 Description     : Returns the value of the FIFOCTR register corresponding to 
 : specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 Return value    : FIFOCTR content
 ******************************************************************************/
uint16_t HW_USB_ReadFifoctr (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *reg_p;

    reg_p = (uint16_t *) HW_USB_get_fifoctr_adr(ptr, pipemode);

    return *reg_p;
} /* eof HW_USB_ReadFifoctr() */

/******************************************************************************
 Function Name   : HW_USB_SetBval
 Description     : Set BVAL (Buffer Memory Valid Flag) to the number given; in 
 : the FIFOCTR corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetBval (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *reg_p;

    reg_p = (uint16_t *) HW_USB_get_fifoctr_adr(ptr, pipemode);

    (*reg_p) |= USB_BVAL;
} /* eof HW_USB_SetBval() */

/******************************************************************************
 Function Name   : HW_USB_SetBclr
 Description     : Set BCLR (CPU Buffer Clear) to the number given; in the 
 : FIFOCTR corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : USB internal structure. Selects USB channel.
 : uint16_t pipemode : CUSE/D0DMA/D1DMA.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetBclr (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *reg_p;

    reg_p = (uint16_t *) HW_USB_get_fifoctr_adr(ptr, pipemode);

    *reg_p = USB_BCLR;
} /* eof HW_USB_SetBclr() */

/******************************************************************************
 Function Name   : HW_USB_WriteIntenb
 Description     : Data is written to INTENB register, 
 : enabling/disabling the various USB interrupts.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteIntenb (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.INTENB0.WORD = data;
    #else
        USB_M1.INTENB0.WORD = data;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->INTENB0.WORD = data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_WriteIntenb() */

/******************************************************************************
 Function Name   : HW_USB_SetIntenb
 Description     : Bit(s) to be set in INTENB register, 
 : enabling the respective USB interrupt(s).
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : Bit pattern: Respective interrupts with '1' 
 : will be enabled.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetIntenb (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.INTENB0.WORD |= data;
    #else
        USB_M1.INTENB0.WORD |= data;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->INTENB0.WORD |= data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetIntenb() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_ClearEnbVbse
 Description     : Clear the VBE-bit of INTENB register,
 : to prohibit VBUS interrupts.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearEnbVbse (usb_utr_t *ptr)
{
    ptr->ipp->INTENB0.WORD &= (~USB_VBSE);
} /* eof HW_USB_ClearEnbVbse() */

/******************************************************************************
 Description     : Clear the SOFE-bit of INTENB register,
 : to prohibit Frame Number Update interrupts.
 Function Name   : HW_USB_ClearEnbSofe
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearEnbSofe (usb_utr_t *ptr)
{
    ptr->ipp->INTENB0.WORD &= (~USB_SOFE);
} /* eof HW_USB_ClearEnbSofe() */

/******************************************************************************
 Function Name   : HW_USB_WriteBrdyenb
 Description     : Data is written to BRDYENB register, 
 : enabling/disabling each respective pipe's BRDY interrupt. 
 : (The BRDY interrupt indicates that a FIFO port is accessible.)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data        : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteBrdyenb (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->BRDYENB.WORD = data;
} /* eof HW_USB_WriteBrdyenb() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_SetBrdyenb
 Description     : A bit is set in the specified pipe's BRDYENB, enabling the 
 : respective pipe BRDY interrupt(s).
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetBrdyenb (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.BRDYENB.WORD |= (1 << pipeno);
    #else
        USB_M1.BRDYENB.WORD |= (1 << pipeno);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->BRDYENB.WORD |= (1 << pipeno);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetBrdyenb() */

/******************************************************************************
 Function Name   : HW_USB_ClearBrdyenb
 Description     : Clear the PIPExBRDYE-bit of the specified pipe to prohibit 
 : BRDY interrupts of that pipe.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearBrdyenb (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.BRDYENB.WORD &= (~(1 << pipeno));
    #else
        USB_M1.BRDYENB.WORD &= (~(1 << pipeno));
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->BRDYENB.WORD &= (~(1 << pipeno));
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_ClearBrdyenb() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_WriteNrdyenb
 Description     : Data is written to NRDYENB register, 
 : enabling/disabling each respective pipe's NRDY interrupt
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteNrdyenb (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->NRDYENB.WORD = data;
} /* eof HW_USB_WriteNrdyenb() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_SetNrdyenb
 Description     : A bit is set in the specified pipe's NRDYENB, enabling the 
 : respective pipe NRDY interrupt(s).
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetNrdyenb (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.NRDYENB.WORD |= (1 << pipeno);
    #else
        USB_M1.NRDYENB.WORD |= (1 << pipeno);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->NRDYENB.WORD |= (1 << pipeno);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetNrdyenb() */

/******************************************************************************
 Function Name   : HW_USB_ClearNrdyenb
 Description     : Clear the PIPExNRDYE-bit of the specified pipe to prohibit 
 : NRDY interrupts of that pipe.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearNrdyenb (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.NRDYENB.WORD |= (1 << pipeno);
    #else
        USB_M1.NRDYENB.WORD |= (1 << pipeno);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->NRDYENB.WORD &= (~(1 << pipeno));
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_ClearNrdyenb() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_WriteBempenb
 Description     : Data is written to BEMPENB register, 
 : enabling/disabling each respective pipe's BEMP interrupt. 
 : (The BEMP interrupt indicates that the USB buffer is empty, 
 : and so the FIFO can now be written to.)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteBempenb (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->BEMPENB.WORD = data;
} /* eof HW_USB_WriteBempenb() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_SetBempenb
 Description     : A bit is set in the specified pipe's BEMPENB enabling the 
 : respective pipe's BEMP interrupt(s).
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetBempenb (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.BEMPENB.WORD |= (1 << pipeno);
    #else
        USB_M1.BEMPENB.WORD |= (1 << pipeno);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->BEMPENB.WORD |= (1 << pipeno);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetBempenb() */

/******************************************************************************
 Function Name   : HW_USB_ClearBempenb
 Description     : Clear the PIPExBEMPE-bit of the specified pipe to prohibit 
 : BEMP interrupts of that pipe.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearBempenb (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.BEMPENB.WORD &= (~(1 << pipeno));
    #else
        USB_M1.BEMPENB.WORD &= (~(1 << pipeno));
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->BEMPENB.WORD &= (~(1 << pipeno));
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_ClearBempenb() */

/******************************************************************************
 Function Name   : HW_USB_SetSofcfg
 Description     : Set Bit pattern for SOFCFG
 : 
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to OR.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetSofcfg (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    #if USB_NUM_USBIP == 2
        if(ptr->ip == USB_USBIP_1)
        {
            ptr->ipp1->SOFCFG.WORD |= data;
        }
    #endif /* USB_NUM_USBIP == 2 */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetSofcfg() */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : HW_USB_ReadIntsts
 Description     : Returns INTSTS0 register content.
 Arguments       :
 Return value    : INTSTS0 content
 ******************************************************************************/
uint16_t HW_USB_ReadIntsts(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    return USB_M0.INTSTS0.WORD;
    #else
    return USB_M1.INTSTS0.WORD;
    #endif
} /* eof HW_USB_ReadIntsts() */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/*#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)*/
/******************************************************************************
 Function Name   : HW_USB_WriteIntsts
 Description     : Data is written to INTSTS0 register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteIntsts (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.INTSTS0.WORD = data;
    #else
        USB_M1.INTSTS0.WORD = data;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->INTSTS0.WORD = data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_WriteIntsts() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_ClearStsSofr
 Description     : Clear the SOFR-bit (Frame Number Refresh Interrupt Status) of 
 : the clear SOF interrupt status.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearStsSofr (usb_utr_t *ptr)
{
    ptr->ipp->INTSTS0.WORD = (uint16_t) ~USB_SOFR;
} /* eof HW_USB_ClearStsSofr() */

/******************************************************************************
 Function Name   : HW_USB_Readbrdysts
 Description     : Returns BRDYSTS register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : BRDYSTS content
 ******************************************************************************/
uint16_t HW_USB_Readbrdysts (usb_utr_t *ptr)
{
    return ptr->ipp->BRDYSTS.WORD;
} /* eof HW_USB_Readbrdysts() */

/******************************************************************************
 Function Name   : HW_USB_WriteBrdysts
 Description     : Data is written to BRDYSTS register, to set the BRDY interrupt status.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteBrdysts (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->BRDYSTS.WORD = data;
} /* eof HW_USB_WriteBrdysts() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_ClearStsBrdy
 Description     : Clear the PIPExBRDY status bit of the specified pipe to clear 
 : its BRDY interrupt status.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearStsBrdy (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.BRDYSTS.WORD = (uint16_t) (~(1 << pipeno) & BRDYSTS_MASK);
    #else
        USB_M1.BRDYSTS.WORD = (uint16_t) (~(1 << pipeno) & BRDYSTS_MASK);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->BRDYSTS.WORD = (uint16_t) (~(1 << pipeno) & BRDYSTS_MASK);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_ClearStsBrdy() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_WriteNrdysts
 Description     : Data is written to NRDYSTS register, to
 : set the NRDY interrupt status.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteNrdysts (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->NRDYSTS.WORD = data;
} /* eof HW_USB_WriteNrdysts() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_ClearStatusNrdy
 Description     : Clear the PIPExNRDY status bit of the specified pipe to clear 
 : its NRDY interrupt status.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearStatusNrdy (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.NRDYSTS.WORD = (uint16_t) (~(1 << pipeno) & NRDYSTS_MASK);
    #else
        USB_M1.NRDYSTS.WORD = (uint16_t) (~(1 << pipeno) & NRDYSTS_MASK);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->NRDYSTS.WORD = (uint16_t) (~(1 << pipeno) & NRDYSTS_MASK);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_ClearStatusNrdy() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_WriteBempsts
 Description     : Data is written to BEMPSTS register, to set the BEMP interrupt status.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data        : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteBempsts (usb_utr_t *ptr, uint16_t data)
{
    ptr->ipp->BEMPSTS.WORD = data;
} /* eof HW_USB_WriteBempsts() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_ClearStatusBemp
 Description     : Clear the PIPExBEMP status bit of the specified pipe to clear 
 : its BEMP interrupt status.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearStatusBemp (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.BEMPSTS.WORD = (uint16_t) (~(1 << pipeno) & BEMPSTS_MASK);
    #else
        USB_M1.BEMPSTS.WORD = (uint16_t) (~(1 << pipeno) & BEMPSTS_MASK);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->BEMPSTS.WORD = (uint16_t) (~(1 << pipeno) & BEMPSTS_MASK);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_ClearStatusBemp() */

/******************************************************************************
 Function Name   : HW_USB_ReadFrmnum
 Description     : Returns FRMNUM register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : FRMNUM content
 ******************************************************************************/
uint16_t HW_USB_ReadFrmnum (usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return (uint16_t)USB_M0.FRMNUM.WORD;
    #else
        return (uint16_t)USB_M1.FRMNUM.WORD;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        return (uint16_t) ptr->ipp->FRMNUM.WORD;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadFrmnum() */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : HW_USB_ReadUsbaddr
 Description     : Returns USBADDR register content.
 Arguments       : none
 Return value    : USBADDR content
 ******************************************************************************/
uint16_t HW_USB_ReadUsbaddr(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBADDR.WORD;
    #else
    return (uint16_t)USB_M1.USBADDR.WORD;
    #endif
} /* eof HW_USB_ReadUsbaddr() */

/******************************************************************************
 Function Name   : HW_USB_ReadUsbreq
 Description     : Returns USBREQ register content.
 Arguments       : none
 Return value    : USBREQ content
 ******************************************************************************/
uint16_t HW_USB_ReadUsbreq(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBREQ.WORD;
    #else
    return (uint16_t)USB_M1.USBREQ.WORD;
    #endif
} /* eof HW_USB_ReadUsbreq() */

/******************************************************************************
 Function Name   : HW_USB_ReadUsbval
 Description     : Returns USBVAL register content.
 Arguments       : none
 Return value    : USBVAL content
 ******************************************************************************/
uint16_t HW_USB_ReadUsbval(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBVAL;
    #else
    return (uint16_t)USB_M1.USBVAL;
    #endif
} /* eof HW_USB_ReadUsbval() */

/******************************************************************************
 Function Name   : HW_USB_ReadUsbindx
 Description     : Returns USBINDX register content.
 Arguments       : none
 Return value    : USBINDX content
 ******************************************************************************/
uint16_t HW_USB_ReadUsbindx(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBINDX;
    #else
    return (uint16_t)USB_M1.USBINDX;
    #endif
} /* eof HW_USB_ReadUsbindx() */

/******************************************************************************
 Function Name   : HW_USB_ReadUsbleng
 Description     : Returns USBLENG register content.
 Arguments       : none
 Return value    : USBLENG content
 ******************************************************************************/
uint16_t HW_USB_ReadUsbleng(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBLENG;
    #else
    return (uint16_t)USB_M1.USBLENG;
    #endif
} /* eof HW_USB_ReadUsbleng() */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 Function Name   : HW_USB_ReadDcpcfg
 Description     : Returns DCPCFG register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : DCPCFG content
 ******************************************************************************/
uint16_t HW_USB_ReadDcpcfg (usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return (uint16_t)USB_M0.DCPCFG.WORD;
    #else
        return (uint16_t)USB_M1.DCPCFG.WORD;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        return (uint16_t) ptr->ipp->DCPCFG.WORD;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadDcpcfg() */

/******************************************************************************
 Function Name   : HW_USB_WriteDcpcfg
 Description     : Specified data is written to DCPCFG register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteDcpcfg (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.DCPCFG.WORD = data;
    #else
        USB_M1.DCPCFG.WORD = data;
    #endif

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->DCPCFG.WORD = data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_WriteDcpcfg()*/

/******************************************************************************
 Function Name   : HW_USB_ReadDcpmaxp
 Description     : Returns DCPMAXP register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : DCPMAXP content
 ******************************************************************************/
uint16_t HW_USB_ReadDcpmaxp (usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return (uint16_t)USB_M0.DCPMAXP.WORD;
    #else
        return (uint16_t)USB_M1.DCPMAXP.WORD;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        return (uint16_t) ptr->ipp->DCPMAXP.WORD;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadDcpmaxp() */

/******************************************************************************
 Function Name   : HW_USB_WriteDcpmxps
 Description     : Specified data is written to DCPMAXP register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WriteDcpmxps (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.DCPMAXP.WORD = data;
    #else
        USB_M1.DCPMAXP.WORD = data;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->DCPMAXP.WORD = data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_WriteDcpmxps() */

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : HW_USB_ReadDcpctr
 Description     : Returns DCPCTR register content.
 Arguments       : none
 Return value    : DCPCTR content
 ******************************************************************************/
uint16_t HW_USB_ReadDcpctr(void)
{
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.DCPCTR.WORD;
    #else
    return (uint16_t)USB_M1.DCPCTR.WORD;
    #endif
} /* eof HW_USB_ReadDcpctr() */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 Function Name   : HW_USB_WritePipesel
 Description     : Specified data is written to PIPESEL register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WritePipesel (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.PIPESEL.WORD = data;
    #else
        USB_M1.PIPESEL.WORD = data;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->PIPESEL.WORD = data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_WritePipesel() */

/******************************************************************************
 Function Name   : HW_USB_ReadPipecfg
 Description     : Returns PIPECFG register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : PIPECFG content
 ******************************************************************************/
uint16_t HW_USB_ReadPipecfg (usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return (uint16_t)USB_M0.PIPECFG.WORD;
    #else
        return (uint16_t)USB_M1.PIPECFG.WORD;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        return (uint16_t) ptr->ipp->PIPECFG.WORD;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadPipecfg() */

/******************************************************************************
 Function Name   : HW_USB_WritePipecfg
 Description     : Specified data is written to PIPECFG register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WritePipecfg (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.PIPECFG.WORD = data;
    #else
        USB_M1.PIPECFG.WORD = data;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->PIPECFG.WORD = data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_WritePipecfg() */

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/******************************************************************************
 Function Name   : HW_USB_WritePipebuf
 Description     : Specified the value by 2nd argument is set to PIPEBUF register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WritePipebuf(usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        #if USB_CFG_USE_USBIP == USB_CFG_IP0

        #else
        USB_M1.PIPEBUF.WORD = data;
        #endif
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        #if USB_NUM_USBIP == 2
        if(ptr->ip == USB_USBIP_1)
        {
            ptr->ipp1->PIPEBUF.WORD = data;
        }
        #endif /* USB_NUM_USBIP == 2 */
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

} /* eof HW_USB_WritePipebuf() */

/******************************************************************************
 Function Name   : HW_USB_ReadPipebuf
 Description     : Returns PIPECFG register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : PIPEBUF content
 ******************************************************************************/
uint16_t HW_USB_ReadPipebuf(usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return 0;
        #else
        return (uint16_t)USB_M1.PIPEBUF.WORD;
        #endif
    #endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
    #if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        #if USB_NUM_USBIP == 2
        if(ptr->ip == USB_USBIP_1)
        {
            return (uint16_t)ptr->ipp1->PIPEBUF.WORD;
        }
        else
        {
            return 0;
        }
        #else  /* USB_NUM_USBIP == 2 */
        return 0;
        #endif /* USB_NUM_USBIP == 2 */
    #endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadPipebuf() */
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

/******************************************************************************
 Function Name   : HW_USB_ReadPipemaxp
 Description     : Returns PIPEMAXP register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : PIPEMAXP content
 ******************************************************************************/
uint16_t HW_USB_ReadPipemaxp (usb_utr_t *ptr)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        return (uint16_t)USB_M0.PIPEMAXP.WORD;
    #else
        return (uint16_t)USB_M1.PIPEMAXP.WORD;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        return (uint16_t) ptr->ipp->PIPEMAXP.WORD;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    return 0;
} /* eof HW_USB_ReadPipemaxp() */

/******************************************************************************
 Function Name   : HW_USB_WritePipemaxp
 Description     : Specified data is written to PIPEMAXP register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WritePipemaxp (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.PIPEMAXP.WORD = data;
    #else
        USB_M1.PIPEMAXP.WORD = data;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->PIPEMAXP.WORD = data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

} /* eof HW_USB_WritePipemaxp() */

/******************************************************************************
 Function Name   : HW_USB_WritePipeperi
 Description     : Specified data is written to PIPEPERI register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WritePipeperi (usb_utr_t *ptr, uint16_t data)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        USB_M0.PIPEPERI.WORD = data;
    #else
        USB_M1.PIPEPERI.WORD = data;
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        ptr->ipp->PIPEPERI.WORD = data;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

} /* eof HW_USB_WritePipeperi() */

/******************************************************************************
 Function Name   : HW_USB_ReadPipectr
 Description     : Returns DCPCTR or the specified pipe's PIPECTR register content.
 : The Pipe Control Register returned is determined by the speci-
 : fied pipe number.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : PIPExCTR content
 ******************************************************************************/
uint16_t HW_USB_ReadPipectr (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (USB_PIPE0 == pipeno)
        {
            reg_p = (uint16_t *)&(USB_M0.DCPCTR);
        }
        else
        {
            reg_p = (uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1);
        }
    #else
        if (USB_PIPE0 == pipeno)
        {
            reg_p = (uint16_t *)&(USB_M1.DCPCTR);
        }
        else
        {
            reg_p = (uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1);
        }
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (USB_PIPE0 == pipeno)
        {
            reg_p = (uint16_t *) &(ptr->ipp->DCPCTR);
        }
        else
        {
            reg_p = (uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1);
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    return *reg_p;
} /* eof HW_USB_ReadPipectr() */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Function Name   : HW_USB_WritePipectr
 Description     : Specified data is written to the specified pipe's PIPEPERI register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WritePipectr (usb_utr_t *ptr, uint16_t pipeno, uint16_t data)
{
    volatile uint16_t *reg_p;

    if (USB_PIPE0 == pipeno)
    {
        reg_p = (uint16_t *) &(ptr->ipp->DCPCTR);
    }
    else
    {
        reg_p = (uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1);
    }
    *reg_p = data;
} /* eof HW_USB_WritePipectr() */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : HW_USB_SetCsclr
 Description     : Set CSCLR bit in the specified pipe's PIPECTR register
 Arguments       : usb_utr_t *ptr   : USB internal structure. Selects USB channel.
 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void HW_USB_SetCsclr (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        reg_p = (uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1);
    #else
        reg_p = (uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        reg_p = (uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    (*reg_p) |= USB_CSCLR;
} /* eof HW_USB_SetCsclr() */

/******************************************************************************
 Function Name   : HW_USB_SetAclrm
 Description     : The ACLRM-bit (Auto Buffer Clear Mode) is set in the specified 
 : pipe's control register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetAclrm (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        reg_p = (uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1);
    #else
        reg_p = (uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        reg_p = (uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    (*reg_p) |= USB_ACLRM;
} /* eof HW_USB_SetAclrm() */

/******************************************************************************
 Function Name   : HW_USB_ClearAclrm
 Description     : Clear the ACLRM bit in the specified pipe's control register
 : to disable Auto Buffer Clear Mode.
 : its BEMP interrupt status.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearAclrm (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        reg_p = (uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1);
    #else
        reg_p = (uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        reg_p = (uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    (*reg_p) &= (~USB_ACLRM);
} /* eof HW_USB_ClearAclrm() */

/******************************************************************************
 Function Name   : HW_USB_SetSqclr
 Description     : The SQCLR-bit (Toggle Bit Clear) is set in the specified pipe's 
 : control register. Setting SQSET to 1 makes DATA0 the expected 
 : data in the pipe's next transfer.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetSqclr (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (pipeno == USB_PIPE0)
        {
            USB_M0.DCPCTR.WORD |= USB_SQCLR;
        }
        else
        {
            volatile uint16_t *reg_p;
            reg_p = ((uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1));
            (*reg_p) |= USB_SQCLR;
        }
    #else
        if (pipeno == USB_PIPE0)
        {
            USB_M1.DCPCTR.WORD |= USB_SQCLR;
        }
        else
        {
            volatile uint16_t *reg_p;
            reg_p = ((uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1));
            (*reg_p) |= USB_SQCLR;
        }
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (pipeno == USB_PIPE0)
        {
            ptr->ipp->DCPCTR.WORD |= USB_SQCLR;
        }
        else
        {
            volatile uint16_t *reg_p;

            reg_p = ((uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1));
            (*reg_p) |= USB_SQCLR;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetSqclr() */

/******************************************************************************
 Function Name   : HW_USB_SetSqset
 Description     : The SQSET-bit (Toggle Bit Set) is set in the specified pipe's 
 : control register. Setting SQSET to 1 makes DATA1 the expected 
 : data in the next transfer.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetSqset (usb_utr_t *ptr, uint16_t pipeno)
{
    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (pipeno == USB_PIPE0)
        {
            USB_M0.DCPCTR.WORD |= USB_SQSET;
        }
        else
        {
            volatile uint16_t *reg_p;
            reg_p = ((uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1));
            (*reg_p) |= USB_SQSET;
        }
    #else
        if (pipeno == USB_PIPE0)
        {
            USB_M1.DCPCTR.WORD |= USB_SQSET;
        }
        else
        {
            volatile uint16_t *reg_p;
            reg_p = ((uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1));
            (*reg_p) |= USB_SQSET;
        }
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (pipeno == USB_PIPE0)
        {
            ptr->ipp->DCPCTR.WORD |= USB_SQSET;
        }
        else
        {
            volatile uint16_t *reg_p;

            reg_p = ((uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1));
            (*reg_p) |= USB_SQSET;
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
} /* eof HW_USB_SetSqset() */

/******************************************************************************
 Function Name   : HW_USB_SetPid
 Description     : Set the specified PID of the specified pipe's DCPCTR/PIPECTR 
 : register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 : uint16_t  data  : NAK/BUF/STALL.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetPid (usb_utr_t *ptr, uint16_t pipeno, uint16_t data)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (pipeno == USB_PIPE0)
        {
            reg_p = ((uint16_t *)&(USB_M0.DCPCTR));
        }
        else
        {
            reg_p = ((uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1));
        }
    #else
        if (pipeno == USB_PIPE0)
        {
            reg_p = ((uint16_t *)&(USB_M1.DCPCTR));
        }
        else
        {
            reg_p = ((uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1));
        }
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (pipeno == USB_PIPE0)
        {
            reg_p = ((uint16_t *) &(ptr->ipp->DCPCTR));
        }
        else
        {
            reg_p = ((uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1));
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    (*reg_p) &= (~USB_PID);
    (*reg_p) |= data;
} /* eof HW_USB_SetPid() */

/******************************************************************************
 Function Name   : HW_USB_ClearPid
 Description     : Clear the specified PID-bits of the specified pipe's DCPCTR/
 : PIPECTR register.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 : uint16_t  data  : NAK/BUF/STALL - to be cleared.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearPid (usb_utr_t *ptr, uint16_t pipeno, uint16_t data)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (pipeno == USB_PIPE0)
        {
            reg_p = ((uint16_t *)&(USB_M0.DCPCTR));
        }
        else
        {
            reg_p = ((uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1));
        }
    #else
        if (pipeno == USB_PIPE0)
        {
            reg_p = ((uint16_t *)&(USB_M1.DCPCTR));
        }
        else
        {
            reg_p = ((uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1));
        }
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        if (pipeno == USB_PIPE0)
        {
            reg_p = ((uint16_t *) &(ptr->ipp->DCPCTR));
        }
        else
        {
            reg_p = ((uint16_t *) &(ptr->ipp->PIPE1CTR) + (pipeno - 1));
        }
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    (*reg_p) &= (~data);
} /* eof HW_USB_ClearPid() */

/******************************************************************************
 Function Name   : HW_USB_SetTrenb
 Description     : The TRENB-bit (Transaction Counter Enable) is set in the speci-
 : fied pipe's control register, to enable the counter.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetTrenb (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        reg_p = (uint16_t *)&(USB_M0.PIPE1TRE) + ((pipeno - 1) * 2);
    #else
        reg_p = (uint16_t *)&(USB_M1.PIPE1TRE) + ((pipeno - 1) * 2);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        reg_p = (uint16_t *) &(ptr->ipp->PIPE1TRE) + ((pipeno - 1) * 2);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    (*reg_p) |= USB_TRENB;
} /* eof HW_USB_SetTrenb() */

/******************************************************************************
 Function Name   : HW_USB_ClearTrenb
 Description     : The TRENB-bit (Transaction Counter Enable) is cleared in the 
 : specified pipe's control register, to disable the counter.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearTrenb (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        reg_p = (uint16_t *)&(USB_M0.PIPE1TRE) + ((pipeno - 1) * 2);
    #else
        reg_p = (uint16_t *)&(USB_M1.PIPE1TRE) + ((pipeno - 1) * 2);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        reg_p = (uint16_t *) &(ptr->ipp->PIPE1TRE) + ((pipeno - 1) * 2);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    (*reg_p) &= (~USB_TRENB);
} /* eof HW_USB_ClearTrenb() */

/******************************************************************************
 Function Name   : HW_USB_SetTrclr
 Description     : The TRENB-bit (Transaction Counter Clear) is set in the speci-
 : fied pipe's control register to clear the current counter 
 : value.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetTrclr (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        reg_p = (uint16_t *)&(USB_M0.PIPE1TRE) + ((pipeno - 1) * 2);
    #else
        reg_p = (uint16_t *)&(USB_M1.PIPE1TRE) + ((pipeno - 1) * 2);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        reg_p = (uint16_t *) &(ptr->ipp->PIPE1TRE) + ((pipeno - 1) * 2);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    (*reg_p) |= USB_TRCLR;
} /* eof HW_USB_SetTrclr() */

/******************************************************************************
 Function Name   : HW_USB_WritePipetrn
 Description     : Specified data is written to the specified pipe's PIPETRN reg-
 : ister.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 : uint16_t  pipeno: Pipe number.
 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void HW_USB_WritePipetrn (usb_utr_t *ptr, uint16_t pipeno, uint16_t data)
{
    volatile uint16_t *reg_p;

    if (USB_NULL == ptr)
    {
#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
    #if USB_CFG_USE_USBIP == USB_CFG_IP0
        reg_p = (uint16_t *)&(USB_M0.PIPE1TRN) + ((pipeno - 1) * 2);
    #else
        reg_p = (uint16_t *)&(USB_M1.PIPE1TRN) + ((pipeno - 1) * 2);
    #endif
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
    else
    {
#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
        reg_p = (uint16_t *) &(ptr->ipp->PIPE1TRN) + ((pipeno - 1) * 2);
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }

    *reg_p = data;
} /* eof HW_USB_WritePipetrn */

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
    #if USB_CFG_BC == USB_CFG_ENABLE
/******************************************************************************
 Function Name   : HW_USB_ReadBcctrl
 Description     : Returns BCCTRL register content.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : BCCTRL content
 ******************************************************************************/
uint16_t HW_USB_ReadBcctrl(usb_utr_t *ptr)
{
        #if USB_NUM_USBIP == 2
    if(ptr->ip == USB_USBIP_1)
    {
            #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        return (uint16_t)ptr->ipp1->BCCTRL.WORD;
            #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
    else
    {
        return 0;
    }
        #else  /* USB_NUM_USBIP == 2 */
    return 0;
        #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_ReadBcctrl() */

/******************************************************************************
 Function Name   : HW_USB_SetVdmsrce
 Description     : Set VDMSRCE bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetVdmsrce(usb_utr_t *ptr)
{
        #if USB_NUM_USBIP == 2
    if(ptr->ip == USB_USBIP_1)
    {
            #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        ptr->ipp1->BCCTRL.WORD |= USB_VDMSRCE;
            #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
        #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_SetVdmsrce() */

/******************************************************************************
 Function Name   : HW_USB_ClearVdmsrce
 Description     : Clear VDMSRCE bits.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearVdmsrce(usb_utr_t *ptr)
{
        #if USB_NUM_USBIP == 2
    if(ptr->ip == USB_USBIP_1)
    {
            #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        ptr->ipp1->BCCTRL.WORD &= (~USB_VDMSRCE);
            #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
        #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_ClearVdmsrce() */

/******************************************************************************
 Function Name   : HW_USB_SetIdpsinke
 Description     : Set IDPSINKE bit.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_SetIdpsinke(usb_utr_t *ptr)
{
        #if USB_NUM_USBIP == 2
    if(ptr->ip == USB_USBIP_1)
    {
            #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        ptr->ipp1->BCCTRL.WORD |= USB_IDPSINKE;
            #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
        #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_SetIdpsinke() */

/******************************************************************************
 Function Name   : HW_USB_ClearIdpsinke
 Description     : Clear IDPSINKE bits.
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearIdpsinke(usb_utr_t *ptr)
{
        #if USB_NUM_USBIP == 2
    if(ptr->ip == USB_USBIP_1)
    {
            #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        ptr->ipp1->BCCTRL.WORD &= (~USB_IDPSINKE);
            #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    }
        #endif /* USB_NUM_USBIP == 2 */
} /* eof HW_USB_ClearIdpsinke() */
    #endif  /* USB_CFG_BC == USB_CFG_ENABLE */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    #if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Function Name   : HW_USB_SetSuspendm
 Description     : Set SUSPM bit.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void HW_USB_SetSuspendm(void)
{
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
    /* Non */
        #else
    USB_M1.LPSTS.WORD |= USB_SUSPENDM;
        #endif
} /* eof HW_USB_SetSuspendm() */

/******************************************************************************
 Function Name   : HW_USB_ClearSuspm
 Description     : Clear SUSPM bit.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void HW_USB_ClearSuspm(void)
{
        #if USB_CFG_USE_USBIP == USB_CFG_IP0
    /* Non */
        #else
    USB_M1.LPSTS.WORD &= (~USB_SUSPENDM);
        #endif
} /* eof HW_USB_ClearSuspm() */
    #endif  /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

/******************************************************************************
 End of file
 ******************************************************************************/
