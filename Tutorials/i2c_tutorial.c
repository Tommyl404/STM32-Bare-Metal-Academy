/**
 ******************************************************************************
 * @file           : i2c_tutorial.c
 * @brief          : Learning I2C without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ██╗██████╗  ██████╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██║╚════██╗██╔════╝    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ██║ █████╔╝██║            ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██║██╔═══╝ ██║            ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ██║███████╗╚██████╗       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚═╝╚══════╝ ╚═════╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: I2C (Inter-Integrated Circuit)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. How I2C communication works (Master/Slave, addressing)
 *  2. How to configure GPIO pins for I2C
 *  3. How to configure I2C as Master
 *  4. How to write and read data from I2C devices
 * 
 *  PREREQUISITES:
 *  - Complete the RCC tutorial first!
 *  - Complete the UART tutorial (GPIO Alternate Functions)
 * 
 *  HARDWARE:
 *  - We'll use I2C1 for this tutorial
 *  - PB8 = I2C1_SCL (Clock)
 *  - PB9 = I2C1_SDA (Data)
 * 
 *  DIFFICULTY: ⭐⭐⭐⭐ (Intermediate-Advanced)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS I2C?
 *  =======================
 * 
 *  I2C uses only 2 wires for communication:
 *  
 *  ┌────────────┬──────────────────────────────────────────────────┐
 *  │ Signal     │ Description                                      │
 *  ├────────────┼──────────────────────────────────────────────────┤
 *  │ SCL        │ Serial Clock - Master generates this             │
 *  │ SDA        │ Serial Data - Bidirectional data line            │
 *  └────────────┴──────────────────────────────────────────────────┘
 *  
 *  KEY FEATURES:
 *  - Uses device ADDRESSES (7-bit or 10-bit)
 *  - Multiple devices on same bus
 *  - Open-drain outputs (need pull-up resistors!)
 *  - Slower than SPI, but fewer wires
 *  
 *  I2C BUS:
 *  
 *      VCC
 *       │
 *      ┌┴┐ ┌┴┐   (Pull-up resistors, typically 4.7kΩ)
 *      │ │ │ │
 *      └┬┘ └┬┘
 *  ─────┴───┴───────────────────────────── SCL
 *       │   │
 *  ─────┴───┴───────────────────────────── SDA
 *       │   │       │        │
 *  ┌────┴───┴──┐ ┌──┴────┐ ┌─┴─────┐
 *  │  MASTER   │ │ SLAVE │ │ SLAVE │
 *  │ (STM32)   │ │ 0x68  │ │ 0x50  │
 *  └───────────┘ └───────┘ └───────┘
 *  
 *  I2C TRANSACTION:
 *  
 *  START → ADDRESS → R/W → ACK → DATA → ACK → ... → STOP
 *  
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOB_BASE      0x58020400UL
#define I2C1_BASE       0x40005400UL

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

#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)

/* ============================================================================
 * 
 *  LESSON 1: I2C REGISTER STRUCTURE
 *  =================================
 * 
 *  ┌──────────────┬──────────────────────────────────────────────┐
 *  │ Register     │ Purpose                                      │
 *  ├──────────────┼──────────────────────────────────────────────┤
 *  │ CR1          │ Control 1 - Enable I2C, configure filters    │
 *  │ CR2          │ Control 2 - Slave address, transfer control  │
 *  │ TIMINGR      │ Timing - Clock speed configuration           │
 *  │ ISR          │ Status - Check TX empty, RX ready, etc       │
 *  │ TXDR         │ Transmit Data Register                       │
 *  │ RXDR         │ Receive Data Register                        │
 *  └──────────────┴──────────────────────────────────────────────┘
 * 
 * ============================================================================ */

typedef struct {
    volatile uint32_t CR1;      /* 0x00 - Control register 1 */
    volatile uint32_t CR2;      /* 0x04 - Control register 2 */
    volatile uint32_t OAR1;     /* 0x08 - Own address register 1 */
    volatile uint32_t OAR2;     /* 0x0C - Own address register 2 */
    volatile uint32_t TIMINGR;  /* 0x10 - Timing register */
    volatile uint32_t TIMEOUTR; /* 0x14 - Timeout register */
    volatile uint32_t ISR;      /* 0x18 - Interrupt and status register */
    volatile uint32_t ICR;      /* 0x1C - Interrupt clear register */
    volatile uint32_t PECR;     /* 0x20 - PEC register */
    volatile uint32_t RXDR;     /* 0x24 - Receive data register */
    volatile uint32_t TXDR;     /* 0x28 - Transmit data register */
} I2C_TypeDef;

