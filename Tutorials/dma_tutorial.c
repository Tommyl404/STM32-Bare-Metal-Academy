/**
 ******************************************************************************
 * @file           : dma_tutorial.c
 * @brief          : Learning DMA without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ██████╗ ███╗   ███╗ █████╗     ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔══██╗████╗ ████║██╔══██╗    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ██║  ██║██╔████╔██║███████║       ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██║  ██║██║╚██╔╝██║██╔══██║       ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ██████╔╝██║ ╚═╝ ██║██║  ██║       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: DMA (Direct Memory Access)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What DMA is and why it's useful
 *  2. How to configure DMA for memory-to-memory transfers
 *  3. How to configure DMA for peripheral transfers
 *  4. How to use DMA with UART
 * 
 *  WHY DMA?
 *  - CPU doesn't have to copy data byte-by-byte
 *  - Transfers happen in the background
 *  - Much faster for large data blocks
 *  - Frees up CPU for other tasks
 * 
 *  DIFFICULTY: ⭐⭐⭐⭐ (Advanced)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS DMA?
 *  =======================
 * 
 *  DMA = Direct Memory Access
 *  
 *  WITHOUT DMA (CPU copies data):
 *  ┌─────┐                        ┌──────────┐
 *  │ CPU │◄──────────────────────►│ Memory   │
 *  └─────┘  1. Read from source   └──────────┘
 *     │     2. Write to dest
 *     │     3. Repeat for each byte
 *     ▼
 *  CPU is BUSY the whole time!
 *  
 *  WITH DMA (DMA copies data):
 *  ┌─────┐    ┌─────┐             ┌──────────┐
 *  │ CPU │───►│ DMA │◄───────────►│ Memory   │
 *  └─────┘    └─────┘             └──────────┘
 *     │        Copies data
 *     ▼        in background
 *  CPU is FREE to do other work!
 *  
 *  DMA MODES:
 *  ┌────────────────────┬────────────────────────────────────┐
 *  │ Mode               │ Description                        │
 *  ├────────────────────┼────────────────────────────────────┤
 *  │ Memory-to-Memory   │ Copy from RAM to RAM               │
 *  │ Peripheral-to-Mem  │ Read from ADC/UART to RAM          │
 *  │ Memory-to-Periph   │ Write from RAM to UART/SPI         │
 *  │ Circular           │ Auto-restart when complete         │
 *  └────────────────────┴────────────────────────────────────┘
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define DMA1_BASE       0x40020000UL
#define DMAMUX1_BASE    0x40020800UL

/* DMA1 has 8 streams (0-7) */
#define DMA1_Stream0    (DMA1_BASE + 0x010)
#define DMA1_Stream1    (DMA1_BASE + 0x028)
#define DMA1_Stream2    (DMA1_BASE + 0x040)
/* ... */

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
 *  DMA STREAM REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t CR;       /* Configuration register */
    volatile uint32_t NDTR;     /* Number of data register */
    volatile uint32_t PAR;      /* Peripheral address register */
    volatile uint32_t M0AR;     /* Memory 0 address register */
    volatile uint32_t M1AR;     /* Memory 1 address register (double buffer) */
    volatile uint32_t FCR;      /* FIFO control register */
} DMA_Stream_TypeDef;

/* DMA Controller registers */
typedef struct {
    volatile uint32_t LISR;     /* Low interrupt status register */
    volatile uint32_t HISR;     /* High interrupt status register */
    volatile uint32_t LIFCR;    /* Low interrupt flag clear register */
    volatile uint32_t HIFCR;    /* High interrupt flag clear register */
} DMA_TypeDef;

#define DMA1            ((DMA_TypeDef *) DMA1_BASE)
#define DMA1_S0         ((DMA_Stream_TypeDef *) DMA1_Stream0)

/* ============================================================================
 *  DMAMUX REGISTERS (STM32H7 uses DMAMUX for request routing)
 * ============================================================================ */
typedef struct {
    volatile uint32_t CCR;      /* Channel configuration register */
} DMAMUX_Channel_TypeDef;

#define DMAMUX1_Channel0    ((DMAMUX_Channel_TypeDef *) DMAMUX1_BASE)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC Clock Enable */
#define RCC_AHB1ENR_DMA1EN      (1U << 0)   /* DMA1 clock enable */

