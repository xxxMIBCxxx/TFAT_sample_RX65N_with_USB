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
* File Name    : r_bsp_config_reference.h
* Device(s)    : RX65N
* Description  : The file r_bsp_config.h is used to configure your BSP. r_bsp_config.h should be included
*                somewhere in your package so that the r_bsp code has access to it. This file (r_bsp_config_reference.h)
*                is just a reference file that the user can use to make their own r_bsp_config.h file.
************************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 21.08.2015 0.40      First Release
*         : 01.12.2015 0.40(WS2) Change the Device(s) from RX651 to RX65N.
*                                Change the comment of #define BSP_CFG_MCU_PART_ENCRYPTION_INCLUDED.
*                                Change the value of #define BSP_CFG_MCU_PART_GROUP.
*                                Change the comment of Trusted Memory.
*                                Change the comment of FAW.
*                                Remove the macro.(BSP_CFG_FILE_REVISION_MAJOR)
*                                Remove the macro.(BSP_CFG_FILE_REVISION_MINOR)
*                                Added ROM Cache.
*         : 08.01.2016 0.50      Added the following macro definition.
*                                - BSP_CFG_MAIN_CLOCK_SOURCE
*                                - BSP_CFG_MOSC_WAIT_TIME
*                                - BSP_CFG_SOSC_WAIT_TIME
*                                Added ROM Capacity.
***********************************************************************************************************************/
#ifndef R_BSP_CONFIG_REF_HEADER_FILE
#define R_BSP_CONFIG_REF_HEADER_FILE

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Enter the product part number for your MCU. This information will be used to obtain information about your MCU such 
   as package and memory size. 
   To help parse this information, the part number will be defined using multiple macros.
   R 5 F 56 5N 9 A D FB
   | | | |  |  | | | |  Macro Name              Description
   | | | |  |  | | | |__BSP_CFG_MCU_PART_PACKAGE             = Package type, number of pins, and pin pitch
   | | | |  |  | | |____not used                             = Products with wide temperature range
   | | | |  |  | |______BSP_CFG_MCU_PART_ENCRYPTION_INCLUDED = Encryption module included/not included
   | | | |  |  |________BSP_CFG_MCU_PART_MEMORY_SIZE         = ROM, RAM, and Data Flash Capacity
   | | | |  |___________BSP_CFG_MCU_PART_GROUP               = Group name  
   | | | |______________BSP_CFG_MCU_PART_SERIES              = Series name
   | | |________________BSP_CFG_MCU_PART_MEMORY_TYPE         = Type of memory (Flash, ROMless)
   | |__________________not used                             = Renesas MCU
   |____________________not used                             = Renesas semiconductor product. 
   */

/* Package type. Set the macro definition based on values below:
   Character(s) = Value for macro = Package Type/Number of Pins/Pin Pitch
   FB           = 0x3             = LQFP/144/0.50
   LK           = 0x4             = TFLGA/145/0.50
   FP           = 0x5             = LQFP/100/0.50
   LJ           = 0x6             = TFLGA/100/0.65
*/
#define BSP_CFG_MCU_PART_PACKAGE        (0x3)

/* Whether Encryption and SDHI/SDSI are included or not. 
   Character(s) = Value for macro = Description
   A            = false           = Encryption module not included, SDHI/SDSI module not included
   B            = false           = Encryption module not included, SDHI/SDSI module included
   E            = true            = Encryption module included, SDHI/SDSI module not included
   F            = true            = Encryption module included, SDHI/SDSI module included
*/
#define BSP_CFG_MCU_PART_ENCRYPTION_INCLUDED   (false)

/* ROM, RAM, and Data Flash Capacity. 
   Character(s) = Value for macro = ROM Size/Ram Size/Data Flash Size
   4            = 0x4             = 512KB/256KB/Not equipped
   7            = 0x7             = 768KB/256KB/Not equipped
   9            = 0x9             = 1MB/256KB/Not equipped
*/
#define BSP_CFG_MCU_PART_MEMORY_SIZE    (0x9)

