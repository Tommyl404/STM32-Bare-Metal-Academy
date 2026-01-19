# 🎓 STM32 Bare-Metal Programming Quiz

## 140 Multiple Choice Questions

**Instructions:** Choose the best answer for each question. Answers are at the end of the document.

---

# 📚 SECTION 1: RCC (Reset and Clock Control)

### Question 1
What is the default clock source for the STM32H753 after reset?

A) HSE (High-Speed External)  
B) PLL (Phase-Locked Loop)  
C) HSI (High-Speed Internal)  
D) LSE (Low-Speed External)

### Question 2
What is the frequency of the HSI oscillator on the STM32H753?

A) 8 MHz  
B) 16 MHz  
C) 32 MHz  
D) 64 MHz

### Question 3
Which bus do GPIO peripherals connect to on the STM32H7?

A) APB1  
B) APB2  
C) AHB1  
D) AHB4

### Question 4
What must you do before using any peripheral?

A) Configure interrupts  
B) Enable its clock in the RCC  
C) Set the system clock to maximum  
D) Disable low-power mode

### Question 5
What is the base address of the RCC peripheral on STM32H753?

A) 0x40020000  
B) 0x58024400  
C) 0x40023800  
D) 0x58020000

### Question 6
Why do we perform a dummy read after enabling a peripheral clock?

A) To verify the clock is running  
B) To reset the peripheral  
C) To ensure the clock enable has propagated before accessing the peripheral  
D) To check for errors

### Question 7
Which register is used to enable GPIO clocks on STM32H7?

A) RCC->APB1ENR  
B) RCC->AHB1ENR  
C) RCC->AHB4ENR  
D) RCC->APB2ENR

### Question 8
What is the purpose of the PLL?

A) To provide backup power  
B) To multiply/divide clock frequency for higher speeds  
C) To reduce power consumption  
D) To synchronize multiple MCUs

### Question 9
Which clock domain contains the CPU on STM32H7?

A) D1 (Domain 1)  
B) D2 (Domain 2)  
C) D3 (Domain 3)  
D) All domains equally

### Question 10
What happens if you access a peripheral without enabling its clock?

A) The peripheral works at reduced speed  
B) A hard fault occurs  
C) The read returns 0 and writes are ignored  
D) The system automatically enables the clock

---

# 📚 SECTION 2: GPIO (General Purpose Input/Output)

### Question 11
How many bits does each pin use in the MODER register?

A) 1 bit  
B) 2 bits  
C) 4 bits  
D) 8 bits

### Question 12
What is the MODER value for configuring a pin as OUTPUT?

A) 00  
B) 01  
C) 10  
D) 11

### Question 13
What is the formula to calculate the bit position for pin 7 in the MODER register?

A) 7 × 1 = 7  
B) 7 × 2 = 14  
C) 7 × 4 = 28  
D) 7 + 16 = 23

### Question 14
What is the advantage of using BSRR over ODR for setting/clearing pins?

A) BSRR is faster  
B) BSRR operations are atomic (no read-modify-write)  
C) BSRR uses less power  
D) BSRR can control more pins

### Question 15
Which register is used to READ the current state of input pins?

A) ODR  
B) IDR  
C) BSRR  
D) MODER

### Question 16
What does open-drain output mode mean?

A) The pin can only drive HIGH  
B) The pin can only drive LOW (needs external pull-up for HIGH)  
C) The pin is always floating  
D) The pin has no output capability

### Question 17
To reset (clear) pin 5 using BSRR, which bit should you set?

A) Bit 5  
B) Bit 21  
C) Bit 10  
D) Bit 16

### Question 18
What is the default GPIO mode after reset?

A) Output  
B) Input  
C) Alternate Function  
D) Analog

### Question 19
What is the PUPDR value for enabling internal pull-up?

A) 00  
B) 01  
C) 10  
D) 11

### Question 20
How many bits does each pin use in the AFR (Alternate Function Register)?

A) 1 bit  
B) 2 bits  
C) 4 bits  
D) 8 bits

---

# 📚 SECTION 3: UART (Universal Asynchronous Receiver/Transmitter)

### Question 21
How many wires does UART use for bidirectional communication?

