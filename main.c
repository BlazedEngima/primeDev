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
};

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

  printk(KERN_INFO "%s:%s(): device open\n", PREFIX_TITLE, __func__);
	return 0;
}

static int drv_release(struct inode* ii, struct file* ff) {
	module_put(THIS_MODULE);
  
  printk(KERN_INFO "%s:%s(): device close\n", PREFIX_TITLE, __func__);
	return 0;
}

static ssize_t drv_read(struct file *filp, char __user *buffer, size_t ss, loff_t* lo) {
	/* Implement read operation for your device */

  if (copy_to_user(buffer, (dma_buf + DMAANSADDR), ss)) {
    return -EFAULT;
  }
  printk(KERN_INFO "%s:%s(): Answer = %u\n", PREFIX_TITLE, __func__, myini(DMAANSADDR));

	return 0;
}

static ssize_t drv_write(struct file *filp, const char __user *buffer, size_t ss, loff_t* lo) {
	/* Implement write operation for your device */
  struct DataIn dataIn;
  
  if (copy_from_user(&dataIn, buffer, ss)) {
    return -EFAULT;
  }
  
  myouti(dataIn.a, DMAOPCODEADDR);
  myouti(dataIn.b, DMAOPERANDBADDR);
  myouti(dataIn.c, DMAOPERANDCADDR);

  INIT_WORK(work_routine, drv_arithmetic_routine);

  printk(KERN_INFO "%s:%s(): Queueing work\n", PREFIX_TITLE, __func__);
  if (!schedule_work(work_routine)) {
    printk(KERN_ALERT "%s:%s(): Error, work routine already queued in the kernel-global workqueue\n", PREFIX_TITLE, __func__);
    return 1;
  }

  if (myini(DMABLOCKADDR)) {
    printk(KERN_INFO "%s:%s(): Blocking\n", PREFIX_TITLE, __func__);
    flush_scheduled_work();
  }

	return 0;
}

static long drv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	/* Implement ioctl setting for your device */
  switch (cmd) {
    case HW5_IOCSETSTUID:
      myouti(1, DMASTUIDADDR);
      printk(KERN_INFO "%s:%s(): Student ID: 119010545\n", PREFIX_TITLE, __func__);
      break;

    case HW5_IOCSETRWOK:
      myouti(1, DMARWOKADDR);
      printk(KERN_INFO "%s:%s(): RW functions complete\n", PREFIX_TITLE, __func__);
      break;

    case HW5_IOCSETIOCOK:
      myouti(1, DMAIOCOKADDR);
      printk(KERN_INFO "%s:%s(): IOCTL function complete\n", PREFIX_TITLE, __func__);
      break;

    case HW5_IOCSETIRQOK:
      myouti(1, DMAIRQOKADDR);
      printk(KERN_INFO "%s:%s(): IRQ function complete\n", PREFIX_TITLE, __func__);
      break;
    
    case HW5_IOCSETBLOCK:
      if (myini(DMABLOCKADDR) == 0) {
        myouti(1, DMABLOCKADDR);
        printk(KERN_INFO "%s:%s(): Set Blocking IO\n", PREFIX_TITLE, __func__);
      }

      else if (myini(DMABLOCKADDR == 1)) {
        myouti(0, DMABLOCKADDR);
        printk(KERN_INFO "%s:%s(): Set Non-Blocking IO\n", PREFIX_TITLE, __func__);
      }

      else {
        printk(KERN_ALERT "%s:%s(): Error Setting Blocking/Non-Blocking IO\n", PREFIX_TITLE, __func__);
        return -EFAULT;
      }

      break;

    case HW5_IOCWAITREADABLE: {
      int readable = 1; 

      myouti(1, DMAREADABLEADDR);
      printk(KERN_INFO "%s:%s(): Set wait readable to 1\n", PREFIX_TITLE, __func__);

      flush_scheduled_work();
      if (copy_to_user((int *) arg, &readable, sizeof(int))) {
        return -EFAULT;
      }

      break;
    }
  }

	return 0;
}

