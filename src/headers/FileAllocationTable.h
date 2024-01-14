/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Andy Cho, Leo Powers, Maliah Chin, Gabriella Joy Arcilla
* Student ID: 920484081, 921661426,921960921 921822951
* GitHub Name: andrewyjo, powersleo, maliah-c, gabriellaarcilla
* Group Name: boberts
 *
* Project: Basic File System
*
* File: FileAllocationTable.h
*
* Description:
*	This is the file system interface.
*	This is the interface needed to prototype the FileAllocationTable functions
*
*
**************************************************************/
#ifndef CSC415_FILESYSTEM_BOBERTS_FILEALLOCATIONTABLE_H
#define CSC415_FILESYSTEM_BOBERTS_FILEALLOCATIONTABLE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../fsLow.h"

#include "fileEntry.h"


//    ______              _______
//    |  ____|     /\     |__   __|
//    | |__       /  \       | |
//    |  __|     / /\ \      | |
//    | |       / ____ \     | |
//    |_|      /_/    \_\    |_|
int isFATThere(VolumeControlBlock * VCB);
void initFAT(VolumeControlBlock * VCB);

#endif //CSC415_FILESYSTEM_BOBERTS_FILEALLOCATIONTABLE_H
