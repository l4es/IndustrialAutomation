/**
  ******************************************************************************
  * @file    IO_Toggle/stm32f0xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"

/** @addtogroup STM32F0_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup IO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
//void SPI1_IRQHandler(void)
//{
//    //SPI1->SR &=~(0x0001);
//    volatile uint8_t data;
//    data = SPI_ReceiveData8(SPI1);
//    GPIOA->BSRR = GPIO_Pin_4;
//}
//
//void DMA1_Channel2_3_IRQHandler(void)
//{
//
//    //SPI1->SR &=~(0x0001);
//    if(DMA_GetFlagStatus(DMA1_FLAG_GL2) == SET)
//        DMA_ClearFlag(DMA1_FLAG_GL2);
//    if(DMA_GetFlagStatus(DMA1_FLAG_GL3) == SET)
//    {
//        if(DMA_GetFlagStatus(DMA1_FLAG_TC3) == SET)
//        {
//            while(SPI_GetTransmissionFIFOStatus(SPI1) !=0 );
//            while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)== SET);
//            while(SPI_GetReceptionFIFOStatus(SPI1) != 0)
//            {
//                SPI_ReceiveData8(SPI1);
//            }
//            SPI_Cmd(SPI1, DISABLE);
//        }
//        DMA_ClearFlag(DMA1_FLAG_GL3);
//    }
//    GPIO_WriteBit(GPIOC,GPIO_Pin_8, RESET);
//
//}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
