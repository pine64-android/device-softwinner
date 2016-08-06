#!/bin/bash
############################################################
## 查找代码
############################################################
####查找所有代码
function allgrep() {
	if [ "$1" ]; then
		find . -name .repo -prune -o -name .git -prune -o -name out -prune -o -type f \( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.h' -o -name '*.java' -o -name '*.xml' -o -name '*.sh' -o -name '*.mk' -o -name '*.rc' -o -name '*.cfg' \) -print0 | xargs -0 grep --color -n $@
	else
		show_vit "what do you want to grep ?"
	fi
}

####查找脚本代码
function shgrep() {
	if [ "$1" ]; then
		find . -name .repo -prune -o -name .git -prune -o -name out -prune -o -type f \( -name '*.sh' -o -name '*.mk' -o -name 'Makefile' -o -name 'makefile' \) -print0 | xargs -0 grep --color -n $@
	else
		show_vit "what do you want to grep ?"
	fi
}

####显示代码
function showfile() {
	if [ $# -lt 3 ]; then
		show_vit "need 3 para"
		return 1
	fi

	if [ $2 -lt $3 ]; then
		sed -n "$2,$3p" $1
	else
		sed -n "$3,$2p" $1
	fi
}
#############################################################
## end for this script file
#############################################################
