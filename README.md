# Limeplayer 3DS
Limeplayer 3ds is a graphical music player for the 3ds that supports midi, mp3, flac, wav and vorbis (in no particular order) and has metadata parsing.

It checks for music in the `music` folder. If it doesn't exist then it just goes to `sdmc:/`, just like ctrmus. 

The default path for midi patchsets is `/3ds/limeplayer3ds/dgguspat`. The config file can be found in `/3ds/limeplayer3ds/confg.json` This should be automaticly be created (it specifies which patchset to use by default). You can change it with the X button. (or the second option of the "settings menu")

(Improved GUI, TBD. Loading cover pictures. All of the menus (although every one except setting's and copyright's use is TBD).)

Keep in mind that this repo will be updated with the source and liscenses when they are ready, which may be in a few days or longer.

# Officaly Supported Formats
Mp3, Wav, Flac, Midi, Xmi, Mus, Hmi, Hmp, Ogg, and Opus.

# Issues
Before posting an issue, I have a quick Q and A for you.

Q: I tried playing a midi file and I get an `ERR: Failed to initalize decoder.` message?
A: Make sure you have a wildmidi patchset on your sd card and that config.json is pointing to the correct (wildmidi/timidity).cfg. Keep in mind that wildmidi does not currently support sf2 files (only gus patchsets).

Q: I tried playing a file and I get an `ERR: Unrecognized filetype.` message?
A: Is your file on the officaly supported formats list? If it is, make sure the extension is correct and see what type of audio format other audio players report. It may not actualy be a supported format.

# Building
Prerequsites:
dkp-pacman, 3ds-dev, 3ds-mpg123, 3ds-libvorbisidec, 3ds-opusfile, 3ds-jansson, and 3ds-wildmidi (if wildmidi isn't avliable just run buildlibs.sh). You will also need [makerom](https://github.com/profi200/Project_CTR) and [bannertool](https://github.com/Steveice10/bannertool/) for cia building (optional).
Compiling:
If wildmidi is not avliable through dkp-portlibs, just run `./buildlibs.sh`. Make sure you have devkitpro-pkgbuild-helpers, cmake, and curl installed for it to work.
From there you should be able to just run make.
Targets: 
3dsx (3dsx only build).
cia (generate cia, 3dsx must be built)
clean (clean compiled files)
If you want to build with compiler optimazitions, just add RELEASE=1 to the end (EX: `make 3dsx RELEASE=1`)
Note: Wildmidi for limeplayer uses a few patches to fix certain issues and allow installing for a non *nix target.

# Developers
These are the contrubuters and maintainers, both big and small (will be updated every once and a while)
Maintainers: Oreo639, TBD
Contributers: N/A
Check the credits for those who made a big impact on development.

# Credits
Limeplayer started as a fork of ctrmus, meaning that the majority of its code came from, is based off, or was built on top of ctrmus.

Deltabeard: For his work on ctrmus which was the foundation for this application.

Astronautlevel & Liquidfernir: For their work on anemone which helped quite a bit when it came to starting with citro2d graphics, and they were very helpful in general.

Liquidferinir: Their config parser from multiupdater was revamped and built on top of.

Smealum, WinterMute, Fincs, and the Devkitpro team: For making/maintaining documentation and libraries for libctru and citro2d/3d which were very helpfull when making this project.

The Easyrpg Team: For inspiring this application with their work porting wildmidi to the 3ds and the switch (the reason I started working on this was because I wondered if there was a midi player for the 3ds).

The Anemone Team: 


~~Linus Walleij: A modified version of his decoder (to be independant from glib's c libraries) for libid3-tag which was origionaly use in GNOMAD2 is used in this application to allow for reading metatadata from mp3 (id3read).~~ (removed during switch from id3tag to mpg123)

Most of the icons under gfx are from the site [icons8.com](https://icons8.com) and are licensed under the [CC-BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/3.0/)

Others: If I forgot to mention someone important (as in their code is used without credit), please let me know.

# Special Thanks
Nintendo Homebrew: A nice discord that is always giving help with homebrew and development.

Family: For providing somewhat helpfull feedback, and testing pre-release versions.

Testers: Asty and liquidfrenir. Their testing was minimal but it was better than nothing.
