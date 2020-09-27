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
 * File Name    : r_usb_hdriver.c
 * Description  : USB Host Control Driver
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
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Static variables and functions
 ******************************************************************************/
static usb_hcdinfo_t *usb_shstd_hcd_msg;
static uint16_t usb_shstd_clr_stall_pipe;
static uint16_t usb_shstd_clr_stall_request[5];
static uint8_t usb_shstd_clr_stall_data[10];
static usb_utr_t usb_shstd_clr_stall_ctrl;
static usb_cb_t usb_shstd_clr_stall_call;

static usb_er_t usb_hstd_set_submitutr (usb_utr_t *ptr);
static void usb_hstd_set_retransfer (usb_utr_t *ptr, uint16_t pipe);
static void usb_hstd_clr_stall_result (usb_utr_t *ptr, uint16_t data1, uint16_t data2);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
/* Device driver (registration) */
usb_hcdreg_t g_usb_hstd_device_drv[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];

/* Root port, status, config num, interface class, speed, */
uint16_t g_usb_hstd_device_info[USB_NUM_USBIP][USB_MAXDEVADDR + 1u][8u];
uint16_t g_usb_hstd_remort_port[2u];

/* Control transfer stage management */
uint16_t g_usb_hstd_ctsq[USB_NUM_USBIP];

/* Manager mode */
uint16_t g_usb_hstd_mgr_mode[USB_NUM_USBIP][2u];

/* DEVSEL & DCPMAXP (Multiple device) */
uint16_t g_usb_hstd_dcp_register[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];

/* Device address */
uint16_t g_usb_hstd_device_addr[USB_NUM_USBIP];

/* Reset handshake result */
uint16_t g_usb_hstd_device_speed[USB_NUM_USBIP];

/* Device driver number */
uint16_t g_usb_hstd_device_num[USB_NUM_USBIP];

/* Ignore count */
uint16_t g_usb_hstd_ignore_cnt[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];

usb_ctrl_t gctrl;

/******************************************************************************
 External variables and functions
 ******************************************************************************/
/*uint16_t        usb_hstd_cmd_submit( usb_utr_t *ptr, usb_cb_t complete);*/

    #if USB_CFG_COMPLIANCE == USB_CFG_ENABLE
extern uint16_t g_usb_hstd_responce_counter;
    #endif /* USB_CFG_COMPLIANCE == USB_CFG_ENABLE */

/******************************************************************************
 Renesas USB Host Driver functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_dev_descriptor
 Description     : Returns buffer header pointer to fetch device descriptor.
 Argument        : none
 Return          : uint8_t *                 : Device Descriptor Pointer
 ******************************************************************************/
uint8_t *usb_hstd_dev_descriptor (usb_utr_t *ptr)
{
    return (uint8_t *) &g_usb_hstd_device_descriptor[ptr->ip];
}/* End of function usb_hstd_dev_descriptor() */

/******************************************************************************
 Function Name   : usb_hstd_con_descriptor
 Description     : Returns buffer header pointer that includes the configuration 
 : descriptor.
 Argument        : none
 Return          : uint8_t *                 : Configuration Descriptor Pointer
 ******************************************************************************/
uint8_t *usb_hstd_con_descriptor (usb_utr_t *ptr)
{
    return (uint8_t *) &g_usb_hstd_config_descriptor[ptr->ip];
}/* End of function usb_hstd_con_descriptor() */

/******************************************************************************
 Function Name   : usb_hstd_change_device_state
 Description     : Request to change status of the connected USB device.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : usb_cb_t complete : callback function.
 : uint16_t msginfo  : Request type.
 : uint16_t member   : Rootport/pipe number.
 Return          : usb_er_t          : USB_OK etc.
 ******************************************************************************/
usb_er_t usb_hstd_change_device_state (usb_utr_t *ptr, usb_cb_t complete, uint16_t msginfo, uint16_t member)
{
    usb_er_t err;

    err = usb_hstd_hcd_snd_mbx(ptr, msginfo, member, (uint16_t*) 0, complete);

    return err;
}/* End of function usb_hstd_change_device_state() */

/******************************************************************************
 Function Name   : usb_hstd_transfer_start
 Description     : Send a request for data transfer to HCD (Host Control Driver) 
 : using the specified pipe.
 Arguments       : usb_utr_t *ptr : USB internal structure. Contains message.
 Return          : usb_er_t                  : USB_OK/USB_QOVR/USB_ERROR
 ******************************************************************************/
