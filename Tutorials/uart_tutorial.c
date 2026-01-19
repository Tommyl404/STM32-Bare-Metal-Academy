/**
 ******************************************************************************
 * @file           : uart_tutorial.c
 * @brief          : Learning UART without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ██╗   ██╗ █████╗ ██████╗ ████████╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██║   ██║██╔══██╗██╔══██╗╚══██╔══╝    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ██║   ██║███████║██████╔╝   ██║          ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██║   ██║██╔══██║██╔══██╗   ██║          ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ╚██████╔╝██║  ██║██║  ██║   ██║          ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *   ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝          ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: UART (Universal Asynchronous Receiver/Transmitter)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. How UART communication works
 *  2. How to configure GPIO pins for UART (Alternate Function)
 *  3. How to set baud rate
 *  4. How to transmit and receive data
 * 
 *  PREREQUISITES:
 *  - Complete the RCC tutorial first!
 *  - You need to understand enabling peripheral clocks
 * 
 *  HARDWARE:
 *  - Nucleo-H753ZI uses USART3 for ST-Link Virtual COM Port
 *  - PD8 = USART3_TX, PD9 = USART3_RX
 * 
 *  DIFFICULTY: ⭐⭐⭐ (Intermediate)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS UART?
 *  ========================
 * 
 *  UART is a serial communication protocol. Data is sent one bit at a time.
 *  
 *  Key concepts:
 *  ┌──────────────┬─────────────────────────────────────────────┐
 *  │ Baud Rate    │ Speed of communication (bits per second)    │
 *  │ TX           │ Transmit pin (data goes OUT)                │
 *  │ RX           │ Receive pin (data comes IN)                 │
 *  │ Start bit    │ Signals beginning of data frame             │
 *  │ Stop bit     │ Signals end of data frame                   │
 *  └──────────────┴─────────────────────────────────────────────┘
 *  
 *  Data frame (8N1 = 8 data bits, No parity, 1 stop bit):
 *  
 *  ┌───────┬───┬───┬───┬───┬───┬───┬───┬───┬────────┐
 *  │ START │ 0 │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │  STOP  │
 *  └───────┴───┴───┴───┴───┴───┴───┴───┴───┴────────┘
 *     1 bit          8 data bits            1 bit
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOD_BASE      0x58020C00UL
#define USART3_BASE     0x40004800UL

/* ============================================================================
 *  RCC REGISTERS (from RCC tutorial)
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

#define RCC     ((RCC_TypeDef *) RCC_BASE)

/* ============================================================================
 *  GPIO REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];   /* AFR[0] = pins 0-7, AFR[1] = pins 8-15 */
} GPIO_TypeDef;

#define GPIOD   ((GPIO_TypeDef *) GPIOD_BASE)

/* ============================================================================
 * 
 *  LESSON 1: USART REGISTER STRUCTURE
 *  ===================================
 * 
 *  The USART peripheral has several registers:
 *  
 *  ┌──────────────┬──────────────────────────────────────────────┐
 *  │ Register     │ Purpose                                      │
 *  ├──────────────┼──────────────────────────────────────────────┤
 *  │ CR1          │ Control register 1 - Enable USART, TX, RX    │
 *  │ CR2          │ Control register 2 - Stop bits, etc.         │
 *  │ CR3          │ Control register 3 - DMA, flow control       │
 *  │ BRR          │ Baud Rate Register - Sets communication speed│
 *  │ ISR          │ Interrupt/Status - Check TX empty, RX ready  │
 *  │ TDR          │ Transmit Data Register - Write data here     │
 *  │ RDR          │ Receive Data Register - Read data here       │
 *  └──────────────┴──────────────────────────────────────────────┘
 * 
 * ============================================================================ */

typedef struct {
    volatile uint32_t CR1;      /* 0x00 - Control register 1 */
    volatile uint32_t CR2;      /* 0x04 - Control register 2 */
    volatile uint32_t CR3;      /* 0x08 - Control register 3 */
    volatile uint32_t BRR;      /* 0x0C - Baud rate register */
    volatile uint32_t GTPR;     /* 0x10 - Guard time and prescaler */
    volatile uint32_t RTOR;     /* 0x14 - Receiver timeout register */
    volatile uint32_t RQR;      /* 0x18 - Request register */
    volatile uint32_t ISR;      /* 0x1C - Interrupt and status register */
    volatile uint32_t ICR;      /* 0x20 - Interrupt flag clear register */
    volatile uint32_t RDR;      /* 0x24 - Receive data register */
    volatile uint32_t TDR;      /* 0x28 - Transmit data register */
    volatile uint32_t PRESC;    /* 0x2C - Prescaler register */
} USART_TypeDef;

