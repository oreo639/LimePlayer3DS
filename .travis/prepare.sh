#!/bin/bash

mkdir -p ~/.local/ && mkdir -p ~/.local/bin
source /etc/profile.d/devkit-env.sh
export PATH=$PATH:~/.local/bin
if ! [ -x "$(command -v bannertool)" ]; then
  echo '=> Bannertool is not installed. ' && echo '=> Installing!'
  git clone --recursive https://github.com/Steveice10/bannertool && cd bannertool/
  make && cp output/linux-x86_64/bannertool ~/.local/bin/bannertool
  cd ../ && rm -rf bannertool/
else
  echo '=> Bannertool is already installed.'
fi
if ! [ -x "$(command -v makerom)" ]; then
  echo '=> Makerom is not installed. ' && echo '=> Installing!'
  git clone https://github.com/profi200/Project_CTR.git && cd Project_CTR/makerom
  make && cp makerom ~/.local/bin/makerom
  cd ../.. && rm -rf Project_CTR
else
  echo '=> Makerom is already installed.'
fi
echo 'Done!'
