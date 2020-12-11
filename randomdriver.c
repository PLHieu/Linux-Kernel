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

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class
static unsigned int rannum;

static int my_open(struct inode *i, struct file *f)
{
	get_random_bytes(&rannum, sizeof(rannum));
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}
static ssize_t my_read(struct file *f, char *buf, size_t len, loff_t *off)
{

	int error_count = 0;
	get_random_bytes(&rannum, sizeof(rannum));
	
	// error_count = copy_to_user(buf, rannum, sizeof(rannum));

    if (copy_to_user(buf, &rannum, sizeof(rannum)) == 0){
        printk(KERN_INFO "Driver: The random number is: %d\n", rannum);
        return 0;
    }
    else {
        printk(KERN_INFO "Generator: Failed to sent a number to the user\n");
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
	//printk(KERN_INFO "A goi: ofcd registered");
	if (alloc_chrdev_region(&first, 0, 1, "Shweta") < 0)
	{
		return -1;
	}
	printk(KERN_INFO "step 1 succeed");
	if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
	{
		unregister_chrdev_region(first, 1);
		return -1;
	}
	printk(KERN_INFO "step 2 succeed");
	if (device_create(cl, NULL, first, NULL, "mynull") == NULL)
	{
		printk(KERN_INFO "step 3 fail");
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	printk(KERN_INFO "step 3 succeed");
	cdev_init(&c_dev, &pugs_fops);
	if (cdev_add(&c_dev, first, 1) == -1)
	{
		printk(KERN_INFO "step 4 fail");
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	printk(KERN_INFO "step 4 succeed\n");
	
	return 0;
}
static void __exit ofcd_exit(void) /* Destructor */
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);

	printk(KERN_INFO "B tra loi: ofcd unregistered \n");
}
module_init(ofcd_init);
module_exit(ofcd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anil Kumar Pugalia");
MODULE_DESCRIPTION("Our First Character Driver");
