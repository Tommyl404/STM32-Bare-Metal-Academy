/**
 ******************************************************************************
 * @file           : rcc_tutorial.c
 * @brief          : Learning RCC (Reset and Clock Control) without HAL
 ******************************************************************************
 * 
 *  ██████╗  ██████╗ ██████╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔══██╗██╔════╝██╔════╝    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ██████╔╝██║     ██║            ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██╔══██╗██║     ██║            ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ██║  ██║╚██████╗╚██████╗       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚═╝  ╚═╝ ╚═════╝ ╚═════╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: RESET AND CLOCK CONTROL
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What RCC is and why it's critical
 *  2. Memory-mapped peripherals concept
 *  3. Enabling peripheral clocks (THE #1 rule!)
 *  4. Checking and switching clock sources
 *  5. Configuring PLL for maximum speed
 * 
 *  HARDWARE (Nucleo-H753ZI LEDs):
 *  ┌──────────┬───────────────────────────────────────────┐
 *  │ PB0      │ Green LED (LD1)                           │
 *  │ PE1      │ Yellow LED (LD2)                          │
 *  │ PB14     │ Red LED (LD3)                             │
 *  └──────────┴───────────────────────────────────────────┘
 * 
 *  DIFFICULTY: ⭐⭐ (Beginner-Intermediate)
 *  
 *  ⚠️ RCC is the FIRST thing to learn - nothing works without clocks!
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHY RCC MATTERS - THE #1 RULE
 *  =========================================
 * 
 *  RCC = Reset and Clock Control
 *  
 *  THE GOLDEN RULE OF STM32:
 *  ═══════════════════════════════════════════════════════════════════════════
 *  
 *         ╔═══════════════════════════════════════════════════════════╗
 *         ║  BEFORE USING ANY PERIPHERAL, YOU MUST ENABLE ITS CLOCK!  ║
 *         ╚═══════════════════════════════════════════════════════════╝
 *  
 *  Without Clock:              With Clock:
 *  ──────────────              ───────────
 *  • Registers read as 0       • Registers work normally
 *  • Writes are ignored        • Can read/write values
 *  • GPIO pins don't work      • Peripheral operates
 *  • "Why doesn't it work?!"   • "It works!" ✓
 *  
 *  
 *  WHY DO CLOCKS NEED TO BE ENABLED?
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  By default, peripheral clocks are OFF to save power:
 *  
 *       ┌──────────┐   Clock OFF    ┌──────────────┐
 *       │   RCC    │───────╳───────│  GPIO, UART   │  ← Dead! No power!
 *       │  (you)   │                │  SPI, I2C... │
 *       └──────────┘                └──────────────┘
 *  
 *       ┌──────────┐   Clock ON     ┌──────────────┐
 *       │   RCC    │───────✓───────│  GPIO, UART   │  ← Alive! Working!
 *       │  (you)   │                │  SPI, I2C... │
 *       └──────────┘                └──────────────┘
 * 
 * 
 *  STM32H7 CLOCK TREE (Simplified):
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │                        CLOCK SOURCES                                │
 *    ├──────────┬──────────────────────────────────────────────────────────┤
 *    │ HSI      │ High Speed Internal - 64 MHz (default at reset)          │
 *    │ HSE      │ High Speed External - 8 MHz crystal on Nucleo            │
 *    │ LSI      │ Low Speed Internal - ~32 kHz (for watchdog, RTC)         │
 *    │ LSE      │ Low Speed External - 32.768 kHz (for RTC)                │
 *    │ PLL      │ Phase-Locked Loop - multiply clock up to 480 MHz!        │
 *    └──────────┴──────────────────────────────────────────────────────────┘
 *  
 *                         ┌─────────┐
 *    HSI (64 MHz) ───────►│         │
 *                         │   MUX   ├──────► SYSCLK ──────► CPU
 *    HSE (8 MHz) ────────►│  (SW)   │                       AHB buses
 *                         │         │                       APB buses
 *    PLL (up to 480) ────►│         │                       Peripherals
 *                         └─────────┘
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  LESSON 1: MEMORY-MAPPED PERIPHERALS
 *  =====================================
 * 
 *  Peripherals are controlled through registers at fixed memory addresses.
 *  
 *  ┌──────────────────────────────────────────────────────────────────────────┐
 *  │                        STM32H7 MEMORY MAP                                │
 *  ├──────────────────┬───────────────────────────────────────────────────────┤
 *  │ 0x00000000       │ Flash memory (your code)                              │
 *  │ 0x20000000       │ SRAM (variables)                                      │
 *  │ 0x40000000       │ APB1 peripherals (UART, SPI, I2C, TIM2-7...)          │
 *  │ 0x40010000       │ APB2 peripherals (TIM1, USART1, SPI1...)              │
 *  │ 0x40020000       │ AHB1 peripherals (DMA, ETH...)                        │
 *  │ 0x50000000       │ AHB2 peripherals (ADC, DCMI...)                       │
 *  │ 0x58020000       │ AHB4 peripherals (GPIO!)                              │
 *  │ 0x58024400       │ RCC (Reset and Clock Control) ← WE ARE HERE           │
 *  └──────────────────┴───────────────────────────────────────────────────────┘
 *  
 *  
 *  HOW WE ACCESS REGISTERS:
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  Method 1: Direct pointer (ugly, hard to read)
 *  
 *      *((volatile uint32_t *)0x580244E0) |= (1 << 1);
 *  
 *  Method 2: Struct pointer (clean, readable!) ← WE USE THIS
 *  
 *      RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 * 
 * ============================================================================ */

