// SPDX-License-Identifier: GPL-2.0+
/*
 *  Copyright (C) 2014-2015 Samsung Electronics
 *  Przemyslaw Marczak  <p.marczak@samsung.com>
 */

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <log.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <power/max77696_pmic.h>

static const struct pmic_child_info pmic_children_info[] = {
	{ .prefix = "LDO", .driver = MAX77696_LDO_DRIVER },
	{ .prefix = "SD", .driver = MAX77696_SD_DRIVER },
	{ .prefix = "BLPWM", .driver = MAX77696_BLPWM_DRIVER },
	{ },
};

static int max77696_reg_count(struct udevice *dev)
{
	return MAX77696_NUM_OF_REGS;
}

static int max77696_write(struct udevice *dev, uint reg, const uint8_t *buff,
			  int len)
{
	if (dm_i2c_write(dev, reg, buff, len)) {
		pr_err("write error to device: %p register: %#x!\n", dev, reg);
		return -EIO;
	}

	return 0;
}

static int max77696_read(struct udevice *dev, uint reg, uint8_t *buff, int len)
{
	if (dm_i2c_read(dev, reg, buff, len)) {
		pr_err("read error from device: %p register: %#x!\n", dev, reg);
		return -EIO;
	}

	return 0;
}

static int max77696_bind(struct udevice *dev)
{
	ofnode sub_node;
	int children, ret = 0;

	//TODO refactor this to remove copied code
	sub_node = dev_read_subnode(dev, "voltage-regulators");
	if (!ofnode_valid(sub_node)) {
		debug("%s: %s regulators subnode not found!\n", __func__,
		      dev->name);
		ret = -ENXIO;
	}
	else {
		debug("%s: '%s' - found regulators subnode\n", __func__, dev->name);

		children = pmic_bind_children(dev, sub_node, pmic_children_info);
		if (!children)
			debug("%s: %s - no child found\n", __func__, dev->name);
	}

	sub_node = dev_read_subnode(dev, "pwm");
	if (!ofnode_valid(sub_node)) {
		debug("%s: %s pwm subnode not found!\n", __func__,
		      dev->name);
	}
	else {
		debug("%s: '%s' - found pwm subnode\n", __func__, dev->name);

		children = pmic_bind_children(dev, sub_node, pmic_children_info);
		if (!children)
			debug("%s: %s - no child found\n", __func__, dev->name);
	}

	/* Always return success for this device */
	return ret;
}

static struct dm_pmic_ops max77696_ops = {
	.reg_count = max77696_reg_count,
	.read = max77696_read,
	.write = max77696_write,
};

static const struct udevice_id max77696_ids[] = {
	{ .compatible = "maxim,max77696" },
	{ }
};

U_BOOT_DRIVER(pmic_max77696) = {
	.name = "max77696_pmic",
	.id = UCLASS_PMIC,
	.of_match = max77696_ids,
	.bind = max77696_bind,
	.ops = &max77696_ops,
};
