/*
 * mx60_wario_board.h
 *
 * Copyright 2010-2015 Amazon Technologies, Inc. All Rights Reserved.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef ___WARIO_BOARD_ID_H__
#define ___WARIO_BOARD_ID_H__

#define BOARD_ID_N              3
#define BOARD_ID_REV_N          5
#define BOARD_ID_REV_LEN        BOARD_ID_REV_N - BOARD_ID_N
#define BOARD_ID_INDEX 			0

#define BOARD_IS_(id, b, n) (strncmp((id+BOARD_ID_INDEX), (b), (n)) == 0)
#define BOARD_REV_GRT_(id, b) (strncmp((id+BOARD_ID_INDEX+BOARD_ID_N), (b+BOARD_ID_INDEX+BOARD_ID_N), BOARD_ID_REV_LEN) > 0)
#define BOARD_REV_GRT_EQ_(id, b) (strncmp((id+BOARD_ID_INDEX+BOARD_ID_N), (b+BOARD_ID_INDEX+BOARD_ID_N), BOARD_ID_REV_LEN) >= 0)

#define BOARD_REV_GREATER(id, b) (BOARD_IS_((id), (b), BOARD_ID_N) && BOARD_REV_GRT_((id), (b)))
#define BOARD_REV_GREATER_EQ(id, b) (BOARD_IS_((id), (b), BOARD_ID_N) && BOARD_REV_GRT_EQ_((id), (b)))
#define BOARD_REV_EQ(id, b) (strncmp((id), b, BOARD_ID_REV_N) == 0)

#define BOARD_ID_WARIO	                    "019"
#define BOARD_ID_WARIO_1                    "01901"
#define BOARD_ID_WARIO_2                    "01902"
#define BOARD_ID_WARIO_2_1                  "01903"
#define BOARD_ID_WARIO_3                    "01904"
#define BOARD_ID_WARIO_3_512M               "01905"
#define BOARD_ID_WARIO_3_256M_4P35BAT       "01906"
#define BOARD_ID_WARIO_3_512M_4P35BAT       "01907"
#define BOARD_ID_WARIO_4_256M_CFG_C         "01908"
#define BOARD_ID_WARIO_4_512M_CFG_B         "01909"
#define BOARD_ID_WARIO_4_1G_CFG_A           "01910"
#define BOARD_ID_WARIO_4_512M_CFG_D         "01911"
#define BOARD_ID_WARIO_4_512M_CFG_D         "01911"
#define BOARD_ID_WARIO_5                    "01911"

#define BOARD_ID_ICEWINE_WARIO              "025"
#define BOARD_ID_ICEWINE_WARIO_P5           "02508"
#define BOARD_ID_ICEWINE_WARIO_EVT1_2       "02510"
#define BOARD_ID_ICEWINE_WARIO_EVT1_3       "02511"
#define BOARD_ID_ICEWINE_WARIO_EVT2_0       "02512"
#define BOARD_ID_ICEWINE_WARIO_EVT3         "02514"

#define BOARD_ID_ICEWINE_WFO_WARIO          "026"
#define BOARD_ID_ICEWINE_WFO_WARIO_P5       "02608"
#define BOARD_ID_ICEWINE_WFO_WARIO_EVT1_2   "02610"
#define BOARD_ID_ICEWINE_WFO_WARIO_EVT1_3   "02611"
#define BOARD_ID_ICEWINE_WFO_WARIO_EVT2_0   "02612"
#define BOARD_ID_ICEWINE_WFO_WARIO_EVT3     "02614"

#define BOARD_ID_ICEWINE_WARIO_512          "047"
#define BOARD_ID_ICEWINE_WARIO_512_EVT3     "04702"
#define BOARD_ID_ICEWINE_WARIO_512_EVT4     "04703"
#define BOARD_ID_ICEWINE_PRQ_NO_LSW         "04750"

#define BOARD_ID_ICEWINE_WFO_WARIO_512      "048"
#define BOARD_ID_ICEWINE_WFO_WARIO_512_EVT3 "04802"
#define BOARD_ID_ICEWINE_WFO_WARIO_512_EVT4 "04803"
#define BOARD_ID_ICEWINE_WFO_PRQ_NO_LSW     "04850"

#define BOARD_ID_PINOT_WFO                  "027"
#define BOARD_ID_PINOT_WFO_EVT1             "02709"
#define BOARD_ID_PINOT_WFO_EVT1_2           "02716"

#define BOARD_ID_PINOT                      "02A"
#define BOARD_ID_PINOT_EVT1_2               "02A16"

#define BOARD_ID_PINOT_WFO_2GB              "02E"
#define BOARD_ID_PINOT_WFO_2GB_EVT1         "02E09"
#define BOARD_ID_PINOT_WFO_2GB_EVT1_2       "02E16"

#define BOARD_ID_PINOT_2GB                  "02F"
#define BOARD_ID_PINOT_2GB_EVT1_2           "02F16"

#define BOARD_ID_BOURBON_WFO                "051"
#define BOARD_ID_BOURBON_WFO_EVT1           "05102"

#define BOARD_ID_BOURBON_WFO_PREEVT2        "062"

#define BOARD_ID_MUSCAT_WFO                 "067"
#define BOARD_ID_MUSCAT_WFO_TOUCH_LS        "06704"

#define BOARD_ID_MUSCAT_WAN                 "068"
#define BOARD_ID_MUSCAT_WAN_TOUCH_LS        "06804"

#define BOARD_ID_MUSCAT_32G_WFO             "13G"

#define BOARD_ID_WHISKY_WFO                 "079"
#define BOARD_ID_WHISKY_WFO_HVT1            "07902"

#define BOARD_ID_WHISKY_WAN                 "078"
#define BOARD_ID_WHISKY_WAN_HVT1            "07802"
#define BOARD_ID_WHISKY_WAN_EVT1            "07803"
#define BOARD_ID_WHISKY_WFO_EVT1            "07903"
#define BOARD_ID_WHISKY_WAN_EVT1_1          "07804"
#define BOARD_ID_WHISKY_WFO_EVT1_1          "07904"
#define BOARD_ID_WHISKY_WAN_DVT1            "07805"
#define BOARD_ID_WHISKY_WFO_DVT1            "07905"
#define BOARD_ID_WHISKY_WAN_DVT1_1          "07806"
#define BOARD_ID_WHISKY_WFO_DVT1_1          "07906"
#define BOARD_ID_WHISKY_WAN_DVT1_1_REV_C    "07807"     /* PMIC - REV-C */
#define BOARD_ID_WHISKY_WFO_DVT1_1_REV_C    "07907"     /* PMIC - REV-C */

