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
 * File Name    : r_usb_hstorage_driver.c
 * Description  : USB Host Storage Driver API
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 04.01.2014 1.00 First Release
 *         : 30.01.2015 1.01    Support Multi device.
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_if.h"
#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_hmsc_if.h"
#include "r_usb_extern.h"
#include "r_usb_hmsc.h"

/******************************************************************************
 External variables and functions
 ******************************************************************************/
extern uint8_t drive_search_que[];
extern uint8_t drive_search_que_cnt;


/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static usb_cb_t usb_shmsc_command_result[USB_NUM_USBIP];
static uint16_t usb_shmsc_command_end_flag[USB_NUM_USBIP];
uint16_t g_usb_hmsc_root_devaddr[USB_NUM_USBIP];

/******************************************************************************
 Renesas Storage Driver API functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : R_USB_HmscStrgDriveTask
 Description     : Storage drive task
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void R_USB_HmscStrgDriveTask (void)
{
    usb_utr_t *mess;
    usb_er_t err;

    /* receive message */
    err = R_USB_RCV_MSG(USB_HSTRG_MBX, (usb_msg_t** )&mess);
    if (USB_OK != err)
    {
        return;
    }

    switch (mess->msginfo)
    {
        case USB_MSG_HMSC_STRG_DRIVE_SEARCH :

            /* Start Drive search */
            usb_hmsc_strg_drive_search_act(mess);
        break;

        case USB_MSG_HMSC_STRG_DRIVE_SEARCH_END :
            g_usb_hmsc_strg_process[mess->ip] = USB_NULL;
            (usb_shmsc_command_result[mess->ip])((usb_utr_t *) mess, g_usb_hmsc_root_devaddr[mess->ip], 0);
        break;

        case USB_MSG_HMSC_STRG_USER_COMMAND :

            /* R_USB_HmscStrgUserCommand Result */
            (usb_shmsc_command_result[mess->ip])(mess, 0, 0);
        break;

        case USB_MSG_HMSC_STRG_RW_END :
            if (mess->result == USB_HMSC_OK)
            {
                usb_shmsc_command_end_flag[mess->ip] = USB_TRUE;
            }
            else
            {
                usb_shmsc_command_end_flag[mess->ip] = USB_ERROR;
            }
        break;

        default :
        break;
    }

    err = R_USB_REL_BLK(USB_HSTRG_MPL, (usb_mh_t )mess);
    if (USB_OK != err)
    {
        USB_PRINTF0("### USB Strg Task rel_blk error\n");
    }
} /* End of function R_USB_HmscStrgDriveTask() */

/******************************************************************************
 Function Name   : R_USB_HmscStrgDriveSearch
 Description     : Searches drive SndMsg
 Arguments       : uint16_t addr        : Device Address
 Return value    : uint16_t             : Status
 ******************************************************************************/
