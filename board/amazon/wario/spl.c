// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2015-2019 Variscite Ltd.
 * Copyright (C) 2019 Parthiban Nallathambi <parthitce@gmail.com>
 */

#include <common.h>
#include <init.h>
#include <spl.h>
#include <mmc.h>
#include <asm/arch/clock.h>
#include <asm/io.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch-mx6/imx-regs.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/crm_regs.h>
#include <fsl_esdhc_imx.h>
#include <malloc.h>
#include "idme.h"
#include "wario_board_id.h"

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_22K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_RST_CTRL (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_UART1_TXD__UART1_TXD | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART1_RXD__UART1_RXD | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_SD2_CLK__USDHC2_CLK	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_CMD__USDHC2_CMD	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT0__USDHC2_DAT0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT1__USDHC2_DAT1	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT2__USDHC2_DAT2	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT3__USDHC2_DAT3	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT4__USDHC2_DAT4	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT5__USDHC2_DAT5	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT6__USDHC2_DAT6	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT7__USDHC2_DAT7	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_RST__USDHC2_RST	| MUX_PAD_CTRL(USDHC_RST_CTRL),
};

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

static void setup_ccm(void)
{
	writel(0xffffffff, CCM_BASE_ADDR + 0x68);
	writel(0xffffffff, CCM_BASE_ADDR + 0x6c);
	writel(0xffffffff, CCM_BASE_ADDR + 0x70);
	writel(0xffffffff, CCM_BASE_ADDR + 0x74);
	writel(0xffffffff, CCM_BASE_ADDR + 0x78);
	writel(0xffffffff, CCM_BASE_ADDR + 0x7c);
	writel(0xffffffff, CCM_BASE_ADDR + 0x80);
}

#define IOMUXC_DDR_SEL_DDR2 0x80000
#define IOMUXC_PUE          0x02000
#define IOMUXC_PKE          0x01000
#define IOMUXC_DSE_48       (0x5 << 3)
#define IOMUXC_DSE_60       (0x4 << 3)

#define BOARD_DRAM_HAS_DS_48(id)	(BOARD_IS_BOURBON(id) || \
									BOARD_IS_MUSCAT_WAN(id) || \
									BOARD_IS_MUSCAT_WFO(id) || \
									BOARD_IS_WHISKY_WAN(id) || \
									BOARD_IS_WHISKY_WFO(id))