/* Group name. 
   Character(s) = Value for macro = Description
   5N           = 0x0             = RX65N Group
   51           = 0x1             = RX651 Group
*/
#define BSP_CFG_MCU_PART_GROUP          (0x0)

/* Series name. 
   Character(s) = Value for macro = Description
   56           = 0x0             = RX600 Series
*/  
#define BSP_CFG_MCU_PART_SERIES         (0x0)

/* Memory type. 
   Character(s) = Value for macro = Description
   F            = 0x0             = Flash memory version
*/
#define BSP_CFG_MCU_PART_MEMORY_TYPE    (0x0)

/* Whether to use 1 stack or 2. RX MCUs have the ability to use 2 stacks: an interrupt stack and a user stack.
 * When using 2 stacks the user stack will be used during normal user code. When an interrupt occurs the CPU
 * will automatically shift to using the interrupt stack. Having 2 stacks can make it easier to figure out how
 * much stack space to allocate since the user does not have to worry about always having enough room on the
 * user stack for if-and-when an interrupt occurs. Some users will not want 2 stacks though because it is not
 * needed in all applications and can lead to wasted RAM (i.e. space in between stacks that is not used).
 * If only 1 stack is used then the interrupt stack is the one that will be used. If 1 stack is chosen then
 * the user may want to remove the 'SU' section from the linker sections to remove any linker warnings.
 *
 * 0 = Use 1 stack. Disable user stack. User stack size set below will be ignored.
 * 1 = Use 2 stacks. User stack and interrupt stack will both be used.
 */
#define BSP_CFG_USER_STACK_ENABLE       (1)

/* The 'BSP_DECLARE_STACK' macro is checked so that the stack is only declared in one place (resetprg.c). Every time a 
   '#pragma stacksize' is encountered, the stack size is increased. This prevents multiplication of stack size. */
#if defined(BSP_DECLARE_STACK)
    /* If only 1 stack is chosen using BSP_CFG_USER_STACK_ENABLE then no RAM will be allocated for the user stack. */
    #if (BSP_CFG_USER_STACK_ENABLE == 1)
    /* User Stack size in bytes. The Renesas RX toolchain sets the stack size using the #pragma stacksize directive. */
    #pragma stacksize su=0x1000
    #endif

/* Interrupt Stack size in bytes. The Renesas RX toolchain sets the stack size using the #pragma stacksize directive.
 * If the interrupt stack is the only stack being used then the user will likely want to increase the default size
 * below.
 */
#pragma stacksize si=0x400
#endif

/* Heap size in bytes.
   To disable the heap you must follow these steps:
   1) Set this macro (BSP_CFG_HEAP_BYTES) to 0.
   2) Set the macro BSP_CFG_IO_LIB_ENABLE to 0.
   3) Disable stdio from being built into the project library. This is done by going into the Renesas RX Toolchain 
      settings and choosing the Standard Library section. After that choose 'Standard Library' for Category in HEW or 
      choose 'Contents' in E2Studio. This will present a list of modules that can be included. Uncheck the box for
      stdio.h. 
*/
#define BSP_CFG_HEAP_BYTES              (0x400)

/* Initializes C input & output library functions.
   0 = Disable I/O library initialization in resetprg.c. If you are not using stdio then use this value.
   1 = Enable I/O library initialization in resetprg.c. This is default and needed if you are using stdio. */
#define BSP_CFG_IO_LIB_ENABLE           (1)

/* If desired the user may redirect the stdio charget() and/or charput() functions to their own respective functions
   by enabling below and providing and replacing the my_sw_... function names with the names of their own functions. */
#define BSP_CFG_USER_CHARGET_ENABLED    (0)
#define BSP_CFG_USER_CHARGET_FUNCTION     my_sw_charget_function

#define BSP_CFG_USER_CHARPUT_ENABLED    (0)
#define BSP_CFG_USER_CHARPUT_FUNCTION     my_sw_charput_function

/* After reset MCU will operate in Supervisor mode. To switch to User mode, set this macro to '1'. For more information
   on the differences between these 2 modes see the CPU >> Processor Mode section of your MCU's hardware manual.
   0 = Stay in Supervisor mode.
   1 = Switch to User mode.
*/
#define BSP_CFG_RUN_IN_USER_MODE        (0)

