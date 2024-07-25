# Leapify

> This driver is work in progress, there may be unexpected / buggy behaviour please report it, [here](https://github.com/Nyabsi/driver_leapify/issues).

SteamVR for Leapmotion Controller 1 & 2 (*And other compatible controllers*) which strives for seamless co-existing with controllers.

## Installation

You can download latest experimental builds from [actions](https://github.com/Nyabsi/driver_leapify/actions).

## Features

- Cross-platform (Windows, Linux)
- Index Emulation for legacy SteamVR Input applications
- Fallback to SteamVR controllers, if Hand Tracking is disabled
- Hand offset estimation
- Settings integration into SteamVR

## Configuration

Navigate to SteamVR -> Settings -> Leapify

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
