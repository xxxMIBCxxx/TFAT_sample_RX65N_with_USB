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
 * File Name    : r_usb_hscheduler.c
 * Description  : USB Host scheduler code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 04.01.2014 1.00 First Release
 *         : 30.01.2015 1.01    Added RX71M.
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_if.h"
#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"

#if ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST )
/******************************************************************************
 Constant macro definitions
 ******************************************************************************/
    #define USB_IDMAX           (11u)                 /* Maximum Task ID +1 */
    #define USB_PRIMAX          (8u)                  /* Maximum Priority number +1 */
    #define USB_BLKMAX          (20u)                 /* Maximum block */
    #define USB_TABLEMAX        (USB_BLKMAX)          /* Maximum priority table */
    #define USB_WAIT_EVENT_MAX  (5u)

/******************************************************************************
External variables and functions
******************************************************************************/
extern void usb_dma_driver(void);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
/* Priority Table */
static usb_msg_t* g_usb_scheduler_table_add[USB_PRIMAX][USB_TABLEMAX];
static uint8_t g_usb_scheduler_table_id[USB_PRIMAX][USB_TABLEMAX];
static uint8_t g_usb_scheduler_pri_r[USB_PRIMAX];
static uint8_t g_usb_scheduler_pri_w[USB_PRIMAX];
static uint8_t g_usb_scheduler_pri[USB_IDMAX];

/* Schedule Set Flag  */
static uint8_t g_usb_scheduler_schedule_flag;

/* Fixed-sized memory pools */
static usb_utr_t g_usb_scheduler_block[USB_BLKMAX];
static uint8_t g_usb_scheduler_blk_flg[USB_BLKMAX];

static usb_msg_t* g_usb_scheduler_add_use;
static uint8_t g_usb_scheduler_id_use;

/* Wait MSG */
static usb_msg_t* g_usb_scheduler_wait_add[USB_IDMAX][USB_WAIT_EVENT_MAX];
static uint16_t g_usb_scheduler_wait_counter[USB_IDMAX][USB_WAIT_EVENT_MAX];

