/*********************************************************************************************
 * File£º	hello.c
 * Author:	Hanson
 * Desc£º	hello module code
 * History:	May 16th 2011
 *********************************************************************************************/

#include <linux/init.h>
#include <linux/module.h>	//ÄÚºËÄ£¿é
#include <linux/types.h>
#include <linux/slab.h>		//Á´±í
#define DEBUGPRINT
#ifdef DEBUGPRINT
#define	PRINTD(fmt,args...) printk(KERN_DEBUG fmt,##args)	//KERN_ALERT
#else
#define	PRINTD(fmt,args...)
#endif


static int __init hello_init(void) {
	

	PRINTD("Hello world\n");
	return 0;
}
static void __exit hello_exit(void) {
	
	PRINTD(" Hello world exit\n");
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");
module_init(hello_init);
module_exit(hello_exit);
