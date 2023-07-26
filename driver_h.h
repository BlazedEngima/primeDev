#ifndef DRIVER_H
#define DRIVER_H

#define PREFIX_TITLE "primeDev

#define HW5_IOC_MAGIC         'k'
#define HW5_IOCSETSTUID       _IOW(HW5_IOC_MAGIC, 1, int)
#define HW5_IOCSETRWOK        _IOW(HW5_IOC_MAGIC, 2, int)
#define HW5_IOCSETIOCOK       _IOW(HW5_IOC_MAGIC, 3, int)
#define HW5_IOCSETIRQOK       _IOW(HW5_IOC_MAGIC, 4, int)
#define HW5_IOCSETBLOCK       _IOW(HW5_IOC_MAGIC, 5, int)
#define HW5_IOCWAITREADABLE   _IOR(HW5_IOC_MAGIC, 6, int)
#define HW5_IOC_MAXNR         6

// DMA
#define DMA_BUFSIZE 64
#define DMASTUIDADDR 0x0        // Student ID
#define DMARWOKADDR 0x4         // RW function complete
#define DMAIOCOKADDR 0x8        // ioctl function complete
#define DMAIRQOKADDR 0xc        // ISR function complete
#define DMACOUNTADDR 0x10       // interrupt count function complete
#define DMAANSADDR 0x14         // Computation answer
#define DMAREADABLEADDR 0x18    // READABLE variable for synchronize
#define DMABLOCKADDR 0x1c       // Blocking or non-blocking IO
#define DMAOPCODEADDR 0x20      // data.a opcode
#define DMAOPERANDBADDR 0x21    // data.b operand1
#define DMAOPERANDCADDR 0x25    // data.c operand2

// Declaration for file operations
static ssize_t drv_read(struct file *filp, char __user *buffer, size_t, loff_t*);
static int drv_open(struct inode*, struct file*);
static ssize_t drv_write(struct file *filp, const char __user *buffer, size_t, loff_t*);
static int drv_release(struct inode*, struct file*);
static long drv_ioctl(struct file *, unsigned int , unsigned long );

// in and out function
void myoutc(unsigned char data,unsigned short int port);
void myouts(unsigned short data,unsigned short int port);
void myouti(unsigned int data,unsigned short int port);
unsigned char myinc(unsigned short int port);
unsigned short myins(unsigned short int port);
unsigned int myini(unsigned short int port);

// Arithmetic funciton
static void drv_arithmetic_routine(struct work_struct* ws);

#endif
