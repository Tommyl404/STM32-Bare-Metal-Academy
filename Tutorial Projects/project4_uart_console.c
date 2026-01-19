/**
 ******************************************************************************
 * @file           : project4_uart_console.c
 * @brief          : Project Tutorial 4 - UART Command Console
 ******************************************************************************
 * 
 *   ██████╗ ██████╗ ███╗   ██╗███████╗ ██████╗ ██╗     ███████╗
 *  ██╔════╝██╔═══██╗████╗  ██║██╔════╝██╔═══██╗██║     ██╔════╝
 *  ██║     ██║   ██║██╔██╗ ██║███████╗██║   ██║██║     █████╗  
 *  ██║     ██║   ██║██║╚██╗██║╚════██║██║   ██║██║     ██╔══╝  
 *  ╚██████╗╚██████╔╝██║ ╚████║███████║╚██████╔╝███████╗███████╗
 *   ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚══════╝╚══════╝
 *
 *  PROJECT TUTORIAL 4: UART COMMAND CONSOLE
 * 
 *  ════════════════════════════════════════════════════════════════════════
 *  THE PROJECT:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  A serial terminal interface that lets you control LEDs with commands!
 *  
 *  Connect via USB (ST-Link provides virtual COM port) at 115200 baud.
 *  
 *  COMMANDS:
 *  ┌────────────────┬───────────────────────────────────────────────────┐
 *  │ Command        │ Action                                            │
 *  ├────────────────┼───────────────────────────────────────────────────┤
 *  │ G or g         │ Toggle GREEN LED                                  │
 *  │ Y or y         │ Toggle YELLOW LED                                 │
 *  │ R or r         │ Toggle RED LED                                    │
 *  │ A or a         │ All LEDs ON                                       │
 *  │ O or o         │ All LEDs OFF                                      │
 *  │ S or s         │ Show STATUS (which LEDs are on)                   │
 *  │ H or h or ?    │ Show HELP menu                                    │
 *  │ P or p         │ Run PARTY mode (LED animation)                    │
 *  └────────────────┴───────────────────────────────────────────────────┘
 *  
 *  ADDITIONAL FEATURES:
 *  • Button press sends "BUTTON PRESSED!" over UART
 *  • Periodic heartbeat message every 5 seconds
 *  • Echo received characters back to terminal
 *  
 *  
 *  CONCEPTS COMBINED IN THIS PROJECT:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  ┌─────────────────┬──────────────────────────────────────────────────┐
 *  │ Concept         │ How it's used                                    │
 *  ├─────────────────┼──────────────────────────────────────────────────┤
 *  │ RCC             │ Enable clocks for all peripherals                │
 *  │ GPIO            │ LED outputs, button input, UART alternate func   │
 *  │ USART           │ Serial communication with PC                     │
 *  │ TIM             │ Delay functions, heartbeat timer                 │
 *  │ EXTI            │ Button interrupt                                 │
 *  │ NVIC            │ UART RX and button interrupts                    │
 *  │ Circular Buffer │ Software pattern for buffering received data     │
 *  │ Command Parser  │ String processing for commands                   │
 *  └─────────────────┴──────────────────────────────────────────────────┘
 *  
 *  
 *  HARDWARE CONNECTIONS:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  UART3 (connected to ST-Link Virtual COM Port on Nucleo):
 *  • PD8 = TX (transmit to PC)
 *  • PD9 = RX (receive from PC)
 *  
 *  LEDs:
 *  • PB0  = Green LED
 *  • PE1  = Yellow LED
 *  • PB14 = Red LED
 *  
 *  Button:
 *  • PC13 = User Button
 * 
 *  DIFFICULTY: ⭐⭐⭐⭐ (Intermediate-Advanced)
 * 
 ******************************************************************************
 */

#include <stdint.h>
#include <string.h>

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */

#define RCC_BASE        0x58024400UL
#define GPIOB_BASE      0x58020400UL
#define GPIOC_BASE      0x58020800UL
#define GPIOD_BASE      0x58020C00UL
#define GPIOE_BASE      0x58021000UL
#define USART3_BASE     0x40004800UL
#define TIM2_BASE       0x40000000UL
#define TIM7_BASE       0x40001400UL
#define EXTI_BASE       0x58000000UL
#define SYSCFG_BASE     0x58000400UL

#define NVIC_ISER_BASE  0xE000E100UL