/* Peripheral Base Addresses */
#define RCC_BASE        0x58024400UL
#define GPIOB_BASE      0x58020400UL
#define GPIOE_BASE      0x58021000UL

/* ============================================================================
 * 
 *  LESSON 2: REGISTER STRUCTURES
 *  ===============================
 * 
 *  We use C structs to map register layouts.
 *  Each field is 32 bits and matches the hardware layout.
 *  
 *  The 'volatile' keyword tells the compiler:
 *  "Don't optimize reads/writes - hardware can change this anytime!"
 * 
 * ============================================================================ */

/* RCC Register Structure (partial - key registers) */
typedef struct {
    volatile uint32_t CR;           /* 0x00 - Clock control register */
    volatile uint32_t HSICFGR;      /* 0x04 - HSI configuration */
    volatile uint32_t CRRCR;        /* 0x08 - Clock recovery RC register */
    volatile uint32_t CSICFGR;      /* 0x0C - CSI configuration */
    volatile uint32_t CFGR;         /* 0x10 - Clock configuration register */
    volatile uint32_t RESERVED1;    /* 0x14 */
    volatile uint32_t D1CFGR;       /* 0x18 - Domain 1 clock config */
    volatile uint32_t D2CFGR;       /* 0x1C - Domain 2 clock config */
    volatile uint32_t D3CFGR;       /* 0x20 - Domain 3 clock config */
    volatile uint32_t RESERVED2;    /* 0x24 */
    volatile uint32_t PLLCKSELR;    /* 0x28 - PLL clock source select */
    volatile uint32_t PLLCFGR;      /* 0x2C - PLL configuration */
    volatile uint32_t PLL1DIVR;     /* 0x30 - PLL1 dividers */
    volatile uint32_t PLL1FRACR;    /* 0x34 - PLL1 fractional */
    volatile uint32_t PLL2DIVR;     /* 0x38 - PLL2 dividers */
    volatile uint32_t PLL2FRACR;    /* 0x3C - PLL2 fractional */
    volatile uint32_t PLL3DIVR;     /* 0x40 - PLL3 dividers */
    volatile uint32_t PLL3FRACR;    /* 0x44 - PLL3 fractional */
    volatile uint32_t RESERVED3;    /* 0x48 */
    volatile uint32_t D1CCIPR;      /* 0x4C - Domain 1 kernel clock */
    volatile uint32_t D2CCIP1R;     /* 0x50 - Domain 2 kernel clock 1 */
    volatile uint32_t D2CCIP2R;     /* 0x54 - Domain 2 kernel clock 2 */
    volatile uint32_t D3CCIPR;      /* 0x58 - Domain 3 kernel clock */
    volatile uint32_t RESERVED4;    /* 0x5C */
    volatile uint32_t CIER;         /* 0x60 - Interrupt enable */
    volatile uint32_t CIFR;         /* 0x64 - Interrupt flag */
    volatile uint32_t CICR;         /* 0x68 - Interrupt clear */
    volatile uint32_t RESERVED5;    /* 0x6C */
    volatile uint32_t BDCR;         /* 0x70 - Backup domain control */
    volatile uint32_t CSR;          /* 0x74 - Control and status */
    volatile uint32_t RESERVED6;    /* 0x78 */
    volatile uint32_t AHB3RSTR;     /* 0x7C - AHB3 reset */
    volatile uint32_t AHB1RSTR;     /* 0x80 - AHB1 reset */
    volatile uint32_t AHB2RSTR;     /* 0x84 - AHB2 reset */
    volatile uint32_t AHB4RSTR;     /* 0x88 - AHB4 reset */
    volatile uint32_t APB3RSTR;     /* 0x8C - APB3 reset */
    volatile uint32_t APB1LRSTR;    /* 0x90 - APB1L reset */
    volatile uint32_t APB1HRSTR;    /* 0x94 - APB1H reset */
    volatile uint32_t APB2RSTR;     /* 0x98 - APB2 reset */
    volatile uint32_t APB4RSTR;     /* 0x9C - APB4 reset */
    volatile uint32_t GCR;          /* 0xA0 - Global control */
    volatile uint32_t RESERVED7;    /* 0xA4 */
    volatile uint32_t D3AMR;        /* 0xA8 - Domain 3 autonomous mode */
    volatile uint32_t RESERVED8[9]; /* 0xAC - 0xCC */
    volatile uint32_t RSR;          /* 0xD0 - Reset status */
    volatile uint32_t AHB3ENR;      /* 0xD4 - AHB3 clock enable */
    volatile uint32_t AHB1ENR;      /* 0xD8 - AHB1 clock enable */
    volatile uint32_t AHB2ENR;      /* 0xDC - AHB2 clock enable */
    volatile uint32_t AHB4ENR;      /* 0xE0 - AHB4 clock enable ← GPIO! */
    volatile uint32_t APB3ENR;      /* 0xE4 - APB3 clock enable */
    volatile uint32_t APB1LENR;     /* 0xE8 - APB1L clock enable */
    volatile uint32_t APB1HENR;     /* 0xEC - APB1H clock enable */
    volatile uint32_t APB2ENR;      /* 0xF0 - APB2 clock enable */
    volatile uint32_t APB4ENR;      /* 0xF4 - APB4 clock enable */
} RCC_TypeDef;

