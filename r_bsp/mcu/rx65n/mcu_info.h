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
* File Name    : mcu_info.h
* Device(s)    : RX65N
* Description  : Information about the MCU on this board.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 21.08.2015 0.40      First Release
*         : 01.12.2015 0.40(WS2) Change the Device(s) from RX651 to RX65N.
*                                Added the macro.(#define BSP_DATA_FLASH_SIZE_BYTES)
*         : 08.01.2016 0.50      Added ROM Capacity.
***********************************************************************************************************************/

#ifndef MCU_INFO
#define MCU_INFO

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Gets MCU configuration information. */
#include "r_bsp_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* MCU Series. */
#if   BSP_CFG_MCU_PART_SERIES == 0x0
    #define BSP_MCU_SERIES_RX600    (1)
#else
    #error "ERROR - BSP_CFG_MCU_PART_SERIES - Unknown MCU Series chosen in r_bsp_config.h"
#endif

/* This macro means that this MCU is part of the RX65x collection of MCUs (i.e. RX651/N). */
#define BSP_MCU_RX65_ALL            (1)

/* MCU Group name. */
#if   BSP_CFG_MCU_PART_GROUP == 0x0
    #define BSP_MCU_RX65N           (1)
#elif BSP_CFG_MCU_PART_GROUP == 0x1
    #define BSP_MCU_RX651           (1)
#else
    #error "ERROR - BSP_CFG_MCU_PART_GROUP - Unknown MCU Group chosen in r_bsp_config.h"
#endif

/* Package. */
#if   BSP_CFG_MCU_PART_PACKAGE == 0x3
    #define BSP_PACKAGE_LQFP144     (1)
    #define BSP_PACKAGE_PINS        (144)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x4
    #define BSP_PACKAGE_TFLGA145    (1)
    #define BSP_PACKAGE_PINS        (145)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x5
    #define BSP_PACKAGE_LQFP100     (1)
    #define BSP_PACKAGE_PINS        (100)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x6
    #define BSP_PACKAGE_TFLGA100    (1)
    #define BSP_PACKAGE_PINS        (100)
#else
    #error "ERROR - BSP_CFG_MCU_PART_PACKAGE - Unknown package chosen in r_bsp_config.h"
#endif

/* Memory size of your MCU. */
#if BSP_CFG_MCU_PART_MEMORY_SIZE == 0x4
    #define BSP_ROM_SIZE_BYTES              (524288)
    #define BSP_RAM_SIZE_BYTES              (262144)
    #define BSP_DATA_FLASH_SIZE_BYTES       (0)
#elif (BSP_CFG_MCU_PART_MEMORY_SIZE == 0x7)
    #define BSP_ROM_SIZE_BYTES              (786432)
    #define BSP_RAM_SIZE_BYTES              (262144)
    #define BSP_DATA_FLASH_SIZE_BYTES       (0)
#elif (BSP_CFG_MCU_PART_MEMORY_SIZE == 0x9)
    #define BSP_ROM_SIZE_BYTES              (1048576)
    #define BSP_RAM_SIZE_BYTES              (262144)
    #define BSP_DATA_FLASH_SIZE_BYTES       (0)
#else
    #error "ERROR - BSP_CFG_MCU_PART_MEMORY_SIZE - Unknown memory size chosen in r_bsp_config.h"
#endif

/* These macros define clock speeds for fixed speed clocks. */
#define BSP_LOCO_HZ                         (240000)        //Low-speed On-Chip Oscillator
#define BSP_SUB_CLOCK_HZ                    (32768)         //Sub-Clock Oscillator


/* Define frequency of HOCO. */
#if   (BSP_CFG_HOCO_FREQUENCY == 0)
    #define BSP_HOCO_HZ                     (16000000)
#elif (BSP_CFG_HOCO_FREQUENCY == 1)
    #define BSP_HOCO_HZ                     (18000000)
#elif (BSP_CFG_HOCO_FREQUENCY == 2)
    #define BSP_HOCO_HZ                     (20000000)
#else
    #error "ERROR - Invalid HOCO frequency chosen in r_bsp_config.h! Set valid value for BSP_CFG_HOCO_FREQUENCY."
#endif

/* Clock source select (CKSEL).
   0 = Low Speed On-Chip Oscillator  (LOCO)
   1 = High Speed On-Chip Oscillator (HOCO)
   2 = Main Clock Oscillator
   3 = Sub-Clock Oscillator
   4 = PLL Circuit
*/ 
#if   (BSP_CFG_CLOCK_SOURCE == 0)
    #define BSP_SELECTED_CLOCK_HZ       (BSP_LOCO_HZ)
#elif (BSP_CFG_CLOCK_SOURCE == 1)
    #define BSP_SELECTED_CLOCK_HZ       (BSP_HOCO_HZ)
#elif (BSP_CFG_CLOCK_SOURCE == 2)
    #define BSP_SELECTED_CLOCK_HZ       (BSP_CFG_XTAL_HZ)
#elif (BSP_CFG_CLOCK_SOURCE == 3)
    #define BSP_SELECTED_CLOCK_HZ       (BSP_SUB_CLOCK_HZ)
#elif (BSP_CFG_CLOCK_SOURCE == 4)
    #if   (BSP_CFG_PLL_SRC == 0)
        #define BSP_SELECTED_CLOCK_HZ   ((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL)
    #elif (BSP_CFG_PLL_SRC == 1)
        #define BSP_SELECTED_CLOCK_HZ   ((BSP_HOCO_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL)
    #else
        #error "ERROR - Valid PLL clock source must be chosen in r_bsp_config.h using BSP_CFG_PLL_SRC macro."
    #endif
#endif

/* System clock speed in Hz. */
#define BSP_ICLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_ICK_DIV)
/* Peripheral Module Clock A speed in Hz. Used for ETHERC and EDMAC. */
#define BSP_PCLKA_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKA_DIV)
/* Peripheral Module Clock B speed in Hz. */
#define BSP_PCLKB_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKB_DIV)
/* Peripheral Module Clock C speed in Hz. */
#define BSP_PCLKC_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKC_DIV)
/* Peripheral Module Clock D speed in Hz. */
#define BSP_PCLKD_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKD_DIV)
/* External bus clock speed in Hz. */
#define BSP_BCLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_BCK_DIV)
/* FlashIF clock speed in Hz. */
#define BSP_FCLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_FCK_DIV)
/* USB clock speed in Hz. */
#define BSP_UCLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_UCK_DIV)

/* Null argument definitions. */
#define FIT_NO_FUNC                 ((void (*)(void *))0x10000000)  //Reserved space on RX
#define FIT_NO_PTR                  ((void *)0x10000000)            //Reserved space on RX

/* Mininum and maximum IPL levels available for this MCU. */
#define BSP_MCU_IPL_MAX             (0xF)
#define BSP_MCU_IPL_MIN             (0)

#endif /* MCU_INFO */

