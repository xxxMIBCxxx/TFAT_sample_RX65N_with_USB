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
 * File Name    : r_usb_hmsc_api.c
 * Description  : USB Host MSC Driver API
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
 Renesas Host MSC Driver API functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : R_USB_HmscTask
 Description     : USB HMSC Task
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void R_USB_HmscTask (void)
{
    usb_hmsc_task();
} /* End of function R_USB_HmscTask() */

/******************************************************************************
 Function Name   : class_driver_start
 Description     : Init host class driver task.
 Argument        : usb_utr_t *ptr   : USB system internal structure. Selects channel.
 Return value    : none
 ******************************************************************************/
void class_driver_start (usb_utr_t *ptr)
{
    R_USB_HmscDriverStart();
} /* End of function class_driver_start() */

/******************************************************************************
 Function Name   : R_USB_HmscDriverStart
 Description     : USB Host Initialize process
 Argument        : none
 Return          : none
 ******************************************************************************/
void R_USB_HmscDriverStart (void)
{
    uint16_t i;
    uint16_t j;

    for (i = 0; i < USB_MAXSTRAGE; i++)
    {
        g_usb_hmsc_drvno_tbl[i].use_flag = USB_NOUSE;
    }

    for (j = 0; j < USB_NUM_USBIP; j++)
    {
        for (i = 0; i < USB_MAXSTRAGE; i++)
        {
            g_usb_hmsc_csw_tag_no[j][i] = (uint16_t) 1;

            g_usb_hmsc_cbw[j][i].dcbw_signature = USB_MSC_CBW_SIGNATURE;
            g_usb_hmsc_cbw[j][i].dcbw_tag = g_usb_hmsc_csw_tag_no[j][i];
            g_usb_hmsc_cbw[j][i].dcbw_dtl_lo = 0;
            g_usb_hmsc_cbw[j][i].dcbw_dtl_ml = 0;
            g_usb_hmsc_cbw[j][i].dcbw_dtl_mh = 0;
            g_usb_hmsc_cbw[j][i].dcbw_dtl_hi = 0;
            g_usb_hmsc_cbw[j][i].bm_cbw_flags.cbw_dir = 0;
            g_usb_hmsc_cbw[j][i].bm_cbw_flags.reserved7 = 0;
            g_usb_hmsc_cbw[j][i].bcbw_lun.bcbw_lun = 0;
            g_usb_hmsc_cbw[j][i].bcbw_lun.reserved4 = 0;
            g_usb_hmsc_cbw[j][i].bcbwcb_length.bcbwcb_length = 0;
            g_usb_hmsc_cbw[j][i].bcbwcb_length.reserved3 = 0;

            g_usb_hmsc_cbw[j][i].cbwcb[0] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[1] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[2] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[3] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[4] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[5] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[6] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[7] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[8] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[9] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[10] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[11] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[12] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[13] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[14] = 0;
            g_usb_hmsc_cbw[j][i].cbwcb[15] = 0;
        }
    }

    usb_cstd_set_task_pri(USB_HMSC_TSK, USB_PRI_3);
    usb_cstd_set_task_pri(USB_HSTRG_TSK, USB_PRI_3);
} /* End of function R_USB_HmscDriverStart() */

/******************************************************************************
 Function Name   : R_USB_HmscClassCheck
 Description     : check connected device
 Arguments       : uint16_t **table : 
 Return value    : none
 ******************************************************************************/
