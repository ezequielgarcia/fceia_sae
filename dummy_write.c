/*
 * A dummy char device driver
 *
 * Copyright (C) 2020 Ezequiel Garcia.
 *
 * Permission to use, copy, modify, and/or distribute
 * this software for any purpose with or without fee
 * is hereby granted.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

/* This is a global 64-byte array.
 * Remember C-strings are 1-byte null-terminated.
 */
#define BUFFER_SIZE 64
static char dummy_string[BUFFER_SIZE];

static int major;

static ssize_t dummy_read(struct file *file, char __user *buf,
			 size_t count, loff_t *offset)
{
	size_t available = strlen(dummy_string);
	loff_t pos = *offset;
	size_t ret;

	if (pos < 0)
		return -EINVAL;
	if (pos >= available || !count)
		return 0;
	if (count > available - pos)
		count = available - pos;
	ret = copy_to_user(buf, dummy_string + pos, count);
	if (ret == count)
		return -EFAULT;
	count -= ret;
	*offset = pos + count;
	return count;
}

/*
 * EXERCISE:
 * Modify dummy_write to implement write operation
 * correctly and without errors.
 */
static ssize_t dummy_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *offset)
{
	size_t ret;

	ret = copy_from_user(dummy_string, buf, count);

	return count;
}

static const struct file_operations fops = {
	.read = dummy_read,
	.write = dummy_write,
};

static int dummy_init(void)
{
	/* This is a string literal used to initialize the global array. */
	const char *init_value = "000000000000000000000000000000000000000000000000000000000000000";

	major = register_chrdev(0, KBUILD_MODNAME, &fops);
	if (major < 0)
		return major;

	/* Initialize the array. */
	memcpy(dummy_string, init_value, strlen(init_value));

	pr_info("device registered with major %d\n", major);
	return 0;
}

static void dummy_exit(void)
{
	return unregister_chrdev(major, KBUILD_MODNAME);
}

module_init(dummy_init);
module_exit(dummy_exit);

MODULE_AUTHOR("Ezequiel Garcia");
MODULE_DESCRIPTION("A simple read-only char device driver");
MODULE_LICENSE("GPL");
