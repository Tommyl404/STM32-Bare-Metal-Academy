/**
 ******************************************************************************
 * @file           : spi_tutorial.c
 * @brief          : Learning SPI without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ███████╗██████╗ ██╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔════╝██╔══██╗██║    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ███████╗██████╔╝██║       ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ╚════██║██╔═══╝ ██║       ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ███████║██║     ██║       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚══════╝╚═╝     ╚═╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: SPI (Serial Peripheral Interface)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. How SPI communication works (Master/Slave, CPOL, CPHA)
 *  2. How to configure GPIO pins for SPI
 *  3. How to configure SPI as Master
 *  4. How to transmit and receive data
 * 
 *  PREREQUISITES:
 *  - Complete the RCC tutorial first!
 *  - Complete the UART tutorial (GPIO Alternate Functions)
 * 
 *  HARDWARE:
 *  - We'll use SPI1 for this tutorial
 *  - PA5 = SPI1_SCK  (Clock)
 *  - PA6 = SPI1_MISO (Master In, Slave Out)
 *  - PA7 = SPI1_MOSI (Master Out, Slave In)
 *  - PA4 = CS (Chip Select - manual GPIO control)
 * 
 *  DIFFICULTY: ⭐⭐⭐ (Intermediate)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS SPI?
 *  =======================
 * 
 *  SPI is a synchronous serial protocol. It's FAST and uses 4 wires:
 *  
 *  ┌────────────┬────────────────────────────────────────────────┐
 *  │ Signal     │ Description                                    │
 *  ├────────────┼────────────────────────────────────────────────┤
 *  │ SCK/SCLK   │ Serial Clock - Master generates this          │
 *  │ MOSI       │ Master Out, Slave In - Data from master       │
 *  │ MISO       │ Master In, Slave Out - Data from slave        │
 *  │ CS/SS      │ Chip Select - Selects which slave to talk to  │
 *  └────────────┴────────────────────────────────────────────────┘
 *  
 *  SPI is FULL DUPLEX - data goes both ways simultaneously!
 *  
 *       MASTER                          SLAVE
 *    ┌─────────┐                    ┌─────────┐
 *    │         │─── SCK ──────────►│         │
 *    │         │─── MOSI ─────────►│         │
 *    │         │◄── MISO ──────────│         │
 *    │         │─── CS ───────────►│         │
 *    └─────────┘                    └─────────┘
 *  
 *  KEY CONCEPTS:
 *  - CPOL (Clock Polarity): Idle state of clock (0=low, 1=high)
 *  - CPHA (Clock Phase): Which edge samples data (0=first, 1=second)
 *  
 *  Common modes:
 *    Mode 0: CPOL=0, CPHA=0 (most common)
 *    Mode 1: CPOL=0, CPHA=1
 *    Mode 2: CPOL=1, CPHA=0
 *    Mode 3: CPOL=1, CPHA=1
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOA_BASE      0x58020000UL
#define SPI1_BASE       0x40013000UL

/* ============================================================================
 *  RCC REGISTERS
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
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

#define GPIOA   ((GPIO_TypeDef *) GPIOA_BASE)

/* ============================================================================
 * 
 *  LESSON 1: SPI REGISTER STRUCTURE
 *  =================================
 * 
 *  The SPI peripheral registers:
 *  
 *  ┌──────────────┬──────────────────────────────────────────────┐
 *  │ Register     │ Purpose                                      │
 *  ├──────────────┼──────────────────────────────────────────────┤
 *  │ CR1          │ Control 1 - Enable SPI, Master mode          │
 *  │ CR2          │ Control 2 - Data size, FIFO threshold        │
 *  │ CFG1         │ Config 1 - Baud rate prescaler               │
 *  │ CFG2         │ Config 2 - Master/Slave, CPOL, CPHA          │
 *  │ SR           │ Status - Check TX empty, RX ready, busy      │
 *  │ TXDR         │ Transmit Data Register                       │
 *  │ RXDR         │ Receive Data Register                        │
 *  └──────────────┴──────────────────────────────────────────────┘
 * 
 * ============================================================================ */

