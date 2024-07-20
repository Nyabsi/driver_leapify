# Leapify

SteamVR for Leapmotion Controller 1 & 2 (*And other compatible controllers*) which strives for seamless co-existing with controllers.

## Installation

### Step 1

Install [Ultraleap Software](https://developer.leapmotion.com/tracking-software-download) for your device (use Gemini for LMC 1, Hyperion for LMC2)

### Step 2

Download [the driver]([../../releases/latest](https://github.com/Nyabsi/driver_leap/releases)) and extract it to `C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR\\drivers\\driver_leapify` 

(NOTE: this path may vary from your Steam installation location, or choise of operating system)

### Step 3

Start SteamVR and it should be working.

## Features

- Support for multiple operating systems (Windows and Linux)
- No configuration, driver works out of box, without intrusive overlays or configuration
- Support for Joycons and Dualshock (if existing controllers are not found)
- Estimates approximately correct hand position automatically for the user
- Automatic CI builds for latest driver improvements through Github

## TODO

- [ ] If existing controllers are found, Leapify will use those for inputs, and forwards only hand tracking data to games
- [ ] Refactor the driver code to improve code uniformity across files
- [ ] Support for Xbox Controllers

## Building

This project is built with CMake and Visual Studio 2022 on Windows. For Linux, you can use instructions below for building on Linux.

```sh
git submodule init && git submodule update
cmake -B build .
cmake --build build
```

## Credit

- thanks to @SDraw for making the original `driver_leap` which this project is built upon.

## License

The code is licensed under `MIT` you can view the full license [here](LICENSE.md)
