# gcodetimer
A simple tool to calculate 3D printing times from gcode files.

When given a gcode file as an input, it can be used to create another file with the extension ".timed.gcode" that contains a copy of the original file with extra M117 commands ("Display Message") containing the remaining time at 1-second intervals. This can then be used to print files from an SD card, ie. without host software on a PC, while displaying the remaining time.

# Usage
## Dependencies
* cmake
* boost (filesystem, tokenizer, property_tree)

## Building
Use cmake to build this project. I've created a very simple "build.sh" script that builds gcodetimer under linux, but building it with custom parameters or under another operating system should be trivial:
* Create an empty build folder anywhere and navigate to it
* run "cmake <path to the gcodetimer src folder>"
* run "make"

## Running
gcodetimer ([-i|--info] [-o|--output <output file>] [-s|--stdout] <gcode file> [<gcode file> ...] | --create-config)

  -i, --info: Only print the estimated time for each file, do not generate gcode
  
  -o, --output: Sets the output filename. Can only be used with a single input file
  
  -s, --stdout: Prints the generated gcode to stdout instead of saving it to a file.
                   Can only be used with a single input file. -o will be ignored
                   
  --create-config: Generates or completes the config file with any missing defaults

If -o is not specified, the program will create a file of the new name with a '.timed' suffix
  for each input file

# Configuration
In order to calculate the remaining time, this software requires some of your printer's parameters. To get started, run
~~~
gcodetimer --create-config
~~~
This will create a config file in the appropriate folder depending on your operating system:
* Windows: %appdata%\gcodetimer\config.xml
* Linux: $HOME/.config/gcodetimer/config.xml
* Mac OS: $HOME/Library/Application Support/gcodetimer/config.xml

Edit this file according to your needs. The configuration parameters are:
* max_print_accel: The maximum acceleration of your printer during print moves (per axis)
* max_move_accel: The maximum acceleration of your printer during non-print moves (per axis)
* max_jerk: The maximum jerk (instantaneous speed change) settings of your printer
* jerk_efficiency: A factor for the heuristic used to calculate the changes in speed. Start with a value 1 and edit it later on if the timing is off. This factor mostly relates to the effectiveness of the path planning algorithms of your printer's firmware, including the amount of moves it buffers.
* accel_efficiency: Shrinks or grows "max_move_accel" and "max_print_accel". The idea behind this factor is that your printer's processor might not have the processing speed to always drive the motors at the specified maximum values. Start with a value 1 and edit it later on if the timing is off.
* speed_multiplier: This scales the speed of every move. Start with a value 1 and edit it later on if the timing is off.


# Limitations and Hints
 * The time estimation is very simple. It works very well for my printer (approximately +-2 minutes per printing hour), but you might get different results
 * The M117 command is not standard, so this might not work for all printers. Check http://reprap.org/wiki/G-code#M117:_Display_Message *before* using this software!
 * The Repetier firmware (V0.92.9) does not display M117 messages when printing from SD card by default. Check my fork of the firmware for the necessary changes: https://github.com/gonzalezjj/Repetier-Firmware/tree/feature/m117_in_sd_mode
