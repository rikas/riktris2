#!/bin/bash

###########
# KEYGENMUSIC CONVERTER
###########
# This bash script converts every .xm .mod .it files to mp3 using VLC Media player
# Basically it supports every media format that VLC supports, and converts it to mp3
# if you comment out the file type checking line below.
# Usage:
#
#      chmod +x xm_mod_it_TO_mp3.sh
#      xm_mod_it_TO_mp3.sh /path/to/folder/*
#
# Uncomment or edit VLC executable path below according to your operation system.
# Feel free to edit script and convert video files also.
# Original: https://wiki.videolan.org/Transcode/#Transcoding_with_the_Command_Prompt
###########


########### VLC executable path - Linux
#vlc="/usr/bin/vlc"
########### VLC executable path - OSX
vlc="/Applications/VLC.app/Contents/MacOS/VLC"


## audio settings
acodec="mp3"
arate="128" #bitrate
mux="mp3"

## video settings
#vcodec="raw"
#bitrate="1024"

echo "CONVERSION STARTED"

if [ ! -e "$vlc" ]; then
    echo "Command '$vlc' does not exist, specify the right executable path in the script"
    exit 1
fi


for file in "$@"; do

    # convert only mod xm and it files
    if [[ ( $file == *.mod ) || ( $file == *.xm ) || ( $file == *.it ) ]]; then

            # check if converted mp3 already exsists
            if [ -f "$file".mp3 ]
            then
            	echo "Skipping '$file'.mp3 - already exsists."
            else
            	echo "=> Transcoding '$file'... "

                dst=`dirname "$file"`
                new=`basename "$file" | sed 's@\.[a-z][a-z][a-z]$@@'`.$mux

                $vlc -I dummy -q "$file" \
                   --sout "#transcode{acodec=mp3,ab=128}:standard{mux=raw,dst=\"$dst/$new\",access=file}" \
                   vlc://quit
                ls -lh "$file" "$dst/$new"
                echo
            fi

    fi

done

echo "CONVERSION ENDED"
