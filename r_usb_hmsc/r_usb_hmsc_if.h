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
 * Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_hmsc_if.h
 * Description  : Interface file for USB HMSC API
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 04.01.2014 1.00 First Release
 *         : 30.01.2015 1.01    Added RX71M.
 ***********************************************************************************************************************/

#ifndef _USB_HMSC_IF_H
    #define _USB_HMSC_IF_H

/*****************************************************************************
 Enum definitions
 ******************************************************************************/
enum
{
    /*--- SFF-8070i command define ---*/
    USB_ATAPI_TEST_UNIT_READY = 0x00u,
    USB_ATAPI_REQUEST_SENSE = 0x03u,
    USB_ATAPI_FORMAT_UNIT = 0x04u,
    USB_ATAPI_INQUIRY = 0x12u,
    USB_ATAPI_MODE_SELECT6 = 0x15u,
    USB_ATAPI_MODE_SENSE6 = 0x1Au,
    USB_ATAPI_START_STOP_UNIT = 0x1Bu,
    USB_ATAPI_PREVENT_ALLOW = 0x1Eu,
    USB_ATAPI_READ_FORMAT_CAPACITY = 0x23u,
    USB_ATAPI_READ_CAPACITY = 0x25u,
    USB_ATAPI_READ10 = 0x28u,
    USB_ATAPI_WRITE10 = 0x2Au,
    USB_ATAPI_SEEK = 0x2Bu,
    USB_ATAPI_WRITE_AND_VERIFY = 0x2Eu,
    USB_ATAPI_VERIFY10 = 0x2Fu,
    USB_ATAPI_MODE_SELECT10 = 0x55u,
    USB_ATAPI_MODE_SENSE10 = 0x5Au,
};

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
void R_USB_HmscTask (void);
void R_USB_HmscDriverStart (void);
void R_USB_HmscClassCheck (usb_utr_t *ptr, uint16_t **table);
uint16_t R_USB_HmscRead10 (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno, uint16_t seccnt,
        uint32_t trans_byte);
uint16_t R_USB_HmscWrite10 (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno, uint16_t seccnt,
        uint32_t trans_byte);
uint16_t R_USB_HmscTestUnit (usb_utr_t *ptr, uint16_t side);
uint16_t R_USB_HmscRequestSense (usb_utr_t *ptr, uint16_t side, uint8_t *buff);
uint16_t R_USB_HmscInquiry (usb_utr_t *ptr, uint16_t side, uint8_t *buff);
uint16_t R_USB_HmscReadCapacity (usb_utr_t *ptr, uint16_t side, uint8_t *buff);
uint16_t R_USB_HmscReadFormatCapacity (usb_utr_t *ptr, uint16_t side, uint8_t *buff);
usb_er_t R_USB_HmscGetMaxUnit (usb_utr_t *ptr, uint16_t addr, uint8_t *buff, usb_cb_t complete);
usb_er_t R_USB_HmscMassStorageReset (usb_utr_t *ptr, uint16_t addr, usb_cb_t complete);
uint16_t R_USB_HmscPreventAllow (usb_utr_t *ptr, uint16_t side, uint8_t *buff);
uint16_t R_USB_HmscModeSelect10 (usb_utr_t *ptr, uint16_t side, uint8_t *buff);
uint16_t R_USB_HmscModeSelect6 (usb_utr_t *ptr, uint16_t side, uint8_t *buff);
uint16_t R_USB_HmscGetDevSts (uint16_t side);
uint16_t R_USB_HmscAllocDrvno (uint16_t devadr);
uint16_t R_USB_HmscFreeDrvno (uint16_t side);
uint16_t R_USB_HmscRefDrvno (uint16_t devadr);

void R_USB_HmscStrgDriveTask (void);
uint16_t R_USB_HmscStrgDriveSearch (usb_utr_t *ptr, uint16_t addr, usb_cb_t complete);
uint16_t R_USB_HmscStrgDriveOpen (usb_utr_t *ptr, uint16_t addr, uint16_t *side);
uint16_t R_USB_HmscStrgDriveClose (usb_utr_t *ptr, uint16_t addr);
uint16_t R_USB_HmscStrgReadSector (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno, uint16_t seccnt,
        uint32_t trans_byte);
uint16_t R_USB_HmscStrgWriteSector (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno, uint16_t seccnt,
        uint32_t trans_byte);
uint16_t R_USB_HmscStrgUserCommand (usb_utr_t *ptr, uint16_t side, uint16_t command, uint8_t *buff, usb_cb_t complete);
uint16_t R_USB_HmscStrgCheckEnd (usb_utr_t *ptr);

#endif /* _USB_HMSC_IF_H */
