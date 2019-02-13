#!/usr/bin/env bash

ROOT=~/CS378h/ray
PICS="${ROOT}/pics"
REF="${ROOT}/ray-solution"
RAY="${ROOT}/build/bin/ray"
RAYARGS="-r 5"
EXT=png
r_flag=false
R_flag=false
c_flag=false
file=''

# if pics doesn't exist, make
if [ ! -d $PICS ]
then
	mkdir $PICS
fi

if [ ! -f $RAY ]
then
	echo "Ray executable not found."
	echo $RAY
	exit 1
fi

if [ ! -f $REF ]
then
	echo "Reference executable not found."
	exit 1
fi

# if -R flag, run reference sol
while getopts 'rRcf:' flag; do
  case "${flag}" in
    r) r_flag=true ;;
    R) R_flag=true ;;
	c) c_flag=true ;;
    f) file="${OPTARG}" ;;
     esac
done

if [ file = '' ] || [ ! -f "${file}" ]
then 
	echo "Enter valid file path"
	exit 1	
fi

# save result as o_ or r_***_render.bmp in pics
filename=${file##*/}
filename=${filename%.*}
ourTrace=${PICS}/o_${filename}.${EXT}
refTrace=${PICS}/r_${filename}.${EXT}
traceDiff=${PICS}/${filename}_diff.${EXT}
if [ $r_flag = true ]
then
	${RAY} ${RAYARGS} ${file} $ourTrace
fi

if [ $R_flag = true ]
then
	${REF} ${RAYARGS} ${file} $refTrace
fi

# diff image
if [ $c_flag = true ] && [ $R_flag = true ] && [ $r_flag = true ]
then
	compare $refTrace $ourTrace -compose src $traceDiff
	montage $ourTrace $refTrace $traceDiff -tile 3x1 -geometry +0+0 ${PICS}/${filename}.${EXT}
	rm $ourTrace
	rm $refTrace
	rm $traceDiff
fi