#define BOARD_ID_WOODY                      "07F"
#define BOARD_ID_WOODY_2                    "07F02"

#define BOARD_IS_MUSCAT_WFO(id)        BOARD_IS_((id), BOARD_ID_MUSCAT_WFO, BOARD_ID_N)
#define BOARD_IS_MUSCAT_WAN(id)        BOARD_IS_((id), BOARD_ID_MUSCAT_WAN, BOARD_ID_N)

#define BOARD_IS_WHISKY_WFO(id)        BOARD_IS_((id), BOARD_ID_WHISKY_WFO, BOARD_ID_N)
#define BOARD_IS_WHISKY_WAN(id)        BOARD_IS_((id), BOARD_ID_WHISKY_WAN, BOARD_ID_N)

#define BOARD_IS_BOURBON(id)           ( BOARD_IS_((id), BOARD_ID_BOURBON_WFO, BOARD_ID_N) \
										|| BOARD_IS_((id), BOARD_ID_BOURBON_WFO_PREEVT2, BOARD_ID_N) )

#define BOARD_IS_WOODY(id)		       BOARD_IS_((id), BOARD_ID_WOODY,       BOARD_ID_N)

#define PLATFORM_IS_WARIO(id)			( BOARD_IS_WARIO(id) || BOARD_IS_ICEWINE_WARIO(id) || \
											BOARD_IS_ICEWINE_WFO_WARIO(id) || \
											BOARD_IS_PINOT(id) || BOARD_IS_PINOT_WFO(id) || BOARD_IS_BOURBON(id) || \
											BOARD_IS_MUSCAT_WAN(id) || BOARD_IS_MUSCAT_WFO(id) ) 
#define PLATFORM_IS_DUET(id)			( BOARD_IS_WOODY(id) || BOARD_IS_WHISKY_WAN(id) || BOARD_IS_WHISKY_WFO(id) )

#define MEMORY_TYPE_UNKNOWN	0
#define MEMORY_TYPE_MDDR	1
#define MEMORY_TYPE_LPDDR2	2

#define MEMORY_SIZE_256MB       (256 * 1024 * 1024)
#define MEMORY_SIZE_512MB       (512 * 1024 * 1024)
#define MEMORY_SIZE_1GB         (1024 * 1024 * 1024)

typedef struct board_type {
	const char *id;  /* Tattoo + rev */
	const char *name;
	unsigned char mem_type;
	unsigned int mem_size;
} board_type;

