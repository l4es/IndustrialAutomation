/*******************************************************************************
* Freescale Semiconductor Inc.
* (c) Copyright 2010 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
********************************************************************************
Services performed by FREESCALE in this matter are performed AS IS and without
any warranty. CUSTOMER retains the final decision relative to the total design
and functionality of the end product. FREESCALE neither guarantees nor will be
held liable by CUSTOMER for the success of this project.
FREESCALE DISCLAIMS ALL WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY INCLUDING,
BUT NOT LIMITED TO, IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR
A PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE ORE ADVISE SUPPLIED 
TO THE PROJECT BY FREESCALE, AND OR NAY PRODUCT RESULTING FROM FREESCALE 
SERVICES. IN NO EVENT SHALL FREESCALE BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THIS AGREEMENT.
CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands 
or actions by anyone on account of any damage, or injury, whether commercial,
contractual, or tortuous, rising directly or indirectly as a result 
of the advise or assistance supplied CUSTOMER in connection with product, 
services or goods supplied under this Agreement.
********************************************************************************
* File:             main.c
* Owner:            b08110
* Version:          2.0
* Date:             Apr-01-2011
* Classification:   General Business Information
* Brief:            Simple FlexCAN Tx Rx
********************************************************************************
* Detailed Description:
*
* Based on MPC5668G-GPIO-PinToggleStationery-V1_4 
*
* 1. CAN_A Tx data.
* 2. CAN_B Rx data.
* 3. Drive the received data on digital output.
* 4. toggle data and go back to 1
*
* Monitor digital output pin PH10 (P28 pin 11). It will toggle after each Tx/Rx.
*
* ------------------------------------------------------------------------------
* Test HW:  MPC5688EVB, PPC5668G Rev.A silicon
* Target :  internal_FLASH, RAM
* Fsys:     100 MHz PLL with 40 MHz crystal reference
* EVB:      CAN_A and CAN_B jumpers should be in their default positions.
*           For default jumper settings, please consult the EVB's User Manual.
*
* We will need to build a simple CAN bus and connect both CAN_A and CAN_B
* transceivers to it. 
*
* You can connect CAN_A_TOP connector to CAN_B_BOTTOM connector on the EVB with 
* a straight cable with CANNON9 male connectors on both ends.

* If you don't have such a cable, you can use two wires to connect P10 pin 1 to
* P11 pin 1 (CANH) and P10 pin 2 to P11 pin 2 (CANL).
*
********************************************************************************
Revision History:
1.0     Feb-18-2010     b08110  Initial Version
1.1     Mar-04-2010     b08110  Fixed PFCRP0 and PFCRP1 configuration
                                  prefetching disabled due to Rev.0 errata
1.2     Mar-18-2010     b08110  DisableWatchdog function added
1.3     Feb-18-2011     b08110  fixed external interrupts (IVOR4 init)
                                  PIT3 periodic interrupt 1s
1.4     Mar-29-2011     b08110  Corrected number of wait states
                                  and enabled prefetching and read buffers.
2.0     Apr-01-2011     b08110  Initial Version of this example. 
                                  Based on V1_4 stationery project.
*******************************************************************************/ 
#include "MPC5668G.h"
#include "INTCInterrupts.h"

/*******************************************************************************
* External objects
*******************************************************************************/

/* Prototype for PRC1 startup */
extern void __start_p1();

/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Constants and macros
*******************************************************************************/

#define PH10        122
#define PH12        124
#define LED_PAD     PH10
#define LED_PAD_2   PH12


#define LED_ON      SIU.GPDO[LED_PAD].R = 0
#define LED_OFF     SIU.GPDO[LED_PAD].R = 1
#define LED_TOGGLE  SIU.GPDO[LED_PAD].R ^= 0x01

#define FLASH_REG FLASH.PFCRP0.R
#define FLASH_DATA  0x08036B13
#define FLASH_REG2 FLASH.PFCRP1.R
#define FLASH_DATA2 0x30036B13