void R_USB_HmscClassCheck (usb_utr_t *ptr, uint16_t **table)
{
    usb_utr_t *pblf;
    usb_er_t err;

    g_usb_hmsc_device_table[ptr->ip] = (uint8_t*) (table[0]);
    g_usb_hmsc_config_table[ptr->ip] = (uint8_t*) (table[1]);
    g_usb_hmsc_interface_table[ptr->ip] = (uint8_t*) (table[2]);
    g_usb_hmsc_speed[ptr->ip] = *table[6];
    g_usb_hmsc_devaddr[ptr->ip] = *table[7];
    *table[3] = USB_OK;

    /* Get mem pool blk */
    if ( R_USB_PGET_BLK(USB_HMSC_MPL,&pblf) == USB_OK)
    {
        pblf->msginfo = USB_MSG_CLS_INIT;
        g_usb_hmsc_init_seq[ptr->ip] = USB_SEQ_0;

        pblf->ip = ptr->ip;
        pblf->ipp = ptr->ipp;

        /* Send message */
        err = R_USB_SND_MSG(USB_HMSC_MBX, (usb_msg_t* )pblf);
        if (USB_OK != err)
        {
            err = R_USB_REL_BLK(USB_HMSC_MPL, (usb_mh_t )pblf);
            USB_PRINTF0("### ClassCheck function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### ClassCheck function pget_blk error\n");
    }
} /* End of function R_USB_HmscClassCheck() */

/******************************************************************************
 Function Name   : R_USB_HmscRead10
 Description     : Read10
 Arguments       : uint16_t side        : Side
 : uint8_t  *buff       : Buffer address
 : uint32_t secno       : Sector number
 : uint16_t seccnt      : Sector count
 : uint32_t trans_byte  : Trans byte
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscRead10 (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno, uint16_t seccnt,
        uint32_t trans_byte)
{
    uint16_t hmsc_retval;

    /* set CBW parameter */
    usb_hmsc_set_rw_cbw(ptr, (uint16_t) USB_ATAPI_READ10, secno, seccnt, trans_byte, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_data_in(ptr, side, buff, trans_byte);
    return (hmsc_retval);
} /* End of function R_USB_HmscRead10() */

/******************************************************************************
 Function Name   : R_USB_HmscWrite10
 Description     : Write10
 Arguments       : uint16_t side       : Side
 : uint8_t *buff       : Buffer address
 : uint32_t secno      : Sector number
 : uint16_t seccnt     : Sector count
 : uint32_t trans_byte : Trans byte
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscWrite10 (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno, uint16_t seccnt,
        uint32_t trans_byte)
{
    uint16_t hmsc_retval;

    /* set CBW parameter */
    usb_hmsc_set_rw_cbw(ptr, (uint16_t) USB_ATAPI_WRITE10, secno, seccnt, trans_byte, side);

    /* Data OUT */
    hmsc_retval = usb_hmsc_data_out(ptr, side, buff, trans_byte);
    return (hmsc_retval);
} /* End of function R_USB_HmscWrite10() */

/******************************************************************************
 Function Name   : R_USB_HmscTestUnit
 Description     : TestUnit
 Arguments       : uint16_t side : 
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscTestUnit (usb_utr_t *ptr, uint16_t side)
{
    uint8_t cbwcb[12];
    uint16_t hmsc_retval;

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 12, cbwcb);

    /* Data set */
    /* Command */
    cbwcb[0] = USB_ATAPI_TEST_UNIT_READY;

    /* Set CBW parameter */
    usb_hmsc_set_els_cbw(ptr, (uint8_t*) &cbwcb, (uint32_t) 0, side);

    /* No Data */
    hmsc_retval = usb_hmsc_no_data(ptr, side);
    return (hmsc_retval);
} /* End of function R_USB_HmscTestUnit() */

/******************************************************************************
 Function Name   : R_USB_HmscRequestSense
 Description     : RequestSense
 Arguments       : uint16_t side : 
 : uint8_t *buff : 
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscRequestSense (usb_utr_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t cbwcb[12];
    uint8_t length = 18;
    uint16_t hmsc_retval;

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 12, cbwcb);

    /* Data set */
    /* Command */
    cbwcb[0] = USB_ATAPI_REQUEST_SENSE;

    /* Allocation length */
    cbwcb[4] = length;

    /* Set CBW parameter */
    usb_hmsc_set_els_cbw(ptr, (uint8_t*) &cbwcb, (uint32_t) length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_data_in(ptr, side, buff, (uint32_t) length);
    return (hmsc_retval);
} /* End of function R_USB_HmscRequestSense() */

