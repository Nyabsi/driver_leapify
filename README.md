# Driver Leap

fork of driver_leap with Controller and Linux support.

## Installation (for users)

* Install [latest Ultraleap Software](https://developer.leapmotion.com/tracking-software-download) for your device (Gemini *or* Hyperion)

* Extract [latest release archive](../../releases/latest) to `<SteamVR_folder>/drivers`

* Add the line below in the `steamvr` section at  `<Steam_folder>/config/steamvr.vrsettings` (unless it already exists, OpenVR-SpaceCalibrator would automatically set this)

```json
"activateMultipleDrivers": true,
```

## Features

This driver has few differences compared to the [upstream driver](https://github.com/SDraw/driver_leap)

- Support for Controllers (Joycons, DualShock) instead of using SteamVR overlay, making input more natural
- Support for Linux, so you can use this on your Linux system
- Automatically set offset hand offset relative to your arm length
- Switching from Visual Studio project to CMake project for broader platform compatibility
- And other tweaks in future

## TODO

- [x] Convert buildsystem from VCXProj to CMake
- [x] Multi-platform support (Linux)
- [x] Support other types of input devices (IE. DualShock or Xbox Controller)
- [x] Support older Leapmotion Controllers
- [ ] Automated CI for driver builds
