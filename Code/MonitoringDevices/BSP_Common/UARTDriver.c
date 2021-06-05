/**
  ******************************************************************************
  * File Name          : UARTDriver.c
  * Description        : The UART initialization routines.
  ******************************************************************************
 
  ******************************************************************************
  */

//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

#ifdef HAL_UART_MODULE_ENABLED

//---------------------------- Defines & Structures ----------------------------
/** @defgroup UART_Stop_Bits   UART Number of Stop Bits
  * @{
  */
#define UART_STOPBITS_1                     (0x00000000U)                                     /*!< UART frame with 1 stop bit    */
#define UART_STOPBITS_2                     ((uint32_t)USART_CR2_STOP_1)                      /*!< UART frame with 2 stop bits   */

/** @defgroup UART_Parity  UART Parity
  * @{
  */
#define UART_PARITY_NONE                    (0x00000000U)                                  /*!< No parity   */
#define UART_PARITY_EVEN                    ((uint32_t)USART_CR1_PCE)                      /*!< Even parity */
#define UART_PARITY_ODD                     ((uint32_t)(USART_CR1_PCE | USART_CR1_PS))     /*!< Odd parity  */


/** @defgroup UART_Hardware_Flow_Control UART Hardware Flow Control
  * @{
  */
#define UART_HWCONTROL_NONE                  (0x00000000U)                                    /*!< No hardware control       */
#define UART_HWCONTROL_RTS                   ((uint32_t)USART_CR3_RTSE)                       /*!< Request To Send           */
#define UART_HWCONTROL_CTS                   ((uint32_t)USART_CR3_CTSE)                       /*!< Clear To Send             */
#define UART_HWCONTROL_RTS_CTS               ((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE))    /*!< Request and Clear To Send */

/** @defgroup UART_Mode UART Transfer Mode
  * @{
  */
#define UART_MODE_RX                        ((uint32_t)USART_CR1_RE)                    /*!< RX mode        */ 
#define UART_MODE_TX                        ((uint32_t)USART_CR1_TE)                    /*!< TX mode        */ 
#define UART_MODE_TX_RX                     ((uint32_t)(USART_CR1_TE |USART_CR1_RE))    /*!< RX and TX mode */ 

/** @defgroup UART_OneBit_Sampling UART One Bit Sampling Method
  * @{
  */
#define UART_ONE_BIT_SAMPLE_DISABLE         (0x00000000U)                   /*!< One-bit sampling disable */
#define UART_ONE_BIT_SAMPLE_ENABLE          ((uint32_t)USART_CR3_ONEBIT)    /*!< One-bit sampling enable  */

/** @defgroup UART_Receiver_TimeOut UART Receiver TimeOut
  * @{
  */
#define UART_RECEIVER_TIMEOUT_DISABLE       (0x00000000U)                   /*!< UART receiver timeout disable */
#define UART_RECEIVER_TIMEOUT_ENABLE        ((uint32_t)USART_CR2_RTOEN)     /*!< UART receiver timeout enable  */

/** @defgroup UARTEx_Word_Length UARTEx Word Length
  * @{
  */
#define UART_WORDLENGTH_8B                  (0x00000000U)              /*!< 8-bit long UART frame */
#define UART_WORDLENGTH_9B                  ((uint32_t)USART_CR1_M)    /*!< 9-bit long UART frame */

/** @defgroup UART_Over_Sampling UART Over Sampling
  * @{
  */
#define UART_OVERSAMPLING_16                (0x00000000U)                   /*!< Oversampling by 16 */
#define UART_OVERSAMPLING_8                 ((uint32_t)USART_CR1_OVER8)     /*!< Oversampling by 8  */

/** @defgroup UART_DMA_Tx    UART DMA Tx
  * @{
  */
#define UART_DMA_TX_DISABLE                 (0x00000000U)                   /*!< UART DMA TX disabled */
#define UART_DMA_TX_ENABLE                  ((uint32_t)USART_CR3_DMAT)      /*!< UART DMA TX enabled  */

/** @defgroup UART_OneBit_Sampling UART One Bit Sampling Method
  * @{
  */
#define UART_ONE_BIT_SAMPLE_DISABLE         (0x00000000U)                   /*!< One-bit sampling disable */
#define UART_ONE_BIT_SAMPLE_ENABLE          ((uint32_t)USART_CR3_ONEBIT)    /*!< One-bit sampling enable  */

/** @brief  BRR division operation to set BRR register in 8-bit oversampling mode.
  * @param  __PCLK__ UART clock.
  * @param  __BAUD__ Baud rate set by the user.
  * @retval Division result
  */
#define UART_DIV_SAMPLING8(__PCLK__, __BAUD__)   ((((__PCLK__)*2U) + ((__BAUD__)/2U)) / (__BAUD__))

/** @brief  BRR division operation to set BRR register in 16-bit oversampling mode.
  * @param  __PCLK__ UART clock.
  * @param  __BAUD__ Baud rate set by the user.
  * @retval Division result
  */
