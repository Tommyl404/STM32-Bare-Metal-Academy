/**
 ******************************************************************************
 * @file           : eth_tutorial.c
 * @brief          : Learning Ethernet (ETH) without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ███████╗████████╗██╗  ██╗███████╗██████╗ ███╗   ██╗███████╗████████╗
 *  ██╔════╝╚══██╔══╝██║  ██║██╔════╝██╔══██╗████╗  ██║██╔════╝╚══██╔══╝
 *  █████╗     ██║   ███████║█████╗  ██████╔╝██╔██╗ ██║█████╗     ██║   
 *  ██╔══╝     ██║   ██╔══██║██╔══╝  ██╔══██╗██║╚██╗██║██╔══╝     ██║   
 *  ███████╗   ██║   ██║  ██║███████╗██║  ██║██║ ╚████║███████╗   ██║   
 *  ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝   ╚═╝   
 * 
 *  INTERACTIVE LEARNING: ETHERNET (MAC + DMA)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. Ethernet architecture (MAC, PHY, RMII)
 *  2. GPIO configuration for Ethernet pins
 *  3. Clock configuration (50 MHz for RMII)
 *  4. MAC initialization
 *  5. DMA descriptors for TX/RX
 *  6. PHY configuration via MDIO
 *  7. Sending and receiving Ethernet frames
 * 
 *  HARDWARE (Nucleo-H753ZI):
 *  - On-board LAN8742A PHY (RMII interface)
 *  - RJ45 connector
 * 
 *  DIFFICULTY: ⭐⭐⭐⭐⭐ (Advanced - Most Complex Peripheral!)
 * 
 *  ⚠️ WARNING: This is a LEARNING tutorial. For production, use a 
 *     proper TCP/IP stack like lwIP!
 * 
 ******************************************************************************
 */

#include <stdint.h>
#include <string.h>

/* ============================================================================
 * 
 *  LESSON 0: ETHERNET ARCHITECTURE
 *  =================================
 * 
 *  Ethernet communication involves multiple layers:
 * 
 *  ┌─────────────────────────────────────────────────────────────────────────┐
 *  │                         YOUR APPLICATION                                │
 *  │                    (TCP/IP Stack - not covered)                         │
 *  └─────────────────────────────────────────────────────────────────────────┘
 *                                    │
 *                                    ▼
 *  ┌─────────────────────────────────────────────────────────────────────────┐
 *  │                          ETHERNET FRAME                                 │
 *  │  ┌──────────┬──────────┬──────┬─────────────────────┬─────┐            │
 *  │  │ Dst MAC  │ Src MAC  │ Type │      Payload        │ CRC │            │
 *  │  │ (6 bytes)│ (6 bytes)│(2 B) │    (46-1500 B)      │(4 B)│            │
 *  │  └──────────┴──────────┴──────┴─────────────────────┴─────┘            │
 *  └─────────────────────────────────────────────────────────────────────────┘
 *                                    │
 *                                    ▼
 *  ┌─────────────────────────────────────────────────────────────────────────┐
 *  │                         STM32H7 MAC                                     │
 *  │              (Media Access Controller - inside MCU)                     │
 *  │                                                                         │
 *  │   • Handles frame transmission/reception                                │
 *  │   • CRC generation/checking                                             │
 *  │   • MAC address filtering                                               │
 *  │   • Uses DMA for efficient data transfer                                │
 *  └─────────────────────────────────────────────────────────────────────────┘
 *                                    │
 *                          RMII Interface (7 signals)
 *                                    │
 *                                    ▼
 *  ┌─────────────────────────────────────────────────────────────────────────┐
 *  │                       EXTERNAL PHY (LAN8742A)                           │
 *  │              (Physical Layer - on Nucleo board)                         │
 *  │                                                                         │
 *  │   • Converts digital signals to analog                                  │
 *  │   • Handles line coding (MLT-3)                                         │
 *  │   • Auto-negotiation (10/100 Mbps)                                      │
 *  │   • Link detection                                                      │
 *  └─────────────────────────────────────────────────────────────────────────┘
 *                                    │
 *                                    ▼
 *                            ┌───────────────┐
 *                            │   RJ45 Jack   │
 *                            │  (Ethernet)   │
 *                            └───────────────┘
 *                                    │
 *                               To Network
 * 
 * 
 *  RMII (Reduced Media Independent Interface):
 *  ─────────────────────────────────────────────────────────────────
 *  Uses only 7 signals (vs 16 for MII):
 *  
 *  ┌────────────┬────────┬────────────────────────────────────────┐
 *  │ Signal     │ Dir    │ Description                            │
 *  ├────────────┼────────┼────────────────────────────────────────┤
 *  │ REF_CLK    │ Input  │ 50 MHz reference clock                 │
 *  │ TX_EN      │ Output │ Transmit enable                        │
 *  │ TXD0       │ Output │ Transmit data bit 0                    │
 *  │ TXD1       │ Output │ Transmit data bit 1                    │
 *  │ CRS_DV     │ Input  │ Carrier sense / Data valid             │
 *  │ RXD0       │ Input  │ Receive data bit 0                     │
 *  │ RXD1       │ Input  │ Receive data bit 1                     │
 *  ├────────────┼────────┼────────────────────────────────────────┤
 *  │ MDIO       │ Bidir  │ Management data (to configure PHY)     │
 *  │ MDC        │ Output │ Management clock (2.5 MHz max)         │
 *  └────────────┴────────┴────────────────────────────────────────┘
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE            0x58024400UL
#define SYSCFG_BASE         0x58000400UL
#define GPIOA_BASE          0x58020000UL
#define GPIOB_BASE          0x58020400UL
#define GPIOC_BASE          0x58020800UL
#define GPIOG_BASE          0x58021800UL

#define ETH_BASE            0x40028000UL
#define ETH_DMA_BASE        (ETH_BASE + 0x1000UL)
#define ETH_MTL_BASE        (ETH_BASE + 0x0C00UL)

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
 *  SYSCFG REGISTERS (for RMII selection)
 * ============================================================================ */
