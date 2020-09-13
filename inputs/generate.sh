#!/bin/bash

# Generate graphics at different resolutions for different devices
# 1.0 (jolla phone), 1.25 (jolla c), 1.5 (tablet), 1.75 (xperia)
ratios="1.0 1.25 1.5 1.75 2.0"

# Generate app icons
sizes="86 108 128 256"
for size in ${sizes}; do
	mkdir -p "../icons/${size}x${size}"
	inkscape -z -e "../icons/${size}x${size}/harbour-contrac.png" -w $size -h $size "./svg/harbour-contrac.svg"
done

# Create the ratio directories
for ratio in ${ratios}; do
	mkdir -p "../qml/images/z${ratio}"
done

# Function for generating PNG images
function generate {
	basex=$1
	basey=$2
	names=$3
	for ratio in ${ratios}; do
		sizex=`echo "${ratio} * ${basex}" | bc`
		sizey=`echo "${ratio} * ${basey}" | bc`
		for name in ${names}; do
			inkscape -z -e "../qml/images/z${ratio}/${name}.png" -w ${sizex} -h ${sizey} "./svg/${name}.svg"
		done
	done
}

# Generate cover action icons
generate 32 32 "cover-action-active cover-action-inactive"

# Generate small icons
generate 32 32 "icon-s-active icon-s-inactive icon-s-warning icon-s-unknown"

# Generate medium icons
#generate 64 64 "icon-m-eject icon-m-replay icon-m-skip"

# Generate large icons
#generate 96 96 "icon-l-replay icon-l-skip"

# Generate titles
generate 304 86 "contrac-title"

# Generate cover overlays
generate 96 96 "cover-active cover-inactive cover-warning cover-unknown"

# Generate background
generate 117 187 "cover-background"