/******************************************************************************
 Function Name   : R_USB_HmscInquiry
 Description     : Inquiry
 Arguments       : uint16_t side : 
 : uint8_t *buff : 
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscInquiry (usb_utr_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t cbwcb[12];
    uint8_t length = 36;
    uint16_t hmsc_retval;

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 12, cbwcb);

    /* Data set */
    /* Command */
    cbwcb[0] = USB_ATAPI_INQUIRY;

    /* Allocation length */
    cbwcb[4] = length;

    /* Set CBW parameter */
    usb_hmsc_set_els_cbw(ptr, (uint8_t*) &cbwcb, (uint32_t) length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_data_in(ptr, side, buff, (uint32_t) length);
    return (hmsc_retval);
} /* End of function R_USB_HmscInquiry() */

/******************************************************************************
 Function Name   : R_USB_HmscReadCapacity
 Description     : ReadCapacity
 Arguments       : uint16_t side : 
 : uint8_t *buff : 
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscReadCapacity (usb_utr_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t cbwcb[12];
    uint8_t length = 8;
    uint16_t hmsc_retval;

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 12, cbwcb);

    /* Data set */
    /* Command */
    cbwcb[0] = USB_ATAPI_READ_CAPACITY;

    /* Set CBW parameter */
    usb_hmsc_set_els_cbw(ptr, (uint8_t*) &cbwcb, (uint32_t) length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_data_in(ptr, side, buff, (uint32_t) length);
    return (hmsc_retval);
} /* End of function R_USB_HmscReadCapacity() */

/******************************************************************************
 Function Name   : R_USB_HmscReadFormatCapacity
 Description     : ReadFormatCapacity
 Arguments       : uint16_t side : 
 : uint8_t *buff : 
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscReadFormatCapacity (usb_utr_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t cbwcb[12];
    uint8_t length = 0x20;
    uint16_t hmsc_retval;

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 12, cbwcb);

    /* Data set */
    /* Command */
    cbwcb[0] = USB_ATAPI_READ_FORMAT_CAPACITY;

    /* Allocation length */
    cbwcb[8] = length;

    /* Set CBW parameter */
    usb_hmsc_set_els_cbw(ptr, (uint8_t*) &cbwcb, (uint32_t) length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_data_in(ptr, side, buff, (uint32_t) length);
    return (hmsc_retval);
} /* End of function R_USB_HmscReadFormatCapacity() */

/******************************************************************************
 Function Name   : R_USB_HmscGetMaxUnit
 Description     : Get Max LUN request
 Argument        : uint16_t addr             : Device Address
 : usb_cb_t complete         : CallBack Function
 Return value    : usb_er_t                  : Error Code
 ******************************************************************************/
usb_er_t R_USB_HmscGetMaxUnit (usb_utr_t *ptr, uint16_t addr, uint8_t *buff, usb_cb_t complete)
{
    usb_er_t err;
    static usb_ctrl_trans_t get_max_lun_table;

    get_max_lun_table.setup.type = 0xFEA1;
    get_max_lun_table.setup.value = 0x0000;
    get_max_lun_table.setup.index = 0x0000;
    get_max_lun_table.setup.length = 0x0001;
    get_max_lun_table.address = addr;

    /* Recieve MaxLUN */
    g_usb_hmsc_class_control[ptr->ip].keyword = USB_PIPE0;
    g_usb_hmsc_class_control[ptr->ip].tranadr = buff;
    g_usb_hmsc_class_control[ptr->ip].tranlen = get_max_lun_table.setup.length;
    g_usb_hmsc_class_control[ptr->ip].setup = (uint16_t *) &get_max_lun_table;
    g_usb_hmsc_class_control[ptr->ip].complete = complete;
    g_usb_hmsc_class_control[ptr->ip].segment = USB_TRAN_END;
    g_usb_hmsc_class_control[ptr->ip].ip = ptr->ip;
    g_usb_hmsc_class_control[ptr->ip].ipp = ptr->ipp;

    err = usb_hstd_transfer_start(&g_usb_hmsc_class_control[ptr->ip]);
    return err;
} /* End of function R_USB_HmscGetMaxUnit() */

