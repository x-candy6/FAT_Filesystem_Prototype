/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Andy Cho, Leo Powers, Maliah Chin, Gabriella Joy Arcilla
* Student ID: 920484081, 921661426,921960921 921822951
 * GitHub Name: andrewyjo, powersleo, maliah-c, gabriellaarcilla
 * Group Name: boberts
 * Project: Basic File System
 *
 * File: fsInit.c
 *
 * Description: Main driver for file system assignment.
 *
 * This file is where you will start and initialize your system
 *
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "fsLow.h"
#include "mfs.h"
#include "headers/dirEntry.h"
#include "headers/FileAllocationTable.h"
#include "headers/fileEntry.h"
#include "headers/rootDir.h"
#include "headers/VCB.h"

//
#define CLUSTER_SIZE 512
#define ROOT_SIZE 8192 //Root can store up to 256 file(8192/32(size of dirEntry))
#define RDO 0          // vim auto-count: ctrl-v, down(select all numbers), 1, ctrl-a, voila
#define HID 1
#define SYS 2
#define VOL 3
#define DIR 4
#define ZIP 5
#define TMP 6 //nevermind, we don't need these definitions, delete during maintenence

//FileAttributes for DirEntry
//0:R, 1:H, 2:S, 3:V, 4:D, 5:Z, 6:T  t for temp, z for zip, d for dir, v for vol, h for hidden, r for readonly
static unsigned int ID_START = 0;

void refVCB(VolumeControlBlock *VCB)
{ //refreshVCB
    LBAwrite(VCB, 1, 0);
    printf("\nChanges written to VCB\n");
}

// dirEntry * dubDotDir(VolumeControlBlock * VCB){
//     struct dirEntry * dubDotDir;
//     dubDotDir->filename[0] = '.';
//     dubDotDir->filename[1] = '.';
//     dubDotDir->attribute = 4;
//     return dubDotDir;
// }

// dirEntry * dotDir(VolumeControlBlock * VCB){
//     struct dirEntry * dotDir;
//     dotDir->filename[0] = '.';
//     dotDir->attribute = 4;
//     return dotDir;
// }

int initRoot(VolumeControlBlock *VCB)
{
    int root_start = VCB->data_start;
    printf("\ninitializing root...\n");
    printf("\nVCB->DE:%ld", sizeof(dirEntry));
    dirEntry *rootDir = malloc(VCB->block_size * 16); //change the 16 to be dynamic

    //Commented out, the root dir doesn't have '..' or '.'
    //but we do have to implement those features
    //// The '.' directory entry is an entry to the current working directory
    //rootDir[0].filename[0] = '.';
    strcpy(rootDir[0].filename, "/");
    rootDir[0].attribute = 4;
    rootDir[0].starting_cluster = VCB->data_start;
    printf("\nrootDir.filename:%s, attr:%d, start:%d \n", rootDir[0].filename, rootDir[0].attribute, rootDir[0].starting_cluster);
    //rootDir[0].starting_cluster = VCB->data_start;
    //rootDir[0].attribute = 4; // 4 for directory

    //// The '..' directory entry is an entry to the parent of the cwd
    //rootDir[1].filename[0] = '.';
    //rootDir[1].filename[1] = '.';
    ////rootDir[1].starting_cluster = VCB->data_start;
    //rootDir[1].attribute = 4; // 4 for directory
    //printf("\nrootDir->fn:%s", rootDir->filename);

    VCB->first_free = 160;//VCB->data_start + 17; //what is 17?
    printf("FIRST FREE: %d \n", VCB->first_free);
   //again hardcoded, change
    LBAwrite(VCB, 1, 0);
    //refVCB(VCB); //refresh VCB

    LBAwrite(rootDir, 16, rootDir[0].starting_cluster);
    printf("\nroot directory has been initialized at cluster %d and ending at cluster %d\n", rootDir[0].starting_cluster, VCB->first_free);

    return 0;
}

int isRootThere(VolumeControlBlock *VCB)
{
    dirEntry *rootProxy = malloc(512);
    LBAread(rootProxy, 1, VCB->data_start);

    if (strcmp(rootProxy->filename, "/") == 0)
    {
        printf("FIRST FREE: %d\n", VCB->first_free);
        return 1;
    }
    else
    {
        printf("\nroot directory doesn't exist at cluster:%d\n", VCB->data_start);
        return 0;
    }
}

int isFATThere(VolumeControlBlock *VCB)
{
    int there;
    FileEntry FAT_proxy[VCB->number_of_clusters];
    int FAT_size = (sizeof(FAT_proxy) / VCB->block_size) + 1; //size in blocks/clusters(clusters=blocks for this test case)
    LBAread(FAT_proxy, FAT_size, 1);
    if (FAT_proxy[0].busy == 0)
    {
        printf("\nFAT exists at cluster [1]");
        return 1;
    }
    printf("\nFAT does not at cluster [1]");
    return 0;
}