/* FlexCAN module register's bit masks */
#define CAN_MCR_MDIS        0x80000000
#define CAN_MCR_FRZ         0x40000000
#define CAN_MCR_FEN         0x20000000
#define CAN_MCR_HALT        0x10000000
#define CAN_MCR_NOTRDY      0x08000000
#define CAN_MCR_SOFTRST     0x02000000
#define CAN_MCR_FRZACK      0x01000000
#define CAN_MCR_WRNEN       0x00200000
#define CAN_MCR_LPMACK      0x00100000
#define CAN_MCR_SRXDIS      0x00020000
#define CAN_MCR_BCC         0x00010000
#define CAN_MCR_LPRIOEN     0x00002000
#define CAN_MCR_AEN         0x00001000

/*******************************************************************************
* Local types
*******************************************************************************/

/*******************************************************************************
* Local function prototypes
*******************************************************************************/
static void InitHW(void);
static void InitSysclk(void);
static void InitGPIO(void);
static void FlashConfig(void);
static void Delay(void);
static void Delay_1s(void);
static void DisableWatchdog(void);
static void PIT3_isr(void);
static void InitPIT3(void);

static void InitFlexCAN_A(void);
static void InitFlexCAN_B(void);

/*******************************************************************************
* Local variables
*******************************************************************************/ 

/*******************************************************************************
* Local functions
*******************************************************************************/

/*******************************************************************************
Function Name : InitHW
Engineer      : b08110
Date          : Feb-18-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : initialization of the hw for the purposes of this example
Issues        : NONE
*******************************************************************************/
static void InitHW(void)
{
    DisableWatchdog();
    InitSysclk();
    FlashConfig();
    InitGPIO();
    InitFlexCAN_A();
    InitFlexCAN_B();    
}

/*******************************************************************************
Function Name : InitSysclk
Engineer      : b08110
Date          : Feb-18-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : initialize Fsys 100 MHz PLL with 40 MHz crystal reference
                monitor PK9 (connector P30 pin 10 on MPC5668EVB) for Fsys/2
Issues        : NONE
*******************************************************************************/
static void InitSysclk(void)
{
    /* External clock enable. ECEN = 1 */
    /* External clock divide by 2. ECDF = 0b01 */
    SIU.ECCR.R = 0x00000009; 
    
    /* configure PK9 for CLKOUT */ 
    /* PA = 0b01 */
    /* OBE = 1 output */
    /* IBE = 0 */
    /* ODE = 0 */
    /* HYS = 0 */
    /* SRC = 0b11 max.slew rate */
    /* WPE = 0 */
    /* WPS = 0 */    
    SIU.PCR[153].R = 0x060C;
 
 #if 1 
    /* EN40MOSC is enabled out of reset by default */  
    /* enable external oscillator */ 
    CRP.CLKSRC.B.EN40MOSC = 0b1;      
 #endif
    
    /*
    1. determine the approptiate value EPREDIV, EMFD, ERFD. 
    for Fsys = 100 MHz with a 40 MHz crystal reference, we will get the final
    values: 

    EPREDIV = 9 (divide by 10) 
    EMFD = 84 (divide by 100) 
    ERFD = 3 (divide by 4) in this case, 
    Fvco will be 400 MHz. 
    */
         
    /* 2. write ERFD = 5 (divide by 6). disable FM (EDEPTH = 0) */ 
    FMPLL.ESYNCR2.R = 5; 

    /* 3. EPREDIV = 9 (div by 10), EMFD = 84 (div by 100) */ 
    FMPLL.ESYNCR1.R = 0xF0090054; 
 
    /* 4. wait for LOCK */ 
    while (FMPLL.SYNSR.B.LOCK != 1) {} 
 
    /* 5. write final ERFD = 3 (divide by 4). disable FM (EDEPTH = 0) */ 
    FMPLL.ESYNCR2.R = 3; 
 
    /* 6. select PLL as the Fsys */ 
    SIU.SYSCLK.B.SYSCLKSEL = 2;         
}

