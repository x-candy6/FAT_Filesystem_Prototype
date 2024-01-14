/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Andy Cho, Leo Powers, Maliah Chin, Gabriella Joy Arcilla
* Student ID: 920484081, 921661426,921960921 921822951
* GitHub Name: andrewyjo, powersleo, maliah-c, gabriellaarcilla
* Group Name: boberts
 *
* Project: Basic File System
*
* File: VCB.h
*
* Description:
*	This is a file system header.
*	This is a header needed to utilize the volumeControlBlock in our filesystem
*
*
**************************************************************/#ifndef CSC415_FILESYSTEM_BOBERTS_VCB_H
#define CSC415_FILESYSTEM_BOBERTS_VCB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../fsLow.h"
#include <time.h>


//    __      _______ ____
//    \ \    / / ____|  _ \
//     \ \  / / |    | |_) |
//      \ \/ /| |    |  _ <
//       \  / | |____| |_) |
//        \/   \_____|____/
struct  {

    // 0:signature, 1:volume_id, 2:block_size
    // 3:number_of_blocks, 4:sectors_per_cluster,
    // 5:number_of_sectors, 6:dirent_start,
    // 7:dirent_size, 8:data_start, 9:data_size,
    // 10:filecount, 11:volume_access_date,
    // 12:volume_mod_date, 13:volume_create_date

    //'security'
    unsigned long signatureP1;
    unsigned long signatureP2;
    unsigned int volume_id;

    //Raw Volume Information
    uint64_t block_size;
    uint64_t number_of_blocks;

    /*sectors_per_cluster dictates what kind of FAT we will have(FAT16, FAT32, etc),
     * It has to be a power of 2 */
    unsigned int sectors_per_cluster;
    unsigned int number_of_sectors;
    unsigned int number_of_clusters;

    unsigned int dirent_start;
    unsigned int dirent_size;

    int first_free; // first free cluster
    unsigned int working_directory; //starting cluster of first dir

    unsigned int data_start; //block number of the root directory AKA the data region
    unsigned int data_size;
    unsigned int file_count;

    time_t file_access_date;
    time_t file_modification_date;
    time_t file_creation_date;


}typedef VolumeControlBlock;

//void * refVCB(VolumeControlBlock * VCB);
void *VCBinit2( uint64_t numBlock, uint64_t bSize);
VolumeControlBlock *VCBinit1(uint64_t numBlock, uint64_t bSize);

#endif //CSC415_FILESYSTEM_BOBERTS_VCB_H
