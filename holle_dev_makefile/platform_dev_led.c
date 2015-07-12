/*
 * platform_dev_led.c
 *
 *  Created on: 2015年5月20日
 *      Author: noki
 *      platform_device_LED
 */

#include <linux/init.h>
#include <linux/module.h>	//内核模块
#include <linux/types.h>
#include <linux/slab.h>		//链表
#include <linux/device.h>
#include <linux/platform_device.h>
#define DEBUGPRINT
#ifdef DEBUGPRINT
#define	PRINTD(fmt,args...) printk(KERN_DEBUG fmt,##args)	//KERN_ALERT
#else
#define	PRINTD(fmt,args...)
#endif

#define STARTADDR	0x56000000
#define RESNAME	"mini2440_led"
#define PLATFORMNAME	"mini2440LED"
struct resource res={
		.start=STARTADDR,
		.end = STARTADDR + SZ_1M -1,
		.name = RESNAME,
		.flags = IORESOURCE_MEM,
};
struct platform_device * pf_dev;
static int __init pf_dev_init(void) {
	pf_dev = platform_device_alloc(PLATFORMNAME,-1);
	platform_device_add(pf_dev);

	PRINTD("Hello Platform dev!\n");
	return 0;
}
static void __exit pf_dev_exit(void) {
	platform_device_unregister(pf_dev);
	platform_device_put(pf_dev);
	PRINTD(" Platform dev exit\n");
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");
module_init(pf_dev_init);
module_exit(pf_dev_exit);


