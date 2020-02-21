# QtSense

An open and extendable C++ version of SoundSense for Dwarf Fortress.

### Prerequisites

You will need: Qt 5, OpenAL and "good" GStreamer plugins.

On Arch Linux (you may need more qt packages):
```
# pacman -Sy qt5-base qt5-multimedia gst-plugins-good openal
```

On Debian (untested):
```
# apt install qtbase5-dev libopenal-dev gstreamer1.0-plugins-good
```

### Installing

```
$ git clone https://github.com/EssGeeEich/QtSense.git
$ cd QtSense
$ qmake
$ make && echo You may want to run 'make install' as root now, or './QtSense' without root.
```

### Usage

By default QtSense will need to be configured to read Dwarf Fortress' gamelog.txt file.
It must be pointed to such file through the Settings dialog.

An advanced plugin for dfhack is available in the 'dfhack' folder (all files need to merged within the root dfhack folder).

### How does it work?

QtSense can work in both a "legacy" mode and in a "modern" mode.

#### Legacy Mode

The legacy mode reads the gamelog.txt file and emits sounds as (mostly) per the usual SoundSense rules.
Some rules may work differently between SoundSense and QtSense.

#### Modern Mode

The modern mode is quite different and does not make use of the gamelog.txt file.

QtSense will start a listen server and the dfhack plugin will try to connect to it;
It will send interpretable "commands" to QtSense, and QtSense will act accordingly.

This allows us to use many more sounds without bloating the gamelog.txt file.

By default, QtSense will listen locally on port 27970.
To change the behavior, you can change your settings (you may need to edit qtsense.lua).

#### Screenshot
![Alt text](/../screenshots/Volume.jpg?raw=true "Volume Regolation")
![Alt text](/../screenshots/Packs.jpg?raw=true "Sound Packs Management")
![Alt text](/../screenshots/DebugAndSettings.jpg?raw=true "Debug and Settings screen")
