#!/bin/bash

################################################################################
####
################################################################################
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

####删除存在的文件
function del_exist_file() {
	if [ "$1" ]; then
		if [ -f "$1" ]; then
			rm "$1"
		fi
	fi
}

# 在repo同级目录下打git补丁
function repo_apply_git_patch() {
	if [ "$1" -a -f "$1" ]; then
		echo "-------------------------------------------------"
	else
		echo "no file $1"
		return 1
	fi
	
	git_patch_file="$1"
	#找项目路径
	PrjPath="`sed -n '1p' $git_patch_file | sed "s/^project //" `"
	PatchPath=`pwd`
	if [ -d "$PrjPath" ]; then
		pFile="${PatchPath}/${git_patch_file}"
		cd "$PrjPath" && pwd
		if [ -f "$pFile" ]; then
			#先检查是否可以打,然后再am
			echo -e "\e[1;33m apply --check : $pFile \e[0m" && git apply --check $pFile && echo "----------am $pFile ----------" && git apply --ignore-whitespace $pFile
			if [ "$?" == "0" ]; then
				#成功,可以删除patch文件了
				show_gtip "----------am $pFile ok, del it." && rm "$pFile"
			else
				#失败,退出处理
				show_vit "---- apply --check fail"
			fi
		else
			show_vit "no file : $pFile"
		fi
	else
		show_vit "no $PrjPath or get git-path error"
	fi
	cd "$PatchPath"
}


# 打repo diff产生的补丁
function apply_repo_patch() {
	if [ "$1" -a -f "$1" ]; then
		echo "now apply : $1"
	else
		echo "no file $1"
		return 1
	fi
	
	repo_patch_file="$1"

	#项目开始行数的集合
	lineRrjStartSet=`sed -n "/^project /=" $repo_patch_file | tr "\n" " "`
	#获取项目结束行数的集合
	lineRrjEndSet=""
	kk=0
	for i in $lineRrjStartSet
	do
		if [ $kk -eq 0 ]; then
			kk=`expr $kk + 1`
		else
			kk=`expr $kk + 1`
			lineRrjEndSet="$lineRrjEndSet `expr $i - 1`"
		fi
	done
	lineRrjEndSet=`echo $lineRrjEndSet | sed "s/^ //"`
	lineRrjEndSet="$lineRrjEndSet `sed -n '$=' $repo_patch_file`"

	#echo "$lineRrjStartSet"
	#echo "$lineRrjEndSet"

	#获取各库补丁
	kk=0
	for i in $lineRrjStartSet
	do
		for j in $lineRrjEndSet
		do
			if [ $i -gt $j ]; then
				continue
			else
				kk=`expr $kk + 1`
				tempPrjFile="${repo_patch_file}.prj${kk}.patch"
				sed -n "$i,${j}p" "$repo_patch_file" > $tempPrjFile
				repo_apply_git_patch $tempPrjFile
				break
			fi
		done
	done
	
}


###################################################################################
##生成邮件补丁，一般用全志的发布tag名做$1参数
###################################################################################
function create_repo_MailPatch() {
	local TagName1=$1
	##local TagName2=$2  ##暂未验证，可能有问题
	local TagName2="HEAD"
	
	if [ ! "$TagName1" ]; then
		echo "need para : tag or branch name"
		return 0
	fi

	local RepoPath=`pwd`
	local RepoName=${RepoPath##*/}
	local SDKPath=${RepoPath%/*}
	local SDKName=${SDKPath##*/}

	MY_DAY=$(date '+%Y%02m%02d')
	MY_TIME=$(date '+%02k%02M')
	MY_TIME_STAMP=${MY_DAY}   ##${MY_DAY}-${MY_TIME}

##生成补丁的目录
	local PatchsDir=${SDKPath}/patch/${MY_TIME_STAMP}/${RepoName}
##生成补丁的git库记录
	local GitPatchRecordFile=${SDKPath}/patch/${MY_TIME_STAMP}/${RepoName}/PatchGitName.log
##将要生成补丁的git库集合
	local DiffGitSet=""
	echo "getting diff $TagName1 $TagName2" 
	DiffGitSet=$(repo forall -p -c git diff --stat $TagName1 $TagName2 | grep "^project " | sed 's/project //')

	if [ "$DiffGitSet" ]; then
#		kk=0
		for CurGit in $DiffGitSet
		do
			show_vit "format-patch $CurGit"
			CurGitPatchPath=$PatchsDir/$CurGit
			mkdir -p $CurGitPatchPath
			## 到相应的git目录中产生补丁
			cd $RepoPath/$CurGit && git format-patch $TagName1 -o $CurGitPatchPath && echo "$CurGit" >> $GitPatchRecordFile
			cd $RepoPath
		done
		
		show_vit "done : $TagName1 patchs"
	else
		show_vit "no diff between $TagName1 $TagName2"
	fi
}

###################################################################################
##应用邮件补丁，一般用全志的发布tag名做$1参数
###################################################################################
function patch_repo-git-patch() {
	local PatchsDir="$1" ##补丁的repo对应目录
	if [ "$PatchsDir" ]; then
		if [ ! -d "$PatchsDir" ]; then
			echo "no $PatchsDir"
			return 1
		fi
	else
		echo "need repo-git-patch-dir"
		return 0
	fi
	local GitPatchRecordFile="$PatchsDir/PatchGitName.log" ##各git目录记录文件
	if [ ! -f $GitPatchRecordFile ]; then
		echo "no $GitPatchRecordFile"
		return 1
	fi

	ApllyPatchCmd=aplly
	if [ "$2" -a "$2" == "am" ]; then
		ApllyPatchCmd=am
	fi

	local RepoPath=`pwd`
	local RepoName=${RepoPath##*/}
	local SDKPath=${RepoPath%/*}
	local SDKName=${SDKPath##*/}
	
	DiffGitSet=$(cat $GitPatchRecordFile | tr '[\r\n]' ' ')
	if [ "$DiffGitSet" ]; then
#		kk=0
		for CurGit in $DiffGitSet
		do
			show_vit "$ApllyPatchCmd patch at $CurGit"
			CurGitPatchPath=$PatchsDir/$CurGit
			CurGitPatchs=`ls $CurGitPatchPath`
			## 到相应的git目录中打补丁
			cd $RepoPath/$CurGit

			for CurPatchFile in $CurGitPatchs
			do
				pFile=$RepoPath/$CurGit/$CurPatchFile
				if [ -f $pFile ]; then
					#先检查是否可以打,然后再应用
					echo -e "\e[1;33m apply --check : $pFile \e[0m" && git apply --check $pFile && git $ApllyPatchCmd --ignore-whitespace $pFile
					if [ "$?" == "0" ]; then
						#成功,可以删除patch文件了
						show_gtip "----------$ApllyPatchCmd $pFile ok, del it." && rm "$pFile"
					else
						#失败,退出处理
						show_vit "---- apply --check fail"
						##break
					fi
				else
					show_vit "no file : $pFile"
				fi
			done

			cd $RepoPath
		done
		
		show_vit "done : $ApllyPatchCmd $TagName1 patchs"
	else
		show_vit "no diff"
	fi
	
}

# ##############################end of file