/* DMA_CR Register Bits */
#define DMA_CR_EN               (1U << 0)   /* Stream enable */
#define DMA_CR_TCIE             (1U << 4)   /* Transfer complete interrupt enable */
#define DMA_CR_HTIE             (1U << 3)   /* Half transfer interrupt enable */
#define DMA_CR_TEIE             (1U << 2)   /* Transfer error interrupt enable */
#define DMA_CR_DIR_P2M          (0U << 6)   /* Peripheral to memory */
#define DMA_CR_DIR_M2P          (1U << 6)   /* Memory to peripheral */
#define DMA_CR_DIR_M2M          (2U << 6)   /* Memory to memory */
#define DMA_CR_CIRC             (1U << 8)   /* Circular mode */
#define DMA_CR_PINC             (1U << 9)   /* Peripheral increment mode */
#define DMA_CR_MINC             (1U << 10)  /* Memory increment mode */
#define DMA_CR_PSIZE_8          (0U << 11)  /* Peripheral size: 8-bit */
#define DMA_CR_PSIZE_16         (1U << 11)  /* Peripheral size: 16-bit */
#define DMA_CR_PSIZE_32         (2U << 11)  /* Peripheral size: 32-bit */
#define DMA_CR_MSIZE_8          (0U << 13)  /* Memory size: 8-bit */
#define DMA_CR_MSIZE_16         (1U << 13)  /* Memory size: 16-bit */
#define DMA_CR_MSIZE_32         (2U << 13)  /* Memory size: 32-bit */
#define DMA_CR_PL_LOW           (0U << 16)  /* Priority: Low */
#define DMA_CR_PL_MEDIUM        (1U << 16)  /* Priority: Medium */
#define DMA_CR_PL_HIGH          (2U << 16)  /* Priority: High */
#define DMA_CR_PL_VERY_HIGH     (3U << 16)  /* Priority: Very high */

/* DMA Status Register Bits (for Stream 0) */
#define DMA_LISR_TCIF0          (1U << 5)   /* Transfer complete flag */
#define DMA_LISR_HTIF0          (1U << 4)   /* Half transfer flag */
#define DMA_LISR_TEIF0          (1U << 3)   /* Transfer error flag */

/* DMA LIFCR (Clear flags for Stream 0) */
#define DMA_LIFCR_CTCIF0        (1U << 5)
#define DMA_LIFCR_CHTIF0        (1U << 4)
#define DMA_LIFCR_CTEIF0        (1U << 3)

/* DMAMUX Request IDs */
#define DMAMUX_REQ_MEM2MEM      0   /* Memory to memory (no request) */
#define DMAMUX_REQ_USART3_RX    45  /* USART3 RX */
#define DMAMUX_REQ_USART3_TX    46  /* USART3 TX */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE DMA CLOCK
 *  ==================================
 * 
 * ============================================================================ */

