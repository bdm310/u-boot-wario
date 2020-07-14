/*
 * cmd_idme.c 
 *
 * Copyright 2010-12 Amazon Technologies, Inc. All Rights Reserved.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <common.h>
#include <command.h>
#include <mmc.h>
#include <malloc.h>
#include <memalign.h>
#include "idme.h"
#include "wario_board_id.h"

static unsigned char save_partition;

static const struct nvram_t *idme_find_var(const char *name) 
{
    int i, len;

    for (i = 0; i < CONFIG_NUM_NV_VARS; i++) {
		len = strlen(nvram_info[i].name);
		if (strncmp(name, nvram_info[i].name, len) == 0) {
			return &nvram_info[i];
		}
    } 

    return NULL;
}

struct mmc *get_id_mmc(void)
{
	struct mmc *mmc;
	mmc = find_mmc_device(CONFIG_MMC_ID_INFO_DEV);
	if (mmc == NULL) {
		printf("Error: Couldn't find flash device");
		return NULL;
    }

	save_partition = mmc_get_blk_desc(mmc)->hwpart;

    if (!IS_SD(mmc) && blk_select_hwpart_devnum(IF_TYPE_MMC, CONFIG_MMC_ID_INFO_DEV, CONFIG_MMC_ID_INFO_PART) != 0) {
		printf("Error: couldn't switch to boot partition\n");
		return NULL;
    }

	return mmc;
}

static int restore_mmc(struct mmc *mmc)
{
	if (!IS_SD(mmc) && blk_select_hwpart_devnum(IF_TYPE_MMC, CONFIG_MMC_ID_INFO_DEV, save_partition) != 0) {
		printf("Error: couldn't restore partition\n");
		return -1;
    }

	return 0;
}

static int idme_print_vars(void) 
{
    int i;
    char value[65];

    for (i = 0; i < CONFIG_NUM_NV_VARS; i++) {
		idme_get_var(nvram_info[i].name, value, sizeof(value));
		value[nvram_info[i].size] = 0;

		printf("%s: %s\n", nvram_info[i].name, value);
    }

    return 0;
}

int idme_get_var(const char *name, char *buf, int buflen)
{
    int ret;
    int val_size, start_address, start_block, offset;
    struct mmc *mmc;
    const struct nvram_t *nv;

    buf[0] = 0;

    nv = idme_find_var(name);
    if (!nv) {
		printf("Error: Couldn't find NV variable %s\n", name);
		return -1;
    }

	val_size = nv->size;

	// buffer is too small, return needed size
    if (val_size + 1 > buflen) {
		return (val_size + 1);
    }

    mmc = get_id_mmc();
	if(mmc == NULL) {
		return -1;
	}

	start_address = CONFIG_MMC_ID_INFO_ADDR + nv->offset;
	start_block = start_address / CONFIG_MMC_ID_BLOCK_SIZE;
	offset = start_address % CONFIG_MMC_ID_BLOCK_SIZE;

	void *varbuf = malloc_cache_aligned(CONFIG_MMC_ID_BLOCK_SIZE);
	//void *varbuf = malloc(CONFIG_MMC_ID_BLOCK_SIZE);
	if(varbuf == NULL) {
		ret = -1;
	}
	else {
		ret = blk_dread(mmc_get_blk_desc(mmc), start_block, 1, varbuf);
	}

	if (ret != 1) {
		printf("Error: Couldn't read block from partition\n");
	}

	memcpy(buf, varbuf + offset, val_size);
	free(varbuf);
    
	restore_mmc(mmc);

    if (ret) {
		return ret;
    }

    buf[val_size] = 0;

    return 0;
}

int idme_update_var(const char *name, const char *value) 
{
    int ret;
    int val_size, start_address, start_block, offset;
    struct mmc *mmc;
    const struct nvram_t *nv;
	size_t sz;

    nv = idme_find_var(name);
    if (!nv) {
		printf("Error: Couldn't find NV variable %s\n", name);
		return -1;
    }

	val_size = nv->size;

    mmc = get_id_mmc();
	if(mmc == NULL) {
		return -1;
	}

	start_address = CONFIG_MMC_ID_INFO_ADDR + nv->offset;
	start_block = start_address / CONFIG_MMC_ID_BLOCK_SIZE;
	offset = start_address % CONFIG_MMC_ID_BLOCK_SIZE;

	void *varbuf = malloc_cache_aligned(CONFIG_MMC_ID_BLOCK_SIZE);
	
	if(varbuf == NULL) {
		ret = -1;
	}
	else {
		ret = blk_dread(mmc_get_blk_desc(mmc), start_block, 1, varbuf);
	}

	if (ret != 1) {
		printf("Error: Couldn't read block from partition\n");
	}

    // clear out old value
    memset(varbuf + offset, 0, val_size);

    if (value != NULL) {
		// copy in new value, truncating to val_size
		sz = strlen(value);
		if(sz > val_size)
			sz = val_size;
		memcpy(varbuf + offset, value, sz);
    }

	ret = blk_dwrite(mmc_get_blk_desc(mmc), start_block, 1, varbuf);
    if (ret != 1) {
		printf("Error: Couldn't write NV variables. (%d)\n", ret);
		/* need to switch back to user partition even on error */
    }

	free(varbuf);
    
	restore_mmc(mmc);

    return ret;
}

int do_idme (struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    if (argc == 1 || strncmp(argv[1], "-s", 2) == 0) {
		return idme_print_vars();
    } else if (argc <= 3) {
		char *value; 

		if (argc == 3) {
			value = argv[2];
			printf("setting '%s' to '%s'\n", argv[1], argv[2]);
		} else {
			value = NULL;
			printf("clearing '%s'\n", argv[1]);
		}

		return idme_update_var(argv[1], value);
    }

    return CMD_RET_USAGE;	
}

/***************************************************/

U_BOOT_CMD(idme, 3, 1, do_idme,
	"Set nv ram variables",
	"idme <var> <value>\n"
	"    - set a variable value\n"
	"idme -s"
	"    - print out known values\n"
);
