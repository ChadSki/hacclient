# Halo Anticheat 2

Halo Anticheat 2 is a new mod, supporting all versions of both Halo CE and Halo PC.

Despite the name, there's no anticheat in this build. This release is an alpha/preview to gather feedback on some of the features and a chance to get them tested on a wider array of machine configurations. Expect there to be bugs and gripes; removing them is the point of this alpha/preview release.

Here's a quick overview of the features currently on offer for testing:

### Map downloading

To help promote custom maps, HAC 2 boasts an in-game map downloading system. If a player attempts to join a server running a map they don't have, HAC 2 will intervene and download the map for you from a network of map mirrors and then continue connecting you to the game. Currently the system grants access to around 1,000 maps and growing.

This system requires absolutely no effort on the part of game server administrators. Players and administrators will be able to add any maps that might be missing from our extensive collection to the network responsible for serving the maps. There are also plans to support map makers further by allowing them to hold beta testing sessions without the hassle of distributing maps beforehand and having to worry about them being leaked by testers.

To keep things speedy, we're seeking bandwidth providers/mirrors for map hosting. You're free to donate as much or as little space and bandwidth as you wish. The only requirement is that you have at least ~10mbit upstream non-residential (other words, no home hosting) bandwidth. If you're interested in helping the service out, contact me or reply to this post and I'll contact you.

### Rapid load times

HAC 2 adds a caching system to Halo CE that allows for near instant game launch times, regardless of how many custom maps you have. Put away the map managers, they're no longer needed. Download as many maps as you wish, your load times will stay constant.

Note: The initial launch time after loading HAC 2 will be the same as usual, so don't be disheartened. You'll see the speed increase on subsequent attempts after the caching has kicked in.

### Increased draw distance

A small addition but the default draw distance has been increased severalfold. This only benefits a few custom maps. If you wish, you can restore the default (1024 units) via the 'draw' console command.

### Doubled BSP poly limit
 
This was mainly done to fix glitches on existing maps but the extra budget could be used to produce more detailed maps. Although the limit has been doubled, it could be increased further in the future if desired. I'm aware that OS does something similiar (wasn't at the time of implementation), although I'm not sure if it's only for .yelo maps. Assuming it's not, having this as a feature in more than one mod will allow map makers to have their higher detailed maps played by a wider audience.


### In-game BSP poly budget display

I've included a basic budget display for map makers that'll allow them to view how many polys are being drawn and how much of their budget they've used. This might prove to be utterly useless but the idea was to help map makers to squeeze the most of engine. That and to deserve as a distraction for a few moments.

### Automated version changing

Having a community split between game versions necessitated a version changer to grant players a wider choice of games to choose from. HAC 2 takes this idea one step further by detecting the version of the server you're attempting to join and silently switching your game to the correct version, ensuring you can join any game, regardless of version differences.

### Unified server list

Another problem, related to the version changing, was only being able to see the server list for a single version of the game. HAC 2 solves this by showing you servers from all versions of the game. Combined with the automated version changer, you needn't worry about which version of the game you're using again.

### Field of view modifier
 
Fairly standard feature but an important one to have. Pressing F6 in-game will allow you to adjust the field of view to your liking. If you're a console command kind of person, you can just type the value directly into the console with 'fov degrees'. Your preferences will be saved so you only need to do it once.

### In-game server bookmarks/favourites

Pressing F4 in a game will allow you to add (or remove with another press) a server to a list of bookmarks. Pressing F5 will allow you to view this list, including live status information to help you pick a game to play in. To quick join a bookmarked server, simply press alt and the corresponding number. To join bookmarks with a number higher than 10, hold the control key down (e.g., alt + ctrl + 5 is 15).

### Quick server information copying

A minor feature but it solves a pet annoyance held by many scrimmers. You can copy the details of a bookmarked server to the clipboard for easy pasting on Xfire/your choice of IM service by typing 'copy' followed by the corresponding bookmark number. To copy the details of the server you're in, regardless of whether it's bookmarked, simply type 'copy'.

### Devmode toggle

Since external apps are irritating, I've added a command to toggle devmode on/off. Simply type devmode 0/1 into the console.

Note: Disable it before switching between games unless it's local (non-dedicated) or the game will crash. I'll patch this in the next update.

### Stuff nobody cares about (misc fixes)

There are a couple of small fixes, such as fixing the console text so that the display time isn't tied to your framerate.

## Installation - A.K.A. "What files do I have to replace this time?"

HAC 2 is a single file that gets dropped into your Halo directory. It does not replace (or proxy) any of your existing files. Uninstalling is a simple matter of pressing the delete key. For more information, see the readme.txt included in the download.

HAC 2 will automatically grab the newest release when you start the game to ensure that you receive all bug fixes with zero effort on your part. Work has been put in to ensure the smallest possible file size (~75KB) to give you a smooth updating experience. Details of fixes and new features will be posted with each update.

## F.A.Q.

### Do I need admin rights to use HAC 2?

No. It was important to me that users didn't need to run the game with admin rights in order to make use of certain features, including map downloading.

### Where do maps get downloaded to?

Maps are downloaded to a subdirectory of where Halo stores your game profile. This was done to allow users to download maps without requiring them to start the game with admin rights as would have been required by downloading to the game's default maps directory. If you wish to copy the maps to the standard directory, simply running the game with admin rights will cause HAC 2 to copy all downloaded maps there.

### What ports does HAC 2 use?

There should be no need to adjust your firewall settings or mess around with port forwarding (only outgoing is required) but port 80 is used for updating and map downloading and port 3750 is used for communicating with the master server (stats tracking, anticheat, aspects of map downloading, etc). 3750 is unused in this release but will likely be made use of in the coming weeks.

If you're able to play the game online, you should be fine.

## Thanks to...

Things wouldn't have progressed so smoothly without the help of the people below. Some of them had no direct involvement but have contributed through posting their research over the years.

In no particular order:
Sehe, As7raios (my personal Q&A department), PM Rave, Harbinger, Sean Aero, Oxide, Goemitar, Souless (UI prototype for the abandoned HAC 1.5), Snafubar (work on the abandoned C# HAC 2), Martz0r (work on HAC 1), SilentK (engine research, pre-OS), FleetAdmiralBacon (your app gave me the idea to increase the draw distance), WaeV and Tucker.
