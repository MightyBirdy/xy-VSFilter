# XySubFilter - Overlay

This branch of XySubFilter renders the subtitles in an overlay window on the primary monitor. 

Ideal for multitaskers watching foreign shows & films.

Example usage: full screen-windowed game on primary monitor and full screen video on secondary monitor.
subtitles will appear overlayed on the game, 
allowing the user to watch a foreign-language video and play a game at the same time without having to look away from the game. 

Fully functional and stable. 
Tested with MPC-HC and madVR from [KCP 0.6.0.7](http://haruhichan.com/forum/showthread.php?7545-KCP-Kawaii-Codec-Pack)

Could use the following improvements:

1. Since it grabs the rendered subtitle as a bitmap before it returns it to the requester, it uses that resolution. 
Hence subtitles will have scaling artifacts if the video window is significantly smaller than the primary monitor.
(If the video window full screen on a monitor the same size as the primary monitor, then no scaling is necessary)
2. Completely ignorant of subtitle timing. Simply displays the most recent subtitle frames rendered. 
Hence the subtitles in the overlay may be out of sync by some number of ms.
3. General architectural improvements. 
ie. Find a more elegant way to hook into the render process.
4. Configuration. Such as ability to turn on/off or chose a different display monitor. 
Currently can only be turned on/off by switching dlls and restarting player
5. Improve alpha-blending of rendered subtitles onto transparent overlay window
