[![Build Status](https://travis-ci.com/Oreo639/LimePlayer3DS.svg?branch=master)](https://travis-ci.com/Oreo639/LimePlayer3DS)
# Limeplayer 3DS
Limeplayer 3ds is a graphical music player for the 3ds with metadata parsing and software midi support.

It checks for music in the `sdmc:/music` folder. If it doesn't exist then it just goes to `sdmc:/`, just like ctrmus. 

The default path for midi patchsets is `/3ds/limeplayer3ds/dgguspat`.

This path be changed in the config file located at `/3ds/limeplayer3ds/config.json` which should be created (if it doesn't already exist) upon launching the application. (Reading of the config file will be reimplemented soon)

(Improved GUI, TBD. Loading cover pictures. All of the menus (although every one except setting's and copyright's use is TBD).)

## Note
Limeplayer3ds is currently undergoing a c++ rewrite (instead of just being in c). Alot of its functionality is not going to be avliable right now.

## Officaly Supported Formats
Mp3, Wav, Flac, Midi, Xmi, Mus, Hmi, Hmp, Ogg, and Opus.

## Building
### Prerequsites:
[dkp-pacman](https://devkitpro.org/wiki/devkitPro_pacman), 3ds-dev, 3ds-mpg123, 3ds-libvorbisidec, 3ds-opusfile, 3ds-jansson, and 3ds-wildmidi.

You will also need [makerom](https://github.com/profi200/Project_CTR) and [bannertool](https://github.com/Steveice10/bannertool/) for cia building (optional).

### Compiling:
Once you complete Prerequsites, run `make` or `make <target>`. Replace `<target>` with one of the targets listed below

### Targets:
+ 3dsx (3dsx only build).
+ cia (generate cia, 3dsx must be built first)
+ clean (clean compiled files)

If you want to build with compiler optimazitions, just add RELEASE=1 to the end (EX: `make 3dsx RELEASE=1`)

## Developers
These are the contrubuters and maintainers, both big and small (will be updated every once and a while)
+ Maintainers: Oreo639, TBD
+ Contributers: liquidfenrir (fixed the makefile)

## Credits
+ Limeplayer started as a fork of ctrmus, meaning that the majority of its code came from, is based off, or was built on top of ctrmus.

+ Deltabeard: For his work on ctrmus which was the original foundation for this application.

+ Astronautlevel & Liquidfernir (and the Anemone3DS team): For their work on Anemone3DS which helped quite a bit when it came to starting with citro2d graphics, and they were very helpful in general.

+ Others: If I forgot to mention someone important (as in their code is used without credit), please let me know.

+ Most of the icons under gfx are from the site [icons8.com](https://icons8.com) and are licensed under the [CC-BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/3.0/)

## Copyright

The application is licensed under GPLv3-or-later. Additional terms 7c apply.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

## Special Thanks
+ Smealum, WinterMute, fincs, and the devkitPro team: For making/maintaining documentation and libraries for libctru and citro2d/3d which were very helpfull when making this project.

+ The Easyrpg Team: For inspiring this application with their work porting wildmidi to the 3ds and the switch (the reason I started working on this was because I wondered if there was a midi player for the 3ds).

+ Family: For providing me helpfull feedback, and testing pre-release versions.
