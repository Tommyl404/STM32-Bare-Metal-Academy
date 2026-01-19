/**
 ******************************************************************************
 * @file           : watchdog_tutorial.c
 * @brief          : Learning IWDG/WWDG (Watchdog) without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ██╗    ██╗ █████╗ ████████╗ ██████╗██╗  ██╗██████╗  ██████╗  ██████╗ 
 *  ██║    ██║██╔══██╗╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔═══██╗██╔════╝ 
 *  ██║ █╗ ██║███████║   ██║   ██║     ███████║██║  ██║██║   ██║██║  ███╗
 *  ██║███╗██║██╔══██║   ██║   ██║     ██╔══██║██║  ██║██║   ██║██║   ██║
 *  ╚███╔███╔╝██║  ██║   ██║   ╚██████╗██║  ██║██████╔╝╚██████╔╝╚██████╔╝
 *   ╚══╝╚══╝ ╚═╝  ╚═╝   ╚═╝    ╚═════╝╚═╝  ╚═╝╚═════╝  ╚═════╝  ╚═════╝ 
 * 
 *  INTERACTIVE LEARNING: WATCHDOG TIMERS
 * 
 *  WHAT YOU'LL LEARN:
 *  1. Why watchdogs are critical for reliability
 *  2. IWDG (Independent Watchdog) - simple timeout
 *  3. WWDG (Window Watchdog) - window-based monitoring
 *  4. How to kick (feed) the dog
 * 
 *  CONCEPT:
 *  If your code freezes or crashes, the watchdog
 *  will automatically RESET the MCU!
 * 
 *  DIFFICULTY: ⭐⭐ (Easy)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS A WATCHDOG?
 *  ==============================
 * 
 *  A watchdog is a safety timer that RESETS the MCU if not "fed" in time.
 *  
 *  Normal Operation (Code Working):
 *  ─────────────────────────────────────────────────────────────────
 *  
 *      Your Code                    Watchdog Timer
 *         │                              │
 *         ├──► Feed the dog! ──────────► Counter resets
 *         │    (Code running fine)       │
 *         │                              ▼
 *         │                         Counts down...
 *         │                              │
 *         ├──► Feed the dog! ──────────► Counter resets
 *         │                              │
 *         ▼                              ▼
 *     (continues)                   (continues)
 *  
 *  
 *  Fault Condition (Code Frozen):
 *  ─────────────────────────────────────────────────────────────────
 *  
 *      Your Code                    Watchdog Timer
 *         │                              │
 *         ├──► Feed the dog! ──────────► Counter resets
 *         │                              │
 *         X                              ▼
 *     (FROZEN!)                     Counts down...
 *                                        │
 *                                        ▼
 *                                   Counts down...
 *                                        │
 *                                        ▼
 *                                   Counter = 0
 *                                        │
 *                                        ▼
 *                                   ⚠️ SYSTEM RESET! ⚠️
 * 
 * 
 *  STM32H7 has TWO watchdogs:
 *  
 *  IWDG (Independent Watchdog):
 *  • Uses separate 32 kHz LSI clock
 *  • Simple: just don't let it timeout
 *  • Works even if main clock fails
 *  
 *  WWDG (Window Watchdog):
 *  • Uses APB clock
 *  • Must refresh WITHIN a time window
 *  • Too early OR too late = reset!
 *  • Better for detecting stuck loops
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOB_BASE      0x58020400UL
#define IWDG1_BASE      0x58004800UL
#define WWDG1_BASE      0x50003000UL

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
 *  IWDG REGISTERS (Independent Watchdog)
 * ============================================================================ */
typedef struct {
    volatile uint32_t KR;       /* 0x00 - Key register */
    volatile uint32_t PR;       /* 0x04 - Prescaler register */
    volatile uint32_t RLR;      /* 0x08 - Reload register */
    volatile uint32_t SR;       /* 0x0C - Status register */
    volatile uint32_t WINR;     /* 0x10 - Window register */
} IWDG_TypeDef;

#define IWDG1   ((IWDG_TypeDef *) IWDG1_BASE)

/* ============================================================================
 *  WWDG REGISTERS (Window Watchdog)
 * ============================================================================ */
typedef struct {
    volatile uint32_t CR;       /* 0x00 - Control register */
    volatile uint32_t CFR;      /* 0x04 - Configuration register */
    volatile uint32_t SR;       /* 0x08 - Status register */
} WWDG_TypeDef;

#define WWDG1   ((WWDG_TypeDef *) WWDG1_BASE)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC */
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_APB3ENR_WWDG1EN     (1U << 6)

