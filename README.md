ContextIcons

Allows setting icons for standard Windows context menu items like "Copy" and "Paste." and much more. Designed for Windows(x64) 8 and up.

This is a modified fork from Fleex255/ContextIcons.


HowTo use:

Copy the ReleaseFolder to C:\Windows\ContextMenuIcon

start RegisterX64AsAdmin.cmd as admin

close explorer and restart

rightclick on any File, you will see a lot of questionmarks

In C:\Users\accountName\AppData\Local\ContextIcon ar two files
ContextIcons.ini and Debug.ini

ContextIcons.ini:

Context Comand= ./ContextIcon/icon.png

The Context Comand will be found using regex, so group longe phrases first, then single words.

After founding a phrase it exits the searchroutine.
If single words are grouped first it will found this word in longer phrases and exit, showing the incorrect icon.

If programs have their own iconhandler running and this one will run afterwards it will override the icons