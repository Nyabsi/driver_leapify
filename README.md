# Leapify

> This driver is work in progress, it may have bugs if so, report it [here](https://github.com/Nyabsi/driver_leapify/issues).

SteamVR for Leapmotion Controller 1 & 2 (*And other compatible controllers*) which strives for seamless co-existence with existing controllers.

If you appreciate the work I do, consider buying me coffee over at [Ko-fi](https://ko-fi.com/nyabsi) ^_^

## Known Issues

- SteamVR will crash when `driver_leapify` is enabled and SteamVR cannot find HMD this is caused by a SteamVR bug.
- When switching between controllers, while not being in-game inputs may not register, this is a SteamVR bug.

## Installation

### Step 1

Install [Ultraleap Software](https://developer.leapmotion.com/tracking-software-download) for your device (use **Gemini** for LMC 1, **Hyperion** for LMC 2)

NOTE: If you have issues activating Hyperion license, please allow `LeapSvc.exe` to connect to the internet through firewall.

### Step 2

Download [the driver](https://github.com/Nyabsi/driver_leap/releases) and extract it to `C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR\\drivers\\` 

(NOTE: this path may vary from your Steam installation location, or choice of operating system)

### Step 3

Start SteamVR and the driver should be working.

## Features

- Cross-platform support (Windows and Linux)
- Automatic swapping between Hand Trackind and Controllers
- Adds spacing between your Hand and the Sensor for more natural interaction
- Skeletal data passthrough (use Leapmotion skeletal data with other controllers)
- Integrates into SteamVR, no external configuration.

## Configuration

> On Desktop

Navigate to SteamVR -> Settings -> Leapify

> On VR

Navigate to SteamVR Settings -> Leapify

![settings](https://github.com/user-attachments/assets/d3bb0dcf-863d-48ff-b028-b53fffc6d3e9)

## Building

This project is built with CMake and Visual Studio 2022 on Windows. 

For Linux, you can use instructions below:

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