/* IWDG Key Register Values */
#define IWDG_KEY_ENABLE         0xCCCC      /* Start the watchdog */
#define IWDG_KEY_WRITE_ACCESS   0x5555      /* Enable register write */
#define IWDG_KEY_RELOAD         0xAAAA      /* Reload the counter (feed the dog) */

/* IWDG Prescaler Values */
#define IWDG_PR_DIV4            0           /* /4 prescaler */
#define IWDG_PR_DIV8            1           /* /8 prescaler */
#define IWDG_PR_DIV16           2           /* /16 prescaler */
#define IWDG_PR_DIV32           3           /* /32 prescaler */
#define IWDG_PR_DIV64           4           /* /64 prescaler */
#define IWDG_PR_DIV128          5           /* /128 prescaler */
#define IWDG_PR_DIV256          6           /* /256 prescaler */

/* IWDG Status Register */
#define IWDG_SR_PVU             (1U << 0)   /* Prescaler value update */
#define IWDG_SR_RVU             (1U << 1)   /* Reload value update */

/* WWDG Control Register */
#define WWDG_CR_WDGA            (1U << 7)   /* Activation bit */
#define WWDG_CR_T_MASK          0x7F        /* 7-bit counter mask */

/* WWDG Configuration Register */
#define WWDG_CFR_EWI            (1U << 9)   /* Early wakeup interrupt */
#define WWDG_CFR_WDGTB_DIV1     (0U << 11)  /* No prescaler */
#define WWDG_CFR_WDGTB_DIV2     (1U << 11)  /* /2 */
#define WWDG_CFR_WDGTB_DIV4     (2U << 11)  /* /4 */
#define WWDG_CFR_WDGTB_DIV8     (3U << 11)  /* /8 */

/* ============================================================================
 * 
 *  PART 1: IWDG (Independent Watchdog)
 *  ====================================
 * 
 *  The IWDG is simpler - just reload before timeout!
 * 
 *  (Reload Register (RLR) = sets the timeout period)
 *  Timeout = (RLR × Prescaler) / LSI_Frequency
 *  
 *  Example: LSI = 32 kHz, Prescaler = 64, RLR = 500
 *  Timeout = (500 × 64) / 32000 = 1 second
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: START THE IWDG
 *  ===============================
 *  Remember this:
 *  CPU:  Write PR → Write RLR → Check SR → Check SR → ... → SR clear → Enable
           ▼           ▼           │                           ▲
           │           │           │                           │
    LSI:   ──────────Sync───────Sync─────────────────────Done──
                                  └──────── ~100 μs ──────────┘
 * 
 * ============================================================================ */