void lpddr2_init(const struct board_type *board) {
	unsigned long common_dse; 
	/* 
	 * common_dse is pad control bits 5-3 for most high speed DRAM lines, 
	 * 5=48 Ohm, 4=60 Ohm. See IMX6SLRM IOMUXC chapter.
	 */
	if (BOARD_DRAM_HAS_DS_48(board->id)) {
		common_dse = IOMUXC_DSE_48;
	} else {
		common_dse = IOMUXC_DSE_60;
	}

	/*
	* CCM Configuration
	*/
	/* CCM_CBCDR (CCM Bus Clock Divider Register) */
	writel(0x00260324, (CCM_BASE_ADDR + 0x18));

	/*
	* IOMUX Configuration
	*/
	/* IOMUXC_SW_PAD_CTRL_GRP_DDRMODE  Differential input mode on data lines */
	writel(0x00020000, (IOMUXC_BASE_ADDR + 0x5c0));
	/* IOMUXC_SW_PAD_CTRL_GRP_DDRPKE Disable Pull/Keeper on DDR pads */
	writel(0x00000000, (IOMUXC_BASE_ADDR + 0x5b4));

	/* CLOCK */
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_SDCLK0_P */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x338));

	/* CONTROL */
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_CAS_B common drive strength */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x300));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_RAS_B common drive strength */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x31c));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_RESET DDR_SEL=DDR2 and common drive strength */
	writel(IOMUXC_DDR_SEL_DDR2 | common_dse, (IOMUXC_BASE_ADDR + 0x320));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_SDCKE0 60 Ohm drive strength */
	writel(0x00000020, (IOMUXC_BASE_ADDR + 0x330));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_SDCKE1 30 Ohm drive strength */
	writel(0x00000020, (IOMUXC_BASE_ADDR + 0x334));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_SDBA2 Disabled drive strength */
	writel(0x00000000, (IOMUXC_BASE_ADDR + 0x32c));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_ODT0 240 Ohm drive strength */
	writel(0x00000008, (IOMUXC_BASE_ADDR + 0x33c));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_OTD1 240 Ohm drive strength */
	writel(0x00000008, (IOMUXC_BASE_ADDR + 0x340));
	/* IOMUXC_SW_PAD_CTRL_GRP_ADDDS common drive strength on DRAM address lines */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x5ac));
	/* IOMUXC_SW_PAD_CTRL_GRP_CTLDS common drive strength on CS, SDBA, CKE, and SDWE pads */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x5c8));

	/* DATA STROBES */
	/* IOMUXC_SW_PAD_CTRL_GRP_DDRMODE_CTL Differential input mode on DQ lines */
	writel(0x00020000, (IOMUXC_BASE_ADDR + 0x5b0));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_SDQS0_P pull, enable pull/keep, common drive strength */
	writel(IOMUXC_PKE | IOMUXC_PUE | common_dse, (IOMUXC_BASE_ADDR + 0x344));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_SDQS1_P pull, enable pull/keep, common drive strength */
	writel(IOMUXC_PKE | IOMUXC_PUE | common_dse, (IOMUXC_BASE_ADDR + 0x348));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_SDQS2_P pull, enable pull/keep, common drive strength */
	writel(IOMUXC_PKE | IOMUXC_PUE | common_dse, (IOMUXC_BASE_ADDR + 0x34c));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_SDQS3_P pull, enable pull/keep, common drive strength */
	writel(IOMUXC_PKE | IOMUXC_PUE | common_dse, (IOMUXC_BASE_ADDR + 0x350));

	/* DATA */
	/* IOMUXC_SW_PAD_CTRL_GRP_DDR_TYPE DDR mode LPDDR2 */
	writel(0x00080000, (IOMUXC_BASE_ADDR + 0x5d0));
	/* IOMUXC_SW_PAD_CTRL_GRP_B0DS common drive strength for DRAM_DATA01-DRAM_DATA07 */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x5c4));
	/* IOMUXC_SW_PAD_CTRL_GRP_B1DS common drive strength for DRAM_DATA08-DRAM_DATA15 */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x5cc));
	/* IOMUXC_SW_PAD_CTRL_GRP_B2DS common drive strength for DRAM_DATA16-DRAM_DATA23 */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x5d4));
	/* IOMUXC_SW_PAD_CTRL_GRP_B3DS common drive strength for DRAM_DATA24-DRAM_DATA31 */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x5d8));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_DQM0 common drive strength */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x30c));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_DQM1 common drive strength */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x310));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_DQM2 common drive strength */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x314));
	/* IOMUXC_SW_PAD_CTRL_PAD_DRAM_DQM3 common drive strength */
	writel(common_dse, (IOMUXC_BASE_ADDR + 0x318));


	/*
	* MMDC Configuration
	*/
	/* MMDC_MDSCR (MMDC Core Special Command Register) Set Configuration Request */
	writel(0x00008000, (MMDC_P0_BASE_ADDR + 0x01c));
	/* MMDC_MPZQLP2CTL (MMDC ZQ LPDDR2 HW Control Register) */
	writel(0x1b4700c7, (MMDC_P0_BASE_ADDR + 0x85c));
	/* MMDC_MPZQHWCTRL (MMDC PHY ZQ HW control register) */
	writel(0xa1390003, (MMDC_P0_BASE_ADDR + 0x800));
	/* MMDC_MPPDCMPR2 (MMDC PHY Pre-defined Compare and CA delay-line Configuration Register) */
	writel(0x00400000, (MMDC_P0_BASE_ADDR + 0x890));
	/* MMDC_MPMUR0 (MMDC PHY Measure Unit Register) */
	writel(0x00000800, (MMDC_P0_BASE_ADDR + 0x8b8));
	/* MMDC_MPRDQBY0DL (MMDC PHY Read DQ Byte0 Delay Register) */
	writel(0x33333333, (MMDC_P0_BASE_ADDR + 0x81c));
	/* MMDC_MPRDQBY1DL (MMDC PHY Read DQ Byte1 Delay Register) */
	writel(0x33333333, (MMDC_P0_BASE_ADDR + 0x820));
	/* MMDC_MPRDQBY2DL (MMDC PHY Read DQ Byte2 Delay Register) */
	writel(0x33333333, (MMDC_P0_BASE_ADDR + 0x824));
	/* MMDC_MPRDQBY3DL (MMDC PHY Read DQ Byte3 Delay Register) */
	writel(0x33333333, (MMDC_P0_BASE_ADDR + 0x828));
	/* MMDC_MPWRDQBY0DL (MMDC PHY Write DQ Byte0 Delay Register) */
	writel(0xf3333333, (MMDC_P0_BASE_ADDR + 0x82c));
	/* MMDC_MPWRDQBY1DL (MMDC PHY Write DQ Byte1 Delay Register) */
	writel(0xf3333333, (MMDC_P0_BASE_ADDR + 0x830));
	/* MMDC_MPWRDQBY2DL (MMDC PHY Write DQ Byte2 Delay Register) */
	writel(0xf3333333, (MMDC_P0_BASE_ADDR + 0x834));
	/* MMDC_MPWRDQBY3DL (MMDC PHY Write DQ Byte3 Delay Register) */
	writel(0xf3333333, (MMDC_P0_BASE_ADDR + 0x838));
	/* MMDC_MPRDDLCTL (MMDC PHY Read delay-lines Configuration Register) */
	writel(0x4241444a, (MMDC_P0_BASE_ADDR + 0x848));
	/* MMDC_MPWRDLCTL (MMDC PHY Write delay-lines Configuration Register) */
	writel(0x3030312b, (MMDC_P0_BASE_ADDR + 0x850));
	/* MMDC_MPDGCTRL0 (MMDC PHY Read DQS Gating Control Register 0) */
	writel(0x20000000, (MMDC_P0_BASE_ADDR + 0x83c));
	/* MMDC_MPDGCTRL1 (MMDC PHY Read DQS Gating Control Register 1) */
	writel(0x0, (MMDC_P0_BASE_ADDR + 0x840));
	/* MMDC_MPDCCR (MMDC Duty Cycle Control Register) */
	writel(0x24911492, (MMDC_P0_BASE_ADDR + 0x8c0));
	/* MMDC_MPMUR0 (MMDC PHY Measure Unit Register) */
	writel(0x00000800, (MMDC_P0_BASE_ADDR + 0x8b8));
	/* MMDC_MDCFG0 (MMDC Core Timing Configuration Register 0) */
	writel(0x33374133, (MMDC_P0_BASE_ADDR + 0x00c));
	/* MMDC_MDPDC (MMDC Core Power Down Control Register) */
	writel(0x00020024, (MMDC_P0_BASE_ADDR + 0x004));
	/* MMDC_MDCFG1 (MMDC Core Timing Configuration Register 1) */
	writel(0x00100A82, (MMDC_P0_BASE_ADDR + 0x010));
	/* MMDC_MDCFG2 (MMDC Core Timing Configuration Register 2) */
	writel(0x00000093, (MMDC_P0_BASE_ADDR + 0x014));
	/* MMDC_MDMISC (MMDC Core Miscellaneous Register) */
	writel(0x00001688, (MMDC_P0_BASE_ADDR + 0x018));
	/* MMDC_MDRWD (MMDC Core Read/Write Command Delay Register) */
	writel(0x0f9f26d2, (MMDC_P0_BASE_ADDR + 0x02c));
	/* MMDC_MDOR (MMDC Core Out of Reset Delays Register) */
    /* RST_to_CKE changed to 14 cycles as per JEDEC spec
       this affects Idle time after first CKE assertion */
	if (PLATFORM_IS_DUET(board->id))
		writel(0x009F0E10, (MMDC_P0_BASE_ADDR + 0x030));
	else
		writel(0x00000010, (MMDC_P0_BASE_ADDR + 0x030));	

	/* MMDC_MDCFG3LP (MMDC Core Timing Configuration Register 3) */
	writel(0x00190778, (MMDC_P0_BASE_ADDR + 0x038));
	/* MMDC_MDOTC (MMDC Core ODT Timming Control Register) */
	writel(0x00000000, (MMDC_P0_BASE_ADDR + 0x008));
	/* MMDC_MDASP (MMDC Core Address Space Partition Register) */
	writel(0x0000004f, (MMDC_P0_BASE_ADDR + 0x040));
	/* MMDC_MDCTL (MMDC Core Control Register) */
	if (board->mem_size == MEMORY_SIZE_512MB) {
		// 10 bits column
		writel(0x83110000, (MMDC_P0_BASE_ADDR + 0x000));
	} else {
		// 9 bits column
		writel(0x83010000, (MMDC_P0_BASE_ADDR + 0x000));
	}
	/* MMDC_MDSCR (MMDC Core Special Command Register) LPDDR2 MRW address 0x3f=0x00, chip reset */
	writel(0x003f8030, (MMDC_P0_BASE_ADDR + 0x01c));
	/* MMDC_MDSCR (MMDC Core Special Command Register) LPDDR2 MRW address 0x0a=0xff, calibration command */
	writel(0xff0a8030, (MMDC_P0_BASE_ADDR + 0x01c));
	/* MMDC_MDSCR (MMDC Core Special Command Register) LPDDR2 MRW address 0x01=0x82, Burst Length 4, Sequential Burst, Wrap, 4 WR clock cycles*/
	writel(0x82018030, (MMDC_P0_BASE_ADDR + 0x01c));
	/* MMDC_MDSCR (MMDC Core Special Command Register) LPDDR2 MRW address 0x02=0x04, RL=6, WL=3 */
	writel(0x04028030, (MMDC_P0_BASE_ADDR + 0x01c));
	/* MMDC_MDSCR (MMDC Core Special Command Register) LPDDR2 MRW address 0x03=0x03, drive strength 48 ohm */
	writel(0x03038030, (MMDC_P0_BASE_ADDR + 0x01c));
	/* MMDC_MPZQHWCTRL (MMDC PHY ZQ Hw control register) */
	writel(0xa1310003, (MMDC_P0_BASE_ADDR + 0x800));
	/* MMDC_MDREF (MMDC Core REfresh Control Register) */
	writel(0x00001800, (MMDC_P0_BASE_ADDR + 0x020));
	/* MMDC_MPODTCTRL (MMDC PHY ODT control register) */
	writel(0x00000000, (MMDC_P0_BASE_ADDR + 0x818));
	/* MMDC_MPMUR0 (MMDC PHY Measurement Unit Register) */
	writel(0x00000800, (MMDC_P0_BASE_ADDR + 0x8b8));
	/* MMDC_MDPDC (MMDC Core Power Down Control Register) */
	writel(0x00025564, (MMDC_P0_BASE_ADDR + 0x004));
	/* MMDC_MAPSR MMDC Core Power Savings Control and  Status Register */
	writel(0x00011006, (MMDC_P0_BASE_ADDR + 0x404));
	/* MMDC_MDSCR (MMDC Core Special Command Register) Clear Configuration Request */
	writel(0x00000000, (MMDC_P0_BASE_ADDR + 0x01c));
}

