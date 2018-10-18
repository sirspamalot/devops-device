// SPDX-License-Identifier: GPL-2.0
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/uaccess.h>


ssize_t devops_read(struct file *filep, char *buff,
			size_t count, loff_t *offp)
{
	static int finished;
	static int run;
	ssize_t res;
	const char *devops_data[] = {
		"Told you so!\n",
		"Fucking Told you so!\n",
		"Are you kidding me!\n",
		"Management bears the risk\n",
	};

	if (finished > 10) {
		finished = 0;
		run = (run + 1) % ARRAY_SIZE(devops_data);
		return 0;
	}

	res = copy_to_user(buff, devops_data[run], strlen(devops_data[run]));
	if (unlikely(res)) {
		pr_err("Kernel -> userspace copy failed!\n");
		return -EFAULT;
	}

	finished++;
	return strlen(devops_data[run]);
}

static const struct file_operations devops_fops = {
	.owner	= THIS_MODULE,
	.read	= devops_read,
};

struct miscdevice devops_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ops",
	.fops = &devops_fops,
	.mode = 0444,
};

static int __init devops_init(void)
{
	int error = misc_register(&devops_device);

	if (error)
		pr_err("failed to register\n");
	return 0;
}

static void __exit devops_exit(void)
{
	misc_deregister(&devops_device);
	pr_info("Thats all folks\n");
}

module_init(devops_init)
module_exit(devops_exit)

MODULE_AUTHOR("Oliver Schneidewind, Florian Werner");
MODULE_DESCRIPTION("A simple reminder for the management");
MODULE_LICENSE("GPL v2");
