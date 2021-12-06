# winampclone
I made this long ago (circa 2007) using VC++ 6. Suprisingly, with few tweaks it still works.

Basically this is just an mp3 player. The keypoints are:
1. The UI/controls (buttons, seekbar, volume bar, clock/time, playlist) are just heavily skinned MFCs. Drag and drop folder is supported
2. DirectShow is used to play mp3 files
3. The histogram control is manually created. Audio sample which runs through DirectShow pipeline are intercepted, copied, and fed to FFT engine to generate histogram animation
