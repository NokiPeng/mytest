#include <linux/errno.h>//�����˶Է���ֵ�ĺ궨�壬�����û����������perror���������Ϣ��
#include <linux/kernel.h>//����Ҫд���ںˣ����ں���ص�ͷ�ļ�
#include <linux/module.h>//��������ļ���֧�ֶ�̬��Ӻ�ж��ģ��
#include <linux/slab.h>//������kcalloc��kzalloc�ڴ���亯���Ķ��塣
#include <linux/input.h>//�жϴ���
#include <linux/init.h>//��ʼ��ͷ�ļ�
#include <linux/serio.h>
#include <linux/delay.h>//��ʱͷ�ļ�
#include <linux/clk.h>
#include <linux/miscdevice.h>//������miscdevice�ṹ�Ķ��弰��صĲ�������
#include <linux/gpio.h>//����ϵͳ��ص�IO���ļ�
#include <linux/cdev.h>//���ַ��豸�ṹcdev�Լ�һϵ�еĲ��������Ķ���
#include <linux/io.h>
#include <linux/fs.h>
//#include <linux/types.h>

#include <asm/io.h>//������ioremap��iowrite���ں˷���IO�ڴ�Ⱥ����Ķ��塣
#include <asm/irq.h>//ʹ���жϱ����ͷ�ļ�
#include <asm/uaccess.h>//������copy_to_user��copy_from_user���ں˷����û������ڴ��ַ�ĺ�������
//#include <plat/regs-timer.h>

#include <mach/regs-clock.h> 
#include <mach/regs-gpio.h>
// /opt/linux-2.6.34/arch/arm/mach-s3c2410/include/mach/regs-gpio.h

//static dev_t dev = MKDEV(0, 0);			//��ʼ��dev_t����
static int mydev_major = 0;
static struct cdev mycdevs;				//ע���豸����

module_param(mydev_major, int, 0);		//����insmod����mydev_major����,int��������,0����sysfs/module�м���ڵ��0Ϊ�ڵ��Ȩ��
MODULE_AUTHOR("Nokioffice");			//����ģ������
MODULE_LICENSE("Dual BSD/GPL");			//��ģ������������֤

//MODULE_DESCRIPTION(description)		//˵��ģ����;�ļ������
//MODULE_VERSION(version_string)		//�����޶���
//MODULE_DEVICE_TABLE(table_info)		//�����û��ռ�ģ����֧�ֵ��豸
//MODULE_ALIAS(alternate_name)			//ģ��ı���
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
    
	cdev_init(dev, fops);			//��ʼ��cdev����
	dev->owner = THIS_MODULE;		//���ö���
	dev->ops = fops;
	err = cdev_add (dev, *devno, 1);	//���豸���뵽ϵͳ��
	/* Fail gracefully if need be */
	if (err)
		printk (KERN_NOTICE "Error %d adding MyDrv%d", err, MINOR(*devno));
}


static int __init mydev_init(void){
	int result;
	dev_t dev = MKDEV(mydev_major, 0);		//��ʼ��dev_t����
	char dev_name[]=DEV_NAME;				//��ʼ���豸��

	/* Figure out our device number. */
	if (mydev_major)						//������豸�Ų�Ϊ0���ֶ���ע���豸
		result = register_chrdev_region(dev, 1, dev_name);
	else {									//�����Զ������ڴ�ע���豸
		result = alloc_chrdev_region(&dev, 0, 1, dev_name);
		mydev_major = MAJOR(dev);			//�������豸��
	}
	if (result < 0) {						//���ע��ʧ���򷵻�
		printk(KERN_WARNING "Mydev: unable to get major %d\n", mydev_major);
		return result;
	}
	if (mydev_major == 0)
		mydev_major = result;

	/* Now set up cdev. */
	setup_cdev(&mycdevs, &dev, &mydev_fops);//��װ�豸
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
EXPORT_SYMBOL(mydev_major);	//�������ŵ��ں˷��ű�/prob/kallsyms��


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