void DMA_EnableClock(void) {
    /* ✏️ YOUR TURN: Enable DMA1 clock */
    RCC->AHB1ENR |= ???;        /* HINT: DMA1 is on AHB1 bus. Find its enable bit. */
    
    /* Small delay */
    (void)RCC->AHB1ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void DMA_EnableClock(void) {
 *     RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
 *     (void)RCC->AHB1ENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: MEMORY-TO-MEMORY DMA TRANSFER
 *  ===============================================
 * 
 *  Copy data from one memory location to another using DMA.
 * 
 * ============================================================================ */

void DMA_MemToMem(uint32_t *src, uint32_t *dest, uint16_t count) {
    /* Disable stream first (required before configuration) */
    DMA1_S0->CR &= ~DMA_CR_EN;
    while (DMA1_S0->CR & DMA_CR_EN);    /* Wait until disabled */
    
    /* Clear all interrupt flags */
    DMA1->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0;
    
    /* ✏️ YOUR TURN: Set source address (peripheral address for M2M) */
    DMA1_S0->PAR = ???;         /* HINT: Cast the source pointer to 32-bit address */
    
    /* ✏️ YOUR TURN: Set destination address */
    DMA1_S0->M0AR = ???;        /* HINT: Cast the destination pointer to 32-bit address */
    
    /* ✏️ YOUR TURN: Set number of data items to transfer */
    DMA1_S0->NDTR = ???;        /* HINT: How many items to transfer? Use the function parameter. */
    
    /* Configure DMA:
     * - Memory to Memory mode
     * - Increment both source and destination
     * - 32-bit data size
     * - High priority
     */
    DMA1_S0->CR = DMA_CR_DIR_M2M        /* Memory to memory */
                | DMA_CR_PINC           /* Increment source */
                | DMA_CR_MINC           /* Increment destination */
                | DMA_CR_PSIZE_32       /* 32-bit source */
                | DMA_CR_MSIZE_32       /* 32-bit destination */
                | DMA_CR_PL_HIGH;       /* High priority */
    
    /* ✏️ YOUR TURN: Enable the DMA stream to start transfer */
    DMA1_S0->CR |= ???;         /* HINT: Which CR bit starts the DMA stream? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * DMA1_S0->PAR = (uint32_t)src;
 * DMA1_S0->M0AR = (uint32_t)dest;
 * DMA1_S0->NDTR = count;
 * DMA1_S0->CR |= DMA_CR_EN;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: WAIT FOR DMA TRANSFER COMPLETE
 *  ================================================
 * 
 * ============================================================================ */

void DMA_WaitComplete(void) {
    /* ✏️ YOUR TURN: Wait for transfer complete flag */
    while (!(DMA1->LISR & ???));    /* HINT: Which LISR flag indicates transfer complete for stream 0? */
    
    /* Clear the flag */
    DMA1->LIFCR = DMA_LIFCR_CTCIF0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void DMA_WaitComplete(void) {
 *     while (!(DMA1->LISR & DMA_LISR_TCIF0));
 *     DMA1->LIFCR = DMA_LIFCR_CTCIF0;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 2: DMA WITH PERIPHERALS
 *  ===============================
 * 
 *  For peripheral transfers, you also need to configure DMAMUX to route
 *  the peripheral request to the DMA stream.
 *  
 *  Example: UART TX with DMA
 *  1. Configure DMAMUX to route USART3_TX request to DMA stream
 *  2. Set peripheral address to USART3->TDR
 *  3. Set memory address to your data buffer
 *  4. Configure as Memory-to-Peripheral
 *  5. Enable DMA in USART3 (CR3 register)
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: CHECK IF DMA IS BUSY
 *  =====================================
 * 
 * ============================================================================ */

uint8_t DMA_IsBusy(void) {
    /* ✏️ YOUR TURN: Check if DMA stream is enabled (busy) */
    return (DMA1_S0->CR & ???) ? 1 : 0;  /* HINT: Check the enable bit to see if transfer is active */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint8_t DMA_IsBusy(void) {
 *     return (DMA1_S0->CR & DMA_CR_EN) ? 1 : 0;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: GET REMAINING TRANSFER COUNT
 *  ==============================================
 * 
 * ============================================================================ */

uint16_t DMA_GetRemaining(void) {
    /* ✏️ YOUR TURN: Return number of data items remaining */
    return ???;                 /* HINT: Which register counts down during transfer? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint16_t DMA_GetRemaining(void) {
 *     return DMA1_S0->NDTR;
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
 *  MAIN PROGRAM - Memory to Memory DMA example
 * 
 * ============================================================================ */

/* Test buffers */
uint32_t source_buffer[16] = {
    0x11111111, 0x22222222, 0x33333333, 0x44444444,
    0x55555555, 0x66666666, 0x77777777, 0x88888888,
    0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC,
    0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF, 0x00000000
};

uint32_t dest_buffer[16] = {0};

int main(void)
{
    uint8_t success = 1;
    
    /* Enable DMA clock */
    DMA_EnableClock();
    
    /* Start DMA transfer: copy source to dest */
    DMA_MemToMem(source_buffer, dest_buffer, 16);
    
    /* Wait for completion */
    DMA_WaitComplete();
    
    /* Verify the copy */
    for (int i = 0; i < 16; i++) {
        if (dest_buffer[i] != source_buffer[i]) {
            success = 0;
            break;
        }
    }
    
    if (success) {
        /* DMA transfer successful! */
    }
    
    for(;;) {
        /* Application loop */
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned DMA without HAL:
 *  
 *  ✅ What DMA is and why it's useful
 *  ✅ Memory-to-memory transfers
 *  ✅ DMA configuration (direction, sizes, increment)
 *  ✅ Checking DMA status
 *  ✅ Waiting for transfer completion
 *  
 *  ADVANCED TOPICS:
 *  • Circular mode for continuous transfers
 *  • Double buffering for ping-pong buffers
 *  • DMA with ADC for continuous sampling
 *  • DMA with UART for efficient serial comms
 *  • DMA interrupts for async operation
 * 
 * ============================================================================ */
