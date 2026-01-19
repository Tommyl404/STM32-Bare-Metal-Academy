/**
 ******************************************************************************
 * @file           : dac_tutorial.c
 * @brief          : Learning DAC without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ██████╗  █████╗  ██████╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔══██╗██╔══██╗██╔════╝    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ██║  ██║███████║██║            ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██║  ██║██╔══██║██║            ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ██████╔╝██║  ██║╚██████╗       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚═════╝ ╚═╝  ╚═╝ ╚═════╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: DAC (Digital-to-Analog Converter)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What DAC does (opposite of ADC)
 *  2. How to configure DAC output
 *  3. How to generate analog voltages
 *  4. How to create simple waveforms
 * 
 *  HARDWARE:
 *  - DAC1 Channel 1 = PA4
 *  - DAC1 Channel 2 = PA5
 *  - Connect oscilloscope or voltmeter to see output
 * 
 *  DIFFICULTY: ⭐⭐ (Easy)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS DAC?
 *  =======================
 * 
 *  DAC converts a digital number to an analog voltage.
 *  It's the OPPOSITE of ADC!
 *  
 *  DIGITAL WORLD           ANALOG WORLD
 *    (Number)                (Voltage)
 *       │                        │
 *       ▼                        ▼
 *      4095 ─────────────► 3.3V (or Vref)
 *      3072 ─────────────► 2.475V
 *      2048 ─────────────► 1.65V (half)
 *      1024 ─────────────► 0.825V
 *      0 ────────────────► 0V
 *  
 *  RESOLUTION:
 *  - STM32H7 DAC is 12-bit (0-4095)
 *  - Step size = Vref / 4096 = 3.3V / 4096 = 0.8 mV
 *  
 *  USE CASES:
 *  • Audio output
 *  • Waveform generation
 *  • Voltage reference
 *  • Motor control (analog setpoint)
 *  • LED dimming (with low-pass filter)
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOA_BASE      0x58020000UL
#define DAC1_BASE       0x40007400UL

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
 *  DAC REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t CR;           /* 0x00 - Control register */
    volatile uint32_t SWTRIGR;      /* 0x04 - Software trigger register */
    volatile uint32_t DHR12R1;      /* 0x08 - Channel 1 12-bit right-aligned data */
    volatile uint32_t DHR12L1;      /* 0x0C - Channel 1 12-bit left-aligned data */
    volatile uint32_t DHR8R1;       /* 0x10 - Channel 1 8-bit right-aligned data */
    volatile uint32_t DHR12R2;      /* 0x14 - Channel 2 12-bit right-aligned data */
    volatile uint32_t DHR12L2;      /* 0x18 - Channel 2 12-bit left-aligned data */
    volatile uint32_t DHR8R2;       /* 0x1C - Channel 2 8-bit right-aligned data */
    volatile uint32_t DHR12RD;      /* 0x20 - Dual 12-bit right-aligned data */
    volatile uint32_t DHR12LD;      /* 0x24 - Dual 12-bit left-aligned data */
    volatile uint32_t DHR8RD;       /* 0x28 - Dual 8-bit right-aligned data */
    volatile uint32_t DOR1;         /* 0x2C - Channel 1 data output register */
    volatile uint32_t DOR2;         /* 0x30 - Channel 2 data output register */
    volatile uint32_t SR;           /* 0x34 - Status register */
    volatile uint32_t CCR;          /* 0x38 - Calibration control register */
    volatile uint32_t MCR;          /* 0x3C - Mode control register */
    volatile uint32_t SHSR1;        /* 0x40 - Sample and hold sample time 1 */
    volatile uint32_t SHSR2;        /* 0x44 - Sample and hold sample time 2 */
    volatile uint32_t SHHR;         /* 0x48 - Sample and hold hold time */
    volatile uint32_t SHRR;         /* 0x4C - Sample and hold refresh time */
} DAC_TypeDef;

#define DAC1    ((DAC_TypeDef *) DAC1_BASE)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC Clock Enable */
#define RCC_AHB4ENR_GPIOAEN     (1U << 0)
#define RCC_APB1LENR_DAC12EN    (1U << 29)

/* DAC_CR Register Bits - Channel 1 */
#define DAC_CR_EN1              (1U << 0)   /* Channel 1 enable */
#define DAC_CR_TEN1             (1U << 1)   /* Channel 1 trigger enable */
#define DAC_CR_TSEL1_SW         (7U << 2)   /* Software trigger */

/* DAC_CR Register Bits - Channel 2 */
#define DAC_CR_EN2              (1U << 16)  /* Channel 2 enable */
#define DAC_CR_TEN2             (1U << 17)  /* Channel 2 trigger enable */
#define DAC_CR_TSEL2_SW         (7U << 18)  /* Software trigger */

/* DAC_SWTRIGR Register Bits */
#define DAC_SWTRIGR_SWTRIG1     (1U << 0)
#define DAC_SWTRIGR_SWTRIG2     (1U << 1)

/* DAC_MCR Register Bits */
#define DAC_MCR_MODE1_BUFFER    (0U << 0)   /* Normal mode with buffer */
#define DAC_MCR_MODE2_BUFFER    (0U << 16)

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE CLOCKS
 *  ==============================
 * 
 * ============================================================================ */

