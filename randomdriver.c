#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/random.h>
#include <linux/uaccess.h> 


#define DEVICE_NAME "ranIntDevice"
#define CLASS_NAME  "ranClass"


static dev_t first; 
static struct cdev c_dev; 
static struct class *cl; 

static int my_open(struct inode *i, struct file *f)
{
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	return 0;
}
static ssize_t my_read(struct file *f, char *buf, size_t len, loff_t *off)
{
	unsigned int rannum=0;
	get_random_bytes(&rannum, sizeof(rannum));
	
	if (copy_to_user(buf, &rannum, sizeof(rannum)) == 0){
		printk(KERN_INFO "Random Driver: The random number is: %d\n", rannum);
		return 4;
	}
	else {
		printk(KERN_INFO "Random Driver: Failed to sent a number to the user\n");
		return -EFAULT;
	}
}

static struct file_operations pugs_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
};

static int __init ofcd_init(void) /* Constructor */
{
	if (alloc_chrdev_region(&first, 0, 1, "randomdriver") < 0)
	{
		return -1;
	}

	if ((cl = class_create(THIS_MODULE, CLASS_NAME)) == NULL)
	{
		unregister_chrdev_region(first, 1);
		return -1;
	}
	
	if (device_create(cl, NULL, first, NULL, DEVICE_NAME) == NULL)
	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}

	cdev_init(&c_dev, &pugs_fops);
	if (cdev_add(&c_dev, first, 1) == -1)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	printk(KERN_INFO "Random Driver: Random device register sucessfully\n");
	return 0;
}
static void __exit ofcd_exit(void) /* Destructor */
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "Random Driver: Random Device unregistered \n");
}

module_init(ofcd_init);
module_exit(ofcd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phan Long Hieu - Huynh Van Hien - Nguyen Dang Trung Tien");
MODULE_DESCRIPTION("A simple Linux char driver - Generate a random number");

