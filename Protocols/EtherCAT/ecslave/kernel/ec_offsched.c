#include <linux/module.h>
#include <linux/fs.h>		/* for file_operations */
#include <linux/version.h>	/* versioning */
#include <linux/cdev.h>
#include "ec_offsched.h"
#include "ecs_slave.h"

#define MODULE_NAME "OFFSHCED :"

static int devices_count = 2;
static dev_t device_number;
static struct cdev cdev;
static int offline_cpuid = 1;
static int offsched_on = 0;
ecat_slave *eslave = 0;
extern void register_offsched(void (*callback) (void), int cpuid);
extern void unregister_offsched(int cpuid);

/*
 * this code regisers offsched callback in offline_cpuid processor. 
 *  to use it please :
 *  insmod it , with module parameters cpuid=1 or any other cpu id other than 0
 *  from command line:
 *   echo 0 > /sys/devices/system/cpu/cpu<cpuid>/online
 *    offshced_main is called as an offlet.
*/

//static struct list_head skb_list;

void ec_offsched_main(void)
{
	offsched_on = 1;
	printk(MODULE_NAME "Main cpu= %d\n", raw_smp_processor_id());
	offsched_on = 0;
}

int driver_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t driver_write(struct file *filp,
			    const char __user * umem, size_t size, loff_t * off)
{
	return -1;
}

static ssize_t driver_read(struct file *filp, char __user * umem,
			   size_t size, loff_t * off)
{
	return -1;
}

int driver_close(struct inode *inode, struct file *filp)
{
	return 0;
}

struct file_operations driver_ops = {
open:	driver_open,
write:	driver_write,
read:	driver_read,
release:driver_close
};

void ec_offsched_cleanup(void)
{
	unregister_offsched(offline_cpuid);
	printk(MODULE_NAME "exit\n");
	cdev_del(&cdev);
}

int ec_offsched_init(ecat_slave * ecs)
{
	int ret;
	int base_minor = 0;
	int index = 1;

	printk(MODULE_NAME "init\n");

	if (alloc_chrdev_region(&device_number,
				base_minor, devices_count, "offsched")) {
		return EBUSY;
	}

	cdev_init(&cdev, &driver_ops);
	cdev.owner = THIS_MODULE;
	ret = cdev_add(&cdev,
		       MKDEV(MAJOR(device_number), index), devices_count);
	if (ret < 0) {
		printk(MODULE_NAME "Failed to cdev_add\n");
		return -1;
	}
	register_offsched(ec_offsched_main, offline_cpuid);
	eslave = ecs;		/* save locally for this module */
	return ret;
}