A) 1  
B) 2  
C) 3  
D) 4

### Question 22
What does the BRR register control?

A) Buffer size  
B) Baud rate  
C) Parity bits  
D) Stop bits

### Question 23
Which flag indicates the transmit data register is empty?

A) RXNE  
B) TC  
C) TXE  
D) IDLE

### Question 24
What is a common baud rate for UART communication?

A) 1 MHz  
B) 115200  
C) 400 kHz  
D) 9600000

### Question 25
Which bit in CR1 enables the UART transmitter?

A) RE  
B) TE  
C) UE  
D) TXEIE

### Question 26
What must be disabled before changing UART configuration registers?

A) Interrupts  
B) DMA  
C) The UART peripheral (UE bit)  
D) The clock

### Question 27
What is the difference between TXE and TC flags?

A) They are identical  
B) TXE = buffer empty (can load next byte), TC = transmission complete  
C) TXE = transmission complete, TC = buffer empty  
D) TXE is for receive, TC is for transmit

### Question 28
What formula calculates the BRR value for a given baud rate?

A) BRR = baud_rate × clock  
B) BRR = clock / baud_rate  
C) BRR = baud_rate / clock  
D) BRR = clock - baud_rate

### Question 29
Which alternate function is typically used for USART3 on STM32H7?

A) AF4  
B) AF5  
C) AF7  
D) AF10

### Question 30
What does the RXNE flag indicate?

A) Receive error  
B) Receive buffer not empty (data available)  
C) Receiver enabled  
D) Receive complete

---

# 📚 SECTION 4: SPI (Serial Peripheral Interface)

### Question 31
How many wires does SPI typically use?

A) 2  
B) 3  
C) 4  
D) 5

### Question 32
What is the purpose of the CS (Chip Select) line?

A) Clock synchronization  
B) Selecting which slave device to communicate with  
C) Data validation  
D) Power control

### Question 33
SPI is what type of communication?

A) Half-duplex only  
B) Simplex only  
C) Full-duplex  
D) Multi-master only

### Question 34
What does MOSI stand for?

A) Master Out Slave In  
B) Master On Slave Interrupt  
C) Multiple Output Single Input  
D) Mode Of Serial Interface

### Question 35
Which flag indicates the transmit buffer is ready for data?

A) RXNE  
B) BSY  
C) TXP  
D) MODF

### Question 36
What is the typical idle state of CS (Chip Select)?

A) LOW  
B) HIGH  
C) Floating  
D) Alternating

### Question 37
Why do we use `*((volatile uint8_t*)&SPI1->RXDR)` instead of just `SPI1->RXDR`?

A) For faster access  
B) To read only 8 bits from the 32-bit register  
C) To enable interrupts  
D) To check for errors

### Question 38
What does the SPI master control?

A) Only the data  
B) The clock signal  
C) The power supply  
D) The slave addresses

### Question 39
What happens when you read from RXDR?

A) The buffer is cleared automatically  
B) Nothing special  
C) The next transfer starts  
D) An interrupt is generated

### Question 40
What is the purpose of the MBR (Master Baud Rate) bits in SPI_CFG1?

A) Set data size  
B) Set clock polarity  
C) Set clock divider (speed)  
D) Enable CRC

---

# 📚 SECTION 5: I2C (Inter-Integrated Circuit)

### Question 41
How many wires does I2C use?

A) 1  
B) 2  
C) 3  
D) 4

### Question 42
What type of output configuration do I2C pins require?

A) Push-pull  
B) Open-drain  
C) Analog  
D) High-impedance

### Question 43
Why does I2C need pull-up resistors?

A) To increase speed  
B) Because open-drain can only pull LOW, pull-ups provide the HIGH  
C) To filter noise  
D) To limit current

### Question 44
What is a typical I2C slave address length?

A) 4 bits  
B) 7 bits  
C) 10 bits  
D) 16 bits

### Question 45
What is a "repeated start" in I2C?

A) Restarting after an error  
B) A new START without a STOP (for read-after-write)  
C) Multiple byte transmission  
D) Clock stretching

### Question 46
What does the TIMINGR register control?

A) Slave address  
B) I2C clock speed and timing parameters  
C) Data buffer size  
D) Interrupt priorities

