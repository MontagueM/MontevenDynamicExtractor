# Building

Only Windows is supported at the moment.

## Setup the environment

* [FBX SDK 2020+](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-3)
* [OpenCV 4+](https://opencv.org/releases/)

**Add the FBX SDK and OpenCV install directories to an environment variable called** `FBXSDK_PATH` **and** `OPENCV_PATH` **respectively, e.g.** `C:\Program Files\Autodesk\FBX\FBX SDK\<version>` **and** `C:\Program Files\OpenCV` **.**

If you update OpenCV past 4.6.0, make sure to go into the DestinyDynamicExtractor project settings, and change the OpenCV version to what version you downloaded in `Linker > Input`.

## Setup the project

Clone the repository

`git clone https://github.com/nblockbuster/MontevenDynamicExtractor.git`

#### Building the projects

Open `DestinyDynamicExtractor.sln`. You most likely want to build on the `Release` configuration.

After it opens:

`Build > Build Solution`

### Post-Build

After you build the solution, make sure to copy these files to the output directory:
* `oo2core_9_win64.dll` from your Destiny 2's `/bin/x64` directory
* `libfbxsdk.dll` from `<FBX SDK Path>/<version>/lib/vs2019/x64/release`
* `opencv_world<version>.dll` from `<OpenCV Path>/build/x64/vc15/bin`
* [`texconv.exe`](https://github.com/microsoft/DirectXTex/releases/latest/)