typedef struct {
    volatile uint32_t RESERVED1;
    volatile uint32_t PMCR;         /* 0x04 - Peripheral mode config */
    volatile uint32_t EXTICR[4];
    volatile uint32_t CFGR;
    volatile uint32_t RESERVED2;
    volatile uint32_t CCCSR;
    volatile uint32_t CCVR;
    volatile uint32_t CCCR;
    volatile uint32_t PWRCR;
    volatile uint32_t RESERVED3[61];
    volatile uint32_t PKGR;
    volatile uint32_t RESERVED4[118];
    volatile uint32_t UR0;
    volatile uint32_t UR1;
    /* ... more registers */
} SYSCFG_TypeDef;

#define SYSCFG  ((SYSCFG_TypeDef *) SYSCFG_BASE)

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
#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOG   ((GPIO_TypeDef *) GPIOG_BASE)

/* ============================================================================
 *  ETH MAC REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t MACCR;        /* 0x000 - MAC Configuration */
    volatile uint32_t MACECR;       /* 0x004 - MAC Extended Configuration */
    volatile uint32_t MACPFR;       /* 0x008 - MAC Packet Filter */
    volatile uint32_t MACWTR;       /* 0x00C - MAC Watchdog Timeout */
    volatile uint32_t MACHT0R;      /* 0x010 - MAC Hash Table 0 */
    volatile uint32_t MACHT1R;      /* 0x014 - MAC Hash Table 1 */
    volatile uint32_t RESERVED1[14];
    volatile uint32_t MACVTR;       /* 0x050 - MAC VLAN Tag */
    volatile uint32_t RESERVED2[1];
    volatile uint32_t MACVHTR;      /* 0x058 - MAC VLAN Hash Table */
    volatile uint32_t RESERVED3[1];
    volatile uint32_t MACVIR;       /* 0x060 - MAC VLAN Incl */
    volatile uint32_t MACIVIR;      /* 0x064 - MAC Inner VLAN Incl */
    volatile uint32_t RESERVED4[2];
    volatile uint32_t MACTFCR;      /* 0x070 - MAC TX Flow Control */
    volatile uint32_t RESERVED5[7];
    volatile uint32_t MACRFCR;      /* 0x090 - MAC RX Flow Control */
    volatile uint32_t RESERVED6[7];
    volatile uint32_t MACISR;       /* 0x0B0 - MAC Interrupt Status */
    volatile uint32_t MACIER;       /* 0x0B4 - MAC Interrupt Enable */
    volatile uint32_t MACRXTXSR;    /* 0x0B8 - MAC RX TX Status */
    volatile uint32_t RESERVED7[1];
    volatile uint32_t MACPCSR;      /* 0x0C0 - MAC PMT Control Status */
    volatile uint32_t MACRWKPFR;    /* 0x0C4 - MAC RWK Packet Filter */
    volatile uint32_t RESERVED8[2];
    volatile uint32_t MACLCSR;      /* 0x0D0 - MAC LPI Control Status */
    volatile uint32_t MACLTCR;      /* 0x0D4 - MAC LPI Timers Control */
    volatile uint32_t MACLETR;      /* 0x0D8 - MAC LPI Entry Timer */
    volatile uint32_t MAC1USTCR;    /* 0x0DC - MAC 1us Tic Counter */
    volatile uint32_t RESERVED9[12];
    volatile uint32_t MACVR;        /* 0x110 - MAC Version */
    volatile uint32_t MACDR;        /* 0x114 - MAC Debug */
    volatile uint32_t RESERVED10[1];
    volatile uint32_t MACHWF0R;     /* 0x11C - MAC HW Feature 0 */
    volatile uint32_t MACHWF1R;     /* 0x120 - MAC HW Feature 1 */
    volatile uint32_t MACHWF2R;     /* 0x124 - MAC HW Feature 2 */
    volatile uint32_t RESERVED11[54];
    volatile uint32_t MACMDIOAR;    /* 0x200 - MAC MDIO Address */
    volatile uint32_t MACMDIODR;    /* 0x204 - MAC MDIO Data */
    volatile uint32_t RESERVED12[2];
    volatile uint32_t MACARPAR;     /* 0x210 - MAC ARP Address */
    volatile uint32_t RESERVED13[7];
    volatile uint32_t RESERVED14[32];
    volatile uint32_t MACA0HR;      /* 0x300 - MAC Address 0 High */
    volatile uint32_t MACA0LR;      /* 0x304 - MAC Address 0 Low */
    volatile uint32_t MACA1HR;      /* 0x308 - MAC Address 1 High */
    volatile uint32_t MACA1LR;      /* 0x30C - MAC Address 1 Low */
    volatile uint32_t MACA2HR;      /* 0x310 - MAC Address 2 High */
    volatile uint32_t MACA2LR;      /* 0x314 - MAC Address 2 Low */
    volatile uint32_t MACA3HR;      /* 0x318 - MAC Address 3 High */
    volatile uint32_t MACA3LR;      /* 0x31C - MAC Address 3 Low */
} ETH_MAC_TypeDef;

#define ETH_MAC ((ETH_MAC_TypeDef *) ETH_BASE)

