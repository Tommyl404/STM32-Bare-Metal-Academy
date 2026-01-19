/**
 ******************************************************************************
 * @file           : rtc_tutorial.c
 * @brief          : Learning RTC (Real-Time Clock) without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ██████╗ ████████╗ ██████╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔══██╗╚══██╔══╝██╔════╝    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ██████╔╝   ██║   ██║            ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██╔══██╗   ██║   ██║            ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ██║  ██║   ██║   ╚██████╗       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚═╝  ╚═╝   ╚═╝    ╚═════╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: REAL-TIME CLOCK
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What RTC does (keeps time even when MCU sleeps)
 *  2. How to configure RTC clock source
 *  3. How to set and read time/date
 *  4. How to set alarms
 * 
 *  SPECIAL:
 *  - RTC runs on backup domain power (VBAT)
 *  - Keeps time during reset/sleep
 *  - Uses 32.768 kHz LSE crystal (accurate) or LSI (less accurate)
 * 
 *  DIFFICULTY: ⭐⭐⭐ (Medium)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS RTC?
 *  =======================
 * 
 *  RTC = Real-Time Clock
 *  It keeps track of:
 *  • Time (hours, minutes, seconds)
 *  • Date (year, month, day, weekday)
 *  • Alarms (wake up at specific time)
 *  
 *  Special Features:
 *  ─────────────────────────────────────────────────────
 *  
 *       Main Power                 VBAT (Battery)
 *           │                           │
 *           ▼                           ▼
 *    ┌─────────────┐             ┌─────────────┐
 *    │   MCU Core  │             │   Backup    │
 *    │   (Sleeps)  │────────────►│   Domain    │
 *    └─────────────┘             │   (RTC!)    │
 *                                └─────────────┘
 *                                      │
 *                                 Keeps running!
 *  
 *  Clock Sources:
 *  • LSE = 32.768 kHz external crystal (accurate!)
 *  • LSI = ~32 kHz internal (less accurate)
 *  • HSE/32 = from main crystal
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define PWR_BASE        0x58024800UL
#define RTC_BASE        0x58004000UL

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
 *  PWR REGISTERS (Power Control)
 * ============================================================================ */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CSR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t CPUCR;
    volatile uint32_t RESERVED1;
    volatile uint32_t D3CR;
    volatile uint32_t RESERVED2;
    volatile uint32_t WKUPCR;
    volatile uint32_t WKUPFR;
    volatile uint32_t WKUPEPR;
} PWR_TypeDef;

#define PWR     ((PWR_TypeDef *) PWR_BASE)

/* ============================================================================
 *  RTC REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t TR;       /* 0x00 - Time register */
    volatile uint32_t DR;       /* 0x04 - Date register */
    volatile uint32_t SSR;      /* 0x08 - Sub second register */
    volatile uint32_t ICSR;     /* 0x0C - Initialization and status register */
    volatile uint32_t PRER;     /* 0x10 - Prescaler register */
    volatile uint32_t WUTR;     /* 0x14 - Wakeup timer register */
    volatile uint32_t CR;       /* 0x18 - Control register */
    volatile uint32_t RESERVED1[2];
    volatile uint32_t WPR;      /* 0x24 - Write protection register */
    volatile uint32_t CALR;     /* 0x28 - Calibration register */
    volatile uint32_t SHIFTR;   /* 0x2C - Shift control register */
    volatile uint32_t TSTR;     /* 0x30 - Time stamp time register */
    volatile uint32_t TSDR;     /* 0x34 - Time stamp date register */
    volatile uint32_t TSSSR;    /* 0x38 - Time stamp sub second register */
    volatile uint32_t RESERVED2;
    volatile uint32_t ALRMAR;   /* 0x40 - Alarm A register */
    volatile uint32_t ALRMASSR; /* 0x44 - Alarm A sub second register */
    volatile uint32_t ALRMBR;   /* 0x48 - Alarm B register */
    volatile uint32_t ALRMBSSR; /* 0x4C - Alarm B sub second register */
    volatile uint32_t SR;       /* 0x50 - Status register */
    volatile uint32_t MISR;     /* 0x54 - Masked interrupt status register */
    volatile uint32_t RESERVED3;
    volatile uint32_t SCR;      /* 0x5C - Status clear register */
} RTC_TypeDef;

