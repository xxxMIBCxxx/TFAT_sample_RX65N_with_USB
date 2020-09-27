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
 * File Name    : r_usb_pstdrequest.c
 * Description  : USB Peripheral standard request code
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

#if ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )
/******************************************************************************
 Static variables and functions
 ******************************************************************************/
static void usb_pstd_get_status1 (void);
static void usb_pstd_get_descriptor1 (void);
static void usb_pstd_get_configuration1 ();
static void usb_pstd_get_interface1 ();
static void usb_pstd_clr_feature0 (void);
static void usb_pstd_clr_feature3 (void);
static void usb_pstd_set_feature0 (void);
static void usb_pstd_set_feature3 (void);
static void usb_pstd_set_address0 (void);
static void usb_pstd_set_address3 (void);
static void usb_pstd_set_descriptor2 ();
static void usb_pstd_set_configuration0 (void);
static void usb_pstd_set_configuration3 (void);
static void usb_pstd_set_interface0 (void);
static void usb_pstd_set_interface3 (void);
static void usb_pstd_synch_rame1 ();

/*****************************************************************************
 Public Variables
 ******************************************************************************/

/******************************************************************************
 Renesas Abstracted Peripheral standard request functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_stand_req0
 Description     : The idle and setup stages of a standard request from host.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_stand_req0 (void)
{
    switch ((g_usb_pstd_req_type & USB_BREQUEST))
    {
        case USB_CLEAR_FEATURE :
            /* Clear Feature0 */
            usb_pstd_clr_feature0();
        break;
        case USB_SET_FEATURE :
            /* Set Feature0 */
            usb_pstd_set_feature0();
        break;
        case USB_SET_ADDRESS :
            /* Set Address0 */
            usb_pstd_set_address0();
        break;
        case USB_SET_CONFIGURATION :
            /* Set Configuration0 */
            usb_pstd_set_configuration0();
        break;
        case USB_SET_INTERFACE :
            /* Set Interface0 */
            usb_pstd_set_interface0();
        break;
        default :
        break;
    }
}
/******************************************************************************
 End of function usb_pstd_stand_req0
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_stand_req1
 Description     : The control read data stage of a standard request from host.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_stand_req1 (void)
{
    switch ((g_usb_pstd_req_type & USB_BREQUEST))
    {
        case USB_GET_STATUS :
            /* Get Status1 */
            usb_pstd_get_status1();
        break;
        case USB_GET_DESCRIPTOR :
            /* Get Descriptor1 */
            usb_pstd_get_descriptor1();
        break;
        case USB_GET_CONFIGURATION :
            /* Get Configuration1 */
            usb_pstd_get_configuration1();
        break;
        case USB_GET_INTERFACE :
            /* Get Interface1 */
            usb_pstd_get_interface1();
        break;
        case USB_SYNCH_FRAME :
            /* Synch Frame */
            usb_pstd_synch_rame1();
        break;
        default :
            /* Set pipe USB_PID_STALL */
            usb_pstd_set_stall_pipe0();
        break;
    }
}
/******************************************************************************
 End of function usb_pstd_stand_req1
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_stand_req2
 Description     : The control write data stage of a standard request from host.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_stand_req2 (void)
{
    if ((g_usb_pstd_req_type & USB_BREQUEST) == USB_SET_DESCRIPTOR)
    {
        /* Set Descriptor2 */
        usb_pstd_set_descriptor2();
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_stand_req2
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_stand_req3
 Description     : Standard request process. This is for the status stage of a
 : control write where there is no data stage.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_stand_req3 (void)
{
    switch ((g_usb_pstd_req_type & USB_BREQUEST))
    {
        case USB_CLEAR_FEATURE :
            /* ClearFeature3 */
            usb_pstd_clr_feature3();
        break;
        case USB_SET_FEATURE :
            /* SetFeature3 */
            usb_pstd_set_feature3();
        break;
        case USB_SET_ADDRESS :
            /* SetAddress3 */
            usb_pstd_set_address3();
        break;
        case USB_SET_CONFIGURATION :
            /* SetConfiguration3 */
            usb_pstd_set_configuration3();
        break;
        case USB_SET_INTERFACE :
            /* SetInterface3 */
            usb_pstd_set_interface3();
        break;
        default :
            /* Set pipe USB_PID_STALL */
            usb_pstd_set_stall_pipe0();
        break;
    }
    /* Control transfer stop(end) */
    usb_pstd_ctrl_end((uint16_t) USB_CTRL_END);
}
/******************************************************************************
 End of function usb_pstd_stand_req3
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_stand_req4
 Description     : The control read status stage of a standard request from host.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_stand_req4 (void)
{
    switch ((g_usb_pstd_req_type & USB_BREQUEST))
    {
        case USB_GET_STATUS :
            /* GetStatus4 */
            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
        break;
        case USB_GET_DESCRIPTOR :
            /* GetDescriptor4 */
            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
        break;
        case USB_GET_CONFIGURATION :
            /* GetConfiguration4 */
            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
        break;
        case USB_GET_INTERFACE :
            /* GetInterface4 */
            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
        break;
        case USB_SYNCH_FRAME :
            /* SynchFrame4 */
            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
        break;
        default :
            /* Set pipe USB_PID_STALL */
            usb_pstd_set_stall_pipe0();
        break;
    }
    /* Control transfer stop(end) */
    usb_pstd_ctrl_end((uint16_t) USB_CTRL_END);
}
/******************************************************************************
 End of function usb_pstd_stand_req4
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_stand_req5
 Description     : The control write status stage of a standard request from host.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_stand_req5 (void)
{
    if ((g_usb_pstd_req_type & USB_BREQUEST) == USB_SET_DESCRIPTOR)
    {
        /* Set pipe PID_BUF */
        usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_set_stall_pipe0();
    }
    /* Control transfer stop(end) */
    usb_pstd_ctrl_end((uint16_t) USB_CTRL_END);
}
/******************************************************************************
 End of function usb_pstd_stand_req5
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_get_status1
 Description     : Analyze a Get Status command and process it.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_get_status1 (void)
{
    static uint8_t tbl[2];
    uint16_t ep;
    uint16_t buffer;
    uint16_t pipe;

    if ((g_usb_pstd_req_value == 0) && (g_usb_pstd_req_length == 2))
    {
        tbl[0] = 0;
        tbl[1] = 0;
        /* Check request type */
        switch ((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP))
        {
            case USB_DEVICE :
                if (g_usb_pstd_req_index == 0)
                {
                    /* Self powered / Bus powered */
                    tbl[0] = usb_pstd_get_current_power();
                    /* Support remote wakeup ? */
                    if (g_usb_pstd_remote_wakeup == USB_TRUE)
                    {
                        tbl[0] |= USB_GS_REMOTEWAKEUP;
                    }
                    /* Control read start */
                    usb_pstd_ctrl_read((uint32_t) 2, tbl);
                }
                else
                {
                    /* Request error */
                    usb_pstd_set_stall_pipe0();
                }
            break;
            case USB_INTERFACE :
                if (usb_pstd_chk_configured() == USB_TRUE)
                {
                    if (g_usb_pstd_req_index < usb_pstd_get_interface_num(g_usb_pstd_config_num))
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) 2, tbl);
                    }
                    else
                    {
                        /* Request error (not exist interface) */
                        usb_pstd_set_stall_pipe0();
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_set_stall_pipe0();
                }
            break;
            case USB_ENDPOINT :
                /* Endpoint number */
                ep = (uint16_t) (g_usb_pstd_req_index & USB_EPNUMFIELD);
                /* Endpoint 0 */
                if (ep == 0)
                {
                    buffer = HW_USB_ReadDcpctr();
                    if ((buffer & USB_PID_STALL) != (uint16_t) 0)
                    {
                        /* Halt set */
                        tbl[0] = USB_GS_HALT;
                    }
                    /* Control read start */
                    usb_pstd_ctrl_read((uint32_t) 2, tbl);
                }
                /* EP1 to max */
                else if (ep <= USB_MAX_EP_NO)
                {
                    if (usb_pstd_chk_configured() == USB_TRUE)
                    {
                        pipe = usb_pstd_epadr2pipe(g_usb_pstd_req_index);
                        if (pipe == USB_ERROR)
                        {
                            /* Set pipe USB_PID_STALL */
                            usb_pstd_set_stall_pipe0();
                        }
                        else
                        {
                            buffer = usb_cstd_get_pid(USB_NULL, pipe);
                            if ((buffer & USB_PID_STALL) != (uint16_t) 0)
                            {
                                /* Halt set */
                                tbl[0] = USB_GS_HALT;
                            }
                            /* Control read start */
                            usb_pstd_ctrl_read((uint32_t) 2, tbl);
                        }
                    }
                    else
                    {
                        /* Set pipe USB_PID_STALL */
                        usb_pstd_set_stall_pipe0();
                    }
                }
                else
                {
                    /* Set pipe USB_PID_STALL */
                    usb_pstd_set_stall_pipe0();
                }
            break;
            default :
                /* Set pipe USB_PID_STALL */
                usb_pstd_set_stall_pipe0();
            break;
        }
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_get_status1
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_get_descriptor1
 Description     : Analyze a Get Descriptor command from host and process it.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_get_descriptor1 (void)
{
    uint16_t len;
    uint16_t idx;
    uint8_t *table;
    uint16_t connect_info;

    if ((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP) == USB_DEVICE)
    {
        idx = (uint16_t) (g_usb_pstd_req_value & USB_DT_INDEX);
        switch ((uint16_t) USB_GET_DT_TYPE(g_usb_pstd_req_value))
        {
            /*---- Device descriptor ----*/
            case USB_DT_DEVICE :
                if ((g_usb_pstd_req_index == (uint16_t) 0) && (idx == (uint16_t) 0))
                {
                    table = g_usb_pstd_driver.devicetbl;
                    if (g_usb_pstd_req_length < table[0])
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) g_usb_pstd_req_length, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) table[0], table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_set_stall_pipe0();
                }
            break;
                /*---- Configuration descriptor ----*/
            case USB_DT_CONFIGURATION :
                if ((g_usb_pstd_req_index == 0) && (idx == (uint16_t) 0))
                {
                    connect_info = usb_cstd_port_speed(USB_NULL, USB_NULL);
                    if (connect_info == USB_HSCONNECT)
                    {
                        table = g_usb_pstd_driver.othertbl;
                    }
                    else
                    {
                        table = g_usb_pstd_driver.configtbl;
                    }
                    len = (uint16_t) (*(uint8_t*) ((uint32_t) table + (uint32_t) 3));
                    len = (uint16_t) (len << 8);
                    len += (uint16_t) (*(uint8_t*) ((uint32_t) table + (uint32_t) 2));
                    /* Descriptor > wLength */
                    if (g_usb_pstd_req_length < len)
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) g_usb_pstd_req_length, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) len, table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_set_stall_pipe0();
                }
            break;
                /*---- String descriptor ----*/
            case USB_DT_STRING :
                if (idx < 7)
                {
                    table = g_usb_pstd_driver.stringtbl[idx];
                    len = (uint16_t) (*(uint8_t*) ((uint32_t) table + (uint32_t) 0));
                    if (g_usb_pstd_req_length < len)
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) g_usb_pstd_req_length, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) len, table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_set_stall_pipe0();
                }
            break;
                /*---- Interface descriptor ----*/
            case USB_DT_INTERFACE :
                /* Set pipe USB_PID_STALL */
                usb_pstd_set_stall_pipe0();
            break;
                /*---- Endpoint descriptor ----*/
            case USB_DT_ENDPOINT :
                /* Set pipe USB_PID_STALL */
                usb_pstd_set_stall_pipe0();
            break;
            case USB_DT_DEVICE_QUALIFIER :
                if ((usb_pstd_hi_speed_enable() == USB_TRUE) && (g_usb_pstd_req_index == 0) && (idx == 0))
                {
                    table = g_usb_pstd_driver.qualitbl;
                    if (g_usb_pstd_req_length < table[0])
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) g_usb_pstd_req_length, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) table[0], table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_set_stall_pipe0();
                }
            break;
            case USB_DT_OTHER_SPEED_CONF :
                if ((usb_pstd_hi_speed_enable() == USB_TRUE) && (g_usb_pstd_req_index == 0) && (idx == (uint16_t) 0))
                {
                    connect_info = usb_cstd_port_speed(USB_NULL, USB_NULL);
                    if (connect_info == USB_HSCONNECT)
                    {
                        table = g_usb_pstd_driver.configtbl;
                    }
                    else
                    {
                        table = g_usb_pstd_driver.othertbl;
                    }
                    len = (uint16_t) (*(uint8_t*) ((uint32_t) table + (uint32_t) 3));
                    len = (uint16_t) (len << 8);
                    len += (uint16_t) (*(uint8_t*) ((uint32_t) table + (uint32_t) 2));
                    /* Descriptor > wLength */
                    if (g_usb_pstd_req_length < len)
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) g_usb_pstd_req_length, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ctrl_read((uint32_t) len, table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_set_stall_pipe0();
                }
            break;
            case USB_DT_INTERFACE_POWER :
                /* Not support */
                usb_pstd_set_stall_pipe0();
            break;
            default :
                /* Set pipe USB_PID_STALL */
                usb_pstd_set_stall_pipe0();
            break;
        }
    }
    else if ((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP) == USB_INTERFACE)
    {
        g_usb_pstd_req_reg.type = g_usb_pstd_req_type;
        g_usb_pstd_req_reg.value = g_usb_pstd_req_value;
        g_usb_pstd_req_reg.index = g_usb_pstd_req_index;
        g_usb_pstd_req_reg.length = g_usb_pstd_req_length;
        if (USB_NULL != g_usb_pstd_driver.ctrltrans)
        {
            (*g_usb_pstd_driver.ctrltrans)((usb_setup_t *) &g_usb_pstd_req_reg, (uint16_t) USB_NO_ARG);
        }
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_get_descriptor1
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_get_configuration1
 Description     : Analyze a Get Configuration command and process it.
 : (for control read data stage)
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_get_configuration1 (void)
{
    static uint8_t tbl[2];

    /* check request */
    if (((((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP) == USB_DEVICE) && (g_usb_pstd_req_value == 0))
            && (g_usb_pstd_req_index == 0)) && (g_usb_pstd_req_length == 1))
    {
        tbl[0] = (uint8_t) g_usb_pstd_config_num;
        /* Control read start */
        usb_pstd_ctrl_read((uint32_t) 1, tbl);
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_get_configuration1
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_get_interface1
 Description     : Analyze a Get Interface command and process it.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_get_interface1 (void)
{
    static uint8_t tbl[2];

    /* check request */
    if ((((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP) == USB_INTERFACE) && (g_usb_pstd_req_value == 0))
            && (g_usb_pstd_req_length == 1))
    {
        if (g_usb_pstd_req_index < USB_ALT_NO)
        {
            tbl[0] = (uint8_t) g_usb_pstd_alt_num[g_usb_pstd_req_index];
            /* Start control read */
            usb_pstd_ctrl_read((uint32_t) 1, tbl);
        }
        else
        {
            /* Request error */
            usb_pstd_set_stall_pipe0();
        }
    }
    else
    {
        /* Request error */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_get_interface1
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_clr_feature0
 Description     : Clear Feature0
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_clr_feature0 (void)
{
    /* Non processing. */
}
/******************************************************************************
 End of function usb_pstd_clr_feature0
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_clr_feature3
 Description     : Analyze a Clear Feature command and process it.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_clr_feature3 (void)
{
    uint16_t pipe;
    uint16_t ep;

    if (g_usb_pstd_req_length == 0)
    {
        /* check request type */
        switch ((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP))
        {
            case USB_DEVICE :
                if ((g_usb_pstd_req_value == USB_DEV_REMOTE_WAKEUP) && (g_usb_pstd_req_index == 0))
                {
                    if (usb_pstd_chk_remote() == USB_TRUE)
                    {
                        g_usb_pstd_remote_wakeup = USB_FALSE;
                        /* Set pipe PID_BUF */
                        usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
                    }
                    else
                    {
                        /* Not support remote wakeup */
                        /* Request error */
                        usb_pstd_set_stall_pipe0();
                    }
                }
                else
                {
                    /* Not specification */
                    usb_pstd_set_stall_pipe0();
                }
            break;
            case USB_INTERFACE :
                /* Request error */
                usb_pstd_set_stall_pipe0();
            break;
            case USB_ENDPOINT :
                /* Endpoint number */
                ep = (uint16_t) (g_usb_pstd_req_index & USB_EPNUMFIELD);
                if (g_usb_pstd_req_value == USB_ENDPOINT_HALT)
                {
                    /* EP0 */
                    if (ep == 0)
                    {
                        /* Stall clear */
                        usb_cstd_clr_stall(USB_NULL, (uint16_t) USB_PIPE0);
                        /* Set pipe PID_BUF */
                        usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
                    }
                    /* EP1 to max */
                    else if (ep <= USB_MAX_EP_NO)
                    {
                        pipe = usb_pstd_epadr2pipe(g_usb_pstd_req_index);
                        if (pipe == USB_ERROR)
                        {
                            /* Request error */
                            usb_pstd_set_stall_pipe0();
                        }
                        else
                        {
                            if (usb_cstd_get_pid(USB_NULL, pipe) == USB_PID_BUF)
                            {
                                usb_cstd_set_nak(USB_NULL, pipe);
                                /* SQCLR=1 */
                                HW_USB_SetSqclr(USB_NULL, pipe);
                                /* Set pipe PID_BUF */
                                usb_cstd_set_buf(USB_NULL, pipe);
                            }
                            else
                            {
                                usb_cstd_clr_stall(USB_NULL, pipe);
                                /* SQCLR=1 */
                                HW_USB_SetSqclr(USB_NULL, pipe);
                            }
                            /* Set pipe PID_BUF */
                            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
                            if (g_usb_pstd_stall_pipe[pipe] == USB_TRUE)
                            {
                                g_usb_pstd_stall_pipe[pipe] = USB_FALSE;
                                (*g_usb_pstd_stall_cb)(USB_NULL, USB_NULL, USB_NULL);
                            }
                        }
                    }
                    else
                    {
                        /* Request error */
                        usb_pstd_set_stall_pipe0();
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_set_stall_pipe0();
                }
            break;
            default :
                usb_pstd_set_stall_pipe0();
            break;
        }
    }
    else
    {
        /* Not specification */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_clr_feature3
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_feature0
 Description     : Set Feature0 (for idle/setup stage)
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_feature0 (void)
{
    /* Non processing. */
}
/******************************************************************************
 End of function usb_pstd_set_feature0
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_feature3
 Description     : Analyze a Set Feature command and process it.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_feature3 (void)
{
    uint16_t pipe;
    uint16_t ep;

    if (g_usb_pstd_req_length == 0)
    {
        /* check request type */
        switch ((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP))
        {
            case USB_DEVICE :
                switch (g_usb_pstd_req_value)
                {
                    case USB_DEV_REMOTE_WAKEUP :
                        if (g_usb_pstd_req_index == 0)
                        {
                            if (usb_pstd_chk_remote() == USB_TRUE)
                            {
                                g_usb_pstd_remote_wakeup = USB_TRUE;
                                /* Set pipe PID_BUF */
                                usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
                            }
                            else
                            {
                                /* Not support remote wakeup */
                                /* Request error */
                                usb_pstd_set_stall_pipe0();
                            }
                        }
                        else
                        {
                            /* Not specification */
                            usb_pstd_set_stall_pipe0();
                        }
                    break;
                    case USB_TEST_MODE :
                        if (usb_cstd_port_speed(USB_NULL, USB_NULL) == USB_HSCONNECT)
                        {
                            if ((g_usb_pstd_req_index < USB_TEST_RESERVED)
                                    || (USB_TEST_VSTMODES <= g_usb_pstd_req_index))
                            {
                                g_usb_pstd_test_mode_flag = USB_TRUE;
                                g_usb_pstd_test_mode_select = g_usb_pstd_req_index;
                                /* Set pipe PID_BUF */
                                usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
                            }
                            else
                            {
                                /* Not specification */
                                usb_pstd_set_stall_pipe0();
                            }
                        }
                        else
                        {
                            /* Not specification */
                            usb_pstd_set_stall_pipe0();
                        }
                    break;
                    default :
                        usb_pstd_set_feature_function();
                    break;
                }
            break;
            case USB_INTERFACE :
                /* Set pipe USB_PID_STALL */
                usb_pstd_set_stall_pipe0();
            break;
            case USB_ENDPOINT :
                /* Endpoint number */
                ep = (uint16_t) (g_usb_pstd_req_index & USB_EPNUMFIELD);
                if (g_usb_pstd_req_value == USB_ENDPOINT_HALT)
                {
                    /* EP0 */
                    if (ep == 0)
                    {
                        /* Set pipe PID_BUF */
                        usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
                    }
                    /* EP1 to max */
                    else if (ep <= USB_MAX_EP_NO)
                    {
                        pipe = usb_pstd_epadr2pipe(g_usb_pstd_req_index);
                        if (pipe == USB_ERROR)
                        {
                            /* Request error */
                            usb_pstd_set_stall_pipe0();
                        }
                        else
                        {
                            /* Set pipe USB_PID_STALL */
                            usb_pstd_set_stall(pipe);
                            /* Set pipe PID_BUF */
                            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
                        }
                    }
                    else
                    {
                        /* Request error */
                        usb_pstd_set_stall_pipe0();
                    }
                }
                else
                {
                    /* Not specification */
                    usb_pstd_set_stall_pipe0();
                }
            break;

            default :
                /* Request error */
                usb_pstd_set_stall_pipe0();
            break;
        }
    }
    else
    {
        /* Request error */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_set_feature3
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_address0
 Description     : Set Address0 (for idle/setup stage).
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_address0 (void)
{
    /* Non processing. */
}
/******************************************************************************
 End of function usb_pstd_set_address0
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_address3
 Description     : Analyze a Set Address command and process it.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_address3 (void)
{
    if ((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP) == USB_DEVICE)
    {
        if ((g_usb_pstd_req_index == 0) && (g_usb_pstd_req_length == 0))
        {
            if (g_usb_pstd_req_value <= 127)
            {
                /* Set pipe PID_BUF */
                usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
            }
            else
            {
                /* Not specification */
                usb_pstd_set_stall_pipe0();
            }
        }
        else
        {
            /* Not specification */
            usb_pstd_set_stall_pipe0();
        }
    }
    else
    {
        /* Request error */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_set_address3
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_descriptor2
 Description     : Return STALL in response to a Set Descriptor command.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_descriptor2 (void)
{
    /* Not specification */
    usb_pstd_set_stall_pipe0();
}
/******************************************************************************
 End of function usb_pstd_set_descriptor2
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_configuration0
 Description     : Call callback function to notify the reception of SetConfiguration command
 : (for idle /setup stage)
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_configuration0 (void)
{
    uint16_t config_num;

    config_num = g_usb_pstd_config_num;

    /* Configuration number set */
    usb_pstd_set_config_num(g_usb_pstd_req_value);

    if (g_usb_pstd_req_value != config_num)
    {
        if (USB_NULL != g_usb_pstd_driver.devconfig)
        {
            /* Registration open function call */
            (*g_usb_pstd_driver.devconfig)(USB_NULL, g_usb_pstd_config_num, USB_NULL);
        }
    }
}
/******************************************************************************
 End of function usb_pstd_set_configuration0
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_configuration3
 Description     : Analyze a Set Configuration command and process it. This is
 : for the status stage of a control write where there is no data
 : stage.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_configuration3 (void)
{
    uint16_t i;

    uint16_t ifc;

    uint16_t cfgok;
    uint16_t * table;
    uint8_t * table2;

    if ((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP) == USB_DEVICE)
    {

        cfgok = USB_ERROR;

        table2 = g_usb_pstd_driver.configtbl;

        if ((((g_usb_pstd_req_value == table2[5]) || (g_usb_pstd_req_value == 0)) && (g_usb_pstd_req_index == 0))
                && (g_usb_pstd_req_length == 0))
        {
            usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
            cfgok = USB_OK;

            if ((g_usb_pstd_req_value > 0) && (g_usb_pstd_req_value != g_usb_pstd_config_num))
            {
                usb_pstd_clr_eptbl_index();
                ifc = usb_pstd_get_interface_num(g_usb_pstd_req_value);
                for (i = 0; i < ifc; ++i)
                {
                    /* Pipe Information Table ("endpoint table") initialize */
                    usb_pstd_set_eptbl_index(g_usb_pstd_req_value, i, (uint16_t) 0);
                }
                table = g_usb_pstd_driver.pipetbl;
                /* Clear pipe configuration register */
                usb_pstd_set_pipe_register((uint16_t) USB_CLRPIPE, table);
                /* Set pipe configuration register */
                usb_pstd_set_pipe_register((uint16_t) USB_PERIPIPE, table);
            }
        }
        if (cfgok == USB_ERROR)
        {
            /* Request error */
            usb_pstd_set_stall_pipe0();
        }
    }
    else
    {
        /* Request error */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_set_configuration3
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_interface0
 Description     : Call callback function to notify reception of SetInterface com-
 : mand. For idle/setup stage.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_interface0 (void)
{
    if (USB_NULL != g_usb_pstd_driver.interface)
    {
        /* Interfaced change function call */
        (*g_usb_pstd_driver.interface)(USB_NULL, g_usb_pstd_alt_num[g_usb_pstd_req_index], USB_NULL);
    }
}
/******************************************************************************
 End of function usb_pstd_set_interface0
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_set_interface3
 Description     : Analyze a Set Interface command and request the process for
 : the command. This is for a status stage of a control write
 : where there is no data stage.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_set_interface3 (void)
{
    uint16_t * table;

    /* Configured ? */
    if ((usb_pstd_chk_configured() == USB_TRUE) && ((g_usb_pstd_req_type & USB_BMREQUESTTYPERECIP) == USB_INTERFACE))
    {
        if ((g_usb_pstd_req_index <= usb_pstd_get_interface_num(g_usb_pstd_config_num)) && (g_usb_pstd_req_length == 0))
        {
            if (g_usb_pstd_req_value <= usb_pstd_get_alternate_num(g_usb_pstd_config_num, g_usb_pstd_req_index))
            {
                g_usb_pstd_alt_num[g_usb_pstd_req_index] = (uint16_t) (g_usb_pstd_req_value & USB_ALT_SET);
                usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);
                usb_pstd_clr_eptbl_index();
                /* Search endpoint setting */
                usb_pstd_set_eptbl_index(g_usb_pstd_config_num, g_usb_pstd_req_index,
                        g_usb_pstd_alt_num[g_usb_pstd_req_index]);
                table = g_usb_pstd_driver.pipetbl;
                /* Set pipe configuration register */
                usb_pstd_set_pipe_register((uint16_t) USB_PERIPIPE, table);
            }
            else
            {
                /* Request error */
                usb_pstd_set_stall_pipe0();
            }
        }
        else
        {
            /* Request error */
            usb_pstd_set_stall_pipe0();
        }
    }
    else
    {
        /* Request error */
        usb_pstd_set_stall_pipe0();
    }
}
/******************************************************************************
 End of function usb_pstd_set_interface3
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_synch_rame1
 Description     : Return STALL response to SynchFrame command.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pstd_synch_rame1 (void)
{
    /* Set pipe USB_PID_STALL */
    usb_pstd_set_stall_pipe0();
}
/******************************************************************************
 End of function usb_pstd_synch_rame1
 ******************************************************************************/
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 End  Of File
 ******************************************************************************/
