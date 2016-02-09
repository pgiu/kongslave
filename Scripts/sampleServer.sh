#!/bin/bash 
# Simulacion de esclavo DNP. Llenar los siguientes parametros y correr este script
port=20000
number_of_dnp_clients=51
number_of_rtus_per_channel=1
start_address=101
master_address=100
../Release/kongslave tcpserver 0.0.0.0 $port $number_of_rtus_per_channel $number_of_dnp_clients  $start_address $master_address