/*******************************************************************************
Function Name : DisableWatchdog
Engineer      : b08110
Date          : Mar-04-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : 
Issues        : 
*******************************************************************************/
static void DisableWatchdog(void)
{
    /* mm.l $fff38000 $ff000002 */
    SWT.CR.R = 0xFF000002;
}

/*******************************************************************************
Function Name : FlashConfig
Engineer      : b08110
Date          : Mar-04-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : PFCRP0 and PFCRP1 adjusted for actual Fsys
Issues        : expected Fsys = 100 MHz, IOP operates 50 MHz
*******************************************************************************/
static void FlashConfig(void) 
{
    unsigned int mem_write_code [] = 
    {

    #if __option(vle)
        /*for processors which support VLE only or for 'VLE on' option*/
        0xD0344400, /* stw r3,(0)r4 machine code: writes r3 contents to addr in r4 then se_nop*/
        0x7C0006AC, /* mbar machine code: ensure prior store completed */
        0x44000004  /* blr machine code: branches to return address in link register */
    #else
        0x90640000, /* stw r3,(0)r4 machine code: writes r3 contents to addr in r4 */
        0x7C0006AC, /* mbar machine code: ensure prior store completed */
        0x4E800020  /* blr machine code: branches to return address in link register */
    #endif // __option(vle)

    };
	

typedef void (*mem_write_code_ptr_t)(unsigned int, unsigned int);
 
    (*((mem_write_code_ptr_t)mem_write_code)) 	/* cast mem_write_code as func ptr*/
                                 /* * de-references func ptr, i.e. converts to func*/
        (FLASH_DATA,            /* which passes integer (in r3) */
        (unsigned int)&FLASH_REG);
      
#if defined(FLASH_REG2)
    (*((mem_write_code_ptr_t)mem_write_code)) 	/* cast mem_write_code as func ptr*/
                                 /* * de-references func ptr, i.e. converts to func*/
        (FLASH_DATA2,            /* which passes integer (in r3) */
        (unsigned int)&FLASH_REG2);
#endif 
 
}

/*******************************************************************************
Function Name : Delay
Engineer      : b08110
Date          : Feb-18-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : software delay
Issues        : NONE
*******************************************************************************/
static void Delay(void)
{
    uint32_t	temp = 0;
    while(temp < 10000000)
    {
        temp++;		
    }    
}

/*******************************************************************************
Function Name : Delay_1s
Engineer      : b08110
Date          : Feb-18-2011
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : - 1 second delay using the PIT3 module, polled implementation.
Issues        : - expecting 100 MHz fsys for the PIT timer
*******************************************************************************/
static void Delay_1s(void)
{
	static int32_t pit_initialized = 0;
	
	if (0 == pit_initialized)
	{
		/* 30: MDIS = 0 to enable clock for PITs. */
    	/* 31: FRZ = 1 for Timers stopped in debug mode */
    	PIT.MCR.R = 0x00000001;    
    	PIT.LDVAL3.R = 100000000 - 1;
    	
    	/* clear the TIF flag */
    	PIT.TFLG3.R = 0x00000001;
    	
    	/* 30: TIE = 0 for interrupt request disabled */
    	/* 31: TEN = 1 for timer active */
    	PIT.TCTRL3.R = 0x00000001;
    	
    	pit_initialized = 1;
	}
	
	while(0 == PIT.TFLG3.B.TIF)
	{
		/* wait for 1 sec boundary */
	}
	
	PIT.TFLG3.R = 0x00000001;
}

/*******************************************************************************
Function Name : InitPIT3
Engineer      : b08110
Date          : Feb-18-2011
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : - 1 second periodic interrup using the PIT3 module
Issues        : - expecting 100 MHz fsys for the PIT timer
*******************************************************************************/
static void InitPIT3(void)
{
	/* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    PIT.MCR.R = 0x00000001;    
    PIT.LDVAL3.R = 100000000 - 1;
    	
    /* clear the TIF flag */
    PIT.TFLG3.R = 0x00000001;
    	
    /* 30: TIE = 1 for interrupt request enabled */
    /* 31: TEN = 1 for timer active */
    PIT.TCTRL3.R = 0x00000003;
}

