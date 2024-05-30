# Driver Leap

fork of driver_leap with improvements for my personal-use.

**NOTE: For now, this fork only supports Leap Motion Controller 2 and Ultraleap Hyperion, it will not work with older software/hardware.** 

## Installation (for users)

* Install [latest Ultraleap Hyperion](https://developer.leapmotion.com/tracking-software-download)
* Extract [latest release archive](../../releases/latest) to `<SteamVR_folder>/drivers`
* Add line in section `steamvr` of `<Steam_folder>/config/steamvr.vrsettings` file:
```JSON
"activateMultipleDrivers": true,
```

## Usage

1. Connect Joycons with Bluetooth
2. Start the driver
3. Profit???

## TODO

- [x] Convert buildsystem from VCXProj to CMake
- [x] Multi-platform support (Linux)
- [ ] Support other types of input devices (IE. DualShock or Xbox Controller)
- [x] Support older Leapmotion Controllers
