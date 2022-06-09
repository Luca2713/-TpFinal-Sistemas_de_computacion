#!/bin/bash -e
#DISCLAIMER: para poder ejecutar este código deberá tener previamente instalado
#archivos relacionados al kernel de linux, y los correspondientes módulos de python.

#Cargo variables de entorno
printf "LOADING env vars...\n"
./env_var.sh
printf "Env vars LOADED\n"

#Borro los archivos

printf "CLEANING old files...\n"
cd char_device_driver
make clean
cd ../api
make clean
cd ..
printf "Old files REMOVED"

#Compilo el código en c
printf "c code COMPILING...\n"
cd char_device_driver
printf "COMPILING kernel module...\n"
make all
printf "Kernel module COMPILED...\n"
cd ../api
printf "COMPILING api...\n"
make all
printf "api COMPILED...\n"cd ..
cd ..
printf "All c code COMPILED...\n"

insmod char_device_driver/sensors_driver.ko

#Levanto el servidor
./api/bin/api

