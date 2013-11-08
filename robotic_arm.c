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
 *
 * Modified by jbh (john.hanson@cccplusplus.co.uk)
 *     -   Device context now held in cmd buffer
 *     -   Allow multiple motors at the same time
 *     -   Add parsed file which allows a single file to control the robot
 *            characters 0..8 select the device to control
 *            character S=stop, F=forward, R=reverse, E=emergency (stop all motors)
 *            eg   echo "0F" >parsed    causes the gripper to close
 *
 */

#define DEBUG 1

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <asm/uaccess.h>


/* Version Information */
#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "Maxin B. John, maxin.john@gmail.com"
#define DRIVER_DESC "USB ROBOTIC ARM Driver"

#define VENDOR_ID       0x1267
#define PRODUCT_ID      0x0000

/* table of devices that work with this driver */
static struct usb_device_id id_table[] = {
	{.idVendor = 0x1267, .match_flags = USB_DEVICE_ID_MATCH_VENDOR,},
	{},
};

MODULE_DEVICE_TABLE(usb, id_table);
#define CMD_SIZE 3
struct usb_robotic_arm {
	struct usb_device *udev;
        u8            parse_point;
        u8            cmd[CMD_SIZE];
};

static void update_robotic_arm(struct usb_robotic_arm *robotic_arm)
{
	int retval = 0;

	retval = usb_control_msg(robotic_arm->udev,
				 usb_sndctrlpipe(robotic_arm->udev, 0),
				 6, 0x40, 0x100, 0, robotic_arm->cmd, 3, 0);


	if (retval)
		dev_dbg(&robotic_arm->udev->dev, "retval = %d\n", retval);

}


static ssize_t show_parsed(struct device *dev, struct device_attribute *attr,
			   char *buf )

{
  struct usb_interface *intf = to_usb_interface(dev);
  struct usb_robotic_arm *robotic_arm = usb_get_intfdata(intf);
  u8 index;
  ssize_t written;
  written = 0;
  for ( index=0;index<8;index++)
  {
    ssize_t count;
    u8 shift, offset;
    shift = ( 2 * ( index & 3 ) );
    offset = ( index >> 2 );

    count=sprintf(buf, "motor %d state %d\n", index, ( robotic_arm->cmd[offset]>>shift ) & 0x03) ;
    buf += count;
    written += count;
  }
  return written;
}


static ssize_t set_parsed(struct device *dev, struct device_attribute *attr,
			  const char *buf, size_t count )
{
  size_t written;
  struct usb_interface *intf = to_usb_interface(dev);
  struct usb_robotic_arm *robotic_arm = usb_get_intfdata(intf);
  u8 update_needed = 0;
  written = 0;
  while ( count-- > 0 )
  {
    char file_data;
    file_data = *buf;
    if (( file_data >= '0' ) && ( file_data <= '9' ) )
    {
      robotic_arm->parse_point = ( file_data - '0' );
    }
    else if ( file_data == 'E' )   /* Emergency stop everything */
    {
      u8 offset;
      for (offset=0;offset<CMD_SIZE;offset++)
      {
        robotic_arm->cmd[offset] = 0;
      }
      update_needed=1;
    }
    else
    {
      u8 shift, offset;
      shift = ( 2 * ( robotic_arm->parse_point & 3 ) );
      offset = ( robotic_arm->parse_point >> 2 );
      if ( offset < CMD_SIZE )
      {
        switch ( file_data )
        {
	  case 'F' :   /* Forward */
            robotic_arm->cmd[offset] &= ~( 0x03 << shift );
            robotic_arm->cmd[offset] |= ( 0x01  << shift );
            update_needed = 1;
	    break;
	  case 'R' :  /* Reverse */
            robotic_arm->cmd[offset] &= ~( 0x03 << shift );
            robotic_arm->cmd[offset] |= ( 0x02  << shift );
            update_needed = 1;
	    break;
	  case 'S' :   /* Stop */
            robotic_arm->cmd[offset] &= ~( 0x03 << shift );
            robotic_arm->cmd[offset] |= ( 0x00  << shift );
            update_needed = 1;
	    break;
	}
      }
    }
    buf++;
    written++;
 }
 if ( update_needed )
 {
   update_robotic_arm(robotic_arm);
 }
 return written;

}


static DEVICE_ATTR(parsed, S_IRUGO | S_IWUSR, show_parsed, set_parsed);



static ssize_t show_common(struct device *dev, struct device_attribute *attr,
			   char *buf, u8 index)
{
  struct usb_interface *intf = to_usb_interface(dev);
  struct usb_robotic_arm *robotic_arm = usb_get_intfdata(intf);
  u8 shift, offset;
  shift = ( 2 * ( index & 3 ) );
  offset = ( index >> 2 );

  return sprintf(buf, "%d\n",
    ( robotic_arm->cmd[offset]>>shift ) & 0x03) ;

}




static ssize_t set_common(struct device *dev, struct device_attribute *attr,
			  const char *buf, size_t count, u8 index)
{
  size_t written;
  struct usb_interface *intf = to_usb_interface(dev);
  struct usb_robotic_arm *robotic_arm = usb_get_intfdata(intf);
  u8 shift, offset;
  shift = ( 2 * ( index & 3 ) );
  offset = ( index >> 2 );
  written = 0;
  while ( count-- > 0 )
  {
    char file_data;
    file_data = *buf;
    switch ( file_data )
    {
      case '0' :
      case '1' :
      case '2' : 
        robotic_arm->cmd[offset] &= ~( 0x03 << shift );
        robotic_arm->cmd[offset] |= ( ( file_data & 0x03 )  << shift );
	break;
    }
    buf++;
    written++;
 }
 update_robotic_arm(robotic_arm);
 return written;

}





#define show_set(value, index)						\
static ssize_t show_##value(struct device *dev, struct device_attribute *attr,\
			char *buf)					\
{									\
  return show_common( dev, attr, buf, index );                          \
}                                                                       \
static ssize_t set_##value(struct device *dev, struct device_attribute *attr,\
				const char *buf, size_t count)		\
{                                                                       \
  return set_common( dev, attr, buf, count, index );                    \
}                                                                       \
static DEVICE_ATTR(value, S_IRUGO | S_IWUSR, show_##value, set_##value);


show_set(led, 8);
show_set(gripmotor,0);
show_set(motor2, 1);
show_set(motor3, 2);
show_set(motor4, 3);
show_set(basemotor, 4);

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
	retval = device_create_file(&interface->dev, &dev_attr_parsed);
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
	device_remove_file(&interface->dev, &dev_attr_parsed);

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
	device_remove_file(&interface->dev, &dev_attr_parsed);

	/* first remove the files, then set the pointer to NULL */
	usb_set_intfdata(interface, NULL);

	usb_put_dev(dev->udev);

	kfree(dev);

	dev_info(&interface->dev, "USB ROBOTIC ARM now disconnected\n");
}

static struct usb_driver robotic_arm_driver = {
	.name = "robotic_arm",
	.probe = robotic_arm_probe,
	.disconnect = robotic_arm_disconnect,
	.id_table = id_table,
};

static int __init usb_robotic_arm_init(void)
{
	return usb_register(&robotic_arm_driver);
}

static void __exit usb_robotic_arm_exit(void)
{
	usb_deregister(&robotic_arm_driver);
}

module_init(usb_robotic_arm_init);
module_exit(usb_robotic_arm_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