#define I2C1    ((I2C_TypeDef *) I2C1_BASE)

/* ============================================================================
 * 
 *  LESSON 2: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC Clock Enable Bits */
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)   /* GPIOB clock enable */
#define RCC_APB1LENR_I2C1EN     (1U << 21)  /* I2C1 clock enable */

/* I2C_CR1 Register Bits */
#define I2C_CR1_PE              (1U << 0)   /* Peripheral Enable */
#define I2C_CR1_ANFOFF          (1U << 12)  /* Analog noise filter OFF */

/* I2C_CR2 Register Bits */
#define I2C_CR2_START           (1U << 13)  /* Start generation */
#define I2C_CR2_STOP            (1U << 14)  /* Stop generation */
#define I2C_CR2_NACK            (1U << 15)  /* NACK generation (for receive) */
#define I2C_CR2_AUTOEND         (1U << 25)  /* Automatic end mode */
#define I2C_CR2_RD_WRN          (1U << 10)  /* Transfer direction (0=write, 1=read) */

/* I2C_ISR Register Bits */
#define I2C_ISR_TXE             (1U << 0)   /* TX buffer empty */
#define I2C_ISR_TXIS            (1U << 1)   /* TX interrupt status */
#define I2C_ISR_RXNE            (1U << 2)   /* RX buffer not empty */
#define I2C_ISR_TC              (1U << 6)   /* Transfer complete */
#define I2C_ISR_NACKF           (1U << 4)   /* NACK received */
#define I2C_ISR_STOPF           (1U << 5)   /* Stop detected */
#define I2C_ISR_BUSY            (1U << 15)  /* Bus busy */

/* I2C_ICR Register Bits */
#define I2C_ICR_NACKCF          (1U << 4)   /* Clear NACK flag */
#define I2C_ICR_STOPCF          (1U << 5)   /* Clear STOP flag */

/* GPIO Alternate Function */
#define GPIO_AF4_I2C1           4U          /* AF4 = I2C1 */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE CLOCKS
 *  ==============================
 * 
 *  Enable clocks for GPIOB (AHB4) and I2C1 (APB1L).
 * 
 * ============================================================================ */

void I2C_EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOB clock */
    RCC->AHB4ENR |= ???;        /* HINT: Which bit enables GPIOB clock? */
    
    /* ✏️ YOUR TURN: Enable I2C1 clock (on APB1L bus) */
    RCC->APB1LENR |= ???;       /* HINT: Which bit enables I2C1 clock? */
    
    (void)RCC->APB1LENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void I2C_EnableClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 *     RCC->APB1LENR |= RCC_APB1LENR_I2C1EN;
 *     (void)RCC->APB1LENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE GPIO FOR I2C
 *  =======================================
 * 
 *  I2C requires OPEN-DRAIN outputs with pull-ups!
 *  
 *  Configure:
 *  - PB8 = I2C1_SCL (Alternate Function 4, Open-Drain)
 *  - PB9 = I2C1_SDA (Alternate Function 4, Open-Drain)
 * 
 * ============================================================================ */

