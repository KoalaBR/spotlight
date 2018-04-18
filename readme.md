# Spotlight

This program downloads wallpapers from Spotlight (Windows 10), Chromecast and
Bing according to the user's configuration. It will store the images in a
directory named "download" in the default picture path. For Windows this would be something like `c:\Users\%Username%\Pictures` while under Linux this could be `~/Pictures`.

You can select which orientation (landscape / portrait) the images should have
in order to be considered for download.

Additionally you can select whether to show titles (if available) or not.


## Technical information

This project was created with Qt-Creator (4.4.1) and Qt 5.10.1 on both Windows 10
and Linux (Devuan ascii/ceres). On Windows the Visual C++ compiler was used to
create a 64 bit based binary. However, it should work fine with Mingw too.

If you are using Visual C++ as your compiler make sure that the LIBS paths in
*Spotlight.pro* are correct for your system.

### Getting it to run

First thing, check *Spotlight.pro* for additional paths and libraries. 
Basically this means, to change LIBS and INCLUDEPATH variables
for correct paths. On Windows with MS VC++ make sure that the SDK paths 
are correct (look below the line "# Only for Visual Studio 2016").

If you use Qt-Creator then the build process should be straight 
forward now: Just load *Spotlight.pro*, configure your kit(s) and
build.

#### Linux

If you want to build it via commandline, enter the source directory and do
`qmake Spotlight.pro` followed by `make release`. You should find
a `deploy` subdirectory including all files needed to start Spotlight.

#### Windows

For Windows use either the x86 or x64 Native tools command line prompt from your 
Visual Studio install. Enter `qmake.exe Spotlight.pro` followed by 
`jom.exe release` to create a deploy directory which contains all files you need
to start Spotlight.exe.


For https to work on Windows, Qt needs `libeay32.dll` and `ssleay32.dll`. 
Windows doesn't provide those - you need to download them. 

Additionally you will need `libcrypto-1_1-x64.dll` and `libssl-1_1-x64.dll`.
You can get those too from the latest Full Installer for OpenSSL.



Just download openssl 1.1.x and extract those files from
the archive. You find a download list on the 
[OpenSSL Wiki](https://wiki.openssl.org/index.php/Binaries). Depeding on 
your target architecture you need either the 32 or 64 Bit version.

For Linux, your distribution should provide those - if not install openssl.


### Add Reverse Image Search ###

Spotlight can use Google Reverse Image Search to find out about an
image without title. This option needs both curl lib and include files
installed on your system and adds a whole bunch of additional
Qt libraries to the deploy dir. Because of that, I excluded the 
feature by default.

To enable it, open `Spotlight.pro` and uncomment the line

`# DEFINES += REVERSE_IMAGE` 

at the start of the file. Now you can just follow the instructions
from *Getting it to run*.

If you compile reverse image search on Windows the build process will
look for curl in `c:\projekte\curl\curl.dll` and 
`c:\projekte\curl\include` .

If you want another base directory, add a file `curl.config` and
enter the directory path.

## Icons

Icons are from the tango icon set or derived from it. Additional parts from [wikimedia commons](https://commons.wikimedia.org/wiki/File:Pictograms-nps-recycling.svg)
 
Both tango icons and wikimedia icon are in the public domain.

# Todo

Quite a bit currently works, but some things are still missing:

* try to reload an accidentally deleted image (if possible)

# Basics

Windows 10 randomly offers login / lock screen images and fitting titles. Those pictures
can be downloaded by external programs too. Similar APIs exist for Chromecast and Bing.

Feature|Bing|Chromecast|Spotlight
-------------|----|----------|---------
Random images|No |Yes       |Yes
Image titles|Yes|No|Yes
Portrait|No|?|Yes

While Spotlight and Chromecast have a pool of available images and return a random list, Bing
only has the images available from today and 7 days back. Earlier images are no longer available.

# How to use

As already noted above, the program currently stores the images in a directory called
`download` which will be automatically created on program start. The program allows you 
to choose the orientation (portrait, landscape or both). 

Additionally you can switch on or off titles and change the display mode. By default all
images are displayed as a table of tumbnails (on the right side). However you may tag your
images and switch display mode to *by tags like directories* and the images will be grouped
according to the tags you assigned.

The button next to the display mode selection allows you to rename, delete or add tags as you wish. 

Finally we have the buttons *Image Dir*, *search* and *Image*. Search or `ALT+S` will start a search for new images 
(which may not be successful). As the program shows a downloaded image in the background
every 10 seconds you may be interested in seeing the complete picture. In that case press
*Image* or type `ALT+B` to hide most of the GUI except for the *back* button which will show the GUI
again. Either clicking *Back* or typing `ESCAPE` will bring back the default GUI.
The button *Image Dir* or `ALT+D` opens the location in a file explorer where Spotlight stored the images. 

## Context menu

The thumbnail table offers a context menu if you right click on images. It allows you to

* Set the given image as background.
  Please note that only Windows (7/10) and KDE was tested / implemented as this is what
  I use. Feel free to offer patches for your desktop environment.
* Delete an image. The images will be deleted from the file system but remain in the 
  database. Using this stored data prevents the program from downloading the picture
  again.
* reload: This will undo a deletion and download the image again resetting its status.
  In case of Bing this may not be possible.  (This feature is not yet implemented)
* Tag an image. You can use more than one tag on an image.
* Show: shows the selected image in the program
* Set title: Allows to set a title for images


# Translations

English and german are supported translations. Depending on the system locale either german (locale starts with "de_") or english (anything else) will be setup.

QLocale::system() is used to identify the locale. This routine seems to look for either `LC_MESSAGES` or `LANG` to setup the locale (Linux).

# Additional licenses

This program uses [curl](https://curl.haxx.se) which uses the [MIT License](https://curl.haxx.se/docs/copyright.html).

This product includes software developed by the OpenSSL Project
for use in the [OpenSSL Toolkit](http://www.openssl.org/). For the complete text of the license please see `LICENSE.openssl`.