#!/bin/bash
sudo dkp-pacman -Syu 3ds-dev --noconfirm --needed
sudo dkp-pacman -Syu 3ds-mpg123 3ds-libvorbisidec 3ds-opusfile 3ds-jansson 3ds-wildmidi --noconfirm
mkdir -p ~/.local/ && mkdir -p ~/.local/bin
source /etc/profile.d/devkit-env.sh
export PATH=$PATH:~/.local/bin
if ! [ -x "$(command -v bannertool)" ]; then
  echo 'Error: bannertool is not installed. ' && echo 'Installing!'
  git clone --recursive https://github.com/Steveice10/bannertool && cd bannertool/
  make && cp output/linux-x86_64/bannertool ~/.local/bin/bannertool
  cd ../ && rm -rf bannertool/
fi
if ! [ -x "$(command -v makerom)" ]; then
  echo 'Error: makerom is not installed. ' && echo 'Installing!'
  git clone https://github.com/profi200/Project_CTR.git && cd Project_CTR/makerom
  make && cp makerom ~/.local/bin/makerom
  cd ../.. && rm -rf Project_CTR
fi
echo 'Done!'