/*******************************************************************************
Function Name : PIT3_isr
Engineer      : b08110
Date          : Feb-18-2011
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : NONE
Issues        : NONE
*******************************************************************************/
static void PIT3_isr(void)
{
    /* toggle LED */
    LED_TOGGLE;

    PIT.TFLG3.R = 0x00000001;    
}

/*******************************************************************************
Function Name : InitGPIO
Engineer      : b08110
Date          : Feb-08-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : 
Issues        : NONE
*******************************************************************************/
static void InitGPIO(void)
{    	 	
 	SIU.GPDO[LED_PAD].R = 0;
 	SIU.PCR[LED_PAD].R = 0x020c; /* GPIO, output, max.slew rate */ 
 	
 	SIU.GPDO[LED_PAD_2].R = 0;
 	SIU.PCR[LED_PAD_2].R = 0x020c; /* GPIO, output, max.slew rate */  	
}

/*******************************************************************************
Function Name : InitFlexCAN_A
Engineer      : b08110
Date          : Mar-17-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : init FlexCAN_A module. including GPIO !
Issues        : NONE
*******************************************************************************/
static void InitFlexCAN_A(void)
{
    uint32_t i = 0;
    
    /* enable the FlexCAN module, reset and freeze */
    CAN_A.MCR.R = (0
                    | CAN_MCR_FRZ  /* enabled to enter Freeze mode */ 
                    | CAN_MCR_HALT /* enter freeze mode if FRZ bit is set */
                    | CAN_MCR_SOFTRST); /* soft reset */                    
                    
                    
    /* wait until soft reset completes */
    while(1 == CAN_A.MCR.B.SOFTRST) {}
    
    /* double check that we are actually in freeze mode */
    while(0 == CAN_A.MCR.B.NOTRDY) {}
    
    CAN_A.MCR.R = (0
                    | CAN_MCR_FRZ  /* enabled to enter Freeze mode */ 
                    | CAN_MCR_HALT /* enter freeze mode if FRZ bit is set */                    
                    | CAN_MCR_SRXDIS /* self reception disabled */
                    | CAN_MCR_BCC  /* individual Rx masking and queue */
                    | CAN_MCR_AEN  /* Safe Tx abort enable */
                    | 0x0000003F); /* enable 64 MBs */        
    
    /* configure CNTX_A and CNRX_A pin functions on PD0 and PD1 */
    SIU.PCR[48].R = 0x060C; /* CNTX_A, push/pull, max slew rate */
    SIU.PCR[49].R = 0x0500; /* CNRX_A, weak pull device disabled */
    
    /* Configure for OSC (40 MHz on MPC5668EVB), 1 MHz bit time */
    /* CAN serial clock frequency = 20.00 MHz => PRESDIV = 1 (divide by 2)
     * CAN bus nominal bit time: 
     *     Synchronization segment = 1 time quantum 
     *     Propagation time segment = 8 time quanta => PROPSEG = 7
     *     Phase buffer segment1 = 5 time quanta => PSEG1 = 4 
     *     Phase buffer segment2 = 6 time quanta => PSEG2 = 5 
     * CAN bus nominal bit rate = 1.00 MHz 
     * Sampling of the received bit occurs at end of PHASE_SEG1 
     * Resynchronization jump width = 4 time quanta */    
#if 1
    CAN_A.CTRL.R = 0x01E50087; /* Configure for 40MHz OSC, 1.0MHz bit time */
#else
    CAN_A.CTRL.R = 0x04DB0006; /* Configure for 40MHz OSC, 500KHz bit time */
#endif

    
    /* Make first 64 message buffers inactive by writing their control fields 
     * to "not active". They will be left
     * inactive until we're ready for communication. */      
    for(i=0;i<64;i++)
    {
        CAN_A.BUF[i].CS.R = 0;
    }
    
    /* set mask registers - all ID bits must match */
    for(i=0;i<64;i++)
    {
        CAN_A.RXIMR[i].R = 0x1FFFFFFF;
    }
    
    /* Finally clear the HALT flag in MCR to enable the FlexCAN
     * to synchronize with the CAN bus and allow
     * participation in communication. */
    CAN_A.MCR.B.HALT = 0;
    
    /* wait until FlexCAN ready */
    while(1 == CAN_A.MCR.B.NOTRDY) {}
}