static struct fsl_esdhc_cfg usdhc_cfg[1] = {
	{USDHC2_BASE_ADDR, 0, 4},
};

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC2_BASE_ADDR:
		ret = 1;
		break;
	}

	return ret;
}

int board_mmc_init(struct bd_info *bis)
{
	struct src *src_regs = (struct src *)SRC_BASE_ADDR;
	unsigned int val;
	unsigned int port;

	val = readl(&src_regs->sbmr1);

	/* Boot from USDHC */
	port = (val >> 11) & 0x3;
	switch (port) {
	case 1:
		imx_iomux_v3_setup_multiple_pads(usdhc2_pads,
						 ARRAY_SIZE(usdhc2_pads));
		usdhc_cfg[0].esdhc_base = USDHC2_BASE_ADDR;
		usdhc_cfg[0].max_bus_width = 8;
		usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
		break;
	}

	gd->arch.sdhc_clk = usdhc_cfg[0].sdhc_clk;
	return fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
}

void board_init_f(ulong dummy)
{
	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	/* Clock init */
	setup_ccm();

	/* setup GP timer */
	timer_init();

	setup_iomux_uart();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	spl_early_init();

	mmc_initialize(gd->bd);

	char pcbsn[BOARD_PCBA_LEN+1];

	idme_get_var("pcbsn", pcbsn, ARRAY_SIZE(pcbsn));

	static const struct board_type *board_info;

	board_info = get_board_type(pcbsn);

	puts("Board: ");
	puts(board_info->name);
	puts("\n");

	if(!BOARD_IS_MUSCAT_WFO(pcbsn)) {
		printf("Untested board type, trying to boot anyway but things might break.\n");
	}

	/* DDR init */
	switch (board_info->mem_type) {
	    case MEMORY_TYPE_LPDDR2:
			if (board_info->mem_size == MEMORY_SIZE_256MB || board_info->mem_size == MEMORY_SIZE_512MB) {
					lpddr2_init(board_info);
			} else {
				printf("Error! invalid memory config!\n");
			}
		break;
		default:
			printf("Error! unsupported memory type!\n");
	}
}

void board_boot_order(u32 *spl_boot_list)
{
	//spl_boot_list[0] = BOOT_DEVICE_NONE;
	//spl_boot_list[0] = spl_boot_device();
	spl_boot_list[0] = BOOT_DEVICE_BOARD;
	//spl_boot_list[1] = BOOT_DEVICE_MMC2;
}
