// SPDX-License-Identifier: GPL-2.0+
/*
 *  Copyright (C) 2012-2015 Samsung Electronics
 *
 *  Rajeshwari Shinde <rajeshwari.s@samsung.com>
 *  Przemyslaw Marczak <p.marczak@samsung.com>
 */

//TODO Lots of copied code here

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <power/max77696_pmic.h>

#define MODE(_id, _val, _name) { \
	.id = _id, \
	.register_value = _val, \
	.name = _name, \
}

static struct dm_regulator_mode max77696_ldo_mode_array[] = {
	MODE(OPMODE_OFF, MAX77696_LDO_MODE_OFF, "OFF"),
	MODE(OPMODE_STANDBY, MAX77696_LDO_MODE_STANDBY, "ON/OFF"),
	MODE(OPMODE_LPM, MAX77696_LDO_MODE_LPM, "LPM"),
	MODE(OPMODE_ON, MAX77696_LDO_MODE_ON, "ON"),
};

static struct dm_regulator_mode max77696_sd_mode_array[] = {
	MODE(OPMODE_OFF, MAX77696_SD_MODE_OFF, "OFF"),
	MODE(OPMODE_STANDBY, MAX77696_SD_MODE_STANDBY, "ON/OFF"),
	MODE(OPMODE_LPM, MAX77696_SD_MODE_LPM, "LPM"),
	MODE(OPMODE_ON, MAX77696_SD_MODE_ON, "ON"),
};

static int max77696_hex2mode(int hex, struct dm_regulator_mode *mode_array, int array_size)
{
	for(int i = 0; i < array_size; i++) {
		if(mode_array[i].register_value == hex)
			return mode_array[i].id;
	}

	return -EINVAL;
}

static int max77696_mode2hex(int *mode_id, struct dm_regulator_mode *mode_array, int array_size)
{
	for(int i = 0; i < array_size; i++) {
		if(mode_array[i].id == *mode_id)
			return mode_array[i].register_value;
	}

	return -EINVAL;
}

static int max77696_sd_modes(int sd, struct dm_regulator_mode **modesp)
{
	int ret = -EINVAL;

	if (sd < 1 || sd > MAX77696_SD_NUM)
		return ret;

	*modesp = max77696_sd_mode_array;
	ret = ARRAY_SIZE(max77696_sd_mode_array);

	return ret;
}

static int max77696_ldo_modes(int ldo, struct dm_regulator_mode **modesp,
				struct udevice *dev)
{
	int ret = -EINVAL;

	if (ldo < 1 || ldo > MAX77696_LDO_NUM)
		return ret;

	*modesp = max77696_ldo_mode_array;
	ret = ARRAY_SIZE(max77696_ldo_mode_array);

	return ret;
}

static int max77696_ldo_mode(struct udevice *dev, int op, int *opmode)
{
	unsigned int adr, mode;
	unsigned char val;
	int ldo, ret;

	if (op == PMIC_OP_GET)
		*opmode = -EINVAL;

	ldo = dev->driver_data;
	if (ldo < 1 || ldo > MAX77696_LDO_NUM) {
		pr_err("Wrong LDO number: %d\n", ldo);
		return -EINVAL;
	}

	adr = max77696_ldo_cnfg1[ldo];

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		val &= MAX77696_LDO_MODE_MASK;
		ret = max77696_hex2mode(val, max77696_ldo_mode_array, ARRAY_SIZE(max77696_ldo_mode_array));
		if (ret < 0)
			return ret;
		*opmode = ret;
		return 0;
	}
	else {
		mode = max77696_mode2hex(opmode, max77696_ldo_mode_array, ARRAY_SIZE(max77696_ldo_mode_array));

		if (mode < 0) {
			pr_err("Wrong mode: %d for ldo%d\n", *opmode, ldo);
			return -EINVAL;
		}

		val &= ~MAX77696_LDO_MODE_MASK;
		val |= mode;
		ret = pmic_write(dev->parent, adr, &val, 1);

		return ret;
	}

	return -EINVAL;
}

static int max77696_sd_mode(struct udevice *dev, int op, int *opmode)
{
	unsigned int adr, mode;
	unsigned char val;
	int sd, ret;

	if (op == PMIC_OP_GET)
		*opmode = -EINVAL;

	sd = dev->driver_data;
	if (sd < 1 || sd > MAX77696_SD_NUM) {
		pr_err("Wrong SD number: %d\n", sd);
		return -EINVAL;
	}

	adr = max77696_sd_cnfg[sd];

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		val &= MAX77696_SD_MODE_MASK;
		ret = max77696_hex2mode(val, max77696_sd_mode_array, ARRAY_SIZE(max77696_sd_mode_array));
		if (ret < 0)
			return ret;
		*opmode = ret;
		return 0;
	}
	else {
		mode = max77696_mode2hex(opmode, max77696_sd_mode_array, ARRAY_SIZE(max77696_sd_mode_array));

		if (mode < 0) {
			pr_err("Wrong mode: %d for SD%d\n", *opmode, sd);
			return -EINVAL;
		}

		val &= ~MAX77696_SD_MODE_MASK;
		val |= mode;
		ret = pmic_write(dev->parent, adr, &val, 1);

		return ret;
	}

	return -EINVAL;
}