/******************************************************************************
 Renesas Scheduler API functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_rec_msg
 Description     : Receive a message to the specified id (mailbox).
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_msg_t** mess  : Message pointer
 : usb_tm_t tm       : Timeout Value
 Return          : uint16_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_rec_msg (uint8_t id, usb_msg_t** mess, usb_tm_t tm)
{
    if ((id < USB_IDMAX) && (g_usb_scheduler_id_use < USB_IDMAX))
    {
        if (id == g_usb_scheduler_id_use)
        {
            *mess = g_usb_scheduler_add_use;
            return USB_OK;
        }
    }
    return USB_ERROR;
}
/******************************************************************************
 End of function usb_cstd_rec_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_RecMsg
 Description     : Receive a message to the specified id (mailbox).
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_msg_t** mess  : Message pointer
 : usb_tm_t tm       : Timeout Value
 Return          : uint16_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_RecMsg (uint8_t id, usb_msg_t** mess, usb_tm_t tm)
{
    usb_er_t err;

    err = usb_cstd_rec_msg(id, mess, tm);

    return err;
}
/******************************************************************************
 End of function usb_cstd_RecMsg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_snd_msg
 Description     : Send a message to the specified id (mailbox).
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_msg_t* mess   : Message pointer
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_snd_msg (uint8_t id, usb_msg_t* mess)
{
    usb_er_t status;

    /* USB interrupt disable */
    usb_cpu_int_disable((usb_utr_t*) mess);
    status = usb_cstd_isnd_msg(id, mess);

    /* USB interrupt enable */
    usb_cpu_int_enable((usb_utr_t*) mess);
    return status;
}
/******************************************************************************
 End of function usb_cstd_snd_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_SndMsg
 Description     : Send a message to the specified id (mailbox).
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_msg_t* mess   : Message pointer
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_SndMsg (uint8_t id, usb_msg_t* mess)
{
    usb_er_t status;

    status = usb_cstd_snd_msg(id, mess);

    return status;
}
/******************************************************************************
 End of function usb_cstd_SndMsg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_isnd_msg
 Description     : Send a message to the specified id (mailbox) while executing 
 : an interrupt.
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_msg_t* mess   : Message pointer
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_isnd_msg (uint8_t id, usb_msg_t* mess)
{
    uint8_t usb_pri; /* Task Priority */
    uint8_t usb_write; /* Priority Table Writing pointer */

    if (id < USB_IDMAX)
    {
        /* Read priority and table pointer */
        usb_pri = g_usb_scheduler_pri[id];
        usb_write = g_usb_scheduler_pri_w[usb_pri];
        if (usb_pri < USB_PRIMAX)
        {
            /* Renewal write pointer */
            usb_write++;
            if (usb_write >= USB_TABLEMAX)
            {
                usb_write = USB_TBLCLR;
            }

            /* Check pointer */
            if (usb_write == g_usb_scheduler_pri_r[usb_pri])
            {
                return USB_ERROR;
            }

            /* Save message */
            /* Set priority table */
            g_usb_scheduler_table_id[usb_pri][usb_write] = id;
            g_usb_scheduler_table_add[usb_pri][usb_write] = mess;
            g_usb_scheduler_pri_w[usb_pri] = usb_write;
            return USB_OK;
        }
    }USB_PRINTF0("SND_MSG ERROR !!\n");
    return USB_ERROR;

}
/******************************************************************************
 End of function usb_cstd_isnd_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_iSndMsg
 Description     : Send a message to the specified id (mailbox) while executing 
 : an interrupt.
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_msg_t* mess   : Message pointer
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_iSndMsg (uint8_t id, usb_msg_t* mess)
{
    usb_er_t err;

    err = usb_cstd_isnd_msg(id, mess);

    return err;
}
/******************************************************************************
 End of function usb_cstd_iSndMsg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_pget_blk
 Description     : Get a memory block for the caller.
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_utr_t** blk   : Memory block pointer.
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_pget_blk (uint8_t id, usb_utr_t** blk)
{
    uint8_t usb_s_pblk_c;

    if (id < USB_IDMAX)
    {
        usb_s_pblk_c = USB_CNTCLR;
        while (USB_BLKMAX != usb_s_pblk_c)
        {
            if (USB_FLGCLR == g_usb_scheduler_blk_flg[usb_s_pblk_c])
            {
                /* Acquire fixed-size memory block */
                *blk = &g_usb_scheduler_block[usb_s_pblk_c];
                g_usb_scheduler_blk_flg[usb_s_pblk_c] = USB_FLGSET;
                return USB_OK;
            }
            usb_s_pblk_c++;
        }

        /* Error of BLK Table Full !!  */
        USB_PRINTF1("usb_scBlkFlg[%d][] Full !!\n",id);
    }
    return USB_ERROR;
}
/******************************************************************************
 End of function usb_cstd_pget_blk
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_PgetBlk
 Description     : Call the get memory block function to get a memory block for 
 : the caller.
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_utr_t** blk   : Memory block pointer.
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_PgetBlk (uint8_t id, usb_utr_t** blk)
{
    usb_er_t err;

    err = usb_cstd_pget_blk(id, blk);

    return usb_cstd_check(err);
}
/******************************************************************************
 End of function usb_cstd_PgetBlk
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_rel_blk
 Description     : Release a memory block.
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_utr_t* blk    : Memory block pointer.
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_rel_blk (uint8_t id, usb_utr_t* blk)
{
    uint16_t usb_s_rblk_c;

    if (id < USB_IDMAX)
    {
        usb_s_rblk_c = USB_CNTCLR;
        while (USB_BLKMAX != usb_s_rblk_c)
        {
            if ((&g_usb_scheduler_block[usb_s_rblk_c]) == blk)
            {
                /* Release fixed-size memory block */
                g_usb_scheduler_blk_flg[usb_s_rblk_c] = USB_FLGCLR;
                return USB_OK;
            }
            usb_s_rblk_c++;
        }

        /* Error of BLK Flag is not CLR !!  */
        USB_PRINTF0("TskBlk NO CLR !!\n");
    }
    return USB_ERROR;
}
/******************************************************************************
 End of function usb_cstd_rel_blk
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_RelBlk
 Description     : Call the function to release a memory block.
 Argument        : uint8_t id        : ID number (mailbox).
 : usb_utr_t* blk    : Memory block pointer.
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_RelBlk (uint8_t id, usb_utr_t* blk)
{
    usb_er_t err;

    err = usb_cstd_rel_blk(id, blk);

    return usb_cstd_check(err);
}
/******************************************************************************
 End of function usb_cstd_RelBlk
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_wai_msg
 Description     : Runs USB_SND_MSG after running the scheduler the specified 
 : number of times.
 Argument        : uint8_t  id       : ID number (mailbox).
 : usb_msg_t *mess   : Message pointer.
 : uint16_t  times   : Timeout value.
 Return          : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_cstd_wai_msg (uint8_t id, usb_msg_t* mess, usb_tm_t times)
{
    uint8_t i;

    if (id < USB_IDMAX)
    {
        for (i = 0; i < USB_WAIT_EVENT_MAX; i++)
        {
            if (0 == g_usb_scheduler_wait_counter[id][i])
            {
                g_usb_scheduler_wait_add[id][i] = mess;
                g_usb_scheduler_wait_counter[id][i] = times;
                return USB_OK;
            }
        }
    }

    /* Error !!  */
    USB_PRINTF0("WAI_MSG ERROR !!\n");
    return USB_ERROR;
}
/******************************************************************************
 End of function usb_cstd_wai_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_WaiMsg
 Description     : Will run USB_SND_MSG after running the scheduler the specified 
 : number of times.
 Argument        : uint8_t  id       : ID number (mailbox).
 : usb_msg_t *mess   : Message pointer.
 : uint16_t  times   : Timeout value.
 Return          : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_cstd_WaiMsg (uint8_t id, usb_msg_t* mess, usb_tm_t times)
{
    usb_er_t err;

    err = usb_cstd_wai_msg(id, mess, times);

    return usb_cstd_check(err);

}
/******************************************************************************
 End of function usb_cstd_WaiMsg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_wait_scheduler
 Description     : Schedules a wait request.
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_cstd_wait_scheduler (void)
{
    usb_er_t err;
    uint8_t id;
    uint8_t i;

    for (id = 0; id < USB_IDMAX; id++)
    {
        for (i = 0; i < USB_WAIT_EVENT_MAX; i++)
        {
            if (0 != g_usb_scheduler_wait_counter[id][i])
            {
                g_usb_scheduler_wait_counter[id][i]--;
                if (0 == g_usb_scheduler_wait_counter[id][i])
                {
                    err = usb_cstd_snd_msg(id, g_usb_scheduler_wait_add[id][i]);
                    if (USB_OK != err)
                    {
                        g_usb_scheduler_wait_counter[id][i]++;
                    }
                }
            }
        }
    }
}
/******************************************************************************
 End of function usb_cstd_wait_scheduler
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_sche_init
 Description     : Scheduler initialization.
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_cstd_sche_init (void)
{
    uint8_t i;
    uint8_t j;

    /* Initial Scheduler */
    g_usb_scheduler_id_use = USB_NULL;
    g_usb_scheduler_schedule_flag = USB_NULL;

    /* Initialize  priority table pointer and priority table */
    for (i = 0; i != USB_PRIMAX; i++)
    {
        g_usb_scheduler_pri_r[i] = USB_NULL;
        g_usb_scheduler_pri_w[i] = USB_NULL;
        for (j = 0; j != USB_TABLEMAX; j++)
        {
            g_usb_scheduler_table_id[i][j] = USB_IDMAX;
        }
    }

    /* Initialize block table */
    for (i = 0; i != USB_BLKMAX; i++)
    {
        g_usb_scheduler_blk_flg[i] = USB_NULL;
    }

    /* Initialize priority */
    for (i = 0; i != USB_IDMAX; i++)
    {
        g_usb_scheduler_pri[i] = (uint8_t) USB_IDCLR;
        for (j = 0; j < USB_WAIT_EVENT_MAX; j++)
        {
            g_usb_scheduler_wait_add[i][j] = (usb_msg_t*) USB_NULL;
            g_usb_scheduler_wait_counter[i][j] = USB_NULL;
        }
    }
}
/******************************************************************************
 End of function usb_cstd_sche_init
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_scheduler
 Description     : The scheduler.
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_cstd_scheduler (void)
{
    uint8_t usb_pri; /* Priority Counter */
    uint8_t usb_read; /* Priority Table read pointer */

    /* wait msg */
    usb_cstd_wait_scheduler();

    /* Priority Table reading */
    usb_pri = USB_CNTCLR;
    while (usb_pri < USB_PRIMAX)
    {
        usb_read = g_usb_scheduler_pri_r[usb_pri];
        if (usb_read != g_usb_scheduler_pri_w[usb_pri])
        {
            /* Priority Table read pointer increment */
            usb_read++;
            if (usb_read >= USB_TABLEMAX)
            {
                usb_read = USB_TBLCLR;
            }

            /* Set practise message */
            g_usb_scheduler_id_use = g_usb_scheduler_table_id[usb_pri][usb_read];
            g_usb_scheduler_add_use = g_usb_scheduler_table_add[usb_pri][usb_read];
            g_usb_scheduler_table_id[usb_pri][usb_read] = USB_IDMAX;
            g_usb_scheduler_pri_r[usb_pri] = usb_read;
            g_usb_scheduler_schedule_flag = USB_FLGSET;
            usb_pri = USB_PRIMAX;
        }
        else
        {
            usb_pri++;
        }
    }