/* ============================================================================
 *  REGISTER STRUCTURES
 * ============================================================================ */

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t HSICFGR;
    volatile uint32_t CRRCR;
    volatile uint32_t CSICFGR;
    volatile uint32_t CFGR;
    volatile uint32_t RESERVED1;
    volatile uint32_t D1CFGR;
    volatile uint32_t D2CFGR;
    volatile uint32_t D3CFGR;
    volatile uint32_t RESERVED2;
    volatile uint32_t PLLCKSELR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t PLL1DIVR;
    volatile uint32_t PLL1FRACR;
    volatile uint32_t PLL2DIVR;
    volatile uint32_t PLL2FRACR;
    volatile uint32_t PLL3DIVR;
    volatile uint32_t PLL3FRACR;
    volatile uint32_t RESERVED3;
    volatile uint32_t D1CCIPR;
    volatile uint32_t D2CCIP1R;
    volatile uint32_t D2CCIP2R;
    volatile uint32_t D3CCIPR;
    volatile uint32_t RESERVED4;
    volatile uint32_t CIER;
    volatile uint32_t CIFR;
    volatile uint32_t CICR;
    volatile uint32_t RESERVED5;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t RESERVED6;
    volatile uint32_t AHB3RSTR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB4RSTR;
    volatile uint32_t APB3RSTR;
    volatile uint32_t APB1LRSTR;
    volatile uint32_t APB1HRSTR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB4RSTR;
    volatile uint32_t GCR;
    volatile uint32_t RESERVED7;
    volatile uint32_t D3AMR;
    volatile uint32_t RESERVED8[9];
    volatile uint32_t RSR;
    volatile uint32_t AHB3ENR;
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB4ENR;
    volatile uint32_t APB3ENR;
    volatile uint32_t APB1LENR;
    volatile uint32_t APB1HENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB4ENR;
} RCC_TypeDef;

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
    volatile uint32_t PRESC;
} USART_TypeDef;

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
} TIM_TypeDef;

typedef struct {
    volatile uint32_t RTSR1;
    volatile uint32_t FTSR1;
    volatile uint32_t SWIER1;
    volatile uint32_t D3PMR1;
    volatile uint32_t D3PCR1L;
    volatile uint32_t D3PCR1H;
    volatile uint32_t RESERVED1[2];
    volatile uint32_t RTSR2;
    volatile uint32_t FTSR2;
    volatile uint32_t SWIER2;
    volatile uint32_t D3PMR2;
    volatile uint32_t D3PCR2L;
    volatile uint32_t D3PCR2H;
    volatile uint32_t RESERVED2[2];
    volatile uint32_t RTSR3;
    volatile uint32_t FTSR3;
    volatile uint32_t SWIER3;
    volatile uint32_t D3PMR3;
    volatile uint32_t D3PCR3L;
    volatile uint32_t D3PCR3H;
    volatile uint32_t RESERVED3[10];
    volatile uint32_t IMR1;
    volatile uint32_t EMR1;
    volatile uint32_t PR1;
    volatile uint32_t RESERVED4;
    volatile uint32_t IMR2;
    volatile uint32_t EMR2;
    volatile uint32_t PR2;
} EXTI_TypeDef;

typedef struct {
    volatile uint32_t RESERVED1;
    volatile uint32_t PMCR;
    volatile uint32_t EXTICR[4];
} SYSCFG_TypeDef;

/* Peripheral Pointers */
#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD   ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE   ((GPIO_TypeDef *) GPIOE_BASE)
#define USART3  ((USART_TypeDef *) USART3_BASE)
#define TIM2    ((TIM_TypeDef *) TIM2_BASE)
#define TIM7    ((TIM_TypeDef *) TIM7_BASE)
#define EXTI    ((EXTI_TypeDef *) EXTI_BASE)
#define SYSCFG  ((SYSCFG_TypeDef *) SYSCFG_BASE)

#define NVIC_ISER   ((volatile uint32_t *) NVIC_ISER_BASE)

/* ============================================================================
 *  BIT DEFINITIONS
 * ============================================================================ */

/* RCC */
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_AHB4ENR_GPIOCEN     (1U << 2)
#define RCC_AHB4ENR_GPIODEN     (1U << 3)
#define RCC_AHB4ENR_GPIOEEN     (1U << 4)
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)
#define RCC_APB1LENR_TIM2EN     (1U << 0)
#define RCC_APB1LENR_TIM7EN     (1U << 5)
#define RCC_APB1LENR_USART3EN   (1U << 18)

/* USART */
#define USART_CR1_UE            (1U << 0)   /* USART Enable */
#define USART_CR1_RE            (1U << 2)   /* Receiver Enable */
#define USART_CR1_TE            (1U << 3)   /* Transmitter Enable */
#define USART_CR1_RXNEIE        (1U << 5)   /* RX Not Empty Interrupt Enable */
#define USART_ISR_TXE           (1U << 7)   /* TX Empty */
#define USART_ISR_TC            (1U << 6)   /* Transmission Complete */
#define USART_ISR_RXNE          (1U << 5)   /* RX Not Empty */
#define USART_ISR_ORE           (1U << 3)   /* Overrun Error */
#define USART_ICR_ORECF         (1U << 3)   /* Clear Overrun */

