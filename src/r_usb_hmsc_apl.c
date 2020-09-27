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
 * File Name    : r_usb_hmsc_apl.c
 * Description  : USB Host MSC application code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 30.09.2014 1.00    First Release
 *         : 30.01.2015 1.01    Added RX71M.
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_hmsc_apl.h"
#include "r_data_file.h"

#define USE_USB_IP (USB_IP0)

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
uint16_t g_apl_state[USB_MAXDEVADDR]; /* HMSC Sample application status */
uint16_t g_write_complete_flag[USB_MAXDEVADDR];

FATFS g_file_object[USB_MAXDEVADDR]; /* File system object structure */

uint8_t g_file_data[FILE_SIZE];

FATFS *g_pfat[] =
{ &g_file_object[0], /* Drive No. 0 */
  &g_file_object[1], /* Drive No. 1 */
  &g_file_object[2], /* Drive No. 2 */
  &g_file_object[3], /* Drive No. 3 */
};

uint8_t g_msc_file[MAX_DEVICE_NUM][15] =
{ "renesas.txt", /* Drive No. 0 */
};

static void R_error(uint8_t err_code);

/******************************************************************************
 Renesas Host MSC Sample Code functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_main
 Description     : Host MSC application main process
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_main (void)
{
    uint16_t event;
    uint8_t i;
    usb_ctrl_t ctrl;
    usb_cfg_t cfg;

    apl_init(); /* Host MSC Sample APL Initialize Command Send */

    ctrl.module = USE_USB_IP;
    ctrl.type = USB_HMSC;
    cfg.usb_speed = USB_FS;
    usb_board_init(&ctrl); /* CPU I/O Initialization process */

    LED0 = LED_ON;

    R_USB_Open(&ctrl, &cfg);
    while (1)
    {
        event = R_USB_GetEvent(&ctrl); /* Get event code */
        switch (event)
        {
            case USB_STS_CONFIGURED :
                msc_connect(&ctrl);
            break;
            case USB_STS_DETACH :
                msc_detach_device(ctrl.address);
            break;
            case USB_STS_NONE :
                for (i = 1; i < USB_MAXDEVADDR; i++)
                {
                    switch (g_apl_state[i])  /* Check application state */
                    {
                        case STATE_DATA_READY : /* Data transfer stand by state */
                        case STATE_DATA_WRITE : /* Data write state */
                            ctrl.address = i;
                            ctrl.module = USE_USB_IP;
                            ctrl.type = USB_HMSC;
                            msc_data_write(&ctrl);
                        break;
                        case STATE_DATA_READ : /* Read data state*/
                            ctrl.address = i;
                            ctrl.module = USE_USB_IP;
                            ctrl.type = USB_HMSC;
                            msc_data_read(&ctrl);
                            while (1)
                            {
                                /* Indicates that the User Sample software has finished the write/read operation successfully */
                                LED0 = 0;
                            }
                        break;
                        default :
                        break;
                    }
                }
            break;
            default :
            break;
        } /* switch( event ) */
    } /* while(1) */
} /* End of function usb_main */

/******************************************************************************
 Function Name   : msc_detach_device
 Description     : Detach process
 Arguments       : uint16_t  devadr  : Device Adrress
 Return value    : none
 ******************************************************************************/
void msc_detach_device (uint8_t adr)
{
    g_apl_state[adr] = STATE_DETACH; /* Set Application status  */
} /* End of function msc_detach_device */

/******************************************************************************
 Function Name   : msc_connect
 Description     : msc device connect process.
 Arguments       : uint16_t  devadr  : Device Adrress
 Return value    : LCD update request(USB_TRUE/USB_FALSE)
 ******************************************************************************/
void msc_connect (usb_ctrl_t *p_ctrl)
{
    uint16_t drv_no;

    drv_no = R_USB_HmscGerDriveNo(p_ctrl); /* refer to TFAT_DRIVE_ALLOC_NUM_** and TFAT_DRIVE_NUM_** */
    R_tfat_f_mount(drv_no, (FATFS *) g_pfat[drv_no]); /* Create a file object. */
    g_write_complete_flag[p_ctrl->address] = WRITE_NO;
    g_apl_state[p_ctrl->address] = STATE_DATA_READY; /* Change state : Drive -> Ready */
} /* End of function msc_connect */

