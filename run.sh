#!/bin/bash

# Compile code
make
# Run code
./main.x
# Convert the image
convert img.ppm img.png
