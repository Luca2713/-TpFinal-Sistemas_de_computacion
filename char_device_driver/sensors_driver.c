#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <uapi/asm-generic/errno-base.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>

/* User-defined macros */
#define NUM_GPIO_PINS 5
#define NUM_DEVICE 2
#define DEVICE_NAME "raspi-gpio"
#define BUF_SIZE 512
#define NANO 1000000000L


/* User-defined data types */
unsigned int device[] = {20, 21};
unsigned int gpio[] = {19, 16, 26, 20, 21};

enum state
{
  low,
  high
};

enum direction
{
  in,
  out
};

/*
 * struct raspi_gpio_dev - Per gpio pin data structure
 * @cdev: instance of struct cdev
 * @pin: instance of struct gpio
 * @dir: direction of a GPIO pin
 */
struct raspi_gpio_dev
{
  struct cdev cdev;
  struct gpio pin;
  enum direction dir;
};

/* Declaration of entry points */
static int raspi_gpio_open(struct inode *inode, struct file *filp);
static ssize_t raspi_gpio_read(struct file *filp,
                               char *buf,
                               size_t count,
                               loff_t *f_pos);

static ssize_t raspi_gpio_write(struct file *filp,
                                const char *buf,
                                size_t count,
                                loff_t *f_pos);

static int raspi_gpio_release(struct inode *inode, struct file *filp);

/* File operation structure */
static struct file_operations raspi_gpio_fops = {
    .owner = THIS_MODULE,
    .open = raspi_gpio_open,
    .release = raspi_gpio_release,
    .read = raspi_gpio_read,
    .write = raspi_gpio_write,
};

/* Forward declaration of functions */
static int raspi_gpio_init(void);
static void raspi_gpio_exit(void);

/* Global varibles for GPIO driver */
struct raspi_gpio_dev *raspi_gpio_devp[NUM_GPIO_PINS];
static dev_t first;
static struct class *raspi_gpio_class;

/*
 * raspi_gpio_open - Open GPIO device node in /dev
 */
static int raspi_gpio_open(struct inode *inode, struct file *filp)
{
  unsigned int gpio = iminor(inode);
  printk(KERN_INFO "GPIO[%d] opened\n", gpio);

  return 0;
}
/*
 * raspi_gpio_release - Release GPIO pin
 */
static int
raspi_gpio_release(struct inode *inode, struct file *filp)
{
  unsigned int gpio = iminor(inode);
  printk(KERN_INFO "Closing GPIO %d\n", gpio);

  return 0;
}

/*
 * raspi_gpio_read - Read the state of GPIO pins
 *
 * This functions allows to read the logic state of input GPIO pins
 * and output GPIO pins.
 */
static ssize_t
raspi_gpio_read(struct file *filp,
                char *buf,
                size_t count,
                loff_t *f_pos)
{
  unsigned int gpio;
  int nr_bytes, valor;
  char *gpio_state = vmalloc(128);
  struct timespec timestamp_one, timestamp_two;

  if ((*f_pos) > 0) /* Tell the application that there is nothing left to read */
    return 0;

  gpio = iminor(filp->f_path.dentry->d_inode);

  // write to user
  switch (gpio)
  {
  case 20:
    valor = ((gpio_get_value(20) + gpio_get_value(26) * 2 + gpio_get_value(16) *
        4 + gpio_get_value(19) * 8) * 100 / 15);

    sprintf(gpio_state, "%d", valor);
    break;
  case 21:
    while (gpio_get_value(21) == 0){}
    
    getnstimeofday(&timestamp_one);
    msleep(150);
    
    while (gpio_get_value(21) == 1){}

    while (gpio_get_value(21) == 0){}

    getnstimeofday(&timestamp_two);

    sprintf(gpio_state, "%ld", (long int) 
        ((timestamp_two.tv_sec*NANO+timestamp_two.tv_nsec)- 
         (timestamp_one.tv_sec*NANO+timestamp_one.tv_nsec))/1000000);

    break;
  }

  nr_bytes = strlen(gpio_state);

  if (count < nr_bytes)
    return -ENOSPC;

  /* Transfiere data desde el espacio de kernel al espacio de usuario */
  /* cat /proc/clipboard                                              */

  if (copy_to_user(buf, gpio_state, nr_bytes))
    return -EINVAL;

  (*f_pos) += count; /* Update the file pointer */

  vfree(gpio_state);

  return nr_bytes;
}
/*
 * raspi_gpio_write - Write to GPIO pin
 *
 * Set GPIO pin logic level (high/low)
 Description
 * "1" Set GPIO pin logic level to high
 * "0" Set GPIO pin logic level to low
 */
