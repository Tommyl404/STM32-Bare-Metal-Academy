/**
 ******************************************************************************
 * @file           : flash_tutorial.c
 * @brief          : Learning Flash Memory without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ███████╗██╗      █████╗ ███████╗██╗  ██╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔════╝██║     ██╔══██╗██╔════╝██║  ██║    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  █████╗  ██║     ███████║███████╗███████║       ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██╔══╝  ██║     ██╔══██║╚════██║██╔══██║       ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ██║     ███████╗██║  ██║███████║██║  ██║       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: Flash Memory Programming
 * 
 *  WHAT YOU'LL LEARN:
 *  1. Flash memory organization (sectors, banks)
 *  2. How to unlock Flash for writing
 *  3. How to erase Flash sectors
 *  4. How to program Flash memory
 *  5. How to read Flash memory
 * 
 *  ⚠️  WARNING: Be VERY careful with Flash programming!
 *  - Erasing the wrong sector can brick your device
 *  - Never erase Bank 1 Sector 0 (contains your program!)
 *  - Always use sectors at the END of Flash for data storage
 * 
 *  DIFFICULTY: ⭐⭐⭐⭐ (Advanced)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: FLASH MEMORY ORGANIZATION
 *  =====================================
 * 
 *  STM32H753 has 2MB Flash organized in 2 banks:
 *  
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │                    FLASH MEMORY MAP                         │
 *  ├─────────────────────────────────────────────────────────────┤
 *  │ Bank 1 (0x0800_0000 - 0x080F_FFFF) - 1MB                   │
 *  │   Sector 0: 0x0800_0000 - 0x0801_FFFF (128KB) ← YOUR CODE! │
 *  │   Sector 1: 0x0802_0000 - 0x0803_FFFF (128KB)              │
 *  │   Sector 2: 0x0804_0000 - 0x0805_FFFF (128KB)              │
 *  │   Sector 3: 0x0806_0000 - 0x0807_FFFF (128KB)              │
 *  │   Sector 4: 0x0808_0000 - 0x0809_FFFF (128KB)              │
 *  │   Sector 5: 0x080A_0000 - 0x080B_FFFF (128KB)              │
 *  │   Sector 6: 0x080C_0000 - 0x080D_FFFF (128KB)              │
 *  │   Sector 7: 0x080E_0000 - 0x080F_FFFF (128KB) ← SAFE!      │
 *  ├─────────────────────────────────────────────────────────────┤
 *  │ Bank 2 (0x0810_0000 - 0x081F_FFFF) - 1MB                   │
 *  │   Sectors 0-7 (same layout as Bank 1)                      │
 *  └─────────────────────────────────────────────────────────────┘
 *  
 *  KEY CONCEPTS:
 *  - Flash can only be erased in SECTORS (128KB each)
 *  - Erasing sets all bits to 1 (0xFF)
 *  - Programming can only change 1→0, never 0→1
 *  - Must erase before reprogramming!
 *  - Programming is done in 256-bit (32-byte) chunks
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define FLASH_BASE          0x52002000UL

/* Flash memory addresses */
#define FLASH_BANK1_BASE    0x08000000UL
#define FLASH_BANK2_BASE    0x08100000UL
#define FLASH_SECTOR_SIZE   0x20000UL       /* 128KB per sector */

/* We'll use Bank 1 Sector 7 for data storage (safe!) */
#define DATA_SECTOR         7
#define DATA_SECTOR_ADDR    (FLASH_BANK1_BASE + (DATA_SECTOR * FLASH_SECTOR_SIZE))