static unsigned int prime(int base, short nth) {
  int ans = base;
  int i, num = 0, isPrime;

  while(num != nth) {
    isPrime = 1;
    ans++;

    for(i = 2; i <= ans / 2; i++) {
      if (ans % i == 0) {
        isPrime = 0;
        break;
      }
    }

    if (isPrime) num++;
  }
 
  return ans;
}

// Standard arithmetic routine
static void drv_arithmetic_routine(struct work_struct* ws) {
	/* Implement arthemetic routine */
  unsigned int b = myini(DMAOPERANDBADDR);
  unsigned short c = myins(DMAOPERANDCADDR);
  int ans;

  switch (myinc(DMAOPCODEADDR)) {
    case '+':
      ans = b + c;
      myouti(ans, DMAANSADDR);
      break;

    case '-':
      ans = b - c;
      myouti(ans, DMAANSADDR);
      break;

    case '*':
      ans = b * c;
      myouti(ans, DMAANSADDR);
      break;

    case '/':
      ans = b / c;
      myouti(ans, DMAANSADDR);
      break;

    case 'p':
      ans = prime(b, c);
      myouti(ans, DMAANSADDR);
      break;

    default:
      printk(KERN_ALERT "%s:%s(): Error, Operation not supported\n", PREFIX_TITLE, __func__);
      break;
  } 

  printk(KERN_INFO "%s:%s(): %u %c %d = %d\n", PREFIX_TITLE, __func__, b, myinc(DMAOPCODEADDR), c, ans);
}

// Init and exit modules
static int __init init_modules(void) {
  int ret = 0;

	printk(KERN_INFO "%s:%s():...............Start...............\n", PREFIX_TITLE, __func__);

	/* Register chrdev */ 
  ret = alloc_chrdev_region(&dev_no, 0, 1, "primeDev");
  if (ret < 0) {
    printk(KERN_ALERT "%s:%s(): Major number allocation failed\n", PREFIX_TITLE, __func__);
    return ret;
  }
  printk(KERN_INFO "%s:%s(): Register primeDev(%d, %d)\n",PREFIX_TITLE, __func__, MAJOR(dev_no), MINOR(dev_no));
  
  cdev_init(&prime_dev, &fops);

	/* Init cdev and make it alive */
  ret = cdev_add(&prime_dev, dev_no, 1);
  if (ret) {
    printk(KERN_ALERT "%s:%s(): Unable to add primeDev\n", PREFIX_TITLE, __func__);
    return ret;
  }
  printk(KERN_INFO "%s:%s(): Added primeDev\n", PREFIX_TITLE, __func__);

	/* Allocate DMA buffer */
  dma_buf = kmalloc(DMA_BUFSIZE, GFP_KERNEL);
  if (!dma_buf) {
    printk(KERN_ALERT "%s:%s(): Unable to allocate DMA Buffer via kmalloc\n", PREFIX_TITLE, __func__);
    return -EFAULT;
  }
  printk(KERN_INFO "%s:%s(): Allocated %zu bytes of memory\n", PREFIX_TITLE, __func__, ksize(dma_buf));

	/* Allocate work routine */
  work_routine = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
  if (!work_routine) {
    printk(KERN_ALERT "%s:%s(): Unable to allocate work routine via kmalloc\n", PREFIX_TITLE, __func__);
    return -EFAULT;
  }
  printk(KERN_INFO "%s:%s(): Allocated %zu bytes of memory\n", PREFIX_TITLE, __func__, ksize(work_routine));
  
	return 0;
}

static void __exit exit_modules(void) {

	/* Free DMA buffer when exit modules */
  kfree(dma_buf);
  printk(KERN_INFO "%s:%s(): Freed DMA buffer", PREFIX_TITLE, __func__);

	/* Delete character device */
  cdev_del(&prime_dev);
  unregister_chrdev_region(dev_no, 1);
  printk(KERN_INFO "%s:%s(): Unregistered chrdev", PREFIX_TITLE, __func__);

	/* Free work routine */
  kfree(work_routine);

	printk(KERN_INFO "%s:%s():..............End..............\n", PREFIX_TITLE, __func__);
}

module_init(init_modules);
module_exit(exit_modules);