/* GPIO Register Structure */
typedef struct {
    volatile uint32_t MODER;    /* 0x00 - Mode register */
    volatile uint32_t OTYPER;   /* 0x04 - Output type */
    volatile uint32_t OSPEEDR;  /* 0x08 - Output speed */
    volatile uint32_t PUPDR;    /* 0x0C - Pull-up/pull-down */
    volatile uint32_t IDR;      /* 0x10 - Input data */
    volatile uint32_t ODR;      /* 0x14 - Output data */
    volatile uint32_t BSRR;     /* 0x18 - Bit set/reset */
    volatile uint32_t LCKR;     /* 0x1C - Lock */
    volatile uint32_t AFR[2];   /* 0x20-0x24 - Alternate function */
} GPIO_TypeDef;

/* Create pointers to access peripherals */
#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOE   ((GPIO_TypeDef *) GPIOE_BASE)

/* ============================================================================
 * 
 *  LESSON 3: BIT DEFINITIONS
 *  ===========================
 * 
 *  Instead of remembering "bit 17 of RCC_CR is HSERDY", we define constants.
 *  
 *  PATTERN: (1U << bit_position)
 *  
 *  Example: Bit 17 → (1U << 17) = 0x00020000
 *  
 *  ┌────────────────────────────────────────────────────────────────────────┐
 *  │ Operation      │ Code                  │ What it does                 │
 *  ├────────────────┼───────────────────────┼──────────────────────────────┤
 *  │ SET a bit      │ REG |= BITMASK;       │ Turn bit ON (1)              │
 *  │ CLEAR a bit    │ REG &= ~BITMASK;      │ Turn bit OFF (0)             │
 *  │ TOGGLE a bit   │ REG ^= BITMASK;       │ Flip 0→1 or 1→0              │
 *  │ CHECK a bit    │ if (REG & BITMASK)    │ Test if bit is 1             │
 *  └────────────────┴───────────────────────┴──────────────────────────────┘
 * 
 * ============================================================================ */