static const struct board_type boards[] = {
    {
		.id = BOARD_ID_WARIO_1,
		.name = "Wario 1.0",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {
		.id = BOARD_ID_WARIO_2,
		.name = "Wario 2.0",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
		.id = BOARD_ID_WARIO_2_1,
		.name = "Wario 2.1",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
		.id = BOARD_ID_WARIO_3,
		.name = "Wario 3 256MB",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
		.id = BOARD_ID_WARIO_3_512M,
		.name = "Wario 3 512MB",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {
		.id = BOARD_ID_WARIO_3_256M_4P35BAT,
		.name = "Wario 3 256MB 4P35BAT",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
		.id = BOARD_ID_WARIO_3_512M_4P35BAT,
		.name = "Wario 3 512MB 4P35BAT",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {	
		.id = BOARD_ID_WARIO_4_256M_CFG_C,
		.name = "Wario 4 256MB CFG_C BOURBON ZFORCE2",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {	
		.id = BOARD_ID_WARIO_4_512M_CFG_B,
		.name = "Wario 4 512MB CFG_B ICEWINE CYTTSP4",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {	
		.id = BOARD_ID_WARIO_4_512M_CFG_D,
		.name = "Wario 4 512MB CFG_D BRCM Wifi+BT 4343W",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {	
		.id = BOARD_ID_WARIO_4_1G_CFG_A,
		.name = "Wario 4 1GB CFG_A WEIM FPGA",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_1GB,
    },
    {
		.id = BOARD_ID_ICEWINE_WARIO,
		.name = "Icewine",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
    	.id = BOARD_ID_ICEWINE_WFO_WARIO,
		.name = "Icewine WFO",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
        .id = BOARD_ID_ICEWINE_WARIO_512,
        .name = "Icewine 512",
        .mem_type = MEMORY_TYPE_LPDDR2,
        .mem_size = MEMORY_SIZE_512MB,
    },
    {
        .id = BOARD_ID_ICEWINE_WFO_WARIO_512,
        .name = "Icewine WFO 512",
        .mem_type = MEMORY_TYPE_LPDDR2,
        .mem_size = MEMORY_SIZE_512MB,
    },
    {
    	.id = BOARD_ID_PINOT_WFO,
		.name = "Pinot WFO",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
    	.id = BOARD_ID_PINOT,
		.name = "Pinot",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
    	.id = BOARD_ID_PINOT_WFO_2GB,
		.name = "Pinot WFO 2GB",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
		.id = BOARD_ID_PINOT_2GB,
		.name = "Pinot 2GB",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
		.id = BOARD_ID_BOURBON_WFO,
		.name = "Bourbon WFO",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
		.id = BOARD_ID_BOURBON_WFO_PREEVT2,
		.name = "Bourbon PREEVT2 WFO",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_256MB,
    },
    {
		.id = BOARD_ID_MUSCAT_WAN,
		.name = "Muscat WAN",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {
		.id = BOARD_ID_MUSCAT_WFO,
		.name = "Muscat WFO",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {
		.id = BOARD_ID_MUSCAT_32G_WFO,
		.name = "Muscat WFO 32GB",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {
		.id = BOARD_ID_WHISKY_WAN,
		.name = "Whisky WAN",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {
		.id = BOARD_ID_WHISKY_WFO,
		.name = "Whisky WFO",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
    {
		.id = BOARD_ID_WOODY,
		.name = "Woody",
		.mem_type = MEMORY_TYPE_LPDDR2,
		.mem_size = MEMORY_SIZE_512MB,
    },
};

#define NUM_KNOWN_BOARDS (sizeof(boards)/sizeof(boards[0]))

/*
 * Do not modify these length fields unless you know what you are doing.
 * Changing these can easily cause data corruption 
 */
#define BOARD_DSN_LEN	16
#define BOARD_MAC_LEN 12
#define BOARD_SEC_LEN 20
#define BOARD_PCBA_LEN	16
#define BOARD_BOOTMODE_LEN 16
#define BOARD_POSTMODE_LEN 16
#define BOARD_QBCOUNT_LEN 10

typedef struct nvram_t {
	const char *name;
	unsigned int offset;
	unsigned int size;
} nvram_t;

static const struct nvram_t nvram_info[] = {
    {
		.name = "serial",
		.offset = 0x00,
		.size = BOARD_DSN_LEN,
    },
    {
		.name = "mac",
		.offset = 0x30,
		.size = BOARD_MAC_LEN,
    },
    {
		.name = "sec",
		.offset = 0x40,
		.size = BOARD_SEC_LEN,
    },
    {
		.name = "pcbsn",
		.offset = 0x60,
		.size = BOARD_PCBA_LEN,
    },
    {
		.name = "bootmode",
		.offset = 0x1000,
		.size = BOARD_BOOTMODE_LEN,
    },
    {
		.name = "postmode",
		.offset = 0x1010,
		.size = BOARD_POSTMODE_LEN,
    },
    {
		.name = "btmac",
		.offset = 0x1040,
		.size = 12,
    },
};

#define CONFIG_NUM_NV_VARS (sizeof(nvram_info)/sizeof(nvram_info[0]))

const struct board_type *get_board_type(char *pcbsn);

#define CONFIG_MMC_ID_BLOCK_SIZE	512
#define CONFIG_MMC_ID_INFO_ADDR		0x5E000
#define CONFIG_MMC_ID_INFO_DEV 		1
#define CONFIG_MMC_ID_INFO_PART		1

#endif /* __WARIO_BOARD_ID_H__ */