/* ============================================================================
 *  FLASH REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t ACR;          /* 0x00 - Access control register */
    volatile uint32_t KEYR1;        /* 0x04 - Key register bank 1 */
    volatile uint32_t OPTKEYR;      /* 0x08 - Option key register */
    volatile uint32_t CR1;          /* 0x0C - Control register bank 1 */
    volatile uint32_t SR1;          /* 0x10 - Status register bank 1 */
    volatile uint32_t CCR1;         /* 0x14 - Clear control register bank 1 */
    volatile uint32_t OPTCR;        /* 0x18 - Option control register */
    volatile uint32_t OPTSR_CUR;    /* 0x1C - Option status current */
    volatile uint32_t OPTSR_PRG;    /* 0x20 - Option status to program */
    volatile uint32_t OPTCCR;       /* 0x24 - Option clear control */
    volatile uint32_t PRAR_CUR1;    /* 0x28 */
    volatile uint32_t PRAR_PRG1;    /* 0x2C */
    volatile uint32_t SCAR_CUR1;    /* 0x30 */
    volatile uint32_t SCAR_PRG1;    /* 0x34 */
    volatile uint32_t WPSN_CUR1;    /* 0x38 */
    volatile uint32_t WPSN_PRG1;    /* 0x3C */
    volatile uint32_t BOOT_CUR;     /* 0x40 */
    volatile uint32_t BOOT_PRG;     /* 0x44 */
    volatile uint32_t RESERVED0[2]; /* 0x48-0x4C */
    volatile uint32_t CRCCR1;       /* 0x50 */
    volatile uint32_t CRCSADD1;     /* 0x54 */
    volatile uint32_t CRCEADD1;     /* 0x58 */
    volatile uint32_t CRCDATA;      /* 0x5C */
    volatile uint32_t ECC_FA1;      /* 0x60 */
    volatile uint32_t RESERVED1[39];/* 0x64-0xFC */
    volatile uint32_t KEYR2;        /* 0x100 - Key register bank 2 */
    volatile uint32_t RESERVED2;    /* 0x104 */
    volatile uint32_t CR2;          /* 0x108 - Control register bank 2 */
    volatile uint32_t SR2;          /* 0x10C - Status register bank 2 */
    volatile uint32_t CCR2;         /* 0x110 - Clear control register bank 2 */
} FLASH_TypeDef;

#define FLASH   ((FLASH_TypeDef *) FLASH_BASE)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* Flash Keys - Magic numbers to unlock Flash */
#define FLASH_KEY1              0x45670123UL
#define FLASH_KEY2              0xCDEF89ABUL

/* FLASH_CR Register Bits */
#define FLASH_CR_LOCK           (1U << 0)   /* Lock bit */
#define FLASH_CR_PG             (1U << 1)   /* Programming enable */
#define FLASH_CR_SER            (1U << 2)   /* Sector erase */
#define FLASH_CR_BER            (1U << 3)   /* Bank erase */
#define FLASH_CR_PSIZE_32       (2U << 4)   /* Program size = 32 bits */
#define FLASH_CR_PSIZE_64       (3U << 4)   /* Program size = 64 bits */
#define FLASH_CR_FW             (1U << 6)   /* Force write */
#define FLASH_CR_START          (1U << 7)   /* Start erase */
#define FLASH_CR_SNB_SHIFT      8           /* Sector number shift */
#define FLASH_CR_SNB_MASK       (7U << 8)   /* Sector number mask */

/* FLASH_SR Register Bits */
#define FLASH_SR_BSY            (1U << 0)   /* Busy */
#define FLASH_SR_QW             (1U << 2)   /* Wait queue flag */
#define FLASH_SR_WRPERR         (1U << 17)  /* Write protection error */
#define FLASH_SR_PGSERR         (1U << 18)  /* Programming sequence error */
#define FLASH_SR_STRBERR        (1U << 19)  /* Strobe error */
#define FLASH_SR_INCERR         (1U << 21)  /* Inconsistency error */
#define FLASH_SR_OPERR          (1U << 22)  /* Operation error */
#define FLASH_SR_EOP            (1U << 16)  /* End of operation */

/* Error mask */
#define FLASH_SR_ERRORS         (FLASH_SR_WRPERR | FLASH_SR_PGSERR | \
                                 FLASH_SR_STRBERR | FLASH_SR_INCERR | FLASH_SR_OPERR)

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: UNLOCK FLASH
 *  =============================
 * 
 *  Flash is locked by default to prevent accidental writes.
 *  You must write two "magic" keys to unlock it.
 * 
 * ============================================================================ */