#define RTC     ((RTC_TypeDef *) RTC_BASE)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC BDCR (Backup Domain Control Register) */
#define RCC_BDCR_LSEON          (1U << 0)   /* LSE oscillator enable */
#define RCC_BDCR_LSERDY         (1U << 1)   /* LSE oscillator ready */
#define RCC_BDCR_LSEBYP         (1U << 2)   /* LSE bypass */
#define RCC_BDCR_RTCSEL_LSE     (1U << 8)   /* LSE as RTC clock */
#define RCC_BDCR_RTCSEL_LSI     (2U << 8)   /* LSI as RTC clock */
#define RCC_BDCR_RTCEN          (1U << 15)  /* RTC clock enable */
#define RCC_BDCR_BDRST          (1U << 16)  /* Backup domain reset */

/* RCC APB4ENR */
#define RCC_APB4ENR_RTCAPBEN    (1U << 16)

/* PWR CR1 */
#define PWR_CR1_DBP             (1U << 8)   /* Disable backup protection */

/* RTC Write Protection Keys */
#define RTC_WPR_KEY1            0xCA
#define RTC_WPR_KEY2            0x53

/* RTC ICSR (Init and Status) */
#define RTC_ICSR_INIT           (1U << 7)   /* Initialization mode */
#define RTC_ICSR_INITF          (1U << 6)   /* Initialization flag */
#define RTC_ICSR_RSF            (1U << 5)   /* Register sync flag */

/* RTC TR (Time Register) Bit Positions */
#define RTC_TR_SU_POS           0           /* Seconds units */
#define RTC_TR_ST_POS           4           /* Seconds tens */
#define RTC_TR_MNU_POS          8           /* Minutes units */
#define RTC_TR_MNT_POS          12          /* Minutes tens */
#define RTC_TR_HU_POS           16          /* Hours units */
#define RTC_TR_HT_POS           20          /* Hours tens */

/* RTC DR (Date Register) Bit Positions */
#define RTC_DR_DU_POS           0           /* Day units */
#define RTC_DR_DT_POS           4           /* Day tens */
#define RTC_DR_MU_POS           8           /* Month units */
#define RTC_DR_MT_POS           12          /* Month tens */
#define RTC_DR_WDU_POS          13          /* Week day units */
#define RTC_DR_YU_POS           16          /* Year units */
#define RTC_DR_YT_POS           20          /* Year tens */

