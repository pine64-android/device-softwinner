#!/bin/bash
############################################################
## show tips function
############################################################
# show very important tip
function show_vit() {
#	echo "num=$#"
#	echo "$@"
	if [ "$1" ]; then
		for mytipid in "$@" ; do
			echo -e "\e[1;31m$mytipid\e[0m"
		done
	fi
}

# show very important tip without line end
function show_vit_nle() {
	if [ "$1" ]; then
		for mytipid in "$@" ; do
			echo -e -n "\e[1;31m$mytipid\e[0m"
		done
	fi
}

# show warning tip
function show_wtip() {
#	echo "num=$#"
#	echo "$@"
	if [ "$1" ]; then
		for mytipid in "$@" ; do
			echo -e "\e[1;33m$mytipid\e[0m"
		done
	fi
}

# show green/good/great tip
function show_gtip() {
#	echo "num=$#"
#	echo "$@"
	if [ "$1" ]; then
		for mytipid in "$@" ; do
			echo -e "\e[1;32m$mytipid\e[0m"
		done
	fi
}

# show purple
function show_vip() {
#	echo "num=$#"
#	echo "$@"
	if [ "$1" ]; then
		for mytipid in "$@" ; do
			echo -e "\e[1;35m$mytipid\e[0m"
		done
	fi
}
#############################################################
## end for this script file
#############################################################