#define UART_DIV_SAMPLING16(__PCLK__, __BAUD__)  (((__PCLK__) + ((__BAUD__)/2U)) / (__BAUD__))


/*!< UART or USART CR1 fields of parameters set by UART_SetConfig API */
#define UART_CR1_FIELDS  ((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | \
                                     USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8)) 

/** @brief  Enable UART.
  * @param  __HANDLE__ specifies the UART Handle.
  * @retval None
  */
#define __HAL_UART_ENABLE(__HANDLE__)                   ((__HANDLE__)->CR1 |=  USART_CR1_UE)

/** @brief  Disable UART.
  * @param  __HANDLE__ specifies the UART Handle.
  * @retval None
  */
#define __HAL_UART_DISABLE(__HANDLE__)                  ((__HANDLE__)->CR1 &=  ~USART_CR1_UE)

/** @defgroup UART_Interruption_Mask    UART Interruptions Flag Mask
  * @{
  */
#define UART_IT_MASK                        (0x001FU)                  /*!< UART interruptions flags mask */

/** @defgroup UART_Interrupt_definition   UARTEx Interrupts Definition
  *        Elements values convention: 000ZZZZZ0XXYYYYYb
  *           - YYYYY  : Interrupt source position in the XX register (5bits)
  *           - XX  : Interrupt source register (2bits)
  *                 - 01: CR1 register
  *                 - 10: CR2 register
  *                 - 11: CR3 register
  *           - ZZZZZ  : Flag position in the ISR register(5bits)
  * @{
  */
#define UART_IT_PE                          (0x0028U)                  /*!< UART parity error interruption                 */
#define UART_IT_TXE                         (0x0727U)                  /*!< UART transmit data register empty interruption */
#define UART_IT_TC                          (0x0626U)                  /*!< UART transmission complete interruption        */
#define UART_IT_RXNE                        (0x0525U)                  /*!< UART read data register not empty interruption */
#define UART_IT_IDLE                        (0x0424U)                  /*!< UART idle interruption                         */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) 
#define UART_IT_LBD                         (0x0846U)                  /*!< UART LIN break detection interruption          */
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */ 
#define UART_IT_CTS                         (0x096AU)                  /*!< UART CTS interruption                          */
#define UART_IT_CM                          (0x112EU)                  /*!< UART character match interruption              */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) 
#define UART_IT_WUF                         (0x1476U)                  /*!< UART wake-up from stop mode interruption       */
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */ 

/** @brief  Enable the specified UART interrupt.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __INTERRUPT__ specifies the UART interrupt source to enable.
  *          This parameter can be one of the following values:
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_IT_WUF  Wakeup from stop mode interrupt (not available on F030xx devices)
  @endif
  *            @arg @ref UART_IT_CM   Character match interrupt
  *            @arg @ref UART_IT_CTS  CTS change interrupt
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_IT_LBD  LIN Break detection interrupt (not available on F030xx devices)
  @endif
  *            @arg @ref UART_IT_TXE  Transmit Data Register empty interrupt
  *            @arg @ref UART_IT_TC   Transmission complete interrupt
  *            @arg @ref UART_IT_RXNE Receive Data register not empty interrupt
  *            @arg @ref UART_IT_IDLE Idle line detection interrupt
  *            @arg @ref UART_IT_PE   Parity Error interrupt
  *            @arg @ref UART_IT_ERR  Error interrupt (Frame error, noise error, overrun error)
  * @retval None
  */
#define __HAL_UART_ENABLE_IT(__HANDLE__, __INTERRUPT__)   (((((uint8_t)(__INTERRUPT__)) >> 5U) == 1U)? ((__HANDLE__)->CR1 |= (1U << ((__INTERRUPT__) & UART_IT_MASK))): \
                                                           ((((uint8_t)(__INTERRUPT__)) >> 5U) == 2U)? ((__HANDLE__)->CR2 |= (1U << ((__INTERRUPT__) & UART_IT_MASK))): \
                                                           ((__HANDLE__)->CR3 |= (1U << ((__INTERRUPT__) & UART_IT_MASK))))


/** @brief  Disable the specified UART interrupt.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __INTERRUPT__ specifies the UART interrupt source to disable.
  *          This parameter can be one of the following values:
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_IT_WUF  Wakeup from stop mode interrupt (not available on F030xx devices)
  @endif
  *            @arg @ref UART_IT_CM   Character match interrupt
  *            @arg @ref UART_IT_CTS  CTS change interrupt
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_IT_LBD  LIN Break detection interrupt (not available on F030xx devices)
  @endif
  *            @arg @ref UART_IT_TXE  Transmit Data Register empty interrupt
  *            @arg @ref UART_IT_TC   Transmission complete interrupt
  *            @arg @ref UART_IT_RXNE Receive Data register not empty interrupt
  *            @arg @ref UART_IT_IDLE Idle line detection interrupt
  *            @arg @ref UART_IT_PE   Parity Error interrupt
  *            @arg @ref UART_IT_ERR  Error interrupt (Frame error, noise error, overrun error)
  * @retval None
  */