/* TIM */
#define TIM_CR1_CEN             (1U << 0)
#define TIM_DIER_UIE            (1U << 0)
#define TIM_SR_UIF              (1U << 0)
#define TIM_EGR_UG              (1U << 0)

/* EXTI */
#define EXTI_LINE13             (1U << 13)

/* IRQ Numbers */
#define TIM7_IRQn               55
#define USART3_IRQn             39
#define EXTI15_10_IRQn          40

/* Pins */
#define LED_GREEN_PIN           0
#define LED_YELLOW_PIN          1
#define LED_RED_PIN             14
#define BUTTON_PIN              13

/* USART3 Alternate Function */
#define GPIO_AF7_USART3         7

/* ============================================================================
 * 
 *  📚 QUICK LESSON: UART vs USART
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  UART = Universal Asynchronous Receiver/Transmitter
 *  USART = Universal Synchronous/Asynchronous Receiver/Transmitter
 *  
 *  The difference is USART can also do synchronous (clocked) communication.
 *  We use it in async mode (like a regular UART).
 *  
 *  Key Parameters:
 *  ┌────────────────┬─────────────────────────────────────────────────────┐
 *  │ Baud Rate      │ Speed in bits/second (e.g., 115200)                 │
 *  │ Data Bits      │ Usually 8 bits                                      │
 *  │ Stop Bits      │ Usually 1 bit                                       │
 *  │ Parity         │ None, Even, or Odd (we use None)                    │
 *  │ Flow Control   │ None, RTS/CTS, XON/XOFF (we use None)               │
 *  └────────────────┴─────────────────────────────────────────────────────┘
 *  
 *  Common settings: 115200 8N1 (115200 baud, 8 data, No parity, 1 stop)
 * 
 * ============================================================================ */

/* ============================================================================
 *  CIRCULAR BUFFER FOR UART RX
 * ============================================================================ */

#define RX_BUFFER_SIZE  64

