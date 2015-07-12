/*
 * pf_dev_but.c
 *
 *  Created on: 2015年6月2日
 *      Author: Administrator
 */


#include <linux/init.h>
#include <linux/module.h>	//内核模块
#include <linux/types.h>
#include <linux/slab.h>		//链表
#include <linux/device.h>
#include <linux/platform_device.h>
#include <mach/irqs.h>

#define DEBUGPRINT
#ifdef DEBUGPRINT
#define	PRINTD(fmt,args...) printk(KERN_DEBUG fmt,##args)	//KERN_ALERT
#else
#define	PRINTD(fmt,args...)
#endif

//#define STARTADDR	0x56000000
//#define RESNAME	"but"
#define PLATFORMNAME	"minibut"
static struct resource key_resource[]=
{
	[0] = {
		.start = IRQ_EINT8,
		.end = IRQ_EINT8,
		.flags = IORESOURCE_IRQ,
	},
	[1] = {
		.start = IRQ_EINT11,
		.end = IRQ_EINT11,
		.flags = IORESOURCE_IRQ,
	},
	[2]= {
		.start = IRQ_EINT13,
		.end = IRQ_EINT13,
		.flags = IORESOURCE_IRQ,
	},
	[3] = {
		.start = IRQ_EINT14,
		.end = IRQ_EINT14,
		.flags = IORESOURCE_IRQ,
	},
	[4] = {
		.start = IRQ_EINT15,
		.end = IRQ_EINT15,
		.flags = IORESOURCE_IRQ,
	},
	[5] = {
		.start = IRQ_EINT19,
		.end = IRQ_EINT19,
		.flags = IORESOURCE_IRQ,
	},
};
struct platform_device * pf_dev;
static int __init pf_dev_init(void) {
	int ret;
	pf_dev = platform_device_alloc(PLATFORMNAME,-1);
	platform_device_add_resources(pf_dev,key_resource,(sizeof(key_resource) / sizeof(key_resource[0])));
	ret=platform_device_add(pf_dev);
	if(ret){
		platform_device_put(pf_dev);
	}
	PRINTD("Buttons Platform dev!\n");
	return ret;
}
static void __exit pf_dev_exit(void) {
	platform_device_unregister(pf_dev);
//	platform_device_put(pf_dev);
	PRINTD("Buttons Platform dev exit\n");
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");
module_init(pf_dev_init);
module_exit(pf_dev_exit);
