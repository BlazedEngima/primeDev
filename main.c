#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include "driver_h.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BlazedEngima");

void *dma_buf;
dev_t dev_no;

// cdev file_operations
static struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = drv_read,
  .write = drv_write,
  .unlocked_ioctl = drv_ioctl,
  .open = drv_open,
  .release = drv_release,
};

// cdev variable
static struct cdev prime_dev = {
  .owner = THIS_MODULE,
};

// Work routine
static struct work_struct *work_routine;

// For input data structure
struct DataIn {
  char a;
  int b;
  short c;
} *dataIn;

// Input and output data from/to DMA
void myoutc(unsigned char data, unsigned short int port) {
  *(volatile unsigned char*)(dma_buf + port) = data;
}

void myouts(unsigned short data, unsigned short int port) {
  *(volatile unsigned short*)(dma_buf + port) = data;
}

void myouti(unsigned int data, unsigned short int port) {
  *(volatile unsigned int*)(dma_buf + port) = data;
}

unsigned char myinc(unsigned short int port) {
  return *(volatile unsigned char*)(dma_buf + port);
}

unsigned short myins(unsigned short int port) {
  return *(volatile unsigned short*)(dma_buf + port);
}

unsigned int myini(unsigned short int port) {
  return *(volatile unsigned int*)(dma_buf + port);
}

// Driver implementation of file_operations 
static int drv_open(struct inode* ii, struct file* ff) {
	try_module_get(THIS_MODULE);

  printk("%s:%s(): device open\n", PREFIX_TITLE, __func__);
	return 0;
}

static int drv_release(struct inode* ii, struct file* ff) {
	module_put(THIS_MODULE);
  
  printk("%s:%s(): device close\n", PREFIX_TITLE, __func__);
	return 0;
}

static ssize_t drv_read(struct file *filp, char __user *buffer, size_t ss, loff_t* lo) {
	/* Implement read operation for your device */
	return 0;
}

static ssize_t drv_write(struct file *filp, const char __user *buffer, size_t ss, loff_t* lo) {
	/* Implement write operation for your device */
	return 0;
}

static long drv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	/* Implement ioctl setting for your device */
	return 0;
}

// Standard arithmetic routine
static void drv_arithmetic_routine(struct work_struct* ws) {
	/* Implement arthemetic routine */
}

// Init and exit modules
static int __init init_modules(void) {
  
	printk("%s:%s():...............Start...............\n", PREFIX_TITLE, __func__);

  int ret = 0;

	/* Register chrdev */ 
  ret = alloc_chrdev_region(&dev_no, 0, 1, "primeDev");
  if (ret < 0) {
    printk(KERN_INFO "%s:%s(): Major number allocation failed\n", PREFIX_TITLE, __func__);
    return ret;
  }
  printk(KERN_INFO "%s:%s(): Register primeDev(%d, %d)\n",PREFIX_TITLE, __func__, MAJOR(dev_no), MINOR(dev_no));
  
  cdev_init(&prime_dev, &fops);

	/* Init cdev and make it alive */

  ret = cdev_add(&prime_dev, dev_no, 1);
  if (ret) {
    printk(KERN_INFO "%s:%s(): Unable to add primeDev\n", PREFIX_TITLE, __func__);
    return ret;
  }
  printk(KERN_INFO "%s:%s(): Added primeDev\n", PREFIX_TITLE, __func__);

	/* Allocate DMA buffer */

  dma_buf = kmalloc(DMA_BUFSIZE, GFP_KERNEL);
  if (!dma_buf) {
    printk(KERN_INFO "%s:%s(): Unable to allocate DMA Buffer via kmalloc\n", PREFIX_TITLE, __func__);
    return dma_buf;
  }
  printk(KERN_INFO "%s:%s(): Allocated %zu bytes of memory\n", PREFIX_TITLE, __func__, ksize(dma_buf));

	/* Allocate work routine */

	return 0;
}

static void __exit exit_modules(void) {

	/* Free DMA buffer when exit modules */

  kfree(dma_buf);

	/* Delete character device */
  cdev_del(&prime_dev);

  unregister_chrdev_region(dev_no, 1);

	/* Free work routine */


	printk("%s:%s():..............End..............\n", PREFIX_TITLE, __func__);
}

module_init(init_modules);
module_exit(exit_modules);
