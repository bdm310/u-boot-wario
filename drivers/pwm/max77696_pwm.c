// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright Brian Moyer
 */

#include <common.h>
#include <dm.h>
#include <log.h>
#include <pwm.h>
#include <asm/io.h>
#include <power/pmic.h>
#include <power/max77696_pmic.h>

static int max77696_pwm_set_config(struct udevice *dev, uint channel,
				uint period_ns, uint duty_ns)
{
	unsigned char current;

	if (channel >= 1)
		return -EINVAL;

	current = (char)(((long)duty_ns * 0xFF) / (long)period_ns);

	debug("%s: Set current to %u\n", __func__, current);

	return 0;
}

static int max77696_pwm_set_enable(struct udevice *dev, uint channel,
				 bool enable)
{
	if (channel >= 1)
		return -EINVAL;

	debug("%s: Enable '%s' channel %u\n", __func__, dev->name, channel);

	return 0;
}

static int max77696_pwm_probe(struct udevice *dev)
{
	return 0;
}

static int max77696_pwm_ofdata_to_platdata(struct udevice *dev)
{
	return 0;
}

static const struct pwm_ops max77696_pwm_ops = {
	.set_config	= max77696_pwm_set_config,
	.set_enable	= max77696_pwm_set_enable,
};

static const struct udevice_id max77696_channels[] = {
	{ .compatible = "maxim,max77696-pwm" },
	{ }
};

U_BOOT_DRIVER(max77696_pwm) = {
	.name	= MAX77696_BLPWM_DRIVER,
	.id	= UCLASS_PWM,
	.of_match = max77696_channels,
	.ops	= &max77696_pwm_ops,
	.probe	= max77696_pwm_probe,
	.ofdata_to_platdata	= max77696_pwm_ofdata_to_platdata,
};
