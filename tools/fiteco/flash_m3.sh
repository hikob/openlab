#!/bin/sh


IMG=/home/root/fiteco/$1

/usr/local/bin/openocd -f /home/root/fiteco/fiteco-m3.cfg -f target/stm32f1x.cfg -c init -c targets -c "reset halt" -c "reset init" -c "flash write_image erase $IMG" -c "verify_image $IMG" -c "reset run" -c shutdown

