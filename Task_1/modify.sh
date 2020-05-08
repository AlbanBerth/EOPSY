#!/bin/bash


#===============================================================================================
#Case -h
if [ $1 == -h ]
then
        echo -e "modify [-r] [-l|-u] <dir/file names...>"
        echo -e "modify [-r] <sed pattern> <dir/file names...>\n"
        echo -e "modify [-h]\n"
        echo -e "-r : Change name recursively \n-l : Change name in lowercase \n-u : Change name in uppercase \n-h : Print this help\n <sed pattern> : Use 'sed' command to modify name"
        exit
fi


#===============================================================================================
#File / Directory exists ?
i=$#
name=$(eval echo \$$i)

if [ ! -d $name ] && [ ! -f $name ]
then
        echo "No such file or directory named $name !"
        exit
fi




#===============================================================================================
#Functions
lowerCase()
{
	name_=$1
	nameLower_=${name_,,}
	if [ $nameLower_ == $name_ ] #test if already in lowercase
	then
		echo "Already in lowercase !"
	else
		mv $name_ $nameLower_ #change name
	fi
}

upperCase()
{
	name_=$1
	nameUpper_=${name_^^}
	if [ $nameUpper_ == $name_ ] #test if already in uppercase
	then
		echo "Already in uppercase !"
	else
		mv $name_ $nameUpper_ #change name
	fi
}

argumentSed=""

for element in $* #creat one argument for the sed command
do
	if [ $element != $name ]
	then
        	argumentSed+=" $element"
	fi
done

sedPattern()
{
	name_=$(echo "$name" | sed $argumentSed) #apply sed command on the name
	mv $name $name_ #change name
}


#===============================================================================================
#Case -l / -u without -r
j=0
if [ $1 != -r ]
then
	for argument in $* #search in all argument different case
	do
		case $argument in
			-l)	lowerCase $name #call founction lowercase
				exit
				;;
			-u)	upperCase $name #call founction uppercase
				exit
				;;
			*)	if [ $name != $argument ]
				then
					sedPattern $argumentSed 2> /dev/null #call founction sed 
				fi
				exit
				;;
		esac
	done
fi


#===============================================================================================
#Case -r
if [ $1 == -r ]
then
	if [ -d $name ]
	then
		oriPath="$PWD"
		cd $name #go in the directory

		ls -R1 > ls.txt #put all the element of the directory (recursivly) in a file
	
		find < ls.txt > abspath.txt #write the absolute path of all the element
	
		file -f abspath.txt | grep directory > ../dirabspath1.txt #keep only the directory
	
		sed 's/:.*$//' < ../dirabspath1.txt > ../dirabspath2.txt #keep only the path

		curPath="$PWD"
	
		sed 's|.|'"$curPath"'|' < ../dirabspath2.txt > ../dirabspath1.txt #write the path from the root of the system
		cat ../dirabspath1.txt > ../dirabspath2.txt #update second file
	
		#remove useless file
		rm ls.txt
		rm abspath.txt
		
		#change name recursivly
		while [ -s ../dirabspath1.txt ]
		do
			cd $(sed '$!d' < ../dirabspath1.txt) #go to the last directory
			for element in *
			do
				if [ $2 == -u ]
				then
					#change to uppercase
					upperCase $element -r
				else
					#change to lowercase
					lowerCase $element -r
				fi
			done
			#return to the first directory
			cd $(echo $curPath)
			#delete the last line (last directory)
			sed '$d' < ../dirabspath2.txt > ../dirabspath1.txt
			cat ../dirabspath1.txt > ../dirabspath2.txt #update second file
		done
	
		#delete all file creat in this script
		cd $(echo $oriPath)
	
		if [ $2 == -u ]
		then
	        	upperCase $name -r
		else
	        	lowerCase $name -r
		fi
	
		rm dirabspath1.txt
		rm dirabspath2.txt

	else
		echo "$name is not a directory !" 
	fi
fi