/* Clock source select (CKSEL).
   0 = Low Speed On-Chip Oscillator  (LOCO)
   1 = High Speed On-Chip Oscillator (HOCO)
   2 = Main Clock Oscillator  
   3 = Sub-Clock Oscillator
   4 = PLL Circuit
*/ 
#define BSP_CFG_CLOCK_SOURCE            (4)

/* Main clock Oscillator Switching (MOSEL).
   0 = Resonator
   1 = External clock input
*/ 
#define BSP_CFG_MAIN_CLOCK_SOURCE       (0)

/* Clock configuration options.
   The input clock frequency is specified and then the system clocks are set by specifying the multipliers used. The
   multiplier settings are used to set the clock registers in resetprg.c. If a 24MHz clock is used and the
   ICLK is 120MHz, PCLKA is 120MHz, PCLKB is 60MHz, PCLKC is 60MHz, PCLKD is 60MHz, FCLK is 60MHz, USB Clock is 48MHz,
   and BCLK is 120MHz then the settings would be:

   BSP_CFG_XTAL_HZ  = 24000000
   BSP_CFG_PLL_DIV  = 1        (24MHz)
   BSP_CFG_PLL_MUL  = 10.0     (24MHz x 10.0 = 240MHz)
   BSP_CFG_ICK_DIV  = 2      : System Clock (ICLK)        =
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_ICK_DIV)  = 120MHz
   BSP_CFG_PCKA_DIV = 2      : Peripheral Clock A (PCLKA) = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKA_DIV) = 120MHz
   BSP_CFG_PCKB_DIV = 4      : Peripheral Clock B (PCLKB) = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKB_DIV) = 60MHz
   BSP_CFG_PCKC_DIV = 4      : Peripheral Clock C (PCLKC) = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKC_DIV) = 60MHz
   BSP_CFG_PCKD_DIV = 4      : Peripheral Clock D (PCLKD) = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKD_DIV) = 60MHz
   BSP_CFG_FCK_DIV =  4      : Flash IF Clock (FCLK)      = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_FCK_DIV)  = 60MHz
   BSP_CFG_BCK_DIV =  2      : External Bus Clock (BCK)   = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_BCK_DIV)  = 120MHz
   BSP_CFG_UCK_DIV =  5      : USB Clock (UCLK)           = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_UCK_DIV)  = 48MHz
*/

/* Input clock frequency in Hz (XTAL or EXTAL). */
#define BSP_CFG_XTAL_HZ                 (24000000)

/* The HOCO can operate at several different frequencies. Choose which one using the macro below.
   Available frequency settings:
   0 = 16MHz     (default)
   1 = 18MHz
   2 = 20MHz
*/
#define BSP_CFG_HOCO_FREQUENCY          (0)

/* PLL clock source (PLLSRCEL). Choose which clock source to input to the PLL circuit.
   Available clock sources:
   0 = Main clock (default)
   1 = HOCO
*/
#define BSP_CFG_PLL_SRC                 (0)

/* PLL Input Frequency Division Ratio Select (PLIDIV).
   Available divisors = /1 (no division), /2, /3
*/
#define BSP_CFG_PLL_DIV                 (1)

/* PLL Frequency Multiplication Factor Select (STC). 
   Available multipliers = x10.0 to x30.0 in 0.5 increments (e.g. 10.0, 10.5, 11.0, 11.5, ..., 29.0, 29.5, 30.0)
*/
#define BSP_CFG_PLL_MUL                 (10.0)

/* System Clock Divider (ICK).
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_ICK_DIV                 (2)

/* Peripheral Module Clock A Divider (PCKA). 
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_PCKA_DIV                (2)

/* Peripheral Module Clock B Divider (PCKB). 
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_PCKB_DIV                (4)

/* Peripheral Module Clock C Divider (PCKC).
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_PCKC_DIV                (4)

/* Peripheral Module Clock D Divider (PCKD).
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_PCKD_DIV                (4)

/* External Bus Clock Divider (BCLK). 
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_BCK_DIV                 (2)

/* Flash IF Clock Divider (FCK). 
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_FCK_DIV                 (4)

/* USB Clock Divider Select. 
   Available divisors = /2, /3, /4, /5
*/
#define BSP_CFG_UCK_DIV                 (5)

