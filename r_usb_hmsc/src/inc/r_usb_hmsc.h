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
 * File Name    : r_usb_hmsc_local.h
 * Description  : USB Host MSC header file
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 04.01.2014 1.00 First Release
 *         : 30.01.2015 1.01    Support Multi device.
 ***********************************************************************************************************************/

#ifndef __R_USB_HMSC_LOCAL_H__
    #define __R_USB_HMSC_LOCAL_H__

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
    #include "r_usb_typedef.h"      /* type define */

/******************************************************************************
 Enum define
 ******************************************************************************/
/* ERROR CODE */
enum g_usb_hmsc_Error
{
    USB_HMSC_OK = (uint16_t) 0, USB_HMSC_STALL = (uint16_t) -1, USB_HMSC_CBW_ERR = (uint16_t) -2, /* CBW error */
    USB_HMSC_DAT_RD_ERR = (uint16_t) -3, /* Data IN error */
    USB_HMSC_DAT_WR_ERR = (uint16_t) -4, /* Data OUT error */
    USB_HMSC_CSW_ERR = (uint16_t) -5, /* CSW error */
    USB_HMSC_CSW_PHASE_ERR = (uint16_t) -6, /* Phase error */
    USB_HMSC_SUBMIT_ERR = (uint16_t) -7, /* Submit error */
};

/* CSW STATUS */
enum usb_gcmsc_CswSts
{
    USB_MSC_CSW_OK = (uint16_t) 0x00, USB_MSC_CSW_NG = (uint16_t) 0x01, USB_MSC_CSW_PHASE_ERR = (uint16_t) 0x02
};

enum
{
    USB_ATAPI_SUCCESS = 0x11,
    /* Command receive events */
    USB_ATAPI_NO_DATA = 0x21,
    USB_ATAPI_A_SND_DATA = 0x22,
    USB_ATAPI_A_RCV_DATA = 0x23,
    USB_ATAPI_SND_DATAS = 0x24,
    USB_ATAPI_RCV_DATAS = 0x25,
    USB_ATAPI_NOT_SUPPORT = 0x26,
    /* Complete events */
    USB_ATAPI_CMD_CONTINUE = 0x31,
    USB_ATAPI_CMD_COMPLETE = 0x32,
    USB_ATAPI_CMD_FAILED = 0x33,
    /* ATAPI Start events */
    USB_ATAPI_READY = 0x41,
    // respond error
    USB_ATAPI_ERROR = 0x51,
    /*** ERR CODE ***/
    USB_ATAPI_ERR_CODE_SEPARATER = 0x100,
    USB_ATAPI_ERR_INVAL = 0x61
};

/*****************************************************************************
 Macro definitions
 ******************************************************************************/
/* Version Number of API. */
    #define RX_USB_HMSC_API_VERSION_MAJOR     (1)
    #define RX_USB_HMSC_API_VERSION_MINOR     (10)

/* Host Sample Task */
    #define USB_HMSC_TSK        (USB_TID_4)           /* Task ID */
    #define USB_HMSC_MBX        (USB_HMSC_TSK)        /* Mailbox ID */
    #define USB_HMSC_MPL        (USB_HMSC_TSK)        /* Memorypool ID */

/* Host Sample Task */
    #define USB_HSTRG_TSK       (USB_TID_5)           /* Task ID */
    #define USB_HSTRG_MBX       (USB_HSTRG_TSK)       /* Mailbox ID */
    #define USB_HSTRG_MPL       (USB_HSTRG_TSK)       /* Memorypool ID */

    #define USB_MAXSTRAGE       (4)

/* CBW definitions */
    #define     USB_MSC_CBWLENGTH       (31)
    #define     USB_MSC_CBWCB_LENGTH    (12)
/* CPU bit endian select (BIT_LITTLE:little, BIT_BIG:big) */
    #if USB_CFG_ENDIAN == USB_CFG_BIG
        #define    USB_MSC_CBW_SIGNATURE   ((uint32_t)0x55534243)
    #else   /* USB_CFG_ENDIAN == USB_CFG_BIG */
        #define    USB_MSC_CBW_SIGNATURE   ((uint32_t)0x43425355)
    #endif  /* USB_CFG_ENDIAN == USB_CFG_BIG */