void DAC_EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOA clock */
    RCC->AHB4ENR |= ???;                         /* HINT: Which bit enables GPIOA? */
    
    /* ✏️ YOUR TURN: Enable DAC1 clock */
    RCC->APB1LENR |= ???;                        /* HINT: Which bit enables DAC1/2? */
    
    (void)RCC->APB1LENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void DAC_EnableClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
 *     RCC->APB1LENR |= RCC_APB1LENR_DAC12EN;
 *     (void)RCC->APB1LENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE GPIO AS ANALOG
 *  =========================================
 * 
 *  DAC output pins must be in ANALOG mode (MODER = 11).
 *  PA4 = DAC1_OUT1
 *  PA5 = DAC1_OUT2
 * 
 * ============================================================================ */

void DAC_ConfigureGPIO(void) {
    /* ✏️ YOUR TURN: Set PA4 to analog mode */
    GPIOA->MODER |= ???;                         /* HINT: What value sets analog mode for pin 4? */
    
    /* Set PA5 to analog mode (for channel 2) */
    GPIOA->MODER |= (3U << (5 * 2));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIOA->MODER |= (3U << (4 * 2));
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: INITIALIZE DAC
 *  ===============================
 * 
 * ============================================================================ */

void DAC_Init(void) {
    /* Configure mode - normal mode with output buffer */
    DAC1->MCR = DAC_MCR_MODE1_BUFFER | DAC_MCR_MODE2_BUFFER;
    
    /* ✏️ YOUR TURN: Enable DAC channel 1 */
    DAC1->CR |= ???;                             /* HINT: Which bit enables channel 1? */
    
    /* Small delay for startup */
    for (volatile int i = 0; i < 1000; i++);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * DAC1->CR |= DAC_CR_EN1;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: SET DAC OUTPUT VALUE
 *  =====================================
 * 
 *  Write a 12-bit value (0-4095) to set the output voltage.
 * 
 * ============================================================================ */

void DAC_SetValue(uint16_t value) {
    /* Limit to 12-bit */
    if (value > 4095) value = 4095;
    
    /* ✏️ YOUR TURN: Write value to DAC data register */
    DAC1->DHR12R1 = ???;                         /* HINT: What value should be written? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void DAC_SetValue(uint16_t value) {
 *     if (value > 4095) value = 4095;
 *     DAC1->DHR12R1 = value;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: SET DAC OUTPUT VOLTAGE
 *  =======================================
 * 
 *  Convert a voltage (0.0 - 3.3V) to DAC value.
 *  Remember:
 *               voltage you want
        value = ─────────────────  × (maximum DAC value)
                 maximum voltage

        value = (voltage / 3.3) × 4095
 * 
 * ============================================================================ */

void DAC_SetVoltage(float voltage) {
    /* Clamp to valid range */
    if (voltage < 0.0f) voltage = 0.0f;
    if (voltage > 3.3f) voltage = 3.3f;
    
    /* ✏️ YOUR TURN: Convert voltage to 12-bit value */
    uint16_t value = ???;                        /* HINT: How do you scale voltage (0-3.3V) to DAC range (0-4095)? */
    
    DAC_SetValue(value);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void DAC_SetVoltage(float voltage) {
 *     if (voltage < 0.0f) voltage = 0.0f;
 *     if (voltage > 3.3f) voltage = 3.3f;
 *     uint16_t value = (voltage / 3.3f) * 4095.0f;
 *     DAC_SetValue(value);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 2: GENERATING WAVEFORMS
 *  ===============================
 * 
 *  By changing DAC value over time, we can create waveforms!
 * 
 * ============================================================================ */

void delay(volatile uint32_t count) {
    while (count--);
}

/* Generate a sawtooth wave */
void DAC_SawtoothWave(void) {
    for (uint16_t i = 0; i < 4096; i += 16) {
        DAC_SetValue(i);
        delay(100);
    }
}

/* Generate a triangle wave */
void DAC_TriangleWave(void) {
    /* Rising edge */
    for (uint16_t i = 0; i < 4096; i += 16) {
        DAC_SetValue(i);
        delay(100);
    }
    /* Falling edge */
    for (uint16_t i = 4095; i > 0; i -= 16) {
        DAC_SetValue(i);
        delay(100);
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
 *  MAIN PROGRAM - Generate different voltages
 * 
 * ============================================================================ */

int main(void)
{
    /* Initialize DAC */
    DAC_EnableClocks();
    DAC_ConfigureGPIO();
    DAC_Init();
    
    /* Test different voltage levels */
    DAC_SetVoltage(0.0f);       /* 0V */
    delay(1000000);
    
    DAC_SetVoltage(1.65f);      /* 1.65V (half) */
    delay(1000000);
    
    DAC_SetVoltage(3.3f);       /* 3.3V (max) */
    delay(1000000);
    
    for(;;) {
        /* Generate continuous triangle wave */
        DAC_TriangleWave();
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned DAC without HAL:
 *  
 *  ✅ What DAC does (digital to analog conversion)
 *  ✅ GPIO analog mode configuration
 *  ✅ DAC initialization
 *  ✅ Setting output value (12-bit)
 *  ✅ Converting voltage to DAC value
 *  ✅ Simple waveform generation
 *  
 *  NEXT STEPS:
 *  • Use Timer trigger for precise waveform timing
 *  • DMA for arbitrary waveform playback
 *  • Dual channel for stereo audio
 *  • Built-in noise/triangle generation
 *  
 *  TIPS:
 *  • Add low-pass filter for smoother output
 *  • Use op-amp buffer for higher current drive
 *  • For audio: use I2S with external DAC for better quality
 * 
 * ============================================================================ */