#define __HAL_UART_DISABLE_IT(__HANDLE__, __INTERRUPT__)  (((((uint8_t)(__INTERRUPT__)) >> 5U) == 1U)? ((__HANDLE__)->CR1 &= ~ (1U << ((__INTERRUPT__) & UART_IT_MASK))): \
                                                           ((((uint8_t)(__INTERRUPT__)) >> 5U) == 2U)? ((__HANDLE__)->CR2 &= ~ (1U << ((__INTERRUPT__) & UART_IT_MASK))): \
                                                           ((__HANDLE__)->CR3 &= ~ (1U << ((__INTERRUPT__) & UART_IT_MASK))))

/** @brief  Check whether the specified UART interrupt has occurred or not.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __IT__ specifies the UART interrupt to check.
  *          This parameter can be one of the following values:
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_IT_WUF  Wakeup from stop mode interrupt (not available on F030xx devices)
  @endif
  *            @arg @ref UART_IT_CM   Character match interrupt
  *            @arg @ref UART_IT_CTS  CTS change interrupt
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_IT_LBD  LIN Break detection interrupt (not available on F030xx devices)
  @endif
  *            @arg @ref UART_IT_TXE  Transmit Data Register empty interrupt
  *            @arg @ref UART_IT_TC   Transmission complete interrupt
  *            @arg @ref UART_IT_RXNE Receive Data register not empty interrupt
  *            @arg @ref UART_IT_IDLE Idle line detection interrupt
  *            @arg @ref UART_IT_ORE  Overrun Error interrupt
  *            @arg @ref UART_IT_NE   Noise Error interrupt
  *            @arg @ref UART_IT_FE   Framing Error interrupt
  *            @arg @ref UART_IT_PE   Parity Error interrupt
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_UART_GET_IT(__HANDLE__, __IT__) ((__HANDLE__)->ISR & (1U << ((__IT__)>> 0x08U)))

/** @defgroup UART_Flags     UARTEx Status Flags
  *        Elements values convention: 0xXXXX
  *           - 0xXXXX  : Flag mask in the ISR register
  * @{
  */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC)
#define UART_FLAG_REACK                     (0x00400000U)              /*!< UART receive enable acknowledge flag      */
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */ 
#define UART_FLAG_TEACK                     (0x00200000U)              /*!< UART transmit enable acknowledge flag     */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC)
#define UART_FLAG_WUF                       (0x00100000U)              /*!< UART wake-up from stop mode flag          */
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */ 
#define UART_FLAG_RWU                       (0x00080000U)              /*!< UART receiver wake-up from mute mode flag */
#define UART_FLAG_SBKF                      (0x00040000U)              /*!< UART send break flag                      */
#define UART_FLAG_CMF                       (0x00020000U)              /*!< UART character match flag                 */
#define UART_FLAG_BUSY                      (0x00010000U)              /*!< UART busy flag                            */
#define UART_FLAG_ABRF                      (0x00008000U)              /*!< UART auto Baud rate flag                  */
#define UART_FLAG_ABRE                      (0x00004000U)              /*!< UART auto Baud rate error                 */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC)
#define UART_FLAG_EOBF                      (0x00001000U)              /*!< UART end of block flag                    */
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */ 
#define UART_FLAG_RTOF                      (0x00000800U)              /*!< UART receiver timeout flag                */
#define UART_FLAG_CTS                       (0x00000400U)              /*!< UART clear to send flag                   */
#define UART_FLAG_CTSIF                     (0x00000200U)              /*!< UART clear to send interrupt flag         */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC)
#define UART_FLAG_LBDF                      (0x00000100U)              /*!< UART LIN break detection flag (not available on F030xx devices)*/
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */ 
#define UART_FLAG_TXE                       (0x00000080U)              /*!< UART transmit data register empty         */
#define UART_FLAG_TC                        (0x00000040U)              /*!< UART transmission complete                */
#define UART_FLAG_RXNE                      (0x00000020U)              /*!< UART read data register not empty         */
#define UART_FLAG_IDLE                      (0x00000010U)              /*!< UART idle flag                            */
#define UART_FLAG_ORE                       (0x00000008U)              /*!< UART overrun error                        */
#define UART_FLAG_NE                        (0x00000004U)              /*!< UART noise error                          */
#define UART_FLAG_FE                        (0x00000002U)              /*!< UART frame error                          */
#define UART_FLAG_PE                        (0x00000001U)              /*!< UART parity error                         */


