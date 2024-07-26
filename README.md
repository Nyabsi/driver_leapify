# Leapify

> This driver is work in progress, there may be unexpected / buggy behaviour please report it, [here](https://github.com/Nyabsi/driver_leapify/issues).

SteamVR for Leapmotion Controller 1 & 2 (*And other compatible controllers*) which strives for seamless co-existing with controllers.

## Installation

### Step 1

Install [Ultraleap Software](https://developer.leapmotion.com/tracking-software-download) for your device (use Gemini for LMC 1, Hyperion for LMC2)

### Step 2

Download [the driver](https://github.com/Nyabsi/driver_leap/releases) and extract it to `C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR\\drivers\\` 

(NOTE: this path may vary from your Steam installation location, or choice of operating system)

### Step 3

Start SteamVR and it should be working.

## Features

- Cross-platform (Windows, Linux)
- Fallback to SteamVR controllers, if Hand Tracking is disabled
- Hand offset estimation
- Automatically Switch between Controllers and Hand Tracking
- Skeletal data passthrough (use Leapmotion skeletal data with any other controller)
- Settings integration into SteamVR

## TODO

- Index Emulation for legacy SteamVR Input applications
- Add Joycon Input back
- System Button + Trigger binds

## Configuration

> On Desktop

Navigate to SteamVR -> Settings -> Leapify

> On VR

Navigate to SteamVR Settings -> Leapify

![settings](https://github.com/user-attachments/assets/d3bb0dcf-863d-48ff-b028-b53fffc6d3e9)

## Building

This project is built with CMake and Visual Studio 2022 on Windows. For Linux, you can use instructions below for building on Linux.

```sh
git submodule init && git submodule update
cmake -B build .
cmake --build build
```

## Credit

- thanks to @SDraw for making the original `driver_leap` which this project uses some parts of.


## Dependencies

- [OpenVR](https://github.com/ValveSoftware/openvr) by @ValveSoftware
- [rcmp](https://github.com/Smertig/rcmp) by @Smertig
- [glm](https://github.com/g-truc/glm) by @g-truc

## License

The code is licensed under `MIT` you can view the full license [here](LICENSE)