### Question 47
How is the slave address placed in CR2 for 7-bit addressing?

A) Bits 0-6  
B) Bits 1-7 (shifted left by 1)  
C) Bits 8-14  
D) The upper byte

### Question 48
What flag indicates receive data is available?

A) TXE  
B) RXNE  
C) TC  
D) BUSY

### Question 49
What is the purpose of AUTOEND mode?

A) Automatic error recovery  
B) Automatically generate STOP after the transfer  
C) Automatic addressing  
D) Auto-increment register address

### Question 50
What typical resistance value is used for I2C pull-ups?

A) 100Ω  
B) 1kΩ  
C) 4.7kΩ  
D) 100kΩ

---

# 📚 SECTION 6: Flash Memory

### Question 51
What must be done before writing to Flash memory?

A) Enable DMA  
B) Unlock the Flash with keys  
C) Disable interrupts  
D) Enter low-power mode

### Question 52
What are the Flash unlock keys for STM32?

A) 0x12345678 and 0x87654321  
B) 0x45670123 and 0xCDEF89AB  
C) 0xFFFFFFFF and 0x00000000  
D) 0xDEADBEEF and 0xCAFEBABE

### Question 53
Why must you erase Flash before writing?

A) To save power  
B) Flash can only change bits from 1→0, erase sets all bits to 1  
C) To verify the data  
D) For security reasons

### Question 54
What is the minimum erase unit for Flash?

A) 1 byte  
B) 1 word (32 bits)  
C) 1 sector  
D) 1 page

### Question 55
Which flag indicates a Flash operation is in progress?

A) BUSY  
B) BSY  
C) QW (Queue Wait)  
D) WRERR

### Question 56
What happens if you try to write to locked Flash?

A) The write succeeds anyway  
B) The write is ignored  
C) A hard fault occurs  
D) The system resets

### Question 57
What is the Flash write parallelism on STM32H7?

A) 8-bit  
B) 16-bit  
C) 32-bit  
D) 256-bit (flash word)

### Question 58
What should you do after completing Flash operations?

A) Reset the MCU  
B) Lock the Flash again for security  
C) Clear all registers  
D) Wait 1 second

### Question 59
Where is the Flash memory typically located in the address map?

A) 0x00000000  
B) 0x08000000  
C) 0x20000000  
D) 0x40000000

### Question 60
What is Option Bytes used for?

A) Extra data storage  
B) Storing temporary variables  
C) Configuration of security, boot mode, and protection  
D) DMA configuration

---

# 📚 SECTION 7: DMA (Direct Memory Access)

### Question 61
What is the main purpose of DMA?

A) Faster CPU execution  
B) Transfer data without CPU intervention  
C) Increase memory size  
D) Provide backup storage

### Question 62
What register holds the source address for DMA?

A) NDTR  
B) PAR (Peripheral Address Register)  
C) M0AR (Memory 0 Address Register)  
D) CR

### Question 63
What does NDTR specify?

A) DMA channel number  
B) Number of data items to transfer  
C) Next DMA transfer request  
D) Node descriptor table register

### Question 64
What is "circular mode" in DMA?

A) Data is encrypted in a loop  
B) Transfer restarts automatically when complete  
C) DMA moves in a circular buffer pattern  
D) Multiple channels work together

### Question 65
Which flag indicates DMA transfer is complete?

A) HTIF (Half Transfer)  
B) TCIF (Transfer Complete)  
C) TEIF (Transfer Error)  
D) BUSY

### Question 66
What is the advantage of DMA over interrupt-driven transfers?

A) DMA is simpler to program  
B) DMA frees the CPU to do other tasks during transfer  
C) DMA is more accurate  
D) DMA uses less power

### Question 67
What does MINC (Memory Increment) mode do?

A) Increases transfer speed  
B) Automatically increments the memory address after each transfer  
C) Uses minimum memory  
D) Increments the DMA channel number

### Question 68
Which peripheral commonly uses DMA for efficiency?

A) GPIO  
B) RCC  
C) ADC (for continuous conversions)  
D) EXTI

### Question 69
What must be enabled in the peripheral to trigger DMA requests?