void FLASH_Unlock(void) {
    /* Check if already unlocked */
    if (FLASH->CR1 & FLASH_CR_LOCK) {
        /* ✏️ YOUR TURN: Write the first key */
        FLASH->KEYR1 = FLASH_KEY1;     /* HINT: FLASH_KEY1 */
        
        /* ✏️ YOUR TURN: Write the second key */
        FLASH->KEYR1 = FLASH_KEY2;     /* HINT: FLASH_KEY2 */
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void FLASH_Unlock(void) {
 *     if (FLASH->CR1 & FLASH_CR_LOCK) {
 *         FLASH->KEYR1 = FLASH_KEY1;
 *         FLASH->KEYR1 = FLASH_KEY2;
 *     }
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: LOCK FLASH
 *  ===========================
 * 
 *  Always lock Flash after you're done to prevent accidents!
 * 
 * ============================================================================ */

void FLASH_Lock(void) {
    /* ✏️ YOUR TURN: Set the lock bit */
    FLASH->CR1 |= ???;                    /* HINT: Which bit locks the Flash? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void FLASH_Lock(void) {
 *     FLASH->CR1 |= FLASH_CR_LOCK;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: WAIT FOR FLASH OPERATION
 *  =========================================
 * 
 *  Flash operations take time. You must wait for completion.
 * 
 * ============================================================================ */

uint32_t FLASH_WaitForOperation(void) {
    /* ✏️ YOUR TURN: Wait while Flash is busy */
    while (FLASH->SR1 & ???);             /* HINT: Which status flag shows Flash is busy? */
    
    /* Check for errors */
    if (FLASH->SR1 & FLASH_SR_ERRORS) {
        return 1;   /* Error occurred */
    }
    
    return 0;       /* Success */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint32_t FLASH_WaitForOperation(void) {
 *     while (FLASH->SR1 & FLASH_SR_BSY);
 *     if (FLASH->SR1 & FLASH_SR_ERRORS) {
 *         return 1;
 *     }
 *     return 0;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 2: ERASING A SECTOR
 *  ===========================
 * 
 *  Before writing to Flash, you MUST erase it first!
 *  Erasing sets all bits to 1 (0xFFFFFFFF).
 *  
 *  Steps:
 *  1. Unlock Flash
 *  2. Wait for any ongoing operation
 *  3. Set sector erase bit and sector number
 *  4. Start the erase
 *  5. Wait for completion
 *  6. Lock Flash
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: ERASE A SECTOR
 *  ===============================
 * 
 * ============================================================================ */

uint32_t FLASH_EraseSector(uint8_t sector) {
    uint32_t status;
    
    /* Safety check - never erase sector 0! */
    if (sector == 0) {
        return 1;   /* Refuse to erase - would brick device! */
    }
    
    /* Unlock Flash */
    FLASH_Unlock();
    
    /* Wait for any ongoing operation */
    FLASH_WaitForOperation();
    
    /* Clear any pending flags */
    FLASH->CCR1 = FLASH_SR_ERRORS | FLASH_SR_EOP;
    
    /* ✏️ YOUR TURN: Configure for sector erase */
    /* Set SER (sector erase) and sector number */
    FLASH->CR1 = FLASH_CR_SER | ((sector << FLASH_CR_SNB_SHIFT) & FLASH_CR_SNB_MASK);
    
    /* ✏️ YOUR TURN: Start the erase operation */
    FLASH->CR1 |= ???;                    /* HINT: Which bit starts the operation? */
    
    /* Wait for completion */
    status = FLASH_WaitForOperation();
    
    /* Clear SER bit */
    FLASH->CR1 &= ~FLASH_CR_SER;
    
    /* Lock Flash */
    FLASH_Lock();
    
    return status;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * FLASH->CR1 |= FLASH_CR_START;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 3: PROGRAMMING FLASH
 *  ============================
 * 
 *  STM32H7 programs Flash in 256-bit (32-byte) chunks called "flash words".
 *  
 *  Steps:
 *  1. Unlock Flash
 *  2. Set programming bit (PG)
 *  3. Write 32 bytes (8 x 32-bit words) to Flash address
 *  4. Wait for completion
 *  5. Lock Flash
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: PROGRAM FLASH (32 bytes)
 *  =========================================
 * 
 * ============================================================================ */

uint32_t FLASH_Program256Bit(uint32_t address, uint32_t *data) {
    uint32_t status;
    
    /* Unlock Flash */
    FLASH_Unlock();
    
    /* Wait for any ongoing operation */
    FLASH_WaitForOperation();
    
    /* Clear any pending flags */
    FLASH->CCR1 = FLASH_SR_ERRORS | FLASH_SR_EOP;
    
    /* ✏️ YOUR TURN: Enable programming */
    FLASH->CR1 |= ???;                    /* HINT: Which bit enables programming mode? */
    
    /* Write 8 words (32 bytes = 256 bits) */
    volatile uint32_t *dest = (volatile uint32_t *)address;
    for (int i = 0; i < 8; i++) {
        dest[i] = data[i];
    }
    
    /* Trigger the programming by writing FW bit */
    FLASH->CR1 |= FLASH_CR_FW;
    
    /* Wait for completion */
    status = FLASH_WaitForOperation();
    
    /* Disable programming */
    FLASH->CR1 &= ~FLASH_CR_PG;
    
    /* Lock Flash */
    FLASH_Lock();
    
    return status;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * FLASH->CR1 |= FLASH_CR_PG;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: READ FROM FLASH
 *  ================================
 * 
 *  Reading Flash is simple - just read from the memory address!
 * 
 * ============================================================================ */

uint32_t FLASH_Read(uint32_t address) {
    /* ✏️ YOUR TURN: Read a 32-bit value from Flash */
    return ???;                                          /* HINT: Cast address to volatile uint32_t pointer and dereference */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint32_t FLASH_Read(uint32_t address) {
 *     return *((volatile uint32_t *)address);
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
 *  MAIN PROGRAM - Store and retrieve data from Flash
 * 
 * ============================================================================ */

int main(void)
{
    uint32_t read_value;
    
    /* Data to write (32 bytes = 8 words) */
    uint32_t data_to_write[8] = {
        0xDEADBEEF,     /* Magic number */
        0x12345678,     /* Counter value */
        0x00000001,     /* Settings byte 1 */
        0x00000000,     /* Settings byte 2 */
        0xCAFEBABE,     /* Another magic */
        0x00000000,     /* Reserved */
        0x00000000,     /* Reserved */
        0x55AA55AA      /* Checksum pattern */
    };
    
    /* Step 1: Erase sector 7 (safe for data storage) */
    if (FLASH_EraseSector(7) != 0) {
        /* Erase failed! */
        while(1);
    }
    
    /* Step 2: Program the data */
    if (FLASH_Program256Bit(DATA_SECTOR_ADDR, data_to_write) != 0) {
        /* Program failed! */
        while(1);
    }
    
    /* Step 3: Read it back and verify */
    read_value = FLASH_Read(DATA_SECTOR_ADDR);
    
    if (read_value == 0xDEADBEEF) {
        /* Success! Data was stored and retrieved correctly */
    }
    
    for(;;) {
        /* Application loop */
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned Flash programming without HAL:
 *  
 *  ✅ Flash memory organization (banks, sectors)
 *  ✅ Unlocking/locking Flash for safety
 *  ✅ Erasing Flash sectors
 *  ✅ Programming Flash (256-bit writes)
 *  ✅ Reading Flash memory
 *  
 *  COMMON USE CASES:
 *  • Storing configuration data
 *  • Saving calibration values
 *  • Non-volatile counters
 *  • Bootloader updates
 *  
 *  ⚠️ IMPORTANT TIPS:
 *  • Flash has limited write cycles (~10,000)
 *  • Use wear leveling for frequently updated data
 *  • Consider EEPROM emulation for small data
 *  • Always keep sector 0 protected!
 * 
 * ============================================================================ */