/******************************************************************************
 Function Name   : msc_data_write
 Description     : USB receive state process
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void msc_data_write (usb_ctrl_t *p_ctrl)
{
    FIL file;
    uint16_t file_size = 0;
    uint16_t drv_no;
    FRESULT  res;

    drv_no = R_USB_HmscGerDriveNo(p_ctrl);

    /* Create a file object. */
    res = R_tfat_f_open(&file, (const char *) &g_msc_file[drv_no][0], (TFAT_FA_CREATE_ALWAYS | TFAT_FA_WRITE));
    if( res != TFAT_FR_OK )
    {
       R_error(1);
    }

    /* Write the data-set to a file. */
    res = R_tfat_f_write(&file, filebuffer, sizeof(filebuffer), &file_size);
    if( res != TFAT_FR_OK )
    {
       R_error(2);
    }

    res = R_tfat_f_close(&file); /* Close the file object. */
    if( res != TFAT_FR_OK )
    {
       R_error(2);
    }

    g_apl_state[p_ctrl->address] = STATE_DATA_READ; /* Set Application status  */
    g_write_complete_flag[p_ctrl->address] = WRITE_YES;
} /* End of function msc_data_write */

/******************************************************************************
 Function Name   : msc_data_read
 Description     : USB receive state process
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void msc_data_read (usb_ctrl_t *p_ctrl)
{
    FIL file;
    uint16_t file_size = 0;
    uint16_t drv_no;
    int32_t  byte_count = 0;
	uint16_t data_count = 0;
    FRESULT  res;

    drv_no = R_USB_HmscGerDriveNo(p_ctrl);

    /* File open in read mode */
    R_tfat_f_open(&file, (const char *) &g_msc_file[drv_no][0], (TFAT_FA_OPEN_ALWAYS | TFAT_FA_READ));
    if( res != TFAT_FR_OK )
    {
        R_error(3);
    }

    R_tfat_f_lseek(&file, 0); /* Move pointer in file object. */
    if( res != TFAT_FR_OK )
    {
    }

    /* Repeat the read process until required no. of bytes are read */
    while (byte_count < (sizeof(filebuffer) - 1))
    {

        /* Read the file. 512 bytes at a time. */
        res = R_tfat_f_read(&file, (void*)g_file_data, sizeof(g_file_data), &file_size);

        /* Check if the read operation was successful & the no. of bytes read
               by the function matches with the size of the R/W buffer */
        if (res != TFAT_FR_OK || file_size < sizeof(g_file_data))
        {
            R_error(4);
        }
        else
        {
            /* Compare the contents of the file with the ROM data */
            for (data_count = 0 ; data_count < sizeof(g_file_data) ; data_count++)
            {
                if (g_file_data[data_count] != filebuffer[byte_count + data_count])
                {
                    /* error indication */
                    /* explicit error data, address */
                    R_error(5);
                }
            }
            /* verify complete */
            /* ================== */
        }
        /* Update the counter with the total no. of bytes read from the file */
        byte_count += file_size;
    }

    res = R_tfat_f_close(&file); /* Close the file object. */
    if( res != TFAT_FR_OK )
    {
    }

} /* End of function msc_data_read */

/******************************************************************************
 Initialization functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_mcu_init
 Description     : USB pin function and port mode setting.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_mcu_init (void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    PORT1.PMR.BIT.B4 = 1u; /* P14 set USB0_OVRCURA */
    PORT1.PMR.BIT.B6 = 1u; /* P24 set USB0_VBUSEN */
    MPC.P14PFS.BYTE = 0x12; /* USB0_OVRCURA */
    MPC.P16PFS.BYTE = 0x12; /* USB0_VBUSEN */

#endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
#if defined(BSP_MCU_RX65N)
    PORT1.PMR.BIT.B4 = 1u; /* P14 set USB0_OVRCURA */
    PORT2.PMR.BIT.B4 = 1u; /* P24 set USB0_VBUSEN */
    MPC.P14PFS.BYTE = 0x12; /* USB0_OVRCURA */
    MPC.P24PFS.BYTE = 0x13; /* USB0_VBUSEN */

#endif  /* defined(BSP_MCU_RX65N) */
#if defined(BSP_MCU_RX63N)
    PORT1.PMR.BIT.B4 = 1u; /* P14 set USB0_OVRCURA */
    PORT1.PMR.BIT.B6 = 1u; /* P16 set USB0_VBUSEN */
    MPC.P14PFS.BYTE = 0x12; /* USB0_OVRCURA */
    MPC.P16PFS.BYTE = 0x12; /* USB0_VBUSEN */

#endif  /* defined(BSP_MCU_RX63N) */