/** @brief  Check whether the specified UART interrupt source is enabled or not.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __IT__ specifies the UART interrupt source to check.
  *          This parameter can be one of the following values:
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_IT_WUF  Wakeup from stop mode interrupt (not available on F030xx devices)
  @endif
  *            @arg @ref UART_IT_CM   Character match interrupt  
  *            @arg @ref UART_IT_CTS  CTS change interrupt
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_IT_LBD  LIN Break detection interrupt (not available on F030xx devices)
  @endif
  *            @arg @ref UART_IT_TXE  Transmit Data Register empty interrupt
  *            @arg @ref UART_IT_TC   Transmission complete interrupt
  *            @arg @ref UART_IT_RXNE Receive Data register not empty interrupt
  *            @arg @ref UART_IT_IDLE Idle line detection interrupt
  *            @arg @ref UART_IT_ERR  Error interrupt (Frame error, noise error, overrun error)
  *            @arg @ref UART_IT_PE   Parity Error interrupt
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_UART_GET_IT_SOURCE(__HANDLE__, __IT__) ((((((uint8_t)(__IT__)) >> 5U) == 1U)? (__HANDLE__)->CR1:(((((uint8_t)(__IT__)) >> 5U) == 2U)? \
                                                       (__HANDLE__)->CR2 : (__HANDLE__)->CR3)) & (1U << (((uint16_t)(__IT__)) & UART_IT_MASK)))
													   

/** @brief  Check whether the specified UART flag is set or not.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __FLAG__ specifies the flag to check.
  *        This parameter can be one of the following values:
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_FLAG_REACK Receive enable acknowledge flag
  @endif
  *            @arg @ref UART_FLAG_TEACK Transmit enable acknowledge flag
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_FLAG_WUF   Wake up from stop mode flag (not available on F030xx devices)
  @endif
  *            @arg @ref UART_FLAG_RWU   Receiver wake up flag (not available on F030xx devices)
  *            @arg @ref UART_FLAG_SBKF  Send Break flag
  *            @arg @ref UART_FLAG_CMF   Character match flag
  *            @arg @ref UART_FLAG_BUSY  Busy flag
  *            @arg @ref UART_FLAG_ABRF  Auto Baud rate detection flag
  *            @arg @ref UART_FLAG_ABRE  Auto Baud rate detection error flag
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_FLAG_EOBF  End of block flag (not available on F030xx devices) 
  @endif
  *            @arg @ref UART_FLAG_RTOF  Receiver timeout flag
  *            @arg @ref UART_FLAG_CTS   CTS Change flag
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_FLAG_LBDF  LIN Break detection flag (not available on F030xx devices)
  @endif
  *            @arg @ref UART_FLAG_TXE   Transmit data register empty flag
  *            @arg @ref UART_FLAG_TC    Transmission Complete flag
  *            @arg @ref UART_FLAG_RXNE  Receive data register not empty flag
  *            @arg @ref UART_FLAG_IDLE  Idle Line detection flag
  *            @arg @ref UART_FLAG_ORE   Overrun Error flag
  *            @arg @ref UART_FLAG_NE    Noise Error flag
  *            @arg @ref UART_FLAG_FE    Framing Error flag
  *            @arg @ref UART_FLAG_PE    Parity Error flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_UART_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->ISR & (__FLAG__)) == (__FLAG__))

/** @defgroup UART_IT_CLEAR_Flags  UARTEx Interruption Clear Flags
  * @{
  */
#define UART_CLEAR_PEF                       USART_ICR_PECF            /*!< Parity Error Clear Flag           */
#define UART_CLEAR_FEF                       USART_ICR_FECF            /*!< Framing Error Clear Flag          */
#define UART_CLEAR_NEF                       USART_ICR_NCF             /*!< Noise detected Clear Flag         */
#define UART_CLEAR_OREF                      USART_ICR_ORECF           /*!< Overrun Error Clear Flag          */
#define UART_CLEAR_IDLEF                     USART_ICR_IDLECF          /*!< IDLE line detected Clear Flag     */
#define UART_CLEAR_TCF                       USART_ICR_TCCF            /*!< Transmission Complete Clear Flag  */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC)
#define UART_CLEAR_LBDF                      USART_ICR_LBDCF           /*!< LIN Break Detection Clear Flag (not available on F030xx devices)*/
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */    
#define UART_CLEAR_CTSF                      USART_ICR_CTSCF           /*!< CTS Interrupt Clear Flag          */
#define UART_CLEAR_RTOF                      USART_ICR_RTOCF           /*!< Receiver Time Out Clear Flag      */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC)
#define UART_CLEAR_EOBF                      USART_ICR_EOBCF           /*!< End Of Block Clear Flag           */
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */    
#define UART_CLEAR_CMF                       USART_ICR_CMCF            /*!< Character Match Clear Flag        */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC)      
#define UART_CLEAR_WUF                       USART_ICR_WUCF            /*!< Wake Up from stop mode Clear Flag */
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */ 

