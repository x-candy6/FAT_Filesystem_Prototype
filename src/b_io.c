/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Andy Cho, Leo Powers, Maliah Chin, Gabriella Joy Arcilla
* Student ID: 920484081, 921661426,921960921 921822951
* GitHub Name: andrewyjo, powersleo, maliah-c, gabriellaarcilla
* Group Name: boberts
* Project: Basic File System
*
* File: b_io.c
*
* Description: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"
#include "headers/VCB.h"
#include "headers/dirEntry.h"
#include "headers/FileAllocationTable.h"
#include "headers/fileEntry.h"
#include "mfs.h"



#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	/** TODO add al the information you need in the file control block **/
    VolumeControlBlock *vcb;
    dirEntry *file;
    char * buff;		//holds the open file buffer
    int currentBlk;		//holds the current position in the buffer
    int buflen;		//holds how many valid bytes are in the buffer
    int index;
    int numBlocks;//holds how many valid bytes are in the buffer
	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].buff = NULL; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].buff == NULL)
			{
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open (char * filename, int flags)
	{
	b_io_fd returnFd;
    VolumeControlBlock *vcb;
    dirEntry * file; // not sure if its dirEnt or vcb
    ppReturn* ret =  parsePath(filename);
    char * buffer;

    int x = ret->index;
    file = ret->directoryEntry->filename;

        //printf("FILE NAME %s \n", ret->directoryEntry->filename);
        if(file == NULL)
        {
            return(-2); //file not found
        }
	//*** TODO ***:  Modify to save or set any information needed
	//
	//
		
	if (startup == 0) b_init();

    buffer = malloc(B_CHUNK_SIZE);//give mem // change to by dynamic
    if(buffer == NULL){
        return (-1);
    }//Initialize our system

    // fileInfo* fileInfoPtr = GetFileInfo(filename); // get fileInfo struct of

    returnFd = b_getFCB();				// get our own file descriptor
										// check for error - all used FCB's

    // initialize fcb

    fcbArray[returnFd].buff = buffer;
    fcbArray[returnFd].vcb = vcb;
    fcbArray[returnFd].index = 0;
    fcbArray[returnFd].currentBlk = 0;
    fcbArray[returnFd].buflen = B_CHUNK_SIZE;
    fcbArray[returnFd].file = file;
    fcbArray[returnFd].numBlocks = vcb->number_of_blocks;

	return (returnFd);						// all set
	}


// Interface to seek function	
int b_seek (b_io_fd fd, off_t offset, int whence)
	{
	if (startup == 0) b_init();  //Initialize our system
    int offsetReturn;
        // check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}

    if(offset < fcbArray[fd].file->filesize)
    {
        offsetReturn = LBAread(fcbArray[fd].buff, offset, whence);
        LBAwrite(fcbArray[fd].index, offset, whence);
    }else{
        printf("error, offset exceeds end of file");
        return(-1);
        }

        return (offsetReturn);//Change this
	}



// Interface to write function	
int b_write (b_io_fd fd, char * buffer, int count)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
		
	return (count); //Change this
	}



// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read (b_io_fd fd, char * buffer, int count)
	{
    int blocksRead;
    int bytesReturn;
    int p1, p2,p3;
    int blocksToCopy;
    int remaningBytes;

	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}





        if(fcbArray[fd].file == NULL){
            printf("file is null");
            return -1;
        }
//calculate each part for readinging into the buffer
    remaningBytes = fcbArray[fd].buflen - fcbArray[fd].index;

    int amountDelivered = (fcbArray[fd].currentBlk*B_CHUNK_SIZE);
    if((count + amountDelivered > fcbArray[fd].file->filesize))
    {
        count = fcbArray[fd].file->filesize - amountDelivered;
        if(count < 0)
        {
            printf("thats not right... leaving");
            //here is end of file
        }
    }

    if(remaningBytes >= count)
    {
        p1 = count;
        p2 = 0;
        p3 = 0;

    }else{ //case of if we need to give more bytes
        p1 = remaningBytes; // whats in our buffer
        p3 = count - remaningBytes; //whats left to give

        blocksToCopy = p3/ B_CHUNK_SIZE; //how many blocks
        p2 = blocksToCopy * B_CHUNK_SIZE; //bytes left in blocks

        p3 = p3-p2; //remaining bytes after whole blocks
    }

    if(p1>0)
    {
        memcpy(buffer,fcbArray[fd].buff + fcbArray[fd].index, p1);
        fcbArray[fd].index = fcbArray[fd].index + p1;

    }

    if(p2>0)
    {
        blocksRead = LBAread(buffer+p1,blocksToCopy, fcbArray[fd].currentBlk + fcbArray[fd].index );
        fcbArray[fd].currentBlk +=blocksToCopy;
        p2 = blocksRead;
    }

    if(p3>0)
    {
        blocksRead = LBAread(fcbArray[fd].buff, 1, fcbArray[fd].currentBlk + fcbArray[fd].file->starting_cluster);
        fcbArray[fd].currentBlk +=1;
        fcbArray[fd].index =0;
        fcbArray[fd].buflen = blocksRead;

        if(blocksRead < p3)
        {
            p3 = blocksRead;
        }
        if(p3>0)
        {
            memcpy(buffer+p1+p2, fcbArray[fd].buff + fcbArray[fd].index, p3);
            fcbArray[fd].index = fcbArray[fd].index +p3;
        }
    }
    bytesReturn = p1+p2+p3;
    return (bytesReturn);


}

// Interface to Close the file
int b_close (b_io_fd fd)
{
    free(fcbArray[fd].buff);
    fcbArray[fd].buff = NULL;
    fcbArray[fd].file = NULL;
    fcbArray[fd].vcb = NULL;

    return 0;
}
