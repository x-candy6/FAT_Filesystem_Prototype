/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Andy Cho, Leo Powers, Maliah Chin, Gabriella Joy Arcilla
* Student ID: 920484081, 921661426,921960921 921822951
* GitHub Name: andrewyjo, powersleo, maliah-c, gabriellaarcilla
* Group Name: boberts
*
* Project: Basic File System
*
* File: dirEntry.h
*
* Description:
*   + This is the dirEntry.h for our FAT filesystem.
*   + It contains
*
*
**************************************************************/
#ifndef CSC415_FILESYSTEM_BOBERTS_DIRENTRY_H
#define CSC415_FILESYSTEM_BOBERTS_DIRENTRY_H
//a dirEntry size of 32b ; TODO figure out how timespec works
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../fsLow.h"
#include "VCB.h"
#include <time.h>

//  _ _        ______   _   _   _______   _____   __     __
//  | (_)      |  ____| | \ | | |__   __| |  __ \  \ \   / /
//__  | |_ _ __| |__    |  \| |    | |    | |__) |  \ \_/ /
///_`| | '__| | __|   | . ` |    | |    |  _  /    \   /
//| (_ | | | |    | |____  | |\  |    | |    | | \ \     | |
//\__,_|_|_|    |______| |_| \_|    |_|    |_|  \_\    |_|


struct  {
    char filename[11]; //first char will mark allocation status(0x00:FREE,0xe5:DELETED)
    unsigned int attribute: 3;
    unsigned int reserved: 4;
    unsigned int starting_cluster: 8;

    //So the following can probably be extracted from 🐱
    //struct timespec file_creation_time_precise;
    //struct timespec file_creation_time;
    //struct timespec file_modification_time;

    //🐱  -from this, but for now i'm commenting it out to get a size of 32 instead of 64
    time_t file_creation_date;
    time_t file_access_date;
    time_t file_modification_date;

    //26-27 Low-order 2 bytes of first cluster(what data type to use?)
    uint64_t filesize; // 0:DIR
}typedef dirEntry;

struct {
    dirEntry * directoryEntry;
    int index;

}typedef ppReturn;

//struct dirEntry * dubDotDir(VolumeControlBlock * VCB);
//
//struct dirEntry * dotDir(VolumeControlBlock * VCB);
#endif //CSC415_FILESYSTEM_BOBERTS_DIRENTRY_H
