# Leapify

> This driver is work in progress, there may be unexpected / buggy behaviour please report it, [here](https://github.com/Nyabsi/driver_leapify/issues).

SteamVR for Leapmotion Controller 1 & 2 (*And other compatible controllers*) which strives for seamless co-existing with controllers.

# Prerequisites

- Leapmotion controller (LMC1, LMC2, IR170, etc..)
- HMD, this driver needs a Headset to work, this is a *SteamVR* limitation.

# Installation

### Step 1

Install [Ultraleap Software](https://developer.leapmotion.com/tracking-software-download) for your device (use Gemini for LMC 1, Hyperion for LMC 2)

*If you have issues activating Hyperion license, allow `LeapSvc.exe` in your firewall, if it still doesn't work. Contact Ultraleap directly.

### Step 2

Download [the driver](https://github.com/Nyabsi/driver_leap/releases) and extract it to `C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR\\drivers\\` 

(NOTE: this path may vary from your Steam installation location and choice of operating system)

### Step 3

Start SteamVR and it should be working.

# Features

- Cross-platform support
- Switching between controllers and Hand Tracking
- Hand offset estimation
- Settings integration into SteamVR

# Advanced Features

- Skeletal data passthrough
- Positional data passthrough (only for Extensions)

# Configuration

> On Desktop

Navigate to SteamVR -> Settings -> Leapify

> On VR

Navigate to SteamVR Settings -> Leapify

![image](https://github.com/user-attachments/assets/8b8329f7-86b5-4209-9e6f-2a6bcf6c2431)

### Hand Tracking

> Toggles Hand Tracking

- Enabled Behavior: Enables Hand Tracking
- Disabled Behavior: Disables Hand Tracking

### Hand Tracking as Fallback

> Prefers controllers over Hand Tracking

- Enabled Behavior: If SteamVR has connected controller, uses them instead of Hand Tracking, when SteamVR has no connected devices, enables Hand Tracking.
- Disabled Behavior: Hand Tracking is always enabled if "Hand Tracking" is enabled.

### Automatic Hand Offset

> Attempts roughly estimation for Hand Offset relative to the Leapmotion sensor

- Enabled Behavior: Automatically sets adjusted Hand Offset for the Hand Tracking.
- Disabled Behavior: Allows you to manually adjust Hand Offset for the Hand Tracking.

## Advanced Configuration

### Skeletal Data Passthrough

> Redirects leap skeletal data to every connected SteamVR device labeled as controller.

- Enabled Behavior: Redirects leap skeletal data to other devices.
- Disabled Behavior: Does nothing.

- ### Positional Data Passthrough

> Redirects leap positional data to every connected SteamVR device labeled as controller.

- Enabled Behavior: Redirects leap positional data to other devices.
- Disabled Behavior: Does nothing.

### Adjust Mounting Offset

> When enabled you can configure the physical mounting position of the Leapmotion Controller to better match the Headset position in SteamVR space.

- Enabled Behavior: Allows you to configure X,Y,Z mounting axises of the Leapmotion.
- Disabled Behavior: Does nothing.

# Building

This project is built with CMake and Visual Studio 2022 on Windows.

You can use instructions below for building on Linux.

```sh
git submodule init && git submodule update
cmake -B build .
cmake --build build
```

# Credit

- thanks to @SDraw for making the original `driver_leap` which this project uses some parts of.


# Dependencies

- [OpenVR](https://github.com/ValveSoftware/openvr) by @ValveSoftware
- [rcmp](https://github.com/Smertig/rcmp) by @Smertig
- [glm](https://github.com/g-truc/glm) by @g-truc

# License

The code is licensed under `MIT` you can view the full license [here](LICENSE)
