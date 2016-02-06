REM Simulacion de esclavo DNP. Llenar los siguientes parametros y correr este bat.
set port=15042
set max_RTU_per_channel=1
set number_of_dnp_clients=100
set start_address=502
set master_address=100
KongSlaveTest.exe tcpserver 0.0.0.0 %port% %max_RTU_per_channel% %number_of_dnp_clients% %start_address% %master_address%
