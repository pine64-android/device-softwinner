#!/bin/bash
############################################################
## for chiphd begin
############################################################
##创建git记录文件
function chiphd-tagOutPrint()
{
	if [ -d "$OUT"/system ]; then
		tagOutTxtFile="$OUT"/system/release_tag.txt
		TIME_TXT=$(date '+%Y-%02m-%02d %02k:%02M')
		echo "now get android-repo-tag..."
		echo -e "android build at $TIME_TXT\n" > $tagOutTxtFile
		repo forall -c 'echo "--------------------------------------------------------------------------------" && \
			pwd && git log -1 && echo ****status**** && git status' >> $tagOutTxtFile
		echo "done : $tagOutTxtFile"
	fi
}


########git am patch
function patch_chiphd_am() {
local thisPatch=$1
if [ "$thisPatch" -a -f "$thisPatch" ]; then
#先检查是否可以打,然后再am
	echo "git apply --check : $thisPatch" && git apply --check $thisPatch && echo "----------am $thisPatch ----------" && git am --ignore-whitespace $thisPatch
	if [ "$?" != "0" ]; then
		#失败,退出处理
		show_vit "---- apply --check fail" && return 1
	fi
	return 0
fi

#扫描当前目录所有patch文件
Loop_i=0
for pFile in *.patch
do
	if [ -f "$pFile" ]; then
		Loop_i=`expr $Loop_i + 1`
		#先检查是否可以打,然后再am
		echo -e "\e[1;33m${Loop_i})\e[0m apply --check : $pFile" && git apply --check $pFile && echo "----------am $pFile ----------" && git am --ignore-whitespace $pFile
		if [ "$?" == "0" ]; then
			#成功,可以删除patch文件了
			show_gtip "----------am $pFile ok, del it." && rm "$pFile"
		else
			#失败,退出处理
			show_vit "---- apply --check fail" && return 1
		fi
	else
		show_vit "no file : $pFile"
	fi
done
}

########apply patch
function patch_chiphd_ap() {
local thisPatch=$1
if [ "$thisPatch" -a -f "$thisPatch" ]; then
	echo "git apply --check : $thisPatch" && git apply --check $thisPatch && echo "----------apply $thisPatch ----------" && git apply --ignore-whitespace $thisPatch
	if [ "$?" != "0" ]; then
		#失败,退出处理
		show_vit "---- apply --check fail" && return 1
	fi
	return 0
fi

#扫描当前目录所有patch文件
Loop_i=0
for pFile in *.patch
do
	if [ -f "$pFile" ]; then
		Loop_i=`expr $Loop_i + 1`
		#先检查是否可以打,然后再am
		echo -e "\e[1;33m${Loop_i})\e[0m apply --check : $pFile" && git apply --check $pFile && echo "----------apply $pFile ----------" && git apply --ignore-whitespace $pFile
		if [ "$?" == "0" ]; then
			#成功,可以删除patch文件了
			show_gtip "----------am $pFile ok, del it." && rm "$pFile"
		else
			#失败,退出处理
			show_vit "---- apply --check fail" && return 1
		fi
	else
		show_vit "no file : $pFile"
	fi
done
}

##输出git记录
function chiphd-tagOut()
{
	if [ -n "$OUT" ]; then
		if [ -d "$OUT" ]; then
			chiphd-tagOutPrint
		fi
	else
		show_vit "NOT lunch"
		return 1
	fi
}

function tagPack()
{
	chiphd-tagOut && pack $@
}

############### git function
## device-git
function ddgit()
{
	if [ "$DEVICE" ]; then
		if [ -d $DEVICE ]; then
			cd $DEVICE
			git $@
			cd - > /dev/null
		else
			show_vit "NOT exist $DEVICE"
		fi
	else
		show_vit "NOT lunch"
	fi
}

## 获取branch集合
function git_get_branch_set()
{
	git branch | sed 's/\*/ /' | sed 's/^  //' | tr '\n' ' ' | sed 's/ $/\n/'
}

