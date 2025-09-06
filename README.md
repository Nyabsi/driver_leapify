# Leapify

SteamVR driver for Leapmotion Controller 1 & 2 *and other compatible controllers*.

> [!IMPORTANT]
> Leapify is an experimental software, it may contain bugs, crashes and other unforeseen issues. If you encounter any issues or you have ideas for improvement, make an [issue](https://github.com/Nyabsi/driver_leapify/issues).

## Features

- Cross-platform support
    - Windows & Linux
- Seamless switching with other input devices
- Hand offset estimation
- Native SteamVR integration
    - Menu integration
    - Gesture input
- Pose Passthrough

## Installation (Windows only!)

### 1. Ultraleap Software

Install [Ultraleap Software](https://www.ultraleap.com/downloads/) for your device (use Gemini for LMC 1, Hyperion for LMC 2)

*If you have issues activating Hyperion license, allow `LeapSvc.exe` in your firewall. If it still doesn't work, contact Ultraleap directly.

### 2. Leapify Installation

Download the installer from [releases](https://github.com/Nyabsi/driver_leap/releases) and install the driver.

### 3. Launching SteamVR

Ensure the **Ultraleap Software** is running and then start SteamVR and the driver should be running.

## Compiling

This project is built with CMake and Visual Studio 2022 on Windows.

You can use instructions below for building on Linux:

```sh
git submodule init && git submodule update
cmake -B build .
cmake --build build
```

## Special Mentions

- thanks to @Ultraleap for creating such amazing product.
- thanks to @SDraw for making the original `driver_leap` which this project uses some parts of.

## Dependencies

- [OpenVR](https://github.com/ValveSoftware/openvr) by @ValveSoftware
- [rcmp](https://github.com/Smertig/rcmp) by @Smertig
- [glm](https://github.com/g-truc/glm) by @g-truc

## License

The code is licensed under `MIT` you can view the full license [here](LICENSE)
