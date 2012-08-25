/*
 * USB ROBOTIC ARM driver
 * robotic_arm - driver for Robotic ARM with USB interface 
 * from Maplin Electronics.This is an experimental driver. 
 * Use at your own risk.
 *
 * Copyright (C) 2012 Maxin B. John (maxin.john@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 *
 * Derived from USB LED driver
 * Copyright (c) 2001 Greg Kroah-Hartman (greg@kroah.com)
 */

#define DEBUG 1

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb.h>

/* Version Information */
#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "Maxin B. John, maxin.john@gmail.com"
#define DRIVER_DESC "USB ROBOTIC ARM Driver"

#define VENDOR_ID       0x1267
#define PRODUCT_ID      0x0000

/* table of devices that work with this driver */
static struct usb_device_id id_table[] = {
     { .idVendor = 0x1267, .match_flags = USB_DEVICE_ID_MATCH_VENDOR, },
     { },
};
MODULE_DEVICE_TABLE (usb, id_table);

struct usb_robotic_arm {
        struct usb_device       *udev;
        unsigned char           led;
        unsigned char           gripmotor;
        unsigned char           motor2;
        unsigned char           motor3;
        unsigned char           motor4;
        unsigned char           basemotor;
};

static void update_robotic_arm(struct usb_robotic_arm *robotic_arm)
{
        int retval = 0;
	u8 cmd[3];

        cmd[0] = 0;
	cmd[1] = 0;
	cmd[2] = 0; 

	/* 
	 * Byte 2 controls LED light inside the grip.
	 * If Bit 0 is 1, LED on, otherwise LED off
	 * 
	 */

	if (robotic_arm->led)
		cmd[2] = 1;

	/* 
	 * Byte 1 controls the Base Motor
	 * If value is 1, Base Motor rotates clockwise
         * If value is 2, Base Motor rotates anti-clockwise
	 * Otherwise, Base Motor is off
	 * 
	 */

	if (robotic_arm->basemotor == 1)
		cmd[1] = 1;

	else if (robotic_arm->basemotor == 2)
		cmd[1] = 2;
	else
		cmd[1] = 0;

	/* 
	 * Byte 0 controls the Grip
	 * If value is 1, Grip tightens
         * If values is 2, Grip Looses
	 * 
	 */
	
	if (robotic_arm->gripmotor == 1)
		cmd[0] = 1;
	else if(robotic_arm->gripmotor == 2)
		cmd[0] = 2;
	/* 
	 * Byte 0 controls the Motor 2
	 * If value is 1, Motor 2 (wrist) moves up
         * If value is 2, Motor 2 (wrist) moves down
	 * 
	 */
	
	else if (robotic_arm->motor2 == 1)
		cmd[0] = 4;
	else if(robotic_arm->motor2 == 2)
		cmd[0] = 8;

	/* 
	 * Byte 0 controls the Motor 3
	 * If value is 1, Motor 3 (elbow) moves up
         * If value is 2, Motor 3 (elbow) moves down
	 * 
	 */
	
	else if (robotic_arm->motor3 == 1)
		cmd[0] = 16;
	else if(robotic_arm->motor3 == 2)
		cmd[0] = 32;

	/* 
	 * Byte 0 controls the Motor 4
	 * If value is 1, Motor 4 (shoulder) moves up
         * If value is 2, Motor 4 (shoulder) moves down
	 * 
	 */
	
	else if (robotic_arm->motor4 == 1)
		cmd[0] = 64;
	else if(robotic_arm->motor4 == 2)
		cmd[0] = 128;
	else
		cmd[0] = 0;

      retval = usb_control_msg(robotic_arm->udev, 
					usb_sndctrlpipe(robotic_arm->udev, 0), 
					6,
                                        0x40,
                                        0x100,
                                        0,
                                        cmd,
                                        3,
                                        0);


        if (retval)
                dev_dbg(&robotic_arm->udev->dev, "retval = %d\n", retval);
invalid_value:
                dev_dbg(&robotic_arm->udev->dev, "Provided invalid input value to the motor\n");
	
}

