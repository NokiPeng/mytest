/*
 * Led_drv.c
 *
 *  Created on: 2015年5月11日
 *      Author: noki
 */
#include <linux/init.h>
#include <linux/module.h>			//内核模块
#include<linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>				//链表
#include <linux/miscdevice.h>	//混杂字符设备
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <mach/regs-clock.h>
#include <plat/regs-timer.h>
#include <mach/regs-gpio.h>

#define DEBUGPRINT
#ifdef DEBUGPRINT
#define	PRINTD(fmt,args...) printk(KERN_DEBUG fmt,##args)	//KERN_ALERT
#else
#define	PRINTD(fmt,args...)
#endif

#define LED_MINOR	20
#define LED_DRVNAME		"led"
#define LED_CMD_TYPE		'L'
#define LED_CMD_ALLOFF		_IOW(LED_CMD_TYPE,0,int)
#define LED_CMD_ALLON		_IOW(LED_CMD_TYPE,1,int)
#define LED_CMD_ONEOFF	_IOW(LED_CMD_TYPE,2,int)
#define LED_CMD_ONEON		_IOW(LED_CMD_TYPE,3,int)
#define LED_CMD_LIST			_IOW(LED_CMD_TYPE,4,int)



static unsigned long led_table [] = {
    S3C2410_GPB(5),
    S3C2410_GPB(6),
    S3C2410_GPB(7),
    S3C2410_GPB(8),
};

static int LED_open(struct inode *p_inode, struct file *p_file){
	int i;
	    for (i = 0; i < 4; i++) {
	        // 设置GPIO引脚的功能：本驱动中LED所涉及的GPIO引脚设为输出功能
	        s3c2410_gpio_cfgpin(led_table[i], S3C2410_GPIO_OUTPUT);
	    }
	return 0;
}

static int LED_ioctl(struct inode *inodep, struct file *filep, unsigned int cmd, unsigned long arg) {
	int i;
	u_int32_t data;
	if(cmd == LED_CMD_ONEON ||cmd == LED_CMD_ONEOFF){
		if(!access_ok(VERIFY_READ,(void __user*)arg,_IOC_SIZE(LED_CMD_ONEON))){
			return -EFAULT;
		}else{
			if (__get_user(data, (unsigned int __user *)arg)) //指针参数传递
			        return -EFAULT;
		}
	}
	switch (cmd) {
	case LED_CMD_ALLOFF:
		for (i = 0; i < 4; i++) {
			s3c2410_gpio_setpin(led_table[i], 1);
		}
		break;
	case LED_CMD_ALLON:
		for (i = 0; i < 4; i++) {
			s3c2410_gpio_setpin(led_table[i], 0);
		}
		break;
	case LED_CMD_LIST:
		mdelay(300);
		break;
	case LED_CMD_ONEON:
		s3c2410_gpio_setpin(led_table[data], 0);
		break;
	case LED_CMD_ONEOFF:
		s3c2410_gpio_setpin(led_table[data], 1);
		break;
	default:
		return -EINVAL;
		break;
	}
	return 0;
}

struct file_operations f_opt = {
		.owner = THIS_MODULE,
		.open = LED_open,
		.ioctl=LED_ioctl, };

struct miscdevice mdev={
	.fops = &f_opt,
	.minor = LED_MINOR,
	.name = LED_DRVNAME,
};

static int __init LED_init(void) {
	if (misc_register(&mdev)) {
		PRINTD("Register dev err!\n");
		return -EFAULT;
	}
	PRINTD("Hello world\n");
	return 0;
}

static void __exit LED_exit(void) {
	misc_deregister(&mdev);
	PRINTD(" Hello world exit\n");
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");
module_init(LED_init);
module_exit(LED_exit);
