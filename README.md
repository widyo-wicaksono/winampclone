I made this long ago (circa 2007) using VC++ 6. Surprisingly, with a few tweaks, it still works.
Basically, this is just an mp3 player. The key points are:
1.	The UI/controls (buttons, seek bar, volume bar, clock/time, playlist) are just heavily skinned MFCs. Drag and drop folder is supported
2.	DirectShow is used to play mp3 files
3.	The histogram control is manually created. Audio sample which runs through the DirectShow pipeline are intercepted using a custom DirectShow filter, copied, and fed to the FFT engine to generate histogram animation
4. Requires lib files (strmbase.lib/strmbasd.lib) from (https://github.com/microsoft/Windows-classic-samples). Specifically in (https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/multimedia/directshow/baseclasses)

![WinAmpun ver 0 0 2 2021-12-07 06-19-11](https://user-images.githubusercontent.com/64301921/144938671-fd08b988-c2df-4931-bd6a-c7ef18fd4a7a.gif)
