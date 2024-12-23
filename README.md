# LimePlayer3DS
LimePlayer3DS is a graphical music player for the 3ds with metadata parsing and software midi support.

It checks for music in the `sdmc:/music` folder. If it doesn't exist then it just goes to `sdmc:/`, just like ctrmus. 

The default path for midi patchsets is `/3ds/limeplayer3ds/8mbgmpat/`.

This path be changed in the config file located at `/3ds/limeplayer3ds/config.json` which should be created (if it doesn't already exist) upon launching the application.

More info about LimePlayer3DS's config.json can be found here: <br/>
https://github.com/Oreo639/LimePlayer3DS/wiki/config.json

[For more information on usage of the various features LimePlayer offers, check here.](https://github.com/Oreo639/LimePlayer3DS/wiki)

## Note
LimePlayer3DS is currently in alpha so not all features are going to be available.

## Officaly Supported Formats
Mp3, Wav, Flac, Midi, Xmi, Mus, Hmi, Hmp, Ogg vorbis, and Opus.

## Building
### Prerequsites:
[dkp-pacman](https://devkitpro.org/wiki/devkitPro_pacman)

3ds-dev 3ds-mpg123 3ds-libvorbisidec 3ds-opusfile 3ds-flac 3ds-wildmidi 3ds-jansson 3ds-pkg-config

You may also need [makerom](https://github.com/profi200/Project_CTR) and [bannertool](https://github.com/Steveice10/bannertool/) for cia building (optional).

### Compiling:
Once you complete Prerequsites, run `make` or `make <target>`. Replace `<target>` with one of the targets listed below

### Targets:
+ 3dsx (3dsx only build)
+ cia (generate cia, 3dsx must be built first)
+ clean (clean compiled files)

If you want to build with compiler optimazitions, just add RELEASE=1 to the end (EX: `make 3dsx RELEASE=1`)

## Contributors
Check [CONTRIBUTORS.md](CONTRIBUTORS.md) for more information

## Credits
+ Deltabeard: For his work on ctrmus which was the original foundation for this application as LimePlayer3DS initally started out as a fork of ctrmus.

+ devkitPro: For providing amazing, easy to use, toolchains that without, this application would have never existed.

+ Astronautlevel & LiquidFernir (and the Anemone3DS team): For their work on Anemone3DS which helped quite a bit when it came to starting with citro2d graphics, and they were very helpful in general.

+ Flagbrew: The GUI code is heavilly based on PKSM's GUI code.

+ Others: If I forgot to mention someone important (for example: your code is used without credit), please let me know.

+ Most of the icons under gfx are from the site [icons8.com](https://icons8.com) and are licensed under the [CC-BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/3.0/)

## Special Thanks
+ The Easyrpg Team: For inspiring this application.

+ smealum, fincs, WinterMute, and devkitPro contributors: For making/maintaining documentation and examples for libctru and citro2d/3d which is incredibly helpfull when working on this project.

+ Testers: For testing, providing feedback, and reporting bugs.

## Copyright
The application is licensed under the GPLv3-or-later. Additional terms 7c apply.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
