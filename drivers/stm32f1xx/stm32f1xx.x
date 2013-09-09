define show_flash
set $FLASH_BASE_ADDRESS=0x40022000
printf "FLASH Registers\n"
printf "===============\n"
printf "ACR: %X   KEYR: %X   OPTKEYR: %X\n", *(volatile uint32_t*)($FLASH_BASE_ADDRESS), *(volatile uint32_t*)($FLASH_BASE_ADDRESS + 4), *(volatile uint32_t*)($FLASH_BASE_ADDRESS + 8)
printf "SR: %X   CR: %X   AR: %X\n", *(volatile uint32_t*)($FLASH_BASE_ADDRESS + 12), *(volatile uint32_t*)($FLASH_BASE_ADDRESS + 16), *(volatile uint32_t*)($FLASH_BASE_ADDRESS + 20)
printf "OBR: %X   WRPR: %X\n", *(volatile uint32_t*)($FLASH_BASE_ADDRESS + 28), *(volatile uint32_t*)($FLASH_BASE_ADDRESS + 32)
end
document show_flash
Show FLASH registers
end

define show_rcc
printf "RCC Registers\n"
printf "=============\n"
printf "CR: %X   CFGR: %X   CIR: %X\n", *(volatile uint32_t*)0x40021000, *(volatile uint32_t*)0x40021004, *(volatile uint32_t*)0x40021008
printf "APB2RSTR: %X   APB1RSTR: %X\n", *(volatile uint32_t*)0x4002100C, *(volatile uint32_t*)0x40021010
printf "AHBENR: %X  APB2ENR: %X   APB1ENR: %X\n", *(volatile uint32_t*)0x40021014, *(volatile uint32_t*)0x40021018, *(volatile uint32_t*)0x4002101C
printf "BDCR: %X   CSR: %X\n", *(volatile uint32_t*)0x40021020, *(volatile uint32_t*)0x40021024
end
document show_rcc
Show RCC registers
end

define show_i2c
printf "I2C Registers\n"
printf "=============\n"
set $I2C_BASE_ADDRESS = 0x40005400 + 0x400 * ($arg0 - 1)
printf "CR1: %X   CR2: %X   OAR1: %X   OAR2: %X\n", *(volatile uint32_t*)($I2C_BASE_ADDRESS), *(volatile uint32_t*)($I2C_BASE_ADDRESS + 4), *(volatile uint32_t*)($I2C_BASE_ADDRESS + 8), *(volatile uint32_t*)($I2C_BASE_ADDRESS + 12)
printf "DR: %X   SR1: %X   SR2: %X\n", *(volatile uint32_t*)($I2C_BASE_ADDRESS + 16), *(volatile uint32_t*)($I2C_BASE_ADDRESS + 20), *(volatile uint32_t*)($I2C_BASE_ADDRESS + 24)
printf "CCR: %X   TRISE: %X\n", *(volatile uint32_t*)($I2C_BASE_ADDRESS + 28), *(volatile uint32_t*)($I2C_BASE_ADDRESS + 32)
end
document show_i2c
Show I2C registers
Argument 1-2
end

define show_sdio
printf "SDIO Registers\n"
printf "==============\n"
printf "POWER: %X   CLKCR: %X   ARG: %X   CMD: %X\n", *(volatile uint32_t*)0x40018000, *(volatile uint32_t*)0x40018004, *(volatile uint32_t*)0x40018008, *(volatile uint32_t*)0x4001800C
printf "RESP1: %X   RESP2: %X   RESP3: %X   RESP4: %X\n", *(volatile uint32_t*)0x40018014, *(volatile uint32_t*)0x40018018, *(volatile uint32_t*)0x4001801C, *(volatile uint32_t*)0x40018020
printf "DTIMER: %X   DLEN: %X   DCTRL: %X   DCOUNT: %X\n", *(volatile uint32_t*)0x40018024, *(volatile uint32_t*)0x40018028, *(volatile uint32_t*)0x4001802C, *(volatile uint32_t*)0x40018030
printf "STA: %X   ICR: %X   MASK: %X   RESPCMD: %X\n", *(volatile uint32_t*)0x40018034, *(volatile uint32_t*)0x40018038, *(volatile uint32_t*)0x4001803C, *(volatile uint32_t*)0x40018010
printf "FIFOCNT: %X\n", *(volatile uint32_t*)0x40018048
end
document show_sdio
Show SDIO registers
end

