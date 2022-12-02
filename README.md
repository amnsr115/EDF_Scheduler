# EDF_Scheduler
This repository shows an implementaion of an Earliest Deadline First Schelduler by modifying the FreeRTOS source code.

The EDF Scheduler was made with the help of the following paper:

Carraro, Enrico, 2016, Implementation and Test of EDF and LLREF Schedulers in FreeRTOS, Universita degli Studi di Padova
http://tesi.cab.unipd.it/51896/1/Implementation_and_Test_of_EDF_and_LLREF_Scgheduler_in_FreeRTOS.pdf

Along with the changes the paper made, other this we added. this includes modifying the Idle Task so that it always remains lower priority (Furthest Deadline), switch tasks when an earlier deadline task becomed unblocked and updating the deadlineof the tasks.

The main.c file contains a program that uses the modified FreeRTOS code.
