
/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Andy Cho, Leo Powers, Maliah Chin, Gabriella Joy Arcilla
* Student ID: 920484081, 921661426,921960921 921822951
* GitHub Name: andrewyjo, powersleo, maliah-c, gabriellaarcilla
* Group Name: boberts
 *
* Project: Basic File System
*
* File: fileEntry.h
*
* Description:
*	This is a file system header.
*	This is a header needed to utilize the FAT attributes in our filesystem
*
*
**************************************************************/
#ifndef CSC415_FILESYSTEM_BOBERTS_FILEENTRY_H
#define CSC415_FILESYSTEM_BOBERTS_FILEENTRY_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../fsLow.h"

//______              _______     ______ _   _ _______ _______     __
//|  ____|     /\     |__   __|   |  ____| \ | |__   __|  __ \ \   / /
//| |__       /  \       | |      | |__  |  \| |  | |  | |__) \ \_/ /
//|  __|     / /\ \      | |      |  __| | . ` |  | |  |  _  / \   /
//| |       / ____ \     | |      | |____| |\  |  | |  | | \ \  | |
//|_|      /_/    \_\    |_|      |______|_| \_|  |_|  |_|  \_\ |_|

/*An entry in the FAT table*/
// TODO P2 make it take 16bits rather than 32bits
struct {
    unsigned int busy : 1; //a bit-wdith of 01 for 0|1 value
    //Change the logic for this, it should be dependent on the volsize
    unsigned int next: 15; //a bit-width of 15 for upto 65536 clusters
}typedef FileEntry;


#endif //CSC415_FILESYSTEM_BOBERTS_FILEENTRY_H