#define USART3  ((USART_TypeDef *) USART3_BASE)

/* ============================================================================
 * 
 *  LESSON 2: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 *  These bits control the USART behavior.
 * 
 * ============================================================================ */

/* RCC Clock Enable Bits */
#define RCC_AHB4ENR_GPIODEN     (1U << 3)   /* GPIOD clock enable */
#define RCC_APB1LENR_USART3EN   (1U << 18)  /* USART3 clock enable */

/* USART_CR1 Register Bits */
#define USART_CR1_UE            (1U << 0)   /* USART Enable */
#define USART_CR1_RE            (1U << 2)   /* Receiver Enable */
#define USART_CR1_TE            (1U << 3)   /* Transmitter Enable */

/* USART_ISR Register Bits (Status flags) */
#define USART_ISR_TXE_TXFNF     (1U << 7)   /* TX data register empty */
#define USART_ISR_TC            (1U << 6)   /* Transmission complete */
#define USART_ISR_RXNE_RXFNE    (1U << 5)   /* RX data register not empty */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE CLOCKS FOR GPIOD AND USART3
 *  ===================================================
 * 
 *  Before using any peripheral, you must enable its clock!
 *  
 *  USART3 is on APB1 bus → Use APB1LENR register
 *  GPIOD is on AHB4 bus → Use AHB4ENR register
 * 
 * ============================================================================ */

void UART_EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOD clock */
    RCC->AHB4ENR |= ???;        /* HINT: Which bit enables GPIO port D? Look at the AHB4 defines. */
    
    /* ✏️ YOUR TURN: Enable USART3 clock */
    RCC->APB1LENR |= ???;       /* HINT: USART3 is on APB1. Find the enable bit constant. */
    
    /* Small delay for clocks to stabilize */
    (void)RCC->APB1LENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void UART_EnableClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;
 *     RCC->APB1LENR |= RCC_APB1LENR_USART3EN;
 *     (void)RCC->APB1LENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 3: GPIO ALTERNATE FUNCTION
 *  ==================================
 * 
 *  For UART, GPIO pins must be configured in "Alternate Function" mode.
 *  This connects the pin to the USART peripheral instead of GPIO.
 *  
 *  MODER = 10 (binary) = Alternate Function mode
 *  
 *  Each pin can have multiple alternate functions (AF0-AF15).
 *  You select which one using the AFR register.
 *  
 *  For STM32H753:
 *    - PD8 = USART3_TX → AF7
 *    - PD9 = USART3_RX → AF7
 *  
 *  AFR register uses 4 bits per pin:
 *    AFR[0] = pins 0-7   (4 bits × 8 pins = 32 bits)
 *    AFR[1] = pins 8-15  (4 bits × 8 pins = 32 bits)
 * 
 * ============================================================================ */

#define GPIO_AF7_USART3     7U  /* Alternate function 7 = USART3 */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE GPIO PINS FOR UART
 *  ==============================================
 * 
 *  Configure PD8 (TX) and PD9 (RX) for USART3:
 *  1. Set MODER to Alternate Function (10)
 *  2. Set AFR to AF7
 * 
 *  Remember:
 *  - MODER uses 2 bits per pin → (pin * 2)
 *  - AFR uses 4 bits per pin → (pin * 4)
 *  - Pins 8-15 use AFR[1], so pin 8 is at position 0 in AFR[1]
 * 
 * ============================================================================ */

