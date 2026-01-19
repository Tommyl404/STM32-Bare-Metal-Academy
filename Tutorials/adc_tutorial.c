/**
 ******************************************************************************
 * @file           : adc_tutorial.c
 * @brief          : Learning ADC without HAL - Bare Metal
 ******************************************************************************
 * 
 *   █████╗ ██████╗  ██████╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔══██╗██╔══██╗██╔════╝    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ███████║██║  ██║██║            ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██╔══██║██║  ██║██║            ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ██║  ██║██████╔╝╚██████╗       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚═╝  ╚═╝╚═════╝  ╚═════╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: ADC (Analog-to-Digital Converter)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What ADC does and how it works
 *  2. How to configure ADC resolution and channels
 *  3. How to perform single conversions
 *  4. How to read analog values (voltage, sensors)
 * 
 *  HARDWARE:
 *  - We'll use ADC1 with PA3 (ADC1_INP15) as input
 *  - Connect a potentiometer or voltage source (0-3.3V)
 * 
 *  DIFFICULTY: ⭐⭐⭐ (Intermediate)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS ADC?
 *  =======================
 * 
 *  ADC converts analog voltage to a digital number.
 *  
 *  ANALOG WORLD          DIGITAL WORLD
 *    (Voltage)             (Number)
 *       │                      │
 *       ▼                      ▼
 *      3.3V ─────────────► 65535 (16-bit)
 *      2.5V ─────────────► 49612
 *      1.65V ────────────► 32768 (half)
 *      0.8V ─────────────► 15887
 *      0V ───────────────► 0
 *  
 *  RESOLUTION:
 *  ┌────────────┬──────────┬────────────────────┐
 *  │ Resolution │ Max Value│ Step Size (3.3V)   │
 *  ├────────────┼──────────┼────────────────────┤
 *  │ 8-bit      │ 255      │ 12.9 mV           │
 *  │ 10-bit     │ 1023     │ 3.22 mV           │
 *  │ 12-bit     │ 4095     │ 0.81 mV           │
 *  │ 16-bit     │ 65535    │ 0.05 mV           │
 *  └────────────┴──────────┴────────────────────┘
 *  
 *  VOLTAGE CALCULATION:
 *    Voltage = (ADC_Value / Max_Value) × Vref
 *    Example: (2048 / 4095) × 3.3V = 1.65V
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOA_BASE      0x58020000UL
#define ADC1_BASE       0x40022000UL
#define ADC12_COMMON    0x40022300UL

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
 *  ADC REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t ISR;          /* 0x00 - Interrupt and status register */
    volatile uint32_t IER;          /* 0x04 - Interrupt enable register */
    volatile uint32_t CR;           /* 0x08 - Control register */
    volatile uint32_t CFGR;         /* 0x0C - Configuration register */
    volatile uint32_t CFGR2;        /* 0x10 - Configuration register 2 */
    volatile uint32_t SMPR1;        /* 0x14 - Sampling time register 1 */
    volatile uint32_t SMPR2;        /* 0x18 - Sampling time register 2 */
    volatile uint32_t PCSEL;        /* 0x1C - Channel preselection register */
    volatile uint32_t LTR1;         /* 0x20 - Watchdog threshold 1 */
    volatile uint32_t HTR1;         /* 0x24 */
    volatile uint32_t RESERVED0[2];
    volatile uint32_t SQR1;         /* 0x30 - Regular sequence register 1 */
    volatile uint32_t SQR2;         /* 0x34 */
    volatile uint32_t SQR3;         /* 0x38 */
    volatile uint32_t SQR4;         /* 0x3C */
    volatile uint32_t DR;           /* 0x40 - Data register */
    volatile uint32_t RESERVED1[2];
    volatile uint32_t JSQR;         /* 0x4C - Injected sequence register */
    volatile uint32_t RESERVED2[4];
    volatile uint32_t OFR1;         /* 0x60 */
    volatile uint32_t OFR2;
    volatile uint32_t OFR3;
    volatile uint32_t OFR4;
    volatile uint32_t RESERVED3[4];
    volatile uint32_t JDR1;         /* 0x80 */
    volatile uint32_t JDR2;
    volatile uint32_t JDR3;
    volatile uint32_t JDR4;
    volatile uint32_t RESERVED4[4];
    volatile uint32_t AWD2CR;       /* 0xA0 */
    volatile uint32_t AWD3CR;       /* 0xA4 */
    volatile uint32_t RESERVED5[2];
    volatile uint32_t LTR2;         /* 0xB0 */
    volatile uint32_t HTR2;
    volatile uint32_t LTR3;
    volatile uint32_t HTR3;
    volatile uint32_t DIFSEL;       /* 0xC0 */
    volatile uint32_t CALFACT;      /* 0xC4 */
    volatile uint32_t CALFACT2;     /* 0xC8 */
} ADC_TypeDef;