typedef struct {
    volatile uint32_t CR1;      /* 0x00 - Control register 1 */
    volatile uint32_t CR2;      /* 0x04 - Control register 2 */
    volatile uint32_t CFG1;     /* 0x08 - Configuration register 1 */
    volatile uint32_t CFG2;     /* 0x0C - Configuration register 2 */
    volatile uint32_t IER;      /* 0x10 - Interrupt enable register */
    volatile uint32_t SR;       /* 0x14 - Status register */
    volatile uint32_t IFCR;     /* 0x18 - Interrupt flag clear register */
    volatile uint32_t RESERVED0;/* 0x1C */
    volatile uint32_t TXDR;     /* 0x20 - Transmit data register */
    volatile uint32_t RESERVED1[3];
    volatile uint32_t RXDR;     /* 0x30 - Receive data register */
    volatile uint32_t RESERVED2[3];
    volatile uint32_t CRCPOLY;  /* 0x40 - CRC polynomial register */
    volatile uint32_t TXCRC;    /* 0x44 - TX CRC register */
    volatile uint32_t RXCRC;    /* 0x48 - RX CRC register */
    volatile uint32_t UDRDR;    /* 0x4C - Underrun data register */
    volatile uint32_t I2SCFGR;  /* 0x50 - I2S configuration register */
} SPI_TypeDef;

#define SPI1    ((SPI_TypeDef *) SPI1_BASE)

/* ============================================================================
 * 
 *  LESSON 2: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC Clock Enable Bits */
#define RCC_AHB4ENR_GPIOAEN     (1U << 0)   /* GPIOA clock enable */
#define RCC_APB2ENR_SPI1EN      (1U << 12)  /* SPI1 clock enable */

/* SPI_CR1 Register Bits */
#define SPI_CR1_SPE             (1U << 0)   /* SPI Enable */
#define SPI_CR1_CSTART          (1U << 9)   /* Master transfer start */

/* SPI_CFG1 Register */
/* DSIZE[4:0] = Data size (bits 0-4), value = data_bits - 1 */
/* MBR[2:0] = Baud rate prescaler (bits 28-30) */
#define SPI_CFG1_DSIZE_8BIT     (7U << 0)   /* 8-bit data (8-1=7) */
#define SPI_CFG1_MBR_DIV8       (2U << 28)  /* Clock / 8 */
#define SPI_CFG1_MBR_DIV32      (4U << 28)  /* Clock / 32 */

/* SPI_CFG2 Register */
#define SPI_CFG2_MASTER         (1U << 22)  /* Master mode */
#define SPI_CFG2_SSOE           (1U << 29)  /* SS output enable */
#define SPI_CFG2_SSM            (1U << 26)  /* Software slave management */
#define SPI_CFG2_SSOM           (1U << 30)  /* SS output management */
#define SPI_CFG2_COMM_FULLDUPLEX (0U << 17) /* Full duplex mode */

/* SPI_SR Register Bits */
#define SPI_SR_TXP              (1U << 1)   /* TX packet space available */
#define SPI_SR_RXP              (1U << 0)   /* RX packet available */
#define SPI_SR_EOT              (1U << 3)   /* End of transfer */
#define SPI_SR_TXTF             (1U << 4)   /* Transmission transfer filled */

/* GPIO Alternate Function */
#define GPIO_AF5_SPI1           5U          /* AF5 = SPI1 */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE CLOCKS
 *  ==============================
 * 
 *  Enable clocks for GPIOA (AHB4) and SPI1 (APB2).
 * 
 * ============================================================================ */