A) The clock only  
B) The DMA request enable bit in the peripheral  
C) Interrupts  
D) Low-power mode

### Question 70
What happens if a DMA transfer error occurs?

A) Transfer continues with errors  
B) TEIF flag is set and transfer stops  
C) System resets  
D) DMA automatically retries

---

# 📚 SECTION 8: EXTI (External Interrupt)

### Question 71
What is the purpose of EXTI?

A) Extended interrupt timing  
B) Detect external events (rising/falling edges) on GPIO pins  
C) External memory interface  
D) Exception handling

### Question 72
Which register selects rising edge trigger?

A) FTSR (Falling Trigger Selection Register)  
B) RTSR (Rising Trigger Selection Register)  
C) IMR (Interrupt Mask Register)  
D) PR (Pending Register)

### Question 73
Which register enables the interrupt for an EXTI line?

A) RTSR  
B) FTSR  
C) IMR (or CPUIMR on H7)  
D) SWIER

### Question 74
How do you clear a pending EXTI interrupt?

A) Read the PR register  
B) Write 1 to the corresponding bit in PR (or CPUPR)  
C) Disable the EXTI line  
D) Reset the MCU

### Question 75
What register maps GPIO pins to EXTI lines on STM32H7?

A) EXTI->IMR  
B) SYSCFG->EXTICR  
C) GPIO->EXTI  
D) RCC->EXTI

### Question 76
Can both rising and falling edges trigger the same EXTI line?

A) No, only one at a time  
B) Yes, by setting both RTSR and FTSR bits  
C) Only on specific lines  
D) Only with DMA

### Question 77
What is the SWIER register used for?

A) Software interrupt enable  
B) Generating a software-triggered interrupt on an EXTI line  
C) Switch interrupt routing  
D) Setting watchdog interrupt

### Question 78
Which EXTI line is typically connected to the RTC alarm?

A) EXTI0  
B) EXTI13  
C) EXTI17  
D) EXTI22

### Question 79
What happens if you don't clear the pending bit in an EXTI ISR?

A) Nothing, it clears automatically  
B) The interrupt keeps firing repeatedly  
C) The system crashes  
D) The next interrupt is missed

### Question 80
How many EXTI lines are available on STM32H7?

A) 16  
B) 23  
C) 44 or more  
D) 8

---

# 📚 SECTION 9: NVIC (Nested Vectored Interrupt Controller)

### Question 81
What is the purpose of NVIC?

A) Network vector interface controller  
B) Manage interrupt priorities and enable/disable interrupts  
C) Non-volatile memory control  
D) Noise vector input control

### Question 82
Which register enables an interrupt in NVIC?

A) NVIC->ICER  
B) NVIC->ISER  
C) NVIC->IPR  
D) NVIC->ICPR

### Question 83
What does a lower priority NUMBER mean in Cortex-M?

A) Lower urgency (less important)  
B) Higher urgency (more important)  
C) Equal to all others  
D) Disabled interrupt

### Question 84
What is interrupt nesting?

A) Putting interrupts in order  
B) A higher priority interrupt can preempt a lower priority one  
C) Multiple interrupts sharing the same handler  
D) Disabling all interrupts

### Question 85
How many priority bits does the STM32H7 implement?

A) 2 bits (4 levels)  
B) 4 bits (16 levels)  
C) 8 bits (256 levels)  
D) 3 bits (8 levels)

### Question 86
What does `NVIC_SetPriority(IRQn, priority)` do?

A) Enables the interrupt  
B) Sets the priority level for the specified interrupt  
C) Clears pending interrupt  
D) Disables the interrupt

### Question 87
Which function disables all interrupts globally?

A) NVIC_DisableIRQ()  
B) __disable_irq()  
C) NVIC_ClearPendingIRQ()  
D) __WFI()

### Question 88
What is the purpose of ICPR (Interrupt Clear Pending Register)?

A) Enable interrupts  
B) Clear a pending interrupt without executing the handler  
C) Increase priority  
D) Configure interrupt routing

### Question 89
Where is the interrupt vector table located by default?

A) 0x08000000 (start of Flash)  
B) 0x20000000 (start of RAM)  
C) 0x40000000 (peripheral region)  
D) 0xE0000000 (system region)

