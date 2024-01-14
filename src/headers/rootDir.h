/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Andy Cho, Leo Powers, Maliah Chin, Gabriella Joy Arcilla
* Student ID: 920484081, 921661426,921960921 921822951
* GitHub Name: andrewyjo, powersleo, maliah-c, gabriellaarcilla
* Group Name: boberts
 *
* Project: Basic File System
*
* File: rootDir.h
*
* Description:
*	This is a file system header.
*	This is a header needed to prototype function used in our filesystem
*
*
**************************************************************/
#ifndef CSC415_FILESYSTEM_BOBERTS_ROOTDIR_H
#define CSC415_FILESYSTEM_BOBERTS_ROOTDIR_H


//_____     ____     ____    _______
//|  __ \   / __ \   / __ \  |__   __|
//| |__) | | |  | | | |  | |    | |
//|  _  /  | |  | | | |  | |    | |
//| | \ \  | |__| | | |__| |    | |
//|_|  \_\  \____/   \____/     |_|

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../fsLow.h"

#include "dirEntry.h"


int isRootThere(VolumeControlBlock * VCB);
int initRoot(VolumeControlBlock * VCB);

#endif //CSC415_FILESYSTEM_BOBERTS_ROOTDIR_H
