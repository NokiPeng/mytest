/*********************************************************************************************
 * File：	hello.c
 * Author:	Hanson
 * Desc：	hello module code
 * History:	May 16th 2011
 *********************************************************************************************/

#include <linux/init.h>
#include <linux/module.h>	//内核模块
#include <linux/types.h>
#include <linux/slab.h>		//链表
#include<linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
//#include <linux/>

#define DEBUGPRINT
#ifdef DEBUGPRINT
#define	PRINTD(fmt,args...) printk(KERN_INFO fmt,##args)
#else
#define	PRINTD(fmt,args...)
#endif

#define DRVNAME	"fchar"
#define DRVCOUNT	2
#define MSGLEN	4096
#define CHARCMDTYPE	'C'
#define CHARCMD_NR_R	_IOR(CHARCMDTYPE,1,int)
#define CHARCMD_NR_W	_IOW(CHARCMDTYPE,0,int)
#define CLASSNAME_CHAR	"chardev"

dev_t dev;
static major, minor;
struct cdev cdev;
struct class *clschar;
DECLARE_MUTEX(sem);

struct msgs {
	char *msgval;
	u_int id;
	struct class *cls;
	struct device *dev;
};
struct msgs * msg;

int char_open(struct inode *p_inode, struct file *p_file) {
	struct msgs *mmsg;
	minor = MINOR(p_inode->i_rdev);
	mmsg = &msg[minor];
	p_file->private_data = mmsg;
	return 0;
}
int char_release(struct inode *p_inode, struct file *p_file) {
	return 0;
}
loff_t char_llseek(struct file *p_file, loff_t offset, int n) {

	loff_t newoff;
	switch (n) {
	case SEEK_SET:
		newoff = offset;
		break;
	case SEEK_CUR:
		newoff = p_file->f_pos + offset;
		break;
	case SEEK_END:
		newoff = MSGLEN + offset;
		break;
	default:
		return -EINVAL;
		break;
	}
	if (newoff < 0 || newoff > MSGLEN)
		return -EINVAL;

	p_file->f_pos = newoff;
	return newoff;
}
ssize_t char_read(struct file *p_file, char __user *buf, size_t size,
		loff_t *offset) {

	if(down_interruptible(&sem)){
		PRINTD("get sem err!");
		return -ERESTARTSYS;
	}
	struct msgs * tmpmsg;
	size_t count = size;
	int res;
	loff_t p = *offset;
	if (p >= MSGLEN)
		return 0;
	if (count > MSGLEN - p)
		count = MSGLEN - p;

	tmpmsg = p_file->private_data;
	if (copy_to_user(buf, (tmpmsg->msgval + p), count)) {
		res = -EFAULT;
	} else {
		*offset += count;
		res = count;
	}

	up(&sem);
	return res;
}
ssize_t char_write(struct file *p_file, const char __user *buf, size_t size,
		loff_t *offset) {

	if (down_interruptible(&sem)) {
		PRINTD("get sem err!");
		return -ERESTARTSYS;
	}
	struct msgs * tmpmsg;
	size_t count = size;
	int res;
	loff_t p = *offset;
	if (p >= MSGLEN)
		return 0;
	if (count > MSGLEN - p)
		count = MSGLEN - p;

	tmpmsg = p_file->private_data;
	if (copy_from_user(tmpmsg->msgval + p, buf, count)) {
		res = -EFAULT;
	} else {
		*offset += count;
		res = count;
	}

	up(&sem);
	return res;
}
int char_ioctl (struct inode *inodep, struct file *filep, unsigned int cmd, unsigned long arg){
	int val,err,res;
	//数据检验
	err = 0;
	if(_IOC_DIR(cmd)&_IOC_READ)
		err = !access_ok(VERIFY_WRITE,(void __user*)arg,_IOC_SIZE(CHARCMD_NR_R));
	if(_IOC_DIR(cmd)&_IOC_WRITE)
		err = !access_ok(VERIFY_READ,(void __user*)arg,_IOC_SIZE(CHARCMD_NR_W));
	if(err)
		return -EFAULT;
	//操作
	switch (cmd) {
		case CHARCMD_NR_R:
			val = 1980;
			res = __put_user(val,(int *)arg);
			PRINTD("send val to user ok\n");
			break;
		case CHARCMD_NR_W:
			res = __get_user(val,(int *)arg);
			PRINTD("get val ok\nval = %d\n",val);
			break;
		default:
			return -EINVAL;
			break;
	}
	return res;
}

struct file_operations f_opt = { .owner = THIS_MODULE, .open = char_open,
		.release = char_release, .llseek = char_llseek, .read = char_read,
		.write = char_write,.ioctl=char_ioctl, };
static int __init char_drv_init(void) {

	int res, i;
	res = alloc_chrdev_region(&dev, 0, DRVCOUNT, DRVNAME);
	if (res < 0) {
		PRINTD("alloc chardev err!\n");
		return res;
	}
	major = MAJOR(dev);
//	minor = MINOR(dev);
	cdev_init(&cdev, &f_opt);
	cdev.owner = THIS_MODULE;
	cdev.ops = &f_opt;
	cdev_add(&cdev, MKDEV(major, 0), DRVCOUNT);

	char *dirname;
	dirname = kmalloc(sizeof(char)*128,GFP_KERNEL);
	msg = kmalloc(sizeof(struct msgs) * DRVCOUNT, GFP_KERNEL);
	memset(msg, 0, sizeof(struct msgs)*DRVCOUNT);
	clschar = class_create(THIS_MODULE,CLASSNAME_CHAR);
	for (i = 0; i < DRVCOUNT; i++) {
		msg[i].id = i;
		msg[i].msgval = kmalloc(sizeof(char) * MSGLEN, GFP_KERNEL);
		memset(msg[i].msgval, 0, sizeof(char)*MSGLEN);
		msg[i].cls = clschar;
		memset(dirname,0,sizeof(char)*128);
		sprintf(dirname,DRVNAME"%d",i);
		msg[i].dev = device_create(clschar,NULL,MKDEV(major,i),NULL,dirname);
	}

	PRINTD("char_drv\n");
	return 0;
}
static void __exit char_drv_exit(void) {
	int i;
	for (i = 0; i < DRVCOUNT; i++) {
		device_destroy(msg[i].cls,MKDEV(major,i));
	}
	cdev_del(&cdev); /*注销设备*/
	kfree(msg);
	unregister_chrdev_region(MKDEV(major, 0), DRVCOUNT);/*释放设备号*/
	class_destroy(clschar);
	PRINTD(" char_drv exit\n");
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");
module_init(char_drv_init);
module_exit(char_drv_exit);