### Question 90
What is the IRQ number for EXTI0 on STM32H7?

A) 0  
B) 6  
C) 16  
D) 23

---

# 📚 SECTION 10: ADC (Analog-to-Digital Converter)

### Question 91
What does an ADC do?

A) Converts digital signals to analog  
B) Converts analog voltages to digital numbers  
C) Amplifies signals  
D) Filters noise

### Question 92
What is the resolution of the STM32H7 ADC?

A) 8-bit  
B) 10-bit  
C) 12-bit (default), up to 16-bit  
D) 24-bit

### Question 93
What does the ADC sample?

A) Current  
B) Resistance  
C) Voltage  
D) Power

### Question 94
What must GPIO pins be configured as for ADC input?

A) Input  
B) Output  
C) Alternate Function  
D) Analog mode

### Question 95
What flag indicates ADC conversion is complete?

A) ADRDY  
B) EOC (End of Conversion)  
C) OVR  
D) STRT

### Question 96
What is the purpose of the ADC sequencer?

A) To order ADC channels  
B) To convert multiple channels in sequence automatically  
C) To synchronize with DAC  
D) To increase resolution

### Question 97
What does the SMPR register control?

A) Sample mode  
B) Sampling time for each channel  
C) Sample polarity  
D) Sequence order

### Question 98
What is ADC calibration used for?

A) Setting the reference voltage  
B) Compensating for manufacturing variations to improve accuracy  
C) Calibrating external sensors  
D) Setting the clock speed

### Question 99
What voltage range does the STM32 ADC typically measure?

A) 0V to 5V  
B) 0V to VREF+ (typically 3.3V)  
C) -3.3V to +3.3V  
D) 0V to 12V

### Question 100
What is the formula to convert ADC reading to voltage (12-bit)?

A) Voltage = ADC_Value × 3.3  
B) Voltage = ADC_Value / 4095 × VREF  
C) Voltage = ADC_Value × 4095 / VREF  
D) Voltage = 3.3 / ADC_Value

---

# 📚 SECTION 11: DAC (Digital-to-Analog Converter)

### Question 101
What does a DAC do?

A) Converts analog to digital  
B) Converts digital numbers to analog voltages  
C) Detects analog signals  
D) Amplifies digital signals

### Question 102
How many DAC channels does the STM32H753 have?

A) 1  
B) 2  
C) 4  
D) 8

### Question 103
What is the resolution of the STM32 DAC?

A) 8-bit  
B) 10-bit  
C) 12-bit  
D) 16-bit

### Question 104
Which pin is typically DAC1 output on STM32?

A) PA0  
B) PA4  
C) PB0  
D) PC0

### Question 105
What is the output voltage range of the DAC?

A) 0 to 5V  
B) 0 to VREF (typically 3.3V)  
C) -3.3V to +3.3V  
D) 0 to 1.8V

### Question 106
What is the formula for DAC output voltage (12-bit)?

A) Vout = Value / 4096 × VREF  
B) Vout = Value × 4096  
C) Vout = VREF / Value  
D) Vout = Value × VREF

### Question 107
What is DAC "wave generation" mode used for?

A) Detecting waves  
B) Generating noise or triangle waves automatically  
C) Analyzing waveforms  
D) Filtering signals

### Question 108
Which register holds the data to output on DAC channel 1?

A) DAC_CR  
B) DAC_DHR12R1 (Data Holding Register)  
C) DAC_DOR1  
D) DAC_SR

### Question 109
Why might you use a buffer on the DAC output?

A) To store more data  
B) To provide higher output current drive capability  
C) To increase resolution  
D) To reduce noise

### Question 110
What triggers a DAC conversion in software mode?

A) Timer  
B) Writing to the data register triggers output update  
C) External pin  
D) ADC complete

---

# 📚 SECTION 12: Timers (TIM)

### Question 111
What is the primary purpose of a timer?

A) Display time  
B) Generate precise time delays, PWM, and count events  
C) Store timestamps  
D) Synchronize clocks

### Question 112
What does the PSC (Prescaler) register do?