void SPI_EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOA clock */
    RCC->AHB4ENR |= ???;        /* HINT: Which bit enables GPIOA clock? */
    
    /* ✏️ YOUR TURN: Enable SPI1 clock (on APB2 bus) */
    RCC->APB2ENR |= ???;        /* HINT: Which bit enables SPI1 clock? */
    
    (void)RCC->APB2ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void SPI_EnableClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
 *     RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
 *     (void)RCC->APB2ENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE GPIO FOR SPI
 *  =======================================
 * 
 *  Configure:
 *  - PA4 = CS (GPIO Output - we control it manually)
 *  - PA5 = SCK (Alternate Function 5)
 *  - PA6 = MISO (Alternate Function 5)
 *  - PA7 = MOSI (Alternate Function 5)
 * 
 * ============================================================================ */

void SPI_ConfigureGPIO(void) {
    /* ────────────────────────────────────────────────────────────────────────
     * PA4 = Chip Select (CS) - Configure as GPIO Output
     * ──────────────────────────────────────────────────────────────────────── */
    GPIOA->MODER &= ~(3U << (4 * 2));
    
    /* ✏️ YOUR TURN: Set PA4 as Output (mode = 01) */
    GPIOA->MODER |= ???;        /* HINT: Output mode = 01, shift to pin 4's position */
    
    /* Start with CS HIGH (deselected) */
    GPIOA->ODR |= (1U << 4);
    
    /* ────────────────────────────────────────────────────────────────────────
     * PA5 = SCK - Configure as Alternate Function
     * ──────────────────────────────────────────────────────────────────────── */
    GPIOA->MODER &= ~(3U << (5 * 2));
    
    /* ✏️ YOUR TURN: Set PA5 as Alternate Function (mode = 10 = 2) */
    GPIOA->MODER |= ???;        /* HINT: AF mode = 10, shift to pin 5's position */
    
    /* Set AF5 for PA5 (pins 0-7 use AFR[0]) */
    GPIOA->AFR[0] &= ~(0xFU << (5 * 4));
    
    /* ✏️ YOUR TURN: Set AF5 for SCK */
    GPIOA->AFR[0] |= ???;       /* HINT: AF5 for SPI1, 4 bits per pin */
    
    /* ────────────────────────────────────────────────────────────────────────
     * PA6 = MISO - Alternate Function
     * ──────────────────────────────────────────────────────────────────────── */
    GPIOA->MODER &= ~(3U << (6 * 2));
    GPIOA->MODER |= (2U << (6 * 2));
    GPIOA->AFR[0] &= ~(0xFU << (6 * 4));
    GPIOA->AFR[0] |= (GPIO_AF5_SPI1 << (6 * 4));
    
    /* ────────────────────────────────────────────────────────────────────────
     * PA7 = MOSI - Alternate Function
     * ──────────────────────────────────────────────────────────────────────── */
    GPIOA->MODER &= ~(3U << (7 * 2));
    GPIOA->MODER |= (2U << (7 * 2));
    GPIOA->AFR[0] &= ~(0xFU << (7 * 4));
    GPIOA->AFR[0] |= (GPIO_AF5_SPI1 << (7 * 4));
    
    /* Set high speed for SPI pins */
    GPIOA->OSPEEDR |= (3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIOA->MODER |= (1U << (4 * 2));     // PA4 output
 * GPIOA->MODER |= (2U << (5 * 2));     // PA5 AF
 * GPIOA->AFR[0] |= (GPIO_AF5_SPI1 << (5 * 4));
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 3: CHIP SELECT (CS) CONTROL
 *  ===================================
 * 
 *  CS is active LOW:
 *  - CS = 0: Slave is SELECTED (listening)
 *  - CS = 1: Slave is DESELECTED (ignoring)
 *  
 *  Always:
 *  1. Pull CS LOW before communication
 *  2. Do your SPI transfers
 *  3. Pull CS HIGH when done
 * 
 * ============================================================================ */

void SPI_CS_Low(void) {
    GPIOA->ODR &= ~(1U << 4);   /* PA4 = 0 (select) */
}

void SPI_CS_High(void) {
    GPIOA->ODR |= (1U << 4);    /* PA4 = 1 (deselect) */
}

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: CONFIGURE SPI
 *  ==============================
 * 
 *  Configure SPI1 as Master, Mode 0 (CPOL=0, CPHA=0), 8-bit data.
 * 
 * ============================================================================ */

void SPI_Configure(void) {
    /* Make sure SPI is disabled before configuration */
    SPI1->CR1 &= ~SPI_CR1_SPE;
    
    /* ────────────────────────────────────────────────────────────────────────
     * CFG1: Set data size to 8-bit, prescaler to div32
     * ──────────────────────────────────────────────────────────────────────── */
    SPI1->CFG1 = 0;
    
    /* ✏️ YOUR TURN: Set 8-bit data size and prescaler */
    SPI1->CFG1 |= ???;          /* HINT: Combine data size and baud rate settings */
    
    /* ────────────────────────────────────────────────────────────────────────
     * CFG2: Master mode, software SS management
     * ──────────────────────────────────────────────────────────────────────── */
    SPI1->CFG2 = 0;
    
    /* ✏️ YOUR TURN: Set Master mode */
    SPI1->CFG2 |= ???;          /* HINT: Which bit sets master mode? */
    
    /* Software slave management (we control CS manually) */
    SPI1->CFG2 |= SPI_CFG2_SSM;
    
    /* ────────────────────────────────────────────────────────────────────────
     * CR1: Enable SPI
     * ──────────────────────────────────────────────────────────────────────── */
    
    /* ✏️ YOUR TURN: Enable SPI */
    SPI1->CR1 |= ???;           /* HINT: Which bit enables the SPI peripheral? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void SPI_Configure(void) {
 *     SPI1->CR1 &= ~SPI_CR1_SPE;
 *     SPI1->CFG1 = SPI_CFG1_DSIZE_8BIT | SPI_CFG1_MBR_DIV32;
 *     SPI1->CFG2 = SPI_CFG2_MASTER | SPI_CFG2_SSM;
 *     SPI1->CR1 |= SPI_CR1_SPE;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 4: SPI TRANSFER
 *  =======================
 * 
 *  SPI is full-duplex: sending and receiving happen simultaneously!
 *  
 *  For STM32H7 SPI:
 *  1. Set transfer size in CR2 (TSIZE)
 *  2. Start transfer with CSTART bit
 *  3. Write to TXDR when TXP=1
 *  4. Read from RXDR when RXP=1
 *  5. Wait for EOT (End of Transfer)
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: TRANSFER ONE BYTE
 *  ===================================
 * 
 *  Send a byte and receive a byte simultaneously.
 * 
 * ============================================================================ */

uint8_t SPI_Transfer(uint8_t data) {
    /* Set transfer size to 1 byte */
    SPI1->CR2 = 1;
    
    /* Start transfer */
    SPI1->CR1 |= SPI_CR1_CSTART;
    
    /* ✏️ YOUR TURN: Wait until TX buffer has space */
    while (!(SPI1->SR & ???));  /* HINT: Which flag indicates TX space available? */
    
    /* ✏️ YOUR TURN: Write data to transmit register */
    *((volatile uint8_t*)&SPI1->TXDR) = ???;  /* HINT: What byte to transmit? */
    
    /* ✏️ YOUR TURN: Wait until RX data is available */
    while (!(SPI1->SR & ???));  /* HINT: Which flag indicates RX data ready? */
    
    /* ✏️ YOUR TURN: Read received data */
    uint8_t received = ???;     /* HINT: Read from the receive data register */
    
    /* Wait for transfer to complete */
    while (!(SPI1->SR & SPI_SR_EOT));
    
    /* Clear EOT flag */
    SPI1->IFCR = SPI_SR_EOT;
    
    return received;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint8_t SPI_Transfer(uint8_t data) {
 *     SPI1->CR2 = 1;
 *     SPI1->CR1 |= SPI_CR1_CSTART;
 *     while (!(SPI1->SR & SPI_SR_TXP));
 *     *((volatile uint8_t*)&SPI1->TXDR) = data;
 *     while (!(SPI1->SR & SPI_SR_RXP));
 *     uint8_t received = *((volatile uint8_t*)&SPI1->RXDR);
 *     while (!(SPI1->SR & SPI_SR_EOT));
 *     SPI1->IFCR = SPI_SR_EOT;
 *     return received;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: READ A REGISTER FROM SPI DEVICE
 *  =================================================
 * 
 *  Many SPI devices (sensors, etc.) have registers.
 *  To read: Send register address, then read the response.
 * 
 * Time →
─────────────────────────────────────────────────────────────

CS:     HIGH ──┐        ┌──────────────────────┐        ┌── HIGH
               └────────┘ LOW (slave listening)└────────┘

               ▼                                 ▼
            Step 1                            Step 4
           "Wake up!"                      "Go to sleep"

               │                                │
               ├─► Step 2: Send 0x8F (address)  │
               │   Slave: "OK, reading reg 0x0F"│
               │                                │
               └─► Step 3: Read 0x33 (data)     │
                   Slave: "Here's the value!"   │
 * 
 * 
 * 
 * ============================================================================ */

uint8_t SPI_ReadRegister(uint8_t reg) {
    uint8_t value;
    
    /* ✏️ YOUR TURN: Assert CS (select the device) */
    ???;                         /* HINT: Call function to pull CS low */
    
    /* Send register address (often with read bit set) */
    SPI_Transfer(reg | 0x80);   /* 0x80 = read flag for many devices */
    
    /* ✏️ YOUR TURN: Read the register value (send dummy 0x00) */
    value = ???;                /* HINT: Send dummy byte to receive response */
    
    /* ✏️ YOUR TURN: Deassert CS (deselect the device) */
    ???;                         /* HINT: Call function to pull CS high */
    
    return value;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint8_t SPI_ReadRegister(uint8_t reg) {
 *     uint8_t value;
 *     SPI_CS_Low();
 *     SPI_Transfer(reg | 0x80);
 *     value = SPI_Transfer(0x00);
 *     SPI_CS_High();
 *     return value;
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
 *  MAIN PROGRAM - Example with a generic SPI device
 * 
 * ============================================================================ */

void delay(volatile uint32_t count) {
    while (count--);
}

int main(void)
{
    uint8_t whoami;
    
    /* Initialize SPI */
    SPI_EnableClocks();
    SPI_ConfigureGPIO();
    SPI_Configure();
    
    /* Example: Read WHO_AM_I register (common on sensors) */
    /* Most accelerometers/gyros have this at register 0x0F or 0x75 */
    whoami = SPI_ReadRegister(0x0F);
    
    /* Your device should return its ID here */
    /* e.g., LIS3DH returns 0x33, MPU6000 returns 0x68 */
    
    for(;;) {
        /* Toggle CS to show we're alive */
        SPI_CS_Low();
        delay(100000);
        SPI_CS_High();
        delay(100000);
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned SPI without HAL:
 *  
 *  ✅ How SPI communication works (4-wire, full duplex)
 *  ✅ Master/Slave concept and CS control
 *  ✅ CPOL and CPHA (clock polarity and phase)
 *  ✅ GPIO Alternate Function configuration
 *  ✅ SPI register configuration
 *  ✅ Transmit and receive data
 *  ✅ Read registers from SPI devices
 *  
 *  COMMON SPI DEVICES TO TRY:
 *  • SD Card (SPI mode)
 *  • SPI Flash (W25Q series)
 *  • Accelerometer (LIS3DH, MPU6000)
 *  • Display (ILI9341, ST7789)
 *  • ADC/DAC chips
 * 
 * ============================================================================ */