usb_er_t usb_hstd_transfer_start (usb_utr_t *ptr)
{
    usb_er_t err;
    uint16_t pipenum;
    uint16_t devsel;
    uint16_t connect_inf;

    pipenum = ptr->keyword;

    /* Pipe Transfer Process check */
    if (USB_NULL != g_usb_hstd_pipe[ptr->ip][pipenum])
    {
        /* Check PIPE TYPE */
        if (usb_cstd_get_pipe_type(ptr, pipenum) != USB_TYPFIELD_ISO)
        {
            USB_PRINTF1("### usb_hstd_transfer_start overlaps %d\n", pipenum);
            return USB_QOVR;
        }
    }

    if (USB_PIPE0 == pipenum)
    {
        devsel = (uint16_t) (ptr->setup[4] << USB_DEVADDRBIT);
    }
    else
    {
        /* Get device address from pipe number */
        devsel = usb_hstd_get_devsel(ptr, pipenum);
    }
    if ((USB_DEVICE_0 == devsel) && (USB_PIPE0 != pipenum))
    {
        USB_PRINTF1("### usb_hstd_transfer_start not configured %x\n", devsel);
        return USB_ERROR;
    }

    /* Get device speed from device address */
    connect_inf = usb_hstd_get_dev_speed(ptr, devsel);
    if (USB_NOCONNECT == connect_inf)
    {
        USB_PRINTF1("### usb_hstd_transfer_start not connect %x\n", devsel);
        return USB_ERROR;
    }

    ptr->msghead = (usb_mh_t) USB_NULL;
    ptr->msginfo = USB_MSG_HCD_SUBMITUTR;

    /* Send message */
    err = USB_SND_MSG(USB_HCD_MBX, (usb_msg_t* )ptr);
    if (USB_OK != err)
    {
        USB_PRINTF1("### usb_hstd_transfer_start snd_msg error (%ld)\n", err);
    }
    return err;
}/* End of function usb_hstd_transfer_start() */

/******************************************************************************
 Function Name   : usb_hstd_device_resume
 Description     : Send request for RESUME signal output to USB device to MGR task.
 Argument        : uint16_t devaddr          : Device Address
 Return          : none
 ******************************************************************************/
void usb_hstd_device_resume (usb_utr_t *ptr, uint16_t devaddr)
{
    usb_hstd_mgr_snd_mbx(ptr, (uint16_t) USB_MSG_HCD_RESUME, devaddr, (uint16_t) 0u);
}/* End of function usb_hstd_device_resume() */

/******************************************************************************
 Function Name   : usb_hstd_hcd_snd_mbx
 Description     : Send specified message to HCD (Host Control Driver) task.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t  msginfo : Message info..
 : uint16_t  dat     : Pipe no.
 : uint16_t  *adr    : Address.
 : usb_cb_t  callback: Callback function pointer.
 Return          : usb_er_t          : USB_OK etc.
 ******************************************************************************/
