# Monteven's Dynamic Extractor (MDE)

MDE is a model extraction tool that converts Destiny 2 dynamic models into fbx files. A dynamic model is one that is animated or is spawned in during the game.
The extractor has:
* partial skeleton support (incorrect rotations)
* texture extraction, DDS -> PNG
* weighted models
* batch extraction for packages
* API hash model extraction, mainly for HD textures for API rips

Things I would like to support in the future:
* better skeleton support with correct rotations
* animations

If you use MDE in any of your projects, it would be highly appreciated if you credit me :)

## Usage

The tool is used via the command line:

MontevenDynamicExtractor -p [packages path] -o [output path] -n [file name] -i [input hash] -t -b [package ID] -a [api hash]

The required arguments are -p and one of -i, -b, or -a. Backslashes will not work for file paths.
* -p [packages path]: package path for Destiny 2. An example might be I:/SteamLibrary/steamapps/common/Destiny 2/packages
* -o [output path]: the output path, by default is the current directory
* -n [file name]: the file name of the target folder and files, by default is the hash provided
* -i [input hash]: the input hash. To get hashes to extract, either use the batch command or the public sheets page
* -t: enable texture extraction
* -b [package ID]: will extract every dynamic model given for a package ID. For the package "w64_sr_combatants_01ba_4", the package ID would be "01ba"
* -a [api hash]: the api hash in numerical form, such as 748485514. Not all API hashes will work since they do not all contain API data

## License/Disclaimer
License/Disclaimer

MDE is licensed under the General Public License 3.0. You are free to use MDE, both it and its source code, under the terms of the GPL. MDE is distributed in the hope it will be useful to, but it comes WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, see the LICENSE file for more information.

This repo is in no shape or form associated with Bungie and its developers. These tools are developed to allow users access to assets for use in awesome stuff. The assets extracted by these tools are property of their respective owners.

## Thanks
Lots of people helped me to get to this stage, including but not limited to:
* Ginsor
* HighRTT
* MrAir
* lowlines
* Thejudsub
* BIOS
* Javano

it's been fun working on Destiny datamining, and I hope I can continue to work on it in the future.