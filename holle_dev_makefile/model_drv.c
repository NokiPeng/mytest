#include <linux/errno.h>//包含了对返回值的宏定义，这样用户程序可以用perror输出错误信息。
#include <linux/kernel.h>//驱动要写入内核，与内核相关的头文件
#include <linux/module.h>//最基本的文件，支持动态添加和卸载模块
#include <linux/slab.h>//包含了kcalloc、kzalloc内存分配函数的定义。
#include <linux/input.h>//中断处理
#include <linux/init.h>//初始化头文件
#include <linux/serio.h>
#include <linux/delay.h>//延时头文件
#include <linux/clk.h>
#include <linux/miscdevice.h>//包含了miscdevice结构的定义及相关的操作函数
#include <linux/gpio.h>//操作系统相关的IO口文件
#include <linux/cdev.h>//对字符设备结构cdev以及一系列的操作函数的定义
#include <linux/io.h>
#include <linux/fs.h>
//#include <linux/types.h>

#include <asm/io.h>//包含了ioremap、iowrite等内核访问IO内存等函数的定义。
#include <asm/irq.h>//使用中断必须的头文件
#include <asm/uaccess.h>//包含了copy_to_user、copy_from_user等内核访问用户进程内存地址的函数定义
//#include <plat/regs-timer.h>

#include <mach/regs-clock.h> 
#include <mach/regs-gpio.h>
// /opt/linux-2.6.34/arch/arm/mach-s3c2410/include/mach/regs-gpio.h

//static dev_t dev = MKDEV(0, 0);			//初始化dev_t对象
static int mydev_major = 0;
static struct cdev mycdevs;				//注册设备对象

module_param(mydev_major, int, 0);		//可在insmod加入mydev_major参数,int参数类型,0不在sysfs/module中加入节点非0为节点加权限
MODULE_AUTHOR("Nokioffice");			//描述模块作者
MODULE_LICENSE("Dual BSD/GPL");			//该模块采用自由许可证

//MODULE_DESCRIPTION(description)		//说明模块用途的简短描述
//MODULE_VERSION(version_string)		//代码修订号
//MODULE_DEVICE_TABLE(table_info)		//告诉用户空间模块所支持的设备
//MODULE_ALIAS(alternate_name)			//模块的别名
/*
#define MYDEV_MAGIC 'k'
#define MYDEV_START_CMD _IO (MYDEV_MAGIC, 1)
#define MYDEV_STOP_CMD _IO (MYDEV_MAGIC, 2)
*/

#define DEV_NAME ("mydev")


int mydev_open (struct inode *inode, struct file *filp)
{
	//s3c2410_gpio_cfgpin(S3C2410_GPB(0), S3C2410_GPIO_OUTPUT);
	//s3c2410_gpio_setpin(S3C2410_GPB(0),0);
	return 0;
}

ssize_t mydev_read(struct file *file, char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

ssize_t mydev_write(struct file *file, const char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

static int mydev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	
	return 0;

}

static int mydev_release(struct inode *node, struct file *file)
{
	return 0;
}

static struct file_operations mydev_fops = {
	.owner   = THIS_MODULE,
	.open    = mydev_open,
	.release = mydev_release,
	.read    = mydev_read,
	.write   = mydev_write,
	.ioctl   = mydev_ioctl,	
};

static void setup_cdev(struct cdev *dev,dev_t *devno, struct file_operations *fops)
{
	int err;
    
	cdev_init(dev, fops);			//初始化cdev对象
	dev->owner = THIS_MODULE;		//设置对象
	dev->ops = fops;
	err = cdev_add (dev, *devno, 1);	//将设备加入到系统中
	/* Fail gracefully if need be */
	if (err)
		printk (KERN_NOTICE "Error %d adding MyDrv%d", err, MINOR(*devno));
}


static int __init mydev_init(void){
	int result;
	dev_t dev = MKDEV(mydev_major, 0);		//初始化dev_t对象
	char dev_name[]=DEV_NAME;				//初始化设备名

	/* Figure out our device number. */
	if (mydev_major)						//如果主设备号不为0则手动生注册设备
		result = register_chrdev_region(dev, 1, dev_name);
	else {									//否则自动分配内存注册设备
		result = alloc_chrdev_region(&dev, 0, 1, dev_name);
		mydev_major = MAJOR(dev);			//返回主设备号
	}
	if (result < 0) {						//如果注册失败则返回
		printk(KERN_WARNING "Mydev: unable to get major %d\n", mydev_major);
		return result;
	}
	if (mydev_major == 0)
		mydev_major = result;

	/* Now set up cdev. */
	setup_cdev(&mycdevs, &dev, &mydev_fops);//安装设备
	printk("beep device installed, with major %d\n", mydev_major);
	printk("The device name is: %s\n", dev_name);
	return 0;
}

static int __exit mydev_exit(void){
	cdev_del(&mycdevs);
	unregister_chrdev_region(MKDEV(mydev_major, 0), 1);
	printk("beep device uninstalled\n");
	return 0;
}

module_init(mydev_init);
module_exit(mydev_exit);
EXPORT_SYMBOL(mydev_major);	//导出符号到内核符号表“/prob/kallsyms”


/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>

#define BEEP_MAGIC 'k'
#define BEEP_START_CMD _IO (BEEP_MAGIC, 1)
#define BEEP_STOP_CMD _IO (BEEP_MAGIC, 2)


	int dev_fd;
	dev_fd = open("/dev/mydev",O_RDWR | O_NONBLOCK);
	if ( dev_fd == -1 ) {
		printf("Cann't open file /dev/mydev\n");
		exit(1);
	}
	printf("Start mydev\n");
	ioctl (dev_fd, MYDEV_START_CMD,0);
	printf("Stop mydev and Close device\n");
	close(dev_fd);

*/
