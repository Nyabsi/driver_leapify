# Leapify

> This driver is work in progress, there may be unexpected / buggy behaviour please report it, [here](https://github.com/Nyabsi/driver_leapify/issues).

SteamVR for Leapmotion Controller 1 & 2 (*And other compatible controllers*) which strives for seamless co-existing with controllers.

## Prerequisites

- Leapmotion controller (LMC1, LMC2, IR170, etc..)
- HMD, this driver needs a Headset to work, this is a *SteamVR* limitation.

## Installation

### Step 1

Install [Ultraleap Software](https://developer.leapmotion.com/tracking-software-download) for your device (use Gemini for LMC 1, Hyperion for LMC 2)

*If you have issues activating Hyperion license, allow `LeapSvc.exe` in your firewall, if it still doesn't work. Contact Ultraleap directly.

### Step 2

Download [the driver](https://github.com/Nyabsi/driver_leap/releases) and extract it to `C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR\\drivers\\` 

(NOTE: this path may vary from your Steam installation location and choice of operating system)

### Step 3

Start SteamVR and it should be working.

## Features

- Cross-platform support
- Switching between controllers and Hand Tracking
- Hand offset estimation
- Settings integration into SteamVR

## Advanced Features

- Skeletal data passthrough
- Positional data passthrough (only for Extensions)

## Extensions

- TBA

## Configuration

> On Desktop

Navigate to SteamVR -> Settings -> Leapify

> On VR

Navigate to SteamVR Settings -> Leapify

![image](https://github.com/user-attachments/assets/1385c29a-2ecc-4f02-8c97-a0d62c384f77)

## Building

This project is built with CMake and Visual Studio 2022 on Windows.

You can use instructions below for building on Linux.

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