/* ============================================================================
 *  ETH MTL (MAC Transaction Layer) REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t MTLOMR;       /* 0x000 - MTL Operation Mode */
    volatile uint32_t RESERVED1[7];
    volatile uint32_t MTLISR;       /* 0x020 - MTL Interrupt Status */
    volatile uint32_t RESERVED2[55];
    volatile uint32_t MTLTQOMR;     /* 0x100 - MTL TX Queue Operation Mode */
    volatile uint32_t MTLTQUR;      /* 0x104 - MTL TX Queue Underflow */
    volatile uint32_t MTLTQDR;      /* 0x108 - MTL TX Queue Debug */
    volatile uint32_t RESERVED3[8];
    volatile uint32_t MTLQICSR;     /* 0x12C - MTL Q Interrupt Control Status */
    volatile uint32_t MTLRQOMR;     /* 0x130 - MTL RX Queue Operation Mode */
    volatile uint32_t MTLRQMPOCR;   /* 0x134 - MTL RX Queue Missed Packet */
    volatile uint32_t MTLRQDR;      /* 0x138 - MTL RX Queue Debug */
} ETH_MTL_TypeDef;

#define ETH_MTL ((ETH_MTL_TypeDef *) ETH_MTL_BASE)

/* ============================================================================
 *  ETH DMA REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t DMAMR;        /* 0x000 - DMA Mode */
    volatile uint32_t DMASBMR;      /* 0x004 - DMA System Bus Mode */
    volatile uint32_t DMAISR;       /* 0x008 - DMA Interrupt Status */
    volatile uint32_t DMADSR;       /* 0x00C - DMA Debug Status */
    volatile uint32_t RESERVED1[60];
    volatile uint32_t DMACCR;       /* 0x100 - DMA Channel Control */
    volatile uint32_t DMACTCR;      /* 0x104 - DMA Channel TX Control */
    volatile uint32_t DMACRCR;      /* 0x108 - DMA Channel RX Control */
    volatile uint32_t RESERVED2[2];
    volatile uint32_t DMACTDLAR;    /* 0x114 - DMA Channel TX Desc List Addr */
    volatile uint32_t RESERVED3[1];
    volatile uint32_t DMACRDLAR;    /* 0x11C - DMA Channel RX Desc List Addr */
    volatile uint32_t DMACTDTPR;    /* 0x120 - DMA Channel TX Desc Tail Ptr */
    volatile uint32_t RESERVED4[1];
    volatile uint32_t DMACRDTPR;    /* 0x128 - DMA Channel RX Desc Tail Ptr */
    volatile uint32_t DMACTDRLR;    /* 0x12C - DMA Channel TX Desc Ring Length */
    volatile uint32_t DMACRDRLR;    /* 0x130 - DMA Channel RX Desc Ring Length */
    volatile uint32_t DMACIER;      /* 0x134 - DMA Channel Interrupt Enable */
    volatile uint32_t DMACRIWTR;    /* 0x138 - DMA Channel RX Interrupt Watchdog */
    volatile uint32_t RESERVED5[2];
    volatile uint32_t DMACCATDR;    /* 0x144 - DMA Channel Current App TX Desc */
    volatile uint32_t RESERVED6[1];
    volatile uint32_t DMACCARDR;    /* 0x14C - DMA Channel Current App RX Desc */
    volatile uint32_t RESERVED7[1];
    volatile uint32_t DMACCATBR;    /* 0x154 - DMA Channel Current App TX Buffer */
    volatile uint32_t RESERVED8[1];
    volatile uint32_t DMACCARBR;    /* 0x15C - DMA Channel Current App RX Buffer */
    volatile uint32_t DMACSR;       /* 0x160 - DMA Channel Status */
    volatile uint32_t RESERVED9[2];
    volatile uint32_t DMACMFCR;     /* 0x16C - DMA Channel Missed Frame Count */
} ETH_DMA_TypeDef;

#define ETH_DMA ((ETH_DMA_TypeDef *) ETH_DMA_BASE)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC Clock Enable */
#define RCC_AHB4ENR_GPIOAEN     (1U << 0)
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_AHB4ENR_GPIOCEN     (1U << 2)
#define RCC_AHB4ENR_GPIOGEN     (1U << 6)
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)
#define RCC_AHB1ENR_ETH1MACEN   (1U << 15)
#define RCC_AHB1ENR_ETH1TXEN    (1U << 16)
#define RCC_AHB1ENR_ETH1RXEN    (1U << 17)

/* SYSCFG PMCR - Ethernet PHY Interface Selection */
#define SYSCFG_PMCR_EPIS_RMII   (4U << 21)  /* RMII mode */

/* ETH MAC Configuration Register (MACCR) */
#define ETH_MACCR_RE            (1U << 0)   /* Receiver Enable */
#define ETH_MACCR_TE            (1U << 1)   /* Transmitter Enable */
#define ETH_MACCR_DCRS          (1U << 9)   /* Disable Carrier Sense During TX */
#define ETH_MACCR_FES           (1U << 14)  /* Fast Ethernet Speed (100 Mbps) */
#define ETH_MACCR_DM            (1U << 13)  /* Duplex Mode */

/* ETH MDIO Address Register */
#define ETH_MACMDIOAR_MB        (1U << 0)   /* MII Busy */
#define ETH_MACMDIOAR_C45E      (1U << 1)   /* Clause 45 Enable */
#define ETH_MACMDIOAR_GOC_READ  (3U << 2)   /* GMII Operation: Read */
#define ETH_MACMDIOAR_GOC_WRITE (1U << 2)   /* GMII Operation: Write */
#define ETH_MACMDIOAR_CR_DIV42  (0U << 8)   /* CSR Clock /42 */
#define ETH_MACMDIOAR_CR_DIV62  (1U << 8)   /* CSR Clock /62 */
#define ETH_MACMDIOAR_CR_DIV16  (2U << 8)   /* CSR Clock /16 */
#define ETH_MACMDIOAR_CR_DIV26  (3U << 8)   /* CSR Clock /26 */
#define ETH_MACMDIOAR_CR_DIV102 (4U << 8)   /* CSR Clock /102 */
#define ETH_MACMDIOAR_CR_DIV124 (5U << 8)   /* CSR Clock /124 */

