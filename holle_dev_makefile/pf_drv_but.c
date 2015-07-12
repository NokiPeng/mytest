/*
 * pf_but.c
 *
 *  Created on: 2015年6月2日
 *      Author: Administrator
 */


#include <linux/init.h>
#include <linux/module.h>	//内核模块
#include <linux/types.h>
#include <linux/slab.h>		//链表
#include <asm/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/wait.h>

#define DEBUGPRINT
#ifdef DEBUGPRINT
#define	PRINTD(fmt,args...) printk(KERN_DEBUG fmt,##args)	//KERN_ALERT
#else
#define	PRINTD(fmt,args...)
#endif


#define PLATFORMNAME	"minibut"
#define BUT_MINOR
#define ERES_LEN	(sizeof(eres)/sizeof(eres[0]))

struct eintres {
	int eintid;
	char *name;
};
struct eintres eres[] = {
		[0] = { .eintid = 0, .name = "KEY0", },
		[1] = { .eintid = 1, .name = "KEY1", },
		[2] = { .eintid = 2, .name = "KEY2", },
		[3] = { .eintid = 3, .name = "KEY3", },
		[4] = { .eintid = 4, .name = "KEY4", },
		[5] = { .eintid = 5, .name = "KEY5", },
};

static volatile int key_values;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static volatile int ev_press = 0;

static irqreturn_t eint_handler(int irq,void *dev_id){
	int i;
	for(i=0;i<ERES_LEN;i++){
		if(irq==eres[i].eintid){
			key_values = i;
			ev_press = 1;
			wake_up_interruptible(&button_waitq);
		}
	}
	return IRQ_RETVAL(IRQ_HANDLED);
}

int but_open (struct inode *pinode, struct file *pfile){
	int i;
	for(i=0;i<ERES_LEN;i++){
		request_irq(eres[i].eintid,eint_handler,IRQ_TYPE_EDGE_BOTH,eres[i].name,&eres[i]);
	}
	return 0;
}
int but_release (struct inode *pinode, struct file *pfile){
	int i;
	for (i = 0; i < ERES_LEN; i++) {
		free_irq(eres[i].eintid, &eres[i]);
	}
	return 0;
}
ssize_t but_read(struct file *pfile, char __user *buf, size_t count,
		loff_t *offt) {
	unsigned long err;

	if (!ev_press) {
		if (pfile->f_flags & O_NONBLOCK)
			return -EAGAIN;
		else
			wait_event_interruptible(button_waitq, ev_press);
	}

	ev_press = 0;

	err = copy_to_user(buf, (const void *) &key_values,
			min(sizeof(key_values), count));

	return err ? -EFAULT : min(sizeof(key_values), count);
}
unsigned int but_poll(struct file *pfile, struct poll_table_struct *wait){
	unsigned int mask = 0;
	poll_wait(pfile, &button_waitq, wait);
	if (ev_press)
		mask |= POLLIN | POLLRDNORM;
	return mask;
}


static struct file_operations fops = {
		.owner = THIS_MODULE,
		.open = but_open,
		.release = but_release,
		.read = but_read,
		.poll = but_poll,
};

static struct miscdevice mdev = {
		.name = PLATFORMNAME,
		.minor = MISC_DYNAMIC_MINOR,
		.fops = &fops,
};

static int BUT_probe(struct platform_device *dev){
	u32 i,r;
	struct resource *res;
	for(i = 0; i<ERES_LEN;i++){
		res = platform_get_resource(dev,IORESOURCE_IRQ,i);
		if(res==NULL)
			return -ENOENT;
		eres[i].eintid = res->start;
	}
	if (misc_register(&mdev)) {
		PRINTD("Register mdev ERR!\n");
		return -EFAULT;
	}

	PRINTD("probe driver!\n");
	return 0;
}
static int BUT_remove(struct platform_device *dev){
	misc_deregister(&mdev);
	PRINTD("remove driver!\n");
	return 0;
}

struct platform_driver pf_drv = {
		.probe =BUT_probe,
		.remove = BUT_remove,
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