/* RCC_CR Register Bits - Clock Control */
#define RCC_CR_HSION        (1U << 0)   /* HSI enable */
#define RCC_CR_HSIRDY       (1U << 2)   /* HSI ready */
#define RCC_CR_HSEON        (1U << 16)  /* HSE enable */
#define RCC_CR_HSERDY       (1U << 17)  /* HSE ready */
#define RCC_CR_HSEBYP       (1U << 18)  /* HSE bypass */
#define RCC_CR_PLL1ON       (1U << 24)  /* PLL1 enable */
#define RCC_CR_PLL1RDY      (1U << 25)  /* PLL1 ready */

/* RCC_CFGR Register Bits - Clock Configuration */
#define RCC_CFGR_SW_HSI     (0U << 0)   /* HSI as SYSCLK */
#define RCC_CFGR_SW_CSI     (1U << 0)   /* CSI as SYSCLK */
#define RCC_CFGR_SW_HSE     (2U << 0)   /* HSE as SYSCLK */
#define RCC_CFGR_SW_PLL1    (3U << 0)   /* PLL1 as SYSCLK */
#define RCC_CFGR_SW_MASK    (7U << 0)   /* SW bits mask */
#define RCC_CFGR_SWS_MASK   (7U << 3)   /* SWS bits mask */
#define RCC_CFGR_SWS_HSI    (0U << 3)   /* HSI is SYSCLK */
#define RCC_CFGR_SWS_HSE    (2U << 3)   /* HSE is SYSCLK */
#define RCC_CFGR_SWS_PLL1   (3U << 3)   /* PLL1 is SYSCLK */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: DEFINE GPIO CLOCK ENABLE BITS
 *  ===============================================
 * 
 *  GPIO clocks are in RCC->AHB4ENR register.
 *  Each GPIO port has a bit:
 *  
 *  ┌──────────┬──────────┐
 *  │ Port     │ Bit      │
 *  ├──────────┼──────────┤
 *  │ GPIOA    │ 0        │
 *  │ GPIOB    │ 1        │
 *  │ GPIOC    │ 2        │
 *  │ GPIOD    │ 3        │
 *  │ GPIOE    │ 4        │
 *  │ GPIOF    │ 5        │
 *  │ GPIOG    │ 6        │
 *  └──────────┴──────────┘
 * 
 * ============================================================================ */

/* RCC_AHB4ENR - GPIO Clock Enable Bits */
#define RCC_AHB4ENR_GPIOAEN (1U << 0)   /* GPIOA clock enable */

/* ✏️ YOUR TURN: Define GPIOB clock enable (bit 1) */
#define RCC_AHB4ENR_GPIOBEN ???         /* HINT: GPIOB is at bit position 1 in the table above */

/* ✏️ YOUR TURN: Define GPIOC clock enable (bit 2) */
#define RCC_AHB4ENR_GPIOCEN ???         /* HINT: Follow the pattern - which bit enables GPIOC? */

#define RCC_AHB4ENR_GPIODEN (1U << 3)   /* GPIOD clock enable */

/* ✏️ YOUR TURN: Define GPIOE clock enable (bit 4) - needed for Yellow LED! */
#define RCC_AHB4ENR_GPIOEEN ???         /* HINT: GPIOE is at bit 4 - use left shift to create the mask */