/* Configure BCLK output pin (only effective when external bus enabled)
   Values 0=no output, 1 = BCK frequency, 2= BCK/2 frequency
*/
#define BSP_CFG_BCLK_OUTPUT             (0)

/* Configure SDCLK output pin (only effective when external bus enabled)
   Values 0=no output, 1 = BCK frequency
*/
#define BSP_CFG_SDCLK_OUTPUT            (0)

/* Main Clock Oscillator Wait Time (MOSCWTCR).
   The value of MOSCWTCR register required for correspondence with the waiting time required to secure stable oscillation
   by the main clock oscillator is obtained by using the maximum frequency for fLOCO in the formula below.

   BSP_CFG_MOSC_WAIT_TIME > (tMAINOSC * (fLOCO_max) + 16)/32
   (tMAINOSC: main clock oscillation stabilization time; fLOCO_max: maximum frequency for fLOCO)

   If tMAINOSC is 9.98 ms and fLOCO_max is 264 kHz (the period is 1/3.78 us), the formula gives 
   BSP_CFG_MOSC_WAIT_TIME > (9.98 ms * (264 kHZ) + 16)/32 = 82.83, so set the BSP_CFG_MOSC_WAIT_TIME to 83(53h).

   NOTE: The waiting time is not required when an external clock signal is input for the main clock oscillator. 
         Set the BSP_CFG_MOSC_WAIT_TIME to 00h.
*/
#define BSP_CFG_MOSC_WAIT_TIME          (0x53)

/* Sub-Clock Oscillator Wait Time (SOSCWTCR).
   The value of SOSCWTCR register required for correspondence with the expected time to secure settling of oscillation
   by the sub-clock oscillator is obtained by using the maximum frequency for fLOCO in the formula below.

   BSP_CFG_SOSC_WAIT_TIME > (tSUBOSC * (fLOCO_max) + 16)/16384
   (tSUBOSC: sub-clock oscillation stabilization time; fLOCO_max: maximum frequency for fLOCO)

   If tSUBOSC is 2 s and fLOCO is 264 kHz (the period is 1/3.78 us), the formula gives
    BSP_CFG_SOSC_WAIT_TIME > (2 s * (264 kHz) +16)/16384 = 32.22, so set the  BSP_CFG_SOSC_WAIT_TIME bits to 33(21h).
*/
#define BSP_CFG_SOSC_WAIT_TIME          (0x21)

/* ROM Cache Enable Register (ROMCE).
   0 = ROM cache operation disabled.
   1 = ROM cache operation enabled.
*/ 
#define BSP_CFG_ROM_CACHE_ENABLE        (0)

