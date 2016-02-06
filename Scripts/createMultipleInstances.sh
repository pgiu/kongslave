#!/bin/sh
# Genera un grupo de bat files para correr multiples instancias de KongSlaveTest con puertos y direccioens DNP continuas

let base=101
let total=1000
let max_outstations_per_channel=5
let start_port=14641
let remainder=0

for ((i=2; i<=$total; ++i )) ; 
do 

let "remainder = $i % $max_outstations_per_channel"

if [ "$remainder" -eq 0 ]; then
	
	let "start_port = $start_port + 1"
	echo "KongSlaveTest.exe tcpserver 0.0.0.0 $start_port $max_outstations_per_channel $base 100" > "slave_$start_port.bat"
	#El orden es asi (primero incrementa start_port y despues $base porque en la GUI hace esto. Si cambio, cambiar esto tb.
	let "base = $base + max_outstations_per_channel"
fi
done