#define RCC_AHB4ENR_GPIOFEN (1U << 5)   /* GPIOF clock enable */
#define RCC_AHB4ENR_GPIOGEN (1U << 6)   /* GPIOG clock enable */

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * #define RCC_AHB4ENR_GPIOBEN (1U << 1)
 * #define RCC_AHB4ENR_GPIOCEN (1U << 2)
 * #define RCC_AHB4ENR_GPIOEEN (1U << 4)
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 4: THE CLOCK ENABLE PATTERN
 *  ====================================
 * 
 *  PATTERN FOR ENABLING ANY PERIPHERAL CLOCK:
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  Step 1: Find which bus the peripheral is on
 *          • GPIO → AHB4ENR
 *          • UART2/3, SPI2/3, I2C → APB1LENR
 *          • UART1, SPI1 → APB2ENR
 *          • Check reference manual!
 *  
 *  Step 2: Find which bit controls that peripheral
 *          • Usually named after the peripheral
 *          • GPIOBEN = GPIOB enable
 *  
 *  Step 3: Set the bit
 *          RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 *  
 *  Step 4: Dummy read (ensures write completes before continuing)
 *          (void)RCC->AHB4ENR;
 *  
 *  
 *  WHY THE DUMMY READ?
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  The write to RCC travels over a bus. The peripheral might be on a
 *  different clock domain. The dummy read forces synchronization:
 *  
 *       CPU ──write──► RCC ───────► Peripheral starts getting clock
 *           ◄──read───     (waits for bus)
 *                          ✓ Now safe to access peripheral!
 * 
 * ============================================================================ */

/* Simple delay function */
void delay(volatile uint32_t count) {
    while (count--);
}

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: ENABLE GPIO CLOCKS FOR ALL LEDs
 *  =================================================
 * 
 *  We need to enable clocks for:
 *  • GPIOB (for PB0 green and PB14 red)
 *  • GPIOE (for PE1 yellow)
 * 
 * ============================================================================ */

void RCC_EnableGPIOClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOB clock */
    RCC->AHB4ENR |= ???;        /* HINT: Use the macro you defined for GPIOB clock enable */
    
    /* ✏️ YOUR TURN: Enable GPIOE clock */
    RCC->AHB4ENR |= ???;        /* HINT: Use the macro you defined for GPIOE clock enable */
    
    /* Dummy read to ensure writes complete */
    (void)RCC->AHB4ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void RCC_EnableGPIOClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
 *     (void)RCC->AHB4ENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 5: CHECKING CLOCK STATUS
 *  =================================
 * 
 *  You can read the CFGR register to see which clock drives SYSCLK.
 *  
 *  ┌────────────────────────────────────────────────────────────────────────┐
 *  │ RCC->CFGR bits [5:3] = SWS (Switch Status)                            │
 *  ├───────────────┬────────────────────────────────────────────────────────┤
 *  │ Value         │ Meaning                                                │
 *  ├───────────────┼────────────────────────────────────────────────────────┤
 *  │ 000 (0)       │ HSI is system clock (DEFAULT AT RESET)                 │
 *  │ 001 (1)       │ CSI is system clock                                    │
 *  │ 010 (2)       │ HSE is system clock                                    │
 *  │ 011 (3)       │ PLL1 is system clock                                   │
 *  └───────────────┴────────────────────────────────────────────────────────┘
 * 
 * ============================================================================ */

/**
 * @brief Get current system clock source
 * @return 0=HSI, 1=CSI, 2=HSE, 3=PLL1
 */
uint32_t RCC_GetSysClkSource(void) {
    return (RCC->CFGR & RCC_CFGR_SWS_MASK) >> 3;
}

/* ============================================================================
 * 
 *  LESSON 6: ENABLING EXTERNAL OSCILLATOR (HSE)
 *  ==============================================
 * 
 *  HSI (internal) is convenient but HSE (external crystal) is more accurate.
 *  
 *  PATTERN: ENABLE → WAIT FOR READY → USE
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  Step 1: Set HSEON bit to start the oscillator
 *  Step 2: Wait for HSERDY bit (oscillator is stable)
 *  Step 3: Only THEN can you switch to HSE
 *  
 *  ┌────────────────────────────────────────────────────────────────────────┐
 *  │ Time ──►                                                               │
 *  │                                                                        │
 *  │ HSEON ──────┐                                                          │
 *  │             │ SET                                                      │
 *  │             └──────────────────────────────────────────────            │
 *  │                                                                        │
 *  │ Crystal     ════════════════════════════════════════════════           │
 *  │ (starting)      oscillating... stabilizing... stable!                  │
 *  │                                                                        │
 *  │ HSERDY ─────────────────────────────────────┐                          │
 *  │                                             │ READY!                   │
 *  │                                             └──────────────────        │
 *  │                                             ▲                          │
 *  │                                     NOW safe to switch!                │
 *  └────────────────────────────────────────────────────────────────────────┘
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: ENABLE HSE OSCILLATOR
 *  ======================================
 * 
 * ============================================================================ */