static int max77696_sd_hex2volt(int buck, int hex)
{
	unsigned uV = 0;

	// TODO Completely ignores DVS
	uV = hex * MAX77696_SD_UV_STEP + MAX77696_SD_UV_MIN;

	printf("uV: %i\n", uV);
	printf("hex: %i\n", hex);

	return uV;
}

static int max77696_sd_volt2hex(int ldo, int uV)
{
	int hex = 0;

	//TODO Shoud we check range here?
	hex = (uV - MAX77696_SD_UV_MIN) / MAX77696_SD_UV_STEP;

	printf("uV: %i\n", uV);
	printf("hex: %i\n", hex);

	return hex;
}

static int max77696_ldo_hex2volt(int ldo, int hex)
{
	unsigned int uV = 0;

	if (hex > MAX77696_LDO_VOLT_MAX_HEX || hex < MAX77696_LDO_VOLT_MIN_HEX) {
		pr_err("Value: %#x is wrong for LDO%d\n", hex, ldo);
		return -EINVAL;
	}

	uV = hex * MAX77696_LDO_UV_STEP + MAX77696_LDO_UV_MIN;

	return uV;
}

static int max77696_ldo_volt2hex(int ldo, int uV)
{
	int hex = 0;

	hex = (uV - MAX77696_LDO_UV_MIN) / MAX77696_LDO_UV_STEP;

	if (hex >= 0 && hex <= MAX77696_LDO_VOLT_MAX_HEX)
		return hex;

	pr_err("Value: %d uV is wrong for LDO%d\n", uV, ldo);
	return -EINVAL;
}

static int max77696_ldo_val(struct udevice *dev, int op, int *uV)
{
	unsigned int adr;
	unsigned char val;
	int ldo, hex, ret;

	if (op == PMIC_OP_GET)
		*uV = 0;

	ldo = dev->driver_data;
	if (ldo < 1 || ldo > MAX77696_LDO_NUM) {
		pr_err("Wrong LDO number: %d\n", ldo);
		return -EINVAL;
	}

	adr = max77696_ldo_cnfg1[ldo];

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		val &= MAX77696_LDO_VOLT_MASK;
		ret = max77696_ldo_hex2volt(ldo, val);
		if (ret < 0)
			return ret;
		*uV = ret;
		return 0;
	}
	else {
		hex = max77696_ldo_volt2hex(ldo, *uV);
		if (hex < 0)
			return hex;

		val &= ~MAX77696_LDO_VOLT_MASK;
		val |= hex;
		ret = pmic_write(dev->parent, adr, &val, 1);
	}

	return ret;
}

static int max77696_sd_val(struct udevice *dev, int op, int *uV)
{
	unsigned int adr;
	unsigned char val;
	int sd, hex, ret;

	sd = dev->driver_data;
	if (sd < 1 || sd > MAX77696_SD_NUM) {
		pr_err("Wrong SD number: %d\n", sd);
		return -EINVAL;
	}

	if (op == PMIC_OP_GET)
		*uV = 0;

	adr = max77696_sd_vout[sd];

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		ret = max77696_sd_hex2volt(sd, val);
		if (ret < 0)
			return ret;
		*uV = ret;
		return 0;
	}
	else {
		hex = max77696_sd_volt2hex(sd, *uV);
		if (hex < 0)
			return hex;

		val = (unsigned char) hex;
		ret = pmic_write(dev->parent, adr, &val, 1);
	}

	return ret;
}

static int max77696_ldo_enable(struct udevice *dev, int op, bool *enable)
{
	int ret =0;
	int on_off;

	if (op == PMIC_OP_GET) {
		ret = max77696_ldo_mode(dev, op, &on_off);
		if (ret)
			return ret;

		switch (on_off) {
		case OPMODE_OFF:
			*enable = false;
			break;
		default:
			*enable = true;
		}
	} else if (op == PMIC_OP_SET) {
		if (*enable)
			on_off = OPMODE_ON;
		else
			on_off = OPMODE_OFF;

		ret = max77696_ldo_mode(dev, op, &on_off);
	}

	return ret;
}

