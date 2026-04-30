#!/bin/bash

mkdir -p build
cd build
cmake ..
make
./audio_rng
