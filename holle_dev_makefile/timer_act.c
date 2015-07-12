/*********************************************************************************************
 * File��	hello.c
 * Author:	Hanson
 * Desc��	hello module code
 * History:	May 16th 2011
 *********************************************************************************************/

#include <linux/init.h>
#include <linux/module.h>	//�ں�ģ��
#include <linux/types.h>
#include <linux/slab.h>		//����
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");


struct timer_list tl;

static void action(unsigned long l){
	printk("the val is %ld",l);
}

static int __init timer_act_init(void) {
	init_timer(&tl);
	tl.function = action;
	tl.expires = jiffies + 5 * HZ;
	tl.data = 100;
	add_timer(&tl);
	//	printk(KERN_ALERT "Hello world\n");
	return 0;
}
static void __exit timer_act_exit(void) {

//	printk(KERN_ALERT " Hello world exit\n");
}
module_init(timer_act_init);
module_exit(timer_act_exit);
