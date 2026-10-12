# 3dCutscenes
A C engine to display 3d-looking cutscenes

# How it works
This tool loads one of many 256-color image to the screen during the execution of a script. Upon pressing any button the overworld is shown and the original script is resumed.

The tool uses a lookup table containing the addresses of the various images inserted in the ROM.

The script requires the images to be pre-built via grit using: `grit INPUTFILE -gB16 -gb -gzl -aw240 -gTFFF600 -ftb`. (options meaning are: 16bpp, bitmap, graphics lz77 compressed, area width is 240, #0xFFF600 is trasparency, binary output file type)
The resulting binary string must be inserted at a 4-aligned address of your choosing, and that address must be inserted into the lookup table.

To use this tool insert a similar script:
```
... stuff before the image is shown ...

setvar 0x8002 IMAGE_ID
callasm MAIN_ADDR
waitstate

... stuff after the image is shown ...
```

where `IMAGE_ID` is the image index in the lookup table (starting from 0) and `MAIN_ADDR` is the address of the `main` function +1 (more on that later)

# How to use
This tool requires:
- python (tested with 3.8.3)
- [grit](https://www.coranac.com/projects/grit/) 0.8.6+
- [devkitARM](https://devkitpro.org/wiki/devkitARM)

Once you have all the necessary software installed and ready to go, just navigate to the root of this project and copy/paste your ROM there. Rename the file to `BPRE0.gba`.  
Open the terminal, navigate to the root of the project, then run `python scripts/build.py 0xFFFFFFFF`, where `0xFFFFFFFF` is the free space in your ROM, and the script should insert everything, creating two new files:
- `multi.gba`: the updated ROM
- `symbols.txt`: a text file containing all the addresses known to the compiler. Conserve this file to know where everything in your ROM was inserted

From the symbols.txt file, retrieve the address of the main function (it should look like a line containing `088007D4 main`), this is what will be used in the callasm shown above.

## Compiling with WSL
Thanks to @jmynes for the addition.  
The project can be compiled in a WSL envrionment by adding the `-wsl` flag to the `python scripts/build.py 0xFFFFFFFF` command.  
Here is a list of commands that will install all the needed dependencies and build the project.
```
git clone https://github.com/DgZiga/FR-ShowImage.git
cd FR-3dCutscenes
git clone https://github.com/devkitPro/grit/
cd grit
sudo apt-get install autoconf automake libtool libfreeimage-dev
./autogen.sh
./configure
make
sudo make install
git clone https://github.com/Kingcom/armips.git
cd armips
mkdir build
cd build
git submodule update --init --recursive
cmake ..
make
sudo cp armips/build/armips /usr/local/bin/
cd ../..
python3 scripts/build.py 0x08A00000 -wsl
```
