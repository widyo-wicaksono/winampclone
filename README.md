I made this long ago (circa 2007) using VC++ 6. Surprisingly, with a few tweaks, it still works.
Basically, this is just an mp3 player. The key points are:
1.	The UI/controls (buttons, seek bar, volume bar, clock/time, playlist) are just heavily skinned MFCs. Drag and drop folder is supported
2.	DirectShow is used to play mp3 files
3.	The histogram control is manually created. Audio sample which runs through the DirectShow pipeline are intercepted using a custom DirectShow filter, copied, and fed to the FFT engine to generate histogram animation
4. Requires lib files from (https://github.com/microsoft/Windows-classic-samples)