/* Configure WDT and IWDT settings.
   OFS0 - Option Function Select Register 0
       b31:b29 Reserved When reading, these bits return the value written by the user. The write value should be 1.
       b28     WDTRSTIRQS - WDT Reset Interrupt Request - What to do on underflow (0=take interrupt, 1=reset MCU)
       b27:b26 WDTRPSS   - WDT Window Start Position Select - (0=25%, 1=50%, 2=75%, 3=100%,don't use)
       b25:b24 WDTRPES   - WDT Window End Position Select - (0=75%, 1=50%, 2=25%, 3=0%,don't use)
       b23:b20 WDTCKS    - WDT Clock Frequency Division Ratio - (1=PCLKB/4, 4=PCLKB/64, 0xF=PCLKB/128, 6=PCLKB/256,
                                                                 7=PCLKB/2048, 8=PCLKB/8192)
       b19:b18 WDTTOPS   - WDT Timeout Period Select (0=1024 cycles, 1=4096, 2=8192, 3=16384)
       b17     WDTSTRT   - WDT Start Mode Select - (0=auto-start after reset, 1=halt after reset)
       b16:b15 Reserved (set to 1)
       b14     IWDTSLCSTP - IWDT Sleep Mode Count Stop Control - (0=can't stop count, 1=stop w/some low power modes)
       b13     Reserved (set to 1)
       b12     IWDTRSTIRQS - IWDT Reset Interrupt Request - What to do on underflow (0=take interrupt, 1=reset MCU)
       b11:b10 IWDTRPSS - IWDT Window Start Position Select - (0=25%, 1=50%, 2=75%, 3=100%,don't use)
       b9:b8   IWDTRPES - IWDT Window End Position Select - (0=75%, 1=50%, 2=25%, 3=0%,don't use)
       b7:b4   IWDTCKS - IWDT Clock Frequency Division Ratio - (0=none, 2=/16, 3 = /32, 4=/64, 0xF=/128, 5=/256)
       b3:b2   IWDTTOPS - IWDT Timeout Period Select - (0=1024 cycles, 1=4096, 2=8192, 3=16384)
       b1      IWDTSTRT - IWDT Start Mode Select - (0=auto-start after reset, 1=halt after reset)
       b0      Reserved (set to 1) */
#define BSP_CFG_OFS0_REG_VALUE  (0xFFFFFFFF) //Disable by default

/* Configure whether voltage detection 0 circuit and HOCO are enabled after reset.
       OFS1 - Option Function Select Register 1
       b31:b9 Reserved (set to 1)
       b8     HOCOEN - Enable/disable HOCO oscillation after a reset (0=enable, 1=disable)
       b7:b3  Reserved When reading, these bits return the value written by the user. The write value should be 1.
       b2     LVDAS - Voltage Detection 0 circuit start (1=monitoring disabled)
       b1:b0  VDSEL - Voltage Detection 0 level select (1=2.94v, 2=2.87v, 3=2.80v) */
#define BSP_CFG_OFS1_REG_VALUE  (0xFFFFFFFF) //Disable by default

/* The RX65N have a 'Trusted Memory' feature. This feature protects against
 * illicit reading of blocks 8 and 9 in the code flash memory. This feature is disabled by default
 * but may be enabled by setting the below to (0xF8FFFFFF).
 * Only 0xF8FFFFFFF and 0xFFFFFFFF are valid settings. */
#define BSP_CFG_TRUSTED_MODE_FUNCTION  (0xFFFFFFFF) //Disable by default

/* Configure FAW register is used to set the write protection flag and boot area select flag 
   for setting the flash access window startaddress and flash access window end address.
       FAW - Flash Access Window Setting Register
       b31      BTFLG - Boot Area Select Flag - 0: FFFF C000h to FFFF DFFFh are used as the boot area
                                              - 1: FFFF E000h to FFFF FFFFh are used as the boot area
       b30:b28  Reserved - When reading, these bits return the value written by the user.The write value should be 1.
       b27:b16  FAWE - Flash Access Window End Address - Flash access window end address
       b15      FSPR - Access Window Protection Flag - 0: With protection (P/E disabled)
                                                     - 1: Without protection (P/E enabled)
       b14:b12  Reserved - When reading, these bits return the value written by the user.The write value should be 1.
       b11:b0   FAWS - Flash Access Window Start Address - Flash access window start address */
#define BSP_CFG_FAW_REG_VALUE  (0xFFFFFFFF) //Disable by default

/* This macro lets other modules no if a RTOS is being used.
   0 = RTOS is not used. 
   1 = RTOS is used.
*/
#define BSP_CFG_RTOS_USED               (0)

/* By default modules will use global locks found in mcu_locks.c. If the user is using a RTOS and would rather use its
   locking mechanisms then they can change this macro. 
   NOTE: If '1' is chosen for this macro then the user must also change the next macro 'BSP_CFG_USER_LOCKING_TYPE'.
   0 = Use default locking (non-RTOS)
   1 = Use user defined locking mechanism. 
*/
#define BSP_CFG_USER_LOCKING_ENABLED    (0)

