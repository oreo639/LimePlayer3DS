#!/bin/bash
sudo dkp-pacman -Syu 3ds-dev --noconfirm --needed
sudo dkp-pacman -Syu 3ds-mpg123 3ds-libvorbisidec 3ds-opusfile 3ds-jansson 3ds-wildmidi --noconfirm
source /etc/profile.d/devkit-env.sh
git clone --recursive https://github.com/Steveice10/bannertool && cd bannertool/
make && sudo cp output/linux-x86_64/bannertool /opt/devkitpro/devkitARM/bin/bannertool
cd ../ && rm -rf bannertool/
git clone https://github.com/profi200/Project_CTR.git && cd Project_CTR/makerom
make && sudo cp makerom /opt/devkitpro/devkitARM/bin/makerom
cd ../.. && rm -rf Project_CTR
