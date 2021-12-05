#!/usr/bin/bash

meson build -Dbuildtype=plain
ninja -C build
sudo ninja -C build install