/* ETH DMA Mode Register (DMAMR) */
#define ETH_DMAMR_SWR           (1U << 0)   /* Software Reset */

/* ETH DMA System Bus Mode Register (DMASBMR) */
#define ETH_DMASBMR_FB          (1U << 0)   /* Fixed Burst */
#define ETH_DMASBMR_AAL         (1U << 12)  /* Address-Aligned Beats */

/* ETH DMA Channel TX Control Register */
#define ETH_DMACTCR_ST          (1U << 0)   /* Start TX */

/* ETH DMA Channel RX Control Register */
#define ETH_DMACRCR_SR          (1U << 0)   /* Start RX */
#define ETH_DMACRCR_RBSZ_SHIFT  1

/* ETH MTL TX Queue Operation Mode */
#define ETH_MTLTQOMR_TSF        (1U << 1)   /* TX Store and Forward */
#define ETH_MTLTQOMR_TQS_SHIFT  16          /* TX Queue Size */

/* ETH MTL RX Queue Operation Mode */
#define ETH_MTLRQOMR_RSF        (1U << 5)   /* RX Store and Forward */
#define ETH_MTLRQOMR_RQS_SHIFT  20          /* RX Queue Size */

/* GPIO Alternate Function */
#define GPIO_AF11_ETH           11U

/* PHY (LAN8742A) Registers */
#define PHY_ADDR                0           /* PHY Address on Nucleo board */
#define PHY_BCR                 0           /* Basic Control Register */
#define PHY_BSR                 1           /* Basic Status Register */
#define PHY_PHYID1              2           /* PHY Identifier 1 */
#define PHY_PHYID2              3           /* PHY Identifier 2 */
#define PHY_SMR                 31          /* Special Modes Register */

/* PHY BCR bits */
#define PHY_BCR_RESET           (1U << 15)
#define PHY_BCR_AUTONEG         (1U << 12)
#define PHY_BCR_FULLDUPLEX      (1U << 8)
#define PHY_BCR_100MBPS         (1U << 13)

/* PHY BSR bits */
#define PHY_BSR_LINK_UP         (1U << 2)
#define PHY_BSR_AUTONEG_DONE    (1U << 5)

/* ============================================================================
 * 
 *  LESSON 2: DMA DESCRIPTORS
 *  ==========================
 * 
 *  Ethernet uses DMA for efficient data transfer.
 *  DMA descriptors tell the DMA engine where to find/put data.
 * 
 *  ┌───────────────────────────────────────────────────────────────────────┐
 *  │ TX Descriptor (Normal - Write-back format)                           │
 *  ├──────────────────┬────────────────────────────────────────────────────┤
 *  │ TDES0            │ Buffer 1 Address                                  │
 *  │ TDES1            │ Buffer 2 Address                                  │
 *  │ TDES2            │ Buffer lengths, VLAN tag                          │
 *  │ TDES3            │ OWN, Status, Frame length                         │
 *  └──────────────────┴────────────────────────────────────────────────────┘
 * 
 *  ┌───────────────────────────────────────────────────────────────────────┐
 *  │ RX Descriptor (Normal - Write-back format)                           │
 *  ├──────────────────┬────────────────────────────────────────────────────┤
 *  │ RDES0            │ Buffer 1 Address                                  │
 *  │ RDES1            │ Reserved                                          │
 *  │ RDES2            │ Buffer 2 Address                                  │
 *  │ RDES3            │ OWN, Buffer addresses valid, IOC                  │
 *  └──────────────────┴────────────────────────────────────────────────────┘
 * 
 * ============================================================================ */

/* DMA Descriptor structure */
typedef struct {
    volatile uint32_t DESC0;
    volatile uint32_t DESC1;
    volatile uint32_t DESC2;
    volatile uint32_t DESC3;
    /* Backup addresses (for write-back format) */
    volatile uint32_t BackupAddr0;
    volatile uint32_t BackupAddr1;
    volatile uint32_t Reserved[2];
} ETH_DMADescTypeDef;

/* TX Descriptor bits (TDES2) */
#define ETH_TDES2_B1L_MASK      0x00003FFFU /* Buffer 1 Length */
#define ETH_TDES2_B2L_SHIFT     16

/* TX Descriptor bits (TDES3) */
#define ETH_TDES3_OWN           (1U << 31)  /* OWN bit - DMA owns descriptor */
#define ETH_TDES3_FD            (1U << 29)  /* First Descriptor */
#define ETH_TDES3_LD            (1U << 28)  /* Last Descriptor */
#define ETH_TDES3_CIC_ALL       (3U << 16)  /* Checksum insertion: all */

/* RX Descriptor bits (RDES3) */
#define ETH_RDES3_OWN           (1U << 31)  /* OWN bit - DMA owns descriptor */
#define ETH_RDES3_IOC           (1U << 30)  /* Interrupt on Completion */
#define ETH_RDES3_BUF1V         (1U << 24)  /* Buffer 1 Address Valid */
#define ETH_RDES3_PL_MASK       0x00007FFFU /* Packet Length mask */
#define ETH_RDES3_ES            (1U << 15)  /* Error Summary */
#define ETH_RDES3_FD            (1U << 29)  /* First Descriptor */
#define ETH_RDES3_LD            (1U << 28)  /* Last Descriptor */

/* Buffer sizes */
#define ETH_RX_BUF_SIZE         1536
#define ETH_TX_BUF_SIZE         1536
#define ETH_RX_DESC_CNT         4
#define ETH_TX_DESC_CNT         4

