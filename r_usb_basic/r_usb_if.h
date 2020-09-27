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
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_if.h
 * Description  : USB Host and Peripheral Driver Interface file
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 09.30.2016 1.00 First Release
 ***********************************************************************************************************************/

#ifndef __R_USB_IF_H__
    #define __R_USB_IF_H__

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
    #include <stdint.h>
    #include "platform.h"
    #include "r_usb_basic_define.h"
    #include "r_usb_basic_config.h"

/******************************************************************************
 Constant macro definitions
 ******************************************************************************/
    #define     USB_NULL        (0)

    #define     USB_IP0         (0)
    #define     USB_IP1         (1)

    #define     USB_PIPE0       (0)
    #define     USB_PIPE1       (1)
    #define     USB_PIPE2       (2)
    #define     USB_PIPE3       (3)
    #define     USB_PIPE4       (4)
    #define     USB_PIPE5       (5)
    #define     USB_PIPE6       (6)
    #define     USB_PIPE7       (7)
    #define     USB_PIPE8       (8)
    #define     USB_PIPE9       (9)

/*****************************************************************************
 Typedef definitions
 ******************************************************************************/
typedef enum usb_err
{
    USB_SUCCESS = 0,
    USB_ERR_PARA,
    USB_ERR_BUSY,
    USB_ERR_NOT_OPEN,
    USB_ERR_NOT_SUSPEND,
    USB_ERR_OVER,
    USB_ERR_SHORT,
    USB_ERR_NG,
} usb_err_t;

enum usb_speed
{
    USB_LS = 0, USB_FS, USB_HS,
};

enum usb_setup_status
{
    USB_ACK = 0, USB_STALL
};

enum usb_status
{
    USB_STS_POWERED = 0,
    USB_STS_DEFAULT,
    USB_STS_ADDRESS,
    USB_STS_CONFIGURED,
    USB_STS_SUSPEND,
    USB_STS_RESUME,
    USB_STS_DETACH,
    USB_STS_REQUEST,
    USB_STS_REQUEST_COMPLETE,
    USB_STS_READ_COMPLETE,
    USB_STS_WRITE_COMPLETE,
    USB_STS_BC,
    USB_STS_OVERCURRENT,
    USB_STS_NONE,
    USB_STS_MSC_CMD_COMPLETE,
};

enum usb_class
{
    USB_PCDC = 0, USB_PCDCC, USB_PHID, USB_PVND,

    /* Host */
    USB_HCDC, USB_HCDCC, USB_HHID, USB_HVND,

    USB_HMSC, USB_PMSC, USB_REQUEST,
};

enum usb_bcport
{
    USB_SDP = 0, USB_CDP, USB_DCP,
};

enum usb_onoff
{
    USB_OFF = 0, USB_ON,
};

enum usb_rw
{
    USB_READ = 0, USB_WRITE,
};

enum usb_transfer
{
    USB_BULK = 0, USB_INT, USB_ISO,
};

typedef struct usb_descriptor
{
    uint8_t *device;
    uint8_t *config_f;
    uint8_t *config_h;
    uint8_t *qualifier;
    uint8_t **string;
} usb_descriptor_t;

typedef struct usb_cfg
{
    uint8_t          usb_speed;
    usb_descriptor_t *usb_reg;
} usb_cfg_t;

typedef struct usb_setup
{
    uint16_t type;
    uint16_t value;
    uint16_t index;
    uint16_t length;
} usb_setup_t;

typedef struct usb_ctrl
{
    uint8_t     module;
    uint8_t     address;
    uint8_t     pipe;
    uint8_t     type;
    uint32_t    size;
    uint8_t     status;
    usb_setup_t setup;
} usb_ctrl_t;

typedef struct usb_pipe
{
    uint8_t  ep;
    uint8_t  type;
    uint16_t mxps;
} usb_pipe_t;

typedef struct usb_info
{
    uint8_t type;
    uint8_t speed;
    uint8_t status;
    uint8_t port;
} usb_info_t;

/******************************************************************************
 Export global functions
 ******************************************************************************/
uint32_t R_USB_GetVersion (void);
usb_err_t R_USB_Open (usb_ctrl_t *, usb_cfg_t *);
usb_err_t R_USB_Close (usb_ctrl_t *);
usb_err_t R_USB_Read (usb_ctrl_t *, uint8_t *, uint32_t);
usb_err_t R_USB_Write (usb_ctrl_t *, uint8_t *, uint32_t);
usb_err_t R_USB_Stop (usb_ctrl_t *, uint16_t);
usb_err_t R_USB_Suspend (usb_ctrl_t *);
usb_err_t R_USB_Resume (usb_ctrl_t *);
usb_err_t R_USB_VbusSetting (usb_ctrl_t *, uint16_t);
usb_err_t R_USB_GetInformation (usb_ctrl_t *, usb_info_t *);
usb_err_t R_USB_PipeRead (usb_ctrl_t *, uint8_t *, uint32_t);
usb_err_t R_USB_PipeWrite (usb_ctrl_t *, uint8_t *, uint32_t);
usb_err_t R_USB_PipeStop (usb_ctrl_t *);
usb_err_t R_USB_GetUsePipe (usb_ctrl_t *, uint16_t *);
usb_err_t R_USB_GetPipeInfo (usb_ctrl_t *, usb_pipe_t *);
uint16_t R_USB_GetEvent (usb_ctrl_t *);
usb_err_t R_USB_HmscStrgCmd (usb_ctrl_t *p_ctrl, uint8_t *buf, uint16_t command);
uint8_t R_USB_HmscGerDriveNo (usb_ctrl_t *p_ctrl);
usb_err_t R_USB_HhidGetType (usb_ctrl_t *p_ctrl, uint8_t *p_type);

#endif  /* __R_USB_BASIC_IF_H__ */