/******************************************************************************
 Function Name   : R_USB_HmscMassStorageReset
 Description     : Mass Strage Reset request
 Argument        : uint16_t addr             : Device Address
 : usb_cb_t complete        : CallBack Function
 Return value    : usb_er_t                 : Error Code
 ******************************************************************************/
usb_er_t R_USB_HmscMassStorageReset (usb_utr_t *ptr, uint16_t addr, usb_cb_t complete)
{
    usb_er_t err;

    static usb_ctrl_trans_t mass_storage_reset_table;

    mass_storage_reset_table.setup.type = 0xFF21;
    mass_storage_reset_table.setup.value = 0x0000;
    mass_storage_reset_table.setup.index = 0x0000;
    mass_storage_reset_table.setup.length = 0x0000;
    mass_storage_reset_table.address = addr;

    /* Set MassStorageReset */
    g_usb_hmsc_class_control[ptr->ip].keyword = USB_PIPE0;
    g_usb_hmsc_class_control[ptr->ip].tranadr = USB_NULL;
    g_usb_hmsc_class_control[ptr->ip].tranlen = mass_storage_reset_table.setup.length;
    g_usb_hmsc_class_control[ptr->ip].setup = (uint16_t *) &mass_storage_reset_table;
    g_usb_hmsc_class_control[ptr->ip].complete = complete;
    g_usb_hmsc_class_control[ptr->ip].segment = USB_TRAN_END;
    g_usb_hmsc_class_control[ptr->ip].ip = ptr->ip;
    g_usb_hmsc_class_control[ptr->ip].ipp = ptr->ipp;

    err = usb_hstd_transfer_start(&g_usb_hmsc_class_control[ptr->ip]);
    return err;
} /* End of function R_USB_HmscMassStorageReset() */

/******************************************************************************
 Function Name   : R_USB_HmscPreventAllow
 Description     : PreventAllow
 Arguments       : uint16_t side : 
 : uint8_t *buff : 
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscPreventAllow (usb_utr_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t cbwcb[12];
    uint16_t hmsc_retval;

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 12, cbwcb);

    /* Data set */
    /* Command */
    cbwcb[0] = USB_ATAPI_PREVENT_ALLOW;

    /* Reserved */
    cbwcb[4] = buff[0];

    /* Set CBW parameter */
    usb_hmsc_set_els_cbw(ptr, (uint8_t*) &cbwcb, (uint32_t) 0, side);

    /* No Data */
    hmsc_retval = usb_hmsc_no_data(ptr, side);
    return (hmsc_retval);
} /* End of function R_USB_HmscPreventAllow() */

/******************************************************************************
 Function Name   : R_USB_HmscModeSelect10
 Description     : ModeSense10
 Arguments       : uint16_t side : 
 : uint8_t *buff : 
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscModeSelect10 (usb_utr_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t cbwcb[12];
    uint8_t length = 26;
    uint16_t hmsc_retval;

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 12, cbwcb);

    /* Data set */
    /* Command */
    cbwcb[0] = USB_ATAPI_MODE_SENSE10;

    /* Set LUN / DBD=1 */
    cbwcb[1] = 0x08;

    /* Allocation length */
    cbwcb[7] = 0x00;

    /* Allocation length */
    cbwcb[8] = 0x02;

    /* Set CBW parameter */
    usb_hmsc_set_els_cbw(ptr, (uint8_t*) &cbwcb, (uint32_t) length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_data_in(ptr, side, buff, (uint32_t) length);
    return (hmsc_retval);
} /* End of function R_USB_HmscModeSelect10() */

/******************************************************************************
 Function Name   : R_USB_HmscModeSelect6
 Description     : ModeSelect6
 Arguments       : uint16_t side : 
 : uint8_t *buff : 
 Return value    : uint16_t : 
 ******************************************************************************/
