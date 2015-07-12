/*
 * miscdevice_test.c
 *
 *  Created on: 2015年5月23日
 *      Author: noki
 */

#include <linux/init.h>
#include <linux/module.h>	//内核模块
#include <linux/types.h>
#include <linux/slab.h>		//链表
#include <asm/io.h>
#include  <linux/delay.h>
#include  <linux/ioport.h>
#include  <linux/fs.h>
#include  <linux/miscdevice.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#define DEBUGPRINT
#ifdef DEBUGPRINT
#define	PRINTD(fmt,args...) printk(KERN_ALERT fmt,##args)	//KERN_ALERT
#else
#define	PRINTD(fmt,args...)
#endif

#define MISCDEV_NAME	"md_led"
#define CMDTYPE_MD_LED	'L'
#define MINOR_LED		100
#define CMD_LED_OFF		_IOW(CMDTYPE_MD_LED,0,int)
#define CMD_LED_ON			_IOW(CMDTYPE_MD_LED,1,int)
#define CMD_LED_FOR		_IOW(CMDTYPE_MD_LED,2,int)
#define CMD_LED_N_OFF		_IOW(CMDTYPE_MD_LED,3,int)
#define CMD_LED_N_ON		_IOW(CMDTYPE_MD_LED,4,int)

#define	GPIO_BASE	0x56000000
#define	GPIO_SIZE		SZ_1M
#define 	GPB_CON	(gpio_base+0X10)
#define	GPB_DAT	(gpio_base+0X14)
#define	GPB_UP		(gpio_base+0X18)

#define 	GPB_INPUT		(~(3<<10))|(~(3<<12))|(~(3<<14))|(~(3<<16))
#define	GPB_UP_DIS		(1<<5)|(1<<6)|(1<<7)|(1<<8)

#define LED1_ON		(~(1<<5))
#define LED2_ON		(~(1<<6))
#define LED3_ON		(~(1<<7))
#define LED4_ON		(~(1<<8))
#define	LED_ALL_ON		(~(0x0F<<5))

#define LED1_OFF	(1<<5)
#define LED2_OFF	(1<<6)
#define LED3_OFF	(1<<7)
#define LED4_OFF	(1<<8)
#define	LED_ALL_OFF	(0x0F<<5)

//	LED1->GPB5
void *gpio_base;

void set_led(u_int32_t dat) {
	u_int32_t tempdat;
	tempdat = ioread32(GPB_DAT);
	PRINTD("1-%08x\n", tempdat);
	tempdat &= LED_ALL_ON;
	PRINTD("2-%08x\n", tempdat);
	tempdat |= (dat & LED_ALL_OFF);
	PRINTD("3-%08x\n", tempdat);
	iowrite32(tempdat, GPB_DAT);
}

int md_open(struct inode *pinode, struct file *pfile) {
	struct resource *res;
	u_int32_t dat;
	res = request_mem_region(GPIO_BASE, GPIO_SIZE, "res_led");
	gpio_base = ioremap(GPIO_BASE, GPIO_SIZE);
	dat = ioread32(GPB_CON);
	dat &= GPB_INPUT;
	dat |= (1 << 10);
	iowrite32(dat, GPB_CON);

	set_led(LED_ALL_OFF);
	mdelay(1000);
	set_led(LED_ALL_ON);
	mdelay(1000);
	set_led(LED_ALL_OFF);
	return 0;
}
int md_ioctl(struct inode *pinode, struct file *pfile, unsigned int cmd,
		unsigned long arg) {
	unsigned int num, i, j;
	u_int32_t leds[] = { LED1_ON, LED2_ON, LED3_ON, LED4_ON };
	u_int32_t tempdat;

	if ((cmd == CMD_LED_N_OFF) || (cmd == CMD_LED_N_ON)) {
		if (copy_from_user(&num, (unsigned int *) arg, sizeof(num)) != 0) {
			return -EFAULT;
		}
		PRINTD("num = %d\n", num);
		tempdat = ioread32(GPB_DAT);
	}

	switch (cmd) {
	case CMD_LED_OFF:
		set_led(LED_ALL_OFF);
		break;
	case CMD_LED_ON:
		set_led(LED_ALL_ON);
		break;
	case CMD_LED_FOR:
		for (i = 0; i < 5; i++) {
			for (j = 0; j < 4; j++) {
				set_led(LED_ALL_OFF);
				set_led(leds[j]);
			}
		}
		break;
	case CMD_LED_N_OFF:
		tempdat |=(~leds[num]);
		set_led(tempdat);
		break;
	case CMD_LED_N_ON:
		tempdat &= leds[num];
		set_led(tempdat);
		break;
	default:
		return -EINVAL;
		break;
	}
	return 0;
}
int md_release(struct inode *pinode, struct file *pfile) {
	PRINTD("This is miscdevice release!!\n");
	return 0;
}
ssize_t md_read(struct file *pfile, char __user *buf, size_t size, loff_t *offs) {
	PRINTD("This is miscdevice Read action!!\n");
	return 0;
}

struct file_operations fops = { .open = md_open, .read = md_read, .ioctl =
		md_ioctl, .release = md_release, .owner = THIS_MODULE, };

struct miscdevice miscdev_led = { .name = MISCDEV_NAME, .minor = MINOR_LED,
		.fops = &fops, };

static int __init miscdev_init(void) {
	if (misc_register(&miscdev_led)) {
		PRINTD("Register ERR!\n");
		return -EFAULT;
	}
//
	PRINTD("Register OK\n");
	return 0;
}
static void __exit miscdev_exit(void) {
	misc_deregister(&miscdev_led);
	PRINTD(" Deregister  exit\n");
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");
module_init(miscdev_init);
module_exit(miscdev_exit);