/* If the user decides to use their own locking mechanism with FIT modules then they will need to redefine the typedef
   that is used for the locks. If the user is using a RTOS then they would likely redefine the typedef to be 
   a semaphore/mutex type of their RTOS. Use the macro below to set the type that will be used for the locks. 
   NOTE: If BSP_CFG_USER_LOCKING_ENABLED == 0 then this typedef is ignored.
   NOTE: Do not surround the type with parentheses '(' ')'.
*/
#define BSP_CFG_USER_LOCKING_TYPE       bsp_lock_t

/* If the user decides to use their own locking mechanism with FIT modules then they will need to define the functions
   that will handle the locking and unlocking. These functions should be defined below. 
   If BSP_CFG_USER_LOCKING_ENABLED is != 0:
   R_BSP_HardwareLock(mcu_lock_t hw_index) will call BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION(mcu_lock_t hw_index)
   R_BSP_HardwareUnlock(mcu_lock_t hw_index) will call BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION(mcu_lock_t hw_index)
       NOTE:With these functions the index into the array holding the global hardware locks is passed as the parameter.
   R_BSP_SoftwareLock(BSP_CFG_USER_LOCKING_TYPE * plock) will call 
   BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION(BSP_CFG_USER_LOCKING_TYPE * plock)
   R_BSP_SoftwareUnlock(BSP_CFG_USER_LOCKING_TYPE * plock) will call 
   BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION(BSP_CFG_USER_LOCKING_TYPE * plock)
       NOTE:With these functions the actual address of the lock to use is passed as the parameter.   
   NOTE: These functions must return a boolean. If lock was obtained or released successfully then return true. Else, 
         return false. 
   NOTE: If BSP_CFG_USER_LOCKING_ENABLED == 0 then this typedef is ignored.
   NOTE: Do not surround the type with parentheses '(' ')'.
*/
#define BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION      my_hw_locking_function
#define BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION    my_hw_unlocking_function
#define BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION      my_sw_locking_function
#define BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION    my_sw_unlocking_function

/* If the user would like to determine if a warm start reset has occurred, then they may enable one or more of the
 * following callback definitions AND provide a call back function name for the respective callback
 * function (to be defined by the user). Setting BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED = 1 will result
 * in a callback to the user defined my_sw_warmstart_prec_function just prior to the initialization of the C
 * runtime environment by resetprg.
 *
 * Setting BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED = 1 will result in a callback to the user defined
 * my_sw_warmstart_postc_function just after the initialization of the C runtime environment by resetprg.
 */
#define BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED    (0)
#define BSP_CFG_USER_WARM_START_PRE_C_FUNCTION     my_sw_warmstart_prec_function

#define BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED    (0)
#define BSP_CFG_USER_WARM_START_POST_C_FUNCTION    my_sw_warmstart_postc_function

/* By default FIT modules will check input parameters to be valid. This is helpful during development but some users
   will want to disable this for production code. The reason for this would be to save execution time and code space. 
   This macro is a global setting for enabling or disabling parameter checking. Each FIT module will also have its
   own local macro for this same purpose. By default the local macros will take the global value from here though
   they can be overridden. Therefore, the local setting has priority over this global setting. Disabling parameter
   checking should only used when inputs are known to be good and the increase in speed or decrease in code space is 
   needed. 
   0 = Global setting for parameter checking is disabled. 
   1 = Global setting for parameter checking is enabled (Default).
*/
#define BSP_CFG_PARAM_CHECKING_ENABLE               (1)

/* This macro is used to define the voltage that is supplied to the MCU (Vcc). This macro is defined in millivolts. This
   macro does not actually change anything on the MCU. Some FIT modules need this information so it is defined here. */
#define BSP_CFG_MCU_VCC_MV                          (3300)

/* There are multiple versions of the RSKRX65N. Choose which board is currently being used below.
   0 = 1st Prototype Board (R0K5RX65Nxxxxxx)
*/
#define BSP_CFG_BOARD_REVISION                      (0)

#endif /* R_BSP_CONFIG_REF_HEADER_FILE */



