# Spotlight

This program downloads wallpapers from Spotlight (Windows 10), Chromecast and
Bing according to the user's configuration. It will store the images in a
directory named "download".

You can select which orientation (landscape / portrait) the images should have
in order to be considered for download.

Additionally you can select whether to show titles (if available) or not.


## Technical information

This project was created with Qt-Creator (4.4.1) and Qt 5.9.2 on both Windows 10
and Linux (Devuan ascii/ceres). On Windows the Visual C++ compiler was used to
create a 64 bit based binary. However, it should work fine with Mingw too.

If you are using Visual C++ as your compiler make sure that the LIBS paths in
*Spotlight.pro* are correct for your system.
 
## Icons

Icons are from the tango icon set or derived from it. Additional parts from [wikimedia commons](https://commons.wikimedia.org/wiki/File:Pictograms-nps-recycling.svg)
 
Both tango icons and wikimedia icon are in the public domain.

# Todo

Quite a bit currently works, but some things are still missing:

* Change the title to your liking
* try to reload an accidentally deleted image (if possible)
* Data should not be stored in the program directory but in a directory the user chooses.
  If nothing gets chosen, default to something sane.
* Translation is missing.
* Check for exif data

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
*download* which will be automatically created on program start. The program allows you 
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
  Please note that only Windows 10 and KDE was tested / implemented as this is what
  I use. Feel free to offer patches for your desktop environment.
* Delete an image. The images will be deleted from the file system but remain in the 
  database. Using this stored data prevents the program from downloading the picture
  again.
* reload: This will undo a deletion and download the image again resetting its status.
  In case of Bing this may not be possible.  (This feature is not yet implemented)
* Tag an image. You can use more than one tag on an image.
* Show: shows the selected image in the program
* Set title: Allows to set a title for images (Not yet implemented)
