# Spotlight

This program downloads wallpapers from Spotlight (Windows 10), Chromecast and
Bing according to the user's configuration. It will store the images in a
directory named "download" in the default picture path. For Windows this would be something like `c:\Users\%Username%\Pictures` while under Linux this could be `~/Pictures`.

You can select which orientation (landscape / portrait) the images should have
in order to be considered for download.

Additionally you can select whether to show titles (if available) or not.


## Technical information

This project was created with Qt-Creator (4.4.1) and Qt 5.9.2 on both Windows 10
and Linux (Devuan ascii/ceres). On Windows the Visual C++ compiler was used to
create a 64 bit based binary. However, it should work fine with Mingw too.

If you are using Visual C++ as your compiler make sure that the LIBS paths in
*Spotlight.pro* are correct for your system.

### Getting it to run

This chapter explains how to create a directory with all files 
need for Spotlight in order to work. You only need to do this,
if you have don't have those libraries installed on your target 
system otherwise Spotlight should already work.

So just start Spotlight via clicking on the icon or command line - 
No errors reported? Fine you should be done otherwise follow these
additional steps:

**1)** Create a program directory

Lets call it `SpotlightDir` but you can name it as you like.

**2)** Copy the program into this directory

**3)** add required Qt libraries to `SpotlightDir`

Spotlight need some libraries in order to work. I


If your target system is missing those or has different versions 
get them from your Qt installation dir you used for compilation, 
e.g.:

Linux `/opt/Qt/5.9.4/gcc_64/lib/`  (the libs will end with `.so`)

`Qt5Core.so
Qt5Gui.so
Qt5Network.so
Qt5Sql.so
Qt5Widgets.so`

Windows: `C:\Qt\5.9.4\msvc2017_64\bin` 

For Windows you'll need the same libs, but those will end with `.dll`.

For https to work Qt needs `libeay32.dll` and ssleay32.dll`. 
Linux distribution should provide those by installing openssl. Windows
doesn't provide those you need to download them. 

Just download openssl 1.0.x (where x >= 1) and extract those files from
the archive. You find a download list on the 
[OpenSSL Wiki](https://wiki.openssl.org/index.php/Binaries)

Finally you will need some sub directories for the required plugins 
(*Note:* copy the directories not the directory's content!):

* `platforms`
* `sqldrivers`
* imageformats`

You can again find them in Qt installation dir, e.g.

`C:\Qt\5.9.4\msvc2017_64\plugins\`

If your copied sub directories contain `.pdb` files you can safely 
remove them from `SpotlightDir\plugins\sqldrivers`

**4)** Optionally remove libraries

The program should now work but the sub directories will contain 
superfluous files. E.g. we currently don't need a driver for MySQL
or ODBC so we could safely remove them.

Same for image formats: gif, tiff or tga.

**5)** For Linux

For Linux just copy spotlight.sh from your project directory to
`SpotlightDir` and `chmod 755 SpotlightDir/spotlight.sh`. 

The script just set `LD_PRELOAD` and starts the program.

## Icons

Icons are from the tango icon set or derived from it. Additional parts from [wikimedia commons](https://commons.wikimedia.org/wiki/File:Pictograms-nps-recycling.svg)
 
Both tango icons and wikimedia icon are in the public domain.

# Todo

Quite a bit currently works, but some things are still missing:

* Enhance build system to create a directory which contains all files needed to run Spotlight
* Use Reverse Image search to suggest titles for unnamed images.
* try to reload an accidentally deleted image (if possible)
* Check for exif data (it looks like neither Bing nor Chromecast nor Spotlight offers those)

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

Finally we have the button *search* and *Image*. Search will start a search for new images 
(which may not be successful). As the program shows a downloaded image in the background
every 10 seconds you may be interested in seeing the complete picture. In that case press
*Image* to hide most of the GUI except for the *back* button which will show the GUI
again.

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