uint32_t RCC_EnableHSE(void) {
    uint32_t timeout = 100000;
    
    /* ✏️ YOUR TURN: Enable HSE by setting HSEON bit */
    RCC->CR |= ???;             /* HINT: Which bit in CR turns ON the external oscillator? Look for a defined constant... */
    
    /* ✏️ YOUR TURN: Wait for HSERDY flag */
    while (!(RCC->CR & ???)) {  /* HINT: Which bit indicates the HSE is Ready and stable? */
        if (--timeout == 0) {
            return 1;           /* Timeout - HSE failed */
        }
    }
    
    return 0;                   /* Success */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint32_t RCC_EnableHSE(void) {
 *     uint32_t timeout = 100000;
 *     RCC->CR |= RCC_CR_HSEON;
 *     while (!(RCC->CR & RCC_CR_HSERDY)) {
 *         if (--timeout == 0) return 1;
 *     }
 *     return 0;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: SWITCH SYSTEM CLOCK TO HSE
 *  ============================================
 * 
 *  After HSE is ready, we can switch the system clock.
 *  
 *  STEPS:
 *  1. Clear SW bits (bits 2:0)
 *  2. Set SW to HSE (value = 2)
 *  3. Wait for SWS to confirm HSE is active
 * 
 * ============================================================================ */

void RCC_SwitchToHSE(void) {
    uint32_t cfgr;
    
    /* Read current CFGR */
    cfgr = RCC->CFGR;
    
    /* ✏️ YOUR TURN: Clear SW bits */
    cfgr &= ???;                /* HINT: AND with inverted mask to clear the Switch bits. Use the _MASK constant. */
    
    /* ✏️ YOUR TURN: Set SW to HSE */
    cfgr |= ???;                /* HINT: OR in the value that selects HSE as source. What constant represents SW=HSE? */
    
    /* Write back */
    RCC->CFGR = cfgr;
    
    /* ✏️ YOUR TURN: Wait until SWS shows HSE is active */
    while ((RCC->CFGR & RCC_CFGR_SWS_MASK) != ???);  /* HINT: Compare with the SWS status value for HSE */
}
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void RCC_SwitchToHSE(void) {
 *     uint32_t cfgr;
 *     cfgr = RCC->CFGR;
 *     cfgr &= ~RCC_CFGR_SW_MASK;
 *     cfgr |= RCC_CFGR_SW_HSE;
 *     RCC->CFGR = cfgr;
 *     while ((RCC->CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_HSE);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: CONFIGURE GPIO PINS AS OUTPUTS
 *  =================================================
 * 
 *  GPIO MODER register: 2 bits per pin
 *  
 *  ┌───────────┬─────────────────────────────┐
 *  │ Value     │ Mode                        │
 *  ├───────────┼─────────────────────────────┤
 *  │ 00        │ Input                       │
 *  │ 01        │ Output ← WE WANT THIS       │
 *  │ 10        │ Alternate function          │
 *  │ 11        │ Analog                      │
 *  └───────────┴─────────────────────────────┘
 *  
 *  Formula: Position = pin × 2
 *  
 *  PB0:  bits [1:0]   → position = 0 × 2 = 0
 *  PB14: bits [29:28] → position = 14 × 2 = 28
 *  PE1:  bits [3:2]   → position = 1 × 2 = 2
 * 
 * ============================================================================ */

void GPIO_ConfigureLEDs(void) {
    /* ═══════════════════════════════════════════════════════════════════════
     * GREEN LED - PB0
     * ═══════════════════════════════════════════════════════════════════════ */
    
    /* ✏️ YOUR TURN: Clear mode bits for PB0 (bits 1:0) */
    GPIOB->MODER &= ???;        /* HINT: ~(3U << (0 * 2)) */
    
    /* ✏️ YOUR TURN: Set PB0 to output mode (01) */
    GPIOB->MODER |= ???;        /* HINT: (1U << (0 * 2)) */
    
    /* ═══════════════════════════════════════════════════════════════════════
     * YELLOW LED - PE1
     * ═══════════════════════════════════════════════════════════════════════ */
    
    /* ✏️ YOUR TURN: Clear mode bits for PE1 (bits 3:2) */
    GPIOE->MODER &= ???;        /* HINT: ~(3U << (1 * 2)) */
    
    /* ✏️ YOUR TURN: Set PE1 to output mode (01) */
    GPIOE->MODER |= ???;        /* HINT: (1U << (1 * 2)) */
    
    /* ═══════════════════════════════════════════════════════════════════════
     * RED LED - PB14
     * ═══════════════════════════════════════════════════════════════════════ */
    
    /* ✏️ YOUR TURN: Clear mode bits for PB14 (bits 29:28) */
    GPIOB->MODER &= ???;        /* HINT: ~(3U << (14 * 2)) */
    
    /* ✏️ YOUR TURN: Set PB14 to output mode (01) */
    GPIOB->MODER |= ???;        /* HINT: (1U << (14 * 2)) */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void GPIO_ConfigureLEDs(void) {
 *     // Green LED - PB0
 *     GPIOB->MODER &= ~(3U << (0 * 2));
 *     GPIOB->MODER |= (1U << (0 * 2));
 *     
 *     // Yellow LED - PE1
 *     GPIOE->MODER &= ~(3U << (1 * 2));
 *     GPIOE->MODER |= (1U << (1 * 2));
 *     
 *     // Red LED - PB14
 *     GPIOB->MODER &= ~(3U << (14 * 2));
 *     GPIOB->MODER |= (1U << (14 * 2));
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: LED CONTROL FUNCTIONS
 *  =======================================
 * 
 *  Using BSRR for atomic set/reset (better than ODR for individual bits).
 *  
 *  BSRR register:
 *  • Bits 0-15:  SET bits (write 1 to set pin HIGH)
 *  • Bits 16-31: RESET bits (write 1 to set pin LOW)
 * 
 * ============================================================================ */

/* LED Pin Definitions */
#define LED_GREEN_PIN   0       /* PB0 */
#define LED_YELLOW_PIN  1       /* PE1 */
#define LED_RED_PIN     14      /* PB14 */

void LED_GreenOn(void) {
    /* ✏️ YOUR TURN: Set PB0 HIGH using BSRR */
    GPIOB->BSRR = ???;          /* HINT: BSRR bits 0-15 SET pins. Which bit is the green LED? */
}

void LED_GreenOff(void) {
    /* ✏️ YOUR TURN: Set PB0 LOW using BSRR reset bits (upper 16 bits) */
    GPIOB->BSRR = ???;          /* HINT: BSRR bits 16-31 RESET pins. Add 16 to the pin number. */
}

void LED_YellowOn(void) {
    GPIOE->BSRR = (1U << LED_YELLOW_PIN);
}

void LED_YellowOff(void) {
    GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16));
}

void LED_RedOn(void) {
    /* ✏️ YOUR TURN: Set PB14 HIGH */
    GPIOB->BSRR = ???;          /* HINT: Use the LED_RED_PIN constant to set the correct bit */
}

void LED_RedOff(void) {
    /* ✏️ YOUR TURN: Set PB14 LOW */
    GPIOB->BSRR = ???;          /* HINT: Reset bits are in upper 16 bits. Pin + 16. */
}

/* Toggle uses XOR on ODR */
void LED_GreenToggle(void) {
    GPIOB->ODR ^= (1U << LED_GREEN_PIN);
}

void LED_YellowToggle(void) {
    GPIOE->ODR ^= (1U << LED_YELLOW_PIN);
}

void LED_RedToggle(void) {
    GPIOB->ODR ^= (1U << LED_RED_PIN);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void LED_GreenOn(void)  { GPIOB->BSRR = (1U << LED_GREEN_PIN); }
 * void LED_GreenOff(void) { GPIOB->BSRR = (1U << (LED_GREEN_PIN + 16)); }
 * void LED_RedOn(void)    { GPIOB->BSRR = (1U << LED_RED_PIN); }
 * void LED_RedOff(void)   { GPIOB->BSRR = (1U << (LED_RED_PIN + 16)); }
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
 *  MAIN PROGRAM - Traffic Light Demo with All 3 LEDs
 * 
 * ============================================================================ */

int main(void)
{
    /* ========================================================================
     * AT RESET: MCU runs on HSI at 64 MHz
     * 
     * You can start using peripherals immediately after enabling their clocks.
     * No other clock configuration is required for basic operation!
     * ======================================================================== */
    
    /* Step 1: Enable GPIO clocks (THE GOLDEN RULE!) */
    RCC_EnableGPIOClocks();
    
    /* Step 2: Configure LED pins as outputs */
    GPIO_ConfigureLEDs();
    
    /* ========================================================================
     * OPTIONAL: Switch to HSE (8 MHz external crystal)
     * 
     * Uncomment to try HSE. The LEDs will blink SLOWER because
     * we drop from 64 MHz to 8 MHz!
     * ======================================================================== */
    
    /*
    if (RCC_EnableHSE() == 0) {
        RCC_SwitchToHSE();
        // Now running at 8 MHz (slower!)
    }
    */
    
    /* ========================================================================
     * TRAFFIC LIGHT DEMO
     * 
     * Pattern: Green → Yellow → Red → (repeat)
     * This demonstrates that all 3 LEDs are working!
     * ======================================================================== */
    
    for(;;) {
        /* GREEN phase */
        LED_GreenOn();
        LED_YellowOff();
        LED_RedOff();
        delay(2000000);
        
        /* YELLOW phase */
        LED_GreenOff();
        LED_YellowOn();
        LED_RedOff();
        delay(1000000);
        
        /* RED phase */
        LED_GreenOff();
        LED_YellowOff();
        LED_RedOn();
        delay(2000000);
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned RCC fundamentals without HAL:
 *  
 *  ✅ THE GOLDEN RULE: Always enable peripheral clocks first!
 *  ✅ Memory-mapped peripheral access
 *  ✅ Register structures and bit definitions
 *  ✅ Enabling GPIO clocks (AHB4ENR)
 *  ✅ Checking which clock source is active
 *  ✅ Enabling HSE (external oscillator)
 *  ✅ Switching system clock sources
 *  ✅ Configuring GPIO pins as outputs
 *  ✅ Controlling LEDs with BSRR
 *  
 *  CLOCK SOURCES SUMMARY:
 *  ─────────────────────────────────────────────────────────────────────────
 *  ┌──────────┬────────────┬─────────────────────────────────────────────┐
 *  │ Source   │ Frequency  │ Notes                                       │
 *  ├──────────┼────────────┼─────────────────────────────────────────────┤
 *  │ HSI      │ 64 MHz     │ Internal, default at reset, less accurate   │
 *  │ HSE      │ 8 MHz      │ External crystal, more accurate             │
 *  │ PLL1     │ up to 480  │ Maximum performance (needs configuration)   │
 *  │ LSI      │ ~32 kHz    │ Internal, for watchdog/RTC                  │
 *  │ LSE      │ 32.768 kHz │ External, precise for RTC                   │
 *  └──────────┴────────────┴─────────────────────────────────────────────┘
 *  
 *  NEXT STEPS:
 *  • Configure PLL for 480 MHz (maximum speed!)
 *  • Configure bus prescalers (D1CFGR, D2CFGR, D3CFGR)
 *  • Enable clocks for other peripherals (UART, SPI, I2C)
 *  
 *  REFERENCE: RM0433 Reference Manual, Chapter 8 (RCC)
 * 
 * ============================================================================ */