/* Buffers and Descriptors (must be in non-cached RAM or cache-managed) */
__attribute__((aligned(4))) ETH_DMADescTypeDef RxDescriptors[ETH_RX_DESC_CNT];
__attribute__((aligned(4))) ETH_DMADescTypeDef TxDescriptors[ETH_TX_DESC_CNT];
__attribute__((aligned(4))) uint8_t RxBuffer[ETH_RX_DESC_CNT][ETH_RX_BUF_SIZE];
__attribute__((aligned(4))) uint8_t TxBuffer[ETH_TX_DESC_CNT][ETH_TX_BUF_SIZE];

/* Current descriptor indices */
volatile uint32_t TxDescIdx = 0;
volatile uint32_t RxDescIdx = 0;

/* ============================================================================
 * 
 *  LESSON 3: ETHERNET GPIO PINS (Nucleo-H753ZI)
 *  =============================================
 * 
 *  RMII Interface pins:
 *  
 *  ┌────────────────┬──────────────┬────────────────────────────────────────┐
 *  │ Function       │ Pin          │ AF11                                   │
 *  ├────────────────┼──────────────┼────────────────────────────────────────┤
 *  │ ETH_REF_CLK    │ PA1          │ 50 MHz from PHY                        │
 *  │ ETH_MDIO       │ PA2          │ Management data                        │
 *  │ ETH_MDC        │ PC1          │ Management clock                       │
 *  │ ETH_CRS_DV     │ PA7          │ Carrier sense / Data valid             │
 *  │ ETH_RXD0       │ PC4          │ Receive data 0                         │
 *  │ ETH_RXD1       │ PC5          │ Receive data 1                         │
 *  │ ETH_TX_EN      │ PG11         │ Transmit enable                        │
 *  │ ETH_TXD0       │ PG13         │ Transmit data 0                        │
 *  │ ETH_TXD1       │ PB13         │ Transmit data 1                        │
 *  └────────────────┴──────────────┴────────────────────────────────────────┘
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE CLOCKS
 *  ==============================
 * 
 * ============================================================================ */

