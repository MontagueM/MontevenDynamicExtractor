# Building

Only Windows is supported at the moment, due to oodle.

## Setup the environment

* [FBX SDK 2020+](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-3)
* [vcpkg](https://vcpkg.io/) (for DirectXTex)

**Add the FBX SDK install directory to an environment variable called** `FBXSDK_PATH` **e.g.** `C:\Program Files\Autodesk\FBX\FBX SDK\<version>`**.**

## Setup the project

Clone the repository

`git clone https://github.com/nblockbuster/MontevenDynamicExtractor.git`

Follow the instructions to setup vcpkg [here](https://vcpkg.io/en/getting-started.html)

Make sure to run the command `vcpkg install directxtex` to install DirectXTex, and `vcpkg integrate install` for Visual Studio to recognize it.

#### Building the project

Open `DestinyDynamicExtractor.sln`. You most likely want to build on the `Release` configuration.

After it opens:

`Build > Build Solution`

### Post-Build

After you build the solution, make sure to copy these files to the output directory:
* `oo2core_9_win64.dll` from your Destiny 2's `/bin/x64` directory
* `libfbxsdk.dll` from `<FBX SDK Path>/<version>/lib/vs2019/x64/release`