/* CSW definitions */
    #define     USB_MSC_CSW_LENGTH      (13)
/* CPU bit endian select (BIT_LITTLE:little, BIT_BIG:big) */
    #if USB_CFG_ENDIAN == USB_CFG_BIG
        #define    USB_MSC_CSW_SIGNATURE   ((uint32_t)0x55534253)
    #else   /* USB_CFG_ENDIAN == USB_CFG_BIG */
        #define    USB_MSC_CSW_SIGNATURE   ((uint32_t)0x53425355)
    #endif  /* USB_CFG_ENDIAN == USB_CFG_BIG */

/* subClass code */
    #define USB_ATAPI_MMC5                      ((uint8_t)0x02)
    #define USB_ATAPI                           ((uint8_t)0x05)
    #define USB_SCSI                            ((uint8_t)0x06)

/* Protocol code */
    #define USB_BOTP                            ((uint8_t)0x50)

/* Message code */
    #define USB_MSG_HMSC_NO_DATA                ((uint16_t)0x501)
    #define USB_MSG_HMSC_DATA_IN                ((uint16_t)0x502)
    #define USB_MSG_HMSC_DATA_OUT               ((uint16_t)0x503)
    #define USB_MSG_HMSC_CSW_PHASE_ERR          ((uint16_t)0x505)
    #define USB_MSG_HMSC_CBW_ERR                ((uint16_t)0x506)

    #define USB_MSG_HMSC_STRG_DRIVE_SEARCH      ((uint16_t)0x601)
    #define USB_MSG_HMSC_STRG_DRIVE_SEARCH_END  ((uint16_t)0x602)
    #define USB_MSG_HMSC_STRG_USER_COMMAND      ((uint16_t)0x605)
    #define USB_MSG_HMSC_STRG_RW_END            ((uint16_t)0x606)

/* Class Request Buffer Size */
    #define USB_HMSC_CLSDATASIZE                (256)

    #define USB_MEDIA_UNLOCK    (0u)          /* Media unlock */
    #define USB_MEDIA_LOCK      (1u)          /* Media Lock */

/*****************************************************************************
 Typedef definitions
 ******************************************************************************/
/* CBW Structure define.    */
typedef struct
{
    uint32_t    dcbw_signature;
    uint32_t    dcbw_tag;
    uint8_t     dcbw_dtl_lo;
    uint8_t     dcbw_dtl_ml;
    uint8_t     dcbw_dtl_mh;
    uint8_t     dcbw_dtl_hi;
    struct
    {
        uint8_t reserved7 :7;
        uint8_t cbw_dir :1;
    } bm_cbw_flags;
    struct
    {
        uint8_t bcbw_lun :4;
        uint8_t reserved4 :4;
    } bcbw_lun;
    struct
    {
        uint8_t bcbwcb_length :5;
        uint8_t reserved3 :3;
    } bcbwcb_length;
//    uint8_t cbwcb[(16 + (512-31))];     /* DTC Receive Size dummy (MAX Packet size = 512Byte) */
    uint8_t    cbwcb[(16 + (64 - 31))]; /* DTC Receive Size dummy (MAX Packet size = 64Byte) */
} usb_msc_cbw_t;

/* CSW Structure define define. */
typedef struct
{
    uint32_t dcsw_signature;
    uint32_t dcsw_tag;
    uint8_t  dcsw_data_residue_lo;
    uint8_t  dcsw_data_residue_ml;
    uint8_t  dcsw_data_residue_mh;
    uint8_t  dcsw_data_residue_hi;
    uint8_t  dcsw_status;
 //   uint8_t     dummy[(512-13)];        /* DTC Receive Size dummy (MAX Packet size = 512Byte) */
    uint8_t  dummy[(64 - 13)]; /* DTC Receive Size dummy (MAX Packet size = 64Byte) */
} usb_msc_csw_t;

/* Structure for DRIVE No. */
typedef struct DRIVE_MANAGEMENT
{
    uint16_t use_flag; /* USE flag */
    uint16_t ip; /* IP No. */
    uint16_t devadr; /* Device address */
} drive_management_t;