void ETH_EnableClocks(void) {
    /* Enable GPIO clocks */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;    
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;    
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;    
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;    
    
    /* Enable SYSCFG clock (for RMII selection) */
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    
    /* ✏️ YOUR TURN: Enable ETH MAC clock */
    RCC->AHB1ENR |= ???;        /* HINT: Which bit enables Ethernet MAC? */
    
    /* ✏️ YOUR TURN: Enable ETH TX clock */
    RCC->AHB1ENR |= ???;        /* HINT: Which bit enables Ethernet TX? */
    
    /* ✏️ YOUR TURN: Enable ETH RX clock */
    RCC->AHB1ENR |= ???;        /* HINT: Which bit enables Ethernet RX? */
    
    (void)RCC->AHB1ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * RCC->AHB1ENR |= RCC_AHB1ENR_ETH1MACEN;
 * RCC->AHB1ENR |= RCC_AHB1ENR_ETH1TXEN;
 * RCC->AHB1ENR |= RCC_AHB1ENR_ETH1RXEN;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE GPIO FOR ETHERNET
 *  =============================================
 * 
 * ============================================================================ */

void GPIO_ConfigurePin(GPIO_TypeDef *port, uint8_t pin, uint8_t af) {
    /* Set Alternate Function mode (10) */
    port->MODER &= ~(3U << (pin * 2));
    port->MODER |= (2U << (pin * 2));
    
    /* Very High Speed */
    port->OSPEEDR |= (3U << (pin * 2));
    
    /* No Pull-up/Pull-down */
    port->PUPDR &= ~(3U << (pin * 2));
    
    /* Push-Pull */
    port->OTYPER &= ~(1U << pin);
    
    /* Set Alternate Function */
    if (pin < 8) {
        port->AFR[0] &= ~(0xFU << (pin * 4));
        port->AFR[0] |= (af << (pin * 4));
    } else {
        port->AFR[1] &= ~(0xFU << ((pin - 8) * 4));
        port->AFR[1] |= (af << ((pin - 8) * 4));
    }
}

void ETH_ConfigureGPIO(void) {
    /* ✏️ YOUR TURN: Configure PA1 (ETH_REF_CLK) with AF11 */
    GPIO_ConfigurePin(GPIOA, ???, ???);     /* HINT: What pin and AF for REF_CLK? */
    
    /* PA2 - MDIO */
    GPIO_ConfigurePin(GPIOA, 2, GPIO_AF11_ETH);
    
    /* PA7 - CRS_DV */
    GPIO_ConfigurePin(GPIOA, 7, GPIO_AF11_ETH);
    
    /* PB13 - TXD1 */
    GPIO_ConfigurePin(GPIOB, 13, GPIO_AF11_ETH);
    
    /* PC1 - MDC */
    GPIO_ConfigurePin(GPIOC, 1, GPIO_AF11_ETH);
    
    /* PC4 - RXD0 */
    GPIO_ConfigurePin(GPIOC, 4, GPIO_AF11_ETH);
    
    /* PC5 - RXD1 */
    GPIO_ConfigurePin(GPIOC, 5, GPIO_AF11_ETH);
    
    /* PG11 - TX_EN */
    GPIO_ConfigurePin(GPIOG, 11, GPIO_AF11_ETH);
    
    /* PG13 - TXD0 */
    GPIO_ConfigurePin(GPIOG, 13, GPIO_AF11_ETH);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIO_ConfigurePin(GPIOA, 1, GPIO_AF11_ETH);
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: SELECT RMII MODE
 *  ==================================
 * 
 * ============================================================================ */

void ETH_SelectRMII(void) {
    /* ✏️ YOUR TURN: Set SYSCFG PMCR to select RMII mode */
    SYSCFG->PMCR |= ???;        /* HINT: Which bits select RMII interface? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void ETH_SelectRMII(void) {
 *     SYSCFG->PMCR |= SYSCFG_PMCR_EPIS_RMII;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: SOFTWARE RESET DMA
 *  ====================================
 * 
 * ============================================================================ */

void ETH_DMAReset(void) {
    /* ✏️ YOUR TURN: Set Software Reset bit */
    ETH_DMA->DMAMR |= ???;      /* HINT: Which bit triggers DMA software reset? */
    
    /* Wait for reset to complete */
    while (ETH_DMA->DMAMR & ETH_DMAMR_SWR);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void ETH_DMAReset(void) {
 *     ETH_DMA->DMAMR |= ETH_DMAMR_SWR;
 *     while (ETH_DMA->DMAMR & ETH_DMAMR_SWR);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 4: MDIO - TALKING TO THE PHY
 *  =====================================
 * 
 *  The MAC communicates with the PHY via MDIO (Management Data I/O).
 *  This is like a mini I2C/SPI for PHY registers.
 * 
 * ============================================================================ */

uint16_t ETH_ReadPHY(uint8_t phy_addr, uint8_t reg_addr) {
    /* Wait if MDIO is busy */
    while (ETH_MAC->MACMDIOAR & ETH_MACMDIOAR_MB);
    
    /* Configure and start read operation */
    ETH_MAC->MACMDIOAR = ((phy_addr & 0x1F) << 21)      /* PHY address */
                       | ((reg_addr & 0x1F) << 16)      /* Register address */
                       | ETH_MACMDIOAR_CR_DIV102        /* Clock divider */
                       | ETH_MACMDIOAR_GOC_READ         /* Read operation */
                       | ETH_MACMDIOAR_MB;              /* Busy (starts op) */
    
    /* Wait for completion */
    while (ETH_MAC->MACMDIOAR & ETH_MACMDIOAR_MB);
    
    /* Return the data */
    return (uint16_t)(ETH_MAC->MACMDIODR & 0xFFFF);
}

void ETH_WritePHY(uint8_t phy_addr, uint8_t reg_addr, uint16_t value) {
    /* Wait if MDIO is busy */
    while (ETH_MAC->MACMDIOAR & ETH_MACMDIOAR_MB);
    
    /* Write data first */
    ETH_MAC->MACMDIODR = value;
    
    /* Configure and start write operation */
    ETH_MAC->MACMDIOAR = ((phy_addr & 0x1F) << 21)
                       | ((reg_addr & 0x1F) << 16)
                       | ETH_MACMDIOAR_CR_DIV102
                       | ETH_MACMDIOAR_GOC_WRITE
                       | ETH_MACMDIOAR_MB;
    
    /* Wait for completion */
    while (ETH_MAC->MACMDIOAR & ETH_MACMDIOAR_MB);
}

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: RESET AND CONFIGURE PHY
 *  =========================================
 * 
 * ============================================================================ */

uint8_t ETH_InitPHY(void) {
    uint32_t timeout = 100000;
    
    /* ✏️ YOUR TURN: Reset the PHY */
    ETH_WritePHY(PHY_ADDR, PHY_BCR, ???);   /* HINT: Which bit resets the PHY? */
    
    /* Wait for reset complete */
    while (ETH_ReadPHY(PHY_ADDR, PHY_BCR) & PHY_BCR_RESET) {
        if (--timeout == 0) return 0;   /* Timeout */
    }
    
    /* Read PHY ID to verify communication */
    uint16_t id1 = ETH_ReadPHY(PHY_ADDR, PHY_PHYID1);
    uint16_t id2 = ETH_ReadPHY(PHY_ADDR, PHY_PHYID2);
    
    /* LAN8742A should return ID 0x0007 C130 */
    if (id1 != 0x0007) {
        return 0;   /* Wrong PHY or communication error */
    }
    
    /* Enable auto-negotiation */
    ETH_WritePHY(PHY_ADDR, PHY_BCR, PHY_BCR_AUTONEG);
    
    /* Wait for link up (with timeout) */
    timeout = 1000000;
    while (!(ETH_ReadPHY(PHY_ADDR, PHY_BSR) & PHY_BSR_LINK_UP)) {
        if (--timeout == 0) return 0;
    }
    
    return 1;   /* Success */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * ETH_WritePHY(PHY_ADDR, PHY_BCR, PHY_BCR_RESET);
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: INITIALIZE DMA DESCRIPTORS
 *  ============================================
 * 
 * ============================================================================ */

void ETH_InitDescriptors(void) {
    uint32_t i;
    
    /* Initialize TX descriptors */
    for (i = 0; i < ETH_TX_DESC_CNT; i++) {
        TxDescriptors[i].DESC0 = 0;
        TxDescriptors[i].DESC1 = 0;
        TxDescriptors[i].DESC2 = 0;
        TxDescriptors[i].DESC3 = 0;     /* OWN = 0, CPU owns it */
    }
    
    /* Initialize RX descriptors */
    for (i = 0; i < ETH_RX_DESC_CNT; i++) {
        /* Set buffer address */
        RxDescriptors[i].DESC0 = (uint32_t)RxBuffer[i];
        RxDescriptors[i].DESC1 = 0;
        RxDescriptors[i].DESC2 = 0;
        
        /* ✏️ YOUR TURN: Set OWN bit (DMA owns) and BUF1V, IOC */
        RxDescriptors[i].DESC3 = ???;   /* HINT: Combine OWN + buffer valid + interrupt flags */
    }
    
    /* Set descriptor list addresses */
    ETH_DMA->DMACTDLAR = (uint32_t)TxDescriptors;
    ETH_DMA->DMACRDLAR = (uint32_t)RxDescriptors;
    
    /* Set ring length (number of descriptors - 1) */
    ETH_DMA->DMACTDRLR = ETH_TX_DESC_CNT - 1;
    ETH_DMA->DMACRDRLR = ETH_RX_DESC_CNT - 1;
    
    /* Set RX buffer size */
    ETH_DMA->DMACRCR = (ETH_RX_BUF_SIZE << ETH_DMACRCR_RBSZ_SHIFT);
    
    /* Set tail pointers */
    ETH_DMA->DMACTDTPR = (uint32_t)&TxDescriptors[0];
    ETH_DMA->DMACRDTPR = (uint32_t)&RxDescriptors[ETH_RX_DESC_CNT - 1];
    
    TxDescIdx = 0;
    RxDescIdx = 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * RxDescriptors[i].DESC3 = ETH_RDES3_OWN | ETH_RDES3_IOC | ETH_RDES3_BUF1V;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 7: SET MAC ADDRESS
 *  =================================
 * 
 * ============================================================================ */

void ETH_SetMACAddress(uint8_t *addr) {
    /* MAC Address format: AA:BB:CC:DD:EE:FF
     * MACA0HR = 0x0000FFEE  (high bytes)
     * MACA0LR = 0xDDCCBBAA  (low bytes)
     */
    
    /* ✏️ YOUR TURN: Set MAC Address Low Register */
    ETH_MAC->MACA0LR = ???;     /* HINT: Pack 4 bytes into 32-bit register */
    
    /* Set MAC Address High Register (with AE = Address Enable) */
    ETH_MAC->MACA0HR = (1U << 31) | (addr[5] << 8) | addr[4];
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * ETH_MAC->MACA0LR = addr[0] | (addr[1] << 8) | (addr[2] << 16) | (addr[3] << 24);
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 8: CONFIGURE AND START MAC
 *  =========================================
 * 
 * ============================================================================ */

void ETH_StartMAC(void) {
    /* Configure MTL - TX Queue */
    ETH_MTL->MTLTQOMR = ETH_MTLTQOMR_TSF          /* Store and Forward */
                      | (7U << ETH_MTLTQOMR_TQS_SHIFT);  /* Queue size */
    
    /* Configure MTL - RX Queue */
    ETH_MTL->MTLRQOMR = ETH_MTLRQOMR_RSF          /* Store and Forward */
                      | (7U << ETH_MTLRQOMR_RQS_SHIFT);  /* Queue size */
    
    /* Configure MAC - 100 Mbps, Full Duplex */
    ETH_MAC->MACCR = ETH_MACCR_FES                /* 100 Mbps */
                   | ETH_MACCR_DM;                /* Full duplex */
    
    /* ✏️ YOUR TURN: Enable TX and RX in MAC */
    ETH_MAC->MACCR |= ???;      /* HINT: Which bits enable transmit and receive? */
    
    /* Start DMA TX */
    ETH_DMA->DMACTCR |= ETH_DMACTCR_ST;
    
    /* Start DMA RX */
    ETH_DMA->DMACRCR |= ETH_DMACRCR_SR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * ETH_MAC->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 5: SENDING AND RECEIVING FRAMES
 *  ========================================
 * 
 * ============================================================================ */

/* Send an Ethernet frame */
uint8_t ETH_SendFrame(uint8_t *data, uint16_t length) {
    ETH_DMADescTypeDef *desc = &TxDescriptors[TxDescIdx];
    
    /* Check if descriptor is available (OWN bit = 0 means CPU owns it) */
    if (desc->DESC3 & ETH_TDES3_OWN) {
        return 0;   /* Still owned by DMA, can't send */
    }
    
    /* Copy data to TX buffer */
    memcpy(TxBuffer[TxDescIdx], data, length);
    
    /* Set buffer address */
    desc->DESC0 = (uint32_t)TxBuffer[TxDescIdx];
    desc->DESC1 = 0;
    
    /* Set frame length */
    desc->DESC2 = length & ETH_TDES2_B1L_MASK;
    
    /* Set control bits: OWN, First, Last, Checksum insertion */
    desc->DESC3 = ETH_TDES3_OWN | ETH_TDES3_FD | ETH_TDES3_LD 
                | ETH_TDES3_CIC_ALL
                | length;
    
    /* Move to next descriptor */
    TxDescIdx = (TxDescIdx + 1) % ETH_TX_DESC_CNT;
    
    /* Update tail pointer to trigger DMA */
    ETH_DMA->DMACTDTPR = (uint32_t)&TxDescriptors[TxDescIdx];
    
    return 1;   /* Success */
}

/* Check for received frame */
uint16_t ETH_ReceiveFrame(uint8_t *buffer, uint16_t max_length) {
    ETH_DMADescTypeDef *desc = &RxDescriptors[RxDescIdx];
    uint16_t length = 0;
    
    /* Check if descriptor has received data (OWN bit = 0 means CPU owns it) */
    if (desc->DESC3 & ETH_RDES3_OWN) {
        return 0;   /* No data yet */
    }
    
    /* Check for errors */
    if (desc->DESC3 & ETH_RDES3_ES) {
        /* Error - re-enable descriptor and move on */
        desc->DESC0 = (uint32_t)RxBuffer[RxDescIdx];
        desc->DESC3 = ETH_RDES3_OWN | ETH_RDES3_IOC | ETH_RDES3_BUF1V;
        RxDescIdx = (RxDescIdx + 1) % ETH_RX_DESC_CNT;
        return 0;
    }
    
    /* Get frame length */
    length = desc->DESC3 & ETH_RDES3_PL_MASK;
    if (length > max_length) length = max_length;
    
    /* Copy data from RX buffer */
    memcpy(buffer, RxBuffer[RxDescIdx], length);
    
    /* Re-enable descriptor for DMA */
    desc->DESC0 = (uint32_t)RxBuffer[RxDescIdx];
    desc->DESC3 = ETH_RDES3_OWN | ETH_RDES3_IOC | ETH_RDES3_BUF1V;
    
    /* Move to next descriptor */
    RxDescIdx = (RxDescIdx + 1) % ETH_RX_DESC_CNT;
    
    /* Update tail pointer */
    ETH_DMA->DMACRDTPR = (uint32_t)&RxDescriptors[RxDescIdx];
    
    return length;
}

/* ============================================================================
 * 
 *  BONUS: BUILD AN ETHERNET FRAME
 *  ================================
 * 
 * ============================================================================ */

/* Ethernet header */
typedef struct {
    uint8_t  dest_mac[6];
    uint8_t  src_mac[6];
    uint16_t ethertype;
} __attribute__((packed)) EthernetHeader;

/* Build a simple Ethernet frame */
uint16_t BuildEthernetFrame(uint8_t *frame, 
                             uint8_t *dest_mac, 
                             uint8_t *src_mac,
                             uint16_t ethertype,
                             uint8_t *payload,
                             uint16_t payload_len) {
    EthernetHeader *hdr = (EthernetHeader *)frame;
    
    /* Copy destination MAC */
    memcpy(hdr->dest_mac, dest_mac, 6);
    
    /* Copy source MAC */
    memcpy(hdr->src_mac, src_mac, 6);
    
    /* Set EtherType (big-endian!) */
    hdr->ethertype = ((ethertype >> 8) & 0xFF) | ((ethertype & 0xFF) << 8);
    
    /* Copy payload */
    memcpy(frame + sizeof(EthernetHeader), payload, payload_len);
    
    /* Pad if necessary (minimum 46 bytes payload) */
    uint16_t total_len = sizeof(EthernetHeader) + payload_len;
    if (total_len < 60) {
        memset(frame + total_len, 0, 60 - total_len);
        total_len = 60;
    }
    
    return total_len;
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
 *  MAIN PROGRAM - Initialize Ethernet and send test frame
 * 
 * ============================================================================ */

void delay(volatile uint32_t count) {
    while (count--);
}

/* Our MAC address (use a locally administered address) */
uint8_t MyMACAddress[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};

/* Broadcast MAC address */
uint8_t BroadcastMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int main(void)
{
    uint8_t frame[1518];
    uint8_t rxframe[1518];
    uint16_t len;
    
    /* Initialize Ethernet */
    ETH_EnableClocks();
    ETH_ConfigureGPIO();
    ETH_SelectRMII();
    
    /* Reset DMA */
    ETH_DMAReset();
    
    /* Initialize PHY */
    if (!ETH_InitPHY()) {
        /* PHY init failed - blink error LED */
        while(1);
    }
    
    /* Set our MAC address */
    ETH_SetMACAddress(MyMACAddress);
    
    /* Initialize descriptors */
    ETH_InitDescriptors();
    
    /* Start MAC */
    ETH_StartMAC();
    
    /* Build a test frame (broadcast) */
    uint8_t test_data[] = "Hello Ethernet!";
    len = BuildEthernetFrame(frame, 
                              BroadcastMAC, 
                              MyMACAddress,
                              0x0800,           /* IPv4 EtherType */
                              test_data, 
                              sizeof(test_data));
    
    for(;;) {
        /* Send test frame every second */
        ETH_SendFrame(frame, len);
        delay(10000000);
        
        /* Check for received frames */
        uint16_t rx_len = ETH_ReceiveFrame(rxframe, sizeof(rxframe));
        if (rx_len > 0) {
            /* Process received frame */
            /* In a real application, pass to TCP/IP stack */
        }
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned ETHERNET without HAL (the hardest peripheral!):
 *  
 *  ✅ Ethernet architecture (MAC, PHY, RMII)
 *  ✅ RMII GPIO configuration (9 pins)
 *  ✅ SYSCFG configuration for RMII mode
 *  ✅ DMA reset and initialization
 *  ✅ MDIO communication with PHY
 *  ✅ PHY reset and auto-negotiation
 *  ✅ DMA descriptor setup (TX and RX)
 *  ✅ MAC address configuration
 *  ✅ MAC/MTL configuration
 *  ✅ Sending Ethernet frames
 *  ✅ Receiving Ethernet frames
 *  
 *  NEXT STEPS:
 *  ────────────────────────────────────────────────────────────────
 *  This tutorial sends/receives raw Ethernet frames.
 *  For real networking, you need:
 *  
 *  1. TCP/IP Stack (lwIP is excellent)
 *     - ARP (Address Resolution Protocol)
 *     - IP (Internet Protocol)
 *     - ICMP (Ping)
 *     - UDP/TCP
 *     - DHCP
 *  
 *  2. Cache management (STM32H7 has D-cache!)
 *     - Place buffers in non-cached RAM, OR
 *     - Use cache invalidate/clean operations
 *  
 *  3. Interrupt handling
 *     - RX complete interrupt
 *     - TX complete interrupt
 *     - Error handling
 *  
 *  DEBUGGING TIPS:
 *  ────────────────────────────────────────────────────────────────
 *  • Check PHY ID reads correctly (0x0007 for LAN8742A)
 *  • Verify link LED on Nucleo board
 *  • Use Wireshark to see frames on network
 *  • Check descriptor OWN bits
 *  • Verify clock configuration (50 MHz for RMII)
 *  
 *  ⚠️ IMPORTANT: For production, use lwIP or similar stack!
 *     This tutorial is for LEARNING the hardware.
 * 
 * ============================================================================ */