/*******************************************************************************
Function Name : InitFlexCAN_B
Engineer      : b08110
Date          : Mar-17-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : init FlexCAN_B module. including GPIO !
Issues        : NONE
*******************************************************************************/
static void InitFlexCAN_B(void)
{    	 	
    uint32_t i = 0;
    
    /* enable the FlexCAN module, reset and freeze */
    CAN_B.MCR.R = (0
                    | CAN_MCR_FRZ  /* enabled to enter Freeze mode */ 
                    | CAN_MCR_HALT /* enter freeze mode if FRZ bit is set */
                    | CAN_MCR_SOFTRST); /* soft reset */                    
                    
                    
    /* wait until soft reset completes */
    while(1 == CAN_B.MCR.B.SOFTRST) {}
    
    /* double check that we are actually in freeze mode */
    while(0 == CAN_B.MCR.B.NOTRDY) {}
    
    CAN_B.MCR.R = (0
                    | CAN_MCR_FRZ  /* enabled to enter Freeze mode */ 
                    | CAN_MCR_HALT /* enter freeze mode if FRZ bit is set */                    
                    | CAN_MCR_SRXDIS /* self reception disabled */
                    | CAN_MCR_BCC  /* individual Rx masking and queue */
                    | CAN_MCR_AEN  /* Safe Tx abort enable */
                    | 0x0000003F); /* enable 64 MBs */
    
    /* configure CNTX_B and CNRX_B pin functions on PD2 and PD3 */
    SIU.PCR[50].R = 0x060C; /* CNTX_B, push/pull, max slew rate */
    SIU.PCR[51].R = 0x0500; /* CNRX_B, weak pull device disabled */
    
    /* Configure for OSC (40 MHz on MPC5668EVB), 1 MHz bit time */
    /* CAN serial clock frequency = 20.00 MHz => PRESDIV = 1 (divide by 2)
     * CAN bus nominal bit time: 
     *     Synchronization segment = 1 time quantum 
     *     Propagation time segment = 8 time quanta => PROPSEG = 7
     *     Phase buffer segment1 = 5 time quanta => PSEG1 = 4 
     *     Phase buffer segment2 = 6 time quanta => PSEG2 = 5 
     * CAN bus nominal bit rate = 1.00 MHz 
     * Sampling of the received bit occurs at end of PHASE_SEG1 
     * Resynchronization jump width = 4 time quanta */
#if 1
    CAN_B.CTRL.R = 0x01E50087; /* Configure for 40MHz OSC, 1.0MHz bit time */
#else
    CAN_B.CTRL.R = 0x04DB0006; /* Configure for 40MHz OSC, 500KHz bit time */
#endif
    
    /* Make first 64 message buffers inactive by writing their control fields 
     * to "not active". They will be left
     * inactive until we're ready for communication. */
    for(i=0;i<64;i++)
    {
        CAN_B.BUF[i].CS.R = 0;
    }
    
    /* set mask registers - all bits of received frame's ID must match */
    for(i=0;i<64;i++)
    {
        CAN_B.RXIMR[i].R = 0x1FFFFFFF;
    }
    
    /* Finally clear the HALT flag in MCR to enable the FlexCAN
     * to synchronize with the CAN bus and allow
     * participation in communication. */
    CAN_B.MCR.B.HALT = 0;
    
    /* wait until FlexCAN ready */
    while(1 == CAN_B.MCR.B.NOTRDY) {} 	
}



/*******************************************************************************
* Global functions
*******************************************************************************/

