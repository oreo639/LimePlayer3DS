#!/bin/bash -ex

source /etc/profile.d/devkit-env.sh

make -C LimePlayer3DS clean
make -C LimePlayer3DS 3dsx
