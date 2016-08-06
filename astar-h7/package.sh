#!/bin/bash

cd $PACKAGE

chip=sun8iw5p1
platform=android
board=h7
debug=uart0
sigmode=none

usage()
{
	printf "Usage: pack [-cCHIP] [-pPLATFORM] [-bBOARD] [-d] [-s] [-h]
	-c CHIP (default: $chip)
	-p PLATFORM (default: $platform)
	-b BOARD (default: $board)
	-d pack firmware with debug info output to card0
	-s pack firmware with signature
	-h print this help message
"
}

while getopts "c:p:b:dsh" arg
do
	case $arg in
		c)
			chip=$OPTARG
			;;
		p)
			platform=$OPTARG
			;;
		b)
			board=$OPTARG
			;;
		d)
			debug=card0
			;;
		s)
			sigmode=sig
			;;
		h)
			usage
			exit 0
			;;
		?)
			exit 1
			;;
	esac
done

./pack -c $chip -p $platform -b $board -d $debug -s $sigmode