/** @brief  Clear the specified UART ISR flag, in setting the proper ICR register flag.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __IT_CLEAR__ specifies the interrupt clear register flag that needs to be set
  *                       to clear the corresponding interrupt
  *          This parameter can be one of the following values:
  *            @arg @ref UART_CLEAR_PEF Parity Error Clear Flag
  *            @arg @ref UART_CLEAR_FEF Framing Error Clear Flag
  *            @arg @ref UART_CLEAR_NEF Noise detected Clear Flag
  *            @arg @ref UART_CLEAR_OREF Overrun Error Clear Flag
  *            @arg @ref UART_CLEAR_IDLEF IDLE line detected Clear Flag
  *            @arg @ref UART_CLEAR_TCF Transmission Complete Clear Flag
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_CLEAR_LBDF LIN Break Detection Clear Flag (not available on F030xx devices)
  @endif
  *            @arg @ref UART_CLEAR_CTSF CTS Interrupt Clear Flag
  *            @arg @ref UART_CLEAR_RTOF Receiver Time Out Clear Flag
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_CLEAR_EOBF End Of Block Clear Flag
  @endif
  *            @arg @ref UART_CLEAR_CMF Character Match Clear Flag
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_CLEAR_WUF  Wake Up from stop mode Clear Flag (not available on F030xx devices)
  @endif
  * @retval None
  */
#define __HAL_UART_CLEAR_IT(__HANDLE__, __IT_CLEAR__) ((__HANDLE__)->ICR = (uint32_t)(__IT_CLEAR__))


/** @defgroup UART_Request_Parameters UARTEx Request Parameters
  * @{
  */
#define UART_AUTOBAUD_REQUEST               ((uint32_t)USART_RQR_ABRRQ)        /*!< Auto-Baud Rate Request      */
#define UART_SENDBREAK_REQUEST              ((uint32_t)USART_RQR_SBKRQ)        /*!< Send Break Request          */
#define UART_MUTE_MODE_REQUEST              ((uint32_t)USART_RQR_MMRQ)         /*!< Mute Mode Request           */
#define UART_RXDATA_FLUSH_REQUEST           ((uint32_t)USART_RQR_RXFRQ)        /*!< Receive Data flush Request  */
#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC)      
#define UART_TXDATA_FLUSH_REQUEST           ((uint32_t)USART_RQR_TXFRQ)        /*!< Transmit data flush Request */
#else
#endif /* !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6)  && !defined(STM32F070xB)  && !defined(STM32F030xC) */ 


/** @brief  Set a specific UART request flag.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __REQ__ specifies the request flag to set
  *          This parameter can be one of the following values:
  *            @arg @ref UART_AUTOBAUD_REQUEST Auto-Baud Rate Request
  *            @arg @ref UART_SENDBREAK_REQUEST Send Break Request
  *            @arg @ref UART_MUTE_MODE_REQUEST Mute Mode Request
  *            @arg @ref UART_RXDATA_FLUSH_REQUEST Receive Data flush Request
  @if STM32F030x6
  @elseif STM32F030x8
  @elseif STM32F030xC
  @elseif STM32F070x6
  @elseif STM32F070xB
  @else
  *            @arg @ref UART_TXDATA_FLUSH_REQUEST Transmit data flush Request (not available on F030xx devices)
  @endif
  * @retval None
  */
#define __HAL_UART_SEND_REQ(__HANDLE__, __REQ__) ((__HANDLE__)->RQR |= (uint32_t)(__REQ__))

//---------------------------- Static Variables --------------------------------



//---------------------------- Global Variables --------------------------------
UART_INSTANT_t gUart1Instant;
UART_INSTANT_t gUart3Instant;


//------------------------- Extern Global Variables ----------------------------


//--------------------------- Private function prototypes ----------------------
static void UART_InitPrams(UART_InitTypeDef *uartParamsCopiedInto, 
						   UART_InitTypeDef *uartParams);

static void UART_InitCommunication(UART_INSTANT_t *uartInstance);

void Init_UARTs(void)
{
	UART_InitTypeDef modbusParams;
	UART_InitTypeDef debugParams;
	
	modbusParams.BaudRate = 19200;
	modbusParams.WordLength = UART_WORDLENGTH_8B;
	modbusParams.HwFlowCtl = UART_HWCONTROL_NONE;
	modbusParams.Parity = UART_PARITY_NONE;
	modbusParams.StopBits = UART_STOPBITS_1;
	modbusParams.OverSampling = UART_OVERSAMPLING_16;
	modbusParams.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	modbusParams.Mode = (UART_MODE_TX | UART_MODE_RX);
	
	UART_Init(UART1_INSTANCE, &modbusParams);
	
	debugParams.BaudRate = 115200;
	debugParams.WordLength = UART_WORDLENGTH_8B;
	debugParams.HwFlowCtl = UART_HWCONTROL_NONE;
	debugParams.Parity = UART_PARITY_NONE;
	debugParams.StopBits = UART_STOPBITS_1;
	debugParams.OverSampling = UART_OVERSAMPLING_8;
	debugParams.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	debugParams.Mode = (UART_MODE_TX | UART_MODE_RX);
	
	UART_Init(UART3_INSTANCE, &debugParams);
	
}

