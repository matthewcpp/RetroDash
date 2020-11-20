### Building for Nintendo 64

You will need to have nodejs and docker installed.

1. Ensure you have built the `mksprite` tool from the [libdragon](https://github.com/DragonMinded/libdragon) repo.  It is loacted in the `tools/mksprite` directory.
1. From the root of the repo run `npm install` to install the [libdragon docker container](https://github.com/anacierdem/libdragon-docker).
1. Build the assets by running `node tools/build_n64.js` passing the path to the `mksprite` executable built above.
1. Run `npx libdragon start` to start the docker container.
1. Run `node tools/prepare_n64_assets.js /path/to/libdragon/tools/mksprite/mksprite` to prepare the game assets.
1. Run `npx libdragon make` to build the ROM file.
1. Run `npx libdragon make clean` to clear build artifacts.


### Testing with cen64

cen64 can easily be built from source on linux and MacOS with [cmake](https://cmake.org/). 
On macOS you will need to install xquartz.  You can run `brew cask install xquartz` to easily install it.

1. Clone [cen64](https://github.com/n64dev/cen64) from github.
1. Create a build directory: `cd cen64 && mkdir build && cd build`
1. Build the emulator: `cmake .. && make`

In order to run the emulator you will need to get access to a `pifdata.bin` BIOS file.
Test the game by invoking the emulator from an `xQuartz` terminal window: 
```shell script
	./cen64 -multithread /path/to/pifdata.bin /path/to/ROM.z64
```

### Building for Desktop

You will need to have cmake, nodejs, and sdl installed.

MacOS: `brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer`  
Ubuntu Linux: `sudo apt-get -y install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev`

```shell script
node tools/prepare_sdl_assets.js
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```
