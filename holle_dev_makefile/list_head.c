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
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nokioffice");

struct s_users {
	char name[100];
	u_int id;
	struct list_head list;
};
struct s_users * users;
struct s_users * each_user;
struct list_head mylist;
struct list_head *each_list;
struct list_head *n;
static int __init list_head_init(void) {
	INIT_LIST_HEAD(&mylist);
	users = kmalloc(sizeof(struct s_users)*5,GFP_ATOMIC);
	memset(users,0,sizeof(struct s_users)*5);

	int i;
	for(i=0;i<5;i++){
		sprintf(users[i].name,"User%d",i+1);
		users[i].id=i+1;
		list_add(&(users[i].list),&mylist);
	}

	list_for_each(each_list,&mylist){
		each_user = list_entry(each_list,struct s_users,list);
		printk("Name = %s\tID = %d\n",each_user->name,each_user->id);
	}

	printk(KERN_ALERT "Hello world\n");
	return 0;
}
static void __exit list_head_exit(void) {
	list_for_each_safe(each_list,n,&mylist){
		list_del(each_list);
	}
	kfree(users);
	printk(KERN_ALERT " Hello world exit\n");
}
module_init(list_head_init);
module_exit(list_head_exit);