#if USB_CFG_DTCDMA == USB_CFG_ENABLE
    usb_dma_driver();           /* USB DMA driver */
#endif  /* USB_CFG_DTCDMA == USB_CFG_ENABLE */
}
/******************************************************************************
 End of function usb_cstd_scheduler
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_set_task_pri
 Description     : Set a task's priority.
 Argument        : uint8_t tasknum   : Task id.
 : uint8_t pri       : The task priority to be set.
 Return          : none
 ******************************************************************************/
void usb_cstd_set_task_pri (uint8_t tasknum, uint8_t pri)
{
    if (tasknum < USB_IDMAX)
    {
        if (pri < USB_PRIMAX)
        {
            g_usb_scheduler_pri[tasknum] = pri;
        }
        else if ((uint8_t) USB_IDCLR == pri)
        {
            g_usb_scheduler_pri[tasknum] = (uint8_t) USB_IDCLR;
        }
        else
        {
            /* Non */
        }
    }
}
/******************************************************************************
 End of function usb_cstd_set_task_pri
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_check_schedule
 Description     : Check schedule flag to see if caller's "time has come", then 
 : clear it.
 Argument        : none
 Return          : flg   : g_usb_scheduler_schedule_flag
 ******************************************************************************/
uint8_t usb_cstd_check_schedule (void)
{
    uint8_t flg;

    flg = g_usb_scheduler_schedule_flag;
    g_usb_scheduler_schedule_flag = USB_FLGCLR;
    return flg;
}
/******************************************************************************
 End of function usb_cstd_check_schedule
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_check
 Description     : 
 Argument        : 
 Return          : 
 ******************************************************************************/
usb_err_t usb_cstd_check (usb_er_t err)
{
    usb_err_t ret_code;

    if (USB_OK == err)
    {
        ret_code = USB_SUCCESS;
    }
    else
    {
        ret_code = USB_ERR_NG;
    }
    return ret_code;
} /* End of function usb_cstd_check() */

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 End  Of File
 ******************************************************************************/
