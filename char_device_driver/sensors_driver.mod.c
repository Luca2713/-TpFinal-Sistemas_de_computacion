#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x2a3af2b6, "module_layout" },
	{ 0xfe990052, "gpio_free" },
	{ 0xa28721e0, "gpiod_direction_output_raw" },
	{ 0x37a0cba, "kfree" },
	{ 0x403f9529, "gpio_request_one" },
	{ 0xe692ab11, "device_create" },
	{ 0x7a1a2a35, "class_destroy" },
	{ 0xce3c4b81, "device_destroy" },
	{ 0x2914d544, "cdev_add" },
	{ 0xf5b80ce6, "cdev_init" },
	{ 0x9929e184, "kmem_cache_alloc_trace" },
	{ 0x50561faf, "kmalloc_caches" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x51518d91, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xd697e69a, "trace_hardirqs_on" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0xdaca81bf, "cpu_hwcaps" },
	{ 0xec3d2e1b, "trace_hardirqs_off" },
	{ 0xf3ecac14, "cpu_hwcap_keys" },
	{ 0x14b89635, "arm64_const_caps_ready" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xf9a482f9, "msleep" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0x541b94d8, "gpiod_get_raw_value" },
	{ 0xa95af66b, "gpio_to_desc" },
	{ 0x999e8297, "vfree" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x98cf60b3, "strlen" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0xc5850110, "printk" },
	{ 0x1fdc7df2, "_mcount" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "44A2AC2B9B6B60B0689CBD9");