usb_er_t usb_hstd_hcd_snd_mbx (usb_utr_t *ptr, uint16_t msginfo, uint16_t dat, uint16_t *adr, usb_cb_t callback)
{
    usb_mh_t p_blf;
    usb_er_t err;
    usb_er_t err2;
    usb_hcdinfo_t *hp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_HCD_MPL, &p_blf);
    if (USB_OK == err)
    {
        hp = (usb_hcdinfo_t*) p_blf;
        hp->msghead = (usb_mh_t) USB_NULL;
        hp->msginfo = msginfo;
        hp->keyword = dat;
        hp->tranadr = adr;
        hp->complete = callback;
        hp->ipp = ptr->ipp;
        hp->ip = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_HCD_MBX, (usb_msg_t* )p_blf);
        if (USB_OK != err)
        {
            USB_PRINTF1("### hHcdSndMbx snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_HCD_MPL, (usb_mh_t )p_blf);
            if (USB_OK != err2)
            {
                USB_PRINTF1("### hHcdSndMbx rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hHcdSndMbx pget_blk error (%ld)\n", err);
    }
    return err;
}/* End of function usb_hstd_hcd_snd_mbx() */

/******************************************************************************
 Function Name   : usb_hstd_mgr_snd_mbx
 Description     : Send the message to MGR(Manager) task
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t msginfo  : Message info.
 : uint16_t dat      : Port no.
 : uint16_t res               : Result
 Return          : none
 ******************************************************************************/
void usb_hstd_mgr_snd_mbx (usb_utr_t *ptr, uint16_t msginfo, uint16_t dat, uint16_t res)
{
    usb_mh_t p_blf;
    usb_er_t err;
    usb_er_t err2;
    usb_mgrinfo_t *mp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_MGR_MPL, &p_blf);
    if (USB_OK == err)
    {
        mp = (usb_mgrinfo_t *) p_blf;
        mp->msghead = (usb_mh_t) USB_NULL;
        mp->msginfo = msginfo;
        mp->keyword = dat;
        mp->result = res;
        mp->ipp = ptr->ipp;
        mp->ip = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_MGR_MBX, (usb_msg_t * )p_blf);
        if (USB_OK != err)
        {
            USB_PRINTF1("### hMgrSndMbx snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_MGR_MPL, (usb_mh_t )p_blf);
            if (USB_OK != err2)
            {
                USB_PRINTF1("### hMgrSndMbx rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hMgrSndMbx pget_blk error (%ld)\n", err);
    }
}/* End of function usb_hstd_mgr_snd_mbx */

/******************************************************************************
 Function Name   : usb_hstd_hcd_rel_mpl
 Description     : Release the secured memory block.
 Argument        : uint16_t n        : Error no.
 Return          : none
 ******************************************************************************/
void usb_hstd_hcd_rel_mpl (usb_utr_t *ptr, uint16_t n)
{
    usb_er_t err;

    /* Memory Pool Release */
    err = USB_REL_BLK(USB_HCD_MPL, (usb_mh_t )ptr);
    if (USB_OK != err)
    {
        USB_PRINTF1("### USB HCD rel_blk error: %d\n", n);
    }
}/* End of function usb_hstd_hcd_rel_mpl() */

/******************************************************************************
 Function Name   : usb_hstd_suspend
 Description     : Request suspend for USB device.
 Argument        : uint16_t port     : Port no.
 Return          : none
 ******************************************************************************/
void usb_hstd_suspend (usb_utr_t *ptr, uint16_t port)
{
    usb_hcdinfo_t* hp;

    /* Control transfer idle stage ? */
    if (USB_IDLEST == g_usb_hstd_ctsq[ptr->ip])
    {
        /* USB suspend process */
        usb_hstd_suspend_process(ptr, port);
        usb_hstd_chk_clk(ptr, port, (uint16_t) USB_SUSPENDED); /* Check clock */
        hp = (usb_hcdinfo_t*) ptr; /* Callback */
        (hp->complete)(ptr, port, ptr->msginfo);
    }
    else
    {
        /* 1ms wait */
        usb_cpu_delay_xms((uint16_t) 1);

        /* Change device state request */
        usb_hstd_change_device_state(ptr, &usb_hstd_status_result, ptr->msginfo, port);
    }
}/* End of function usb_hstd_suspend() */

/******************************************************************************
 Function Name   : usb_hstd_set_submitutr
 Description     : Submit utr: Get the device address via the specified pipe num-
 : ber and do a USB transfer.
 Arguments       : usb_utr_t *ptr : USB system internal structure. Also used in 
 : this function to get device address, and specifies keyword and
 : USB channel.
 Return          : usb_er_t                      : USB_OK
 ******************************************************************************/
static usb_er_t usb_hstd_set_submitutr (usb_utr_t *ptr)
{
    uint16_t pipenum;
    uint16_t devsel;
    uint16_t connect_inf;
    uint16_t end_flag;
    usb_utr_t *pp;

    pipenum = ptr->keyword;
    g_usb_hstd_pipe[ptr->ip][pipenum] = ptr;

    /* Get device address from pipe number */
    if (USB_PIPE0 == pipenum)
    {
        devsel = (uint16_t) (ptr->setup[4] << USB_DEVADDRBIT);
    }
    else
    {
        /* Get device address from pipe number */
        devsel = usb_hstd_get_devsel(ptr, pipenum);
    }
    if ((USB_DEVICE_0 == devsel) && (USB_PIPE0 != pipenum))
    {
        /* End of data transfer (IN/OUT) */
        usb_hstd_forced_termination(ptr, pipenum, (uint16_t) USB_DATA_ERR);
        return USB_OK;
    }

    /* Get device speed from device address */
    connect_inf = usb_hstd_get_dev_speed(ptr, devsel);
    if (USB_NOCONNECT == connect_inf)
    {
        if (USB_PIPE0 == pipenum)
        {
            /* Control Read/Write End */
            usb_hstd_ctrl_end(ptr, (uint16_t) USB_DATA_ERR);
        }
        else
        {
            /* End of data transfer (IN/OUT) */
            usb_hstd_forced_termination(ptr, pipenum, (uint16_t) USB_DATA_ERR);
        }
        return USB_OK;
    }

    /* Control Transfer */
    if (USB_PIPE0 == pipenum)
    {
        /* Control transfer idle stage ? */
        if (USB_IDLEST == g_usb_hstd_ctsq[ptr->ip])
        {
            usb_hstd_setup_start(ptr);
        }

        /* Control Read Data */
        else if (USB_DATARDCNT == g_usb_hstd_ctsq[ptr->ip])
        {
            pp = g_usb_hstd_pipe[ptr->ip][USB_PIPE0];
            usb_hstd_ctrl_read_start(ptr, pp->tranlen, (uint8_t*) pp->tranadr); /* Control read start */
        }

        /* Control Write Data */
        else if (USB_DATAWRCNT == g_usb_hstd_ctsq[ptr->ip])
        {
            pp = g_usb_hstd_pipe[ptr->ip][USB_PIPE0];

            /* Control write start */
            end_flag = usb_hstd_ctrl_write_start(ptr, pp->tranlen, (uint8_t*) pp->tranadr);
            if (USB_FIFOERROR == end_flag)
            {
                USB_PRINTF0("### FIFO access error \n");
                usb_hstd_ctrl_end(ptr, (uint16_t) USB_DATA_ERR); /* Control Read/Write End */
            }
        }
        else
        {
            USB_PRINTF0("### Control transfer seaquence error \n");

            /* Control Read/Write End */
            usb_hstd_ctrl_end(ptr, (uint16_t) USB_DATA_ERR);
        }
    }
    else
    {
        gctrl.pipe = pipenum;
        usb_hstd_set_retransfer(ptr, pipenum); /* Data Transfer */
    }
    return USB_OK;
}/* End of function usb_hstd_set_submitutr() */

/******************************************************************************
 Function Name   : usb_hstd_set_retransfer
 Description     : Start IN/OUT transfer based on the specified pipe.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t  pipe : Pipe number
 Return          : none
 ******************************************************************************/
static void usb_hstd_set_retransfer (usb_utr_t *ptr, uint16_t pipe)
{
    /* Data Transfer */
    if (usb_cstd_get_pipe_dir(ptr, pipe) == USB_DIR_H_IN)
    {
        /* IN Transfer */
        usb_hstd_receive_start(ptr, pipe);
    }
    else
    {
        /* OUT Transfer */
        usb_hstd_send_start(ptr, pipe);
    }
}/* End of function usb_hstd_set_retransfer() */

/******************************************************************************
 Function Name   : usb_hstd_bus_int_disable
 Description     : Disable USB Bus Interrupts OVRCR, ATTCH, DTCH, and BCHG.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t  port : Port number.  //$REA - redundant!
 Return          : none
 ******************************************************************************/
void usb_hstd_bus_int_disable (usb_utr_t *ptr, uint16_t port)
{
    /* ATTCH interrupt disable */
    usb_hstd_attch_disable(ptr, port);

    /* DTCH     interrupt disable */
    usb_hstd_dtch_disable(ptr, port);

    /* BCHG     interrupt disable */
    usb_hstd_bchg_disable(ptr, port);
}/* End of function usb_hstd_bus_int_disable() */

/******************************************************************************
 Function Name   : usb_hstd_interrupt
 Description     : Execute appropriate process depending on which USB interrupt 
 : occurred.
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 Return          : none
 ******************************************************************************/
static void usb_hstd_interrupt (usb_utr_t *ptr)
{
    uint16_t intsts;
    uint16_t end_flag;
    usb_utr_t *pp;

    intsts = ptr->keyword;

    /*    bitsts = ptr->status;*/
    switch (intsts)
    {

        /***** Processing PIPE0-MAX_PIPE_NO data *****/
        case USB_INT_BRDY :
            usb_hstd_brdy_pipe(ptr);
        break;
        case USB_INT_BEMP :
            usb_hstd_bemp_pipe(ptr);
        break;
        case USB_INT_NRDY :
            usb_hstd_nrdy_pipe(ptr);
        break;

            /***** Processing Setup transaction *****/
        case USB_INT_SACK :
            switch (g_usb_hstd_ctsq[ptr->ip])
            {
                case USB_SETUPRD :

                    /* Next stage to Control read data */
                    /* continue */
                case USB_SETUPRDCNT :

                    /* Next stage to Control read data */
                    pp = g_usb_hstd_pipe[ptr->ip][USB_PIPE0];

                    /* Control read start */
                    usb_hstd_ctrl_read_start(ptr, pp->tranlen, (uint8_t*) pp->tranadr);
                break;
                case USB_SETUPWR :

                    /* Next stage to Control Write data */
                    /* continue */
                case USB_SETUPWRCNT :

                    /* Next stage to Control Write data */
                    pp = g_usb_hstd_pipe[ptr->ip][USB_PIPE0];

                    /* Control write start */
                    end_flag = usb_hstd_ctrl_write_start(ptr, pp->tranlen, (uint8_t*) pp->tranadr);
                    if (USB_FIFOERROR == end_flag)
                    {
                        USB_PRINTF0("### FIFO access error \n");

                        /* Control Read/Write End */
                        usb_hstd_ctrl_end(ptr, (uint16_t) USB_DATA_ERR);
                    }
                break;
                case USB_SETUPNDC :

                    /* Next stage to Control write no data */
                    usb_hstd_status_start(ptr);
                break;
                default :
                break;
            }
        break;
        case USB_INT_SIGN :
            USB_PRINTF0("***SIGN\n");
    #if USB_CFG_COMPLIANCE == USB_CFG_ENABLE
//        USB_COMPLIANCE_DISP(ptr, USB_COMP_ERR,USB_NO_ARG);
            usb_compliance_disp( void * );
    #endif /* USB_CFG_COMPLIANCE == USB_CFG_ENABLE */
            /* Ignore count */
            g_usb_hstd_ignore_cnt[ptr->ip][USB_PIPE0]++;
            USB_PRINTF2("### IGNORE Pipe %d is %d times (Setup) \n", USB_PIPE0, g_usb_hstd_ignore_cnt[ptr->ip][USB_PIPE0]);
            if (USB_PIPEERROR == g_usb_hstd_ignore_cnt[ptr->ip][USB_PIPE0])
            {
                /* Setup Device Ignore count over */
                usb_hstd_ctrl_end(ptr, (uint16_t) USB_DATA_ERR);
            }
            else
            {
                /* Interrupt enable */
                /* 5ms wait */
                usb_cpu_delay_xms((uint16_t) 5u);

                /* Status Clear */
                HW_USB_HClearStsSign(ptr);
                HW_USB_HClearStsSack(ptr);

                /* Setup Ignore,Setup Acknowledge enable */
                HW_USB_HSetEnbSigne(ptr);
                HW_USB_HSetEnbSacke(ptr);

                /* SETUP request send */
                /* Send SETUP request */
                HW_USB_HSetSureq(ptr);
            }
        break;

            /***** Processing rootport0 *****/
        case USB_INT_OVRCR0 :

            /* Port0 OVCR interrupt function */
            usb_hstd_ovrcr0function(ptr);
        break;
        case USB_INT_EOFERR0 :

            /* User program */
        break;
        case USB_INT_ATTCH0 :

            /* Port0 ATCH interrupt function */
            usb_hstd_attach_process(ptr, (uint16_t) USB_PORT0);
        break;
        case USB_INT_BCHG0 :
            USB_PRINTF0("BCHG int port0\n");

            /* Port0 BCHG interrupt function */
            usb_hstd_bchg0function(ptr);
        break;
        case USB_INT_DTCH0 :
            USB_PRINTF0("DTCH int port0\n");

            /* USB detach process */
            usb_hstd_detach_process(ptr, (uint16_t) USB_PORT0);
        break;
    #if USB_CFG_BC == USB_CFG_ENABLE
            case USB_INT_PDDETINT0:
            /* Port0 PDDETINT interrupt function */
            if(USB_BC_SUPPORT_IP == ptr->ip)
            {
                usb_hstd_pddetint_process(ptr, (uint16_t)USB_PORT0);
            }
            break;
    #endif
        case USB_INT_VBINT :

            /* User program */
            HW_USB_ClearEnbVbse(ptr);
        break;
        case USB_INT_SOFR :
    #if USB_CFG_COMPLIANCE == USB_CFG_ENABLE
            g_usb_hstd_responce_counter++;
            if(g_usb_hstd_responce_counter == USB_RESPONCE_COUNTER_VALUE)
            {
                HW_USB_ClearEnbSofe( ptr );
//            USB_COMPLIANCE_DISP(ptr, USB_COMP_NOTRESP, USB_NO_ARG);
                usb_compliance_disp( void * );
                usb_hstd_ctrl_end(ptr, (uint16_t)USB_DATA_STOP);
            }
    #else  /* USB_CFG_COMPLIANCE == USB_CFG_ENABLE */
            /* User program */
            HW_USB_ClearEnbSofe(ptr);
    #endif /* USB_CFG_COMPLIANCE == USB_CFG_ENABLE */
        break;

            /*** ERROR ***/
        case USB_INT_UNKNOWN :
            USB_PRINTF0("hINT_UNKNOWN\n");
        break;
        default :
            USB_PRINTF1("hINT_default %X\n", intsts);
        break;
    }
}/* End of function usb_hstd_interrupt() */

/******************************************************************************
 Function Name   : usb_hstd_clr_feature
 Description     : Send ClearFeature command to the connected USB device.
 Arguments       : uint16_t addr     : Device address.
 : uint16_t epnum    : Endpoint number.
 : usb_cb_t complete : Callback function.
 Return value    : uint16_t          : Error info.
 ******************************************************************************/
usb_er_t usb_hstd_clr_feature (usb_utr_t *ptr, uint16_t addr, uint16_t epnum, usb_cb_t complete)
{
    usb_er_t ret_code;

    if (0xFF == epnum)
    {
        /* ClearFeature(Device) */
        usb_shstd_clr_stall_request[0] = USB_CLEAR_FEATURE | USB_HOST_TO_DEV | USB_STANDARD | USB_DEVICE;
        usb_shstd_clr_stall_request[1] = USB_DEV_REMOTE_WAKEUP;
        usb_shstd_clr_stall_request[2] = (uint16_t) 0x0000;
    }
    else
    {
        /* ClearFeature(endpoint) */
        usb_shstd_clr_stall_request[0] = USB_CLEAR_FEATURE | USB_HOST_TO_DEV | USB_STANDARD | USB_ENDPOINT;
        usb_shstd_clr_stall_request[1] = USB_ENDPOINT_HALT;
        usb_shstd_clr_stall_request[2] = epnum;
    }
    usb_shstd_clr_stall_request[3] = (uint16_t) 0x0000;
    usb_shstd_clr_stall_request[4] = addr;

    usb_shstd_clr_stall_ctrl.tranadr = (void*) usb_shstd_clr_stall_data;
    usb_shstd_clr_stall_ctrl.complete = complete;
    usb_shstd_clr_stall_ctrl.tranlen = (uint32_t) usb_shstd_clr_stall_request[3];
    usb_shstd_clr_stall_ctrl.keyword = USB_PIPE0;
    usb_shstd_clr_stall_ctrl.setup = usb_shstd_clr_stall_request;
    usb_shstd_clr_stall_ctrl.segment = USB_TRAN_END;

    usb_shstd_clr_stall_ctrl.ip = ptr->ip;
    usb_shstd_clr_stall_ctrl.ipp = ptr->ipp;

    ret_code = usb_hstd_transfer_start(&usb_shstd_clr_stall_ctrl);

    return ret_code;
}/* End of function usb_hstd_clr_feature() */

/******************************************************************************
 Function Name   : usb_hstd_clr_stall
 Description     : Clear Stall
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t pipe      : Pipe number.
 : usb_cb_t complete        : Callback function
 Return value    : uint16_t           : Error info.
 ******************************************************************************/
usb_er_t usb_hstd_clr_stall (usb_utr_t *ptr, uint16_t pipe, usb_cb_t complete)
{
    usb_er_t err;
    uint8_t dir_ep;
    uint16_t devsel;

    dir_ep = usb_hstd_pipe_to_epadr(ptr, pipe);
    devsel = usb_hstd_get_device_address(ptr, pipe);

    err = usb_hstd_clr_feature(ptr, (uint16_t) (devsel >> USB_DEVADDRBIT), (uint16_t) dir_ep, complete);
    return err;
}/* End of function usb_hstd_clr_stall() */

/******************************************************************************
 Function Name   : usb_hstd_clr_stall_result
 Description     : Callback function to notify HCD task that usb_hstd_clr_stall function is completed
 Arguments       : usb_utr_t *ptr : USB internal structure. Selects e.g. channel.
 : uint16_t      data1       : Not Use
 : uint16_t      data2       : Not Use
 Return value    : none
 ******************************************************************************/
static void usb_hstd_clr_stall_result (usb_utr_t *ptr, uint16_t data1, uint16_t data2)
{
    usb_mh_t p_blf;
    usb_er_t err;
    usb_er_t err2;
    usb_utr_t *up;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_HCD_MPL, &p_blf);
    if (USB_OK == err)
    {
        up = (usb_utr_t*) p_blf;
        up->msghead = (usb_mh_t) USB_NULL;
        up->msginfo = USB_MSG_HCD_CLR_STALL_RESULT;
        up->status = ptr->status;

        up->ipp = ptr->ipp;
        up->ip = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_HCD_MBX, (usb_msg_t* )p_blf);
        if (USB_OK != err)
        {
            USB_PRINTF1("### hHcdSndMbx snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_HCD_MPL, (usb_mh_t )p_blf);
            if (USB_OK != err2)
            {
                USB_PRINTF1("### hHcdSndMbx rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hHcdSndMbx pget_blk error (%ld)\n", err);
    }
}/* End of function usb_hstd_clr_stall_result() */

/******************************************************************************
 Function Name   : usb_hstd_hcd_task
 Description     : USB Host Control Driver Task.
 Argument        : usb_vp_int_t stacd  : Task Start Code.
 Return          : none
 ******************************************************************************/
void usb_hstd_hcd_task (usb_vp_int_t stacd)
{
    usb_utr_t *mess;
    usb_utr_t *ptr;
    usb_er_t err;
    uint16_t rootport;
    uint16_t pipenum;
    uint16_t msginfo;
    uint16_t connect_inf;
    uint16_t retval;
    usb_hcdinfo_t* hp;

    /* Receive message */
    err = USB_TRCV_MSG(USB_HCD_MBX, (usb_msg_t** )&mess, (usb_tm_t )10000);
    if (USB_OK != err)
    {
        return;
    }
    else
    {
        ptr = (usb_utr_t *) mess;
        hp = (usb_hcdinfo_t*) mess;
        rootport = ptr->keyword;
        pipenum = ptr->keyword;

        /* Branch Hcd Task receive Message Command */
        msginfo = ptr->msginfo;
        switch (msginfo)
        {
            case USB_MSG_HCD_INT :

                /* USB INT */
                usb_hstd_interrupt(ptr);
            break;

            case USB_MSG_HCD_PCUTINT :
                ptr = (usb_utr_t*) usb_shstd_hcd_msg;

                /* USB interrupt Handler */
                usb_hstd_interrupt_handler(ptr);

                /* USB INT */
                usb_hstd_interrupt(ptr);
                ptr->msginfo = USB_MSG_HCD_INT;
            break;

            case USB_MSG_HCD_SUBMITUTR :

                /* USB Submit utr */
                usb_hstd_set_submitutr(ptr);
            break;

            case USB_MSG_HCD_ATTACH :

                /* USB attach / detach */
                usb_hstd_attach_process(ptr, rootport);

                /* Callback */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_ATTACH);

                /* Release Memory Block */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_ATTACH_MGR :

                /* USB attach / detach */
                usb_hstd_attach_process(ptr, rootport);
                connect_inf = usb_cstd_port_speed(ptr, rootport);

                /* Callback */
                (hp->complete)(ptr, rootport, connect_inf);

                /* Release Memory Block */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_DETACH :

                /* USB detach process */
                usb_hstd_detach_process(ptr, rootport);

                /* Callback */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_DETACH);

                /* Release Memory Block */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_DETACH_MGR :
                HW_USB_ClearDvstctr(ptr, USB_PORT0, (USB_RWUPE | USB_USBRST | USB_RESUME | USB_UACT));

                usb_cpu_delay_xms(1);

                /* interrupt disable */
                usb_hstd_attch_disable(ptr, rootport);
                usb_hstd_dtch_disable(ptr, rootport);
                usb_hstd_bchg_disable(ptr, rootport);
                (hp->complete)(ptr, rootport, USB_MSG_HCD_DETACH_MGR);
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_USBRESET :

                /* USB bus reset */
                usb_hstd_bus_reset(ptr, rootport);

                /* Check current port speed */
                connect_inf = usb_cstd_port_speed(ptr, rootport);

                /* Callback */
                (hp->complete)(ptr, rootport, connect_inf);

                /* Release Memory Block */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_REMOTE :

                /* Suspend device */
                g_usb_hstd_remort_port[rootport] = USB_SUSPENDED;
                usb_hstd_suspend(ptr, rootport);

                /* CallBack */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_REMOTE);

                /* Release Memory Block */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_SUSPEND :
                usb_hstd_suspend(ptr, rootport); /* Suspend device */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_SUSPEND);
                usb_hstd_hcd_rel_mpl(ptr, msginfo); /* Release Memory Block */
            break;

            case USB_MSG_HCD_RESUME :
                usb_hstd_resume_process(ptr, rootport); /* USB resume */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_RESUME); /* Callback */
                usb_hstd_hcd_rel_mpl(ptr, msginfo); /* Release Memory Block */
            break;

            case USB_MSG_HCD_VBON :

                usb_hstd_ovrcr_enable(ptr, rootport); /* Interrupt Enable */

                usb_hstd_vbus_control(ptr, rootport, (uint16_t) USB_VBON); /* USB VBUS control ON */
    #if USB_CFG_BC == USB_CFG_DISABLE
                /* 100ms wait */
                usb_cpu_delay_xms((uint16_t) 100u);

    #endif /* USB_CFG_BC == USB_CFG_DISABLE */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_VBON); /* Callback */
                usb_hstd_hcd_rel_mpl(ptr, msginfo); /* Release Memory Block */
            break;

            case USB_MSG_HCD_VBOFF :
                usb_hstd_vbus_control(ptr, rootport, (uint16_t) USB_VBOFF); /* USB VBUS control OFF */
                usb_hstd_ovrcr_disable(ptr, rootport);

                usb_cpu_delay_xms((uint16_t) 100u); /* 100ms wait */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_VBOFF); /* Callback */
                usb_hstd_hcd_rel_mpl(ptr, msginfo); /* Release Memory Block */
            break;

            case USB_MSG_HCD_CLR_STALLBIT :
                usb_cstd_clr_stall(ptr, pipenum); /* STALL */
                (hp->complete)(ptr, (uint16_t) USB_NO_ARG, (uint16_t) USB_MSG_HCD_CLR_STALLBIT); /* Callback */
                usb_hstd_hcd_rel_mpl(ptr, msginfo); /* Release Memory Block */
            break;

            case USB_MSG_HCD_SQTGLBIT :
                pipenum = ptr->keyword & USB_PIPENM;
                usb_hstd_do_sqtgl(ptr, pipenum, ptr->keyword); /* SQ toggle */
                (hp->complete)(ptr, (uint16_t) USB_NO_ARG, (uint16_t) USB_MSG_HCD_SQTGLBIT); /* Callback */
                usb_hstd_hcd_rel_mpl(ptr, msginfo); /* Release Memory Block */
            break;

            case USB_MSG_HCD_CLR_STALL :
                usb_shstd_clr_stall_call = hp->complete;
                usb_shstd_clr_stall_pipe = pipenum;
                err = usb_hstd_clr_stall(ptr, pipenum, (usb_cb_t) &usb_hstd_clr_stall_result);
                if ( USB_QOVR == err)
                {
                    USB_WAI_MSG(USB_HCD_MBX, ptr, 1000); /* Retry */
                }
                else
                {
                    /* Release Memory Block */
                    usb_hstd_hcd_rel_mpl(ptr, msginfo);
                }
            break;
            case USB_MSG_HCD_CLR_STALL_RESULT :
                ptr = (usb_utr_t*) mess;
                retval = ptr->status;

                if (USB_DATA_TMO == retval)
                {
                    USB_PRINTF0("*** Standard Request Timeout error !\n");
                }
                else if (USB_DATA_STALL == retval)
                {
                    USB_PRINTF0("*** Standard Request STALL !\n");
                }
                else if (USB_CTRL_END != retval)
                {
                    USB_PRINTF0("*** Standard Request error !\n");
                }
                else
                {
                    usb_cstd_clr_stall(ptr, usb_shstd_clr_stall_pipe);
                    HW_USB_SetSqclr(ptr, usb_shstd_clr_stall_pipe); /* SQCLR */
                }
                (*usb_shstd_clr_stall_call)(ptr, retval, USB_MSG_HCD_CLR_STALL);

                /* Release Memory Block */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_CLRSEQBIT :
                HW_USB_SetSqclr(ptr, pipenum); /* SQCLR */
                (hp->complete)(ptr, (uint16_t) USB_NO_ARG, (uint16_t) USB_MSG_HCD_CLRSEQBIT); /* Callback */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_SETSEQBIT :
                HW_USB_SetSqset(ptr, pipenum); /* SQSET */
                (hp->complete)(ptr, (uint16_t) USB_NO_ARG, (uint16_t) USB_MSG_HCD_SETSEQBIT); /* Callback */
                usb_hstd_hcd_rel_mpl(ptr, msginfo); /* Release Memory Block */
            break;

            case USB_MSG_HCD_TRANSEND1 :

                /* Pipe Transfer Process check */
                if (USB_NULL != g_usb_hstd_pipe[ptr->ip][pipenum])
                {
                    /* Control Transfer stop */
                    if (USB_PIPE0 == pipenum)
                    {
                        /* Control Read/Write End */
                        usb_hstd_ctrl_end(ptr, (uint16_t) USB_DATA_TMO);
                    }
                    else
                    {
                        /* Transfer stop */
                        usb_hstd_forced_termination(ptr, pipenum, (uint16_t) USB_DATA_TMO);
                    }
                }
                else
                {
                    USB_PRINTF1("### Host not transferd %d\n",pipenum);
                }

                /* Release Memory Block */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_TRANSEND2 :

                /* Pipe Transfer Process check */
                if (USB_NULL != g_usb_hstd_pipe[ptr->ip][pipenum])
                {
                    /* Control Transfer stop */
                    if (USB_PIPE0 == pipenum)
                    {
                        /* Control Read/Write End */
                        usb_hstd_ctrl_end(ptr, (uint16_t) USB_DATA_STOP);
                    }
                    else
                    {
                        /* Transfer stop */
                        usb_hstd_forced_termination(ptr, pipenum, (uint16_t) USB_DATA_STOP);
                    }
                }
                else
                {
                    USB_PRINTF1("### Host not transferd %d\n",pipenum);
                }

                /* Release Memory Block */
                usb_hstd_hcd_rel_mpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_D1FIFO_INT :
            break;

            case USB_MSG_HCD_RESM_INT :
            break;

            default :
            break;
        }
    }
}/* End of function usb_hstd_hcd_task() */

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 End  Of File
 ******************************************************************************/