static ssize_t
raspi_gpio_write(struct file *filp,
                 const char *buf,
                 size_t count,
                 loff_t *f_pos)
{
  unsigned int gpio;
  gpio = iminor(filp->f_path.dentry->d_inode);

  printk(KERN_INFO "User tried to write GPIO %d\n", gpio);

  return count;
}

/*
 * raspi_gpio_init - Initialize GPIO device driver
 *
 * This function performs the following tasks:
 * Dynamically register a character device major
 * Create "raspi-gpio" class
 * Claim GPIO resource
 * Initialize the per-device data structure raspi_gpio_dev
 * Register character device to the kernel
 * Create device nodes to expose GPIO resource
 */
static int __init
raspi_gpio_init(void)
{
  int i, ret, index = 0;

  if (alloc_chrdev_region(&first,
                          0,
                          NUM_GPIO_PINS,
                          DEVICE_NAME) < 0)
  {
    printk(KERN_DEBUG "Cannot register device\n");
    return -1;
  }
  if ((raspi_gpio_class = class_create(THIS_MODULE,
                                       DEVICE_NAME)) == NULL)
  {
    printk(KERN_DEBUG "Cannot create class %s\n", DEVICE_NAME);
    unregister_chrdev_region(first, NUM_GPIO_PINS);
    return -EINVAL;
  }
  for (i = 0; i < NUM_DEVICE; i++)
  {
    {
      raspi_gpio_devp[index] = kmalloc(sizeof(struct raspi_gpio_dev),
                                       GFP_KERNEL);
      if (!raspi_gpio_devp[index])
      {
        printk("Bad kmalloc\n");
        return -ENOMEM;
      }
      raspi_gpio_devp[index]->dir = in;
      raspi_gpio_devp[index]->cdev.owner = THIS_MODULE;
      cdev_init(&raspi_gpio_devp[index]->cdev, &raspi_gpio_fops);
      if ((ret = cdev_add(&raspi_gpio_devp[index]->cdev,
                          (first + device[i]),
                          1)))
      {
        printk(KERN_ALERT "Error %d adding cdev\n", ret);
        for (i = 0; i < NUM_DEVICE; i++)
        {
          device_destroy(raspi_gpio_class,
                         MKDEV(MAJOR(first),
                               MINOR(first) + device[i]));
        }
        class_destroy(raspi_gpio_class);
        unregister_chrdev_region(first, NUM_GPIO_PINS);
        return ret;
      }
      if (device_create(raspi_gpio_class,
                        NULL,
                        MKDEV(MAJOR(first), MINOR(first) + device[i]),
                        NULL,
                        "raspiGpio%d",
                        device[i]) == NULL)
      {
        class_destroy(raspi_gpio_class);
        unregister_chrdev_region(first, NUM_GPIO_PINS);
        return -1;
      }
      index++;
    }
  }

  for (i = 0; i < NUM_GPIO_PINS; i++)
  {
    {
      if (gpio_request_one(gpio[i], GPIOF_IN, NULL) < 0)
      {
        printk(KERN_ALERT "Error requesting GPIO %d\n", gpio[i]);
        return -ENODEV;
      }
    }
  }

  printk("RaspberryPi GPIO driver initialized\n");
  return 0;
}
/*
 * raspi_gpio_exit - Clean up GPIO device driver when unloaded
 *
 * This functions performs the following tasks:
 * Release major number
 * Release device nodes in /dev
 * Release per-device structure arrays
 * Detroy class in /sys
 * Set all GPIO pins to output, low level
 */
static void __exit
raspi_gpio_exit(void)
{
  int i = 0;
  unregister_chrdev_region(first, NUM_DEVICE);
  for (i = 0; i < NUM_DEVICE; i++)
    kfree(raspi_gpio_devp[i]);
  for (i = 0; i < NUM_DEVICE; i++)
  {
    device_destroy(raspi_gpio_class,
                   MKDEV(MAJOR(first), MINOR(first) + device[i]));
  }

  for (i = 0; i < NUM_GPIO_PINS; i++)
  {
    gpio_direction_output(gpio[i], 0);
    gpio_free(gpio[i]);
  }

  class_destroy(raspi_gpio_class);
  printk(KERN_INFO "RaspberryPi GPIO driver removed\n");
}
module_init(raspi_gpio_init);
module_exit(raspi_gpio_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Programmatori-di-spaghetti");
MODULE_DESCRIPTION("GPIO device driver for Raspberry Pi 4 Model B ARM64");
