#!/bin/bash

#./waifu.sh | tee -a log_flintstones_waifu.txt

echo Inicio

#paths
path_in=Flintstones/Frames
path_out=Flintstones/waifu	
SECONDS=0

#for image
for file_path in $path_in/*.png; do 
	tempo_por_img=$SECONDS

	#paths
	file_name=$(basename $file_path)
	file_path_out=$path_out/$file_name
	
	#run
	sudo docker run --rm -i siomiz/waifu2x -m noise_scale --noise_level 2 --scale_ratio 4 < $file_path > $file_path_out

	#print
	tempo_por_img=$(($SECONDS-$tempo_por_img))
	echo $file_name [$tempo_por_img s / $SECONDS segundos]
done

echo Fim [$SECONDS segundos]