/* ============================================================================
 * 
 *  LESSON 2: WHY THE WRITE PROTECTION KEYS?
 *  ==========================================
 * 
 *  The RTC is CRITICAL - it keeps time during resets, sleep, and power loss.
 *  Write protection prevents accidental corruption!
 * 
 *  🔐 THE PROBLEM:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  Without protection, a software bug could corrupt time:
 *  
 *    Before:  12:30:45 PM  ✓
 *    After:   87:99:FF ??  ✗  ← Random write destroyed time!
 *  
 *  Protection prevents:
 *  • Accidental writes from runaway code
 *  • Pointer errors (writing to wrong address)
 *  • Buffer overflows corrupting RTC
 *  • EMI/electrical noise causing spurious writes
 * 
 * 
 *  🔑 THE TWO-KEY SEQUENCE:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  RTC->WPR = 0xCA;   ← First key
 *  RTC->WPR = 0x53;   ← Second key (MUST be in this order!)
 *  
 *  Why TWO keys instead of one?
 *  
 *    Single Key:  Random noise writes 0xCA
 *                 Probability = 1/256 = 0.4%
 *  
 *    Two Keys:    Must write 0xCA, THEN 0x53 in sequence
 *                 Probability = 1/65,536 = 0.0015%
 *                 
 *                 Result: 430× SAFER! 🛡️
 * 
 * 
 *  🎯 WHY THESE SPECIFIC VALUES?
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  ┌──────┬────────┬────────────┬──────────────────────────────────────────┐
 *  │ Key  │  Hex   │   Binary   │ Reason                                   │
 *  ├──────┼────────┼────────────┼──────────────────────────────────────────┤
 *  │ KEY1 │  0xCA  │ 11001010   │ Distinct pattern (not 0x00, 0xFF, 0xAA)  │
 *  │ KEY2 │  0x53  │ 01010011   │ Same to KEY1                             │
 *  └──────┴────────┴────────────┴──────────────────────────────────────────┘
 *  
 *  These values are NOT common patterns like:
 *  • 0x00 (all zeros - default/uninitialized)
 *  • 0xFF (all ones - erased flash)
 *  • 0xAA, 0x55 (alternating bits - test patterns)
 *  
 *  They have balanced bit patterns (mix of 0s and 1s) making them resistant
 *  to electrical noise.
 * 
 * 
 *  🔒 HOW THE HARDWARE STATE MACHINE WORKS:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *            Write to WPR
 *                 │
 *                 ▼
 *         ┌─────────────┐
 *         │   LOCKED    │◄────────────────┐
 *         └──────┬──────┘                 │
 *                │                        │
 *          Write 0xCA?                    │
 *          Yes │   │ No                   │
 *              │   └──────────────────────┘
 *              ▼
 *         ┌─────────────┐
 *         │ FIRST_KEY   │
 *         └──────┬──────┘
 *                │
 *          Write 0x53?
 *          Yes │   │ No
 *              │   └──────────────────────┐
 *              ▼                          │
 *         ┌─────────────┐                 │
 *         │  UNLOCKED   │                 │
 *         │ (writable!) │                 │
 *         └──────┬──────┘                 │
 *                │                        │
 *         Write anything                  │
 *         else to WPR                     │
 *                │                        │
 *                └────────────────────────┘
 *  
 *  IMPORTANT: Writing ANY other value re-locks protection!
 *  
 *    RTC->WPR = 0xFF;  ← Any wrong value → LOCKED again
 * 
 * 
 *  🛡️ RTC HAS THREE PROTECTION LAYERS:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *    Layer 1:  PWR->CR1 |= PWR_CR1_DBP
 *              ↓
 *              Backup domain access enabled
 *              
 *    Layer 2:  RTC->WPR = 0xCA
 *              RTC->WPR = 0x53
 *              ↓
 *              Write protection disabled
 *              
 *    Layer 3:  RTC->ICSR |= RTC_ICSR_INIT
 *              Wait for INITF flag
 *              ↓
 *              Can modify TR/DR registers!
 *  
 *  This "defense in depth" ensures maximum safety for critical time data.
 * 
 * 
 *  💡 SIMILAR MECHANISMS IN OTHER STM32 PERIPHERALS:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  Other critical peripherals use similar multi-key protection:
 *  
 *  ┌─────────────┬────────────────────┬──────────────────────────────────┐
 *  │ Peripheral  │ Register           │ Keys / Sequence                  │
 *  ├─────────────┼────────────────────┼──────────────────────────────────┤
 *  │ IWDG        │ KR                 │ 0xCCCC (start), 0xAAAA (reload)  │
 *  │ Flash       │ OPTKEYR            │ 0x08192A3B, then 0x4C5D6E7F      │
 *  │ RTC         │ WPR                │ 0xCA, then 0x53                  │
 *  │ Backup SRAM │ PWR_CR1            │ DBP bit (similar to RTC)         │
 *  └─────────────┴────────────────────┴──────────────────────────────────┘
 *  
 *  All follow the same principle:
 *  
 *    Multiple specific values in sequence = Intentional action
 *  
 *  It's like requiring a two-key launch sequence to prevent accidental
 *  missile launches! 🚀🔐
 * 
 * 
 *  📝 SUMMARY:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  The keys (0xCA, 0x53) aren't "magic" - they're a carefully designed 
 *  safety mechanism:
 *  
 *  • Two different, non-obvious values
 *  • Must be written in correct sequence
 *  • 430× less likely to happen by accident
 *  • Part of multi-layer protection strategy
 *  • Used across many STM32 critical peripherals
 *  
 *  Bottom line: Intentional sequence = Write permission granted ✓
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  HELPER: BCD CONVERSION
 *  =======================
 * 
 *  RTC stores time in BCD (Binary Coded Decimal) format.
 *  Example: 23 is stored as 0x23 (not 0x17)
 * 
 * ============================================================================ */

uint8_t DecToBcd(uint8_t value) {
    return ((value / 10) << 4) | (value % 10);
}

uint8_t BcdToDec(uint8_t value) {
    return ((value >> 4) * 10) + (value & 0x0F);
}

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE BACKUP DOMAIN ACCESS
 *  ============================================
 * 
 *  The RTC is in a protected "backup domain". We must unlock it first!
 * 
 * ============================================================================ */