typedef struct {
    volatile char buffer[RX_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} CircularBuffer_t;

CircularBuffer_t rx_buffer = {.head = 0, .tail = 0};

/* ============================================================================
 * 
 *  📚 QUICK LESSON: CIRCULAR BUFFER
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  A circular buffer (ring buffer) lets us receive data in an interrupt
 *  and process it later in the main loop without losing bytes.
 *  
 *      ┌───┬───┬───┬───┬───┬───┬───┬───┐
 *      │ H │ E │ L │ L │ O │   │   │   │
 *      └───┴───┴───┴───┴───┴───┴───┴───┘
 *        ↑                   ↑
 *       tail               head
 *       (read)             (write)
 *  
 *  • ISR writes at head, increments head
 *  • Main loop reads at tail, increments tail
 *  • When head == tail, buffer is empty
 *  • Both wrap around when they reach the end
 * 
 * ============================================================================ */

uint8_t Buffer_IsEmpty(CircularBuffer_t *buf) {
    return buf->head == buf->tail;
}

uint8_t Buffer_IsFull(CircularBuffer_t *buf) {
    return ((buf->head + 1) % RX_BUFFER_SIZE) == buf->tail;
}

void Buffer_Put(CircularBuffer_t *buf, char c) {
    if (!Buffer_IsFull(buf)) {
        buf->buffer[buf->head] = c;
        buf->head = (buf->head + 1) % RX_BUFFER_SIZE;
    }
}

char Buffer_Get(CircularBuffer_t *buf) {
    if (Buffer_IsEmpty(buf)) {
        return 0;
    }
    char c = buf->buffer[buf->tail];
    buf->tail = (buf->tail + 1) % RX_BUFFER_SIZE;
    return c;
}

/* ============================================================================
 *  GLOBAL STATE
 * ============================================================================ */

volatile uint8_t button_pressed = 0;
volatile uint8_t heartbeat_tick = 0;
volatile uint32_t uptime_seconds = 0;

/* LED state tracking */
uint8_t led_green_on = 0;
uint8_t led_yellow_on = 0;
uint8_t led_red_on = 0;

/* ============================================================================
 * 
 *  STEP 1: ENABLE CLOCKS
 *  =======================
 * 
 * ============================================================================ */

void EnableClocks(void) {
    /* Enable GPIO clocks */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;    /* For UART pins */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    
    /* Enable SYSCFG for EXTI */
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    
    /* Enable TIM2 for delays */
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
    
    /* Enable TIM7 for heartbeat */
    RCC->APB1LENR |= RCC_APB1LENR_TIM7EN;
    
    /* ✏️ YOUR TURN: Enable USART3 clock */
    RCC->APB1LENR |= ???;       /* HINT: RCC_APB1LENR_USART3EN */
    
    (void)RCC->APB4ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * RCC->APB1LENR |= RCC_APB1LENR_USART3EN;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 2: CONFIGURE GPIO
 *  ========================
 * 
 * ============================================================================ */

void ConfigureGPIO(void) {
    /* ✏️ YOUR TURN: Configure Green LED (PB0) as output */
    GPIOB->MODER &= ~(??? << (LED_GREEN_PIN * 2));    /* HINT: Clear 2 bits = 3U */
    GPIOB->MODER |= (??? << (LED_GREEN_PIN * 2));     /* HINT: Output mode = 1U */
    
    /* Configure Yellow LED (PE1) as output */
    GPIOE->MODER &= ~(3U << (LED_YELLOW_PIN * 2));
    GPIOE->MODER |= (1U << (LED_YELLOW_PIN * 2));
    
    /* ✏️ YOUR TURN: Configure Red LED (PB14) as output */
    GPIOB->MODER &= ~(3U << (??? * 2));              /* HINT: LED_RED_PIN */
    GPIOB->MODER |= (1U << (??? * 2));               /* HINT: LED_RED_PIN */
    
    /* Button as input (MODER = 00, which is default after reset) */
    GPIOC->MODER &= ~(3U << (BUTTON_PIN * 2));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIOB->MODER &= ~(3U << (LED_GREEN_PIN * 2));
 * GPIOB->MODER |= (1U << (LED_GREEN_PIN * 2));
 * 
 * GPIOB->MODER &= ~(3U << (LED_RED_PIN * 2));
 * GPIOB->MODER |= (1U << (LED_RED_PIN * 2));
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 3: CONFIGURE UART GPIO PINS
 *  ==================================
 * 
 *  📚 REMINDER: ALTERNATE FUNCTIONS
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  USART3 uses PD8 (TX) and PD9 (RX).
 *  These pins must be configured for:
 *  1. Alternate Function mode (MODER = 10)
 *  2. The correct AF number (AF7 for USART3)
 *  
 *  AFR[0] controls pins 0-7, AFR[1] controls pins 8-15
 *  Each pin gets 4 bits to select AF0-AF15
 * 
 * ============================================================================ */

void ConfigureUARTGPIO(void) {
    /* PD8 = USART3_TX */
    /* ✏️ YOUR TURN: Set to Alternate Function mode (MODER = 10 binary = 2) */
    GPIOD->MODER &= ~(3U << (8 * 2));
    GPIOD->MODER |= (??? << (8 * 2));    /* HINT: 2U for AF mode */
    
    /* ✏️ YOUR TURN: Set AF7 for PD8 (in AFR[1], pin 8 = bits 0-3) */
    GPIOD->AFR[1] &= ~(0xFU << 0);
    GPIOD->AFR[1] |= (??? << 0);         /* HINT: GPIO_AF7_USART3 = 7 */
    
    /* PD9 = USART3_RX */
    /* ✏️ YOUR TURN: Set to Alternate Function mode */
    GPIOD->MODER &= ~(3U << (9 * 2));
    GPIOD->MODER |= (??? << (9 * 2));    /* HINT: Same as TX, 2U */
    
    /* ✏️ YOUR TURN: Set AF7 for PD9 (in AFR[1], pin 9 = bits 4-7) */
    GPIOD->AFR[1] &= ~(0xFU << ???);     /* HINT: Pin 9 starts at bit 4 */
    GPIOD->AFR[1] |= (GPIO_AF7_USART3 << ???);  /* HINT: Same bit position */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIOD->MODER |= (2U << (8 * 2));      // PD8 AF mode
 * GPIOD->AFR[1] |= (7U << 0);           // PD8 = AF7
 * 
 * GPIOD->MODER |= (2U << (9 * 2));      // PD9 AF mode  
 * GPIOD->AFR[1] &= ~(0xFU << 4);        // Clear bits 4-7
 * GPIOD->AFR[1] |= (7U << 4);           // PD9 = AF7
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 4: CONFIGURE USART3
 *  ==========================
 * 
 *  📚 REMINDER: BAUD RATE CALCULATION
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  BRR = USART_Clock / Baud_Rate
 *  
 *  For 115200 baud with 64 MHz clock:
 *  BRR = 64,000,000 / 115200 ≈ 556
 *  
 *  The fractional part is handled automatically in oversampling by 16 mode.
 * 
 * ============================================================================ */

void ConfigureUSART3(void) {
    /* Make sure USART is disabled during configuration */
    USART3->CR1 &= ~USART_CR1_UE;
    
    /* ✏️ YOUR TURN: Set baud rate for 115200 (clock is 64 MHz) */
    USART3->BRR = ???;          /* HINT: 64000000 / 115200 = 556 */
    
    /* Configure: 8 data bits, 1 stop bit, no parity (default) */
    USART3->CR1 = 0;            /* Reset CR1 */
    USART3->CR2 = 0;            /* 1 stop bit (default) */
    USART3->CR3 = 0;            /* No flow control */
    
    /* ✏️ YOUR TURN: Enable Transmitter and Receiver */
    USART3->CR1 |= ??? | ???;   /* HINT: USART_CR1_TE | USART_CR1_RE */
    
    /* ✏️ YOUR TURN: Enable RX interrupt (fires when byte received) */
    USART3->CR1 |= ???;         /* HINT: USART_CR1_RXNEIE */
    
    /* ✏️ YOUR TURN: Enable USART */
    USART3->CR1 |= ???;         /* HINT: USART_CR1_UE */
    
    /* Enable interrupt in NVIC */
    NVIC_ISER[1] = (1U << (USART3_IRQn - 32));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * USART3->BRR = 556;                           // 64000000 / 115200
 * USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;  // Enable TX and RX
 * USART3->CR1 |= USART_CR1_RXNEIE;             // Enable RX interrupt
 * USART3->CR1 |= USART_CR1_UE;                 // Enable USART
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 5: UART TX/RX FUNCTIONS
 *  ==============================
 * 
 * ============================================================================ */

void UART_SendChar(char c) {
    /* ✏️ YOUR TURN: Wait until TX register is empty */
    while (!(USART3->??? & ???));   /* HINT: ISR register, USART_ISR_TXE flag */
    
    /* ✏️ YOUR TURN: Write character to transmit data register */
    USART3->??? = c;                /* HINT: TDR (Transmit Data Register) */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * while (!(USART3->ISR & USART_ISR_TXE));  // Wait for TX empty
 * USART3->TDR = c;                          // Write to transmit register
 * ───────────────────────────────────────────────────────────────────────────── */

void UART_SendString(const char *str) {
    while (*str) {
        UART_SendChar(*str++);
    }
}

void UART_SendLine(const char *str) {
    UART_SendString(str);
    UART_SendString("\r\n");
}

/* Send a number as text */
void UART_SendNumber(uint32_t num) {
    char buf[12];
    int i = 0;
    
    if (num == 0) {
        UART_SendChar('0');
        return;
    }
    
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        UART_SendChar(buf[--i]);
    }
}

/* ============================================================================
 * 
 *  STEP 6: CONFIGURE DELAY TIMER (TIM2)
 *  ======================================
 * 
 * ============================================================================ */

void ConfigureDelayTimer(void) {
    /* ✏️ YOUR TURN: Set prescaler for 1 MHz (1 µs ticks) from 64 MHz */
    TIM2->PSC = ???;            /* HINT: 64 - 1 = 63 */
    
    /* Set max count (32-bit timer) */
    TIM2->ARR = 0xFFFFFFFF;
    
    /* ✏️ YOUR TURN: Generate update event to load prescaler */
    TIM2->EGR = ???;            /* HINT: TIM_EGR_UG */
    
    /* ✏️ YOUR TURN: Start the timer */
    TIM2->CR1 |= ???;           /* HINT: TIM_CR1_CEN */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * TIM2->PSC = 63;          // 64 MHz / 64 = 1 MHz
 * TIM2->EGR = TIM_EGR_UG;  // Generate update event
 * TIM2->CR1 |= TIM_CR1_CEN; // Counter enable
 * ───────────────────────────────────────────────────────────────────────────── */

void delay_ms(uint32_t ms) {
    uint32_t start = TIM2->CNT;
    while ((TIM2->CNT - start) < (ms * 1000));
}

/* ============================================================================
 * 
 *  STEP 7: CONFIGURE HEARTBEAT TIMER (TIM7)
 *  ==========================================
 * 
 *  Generates an interrupt every 5 seconds for status messages.
 * 
 * ============================================================================ */

void ConfigureHeartbeatTimer(void) {
    /* ✏️ YOUR TURN: Set prescaler for 1 kHz from 64 MHz */
    /* Formula: 64 MHz / (PSC+1) = 1 kHz → PSC = 63999 */
    TIM7->PSC = ???;            /* HINT: 64000 - 1 = 63999 */
    
    /* ✏️ YOUR TURN: Set ARR for 5 second period (at 1 kHz) */
    /* Formula: 1 kHz / (ARR+1) = 0.2 Hz → ARR = 4999 */
    TIM7->ARR = ???;            /* HINT: 5000 - 1 = 4999 */
    
    /* ✏️ YOUR TURN: Enable update interrupt */
    TIM7->DIER |= ???;          /* HINT: TIM_DIER_UIE */
    
    /* Generate update event to load values */
    TIM7->EGR = TIM_EGR_UG;
    TIM7->SR &= ~TIM_SR_UIF;    /* Clear pending flag */
    
    /* Enable in NVIC */
    NVIC_ISER[1] = (1U << (TIM7_IRQn - 32));
    
    /* Start timer */
    TIM7->CR1 |= TIM_CR1_CEN;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * TIM7->PSC = 63999;        // 64 MHz / 64000 = 1 kHz
 * TIM7->ARR = 4999;         // 1 kHz / 5000 = 5 second period
 * TIM7->DIER |= TIM_DIER_UIE;  // Update interrupt enable
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 8: CONFIGURE BUTTON INTERRUPT
 *  ====================================
 * 
 * ============================================================================ */

void ConfigureButtonEXTI(void) {
    /* ✏️ YOUR TURN: Select Port C for EXTI13 (PC13) */
    /* EXTICR[3] handles lines 12-15, line 13 is at bits 4-7 */
    SYSCFG->EXTICR[3] &= ~(0xFU << 4);   /* Clear */
    SYSCFG->EXTICR[3] |= (??? << 4);     /* HINT: 0x02 = Port C */
    
    /* ✏️ YOUR TURN: Configure falling edge trigger (button press) */
    EXTI->??? |= EXTI_LINE13;            /* HINT: FTSR1 for Falling edge */
    
    /* ✏️ YOUR TURN: Unmask (enable) the interrupt line */
    EXTI->??? |= EXTI_LINE13;            /* HINT: IMR1 for Interrupt Mask */
    
    /* Enable in NVIC */
    NVIC_ISER[1] = (1U << (EXTI15_10_IRQn - 32));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * SYSCFG->EXTICR[3] |= (0x02U << 4);  // Port C for line 13
 * EXTI->FTSR1 |= EXTI_LINE13;          // Falling edge trigger
 * EXTI->IMR1 |= EXTI_LINE13;           // Unmask interrupt
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 *  LED CONTROL FUNCTIONS
 * ============================================================================ */

void LED_AllOff(void) {
    GPIOB->BSRR = (1U << (LED_GREEN_PIN + 16));
    GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16));
    GPIOB->BSRR = (1U << (LED_RED_PIN + 16));
    led_green_on = led_yellow_on = led_red_on = 0;
}

void LED_AllOn(void) {
    GPIOB->BSRR = (1U << LED_GREEN_PIN);
    GPIOE->BSRR = (1U << LED_YELLOW_PIN);
    GPIOB->BSRR = (1U << LED_RED_PIN);
    led_green_on = led_yellow_on = led_red_on = 1;
}

void LED_ToggleGreen(void) {
    if (led_green_on) {
        GPIOB->BSRR = (1U << (LED_GREEN_PIN + 16));
        led_green_on = 0;
    } else {
        GPIOB->BSRR = (1U << LED_GREEN_PIN);
        led_green_on = 1;
    }
}

void LED_ToggleYellow(void) {
    if (led_yellow_on) {
        GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16));
        led_yellow_on = 0;
    } else {
        GPIOE->BSRR = (1U << LED_YELLOW_PIN);
        led_yellow_on = 1;
    }
}

void LED_ToggleRed(void) {
    if (led_red_on) {
        GPIOB->BSRR = (1U << (LED_RED_PIN + 16));
        led_red_on = 0;
    } else {
        GPIOB->BSRR = (1U << LED_RED_PIN);
        led_red_on = 1;
    }
}

/* ============================================================================
 *  INTERRUPT HANDLERS
 * ============================================================================ */

void USART3_IRQHandler(void) {
    /* ✏️ YOUR TURN: Check if receive buffer not empty (data available) */
    if (USART3->ISR & ???) {             /* HINT: USART_ISR_RXNE */
        /* ✏️ YOUR TURN: Read received character from data register */
        char c = USART3->???;            /* HINT: RDR (Receive Data Register) */
        
        /* Store in buffer for processing in main loop */
        Buffer_Put(&rx_buffer, c);
        
        /* Echo back to terminal */
        UART_SendChar(c);
    }
    
    /* Clear overrun error if it occurred */
    if (USART3->ISR & USART_ISR_ORE) {
        USART3->ICR = USART_ICR_ORECF;
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * if (USART3->ISR & USART_ISR_RXNE) {   // Check RXNE flag
 *     char c = USART3->RDR;              // Read from receive register
 * ───────────────────────────────────────────────────────────────────────────── */

void EXTI15_10_IRQHandler(void) {
    /* ✏️ YOUR TURN: Check if line 13 triggered the interrupt */
    if (EXTI->??? & EXTI_LINE13) {       /* HINT: PR1 = Pending Register */
        /* ✏️ YOUR TURN: Clear the pending flag (write 1 to clear) */
        EXTI->??? = EXTI_LINE13;         /* HINT: PR1 */
        button_pressed = 1;
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * if (EXTI->PR1 & EXTI_LINE13) {  // Check pending
 *     EXTI->PR1 = EXTI_LINE13;     // Clear by writing 1
 * ───────────────────────────────────────────────────────────────────────────── */

void TIM7_IRQHandler(void) {
    /* ✏️ YOUR TURN: Check if update interrupt flag is set */
    if (TIM7->??? & ???) {               /* HINT: SR register, TIM_SR_UIF flag */
        /* ✏️ YOUR TURN: Clear the flag */
        TIM7->??? &= ~???;               /* HINT: SR, TIM_SR_UIF */
        heartbeat_tick = 1;
        uptime_seconds += 5;
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * if (TIM7->SR & TIM_SR_UIF) {   // Check update interrupt flag
 *     TIM7->SR &= ~TIM_SR_UIF;    // Clear the flag
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 *  COMMAND PROCESSING
 * ============================================================================ */

void ShowHelp(void) {
    UART_SendLine("");
    UART_SendLine("╔═══════════════════════════════════════╗");
    UART_SendLine("║     LED COMMAND CONSOLE - HELP        ║");
    UART_SendLine("╠═══════════════════════════════════════╣");
    UART_SendLine("║  G - Toggle GREEN LED                 ║");
    UART_SendLine("║  Y - Toggle YELLOW LED                ║");
    UART_SendLine("║  R - Toggle RED LED                   ║");
    UART_SendLine("║  A - All LEDs ON                      ║");
    UART_SendLine("║  O - All LEDs OFF                     ║");
    UART_SendLine("║  S - Show status                      ║");
    UART_SendLine("║  P - Party mode!                      ║");
    UART_SendLine("║  H - Show this help                   ║");
    UART_SendLine("╚═══════════════════════════════════════╝");
    UART_SendLine("");
}

void ShowStatus(void) {
    UART_SendLine("");
    UART_SendString("LED Status: GREEN=");
    UART_SendString(led_green_on ? "ON" : "OFF");
    UART_SendString(", YELLOW=");
    UART_SendString(led_yellow_on ? "ON" : "OFF");
    UART_SendString(", RED=");
    UART_SendLine(led_red_on ? "ON" : "OFF");
    
    UART_SendString("Uptime: ");
    UART_SendNumber(uptime_seconds);
    UART_SendLine(" seconds");
}

void PartyMode(void) {
    UART_SendLine("");
    UART_SendLine("*** PARTY MODE! ***");
    
    for (int cycle = 0; cycle < 3; cycle++) {
        /* Chase pattern */
        LED_AllOff();
        GPIOB->BSRR = (1U << LED_GREEN_PIN);
        delay_ms(100);
        
        LED_AllOff();
        GPIOE->BSRR = (1U << LED_YELLOW_PIN);
        delay_ms(100);
        
        LED_AllOff();
        GPIOB->BSRR = (1U << LED_RED_PIN);
        delay_ms(100);
        
        /* Flash all */
        LED_AllOn();
        delay_ms(100);
        LED_AllOff();
        delay_ms(100);
    }
    
    LED_AllOff();
    UART_SendLine("Party's over!");
}

void ProcessCommand(char cmd) {
    switch (cmd) {
        case 'G':
        case 'g':
            LED_ToggleGreen();
            UART_SendString("\r\nGreen LED ");
            UART_SendLine(led_green_on ? "ON" : "OFF");
            break;
            
        case 'Y':
        case 'y':
            LED_ToggleYellow();
            UART_SendString("\r\nYellow LED ");
            UART_SendLine(led_yellow_on ? "ON" : "OFF");
            break;
            
        case 'R':
        case 'r':
            LED_ToggleRed();
            UART_SendString("\r\nRed LED ");
            UART_SendLine(led_red_on ? "ON" : "OFF");
            break;
            
        case 'A':
        case 'a':
            LED_AllOn();
            UART_SendLine("\r\nAll LEDs ON");
            break;
            
        case 'O':
        case 'o':
            LED_AllOff();
            UART_SendLine("\r\nAll LEDs OFF");
            break;
            
        case 'S':
        case 's':
            ShowStatus();
            break;
            
        case 'H':
        case 'h':
        case '?':
            ShowHelp();
            break;
            
        case 'P':
        case 'p':
            PartyMode();
            break;
            
        case '\r':
        case '\n':
            /* Ignore enter key */
            break;
            
        default:
            UART_SendString("\r\nUnknown command: ");
            UART_SendChar(cmd);
            UART_SendLine(" (Press H for help)");
            break;
    }
}

/* ============================================================================
 * 
 *  ██████╗ ██████╗  █████╗  ██████╗████████╗██╗ ██████╗███████╗
 *  ██╔══██╗██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██║██╔════╝██╔════╝
 *  ██████╔╝██████╔╝███████║██║        ██║   ██║██║     █████╗  
 *  ██╔═══╝ ██╔══██╗██╔══██║██║        ██║   ██║██║     ██╔══╝  
 *  ██║     ██║  ██║██║  ██║╚██████╗   ██║   ██║╚██████╗███████╗
 *  ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝╚══════╝
 *  
 *  MAIN PROGRAM
 * 
 * ============================================================================ */

int main(void) {
    /* Initialize all peripherals */
    EnableClocks();
    ConfigureGPIO();
    ConfigureUARTGPIO();
    ConfigureUSART3();
    ConfigureDelayTimer();
    ConfigureHeartbeatTimer();
    ConfigureButtonEXTI();
    
    LED_AllOff();
    
    /* Send welcome message */
    UART_SendLine("");
    UART_SendLine("╔═══════════════════════════════════════╗");
    UART_SendLine("║    STM32H7 LED COMMAND CONSOLE        ║");
    UART_SendLine("║    Press H for help                   ║");
    UART_SendLine("╚═══════════════════════════════════════╝");
    UART_SendLine("");
    UART_SendString("> ");
    
    for (;;) {
        /* ═══════════════════════════════════════════════════════════════════
         * PROCESS RECEIVED COMMANDS
         * ═══════════════════════════════════════════════════════════════════ */
        while (!Buffer_IsEmpty(&rx_buffer)) {
            char c = Buffer_Get(&rx_buffer);
            ProcessCommand(c);
            UART_SendString("> ");
        }
        
        /* ═══════════════════════════════════════════════════════════════════
         * HANDLE BUTTON PRESS
         * ═══════════════════════════════════════════════════════════════════ */
        if (button_pressed) {
            button_pressed = 0;
            delay_ms(50);   /* Debounce */
            
            UART_SendLine("\r\n*** BUTTON PRESSED! ***");
            LED_ToggleGreen();
            UART_SendString("> ");
        }
        
        /* ═══════════════════════════════════════════════════════════════════
         * HEARTBEAT MESSAGE (every 5 seconds)
         * ═══════════════════════════════════════════════════════════════════ */
        if (heartbeat_tick) {
            heartbeat_tick = 0;
            
            UART_SendString("\r\n[Heartbeat] Uptime: ");
            UART_SendNumber(uptime_seconds);
            UART_SendLine(" seconds");
            UART_SendString("> ");
        }
    }
}

/* ============================================================================
 * 
 *  📡 HOW TO USE:
 *  
 *  1. Flash this code to your Nucleo board
 *  2. Connect to the ST-Link Virtual COM port:
 *     • Windows: Open Device Manager, find COM port, use PuTTY/Tera Term
 *     • Linux: Use screen /dev/ttyACM0 115200
 *     • macOS: Use screen /dev/tty.usbmodem* 115200
 *  3. Settings: 115200 baud, 8N1, no flow control
 *  4. Type commands to control LEDs!
 *  
 *  TERMINAL TIPS:
 *  • Characters echo as you type
 *  • Heartbeat message appears every 5 seconds
 *  • Press button on board to see message
 *  
 *  
 *  🎓 WHAT YOU LEARNED:
 *  
 *  ✅ USART: Configuration (baud rate, TX/RX enable, interrupts)
 *  ✅ GPIO Alternate Functions: Setting pins for peripheral use
 *  ✅ Circular Buffer: Handling async data in interrupts
 *  ✅ Command Parser: Processing text commands
 *  ✅ Multiple NVIC Sources: Timer, UART, and EXTI interrupts together
 *  ✅ TIM: Using one timer for delays, another for periodic events
 *  ✅ String Handling: Sending strings over UART
 *  
 *  
 *  📚 UART KEY CONCEPTS:
 *  
 *  1. TXE flag = Ready to accept next byte to transmit
 *  2. RXNE flag = Byte received and waiting to be read
 *  3. Reading RDR clears RXNE automatically
 *  4. ORE (Overrun Error) occurs if you don't read fast enough
 *  5. BRR = Clock / Baud_Rate (simple integer math)
 *  
 *  
 *  🔧 EXPERIMENT IDEAS:
 *  
 *  • Add PWM brightness control command (e.g., "G50" = Green at 50%)
 *  • Implement command history with arrow keys
 *  • Add ADC reading command to show voltage
 *  • Create macros (e.g., "BLINK 5" = blink LED 5 times)
 *  • Log events with RTC timestamps
 * 
 * ============================================================================ */
