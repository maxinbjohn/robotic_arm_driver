use std::fs::File;
use std::io::prelude::*;
use std::thread;
use std::time;
use capturing_glob::glob_with;
use capturing_glob::MatchOptions;

fn find_usb_device() -> String {
    let options = MatchOptions {
    case_sensitive: false,
    require_literal_separator: false,
    require_literal_leading_dot: false,
    };

    for entry in glob_with("/sys/bus/usb/drivers/robotic_arm/*:*", &options).unwrap() {
        if let Ok(entry) = entry {
           return entry.path().display().to_string()
        }
    }

    panic!("Please check if device is attached or we have the privilege to access it !");
}

fn robotic_arm_led_on() -> std::io::Result<()> {
    let robotic_arm_led = format!("{}{}", find_usb_device(), "/led");
    let mut f = File::create(robotic_arm_led)?;
    f.write_all(b"1")?;
    f.sync_all()?;
    Ok(())
}

fn robotic_arm_led_off() -> std::io::Result<()> {
    let robotic_arm_led = format!("{}{}", find_usb_device(), "/led");
    let mut f = File::create(robotic_arm_led)?;
    f.write_all(b"0")?;
    f.sync_all()?;
    Ok(())
}

fn base_rotate_clockwise() -> std::io::Result<()> {
    let robotic_arm_basemotor = format!("{}{}", find_usb_device(), "/basemotor");
    let mut f = File::create(robotic_arm_basemotor)?;
    f.write_all(b"1")?;
    f.sync_all()?;
    Ok(())
}

fn base_rotate_anticlockwise() -> std::io::Result<()> {
    let robotic_arm_basemotor = format!("{}{}", find_usb_device(), "/basemotor");
    let mut f = File::create(robotic_arm_basemotor)?;
    f.write_all(b"2")?;
    f.sync_all()?;
    Ok(())
}

fn base_rotate_stop() -> std::io::Result<()> {
    let robotic_arm_basemotor = format!("{}{}", find_usb_device(), "/basemotor");
    let mut f = File::create(robotic_arm_basemotor)?;
    f.write_all(b"0")?;
    f.sync_all()?;
    Ok(())
}

fn main() {

    let time_sleep = time::Duration::from_secs(1);

    match robotic_arm_led_on() {
        Ok(()) => println!("Led ON"),
        Err(err) => eprintln!("error: {}", err),
    };

    thread::sleep(time_sleep);
    match robotic_arm_led_off() {
        Ok(()) => println!("Led OFF"),
        Err(err) => eprintln!("error: {}", err),
    };

    thread::sleep(time_sleep);
    match base_rotate_clockwise() {
        Ok(()) => println!("Base clockwise rotation"),
        Err(err) => eprintln!("error: {}", err),
    };

    thread::sleep(time_sleep);
    match base_rotate_anticlockwise() {
        Ok(()) => println!("Base anticlockwise rotation"),
        Err(err) => eprintln!("error: {}", err),
    };

    thread::sleep(time_sleep);
    match base_rotate_stop() {
        Ok(()) => println!("Base motor OFF"),
        Err(err) => eprintln!("error: {}", err),
    };
}