void I2C_ConfigureGPIO(void) {
    /* ────────────────────────────────────────────────────────────────────────
     * PB8 = SCL
     * ──────────────────────────────────────────────────────────────────────── */
    
    /* Clear mode bits for PB8 */
    GPIOB->MODER &= ~(3U << (8 * 2));
    
    /* ✏️ YOUR TURN: Set PB8 as Alternate Function (mode = 10 = 2) */
    GPIOB->MODER |= ???;        /* HINT: AF mode = 10, shift to pin 8's position */
    
    /* ✏️ YOUR TURN: Set PB8 as Open-Drain (OTYPER bit = 1) */
    GPIOB->OTYPER |= ???;       /* HINT: Set the bit for pin 8 */
    
    /* Set high speed */
    GPIOB->OSPEEDR |= (3U << (8 * 2));
    
    /* Enable pull-up (PUPDR = 01) */
    GPIOB->PUPDR &= ~(3U << (8 * 2));
    GPIOB->PUPDR |= (1U << (8 * 2));
    
    /* Set AF4 for PB8 (pins 8-15 use AFR[1], so PB8 is at position 0 in AFR[1]) */
    GPIOB->AFR[1] &= ~(0xFU << (0 * 4));
    
    /* ✏️ YOUR TURN: Set AF4 for SCL (Each pin gets 4 bits in the AFR (Alternate Function Register)) */
    GPIOB->AFR[1] |= ???;       /* HINT: AF4 for I2C, PB8 is at position 0 in AFR[1] */
    
    /* ────────────────────────────────────────────────────────────────────────
     * PB9 = SDA (same configuration as SCL)
     * ──────────────────────────────────────────────────────────────────────── */
    GPIOB->MODER &= ~(3U << (9 * 2));
    GPIOB->MODER |= (2U << (9 * 2));
    GPIOB->OTYPER |= (1U << 9);
    GPIOB->OSPEEDR |= (3U << (9 * 2));
    GPIOB->PUPDR &= ~(3U << (9 * 2));
    GPIOB->PUPDR |= (1U << (9 * 2));
    GPIOB->AFR[1] &= ~(0xFU << (1 * 4));
    GPIOB->AFR[1] |= (GPIO_AF4_I2C1 << (1 * 4));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIOB->MODER |= (2U << (8 * 2));
 * GPIOB->OTYPER |= (1U << 8);
 * GPIOB->AFR[1] |= (GPIO_AF4_I2C1 << (0 * 4));
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 3: I2C TIMING
 *  =====================
 * 
 *  The TIMINGR register controls I2C clock speed.
 *  It's complex to calculate, so use STM32CubeMX or a table:
 *  
 *  For 64 MHz clock source:
 *  ┌────────────┬─────────────────────────────┐
 *  │ I2C Speed  │ TIMINGR Value               │
 *  ├────────────┼─────────────────────────────┤
 *  │ 100 kHz    │ 0x40E03758                  │
 *  │ 400 kHz    │ 0x10D03FCE                  │
 *  │ 1 MHz      │ 0x00700619                  │
 *  └────────────┴─────────────────────────────┘
 *  
 *  TIMINGR fields:
 *  - PRESC: Prescaler (bits 28-31)
 *  - SCLDEL: Data setup time (bits 20-23)
 *  - SDADEL: Data hold time (bits 16-19)
 *  - SCLH: SCL high period (bits 8-15)
 *  - SCLL: SCL low period (bits 0-7)
 * 
 * ============================================================================ */

/* Timing value for 100 kHz with 64 MHz clock */
#define I2C_TIMING_100KHZ   0x40E03758UL

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: CONFIGURE I2C
 *  ==============================
 * 
 *  Configure I2C1 for 100 kHz operation.
 * 
 * ============================================================================ */

void I2C_Configure(void) {
    /* ✏️ YOUR TURN: Disable I2C before configuration */
    I2C1->CR1 &= ~???;          /* HINT: Which bit enables/disables I2C? */
    
    /* Set timing for 100 kHz */
    I2C1->TIMINGR = I2C_TIMING_100KHZ;
    
    /* No analog filter (optional, can leave enabled) */
    /* I2C1->CR1 |= I2C_CR1_ANFOFF; */
    
    /* ✏️ YOUR TURN: Enable I2C peripheral */
    I2C1->CR1 |= ???;           /* HINT: Which bit enables the I2C peripheral? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void I2C_Configure(void) {
 *     I2C1->CR1 &= ~I2C_CR1_PE;
 *     I2C1->TIMINGR = I2C_TIMING_100KHZ;
 *     I2C1->CR1 |= I2C_CR1_PE;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 4: I2C TRANSFER SETUP
 *  =============================
 * 
 *  CR2 register controls each transfer:
 *  
 *  ┌─────────────────┬──────────────────────────────────────────────┐
 *  │ Field           │ Purpose                                      │
 *  ├─────────────────┼──────────────────────────────────────────────┤
 *  │ SADD[6:0]       │ Slave address (7-bit mode)                   │
 *  │ RD_WRN          │ 0=Write, 1=Read                              │
 *  │ NBYTES[7:0]     │ Number of bytes to transfer                  │
 *  │ AUTOEND         │ Automatic STOP after NBYTES                  │
 *  │ START           │ Generate START condition                     │
 *  │ STOP            │ Generate STOP condition                      │
 *  └─────────────────┴──────────────────────────────────────────────┘
 *  
 *  Address format in CR2 (7-bit addressing):
 *  - Bits 1-7 contain the 7-bit slave address
 *  - Bit 0 is not used (direction is in RD_WRN)
 *  - So: SADD = (slave_address << 1)
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: WRITE BYTES TO I2C DEVICE
 *  ==========================================
 * 
 *  Write data to a slave device.
 *  Remember:
 *  START → ADDRESS → R/W → ACK → DATA → ACK → ... → STOP
 * ============================================================================ */

void I2C_Write(uint8_t slave_addr, uint8_t *data, uint8_t length) {
    /* Wait until bus is not busy */
    while (I2C1->ISR & I2C_ISR_BUSY);
    
    /* Configure transfer:
     * - Slave address (shifted left by 1)
     * - Write direction (RD_WRN = 0)
     * - Number of bytes
     * - Auto-end mode
     */
    I2C1->CR2 = ((slave_addr << 1) & 0xFE)  /* 7-bit address in bits 1-7 */
              | (length << 16)               /* NBYTES */
              | I2C_CR2_AUTOEND;            /* Auto STOP */
    
    /* ✏️ YOUR TURN: Generate START condition */
    I2C1->CR2 |= ???;           /* HINT: Which bit generates START? */
    
    /* Send all bytes */
    for (uint8_t i = 0; i < length; i++) {
        /* ✏️ YOUR TURN: Wait until TX buffer is empty */
        while (!(I2C1->ISR & ???)); /* HINT: Which flag indicates TX ready? */
        
        /* ✏️ YOUR TURN: Write data byte */
        I2C1->TXDR = ???;       /* HINT: Current byte from the data array */
    }
    
    /* Wait for transfer complete */
    while (!(I2C1->ISR & I2C_ISR_STOPF));
    
    /* Clear STOP flag */
    I2C1->ICR = I2C_ICR_STOPCF;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * I2C1->CR2 |= I2C_CR2_START;
 * while (!(I2C1->ISR & I2C_ISR_TXIS));
 * I2C1->TXDR = data[i];
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: READ BYTES FROM I2C DEVICE
 *  ===========================================
 * 
 *  Read data from a slave device.
 *  Remember:
 *  START → ADDRESS → R/W → ACK → DATA → ACK → ... → STOP
 * ============================================================================ */

void I2C_Read(uint8_t slave_addr, uint8_t *data, uint8_t length) {
    /* Wait until bus is not busy */
    while (I2C1->ISR & I2C_ISR_BUSY);
    
    /* Configure transfer:
     * - Slave address
     * - READ direction
     * - Number of bytes
     * - Auto-end mode
     */
    I2C1->CR2 = ((slave_addr << 1) & 0xFE)  /* 7-bit address */
              | I2C_CR2_RD_WRN              /* Read mode */
              | (length << 16)               /* NBYTES */
              | I2C_CR2_AUTOEND;            /* Auto STOP */
    
    /* Generate START */
    I2C1->CR2 |= I2C_CR2_START;
    
    /* Read all bytes */
    for (uint8_t i = 0; i < length; i++) {
        /* ✏️ YOUR TURN: Wait until RX buffer has data */
        while (!(I2C1->ISR & ???)); /* HINT: Which flag indicates RX data available? */
        
        /* ✏️ YOUR TURN: Read data byte */
        data[i] = ???;          /* HINT: Read from receive data register */
    }
    
    /* Wait for STOP */
    while (!(I2C1->ISR & I2C_ISR_STOPF));
    
    /* Clear STOP flag */
    I2C1->ICR = I2C_ICR_STOPCF;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * while (!(I2C1->ISR & I2C_ISR_RXNE));
 * data[i] = I2C1->RXDR;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: WRITE THEN READ (COMMON PATTERN)
 *  ==================================================
 * 
 *  Most I2C devices use this pattern:
 *  1. Write the register address
 *  2. Read the register value
 * 
 *  This is called a "combined transfer" or "repeated start"
 *  START → ADDRESS → R/W → ACK → DATA → ACK → ... → STOP
 * ============================================================================ */

uint8_t I2C_ReadRegister(uint8_t slave_addr, uint8_t reg_addr) {
    uint8_t value;
    
    /* Wait until bus is not busy */
    while (I2C1->ISR & I2C_ISR_BUSY);
    
    /* ────────────────────────────────────────────────────────────────────────
     * STEP 1: Write the register address (no AUTOEND, no STOP)
     * ──────────────────────────────────────────────────────────────────────── */
    I2C1->CR2 = ((slave_addr << 1) & 0xFE)  /* Address */
              | (1 << 16);                    /* 1 byte to send */
    /* Note: No AUTOEND - we want a repeated start, not a stop */
    
    /* ✏️ YOUR TURN: Generate START */
    I2C1->CR2 |= ???;            /* HINT: Which bit generates START? */
    
    /* Wait for TX empty */
    while (!(I2C1->ISR & I2C_ISR_TXIS));
    
    /* ✏️ YOUR TURN: Send register address */
    I2C1->TXDR = ???;           /* HINT: Register address to read from */
    
    /* Wait for transfer complete (not STOP, just TC) */
    while (!(I2C1->ISR & I2C_ISR_TC));
    
    /* ────────────────────────────────────────────────────────────────────────
     * STEP 2: Read the register value (with AUTOEND for STOP)
     * ──────────────────────────────────────────────────────────────────────── */
    I2C1->CR2 = ((slave_addr << 1) & 0xFE)
              | I2C_CR2_RD_WRN              /* Read */
              | (1 << 16)                    /* 1 byte to read */
              | I2C_CR2_AUTOEND;            /* Auto STOP */
    
    /* Generate repeated START */
    I2C1->CR2 |= I2C_CR2_START;
    
    /* Wait for RX data */
    while (!(I2C1->ISR & I2C_ISR_RXNE));
    
    /* ✏️ YOUR TURN: Read the value */
    value = ???;                /* HINT: Read from receive data register */
    
    /* Wait for STOP */
    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF;
    
    return value;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * I2C1->CR2 |= I2C_CR2_START;
 * I2C1->TXDR = reg_addr;
 * value = I2C1->RXDR;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  BONUS: WRITE TO A REGISTER
 *  ===========================
 * 
 * ============================================================================ */

void I2C_WriteRegister(uint8_t slave_addr, uint8_t reg_addr, uint8_t value) {
    uint8_t data[2] = {reg_addr, value};
    I2C_Write(slave_addr, data, 2);
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
 *  MAIN PROGRAM - Example with a common I2C device
 * 
 * ============================================================================ */

void delay(volatile uint32_t count) {
    while (count--);
}

int main(void)
{
    uint8_t whoami;
    
    /* ────────────────────────────────────────────────────────────────────────
     * Common I2C device addresses (7-bit):
     * 
     * 0x68 - MPU6050 (Accelerometer/Gyro)
     * 0x76/0x77 - BME280/BMP280 (Pressure/Temp)
     * 0x50-0x57 - AT24C EEPROM
     * 0x3C/0x3D - SSD1306 OLED Display
     * 0x48-0x4F - TMP102 Temperature Sensor
     * ──────────────────────────────────────────────────────────────────────── */
    
    #define MPU6050_ADDR    0x68
    #define MPU6050_WHO_AM_I 0x75
    
    /* Initialize I2C */
    I2C_EnableClocks();
    I2C_ConfigureGPIO();
    I2C_Configure();
    
    /* Read WHO_AM_I register from MPU6050 */
    /* Should return 0x68 */
    whoami = I2C_ReadRegister(MPU6050_ADDR, MPU6050_WHO_AM_I);
    
    /* If you have an EEPROM, you could test like this: */
    /* I2C_WriteRegister(0x50, 0x00, 0xAB); */
    /* delay(10000); // EEPROM needs write time */
    /* uint8_t read_back = I2C_ReadRegister(0x50, 0x00); */
    
    for(;;) {
        /* Your application code here */
        delay(1000000);
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned I2C without HAL:
 *  
 *  ✅ How I2C works (2-wire, addressing, ACK/NACK)
 *  ✅ Open-drain outputs with pull-ups
 *  ✅ GPIO configuration for I2C
 *  ✅ I2C timing configuration
 *  ✅ Writing data to I2C devices
 *  ✅ Reading data from I2C devices
 *  ✅ Combined write-then-read (repeated start)
 *  
 *  COMMON I2C DEVICES TO TRY:
 *  • EEPROM (24LC, AT24)
 *  • Temperature sensors (TMP102, LM75)
 *  • Accelerometer/Gyro (MPU6050)
 *  • Pressure sensors (BMP280, BME280)
 *  • OLED Displays (SSD1306)
 *  • RTC (DS3231)
 *  
 *  DEBUGGING TIPS:
 *  • Check pull-up resistors (4.7kΩ typical)
 *  • Verify slave address (some datasheets show 8-bit, divide by 2!)
 *  • Use logic analyzer to see SCL/SDA waveforms
 *  • Check NACK flag for addressing problems
 * 
 * ============================================================================ */