void IWDG_Init(uint16_t reload_value, uint8_t prescaler) {
    /* ✏️ YOUR TURN: Enable write access to IWDG registers */
    IWDG1->KR = ???;                              /* HINT: Which key enables register writes? */
    
    /* Set prescaler */
    IWDG1->PR = prescaler;
    
    /* Set reload value */
    IWDG1->RLR = reload_value;
    
    /* Wait for registers to update */
    while (IWDG1->SR & (IWDG_SR_PVU | IWDG_SR_RVU));
    
    /* ✏️ YOUR TURN: Start the watchdog */
    IWDG1->KR = ???;                              /* HINT: Which key starts the watchdog? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void IWDG_Init(uint16_t reload_value, uint8_t prescaler) {
 *     IWDG1->KR = IWDG_KEY_WRITE_ACCESS;
 *     IWDG1->PR = prescaler;
 *     IWDG1->RLR = reload_value;
 *     while (IWDG1->SR & (IWDG_SR_PVU | IWDG_SR_RVU));
 *     IWDG1->KR = IWDG_KEY_ENABLE;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: FEED THE IWDG
 *  ==============================
 * 
 *  You MUST call this function periodically, or the MCU resets!
 * 
 * ============================================================================ */

void IWDG_Feed(void) {
    /* ✏️ YOUR TURN: Write reload key to reset the counter */
    IWDG1->KR = ???;                              /* HINT: Which key reloads the counter? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void IWDG_Feed(void) {
 *     IWDG1->KR = IWDG_KEY_RELOAD;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  PART 2: WWDG (Window Watchdog)
 *  ==============================
 * 
 *  WWDG is more strict - you must refresh WITHIN a window!
 *  
 *       ◄──── Too Early ────►◄── OK Window ──►◄─ Too Late ─►
 *       
 *       ┌─────────────────────┬───────────────┬────────────┐
 *       │      RESET!         │     SAFE      │   RESET!   │
 *       └─────────────────────┴───────────────┴────────────┘
 *                             ▲               ▲
 *                           Window          Counter
 *                           Value           = 0x3F
 * 
 *  Counter counts DOWN from 0x7F to 0x3F, then RESET.
 *  Window defines when it's safe to refresh.
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: ENABLE WWDG CLOCK
 *  ==================================
 * 
 * ============================================================================ */

void WWDG_EnableClock(void) {
    /* ✏️ YOUR TURN: Enable WWDG1 clock in APB3ENR */
    RCC->APB3ENR |= ???;                          /* HINT: Which bit enables WWDG1? */
    
    (void)RCC->APB3ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void WWDG_EnableClock(void) {
 *     RCC->APB3ENR |= RCC_APB3ENR_WWDG1EN;
 *     (void)RCC->APB3ENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: INITIALIZE WWDG
 *  ================================
 * 
 * ============================================================================ */

void WWDG_Init(uint8_t window, uint8_t counter, uint32_t prescaler) {
    /* Enable clock first */
    WWDG_EnableClock();
    
    /* Set prescaler and window value */
    WWDG1->CFR = prescaler | (window & WWDG_CR_T_MASK);
    
    /* ✏️ YOUR TURN: Set counter and enable watchdog */
    WWDG1->CR = ???;                              /* HINT: Combine activation bit with masked counter */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void WWDG_Init(uint8_t window, uint8_t counter, uint32_t prescaler) {
 *     WWDG_EnableClock();
 *     WWDG1->CFR = prescaler | (window & WWDG_CR_T_MASK);
 *     WWDG1->CR = WWDG_CR_WDGA | (counter & WWDG_CR_T_MASK);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: REFRESH WWDG
 *  =============================
 * 
 *  Must be called within the window, or RESET!
 * 
 * ============================================================================ */

void WWDG_Refresh(uint8_t counter) {
    /* ✏️ YOUR TURN: Write new counter value (keep WDGA bit set) */
    WWDG1->CR = ???;                              /* HINT: Same as init - activation + counter */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void WWDG_Refresh(uint8_t counter) {
 *     WWDG1->CR = WWDG_CR_WDGA | (counter & WWDG_CR_T_MASK);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 *  LED HELPER FUNCTIONS
 * ============================================================================ */

void LED_Init(void) {
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    (void)RCC->AHB4ENR;
    
    /* PB0 as output */
    GPIOB->MODER &= ~(3U << (0 * 2));
    GPIOB->MODER |= (1U << (0 * 2));
}

void LED_On(void) {
    GPIOB->ODR |= (1U << 0);
}

void LED_Off(void) {
    GPIOB->ODR &= ~(1U << 0);
}

void LED_Toggle(void) {
    GPIOB->ODR ^= (1U << 0);
}

void delay(volatile uint32_t count) {
    while (count--);
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
 *  MAIN PROGRAM - Test IWDG (blink LED while feeding watchdog)
 * 
 * ============================================================================ */

int main(void)
{
    LED_Init();
    
    /* Flash LED quickly 3 times at startup to show reset occurred */
    for (int i = 0; i < 6; i++) {
        LED_Toggle();
        delay(500000);
    }
    
    /* Initialize IWDG with ~1 second timeout
     * LSI ≈ 32 kHz
     * Prescaler = /64 → 500 Hz
     * Reload = 500 → 1 second timeout
     */
    IWDG_Init(500, IWDG_PR_DIV64);
    
    for(;;) {
        LED_Toggle();
        delay(2000000);
        
        /* ⚠️ IMPORTANT: Feed the watchdog before timeout! */
        IWDG_Feed();
        
        /* 💡 Try commenting out IWDG_Feed() above.
         *    The MCU will reset every ~1 second!
         *    You'll see the 3-blink startup pattern repeat.
         */
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned WATCHDOGS without HAL:
 *  
 *  ✅ Why watchdogs are essential for reliability
 *  ✅ IWDG initialization with prescaler and reload
 *  ✅ IWDG feeding (reload key)
 *  ✅ WWDG window concept
 *  ✅ WWDG initialization and refresh
 *  
 *  WHEN TO USE EACH:
 *  • IWDG: Simple protection, independent clock (works if main clock fails)
 *  • WWDG: Precise timing, detect code running too fast or too slow
 *  
 *  BEST PRACTICES:
 *  • Put watchdog feed in main loop
 *  • Don't feed in interrupts (code could be stuck in main loop)
 *  • Set timeout longer than worst-case loop time
 *  • Consider using both IWDG and WWDG for maximum safety
 * 
 * ============================================================================ */
