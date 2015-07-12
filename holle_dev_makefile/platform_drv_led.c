/*
 * platform_drv_led.c
 *
 *  Created on: 2015年5月20日
 *      Author: noki
 *      platform_driver_LED
 */

#include <linux/init.h>
#include <linux/module.h>	//内核模块
#include <linux/types.h>
#include <linux/slab.h>		//链表
#include <asm/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#define DEBUGPRINT
#ifdef DEBUGPRINT
#define	PRINTD(fmt,args...) printk(KERN_DEBUG fmt,##args)	//KERN_ALERT
#else
#define	PRINTD(fmt,args...)
#endif


#define PLATFORMNAME	"mini2440LED"
struct resource *res;
u32 *addrbase;

static int LED_probe(struct platform_device *dev){
	u32 start,size;

	start = dev->resource->start;
	size =dev->resource->start - dev->resource->end + 1;

	res = request_mem_region(start,size,"MINI_LED");
	addrbase =ioremap(start,size);

	PRINTD("probe driver!\n");
	return 0;
}
static int LED_remove(struct platform_device *dev){
	PRINTD("remove driver!\n");
	return 0;
}

struct platform_driver pf_drv = {
		.probe =LED_probe,
		.remove = LED_remove,
		.driver ={
				.owner = THIS_MODULE,
				.name = PLATFORMNAME,
		},
};

static int __init pf_drv_init(void) {

	platform_driver_register(&pf_drv);
	PRINTD("Hello world\n");
	return 0;
}
static void __exit pf_drv_exit(void) {
	platform_driver_unregister(&pf_drv);
	PRINTD(" Hello world exit\n");
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");
module_init(pf_drv_init);
module_exit(pf_drv_exit);