/*
+------------------------------------------------------------------------------
| Function : UART_Init(...)
+------------------------------------------------------------------------------
| Purpose: This function initializes the UART 2 for Modbus communication.
+------------------------------------------------------------------------------
| Algorithms: 
|           
|			
|	@note: The global instance for UART1/UART3 is used to send and recieve data over 
|			Modbus.
|		   The UART instance number as follows
|			- UART1_INSTANCE,  for Modbus Uart
|			- UART3_INSTANCE for Debugg port
|
+------------------------------------------------------------------------------
| Parameters:
|		UART_INSTANCE_NUM_e - Uart Instance number
|  		UART_InitTypeDef * - Uart Communication parameters
+------------------------------------------------------------------------------
| Return Value: 
|		0 = Success
|		1 = fail
|  
+------------------------------------------------------------------------------
*/
int UART_Init(UART_INSTANCE_NUM_e uartInstanceNo, UART_InitTypeDef *uartParams)
{
	UART_InitTypeDef *paramCopiedInto;
	UART_INSTANT_t *uartInstance;
	
	// Based on Instance number init Register Array and communication params
	if(uartInstanceNo == UART1_INSTANCE)
	{
		// Uart 2 parameter initialization for Modbus 
		gUart1Instant.pRegInstance = USART1;
		paramCopiedInto = &gUart1Instant.Init;
		uartInstance = &gUart1Instant;
		
		// Enable the UART module clock for RCC
		__HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_SYSCLK);
		__HAL_RCC_USART1_CLK_ENABLE();	
	}
	else if(uartInstanceNo == UART3_INSTANCE)
	{
		// Uart 3 parameters initialization for Debugg
		gUart3Instant.pRegInstance = USART3;
		paramCopiedInto = &gUart3Instant.Init;
		uartInstance = &gUart3Instant;
		
		// Enable the UART module clock for RCC
		__HAL_RCC_USART3_CLK_ENABLE();
	}
	
	// Copy UART communication parameters to its data structure which is used 
	// while configuring the UART Module
	UART_InitPrams(paramCopiedInto, uartParams);
		
	// Initialiaze rspective registers of UART module
	UART_InitCommunication(uartInstance);
	
	if(uartInstanceNo == UART1_INSTANCE)
	{
		// Enable 8xUSARTs Receive interrupts
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
	}
	else if(uartInstanceNo == UART3_INSTANCE)
	{
		// Enable 8xUSARTs Receive interrupts
		HAL_NVIC_EnableIRQ(USART3_4_IRQn);
		HAL_NVIC_SetPriority(USART3_4_IRQn, 3, 0);
	}
	
	// Start UART communication
	__HAL_UART_ENABLE(uartInstance->pRegInstance);
	
	return 1;
}