void initFAT(VolumeControlBlock *VCB)
{
    //Check if FAT table exists
    //Check if RootDir is initiated
    //int FAT_check = isFATThere(VCB);
    //init FAT to be LBAwritten
    FileEntry FAT[VCB->number_of_clusters];

    printf("\nFAT:%ld", sizeof(FAT));
    int FAT_size = (sizeof(FAT) / VCB->block_size) + 1; //size in blocks/clusters(clusters=blocks for this test case)
    printf("\nFAT_Size:%d", FAT_size);
    VCB->data_start = FAT_size + 1;
    VCB->working_directory = FAT_size + 1;
    printf("\nVCB->data_start:%d", VCB->data_start);
    LBAwrite(VCB, 1, 0);

    //test
    //LBAread(VCB, 1, 0);
    //printf("\nVCB->data_start:%d", VCB->data_start);

    //FAT 0 - VCB
    FAT[0].busy = 0;
    FAT[0].next = -1;

    //FAT 1 - FileAllocationTable(aka FAT)
    FAT[1].busy = 1;
    FAT[1].next = -1;

    //FAT 2 - RootDirEntry
    FAT[2].busy = 1;
    FAT[2].next =
    printf("\nFE:%ld", sizeof(FAT[2]));
    printf("\nDE:%ld", sizeof(dirEntry));
    //TODO need to initRoot()
    int rootCheck = isRootThere(VCB);
    if (rootCheck == 0)
    {
        initRoot(VCB);
        rootCheck = isRootThere(VCB);
        if (rootCheck == 0){
            printf("no root");
        }
    }
    //FAT[2].next = ;
    LBAwrite(FAT, FAT_size, 1);

    printf("\nFat has been initialized\n");
}

// get next free block chunk if none return -1;
// might need function to reset block chunks to free? maybe we just free blocks
// as needed?

// How many Directory Entries you want for a directory, an initial amount
// Multiply the size of the entry by the number of entries
//  prod1 = 2 * sizeof(dirEntry)
//  blocksReq = prod1/512 + 1
//  ask for blocksReq

void volumeAuth(VolumeControlBlock vcb)
{
    return;
}

// Returns a pointer to a VCB object
VolumeControlBlock *VCBinit1(uint64_t numBlock, uint64_t bSize)
{
    printf("\nInitializing VCB attributes...\n");
    VolumeControlBlock *VCB = (VolumeControlBlock *)malloc(sizeof(VolumeControlBlock));

    //auth
    printf("\ninitializing signatures");
    VCB->signatureP1 = PART_SIGNATURE;
    VCB->signatureP2 = PART_SIGNATURE2;
    //Set volume_id and then increment the global var ID_START
    VCB->volume_id = ID_START;
    ID_START++;

    //LBA info
    printf("\nsetting sector/cluster sizes");
    VCB->block_size = bSize;
    VCB->number_of_blocks = numBlock;

    int sec_per_clu;
    //FIX :: faulty logic here
    sec_per_clu = CLUSTER_SIZE / bSize;
    VCB->number_of_sectors = numBlock;
    VCB->sectors_per_cluster = sec_per_clu;
    VCB->number_of_clusters = 19531;//(numBlock/sec_per_clu) + 1;
    VCB->first_free = 155;
    LBAwrite(VCB, 1, 0);
    printf("\ninitializing FAT...");

    //07 : dirent_start
    // Here we need to init the FSM aka FAT table
    initFAT(VCB);
    //testing

    //printf("\n%d\n%d\n", VCB->volume_id, ID_START);

    return VCB;
}
//Function to validate volume, contrary to naming standards, this is called first, and then VCBinit1
void *VCBinit2(uint64_t numBlock, uint64_t bSize)
{
    printf("\nInitializing VCB attributes...\n");
    VolumeControlBlock *authVCB = (VolumeControlBlock *)malloc(512);
    LBAread(authVCB, 1, 0);

    if (authVCB->signatureP1 == PART_SIGNATURE)
    { // P2 ISSUE: find out how to mitigate that == warning
        if (authVCB->signatureP2 == PART_SIGNATURE2)
        {
            printf("\nVCB Check Passed!");
            return authVCB;
        }
        else
        {
            printf("\nVCB Check 2 Failed: signatureP2 != PART_SIGNATURE2\nInitializing new Volume...\n");
            //Call function to init vcb
            VCBinit1(numBlock, bSize);
        }
    }
    else
    {
        printf("\nVCB Check 1 Failed: signatureP1 != PART_SIGNATURE\nInitializing new Volume...\n");
        //Call function to init vcb
        VCBinit1(numBlock, bSize);
    }
}
int initFileSystem(uint64_t numberOfBlocks, uint64_t blockSize)
{
    printf("Initializing File System with %ld blocks with a block size of %ld\n",
           numberOfBlocks, blockSize);
    /* TODO: Add any code you need to initialize your file system. */

    printf("\nInitializing VCB...\n");
    //VolumeControlBlock * VCB = (VolumeControlBlock *) malloc(sizeof(VolumeControlBlock));
    //memcpy(VCB,VCBinit1(numberOfBlocks, blockSize), sizeof(VolumeControlBlock));
    VCBinit2(numberOfBlocks, blockSize);

    //Because we are using a compound-literal to initialize our struct
    //Here are the variable values for reference
    // 0:signature
    // 1:volume_id
    // 2:block_size
    // 3:number_of_blocks
    // 4:sectors_per_cluster
    // 5:number_of_sectors
    // 6:dirent_start
    // 7:dirent_size
    // 8:data_start
    // 9:data_size
    // 10:filecount
    // 11:volume_access_date
    // 12:volume_mod_date
    // 13:volume_create_date

    int sigCheck = 0;

    VCBinit1(numberOfBlocks, blockSize);
    printf("\nVolume has been initialized...\n");

    //VolumeControlBlock * VCB_proxy = (VolumeControlBlock *) malloc(512);
    //LBAread(VCB_proxy, 1, 0);
    //printf("\nVCB_Proxy:%d, root_start:%d", VCB_proxy->volume_id, VCB_proxy->number_of_clusters);

    return 0;
}
void exitFileSystem() { printf("Hi i'm a filesystem, i'm exiting, System exiting!\n"); }