#define show_set(value) \
static ssize_t show_##value(struct device *dev, struct device_attribute *attr,\
                            char *buf)                                  \
{                                                                       \
        struct usb_interface *intf = to_usb_interface(dev);             \
        struct usb_robotic_arm *robotic_arm = usb_get_intfdata(intf);   \
                                                                        \
        return sprintf(buf, "%d\n", robotic_arm->value);                \
}                                                                       \
static ssize_t set_##value(struct device *dev, struct device_attribute *attr,\
                           const char *buf, size_t count)               \
{                                                                       \
        struct usb_interface *intf = to_usb_interface(dev);             \
        struct usb_robotic_arm *robotic_arm = usb_get_intfdata(intf);   \
        int temp = simple_strtoul(buf, NULL, 10);                       \
                                                                        \
        robotic_arm->value = temp;                                      \
	update_robotic_arm(robotic_arm);                                \
        return count;                                                   \
}                                                                       \
static DEVICE_ATTR(value, S_IRUGO | S_IWUSR, show_##value, set_##value);
show_set(led);
show_set(gripmotor);
show_set(motor2);
show_set(motor3);
show_set(motor4);
show_set(basemotor);

static int robotic_arm_probe(struct usb_interface *interface,
                     const struct usb_device_id *id)
{
        struct usb_device *udev;
        struct usb_robotic_arm *dev;
        int retval = -ENOMEM;

	udev = interface_to_usbdev(interface);
        dev = kzalloc(sizeof(struct usb_robotic_arm), GFP_KERNEL);
        if (dev == NULL) {
                dev_err(&interface->dev, "out of memory\n");
                goto error_mem;
        }
	
        dev->udev = usb_get_dev(udev);

        usb_set_intfdata(interface, dev);

        retval = device_create_file(&interface->dev, &dev_attr_led);
        if (retval)
                goto error;
        retval = device_create_file(&interface->dev, &dev_attr_gripmotor);
        if (retval)
                goto error;
        retval = device_create_file(&interface->dev, &dev_attr_motor2);
        if (retval)
                goto error;
        retval = device_create_file(&interface->dev, &dev_attr_motor3);
        if (retval)
                goto error;
        retval = device_create_file(&interface->dev, &dev_attr_motor4);
        if (retval)
                goto error;
        retval = device_create_file(&interface->dev, &dev_attr_basemotor);
        if (retval)
                goto error;

        dev_info(&interface->dev, "USB ROBOTIC ARM now attached\n");
        return 0;

error:
        device_remove_file(&interface->dev, &dev_attr_led);
        device_remove_file(&interface->dev, &dev_attr_gripmotor);
        device_remove_file(&interface->dev, &dev_attr_motor2);
        device_remove_file(&interface->dev, &dev_attr_motor3);
        device_remove_file(&interface->dev, &dev_attr_motor4);
        device_remove_file(&interface->dev, &dev_attr_basemotor);
       
	usb_set_intfdata(interface, NULL);
        usb_put_dev(dev->udev);
        kfree(dev);

error_mem:
        return retval;
}

static void robotic_arm_disconnect(struct usb_interface *interface)
{
        struct usb_robotic_arm *dev = NULL;

        dev = usb_get_intfdata(interface);

        device_remove_file(&interface->dev, &dev_attr_led);
        device_remove_file(&interface->dev, &dev_attr_gripmotor);
        device_remove_file(&interface->dev, &dev_attr_motor2);
        device_remove_file(&interface->dev, &dev_attr_motor3);
        device_remove_file(&interface->dev, &dev_attr_motor4);
        device_remove_file(&interface->dev, &dev_attr_basemotor);

        /* first remove the files, then set the pointer to NULL */
        usb_set_intfdata(interface, NULL);

        usb_put_dev(dev->udev);

        kfree(dev);

        dev_info(&interface->dev, "USB ROBOTIC ARM now disconnected\n");
}

static struct usb_driver robotic_arm_driver = {
        .name =         "robotic_arm",
        .probe =        robotic_arm_probe,
        .disconnect =   robotic_arm_disconnect,
        .id_table =     id_table,
};

static int __init usb_robotic_arm_init(void)
{
        int retval = 0;
        retval = usb_register(&robotic_arm_driver);
        if (retval)
                err("usb_register failed. Error number %d", retval);
        return retval;
}

static void __exit usb_robotic_arm_exit(void)
{
        usb_deregister(&robotic_arm_driver);
}

module_init (usb_robotic_arm_init);
module_exit (usb_robotic_arm_exit);

module_usb_driver(robotic_arm_driver);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

