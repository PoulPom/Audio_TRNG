#!/bin/bash

g++ audio_rng.cpp -o audio_rng -Wall -std=c++26 -lpthread -lm -msse2 && \
./audio_rng