void UART_ConfigureGPIO(void) {
    /* ────────────────────────────────────────────────────────────────────────
     * Step 1: Configure PD8 (TX) as Alternate Function
     * ──────────────────────────────────────────────────────────────────────── */
    
    /* Clear mode bits for pin 8 */
    GPIOD->MODER &= ~(3U << (8 * 2));
    
    /* ✏️ YOUR TURN: Set mode to Alternate Function (binary 10 = 2) */
    GPIOD->MODER |= ???;        /* HINT: AF mode = 10 binary = 2. Shift to pin 8's position. */
    
    /* ────────────────────────────────────────────────────────────────────────
     * Step 2: Configure PD9 (RX) as Alternate Function
     * ──────────────────────────────────────────────────────────────────────── */
    
    GPIOD->MODER &= ~(3U << (9 * 2));
    
    /* ✏️ YOUR TURN: Set mode to Alternate Function */
    GPIOD->MODER |= ???;        /* HINT: Same pattern as pin 8, but for pin 9 */
    
    /* ────────────────────────────────────────────────────────────────────────
     * Step 3: Set Alternate Function to AF7 for both pins
     * 
     * Pins 8-15 use AFR[1]. Pin 8 is at bit position 0 in AFR[1].
     * Pin position in AFR[1] = (pin - 8) * 4
     * ──────────────────────────────────────────────────────────────────────── */
    
    /* Clear AF bits for pin 8: position = (8-8)*4 = 0 */
    GPIOD->AFR[1] &= ~(0xFU << ((8 - 8) * 4));
    
    /* ✏️ YOUR TURN: Set AF7 for pin 8 */
    GPIOD->AFR[1] |= ???;       /* HINT: Use GPIO_AF7_USART3. AFR[1] is for pins 8-15, 4 bits each. */
    
    /* Clear AF bits for pin 9: position = (9-8)*4 = 4 */
    GPIOD->AFR[1] &= ~(0xFU << ((9 - 8) * 4));
    
    /* ✏️ YOUR TURN: Set AF7 for pin 9 */
    GPIOD->AFR[1] |= ???;       /* HINT: Same AF value, but shifted to pin 9's position in AFR[1] */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void UART_ConfigureGPIO(void) {
 *     GPIOD->MODER &= ~(3U << (8 * 2));
 *     GPIOD->MODER |= (2U << (8 * 2));
 *     
 *     GPIOD->MODER &= ~(3U << (9 * 2));
 *     GPIOD->MODER |= (2U << (9 * 2));
 *     
 *     GPIOD->AFR[1] &= ~(0xFU << ((8 - 8) * 4));
 *     GPIOD->AFR[1] |= (GPIO_AF7_USART3 << ((8 - 8) * 4));
 *     
 *     GPIOD->AFR[1] &= ~(0xFU << ((9 - 8) * 4));
 *     GPIOD->AFR[1] |= (GPIO_AF7_USART3 << ((9 - 8) * 4));
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 4: CALCULATING BAUD RATE
 *  ================================
 * 
 *  Baud rate = How many bits per second are transmitted
 *  Common values: 9600, 115200, 921600
 *  
 *  Formula:
 *  
 *    BRR = USART_clock / baud_rate
 *  
 *  For STM32H753 at default settings:
 *    - HSI = 64 MHz
 *    - APB1 clock = HSI / 1 = 64 MHz (default, no prescaler)
 *    - USART3 clock = APB1 clock = 64 MHz
 *  
 *  Example for 115200 baud:
 *    BRR = 64,000,000 / 115,200 = 555.55... ≈ 556
 * 
 * ============================================================================ */

#define HSI_CLOCK       64000000UL  /* 64 MHz */
#define BAUD_RATE       115200UL

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: CONFIGURE USART3
 *  ==================================
 * 
 *  Configure USART3 for 115200 baud, 8N1:
 *  1. Disable USART first (required before changing settings)
 *  2. Set baud rate in BRR
 *  3. Enable TX and RX
 *  4. Enable USART
 * 
 * ============================================================================ */

void UART_Configure(void) {
    /* Step 1: Make sure USART is disabled */
    USART3->CR1 &= ~USART_CR1_UE;
    
    /* ✏️ YOUR TURN: Step 2 - Calculate and set baud rate */
    /* BRR = clock / baud_rate */
    USART3->BRR = ???;          /* HINT: Divide the clock frequency by the baud rate. Use the defined constants. */
    
    /* ✏️ YOUR TURN: Step 3 - Enable Transmitter and Receiver */
    USART3->CR1 |= ???;         /* HINT: OR together the TX enable and RX enable bits. Check the defines. */
    
    /* ✏️ YOUR TURN: Step 4 - Enable USART */
    USART3->CR1 |= ???;         /* HINT: Which bit turns the USART on? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void UART_Configure(void) {
 *     USART3->CR1 &= ~USART_CR1_UE;
 *     USART3->BRR = HSI_CLOCK / BAUD_RATE;
 *     USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
 *     USART3->CR1 |= USART_CR1_UE;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 5: TRANSMITTING DATA
 *  ============================
 * 
 *  To send a byte:
 *  1. Wait until TXE (TX Empty) flag is set in ISR
 *  2. Write data to TDR (Transmit Data Register)
 *  
 *  TXE = 1 means the transmit buffer is empty and ready for new data
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: SEND A SINGLE CHARACTER
 *  ========================================
 * 
 *  Write a function to send one character over UART.
 * 
 * ============================================================================ */

void UART_SendChar(char c) {
    /* ✏️ YOUR TURN: Wait until TX buffer is empty */
    while (!(USART3->ISR & ???));   /* HINT: Which ISR flag indicates TX buffer is ready for new data? */
    
    /* ✏️ YOUR TURN: Write character to transmit register */
    USART3->TDR = ???;              /* HINT: What value do you want to transmit? (It's the function parameter) */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void UART_SendChar(char c) {
 *     while (!(USART3->ISR & USART_ISR_TXE_TXFNF));
 *     USART3->TDR = c;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: SEND A STRING
 *  ==============================
 * 
 *  Write a function to send a null-terminated string.
 * 
 * ============================================================================ */

void UART_SendString(const char* str) {
    /* ✏️ YOUR TURN: Loop through each character and send it */
    while (*str) {
        ???              /* HINT: Call the SendChar function. Don't forget to advance the pointer! */
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void UART_SendString(const char* str) {
 *     while (*str) {
 *         UART_SendChar(*str++);
 *     }
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 6: RECEIVING DATA
 *  =========================
 * 
 *  To receive a byte:
 *  1. Check if RXNE (RX Not Empty) flag is set in ISR
 *  2. Read data from RDR (Receive Data Register)
 *  
 *  RXNE = 1 means data has been received and is waiting to be read
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: RECEIVE A CHARACTER
 *  =====================================
 * 
 *  Write a function to receive one character (blocking).
 * 
 * ============================================================================ */

char UART_ReceiveChar(void) {
    /* ✏️ YOUR TURN: Wait until data is received */
    while (!(USART3->ISR & ???));   /* HINT: Which ISR flag indicates data has arrived? */
    
    /* ✏️ YOUR TURN: Read and return the received character */
    return (char)???;               /* HINT: Which register holds the received data? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * char UART_ReceiveChar(void) {
 *     while (!(USART3->ISR & USART_ISR_RXNE_RXFNE));
 *     return (char)USART3->RDR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ██████╗ ██████╗  █████╗  ██████╗████████╗██╗ ██████╗███████╗
 *  ██╔══██╗██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██║██╔════╝██╔════╝
 *  ██████╔╝██████╔╝███████║██║        ██║   ██║██║     █████╗  
 *  ██╔═══╝ ██╔══██╗██╔══██║██║        ██║   ██║██║     ██╔══╝  
 *  ██║     ██║  ██║██║  ██║╚██████╗   ██║   ██║╚██████╗███████╗
 *  ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝╚══════╝
 *  
 *  MAIN PROGRAM - Put it all together!
 * 
 * ============================================================================ */

int main(void)
{
    char received;
    
    /* Initialize UART */
    UART_EnableClocks();
    UART_ConfigureGPIO();
    UART_Configure();
    
    /* Send welcome message */
    UART_SendString("Hello from STM32H753 UART!\r\n");
    UART_SendString("Type something and I'll echo it back:\r\n");
    
    /* Echo loop - receive and send back */
    for(;;) {
        received = UART_ReceiveChar();
        UART_SendChar(received);
        
        /* Also send newline if Enter was pressed */
        if (received == '\r') {
            UART_SendChar('\n');
        }
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned UART without HAL:
 *  
 *  ✅ How UART serial communication works
 *  ✅ How to configure GPIO for Alternate Function
 *  ✅ How to calculate and set baud rate
 *  ✅ How to transmit data (polling)
 *  ✅ How to receive data (polling)
 *  
 *  NEXT STEPS:
 *  • Add interrupt-driven TX/RX (more efficient)
 *  • Use DMA for bulk transfers
 *  • Add printf() support by redirecting to UART
 *  
 *  TO TEST:
 *  Connect to the Nucleo's Virtual COM Port using:
 *  - PuTTY, Tera Term, or any serial terminal
 *  - Baud: 115200, 8N1
 * 
 * ============================================================================ */