uint16_t R_USB_HmscStrgDriveSearch (usb_utr_t *ptr, uint16_t addr, usb_cb_t complete)
{
    usb_utr_t *p_blf;
    usb_er_t err;

    g_usb_hmsc_root_devaddr[ptr->ip] = addr;
    usb_shmsc_command_result[ptr->ip] = complete;
    g_usb_hmsc_strg_drive_search_seq[ptr->ip] = USB_SEQ_0;

    /* Get mem pool blk */
    if ( R_USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_OK)
    {
        p_blf->msginfo = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        p_blf->keyword = addr;
        p_blf->complete = complete;
        p_blf->ip = ptr->ip;
        p_blf->ipp = ptr->ipp;

        /* Send message */
        err = R_USB_SND_MSG(USB_HSTRG_MBX, (usb_msg_t* )p_blf);
        if (USB_OK != err)
        {
            err = R_USB_REL_BLK(USB_HSTRG_MPL, (usb_mh_t )p_blf);
            USB_PRINTF0("### StrgDriveSearch function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### StrgDriveSearch function pget_blk error\n");
    }
    return (err);
} /* End of function R_USB_HmscStrgDriveSearch() */

/******************************************************************************
 Function Name   : R_USB_HmscStrgDriveOpen
 Description     : Releases drive
 Arguments       : uint16_t addr    : Device Address
 uint16_t *side   : Side pointer
 Return value    : uint16_t         : [OK/ERROR]
 ******************************************************************************/
uint16_t R_USB_HmscStrgDriveOpen (usb_utr_t *ptr, uint16_t addr, uint16_t *side)
{
    uint16_t retval;
    uint16_t ip_addr;

    ip_addr = addr;
    if ( USB_IP1 == ptr->ip)
    {
        ip_addr |= USBA_ADDRESS_OFFSET;
    }
    *side = R_USB_HmscAllocDrvno(ip_addr);

    if (USB_ERROR == (*side))
    {
        return USB_ERROR;
    }

    /* Set SubClass */
    g_usb_hmsc_sub_class[ptr->ip][*side] = g_usb_hmsc_tmp_sub_class[ptr->ip];

    /* Pipe Information table set */
    retval = usb_hmsc_smp_pipe_info(ptr, g_usb_hmsc_interface_table[ptr->ip], *side, g_usb_hmsc_speed[ptr->ip],
            (uint16_t) g_usb_hmsc_config_table[ptr->ip][2]);
    if (USB_ERROR == retval)
    {
        USB_PRINTF0("### Device information error !\n");
    }

    g_usb_hmsc_tmp_ep_tbl[ptr->ip][*side][0][3] |= (uint16_t) (addr << USB_DEVADDRBIT);
    g_usb_hmsc_tmp_ep_tbl[ptr->ip][*side][1][3] |= (uint16_t) (addr << USB_DEVADDRBIT);

    drive_search_que[drive_search_que_cnt] = ip_addr;
    drive_search_que_cnt++;

    return USB_OK;
} /* End of function R_USB_HmscStrgDriveOpen() */

/******************************************************************************
 Function Name   : R_USB_HmscStrgDriveClose
 Description     : Releases drive
 Arguments       : uint16_t side    : Side
 Return value    : uint16_t         : [OK/ERROR]
 ******************************************************************************/
uint16_t R_USB_HmscStrgDriveClose (usb_utr_t *ptr, uint16_t side)
{
    uint16_t result;

    /* Clear Pipe */
    g_usb_hmsc_out_pipe[ptr->ip][side] = USB_NOPORT; /* Pipe initial */
    g_usb_hmsc_in_pipe[ptr->ip][side] = USB_NOPORT; /* Pipe initial */
    g_usb_hmsc_out_pipectr[ptr->ip][side] = 0; /* Toggle clear */
    g_usb_hmsc_in_pipectr[ptr->ip][side] = 0; /* Toggle clear */

    g_usb_hmsc_tmp_ep_tbl[ptr->ip][side][0][1] &= (USB_BFREON | USB_CFG_DBLBON | USB_CFG_CNTMDON | USB_CFG_SHTNAKON); /* PIPECFG */
    g_usb_hmsc_tmp_ep_tbl[ptr->ip][side][0][3] = USB_NULL; /* PIPEPERI */
    g_usb_hmsc_tmp_ep_tbl[ptr->ip][side][0][4] = USB_NULL; /* PIPEPERI */
    g_usb_hmsc_tmp_ep_tbl[ptr->ip][side][1][1] &= (USB_BFREON | USB_CFG_DBLBON | USB_CFG_CNTMDON | USB_CFG_SHTNAKON); /* PIPECFG */
    g_usb_hmsc_tmp_ep_tbl[ptr->ip][side][1][3] = USB_NULL; /* PIPEPERI */
    g_usb_hmsc_tmp_ep_tbl[ptr->ip][side][1][4] = USB_NULL; /* PIPEPERI */

    result = R_USB_HmscFreeDrvno(side);

    return (result);
} /* End of function R_USB_HmscStrgDriveClose() */

/******************************************************************************
 Function Name   : R_USB_HmscStrgReadSector
 Description     : Releases drive
 Arguments       : uint16_t side        : Side
 : uint8_t  *buff       : Buffer address
 : uint32_t secno       : Sector number
 : uint16_t seccnt      : Sector count
 : uint32_t trans_byte  : Trans byte
 Return value    : uint16_t             : [DONE/ERROR]
 ******************************************************************************/
uint16_t R_USB_HmscStrgReadSector (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno, uint16_t seccnt,
        uint32_t trans_byte)
{
    uint16_t result;

    /* Device Status */
    if ( USB_TRUE != R_USB_HmscGetDevSts(side))
    {
        USB_PRINTF1("### device det(R_USB_HmscStrgReadSector:side=%d)\n", side);
        return (USB_ERROR);
    }
    g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_RW_END;
    usb_shmsc_command_end_flag[ptr->ip] = USB_FALSE;
    result = R_USB_HmscRead10(ptr, side, buff, secno, seccnt, trans_byte);

    return (result);
} /* End of function R_USB_HmscStrgReadSector() */

/******************************************************************************
 Function Name   : R_USB_HmscStrgWriteSector
 Description     : Writes sector information
 Arguments       : uint16_t side        : Side
 : uint8_t  *buff       : Buffer address
 : uint32_t secno       : Sector number
 : uint16_t seccnt      : Sector count
 : uint32_t trans_byte  : Trans byte
 Return value    : uint16_t             : [DONE/ERROR]
 ******************************************************************************/
uint16_t R_USB_HmscStrgWriteSector (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno, uint16_t seccnt,
        uint32_t trans_byte)
{
    uint16_t result;

    /* Device Status */
    if ( USB_TRUE != R_USB_HmscGetDevSts(side))
    {
        USB_PRINTF1("### device det(R_USB_HmscStrgWriteSector:side=%d)\n", side);
        return (USB_ERROR);
    }
    g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_RW_END;
    usb_shmsc_command_end_flag[ptr->ip] = USB_FALSE;
    result = R_USB_HmscWrite10(ptr, side, buff, secno, seccnt, trans_byte);

    return (result);
} /* End of function R_USB_HmscStrgWriteSector() */

/******************************************************************************
 Function Name   : R_USB_HmscStrgCheckEnd
 Description     : Check Read/Write end
 Arguments       : void
 Return value    : uint16_t             : [USB_TRUE/USB_FALSE/USB_ERROR]
 ******************************************************************************/
uint16_t R_USB_HmscStrgCheckEnd (usb_utr_t *ptr)
{
    uint16_t end_flag;

    end_flag = usb_shmsc_command_end_flag[ptr->ip];
    usb_shmsc_command_end_flag[ptr->ip] = USB_FALSE;

    return end_flag;
} /* End of function R_USB_HmscStrgCheckEnd() */

/******************************************************************************
 Function Name   : R_USB_HmscStrgUserCommand
 Description     : USB Mass Storage Command
 Arguments       : uint16_t side        : Side
 : uint16_t command     : Command
 : uint8_t  *buff       : Buffer address
 : usb_cb_t complete    : callback info
 Return value    : uint16_t             : [DONE/ERROR]
 ******************************************************************************/
uint16_t R_USB_HmscStrgUserCommand (usb_utr_t *ptr, uint16_t side, uint16_t command, uint8_t *buff, usb_cb_t complete)
{
    uint16_t result;

    /* Device Status */
    if ( USB_TRUE != R_USB_HmscGetDevSts(side))
    {
        USB_PRINTF1("### device det(StrgUserCommand:side=%d)\n", side);
        return (USB_ERROR);
    }

    switch (command)
    {
        case USB_ATAPI_TEST_UNIT_READY :
            g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;

            /* Test unit */
            result = R_USB_HmscTestUnit(ptr, side);
        break;
        case USB_ATAPI_REQUEST_SENSE :
            g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;

            /*Request sense */
            result = R_USB_HmscRequestSense(ptr, side, buff);
        break;
        case USB_ATAPI_FORMAT_UNIT :
            return (USB_ERROR);
        break;
        case USB_ATAPI_INQUIRY :
            g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;

            /* Inquiry */
            result = R_USB_HmscInquiry(ptr, side, buff);
        break;
        case USB_ATAPI_MODE_SELECT6 :
            g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;

            /* Mode select6 */
            result = R_USB_HmscModeSelect6(ptr, side, buff);
        break;
        case USB_ATAPI_MODE_SENSE6 :
            return (USB_ERROR);
        break;
        case USB_ATAPI_START_STOP_UNIT :
            return (USB_ERROR);
        break;
        case USB_ATAPI_PREVENT_ALLOW :
            g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;

            /* Prevent allow */
            result = R_USB_HmscPreventAllow(ptr, side, buff);
        break;
        case USB_ATAPI_READ_FORMAT_CAPACITY :
            g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;

            /* Read format capacity */
            result = R_USB_HmscReadFormatCapacity(ptr, side, buff);
        break;
        case USB_ATAPI_READ_CAPACITY :
            g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;

            /* Read capacity */
            result = R_USB_HmscReadCapacity(ptr, side, buff);
        break;
        case USB_ATAPI_READ10 :
        case USB_ATAPI_WRITE10 :
            return (USB_ERROR);
        break;
        case USB_ATAPI_SEEK :
        case USB_ATAPI_WRITE_AND_VERIFY :
        case USB_ATAPI_VERIFY10 :
            return (USB_ERROR);
        break;
        case USB_ATAPI_MODE_SELECT10 :
            return (USB_ERROR);
        break;
        case USB_ATAPI_MODE_SENSE10 :
            g_usb_hmsc_strg_process[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;

            /* Mode sense10 */
            result = R_USB_HmscModeSelect10(ptr, side, buff);
        break;
        default :
            return (USB_ERROR);
        break;
    }
    if (result == USB_HMSC_OK)
    {
        return (USB_OK);
    }
    else
    {
        return (USB_ERROR);
    }
} /* End of function R_USB_HmscStrgUserCommand() */

/******************************************************************************
 End  Of File
 ******************************************************************************/
