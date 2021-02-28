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

On Fedora/CentOS:
```
# dnf install qt5-qtdeclarative-devel qt5-qtmultimedia-devel gstreamer1-plugins-good* openal-soft-devel
```

### Installing

```
$ git clone https://github.com/EssGeeEich/QtSense.git
$ cd QtSense
$ qmake # qmake-qt5 on Fedora/CentOS
$ make && echo You may want to run 'make install' as root now, or './QtSense' without root.
```

### Installing Sounds

Sounds may be anywhere on your system (you will need to set up the path in QtSense's Settings page).

Please note that **traditional SoundSense files will not work**.

You can, however, download modern SoundSense files and XML files from the following links:

* [XML files](https://github.com/EssGeeEich/QtSense/zipball/sounds/)
* [Sound files](https://github.com/EssGeeEich/QtSense/releases/download/Latest/packs.zip)

#### Why different files?

The difference stems from the fact that this project aims to integrate in a much deeper way into Dwarf Fortress. As such, most XML files will need to be edited to support new features.

I took this opportunity to fix most typos in those XML files and to define in a much clearer way the range of some values.

#### Why different links?

Git is not made for binary files. As such, binary files are uploaded as binary release files.

XML files are text files and are easily versionable.

The [sounds](https://github.com/EssGeeEich/QtSense/tree/sounds) repository is dedicated to those XML files and allows for easier community contributions.

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