A) Preset counter value  
B) Divide the timer clock frequency  
C) Set PWM duty cycle  
D) Configure capture/compare

### Question 113
What does the ARR (Auto-Reload Register) determine?

A) The timer clock source  
B) When the counter resets (period)  
C) The prescaler value  
D) Interrupt priority

### Question 114
What is PWM (Pulse Width Modulation)?

A) Power width measurement  
B) A signal with variable duty cycle for analog-like control  
C) Pulse wave memory  
D) Precision waveform mode

### Question 115
Which register sets the PWM duty cycle?

A) ARR  
B) PSC  
C) CCR (Capture/Compare Register)  
D) CNT

### Question 116
What formula calculates timer overflow frequency?

A) f = Timer_Clock × PSC × ARR  
B) f = Timer_Clock / ((PSC + 1) × (ARR + 1))  
C) f = (PSC + 1) / Timer_Clock  
D) f = ARR / Timer_Clock

### Question 117
What is "input capture" mode?

A) Capturing external power  
B) Recording the timer count when an external event occurs  
C) Capturing input voltage  
D) Reading input pins

### Question 118
What is a "one-pulse mode"?

A) Single PWM pulse  
B) Timer generates a single pulse and stops  
C) One clock pulse per second  
D) Mono-channel output

### Question 119
Which timer is typically used for microsecond delays?

A) RTC  
B) SysTick  
C) General-purpose timer (TIM2-TIM5)  
D) Watchdog

### Question 120
What flag indicates timer counter overflow (update event)?

A) CC1IF  
B) UIF (Update Interrupt Flag)  
C) TIF  
D) COMIF

---

# 📚 SECTION 13: Watchdog (IWDG/WWDG)

### Question 121
What is the purpose of a watchdog timer?

A) Guard against unauthorized access  
B) Reset the system if software hangs or crashes  
C) Watch for external events  
D) Monitor power supply

### Question 122
What must software do to prevent watchdog reset?

A) Disable interrupts  
B) Refresh (kick/feed) the watchdog before timeout  
C) Write to Flash  
D) Enter low-power mode

### Question 123
What is the difference between IWDG and WWDG?

A) IWDG is faster  
B) IWDG is independent (LSI clock), WWDG uses system clock with window  
C) IWDG is for input, WWDG for output  
D) There is no difference

### Question 124
Can the IWDG be disabled once started?

A) Yes, by writing to the control register  
B) No, once started it cannot be stopped  
C) Yes, by entering low-power mode  
D) Yes, after a reset

### Question 125
What is the "window" in WWDG?

A) A visual display  
B) A time range where refresh is valid (too early = reset)  
C) Memory window  
D) Debug window

### Question 126
What clock source does IWDG use?

A) HSI  
B) HSE  
C) LSI (Low-Speed Internal, ~32 kHz)  
D) System clock

### Question 127
What is the unlock key for IWDG?

A) 0x0000  
B) 0xAAAA  
C) 0x5555  
D) 0xCCCC

### Question 128
What value refreshes (kicks) the IWDG?

A) 0x0000  
B) 0xFFFF  
C) 0xAAAA  
D) 0xCCCC

### Question 129
What happens if WWDG refresh occurs too early?

A) Nothing  
B) System reset (outside the window)  
C) Watchdog disables  
D) Counter pauses

### Question 130
When should you use IWDG vs WWDG?

A) IWDG for precise timing, WWDG for rough timing  
B) IWDG for independence from main clock failures, WWDG for detecting timing errors  
C) They are interchangeable  
D) IWDG for debug, WWDG for production

---

# 📚 SECTION 14: RTC (Real-Time Clock)

### Question 131
What is the purpose of the RTC?

A) Real-time calculations  
B) Keep track of time/date even in low-power modes  
C) Record execution time  
D) Generate random numbers

### Question 132
What power domain does the RTC belong to?

A) VDD domain  
B) Backup domain (VBAT)  
C) Core domain  
D) Analog domain

### Question 133
What clock source is typically used for RTC?

A) HSI  
B) HSE  
C) LSE (32.768 kHz crystal)  
D) PLL

### Question 134
Why is 32.768 kHz used for RTC?