/* ADC Common registers */
typedef struct {
    volatile uint32_t CSR;          /* 0x00 - Common status register */
    volatile uint32_t RESERVED;
    volatile uint32_t CCR;          /* 0x08 - Common control register */
    volatile uint32_t CDR;          /* 0x0C - Common data register */
} ADC_Common_TypeDef;

#define ADC1        ((ADC_TypeDef *) ADC1_BASE)
#define ADC12_CMN   ((ADC_Common_TypeDef *) ADC12_COMMON)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC Clock Enable */
#define RCC_AHB4ENR_GPIOAEN     (1U << 0)
#define RCC_AHB1ENR_ADC12EN     (1U << 5)

/* ADC_CR Register Bits */
#define ADC_CR_ADEN             (1U << 0)   /* ADC enable */
#define ADC_CR_ADDIS            (1U << 1)   /* ADC disable */
#define ADC_CR_ADSTART          (1U << 2)   /* Start regular conversion */
#define ADC_CR_ADSTP            (1U << 4)   /* Stop regular conversion */
#define ADC_CR_ADVREGEN         (1U << 28)  /* Voltage regulator enable */
#define ADC_CR_DEEPPWD          (1U << 29)  /* Deep power down */
#define ADC_CR_ADCAL            (1U << 31)  /* Calibration */

/* ADC_ISR Register Bits */
#define ADC_ISR_ADRDY           (1U << 0)   /* ADC ready */
#define ADC_ISR_EOC             (1U << 2)   /* End of conversion */
#define ADC_ISR_EOS             (1U << 3)   /* End of sequence */

/* ADC_CFGR Register Bits */
#define ADC_CFGR_RES_16BIT      (0U << 2)   /* 16-bit resolution */
#define ADC_CFGR_RES_14BIT      (1U << 2)   /* 14-bit resolution */
#define ADC_CFGR_RES_12BIT      (2U << 2)   /* 12-bit resolution */
#define ADC_CFGR_RES_10BIT      (3U << 2)   /* 10-bit resolution */
#define ADC_CFGR_RES_8BIT       (4U << 2)   /* 8-bit resolution */
#define ADC_CFGR_CONT           (1U << 13)  /* Continuous mode */

/* ADC Channel */
#define ADC_CHANNEL_15          15          /* PA3 = ADC1_INP15 */

/* Sampling time (for SMPR2, channel 15) */
#define ADC_SMPR_64_5_CYCLES    (5U << ((15-10) * 3))  /* 64.5 cycles */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE CLOCKS
 *  ==============================
 * 
 * ============================================================================ */

void ADC_EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOA clock */
    RCC->AHB4ENR |= ???;        /* HINT: Which AHB4 bit enables GPIO port A? */
    
    /* ✏️ YOUR TURN: Enable ADC1/2 clock */
    RCC->AHB1ENR |= ???;        /* HINT: ADC1 and ADC2 share a clock on AHB1. Find the enable bit. */
    
    (void)RCC->AHB1ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void ADC_EnableClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
 *     RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;
 *     (void)RCC->AHB1ENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE GPIO AS ANALOG
 *  =========================================
 * 
 *  For ADC input, GPIO must be in ANALOG mode (MODER = 11).
 * 
 * ============================================================================ */