/*****************************************************************************
 Public Valiables
 ******************************************************************************/
/* HMSC driver */
extern usb_utr_t g_usb_hmsc_class_control[USB_NUM_USBIP];
extern uint8_t g_usb_hmsc_class_data[USB_NUM_USBIP][USB_HMSC_CLSDATASIZE];
extern drive_management_t g_usb_hmsc_drvno_tbl[USB_MAXSTRAGE];
extern uint16_t g_usb_hmsc_in_pipe[USB_NUM_USBIP][USB_MAXSTRAGE];
extern uint16_t g_usb_hmsc_out_pipe[USB_NUM_USBIP][USB_MAXSTRAGE];
extern usb_msc_cbw_t g_usb_hmsc_cbw[USB_NUM_USBIP][USB_MAXSTRAGE];
extern usb_msc_csw_t g_usb_hmsc_csw[USB_NUM_USBIP][USB_MAXSTRAGE];
extern uint32_t g_usb_hmsc_csw_tag_no[USB_NUM_USBIP][USB_MAXSTRAGE];
extern uint8_t g_usb_hmsc_sub_class[USB_NUM_USBIP][USB_MAXSTRAGE];
extern uint8_t g_usb_hmsc_tmp_sub_class[USB_NUM_USBIP];
extern uint8_t *g_usb_hmsc_device_table[USB_NUM_USBIP];
extern uint8_t *g_usb_hmsc_config_table[USB_NUM_USBIP];
extern uint8_t *g_usb_hmsc_interface_table[USB_NUM_USBIP];
extern uint16_t g_usb_hmsc_devaddr[USB_NUM_USBIP];
extern uint16_t g_usb_hmsc_init_seq[USB_NUM_USBIP];

extern uint16_t g_usb_hmsc_def_ep_tbl[USB_NUM_USBIP][USB_EPL + 1];
extern uint16_t g_usb_hmsc_speed[USB_NUM_USBIP];
extern uint16_t g_usb_hmsc_tmp_ep_tbl[USB_NUM_USBIP][USB_MAXSTRAGE][2][USB_EPL];
extern uint16_t g_usb_hmsc_in_pipectr[USB_NUM_USBIP][USB_MAXSTRAGE];
extern uint16_t g_usb_hmsc_out_pipectr[USB_NUM_USBIP][USB_MAXSTRAGE];

/* Storage Driver */
extern uint16_t g_usb_hmsc_strg_process[USB_NUM_USBIP];
extern uint16_t g_usb_hmsc_strg_drive_search_seq[USB_NUM_USBIP];


/*extern*/
uint16_t usb_hmsc_smp_pipe_info (usb_utr_t *ptr, uint8_t *table, uint16_t msgnum, uint16_t speed, uint16_t length);


/*****************************************************************************
 Public Functions
 ******************************************************************************/
/* HMSC driver */
void usb_hmsc_task (void);
void usb_hmsc_set_els_cbw (usb_utr_t *ptr, uint8_t *data, uint32_t trans_byte, uint16_t side);
void usb_hmsc_set_rw_cbw (usb_utr_t *ptr, uint16_t command, uint32_t secno, uint16_t seccnt, uint32_t trans_byte,
        uint16_t side);
void usb_hmsc_clr_data (uint16_t len, uint8_t *buf);
uint16_t usb_hmsc_no_data (usb_utr_t *ptr, uint16_t side);
uint16_t usb_hmsc_data_in (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t size);
uint16_t usb_hmsc_data_out (usb_utr_t *ptr, uint16_t side, uint8_t *buff, uint32_t size);

void usb_hmsc_smp_drive2_addr (uint16_t side, usb_utr_t *devadr);
void usb_hmsc_message_retry (uint16_t id, usb_utr_t *mess);

/* Storage Driver */
void usb_hmsc_strg_drive_search_act (usb_utr_t *mess);

#endif  /* __R_USB_HMSC_LOCAL_H__ */
/******************************************************************************
 End  Of File
 ******************************************************************************/