void RTC_EnableAccess(void) {
    /* Enable PWR clock */
    RCC->APB4ENR |= RCC_APB4ENR_RTCAPBEN;
    (void)RCC->APB4ENR;
    
    /* ✏️ YOUR TURN: Disable backup domain write protection */
    PWR->CR1 |= ???;                     /* HINT: Which bit enables backup domain access? */
    
    /* Wait for access to be granted */
    while (!(PWR->CR1 & PWR_CR1_DBP));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void RTC_EnableAccess(void) {
 *     RCC->APB4ENR |= RCC_APB4ENR_RTCAPBEN;
 *     (void)RCC->APB4ENR;
 *     PWR->CR1 |= PWR_CR1_DBP;
 *     while (!(PWR->CR1 & PWR_CR1_DBP));
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: ENABLE LSE AND RTC CLOCK
 *  =========================================
 * 
 *  LSE = Low Speed External oscillator (32.768 kHz crystal)
 * 
 * ============================================================================ */

void RTC_ConfigureClock(void) {
    /* ✏️ YOUR TURN: Enable LSE oscillator */
    RCC->BDCR |= ???;                      /* HINT: Which bit turns on LSE? */
    
    /* Wait for LSE to be ready */
    while (!(RCC->BDCR & RCC_BDCR_LSERDY));
    
    /* ✏️ YOUR TURN: Select LSE as RTC clock source */
    RCC->BDCR |= ???;                      /* HINT: Which bits select LSE as RTC source? */
    
    /* ✏️ YOUR TURN: Enable RTC clock */
    RCC->BDCR |= ???;                      /* HINT: Which bit enables the RTC? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void RTC_ConfigureClock(void) {
 *     RCC->BDCR |= RCC_BDCR_LSEON;
 *     while (!(RCC->BDCR & RCC_BDCR_LSERDY));
 *     RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
 *     RCC->BDCR |= RCC_BDCR_RTCEN;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: UNLOCK RTC FOR WRITING
 *  =======================================
 * 
 *  RTC registers are write-protected. We need to write magic keys!
 * 
 * ============================================================================ */

void RTC_DisableWriteProtection(void) {
    /* ✏️ YOUR TURN: Write first key */
    RTC->WPR = ???;                        /* HINT: First unlock key constant? */
    
    /* ✏️ YOUR TURN: Write second key */
    RTC->WPR = ???;                        /* HINT: Second unlock key constant? */
}

void RTC_EnableWriteProtection(void) {
    /* Any wrong value re-enables protection */
    RTC->WPR = 0xFF;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void RTC_DisableWriteProtection(void) {
 *     RTC->WPR = RTC_WPR_KEY1;
 *     RTC->WPR = RTC_WPR_KEY2;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: ENTER INITIALIZATION MODE
 *  ==========================================
 * 
 * ============================================================================ */

void RTC_EnterInitMode(void) {
    /* ✏️ YOUR TURN: Set initialization mode bit */
    RTC->ICSR |= ???;                      /* HINT: Which bit enters init mode? */
    
    /* ✏️ YOUR TURN: Wait for init flag */
    while (!(RTC->ICSR & ???));            /* HINT: Which flag indicates init ready? */
}

void RTC_ExitInitMode(void) {
    /* Clear init bit */
    RTC->ICSR &= ~RTC_ICSR_INIT;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void RTC_EnterInitMode(void) {
 *     RTC->ICSR |= RTC_ICSR_INIT;
 *     while (!(RTC->ICSR & RTC_ICSR_INITF));
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: SET THE TIME
 *  =============================
 * 
 * ============================================================================ */

void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    uint32_t tr;
    
    RTC_DisableWriteProtection();
    RTC_EnterInitMode();
    
    /* ✏️ YOUR TURN: Build time register value in BCD
     * Format: HT(2) HU(4) | MNT(3) MNU(4) | ST(3) SU(4) */
    tr = (DecToBcd(hours) << 16) | 
         (DecToBcd(minutes) << 8) | 
         (???);                            /* HINT: What goes in the lowest byte? */
    
    RTC->TR = tr;
    
    RTC_ExitInitMode();
    RTC_EnableWriteProtection();
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
 *     uint32_t tr;
 *     RTC_DisableWriteProtection();
 *     RTC_EnterInitMode();
 *     tr = (DecToBcd(hours) << 16) | 
 *          (DecToBcd(minutes) << 8) | 
 *          (DecToBcd(seconds));
 *     RTC->TR = tr;
 *     RTC_ExitInitMode();
 *     RTC_EnableWriteProtection();
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: READ THE TIME
 *  ==============================
 * 
 * ============================================================================ */

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} RTC_Time_t;

void RTC_GetTime(RTC_Time_t *time) {
    uint32_t tr;
    
    /* Wait for shadow registers to sync */
    RTC->ICSR &= ~RTC_ICSR_RSF;
    while (!(RTC->ICSR & RTC_ICSR_RSF));
    
    /* ✏️ YOUR TURN: Read time register */
    tr = ???;                              /* HINT: Which register holds the current time? */
    
    /* Extract and convert from BCD */
    time->hours = BcdToDec((tr >> 16) & 0x3F);
    time->minutes = BcdToDec((tr >> 8) & 0x7F);
    time->seconds = BcdToDec(tr & 0x7F);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void RTC_GetTime(RTC_Time_t *time) {
 *     uint32_t tr;
 *     RTC->ICSR &= ~RTC_ICSR_RSF;
 *     while (!(RTC->ICSR & RTC_ICSR_RSF));
 *     tr = RTC->TR;
 *     time->hours = BcdToDec((tr >> 16) & 0x3F);
 *     time->minutes = BcdToDec((tr >> 8) & 0x7F);
 *     time->seconds = BcdToDec(tr & 0x7F);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  COMPLETE: SET DATE
 *  ===================
 * 
 * ============================================================================ */

void RTC_SetDate(uint8_t year, uint8_t month, uint8_t day, uint8_t weekday) {
    uint32_t dr;
    
    RTC_DisableWriteProtection();
    RTC_EnterInitMode();
    
    dr = (DecToBcd(year) << 16) |
         (weekday << 13) |
         (DecToBcd(month) << 8) |
         (DecToBcd(day));
    
    RTC->DR = dr;
    
    RTC_ExitInitMode();
    RTC_EnableWriteProtection();
}

typedef struct {
    uint8_t year;       /* 0-99 */
    uint8_t month;      /* 1-12 */
    uint8_t day;        /* 1-31 */
    uint8_t weekday;    /* 1-7 (1=Monday) */
} RTC_Date_t;

void RTC_GetDate(RTC_Date_t *date) {
    uint32_t dr;
    
    /* Wait for shadow registers to sync */
    RTC->ICSR &= ~RTC_ICSR_RSF;
    while (!(RTC->ICSR & RTC_ICSR_RSF));
    
    dr = RTC->DR;
    
    date->year = BcdToDec((dr >> 16) & 0xFF);
    date->month = BcdToDec((dr >> 8) & 0x1F);
    date->day = BcdToDec(dr & 0x3F);
    date->weekday = (dr >> 13) & 0x07;
}

/* ============================================================================
 * 
 *  COMPLETE: INITIALIZE RTC WITH PRESCALER
 *  =========================================
 * 
 *  For 32.768 kHz LSE:
 *  Async prescaler = 127 (divide by 128)
 *  Sync prescaler = 255 (divide by 256)
 *  Result: 32768 / 128 / 256 = 1 Hz
 * 
 * ============================================================================ */

void RTC_Init(void) {
    RTC_EnableAccess();
    RTC_ConfigureClock();
    
    RTC_DisableWriteProtection();
    RTC_EnterInitMode();
    
    /* Set prescaler for 1 Hz from 32.768 kHz */
    /* PREDIV_A = 127 (bits 16-22), PREDIV_S = 255 (bits 0-14) */
    RTC->PRER = (127U << 16) | 255U;
    
    RTC_ExitInitMode();
    RTC_EnableWriteProtection();
}

/* ============================================================================
 *  HELPER: Simple delay
 * ============================================================================ */

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
 *  MAIN PROGRAM - Display time (requires UART from uart_tutorial.c)
 * 
 * ============================================================================ */

int main(void)
{
    RTC_Time_t time;
    RTC_Date_t date;
    
    /* Initialize RTC */
    RTC_Init();
    
    /* Set initial time: 12:30:00 */
    RTC_SetTime(12, 30, 0);
    
    /* Set initial date: 2025-01-15, Wednesday (3) */
    RTC_SetDate(25, 1, 15, 3);
    
    for(;;) {
        /* Read current time */
        RTC_GetTime(&time);
        RTC_GetDate(&date);
        
        /* 
         * To see the time, you can:
         * 1. Use debugger to inspect 'time' and 'date' structs
         * 2. Add UART output (from uart_tutorial.c)
         * 3. Display on LCD
         * 
         * Time format: time.hours:time.minutes:time.seconds
         * Date format: 20date.year-date.month-date.day
         */
        
        delay(10000000);  /* Wait ~1 second */
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned RTC without HAL:
 *  
 *  ✅ Backup domain access
 *  ✅ LSE oscillator configuration
 *  ✅ RTC clock enabling
 *  ✅ Write protection handling
 *  ✅ Initialization mode
 *  ✅ BCD conversion
 *  ✅ Setting and reading time/date
 *  
 *  KEY CONCEPTS:
 *  • RTC uses BCD format (23 = 0x23)
 *  • Multiple protection layers (PWR, RCC, RTC write protection)
 *  • 32.768 kHz crystal divides evenly to 1 Hz
 *  
 *  NEXT STEPS:
 *  • Configure alarms (ALRMAR, ALRMBR)
 *  • Wakeup timer for low-power modes
 *  • Calibration for accuracy
 *  • Timestamp feature
 *  • Use VBAT to maintain time during power loss
 * 
 * ============================================================================ */