uint16_t R_USB_HmscModeSelect6 (usb_utr_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t cbwcb[12];
    uint8_t length = 18;
    uint16_t hmsc_retval;

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 12, cbwcb);

    /* Data set */
    /* Command */
    cbwcb[0] = USB_ATAPI_MODE_SELECT6;

    /* SP=0 */
    cbwcb[1] = 0x10;

    /* Parameter list length ??? */
    cbwcb[4] = 0x18;

    /* Set CBW parameter */
    usb_hmsc_set_els_cbw(ptr, (uint8_t*) &cbwcb, (uint32_t) length, side);

    /* Data clear */
    usb_hmsc_clr_data((uint16_t) 24, buff);

    /* Data set */
    buff[3] = 0x08;
    buff[10] = 0x02;
    buff[12] = 0x08;
    buff[13] = 0x0A;

    /* Data OUT */
    hmsc_retval = usb_hmsc_data_out(ptr, side, buff, (uint32_t) length);
    return (hmsc_retval);
} /* End of function R_USB_HmscModeSelect6() */

/******************************************************************************
 Function Name   : R_USB_HmscGetDevSts
 Description     : Responds to HMSCD operation state
 Arguments       : none
 Return value    : uint16_t :
 ******************************************************************************/
uint16_t R_USB_HmscGetDevSts (uint16_t side)
{
    return (g_usb_hmsc_drvno_tbl[side].use_flag);
} /* End of function R_USB_HmscGetDevSts() */

/******************************************************************************
 Function Name   : R_USB_HmscAllocDrvno
 Description     : Alloc Drive no.
 Arguments       : uint16_t devadr       : Device address
 Return value    : Drive no.
 ******************************************************************************/
uint16_t R_USB_HmscAllocDrvno (uint16_t devadr)
{
    uint16_t side;
    uint16_t ipno;

    if (USBA_ADDRESS_OFFSET == (devadr & USB_IP_MASK))
    {
        ipno = USB_IP1;
    }
    else
    {
        ipno = USB_IP0;
    }
    devadr &= USB_ADDRESS_MASK;

    for (side = 0; side < USB_MAXSTRAGE; side++)
    {
        if ( USB_NOUSE == g_usb_hmsc_drvno_tbl[side].use_flag)
        {
            g_usb_hmsc_drvno_tbl[side].use_flag = USB_TRUE;
            g_usb_hmsc_drvno_tbl[side].devadr = devadr;
            g_usb_hmsc_drvno_tbl[side].ip = ipno;
            return side;
        }
    }

    USB_PRINTF0("DRIVE ALLOC ERROR");
    return USB_ERROR;
} /* End of function R_USB_HmscAllocDrvno() */

/******************************************************************************
 Function Name   : R_USB_HmscFreeDrvno
 Description     : Release Drive no.
 Arguments       : uint16_t side        : Drive no.
 Return value    : result
 ******************************************************************************/
uint16_t R_USB_HmscFreeDrvno (uint16_t side)
{
    if (side > USB_MAXSTRAGE)
    {
        return USB_ERROR;
    }

    g_usb_hmsc_drvno_tbl[side].use_flag = USB_NOUSE;
    g_usb_hmsc_drvno_tbl[side].ip = USB_NOUSE;
    g_usb_hmsc_drvno_tbl[side].devadr = USB_NOUSE;
    return USB_OK;
} /* End of function R_USB_HmscFreeDrvno() */

/******************************************************************************
 Function Name   : R_USB_HmscRefDrvno
 Description     : Get Drive no.
 Arguments       : uint16_t devadr       : Device address
 Return value    : Drive no.
 ******************************************************************************/
uint16_t R_USB_HmscRefDrvno (uint16_t devadr)
{
    uint16_t side;
    uint16_t ipno;

    if ((devadr & USB_IP_MASK) == USBA_ADDRESS_OFFSET)
    {
        ipno = USB_IP1;
    }
    else
    {
        ipno = USB_IP0;
    }
    devadr &= USB_ADDRESS_MASK;

    for (side = 0; side < USB_MAXSTRAGE; side++)
    {
        if ((devadr == g_usb_hmsc_drvno_tbl[side].devadr) && (ipno == g_usb_hmsc_drvno_tbl[side].ip))
        {
            return side;
        }
    }
    return USB_ERROR;
} /* End of function R_USB_HmscRefDrvno() */

/******************************************************************************
 End  Of File
 ******************************************************************************/