define show_gpio
set $GPIO_BASE_ADDRESS=0x40010800 + 0x400 * $arg0
printf "GPIO Registers\n"
printf "==============\n"
printf "CRL: %X   CRH: %X\n", *(volatile uint32_t*)($GPIO_BASE_ADDRESS), *(volatile uint32_t*)($GPIO_BASE_ADDRESS + 4)
printf "IDR: %X   ODR: %X\n", *(volatile uint32_t*)($GPIO_BASE_ADDRESS + 8), *(volatile uint32_t*)($GPIO_BASE_ADDRESS + 12)
printf "LCKR: %X\n", *(volatile uint32_t*)($GPIO_BASE_ADDRESS + 24)
end
document show_gpio
Show GPIO registers
Argument 0->A .. 6->G
end

define show_dma
if (($arg0 == 1) && ($arg1 > 0) && ($arg1 < 8)) || (($arg0 == 2) && ($arg1 > 0) && ($arg1 < 6))
	set $DMA_BASE_ADDRESS=0x40020000 + 0x400 * ($arg0 - 1)
	set $DMA_CHANNEL_BASE_ADDRESS=$DMA_BASE_ADDRESS + 0x8 + 0x14 * ($arg1 - 1)
	printf "DMA%d channel %d registers\n", $arg0, $arg1
	printf "========================\n"
	printf "DMA_ISR:   %8X   DMA_IFCR:   %8X\n", *(volatile uint32_t*)($DMA_BASE_ADDRESS), *(volatile uint32_t*)($DMA_BASE_ADDRESS + 4)
	printf "DMA_CCR%d:  %8X   DMA_CNDTR%d: %8X\n", $arg1, *(volatile uint32_t*)($DMA_CHANNEL_BASE_ADDRESS), $arg1, *(volatile uint32_t*)($DMA_CHANNEL_BASE_ADDRESS + 4)
	printf "DMA_CPAR%d: %8X   DMA_CMAR%d:  %8X\n", $arg1, *(volatile uint32_t*)($DMA_CHANNEL_BASE_ADDRESS + 8), $arg1, *(volatile uint32_t*)($DMA_CHANNEL_BASE_ADDRESS + 12)
else
	printf "/!\\ Wrong parameters, type \"help show_dma\"\n"
end
end
document show_dma
Show DMA registers
First argument: DMA index:  1 or 2
Second argument: DMA channel: 1 to 7 for DMA1, 1 to 5 for DMA2
end

define show_usb
set $USB_BASE_ADDRESS=0x40005C00
printf "USB Registers\n"
printf "=============\n"
printf "USB_CNTR: %8X    USB_ISTR: %8X    USB_FNR: %8X\n", *(volatile uint32_t*)($USB_BASE_ADDRESS + 0X40),  *(volatile uint32_t*)($USB_BASE_ADDRESS + 0X44),  *(volatile uint32_t*)($USB_BASE_ADDRESS + 0X48)
printf "DADDR: %8X    USB_BTABLE: %8X\n", *(volatile uint32_t*)($USB_BASE_ADDRESS + 0X4C),  *(volatile uint32_t*)($USB_BASE_ADDRESS + 0X50)
end
document show_usb
Show USB registers
end

define show_usb_endpoints
set $btable =  0x40006000 + *(volatile uint32_t*)($USB_BASE_ADDRESS + 0X50)
set $cpt = 0
while $cpt < 8
	printf "->Endpoint #%d\n", $cpt
	printf "\tEP%dR: %8X\n", $cpt,  *(volatile uint32_t*)($USB_BASE_ADDRESS + 4 * $cpt)
	printf "\tUSB_ADDR%d_TX:  %8X\n", $cpt, *(volatile uint32_t*)($btable + 16 * $cpt)
	printf "\tUSB_COUNT%d_TX: %8X\n", $cpt, *(volatile uint32_t*)($btable + 16 * $cpt + 4)
	printf "\tUSB_ADDR%d_RX:  %8X\n", $cpt, *(volatile uint32_t*)($btable + 16 * $cpt + 8)
	printf "\tUSB_COUNT%d_RX: %8X\n", $cpt, *(volatile uint32_t*)($btable + 16 * $cpt + 12)
	set $cpt = $cpt + 1
end
end
document show_usb_endpoints
Show USB endpoints registers
end