void ADC_ConfigureGPIO(void) {
    /* PA3 = ADC1_INP15 */
    /* ✏️ YOUR TURN: Set PA3 to analog mode (11 = 3) */
    GPIOA->MODER |= ???;        /* HINT: Analog mode = 11 binary = 3. Shift to pin 3's position. */
    
    /* Disable pull-up/pull-down */
    GPIOA->PUPDR &= ~(3U << (3 * 2));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIOA->MODER |= (3U << (3 * 2));
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: INITIALIZE ADC
 *  ===============================
 * 
 *  ADC initialization requires several steps:
 *  1. Exit deep power down
 *  (Why? By default, the ADC is in deep sleep to save power. It's
 *   completely off - no circuits powered, no reference voltage, nothing.
 *   This step "wakes it up" so current can flow.)
 *  2. Enable voltage regulator
 *  3. Wait for regulator startup
 *  4. Calibrate
 *  5. Enable ADC
 * 
 * ============================================================================ */

void ADC_Init(void) {
    /* Step 1: Exit deep power down mode */
    ADC1->CR &= ~ADC_CR_DEEPPWD;
    
    /* Step 2: Enable voltage regulator */
    ADC1->CR |= ADC_CR_ADVREGEN;
    
    /* Wait for voltage regulator startup (~10µs) */
    for (volatile int i = 0; i < 10000; i++);
    
    /* Step 3: Start calibration */
    ADC1->CR |= ADC_CR_ADCAL;
    
    /* ✏️ YOUR TURN: Wait for calibration to complete */
    while (ADC1->CR & ???);     /* HINT: Which CR bit is cleared when calibration finishes? */
    
    /* Step 4: Configure resolution (12-bit) */
    ADC1->CFGR = ADC_CFGR_RES_12BIT;
    
    /* Step 5: Enable ADC */
    ADC1->CR |= ADC_CR_ADEN;
    
    /* ✏️ YOUR TURN: Wait for ADC ready */
    while (!(ADC1->ISR & ???)); /* HINT: Which ISR flag indicates ADC is ready to convert? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * while (ADC1->CR & ADC_CR_ADCAL);     // Wait for calibration
 * while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait for ready
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 2: WHAT ARE ADC CHANNELS?
 *  =================================
 * 
 *  One ADC can measure MULTIPLE analog inputs by switching between them.
 *  Each input is called a "channel."
 *  
 *  MULTIPLEXER CONCEPT:
 *  
 *      PA0 (Ch 16) ─┐
 *      PA1 (Ch 17) ─┤
 *      PA2 (Ch 14) ─┤         ┌─────────┐
 *      PA3 (Ch 15) ─┼────────►│   ADC   │───► Digital Value
 *      PA4 (Ch 18) ─┤  MUX    └─────────┘
 *      ...         ─┘
 *  
 *  The ADC has an internal multiplexer (MUX) that connects ONE channel
 *  at a time to the conversion circuitry.
 *  
 *  CHANNEL MAPPING (STM32H7):
 *  ┌──────────┬─────────────┬──────────────────┐
 *  │ GPIO Pin │ ADC Channel │ Usage            │
 *  ├──────────┼─────────────┼──────────────────┤
 *  │ PA0      │ INP16       │ External input   │
 *  │ PA1      │ INP17       │ External input   │
 *  │ PA3      │ INP15       │ External input   │ ← We use this!
 *  │ Internal │ INP18       │ Temperature      │
 *  │ Internal │ INP19       │ Vrefint          │
 *  └──────────┴─────────────┴──────────────────┘
 *  
 *  SAMPLING TIME:
 *  Before converting, the ADC needs time to "sample" the voltage.
 *  Think of it like filling a bucket - longer time = more accurate.
 *  
 *  Trade-off: Longer sampling = more accurate, but slower conversion
 *  
 *  Common values: 1.5, 2.5, 8.5, 16.5, 32.5, 64.5, 387.5, 810.5 cycles
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: CONFIGURE CHANNEL
 *  ===================================
 * 
 *  Select which channel to convert and set sampling time.
 * 
 * ============================================================================ */

void ADC_ConfigureChannel(uint8_t channel) {
    /* Pre-select channel (required for H7) */
    ADC1->PCSEL |= (1U << channel);
    
    /* Set sampling time for channel (channels 10-19 use SMPR2) */
    ADC1->SMPR2 |= ADC_SMPR_64_5_CYCLES;
    
    /* ✏️ YOUR TURN: Set sequence length to 1 and select channel */
    /* SQR1: L[3:0] = 0 (1 conversion), SQ1[4:0] = channel number */
    ADC1->SQR1 = ???;           /* HINT: L=0 for 1 conv. SQ1 starts at bit 6. Place channel there. */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * ADC1->SQR1 = (channel << 6);    // SQ1 = channel, L = 0 (1 conversion)
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: READ ADC VALUE
 *  ===============================
 * 
 * ============================================================================ */

uint16_t ADC_Read(void) {
    /* ✏️ YOUR TURN: Start conversion */
    ADC1->CR |= ???;            /* HINT: Which CR bit triggers a conversion to begin? */
    
    /* ✏️ YOUR TURN: Wait for end of conversion */
    while (!(ADC1->ISR & ???)); /* HINT: Which ISR flag indicates conversion is complete? */
    
    /* ✏️ YOUR TURN: Read and return the result */
    return ???;                 /* HINT: Which register holds the converted digital value? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint16_t ADC_Read(void) {
 *     ADC1->CR |= ADC_CR_ADSTART;
 *     while (!(ADC1->ISR & ADC_ISR_EOC));
 *     return ADC1->DR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 3: CONVERTING ADC VALUE TO VOLTAGE
 *  ==========================================
 * 
 *  The ADC gives you a NUMBER, but you want VOLTAGE. How do you convert?
 *  
 *  THE FORMULA:
 *  ────────────
 *  
 *      Voltage = (ADC_Value / Max_ADC_Value) × Vref
 *  
 *  Where:
 *  • ADC_Value = The number you read (0 to 4095 for 12-bit)
 *  • Max_ADC_Value = Maximum possible ADC value (4095 for 12-bit)
 *  • Vref = Reference voltage (3.3V on STM32H7)
 *  
 *  
 *  WHY THIS FORMULA?
 *  ─────────────────
 *  
 *  The ADC divides the voltage range into equal steps:
 *  
 *      0V ────────────────────────────► 3.3V
 *      │                                  │
 *      0        2048                   4095  (12-bit)
 *  
 *  Each step represents: 3.3V / 4095 = 0.000806V = 0.806mV
 *  
 *  So if ADC reads 2048:
 *    2048 / 4095 = 0.5 (halfway)
 *    0.5 × 3.3V = 1.65V ✅
 *  
 *  
 *  STEP-BY-STEP EXAMPLE:
 *  ──────────────────────
 *  
 *  ADC reads: 3000
 *  
 *  Step 1: Calculate fraction of full scale
 *    3000 / 4095 = 0.7326
 *  
 *  Step 2: Multiply by reference voltage
 *    0.7326 × 3.3V = 2.418V
 *  
 *  ✅ Input voltage is 2.418V
 *  
 *  
 *  QUICK REFERENCE TABLE (12-bit, 3.3V):
 *  ──────────────────────────────────────
 *  ┌───────────┬──────────┐
 *  │ ADC Value │ Voltage  │
 *  ├───────────┼──────────┤
 *  │ 0         │ 0.00 V   │
 *  │ 1024      │ 0.83 V   │
 *  │ 2048      │ 1.65 V   │
 *  │ 3072      │ 2.48 V   │
 *  │ 4095      │ 3.30 V   │
 *  └───────────┴──────────┘
 *  
 *  
 *  CODING TIP:
 *  ───────────
 *  Use FLOAT division (4095.0f) not INTEGER (4095)!
 *  
 *  ❌ WRONG:  voltage = (adc_value / 4095) * 3.3f;
 *             Result: 0 or 3.3 only! (integer division truncates)
 *  
 *  ✅ CORRECT: voltage = (adc_value / 4095.0f) * 3.3f;
 *              Result: Precise fractional voltage
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: CONVERT TO VOLTAGE
 *  ====================================
 * 
 * ============================================================================ */

float ADC_ToVoltage(uint16_t adc_value) {
    /* For 12-bit: max value = 4095, Vref = 3.3V */
    /* ✏️ YOUR TURN: Calculate voltage */
    return ???;                 /* HINT: Voltage = (adc / max_adc) × Vref. Use float division! */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * float ADC_ToVoltage(uint16_t adc_value) {
 *     return (adc_value / 4095.0f) * 3.3f;
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
 *  MAIN PROGRAM - Read analog voltage
 * 
 * ============================================================================ */

int main(void)
{
    uint16_t adc_value;
    float voltage;
    
    /* Initialize ADC */
    ADC_EnableClocks();
    ADC_ConfigureGPIO();
    ADC_Init();
    ADC_ConfigureChannel(ADC_CHANNEL_15);
    
    for(;;) {
        /* Read ADC value */
        adc_value = ADC_Read();
        
        /* Convert to voltage */
        voltage = ADC_ToVoltage(adc_value);
        
        /* voltage now contains the analog input in volts (0.0 - 3.3) */
        /* You can send this over UART or use it for control */
        
        /* Simple delay */
        for (volatile int i = 0; i < 100000; i++);
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned ADC without HAL:
 *  
 *  ✅ What ADC does (analog to digital conversion)
 *  ✅ Resolution and step size
 *  ✅ GPIO analog mode configuration
 *  ✅ ADC initialization and calibration
 *  ✅ Single channel conversion
 *  ✅ Converting ADC values to voltage
 *  
 *  NEXT STEPS:
 *  • Multiple channels (scan mode)
 *  • Continuous conversion
 *  • DMA for fast sampling
 *  • Interrupt-driven conversion
 *  • Analog watchdog
 *  
 *  COMMON APPLICATIONS:
 *  • Potentiometer reading
 *  • Temperature sensors (NTC, LM35)
 *  • Light sensors
 *  • Battery voltage monitoring
 *  • Current sensing
 * 
 * ============================================================================ */