## 打包已提交的文件
function tar_repo_committed()
{
	if [ $# -lt 1 ]; then
		echo -e -n "call help-chiphd for more info, e.g. : " && show_vit "tar_repo_committed --committer=blue"
		return 0
	fi

	MyTop=$(gettop)
	if [ -z "$MyTop" ]; then
		show_vit "not android path"
		return 0
	fi
	this_record_pwd

	TIME_Name=$(date '+%Y-%02m-%02d_%02k%02M')

	cd $MyTop
	MyTop=$(pwd)
	MyTopName=${PWD##*/}

	MfileName="tar_${TIME_Name}_pre.txt"
	DfileName="tar_${TIME_Name}_deleted.txt"
	EfileName="tar_${TIME_Name}_exist.txt"
	TfileName="${MyTopName}_committed.${TIME_Name}.tar.gz"

	## 获取曾经修改过的文件
	echo "now get commit-files by $@ ..."
	myrepo_dir_set=$(repo forall -c 'echo $REPO_PATH' | tr "\n" " ")
	for ii in $myrepo_dir_set
	do
		cd $ii
		if [ $? = 0 ]; then
			git log  --pretty=format:"" --name-only "$@" | sed '/^$/d' | sed "s%^%$MyTopName/$ii/%g" >> $MyTop/${MfileName}

			cd - > /dev/null
		fi
	done

	if [ -f $MyTop/${MfileName} ]; then
		## 判断文件是否还存在
		echo "now check files..."
		my_modify_set=$(cat $MyTop/${MfileName} | tr "\n" " ")
		cd ..
		for ii in $my_modify_set
		do
			if [ -f "$ii" ]; then
				echo $ii >> $MyTop/${EfileName}
			else
				echo $ii >> $MyTop/${DfileName}
			fi
		done

		## 打包文件
		if [ -f $MyTop/${DfileName} ]; then
			echo $MyTopName/${DfileName} >> $MyTop/${EfileName}
		fi

		if [ -f $MyTop/${EfileName} ]; then
			echo "now tar ..."
			tar -zcvf ${TfileName} --files-from=$MyTop/${EfileName} && \
				echo -e -n "tar file : " && show_vit "$MyTop/../${TfileName}"
		else
			show_vit_nle "find nothing by: git log  --pretty=format:\"\" --name-only $@"
			echo ""
		fi

		## 删除临时文件
		del_exist_file $MyTop/${DfileName}
		del_exist_file $MyTop/${EfileName}
		del_exist_file $MyTop/${MfileName}
	else
		show_vit_nle "find nothing by: git log  --pretty=format:\"\" --name-only $@"
		echo ""
	fi

	this_resume_pwd
}

## 打包正在修改的文件(未提交)
function tar_repo_now_modify()
{
	MyTop=$(gettop)
	if [ -z "$MyTop" ]; then
		show_vit "not android path"
		return 0
	fi
	this_record_pwd
	
	TIME_Name=$(date '+%Y-%02m-%02d_%02k%02M')

	cd $MyTop
	MyTop=$(pwd)
	MyTopName=${PWD##*/}

	MfileName="tar_${TIME_Name}_pre.txt"
	DfileName="tar_${TIME_Name}_deleted.txt"
	EfileName="tar_${TIME_Name}_exist.txt"
	TfileName="${MyTopName}_newModify.${TIME_Name}.tar.gz"

	echo "now get modify files ..."
	repo forall -c 'git ls-files -m | sed "s%^%$REPO_PATH/%g" && git ls-files -o --exclude-standard | sed "s%^%$REPO_PATH/%g" ' >> $MfileName
	sed -i "s%^%${MyTopName}/%g" $MfileName

	if [ -f $MyTop/${MfileName} ]; then
		## 判断文件是否还存在
		echo "now check files..."
		my_modify_set=$(cat $MyTop/${MfileName} | tr "\n" " ")
		cd ..
		for ii in $my_modify_set
		do
			if [ -f "$ii" ]; then
				echo $ii >> $MyTop/${EfileName}
			else
				echo $ii >> $MyTop/${DfileName}
			fi
		done

		## 打包文件
		if [ -f $MyTop/${DfileName} ]; then
			echo $MyTopName/${DfileName} >> $MyTop/${EfileName}
		fi

		if [ -f $MyTop/${EfileName} ]; then
			echo "now tar ..."
			tar -zcvf ${TfileName} --files-from=$MyTop/${EfileName} && \
				echo -e -n "tar file : " && show_vit "$MyTop/../${TfileName}"
		else
			show_vit_nle "find nothing by: git log  --pretty=format:\"\" --name-only $@"
			echo ""
		fi

		## 删除临时文件
		del_exist_file $MyTop/${DfileName}
		del_exist_file $MyTop/${EfileName}
		del_exist_file $MyTop/${MfileName}
	else
		show_vit_nle "find nothing modified"
		echo ""
	fi

	this_resume_pwd
}

## 检查是否已经升级好,用法 repo_check_update 参考分支 检查分支
function repo_check_update() ########## todo........
{
	this_record_pwd

	cd $(gettop)
	if [ $# -lt 2 ]; then
		echo -e -n "need branch name, e.g. : " && show_vit "repo_check_update base_branch_name be_checked_branch_name"
		cd $OLD_PWD
		return 0
	fi

	is_found_branch=0
	check_branch=$1

	myrepo_dir_set=$(repo forall -c 'echo $REPO_PATH' | tr "\n" " ")

	for ii in $myrepo_dir_set
	do
		cd $ii

		cd - > /dev/null
	done

	this_resume_pwd
}


## 清除各项目的修改
function repo-clean-all-prj()
{
	this_record_pwd

	cd $(gettop)
	local all_prj=`repo status | awk '$1=="project" && NF > 2  {printf $2 "  " }' | tr "[\r\n]" " "`
	local AATop=`pwd`
	if [ "$all_prj" ]; then
		for ii in $all_prj
		do
			if [ -d $ii -a -d "${ii}/.git"  ]; then
			cd $ii
				echo "now clean : $ii"
				git reset HEAD .
				git clean -df
				git checkout -- .
				cd $AATop
			fi
		done
	fi

	this_resume_pwd
}

############### git function end

#############################################################
## end for this script file
#############################################################