/*******************************************************************************
Function Name : main
Engineer      : b08110
Date          : Mar-18-2010
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : application's main finite state machine
Issues        : NONE
*******************************************************************************/
int32_t main(void) 
{
    uint32_t i = 0;
    
    uint32_t  fsm = 0;       /* application's finite state machine */
    uint8_t   u8_txdata = 0; /* tranmitted data */
    uint8_t   u8_rxdata = 0; /* received data */    
    vuint32_t temp = 0;      /* temporary and volatile 32bit */
    vuint8_t  u8_temp = 0;   /* temporary and volatile 8bit */


    InitHW();    
    
    /* Install PIT3_isr into the interrupt vector table */
    INTC_InstallINTCInterruptHandler(PIT3_isr, 151, 1);
    
    /* lower current INTC priority to start INTC interrupts */
    INTC.CPR_PRC0.R = 0;
    
    /* 1 sec periodical interrupt */
    InitPIT3();

    
#if 0
    /* not using the Z0 core in this example */

    /* Start the other core by writing CRP.Z0VEC.R */ 
    CRP.Z0VEC.R = (unsigned long)__start_p1;
#endif 

    /* Toggle LED forever in the PIT3 isr */
    
    
    /* subscribe FlexCAN_B's MB9 for Rx - extended frame format */
    CAN_B.BUF[9].CS.R = 0x00200000; /* Rx inactive + IDE */
    CAN_B.BUF[9].ID.R = 0x12340000; /* Ext.ID = 0x12340000 */
    CAN_B.BUF[9].CS.R = 0x04200000; /* Rx active and empty + IDE */

    /* application's finite state machine */
    while(1)
    {
        switch(fsm)
        {
            case 0:
                /* prepare data to send */                
                u8_txdata ^= 0x01; /* toggle bit 0 */
            
                /* CAN_A MB8 Tx Send Once, data length to send = 1 byte */
                CAN_A.BUF[8].CS.R = 0x08600000; /* Tx INACTIVE + IDE + SRR */
                CAN_A.BUF[8].ID.R = 0x12340000; /* Ext.ID = 0x12340000 */
                CAN_A.BUF[8].DATA.B[0] = u8_txdata; /* data to send */
                CAN_A.BUF[8].CS.R = 0x0C610000; /* send 1 byte, ext.frame */
                
                fsm = 1;
            break;
            
            case 1:
                /* Poll CAN_A's Tx flag to track successfull transmission */
                if(1 == CAN_A.IFLAG1.B.BUF08I) 
                {
                    /* clear the BUF08I bit - bit 8 */
                    CAN_A.IFLAG1.R = 0x00000100;

                    fsm = 2;
                }
            break;
            
            case 2:
                /* Poll CAN_B's Rx flag to track successfull reception */
                if(1 == CAN_B.IFLAG1.B.BUF09I) 
                {
                    /* upon receiving the interrupt flag */
                
                    /* mandatory - read control/status word - lock the MB */
                    u8_temp = *(vuint8_t*)&CAN_B.BUF[9].CS.R;

                    /* check the ID field */
                    /* not really needed in this example */
                    /* useful in case when multiple IDs are received
                     * by one message buffer (Rx mask feature) */ 
                    temp = CAN_B.BUF[9].ID.R;
                    if(0x12340000 != temp)
                    {
                        /* error ! trap here.
                         * we have configured this MB to receive 
                         * just ID 0x12340000 and not another */
                        while(1) {}
                    }

                    /* read the received data */
                    u8_rxdata = CAN_B.BUF[9].DATA.B[0];
                    
                    /* release the internal lock for all Rx MBs
                     * by reading the TIMER */
                    temp = CAN_B.TIMER.R; 
                
                    /* clear the BUF09I bit - bit 9 */
                    CAN_B.IFLAG1.R = 0x00000200;

                    fsm = 3;
                }
            break;
            
            case 3:
                /* do someting with the received data */
                /* drive the output pin with the received data */
                SIU.GPDO[LED_PAD_2].R = u8_rxdata;   
                            
                /* repeat */
                fsm = 0;
            break;
        }
    }
}



