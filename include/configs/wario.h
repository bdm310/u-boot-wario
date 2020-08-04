/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2013 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Freescale i.MX6SL EVK board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(3 * SZ_1M)

#ifndef CONFIG_SPL_BUILD
#define BOOT_TARGET_DEVICES(func) \
        func(MMC, mmc, 1) 
#include <config_distro_bootcmd.h>
#endif


#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=ttymxc0\0" \
	"fdt_addr=${fdtcontroladdr}\0" \
	"ramdisk_addr_r=0x88300000\0" \
    "kernel_addr_r=0x82000000\0" \
	"pxefile_addr_r=0x88100000\0" \
	"scriptaddr=0x88200000\0" \
	BOOTENV

/* Miscellaneous configurable options */

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Environment organization */

/* USB Configs */
#ifdef CONFIG_CMD_USB
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS		0
#define CONFIG_USB_MAX_CONTROLLER_COUNT	2
#endif

#define CONFIG_SYS_FSL_USDHC_NUM	3
#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_SYS_MMC_ENV_DEV		1	/* SDHC2*/
#endif

#ifdef CONFIG_SPL_BUILD

#include "imx6_spl.h"

/* 
 * Have to undef the SPL memory parameters because we need
 * functional bss and malloc before DDR is initialized to
 * read the board ID info from eMMC and thus have to locate
 * these blocks in OCRAM
 */
#undef CONFIG_SPL_BSS_START_ADDR
#undef CONFIG_SPL_BSS_MAX_SIZE
#undef CONFIG_SYS_SPL_MALLOC_START
#undef CONFIG_SYS_SPL_MALLOC_SIZE
#define CONFIG_SPL_BSS_START_ADDR      0x907000
#define CONFIG_SPL_BSS_MAX_SIZE        0x1000
#define CONFIG_SYS_SPL_MALLOC_START    0x900000
#define CONFIG_SYS_SPL_MALLOC_SIZE     0x7000

#define CONFIG_MXC_UART_BASE	UART1_BASE
#define CONSOLE_DEV		"ttymxc0"

#define CONFIG_SYS_FSL_ESDHC_ADDR	USDHC2_BASE_ADDR
#define CONFIG_MMC_BROKEN_CD

#endif

#endif				/* __CONFIG_H */