/*
+------------------------------------------------------------------------------
| Function : UART_InitPrams(...)
+------------------------------------------------------------------------------
| Purpose: This function copies the communication parameters to global data 
|			structure.
+------------------------------------------------------------------------------
| Algorithms: 
|           
|			
|
+------------------------------------------------------------------------------
| Parameters:  
|		UART_InitTypeDef * - copied into 
|  		UART_InitTypeDef * - Uart Communication parameters
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
static void UART_InitPrams(UART_InitTypeDef *uartParamsCopiedInto, 
						   UART_InitTypeDef *uartParams)
{
	uartParamsCopiedInto->BaudRate = uartParams->BaudRate;
	uartParamsCopiedInto->WordLength = uartParams->WordLength;
	uartParamsCopiedInto->StopBits = uartParams->StopBits;
	uartParamsCopiedInto->Parity = uartParams->Parity;
	uartParamsCopiedInto->Mode = uartParams->Mode;
	uartParamsCopiedInto->HwFlowCtl = uartParams->HwFlowCtl;
	uartParamsCopiedInto->OverSampling = uartParams->OverSampling;
	uartParamsCopiedInto->OneBitSampling = uartParams->OneBitSampling;
}

/*
+------------------------------------------------------------------------------
| Function : UART_InitCommunication(...)
+------------------------------------------------------------------------------
| Purpose: This function initialize the UART module with necessary parameters.
+------------------------------------------------------------------------------
| Algorithms: 
|       - Initialize CR1, CR2, CR3 registers
|		- Initialize the baudrate			
|
+------------------------------------------------------------------------------
| Parameters:  
|		UART_INSTANT_t * - Uart Instance
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
static void UART_InitCommunication(UART_INSTANT_t *uartInstance)
{
	uint32_t tmpreg = 0x00000000U;
	uint16_t usartdiv = 0x0000U;
	uint16_t brrtemp = 0x0000U;

	// Disable USART
	uartInstance->pRegInstance->CR1 &= (uint32_t)~((uint32_t)USART_CR1_UE);
	
	/*-------------------------- USART CR1 Configuration -----------------------*/
	/* Clear M, PCE, PS, TE, RE and OVER8 bits and configure
	*  the UART Word Length, Parity, Mode and oversampling:
	*  set the M bits according to uartInstance->Init.WordLength value
	*  set PCE and PS bits according to uartInstance->Init.Parity value
	*  set TE and RE bits according to uartInstance->Init.Mode value
	*  set OVER8 bit according to uartInstance->Init.OverSampling value */
	tmpreg = (uint32_t)(uartInstance->Init.WordLength | 
						uartInstance->Init.Parity | 
						uartInstance->Init.Mode | 
						uartInstance->Init.OverSampling);
	MODIFY_REG(uartInstance->pRegInstance->CR1, UART_CR1_FIELDS, tmpreg);

	/*-------------------------- USART CR2 Configuration -----------------------*/
	/* Configure the UART Stop Bits: Set STOP[13:12] bits according
	* to uartInstance->Init.StopBits value */
	MODIFY_REG(uartInstance->pRegInstance->CR2, USART_CR2_STOP, uartInstance->Init.StopBits);

	/*-------------------------- USART CR3 Configuration -----------------------*/
	/* Configure
	* - UART HardWare Flow Control: set CTSE and RTSE bits according
	*   to huart->Init.HwFlowCtl value
	* - one-bit sampling method versus three samples' majority rule according
	*   to huart->Init.OneBitSampling */
	tmpreg = (uint32_t)uartInstance->Init.HwFlowCtl | uartInstance->Init.OneBitSampling ;
	MODIFY_REG(uartInstance->pRegInstance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT), tmpreg);
	
	/*-------------------------- USART BRR Configuration -----------------------*/
	/* Check UART Over Sampling to set Baud Rate Register */
	if(uartInstance->Init.OverSampling == UART_OVERSAMPLING_8)
	{
		//TODO: We can replace HAL_RCC_GetSysClockFreq() func call with Macro for system clock
		usartdiv = (uint16_t)(UART_DIV_SAMPLING8(HAL_RCC_GetSysClockFreq(), uartInstance->Init.BaudRate));
	}
	else
	{
		//TODO: We can replace HAL_RCC_GetSysClockFreq() func call with Macro for system clock
		usartdiv = (uint16_t)(UART_DIV_SAMPLING16(HAL_RCC_GetSysClockFreq(), uartInstance->Init.BaudRate));
	}
	
	brrtemp = usartdiv & 0xFFF0U;
    brrtemp |= (uint16_t)((usartdiv & (uint16_t)0x000FU) >> 1U);
    uartInstance->pRegInstance->BRR = brrtemp;
	
	/*-------------------------- USART Interrupt Configuration -----------------------*/
	__HAL_UART_ENABLE_IT(uartInstance->pRegInstance, UART_IT_RXNE);
}

/*
+------------------------------------------------------------------------------
| Function : UART_StartTxInterrupt(...)
+------------------------------------------------------------------------------
| Purpose: This function enables transmitt interrupt bit in USART status register.
+------------------------------------------------------------------------------
| Algorithms: 
|       - Initialize CR1, CR2, CR3 registers
|		- Initialize the baudrate			
|
+------------------------------------------------------------------------------
| Parameters:  
|		UART_INSTANCE_NUM_e - Uart Instance number
|		uint8_t	- ENABLE = 1, DISABLE = 0
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void UART_StartTxInterrupt(UART_INSTANCE_NUM_e uartInstanceNo, uint8_t enableDisableStatus)
{
	if(enableDisableStatus == ENABLE)
	{
		if(uartInstanceNo == UART1_INSTANCE)
		{
			__HAL_UART_ENABLE_IT(gUart1Instant.pRegInstance, UART_IT_TXE);
		}
		else
		{
			__HAL_UART_ENABLE_IT(gUart3Instant.pRegInstance, UART_IT_TXE);
		}
	}
	else
	{
		if(uartInstanceNo == UART1_INSTANCE)
		{
			__HAL_UART_DISABLE_IT(gUart1Instant.pRegInstance, UART_IT_TXE);
		}
		else
		{
			__HAL_UART_DISABLE_IT(gUart3Instant.pRegInstance, UART_IT_TXE);
		}
	}
}

uint8_t UART_TransmitData(UART_INSTANCE_NUM_e uartInstanceNo, 
							uint8_t *pData, 
							uint16_t dataSize, 
							uint32_t dataXferTimeout)
{
	uint8_t retVal = 0;
	uint32_t tickstart = 0U;
	uint32_t txXferCount = 0;
	UART_INSTANT_t *localInstance;
	
	/* Get the local instance of UART */
	if(uartInstanceNo == UART1_INSTANCE)
	{
		localInstance = &gUart1Instant;
	}
	else
	{
		localInstance = &gUart3Instant;
	}

    /* Init tickstart for timeout managment*/
    tickstart = HAL_GetTick();
	txXferCount = dataSize;
	
	while(txXferCount > 0)
	{
		txXferCount--;
		/* transfer continous data */
		localInstance->pRegInstance->TDR = (uint16_t)(*pData++ & (uint8_t)0xFFU);
		
		/* check for Transmit flag, else check for timeout */
		while(__HAL_UART_GET_FLAG(localInstance->pRegInstance, UART_FLAG_TXE) == RESET)
		{
			/* Check for the Timeout */
			if((dataXferTimeout == 0U) || ((HAL_GetTick()-tickstart) > dataXferTimeout))
			{
				retVal = 1;
				break;
			}
		}
		
		/* If timout occured then no need to procced for further data transfer */
		if(retVal)
		{
			break;
		}
	}
	
	/* return status */
	return retVal;
}