static int max77696_sd_enable(struct udevice *dev, int op, bool *enable)
{
	int ret =0;
	int on_off;

	if (op == PMIC_OP_GET) {
		ret = max77696_sd_mode(dev, op, &on_off);
		if (ret)
			return ret;

		switch (on_off) {
		case OPMODE_OFF:
			*enable = false;
			break;
		default:
			*enable = true;
		}
	} else if (op == PMIC_OP_SET) {
		if (*enable)
			on_off = OPMODE_ON;
		else
			on_off = OPMODE_OFF;

		ret = max77696_sd_mode(dev, op, &on_off);
	}

	return ret;
}

static int max77696_ldo_probe(struct udevice *dev)
{
	struct dm_regulator_uclass_platdata *uc_pdata;

	uc_pdata = dev_get_uclass_platdata(dev);

	uc_pdata->type = REGULATOR_TYPE_LDO;
	uc_pdata->mode_count = max77696_ldo_modes(dev->driver_data,
						  &uc_pdata->mode, dev);

	return 0;
}

static int ldo_get_value(struct udevice *dev)
{
	int uV;
	int ret;

	ret = max77696_ldo_val(dev, PMIC_OP_GET, &uV);
	if (ret)
		return ret;

	return uV;
}

static int ldo_set_value(struct udevice *dev, int uV)
{
	return max77696_ldo_val(dev, PMIC_OP_SET, &uV);
}

static int ldo_get_enable(struct udevice *dev)
{
	bool enable = false;
	int ret;

	ret = max77696_ldo_enable(dev, PMIC_OP_GET, &enable);
	if (ret)
		return ret;

	return enable;
}

static int ldo_set_enable(struct udevice *dev, bool enable)
{
	return max77696_ldo_enable(dev, PMIC_OP_SET, &enable);
}

static int ldo_get_mode(struct udevice *dev)
{
	int mode;
	int ret;

	ret = max77696_ldo_mode(dev, PMIC_OP_GET, &mode);
	if (ret)
		return ret;

	return mode;
}

static int ldo_set_mode(struct udevice *dev, int mode)
{
	return max77696_ldo_mode(dev, PMIC_OP_SET, &mode);
}

static int max77696_sd_probe(struct udevice *dev)
{
	struct dm_regulator_uclass_platdata *uc_pdata;

	uc_pdata = dev_get_uclass_platdata(dev);

	uc_pdata->type = REGULATOR_TYPE_BUCK;
	uc_pdata->mode_count = max77696_sd_modes(dev->driver_data,
						   &uc_pdata->mode);

	return 0;
}

static int sd_get_value(struct udevice *dev)
{
	int uV;
	int ret;

	ret = max77696_sd_val(dev, PMIC_OP_GET, &uV);
	if (ret)
		return ret;

	return uV;
}

static int sd_set_value(struct udevice *dev, int uV)
{
	return max77696_sd_val(dev, PMIC_OP_SET, &uV);
}

static int sd_get_enable(struct udevice *dev)
{
	bool enable = false;
	int ret;

	ret = max77696_sd_enable(dev, PMIC_OP_GET, &enable);
	if (ret)
		return ret;

	return enable;
}

static int sd_set_enable(struct udevice *dev, bool enable)
{
	return max77696_sd_enable(dev, PMIC_OP_SET, &enable);
}

static int sd_get_mode(struct udevice *dev)
{
	int mode;
	int ret;

	ret = max77696_sd_mode(dev, PMIC_OP_GET, &mode);
	if (ret)
		return ret;

	return mode;
}

static int sd_set_mode(struct udevice *dev, int mode)
{
	return max77696_sd_mode(dev, PMIC_OP_SET, &mode);
}

static const struct dm_regulator_ops max77696_ldo_ops = {
	.get_value  = ldo_get_value,
	.set_value  = ldo_set_value,
	.get_enable = ldo_get_enable,
	.set_enable = ldo_set_enable,
	.get_mode   = ldo_get_mode,
	.set_mode   = ldo_set_mode,
};

U_BOOT_DRIVER(max77696_ldo) = {
	.name = MAX77696_LDO_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &max77696_ldo_ops,
	.probe = max77696_ldo_probe,
};

static const struct dm_regulator_ops max77696_sd_ops = {
	.get_value  = sd_get_value,
	.set_value  = sd_set_value,
	.get_enable = sd_get_enable,
	.set_enable = sd_set_enable,
	.get_mode   = sd_get_mode,
	.set_mode   = sd_set_mode,
};

U_BOOT_DRIVER(max77696_sd) = {
	.name = MAX77696_SD_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &max77696_sd_ops,
	.probe = max77696_sd_probe,
};