A) It's cheap  
B) 32768 = 2^15, easy to divide to 1 Hz  
C) It's the most accurate  
D) Required by USB

### Question 135
What must be done before configuring RTC registers?

A) Enter low-power mode  
B) Disable backup domain write protection  
C) Enable DMA  
D) Configure GPIO

### Question 136
What does the PRER register configure?

A) RTC priority  
B) Prescaler to generate 1 Hz clock from 32.768 kHz  
C) Previous value backup  
D) RTC precision

### Question 137
What is an RTC alarm used for?

A) Wake up from a crash  
B) Generate interrupt at a specific time  
C) Sound an external alarm  
D) Record alarm events

### Question 138
Which register holds the current time in BCD format?

A) RTC_DR  
B) RTC_TR  
C) RTC_PRER  
D) RTC_CR

### Question 139
What is BCD format?

A) Binary Coded Data  
B) Each decimal digit stored as 4-bit binary (Binary Coded Decimal)  
C) Block Cipher Data  
D) Base Clock Division

### Question 140
What happens to RTC when main power is lost but VBAT is connected?

A) RTC stops  
B) RTC continues running from VBAT  
C) RTC resets to 0  
D) RTC enters error state

---

# ✅ ANSWER KEY

| Q# | Answer | Q# | Answer | Q# | Answer | Q# | Answer |
|----|--------|----|---------|----|--------|----|--------|
| 1  | C | 36 | B | 71 | B | 106 | A |
| 2  | D | 37 | B | 72 | B | 107 | B |
| 3  | D | 38 | B | 73 | C | 108 | B |
| 4  | B | 39 | A | 74 | B | 109 | B |
| 5  | B | 40 | C | 75 | B | 110 | B |
| 6  | C | 41 | B | 76 | B | 111 | B |
| 7  | C | 42 | B | 77 | B | 112 | B |
| 8  | B | 43 | B | 78 | C | 113 | B |
| 9  | A | 44 | B | 79 | B | 114 | B |
| 10 | C | 45 | B | 80 | C | 115 | C |
| 11 | B | 46 | B | 81 | B | 116 | B |
| 12 | B | 47 | B | 82 | B | 117 | B |
| 13 | B | 48 | B | 83 | B | 118 | B |
| 14 | B | 49 | B | 84 | B | 119 | C |
| 15 | B | 50 | C | 85 | B | 120 | B |
| 16 | B | 51 | B | 86 | B | 121 | B |
| 17 | B | 52 | B | 87 | B | 122 | B |
| 18 | B | 53 | B | 88 | B | 123 | B |
| 19 | B | 54 | C | 89 | A | 124 | B |
| 20 | C | 55 | C | 90 | B | 125 | B |
| 21 | B | 56 | B | 91 | B | 126 | C |
| 22 | B | 57 | D | 92 | C | 127 | C |
| 23 | C | 58 | B | 93 | C | 128 | C |
| 24 | B | 59 | B | 94 | D | 129 | B |
| 25 | B | 60 | C | 95 | B | 130 | B |
| 26 | C | 61 | B | 96 | B | 131 | B |
| 27 | B | 62 | B | 97 | B | 132 | B |
| 28 | B | 63 | B | 98 | B | 133 | C |
| 29 | C | 64 | B | 99 | B | 134 | B |
| 30 | B | 65 | B | 100 | B | 135 | B |
| 31 | C | 66 | B | 101 | B | 136 | B |
| 32 | B | 67 | B | 102 | B | 137 | B |
| 33 | C | 68 | C | 103 | C | 138 | B |
| 34 | A | 69 | B | 104 | B | 139 | B |
| 35 | C | 70 | B | 105 | B | 140 | B |

---

## 📊 Score Guide

| Score | Level |
|-------|-------|
| 126-140 (90%+) | 🏆 Expert - Ready for professional embedded development! |
| 112-125 (80-89%) | 🥇 Advanced - Strong understanding, minor review needed |
| 98-111 (70-79%) | 🥈 Intermediate - Good foundation, review weak areas |
| 84-97 (60-69%) | 🥉 Beginner - More practice with tutorials recommended |
| Below 84 | 📚 Keep studying! Re-read the tutorials and try again |

---

**Good luck! 🍀**
