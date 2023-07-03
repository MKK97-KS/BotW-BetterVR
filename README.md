# BotW-BetterVR

A project aimed at providing a PC VR mode for Breath of the Wild using the Wii U emulator called Cemu. To provide the
better experience it tries to run the game at high framerates and resolutions.

### Temporary Build Instructions

1. Install [vcpkg](https://github.com/microsoft/vcpkg) and
   use `vcpkg install openxr-loader:x64-windows-static-md eigen3:x64-windows-static-md glm:x64-windows-static-md vulkan-headers:x64-windows-static-md`

2. Install the latest Vulkan SDK from https://vulkan.lunarg.com/sdk/home#windows and make sure that VULKAN_SDK was added
   to your environment variables.

3. [Optional] Download and extract a new Cemu installation to the Cemu folder that's included. Step technically not
   required, but it's the default install location and makes debugging much easier.

4. Use Clion or Visual Studio to open the project. If you do use Clion (recommended), you'll still need to have Visual
   Studio installed to use it as the compiler.

5. If you want to use it outside visual studio, you can go to the /[cmake-output-folder]/bin/ folder for the
   BetterVR_Layer.dll. The `BetterVR_Layer.json` and `Launch_BetterVR.bat` can be found in the [resources](/resources)
   folder. Then you can launch Cemu with the hook using the Launch_BetterVR.bat file to start Cemu with the hook.

### Licenses

This project is licensed under the MIT license.
It also includes [vkroots](https://github.com/Joshua-Ashton/vkroots/blob/main/LICENSES/MIT.txt).


---

### Remaining text have to be updated for the new project

### Requirements

#### Supported VR headsets:

The app currently utilizes OpenXR, which is supported on all the major headsets (Valve Index, HTC Vive, Oculus Rift,
Windows Mixed Reality etc). You can also use Trinus VR to use this with your phone's VR accessory.

For Oculus headsets you have to switch to the Public Test Channel for OpenXR support.

For Trinus VR you'll have to enable OpenXR in the Trinus VR settings menu.

#### Other Requirements:

* A pretty decent PC with a good CPU (a recent Intel i5 or Ryzen 5 are recommended at least)!

* A copy of BotW for the Wii U

* A properly set up [Cemu](http://cemu.info/) emulator that's already able to run at 60FPS or higher.
  See [this guide](https://cemu.cfw.guide/) for more info.

### Mod Installation

// todo: Fully finish these instructions

1. Download the latest release of the mod from the [Releases](https://github.com/Crementif/BotW-BetterVR/releases) page.

2. Extract the downloaded `.zip` file where your `Cemu.exe` is stored.

3. Double click on `BetterVR-Launcher.bat` to start Cemu.

4. Go to `Options`-> `Graphic packs`-> `The Legend of Zelda: Breath of the Wild` and check the graphic pack
   named `BetterVR`.

5. Start the game like you normally would.

6. Go to `Options`->`Fullscreen` to make the game fullscreen or alternatively press `Alt+Enter`.

7. Put the VR Headset on your head and enjoy.

Each time you want to play the game in VR from now on you can just use the `BetterVR-launcher.bat` file to start it in
VR mode again.

You can leave the graphic pack enabled while playing in non-VR without any issue.

### Building

1. Make sure that you've got installed.

2. Install the required dependencies using `vcpkg install openxr-loader:x64-windows eigen3:x64-windows glm:x64-windows`.

3. Open the project using Visual Studio and build it.

The OpenXR application has three different modes that you can change in the `BotWHook.cpp` file:

- `GFX_PACK_PASSTHROUGH` is only useful for releasing. It moves the camera computations from the app to the compiled
  code in the graphic pack itself to reduce the input latency by asynchronously updating the headset positions.

- `APP_CALC_CONVERT` is used as an intermediary form of the library-dependent code and is able to be compiled into a
  graphic pack patch that works with the `GFX_PACK_PASSTHROUGH` code.

- `APP_CALC_LIBRARY` will be the most useful mode for most people. It allows you to interact with the camera using the
  eigen and glm libraries so that it's easy to develop and experiment with.