/*
+------------------------------------------------------------------------------
| Function : USART1_IRQHandler(...)
+------------------------------------------------------------------------------
| Purpose: This is ISR handler for UART1
+------------------------------------------------------------------------------
| Algorithms: 
|       		
|
+------------------------------------------------------------------------------
| Parameters:  
|		None
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void USART1_IRQHandler(void)
{
	uint16_t  recData;
	
	if((__HAL_UART_GET_FLAG(gUart1Instant.pRegInstance, UART_FLAG_RXNE) && 
		__HAL_UART_GET_IT_SOURCE(gUart1Instant.pRegInstance, UART_IT_RXNE)))
	{
		recData = READ_REG(gUart1Instant.pRegInstance->RDR);
		UART1_RX_Handler(recData);
	}
	
	if((__HAL_UART_GET_FLAG(gUart1Instant.pRegInstance, UART_FLAG_TXE) &&
		__HAL_UART_GET_IT_SOURCE(gUart1Instant.pRegInstance, UART_IT_TXE)))
	{
		UART1_TX_Handler();
	}
}

/*
+------------------------------------------------------------------------------
| Function : UART1_RX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is weak receiving function of UART1.
+------------------------------------------------------------------------------
| Algorithms: 
|       This function must be implemented in User file to handle recieved data 
|		of UART1		
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint16_t - received data
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
__weak void UART1_RX_Handler(uint16_t receivedData)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
	the UART1_RX_Receving could be implemented in the user file
	*/ 
}

/*
+------------------------------------------------------------------------------
| Function : UART1_TX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is weak transmitting function of UART3.
+------------------------------------------------------------------------------
| Algorithms: 
|       This function must be implemented in User file to handle transmission
|		of UART1		
|
+------------------------------------------------------------------------------
| Parameters:  
|		None
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
__weak void UART1_TX_Handler(void)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
	the UART1_TX_Receving could be implemented in the user file
	*/ 
}


/*
+------------------------------------------------------------------------------
| Function : USART3_4_IRQHandler(...)
+------------------------------------------------------------------------------
| Purpose: This is ISR handler for UART3
+------------------------------------------------------------------------------
| Algorithms: 
|       		
|
+------------------------------------------------------------------------------
| Parameters:  
|		None
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void USART3_4_IRQHandler(void)
{
	uint16_t  recData;
	
	if((__HAL_UART_GET_FLAG(gUart3Instant.pRegInstance, UART_FLAG_RXNE) && 
		__HAL_UART_GET_IT_SOURCE(gUart3Instant.pRegInstance, UART_IT_RXNE)))
	{
		recData = READ_REG(gUart3Instant.pRegInstance->RDR);
		UART3_RX_Handler(recData);
	}
	
	if((__HAL_UART_GET_FLAG(gUart3Instant.pRegInstance, UART_FLAG_TXE) &&
		__HAL_UART_GET_IT_SOURCE(gUart3Instant.pRegInstance, UART_IT_TXE)))
	{
		UART3_TX_Handler();
	}
}

/*
+------------------------------------------------------------------------------
| Function : UART3_RX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is weak receiving function of UART3.
+------------------------------------------------------------------------------
| Algorithms: 
|       This function must be implemented in User file to handle recieved data 
|		of UART3		
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint16_t - received data
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
__weak void UART3_RX_Handler(uint16_t receivedData)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
	the UART3_RX_Receving could be implemented in the user file
	*/ 
}

/*
+------------------------------------------------------------------------------
| Function : UART3_TX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is weak transmitting function of UART3.
+------------------------------------------------------------------------------
| Algorithms: 
|       This function must be implemented in User file to handle transmission
|		of UART3		
|
+------------------------------------------------------------------------------
| Parameters:  
|		None
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
__weak void UART3_TX_Handler(void)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
	the UART3_TX_Transmitting could be implemented in the user file
	*/ 
}

#endif // HAL_UART_MODULE_ENABLED