#if USB_NUM_USBIP == 2
    #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    PORT1.PMR.BIT.B0 = 1u; /* P10 set USBHS_OVRCURA */
    PORT1.PMR.BIT.B1 = 1u; /* P11 set USBHS_VBUSEN */
    MPC.P10PFS.BYTE = 0x15; /* USBHS_OVRCURA */
    MPC.P11PFS.BYTE = 0x15; /* USBHS_VBUSEN */

    #endif  /* defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    #if defined(BSP_MCU_RX65N)
    PORT1.PMR.BIT.B0 = 1u; /* P10 set USBHS_OVRCURA */
    PORT1.PMR.BIT.B1 = 1u; /* P11 set USBHS_VBUSEN */
    MPC.P10PFS.BYTE = 0x15; /* USBHS_OVRCURA */
    MPC.P11PFS.BYTE = 0x15; /* USBHS_VBUSEN */

    #endif  /* defined(BSP_MCU_RX65N) */
    #if defined(BSP_MCU_RX63N)
    PORT1.PMR.BIT.B0 = 1u; /* P10 set USBHS_OVRCURA */
    PORT1.PMR.BIT.B1 = 1u; /* P11 set USBHS_VBUSEN */
    MPC.P10PFS.BYTE = 0x15; /* USBHS_OVRCURA */
    MPC.P11PFS.BYTE = 0x15; /* USBHS_VBUSEN */

    #endif  /* defined(BSP_MCU_RX63N) */
#endif  /* USB_NUM_USBIP == 2 */

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
} /* End of function usb_mcu_init */

/******************************************************************************
 Function Name   : usb_board_init
 Description     : CPU I/O Initialization process
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_board_init (usb_ctrl_t *p_ctrl)
{
#if USB_CFG_DTCDMA == USB_CFG_ENABLE
    dmaca_return_t ret;

    R_DMACA_Init();

    if (USB_IP0 == p_ctrl->module)
    {
        ret = R_DMACA_Int_Callback(USB_DMA_CH, (void *) usb_usb0_d0fifo_isr);
    }
    else
    {
        ret = R_DMACA_Int_Callback(USB_DMA_CH, (void *) usb_usb2_d0fifo_isr);
    }
    if (DMACA_SUCCESS != ret)
    {
        /* do something */
    }

    ret = R_DMACA_Int_Enable(USB_DMA_CH, USB_DMA_CH_PRI);
    if (DMACA_SUCCESS != ret)
    {
        /* do something */
    }

    ret = R_DMACA_Open(USB_DMA_CH);

    if (USB_IP0 == p_ctrl->module)
    {
        ret = R_DMACA_Int_Callback(USB_DMA_CH2, (void *) usb_usb0_d1fifo_isr);
    }
    else
    {
        ret = R_DMACA_Int_Callback(USB_DMA_CH2, (void *) usb_usb2_d1fifo_isr);
    }
    if (DMACA_SUCCESS != ret)
    {
        /* do something */
    }

    ret = R_DMACA_Int_Enable(USB_DMA_CH2, USB_DMA_CH2_PRI);
    if (DMACA_SUCCESS != ret)
    {
        /* do something */
    }

    ret = R_DMACA_Open(USB_DMA_CH2);

#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */
    usb_mcu_init(); /* USB pin function and port mode setting. */
} /* End of function usb_board_init */

/******************************************************************************
 Function Name   : apl_init
 Description     : Host MSC Sample APL Initialize Command Send
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void apl_init (void)
{
    uint16_t i;

    /* Initialize msc device state */
    for (i = 0; i < USB_MAXDEVADDR; i++)
    {
        g_write_complete_flag[i] = WRITE_NO;
        g_apl_state[i] = STATE_DETACH; /* Set Application status  */
    }
    for (i = 0; i < FILE_SIZE; ++i) /* Create a data set to write to file. */
    {
        g_file_data[i] = FILE_DATA; /* 512 'a'. */
    }
} /* End of function apl_init */

/*******************************************************************************
* Declaration  : static void R_error(uint8_t err_code)
* Function Name: R_error
* Description  : Error handling function
* Arguments    : Error code
* Return Value : none
*******************************************************************************/
static void R_error(uint8_t err_code)
{
	uint8_t	i= 1;

	switch (err_code)
	{
		case 1:
			/* Error while opening the file for writing */
			while (i);
			break;
		case 2:
			/* Write function was not successful or all the bytes were not written to the file */
			while (i);
			break;
		case 3:
			/* Error while opening the file for reading */
			while (i);
			break;
		case 4:
			/* Error during the file read operation	*/
			while (i);
			break;
		case 5:
			/* File written on the CARD does not match filebuffer[] contents */
			while (i);
			break;
		case 6:
			/* R_tfat_f_mount function failed */
			while (i);
			break;
	}
}
/******************************************************************************
 Other functions
 ******************************************************************************/

/******************************************************************************
 End  Of File
 ******************************************************************************/
