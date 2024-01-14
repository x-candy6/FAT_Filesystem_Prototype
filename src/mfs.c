/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Andy Cho, Leo Powers, Maliah Chin, Gabriella Joy Arcilla
* Student ID: 920484081, 921661426,921960921 921822951
* GitHub Name: andrewyjo, powersleo, maliah-c, gabriellaarcilla
* Group Name: boberts
 *
* Project: Basic File System
*
* File: mfs.c
*
* Description:
*	This is the file system interface.
*	This is the interface needed by the driver to interact with
*	your filesystem.
*
**************************************************************/
#include "mfs.h"
#include "fsLow.h"
#include "headers/VCB.h"
#include "headers/dirEntry.h"
#include "headers/FileAllocationTable.h"
#include "headers/fileEntry.h"

//get VCB from memory
VolumeControlBlock * getVCB() {
    VolumeControlBlock *vcb = malloc(512);
    LBAread(vcb, 1, 0);
    return vcb;
}





//get the FAT table from Memory
FileEntry * getFAT(){
    VolumeControlBlock * vcb = malloc(512);
    LBAread(vcb, 1, 0);
    FileEntry  * FAT_proxy = malloc(sizeof(FileEntry) * vcb->number_of_clusters);
    printf("\nsizeof FE:%ld\n", sizeof(FileEntry));
    int FAT_size = (sizeof(FAT_proxy) / vcb->block_size) + 1; //size in blocks/clusters(clusters=blocks for this test case)
    LBAread(FAT_proxy, FAT_size, 1);
    free(vcb);
    return FAT_proxy;
}

//swap files in the directory
int fs_swapFiles(const char *path, const char *path2){
    //set file locations to be negative 1
    int fileLoc1 = -1;
    int fileLoc2 = -1;
    //parsePath to get file location of path1
    ppReturn *ret = parsePath(path);
    int directoryLoc = ret->index;

    dirEntry *readBlock = malloc(512);
    LBAread(readBlock, 1, directoryLoc);
    for(int i = 0; i < 16; i++){
        if(strcmp(readBlock[i].filename, path) == 0 ){
            fileLoc1 = i;

        }
        if(strcmp(readBlock[i].filename, path2) == 0){
            fileLoc2 = i;
        }
    }
    //if we find file locaton swap the files
    if(fileLoc1 != -1&& fileLoc2 != -1){
        dirEntry temp = readBlock[fileLoc1];
        readBlock[fileLoc1] = readBlock[fileLoc2];
        readBlock[fileLoc2] = temp;
        LBAwrite(readBlock, 1, directoryLoc);
    }
    return 0;




}
//gets first free Entry within the directory located in the cluster
int firstFreeEntry(int clusterNum)
{
    dirEntry *readBlock = malloc(512);
    LBAread(readBlock, 1, clusterNum);
    int firstFree = -1;
    for (int i = 0; i < 16; i++)
    {
        if (strcmp(readBlock[i].filename, "") == 0)
        {
            firstFree = i;
            break;
        }
    }
    printf("\nFirst Free Cluster Index:%d\n", firstFree);
    free(readBlock);
    return firstFree;
}




#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
//returns n-1 directory if found; else return -1
ppReturn * parsePath(const char *path){
    ppReturn * out = malloc(sizeof(ppReturn));
    VolumeControlBlock * vcb = getVCB();
    FileEntry * FAT = getFAT();

    char * currentlyParsing;    //path is parsed one char at a time
    unsigned int entry_size = sizeof(dirEntry);  //A 32b entry designated for iteration across that fileblock
    unsigned int current_wd;    //  Insinuated to be the cluster of the current working directory
    //set is there to be 1
    int isThere = 1;            // Does the file exist?
    dirEntry *readBlock = (dirEntry * )malloc(vcb->block_size);    // A buffer to read the block into

    if(path[0] == '/'){    //test if relative
        current_wd = vcb->data_start;  // if it is the working directory is the root(?)
    }else{
        current_wd = vcb->working_directory; //otherwise set the wd to the that in the vcb
    }



    //determine if absolute or relative path
    //might need to strip spaces before, tbh tho users should just not


    //find end of current cluster
    unsigned int endOfFiles = firstFreeEntry(current_wd);    //endOfFiles: first free directory entry


    //need 2 output inorder to save the n-1 path rather than n

    //set currentPath to zero inorder to start from most recent path
    int currentPath = current_wd;    // sentinel
    int outputPath = current_wd;         // end-value

    currentlyParsing = strtok(path, "/ ");  //Parse through the given path, it will end in \0

    //tokenizing inorder to parse files given with a path
    //adding space to delimiter disallows spaces from our file path this is in line with other linux filesystems, just be aware

    LBAread(readBlock, 1, current_wd);
    //get directory entries for current directory


    while( currentlyParsing != NULL ) {



        //parse from current path because files inside other directories should exist in memory blocks further down
        for (int x = 0; x < endOfFiles; x++)
        {


            isThere = strcmp(currentlyParsing, readBlock[x].filename);
            if (isThere == 0)
            {
                outputPath = currentPath;
                currentPath = readBlock[x].starting_cluster;
                LBAread(readBlock, 1, readBlock[x].starting_cluster);
                //if we find it read into block and break inner for loop
                break;

            }
        }
        if(isThere != 0){
            out->directoryEntry = NULL;
            out->index = -1;
            return out;
        }
        currentlyParsing = strtok(NULL, "/ ");
        //check to see if directory exists inside
        //going into next cluster if other is found
    }
    out->directoryEntry = readBlock;
    out->index = outputPath;

    return out;

}
#pragma clang diagnostic pop


//Helper function
void printVCB(VolumeControlBlock *filen)
{
    //struct VolumeControlBlock * filen = (struct VolumeControlBlock *) malloc(512);
    LBAread(filen, 1, 0);
    printf("\n•VCB INFO•\nVCB id:%d\n", filen->volume_id);
    printf("VCB numClusters:%d\n", filen->number_of_clusters);
    printf("VCB first free:%d\n", filen->first_free);
    printf("VCB data start:%d\n", filen->data_start);
    printf("VCB data size:%d\n", filen->data_size);
    printf("VCB working_directory:%d\n", filen->working_directory);
    printf("VCB sizeof(VCB):%ld\n", sizeof(filen));
    printf("VCB file count:%d\n•END VCB INFO•\n", filen->file_count);
    //printf("\nVCB:%d\n", VCB->first_free);
    //printf("\nVCB:%d\n", VCB->volume_id);
}


//set the current working directory
int fs_setcwd(char *pathname)
{

    printf("\n%ld\n", sizeof(dirEntry));
    VolumeControlBlock *filen = (VolumeControlBlock *)malloc(512);
    LBAread(filen, 1, 0);
    unsigned int current_wd = filen->working_directory;
    dirEntry * entry = (dirEntry *) malloc(512);
    LBAread(entry, 1, current_wd);

    int found = 0;
    for(int i = 0; i < 16; i++){
        printf("\n%s", entry[i].filename);
       if(strcmp(entry[i].filename, pathname)){
           filen->working_directory = i;
           LBAwrite(filen, 1, 0);
           free(filen);
           free(entry);

           return 0;
       }
    }

    //struct dirEntry * readBlock = (struct dirEntry *) malloc(vcb->block_size);
    //LBAread()
    //struct dirEntry * readBlock = (struct dirEntry *) malloc(filen);

    //parsepath to get the starting cluster of pathname
    //set vcb->cwd to ^^^
    //return the vcb-cwd
    //if error return -1

    free(filen);
    free(entry);
    return -1;
} //linux chdir

/**
 * __________________________________________________
 *│  0   │  1  │ 2 │  3 │   4 │  5│   6  │  7 │8 │9  │
 *│ VCB  │ FAT │   │ FAT│ ROOT│   │ ROOT │    │  │   │
 *└──────┴─────┴───┴────┴─────┴───┴──────┴────┴──┴───┘
 *
 * mkdir:
 * -> start with only 1 dirEntry(root)
 * -> 'mkdir asdf'
 * -> read vcb and get first free cluster(the first one after root, block 7 in this case)🐲
 * -> get cwd , which is root
 * -> read the root dir, find the first free entry([1] in this case)
 * -> Set that entry to the dirEntry and set the cluster number to the first free cluster from 🐲
 * -> Increment the first_free_cluster in vcb
 * -> Set the FAT entry to 1:starting_cluster
 * **/
int fs_mkdir(const char *pathname, mode_t mode){

    VolumeControlBlock * vcb = (VolumeControlBlock *) malloc(512);//getVCB(); //TODO - again hardcoded, change
    LBAread(vcb, 1, 0);

    FileEntry * FAT = (FileEntry *)malloc(512 *  153);  //🐱
    LBAread(FAT, 153, 1);                               //🐱

    //printf("\nbusy:%d", FAT[0].busy);

    unsigned int current_wd;
    int firstFree;
    firstFree = vcb->first_free;
    current_wd = vcb->working_directory;
    printf("\ntest:%d firstfree: %u ", current_wd, vcb->first_free);
    printf("\ntest:%d wd: %d ", current_wd, vcb->working_directory);
    dirEntry * entries = (dirEntry *)malloc(40 * 16);
    LBAread(entries, 1, current_wd);
    //int firstFreeEntry = 1;
    //for(int i = 0; i < 16; i++){
    //    if(strcmp(entries[i].filename , "\0")){
    //        firstFreeEntry = i;
    //       break;
    //    } else {
    //        printf("\nfirstFree entry in dir: %d", firstFreeEntry);

    //    }
    //}
    //if (firstFreeEntry== -1){
    //    firstFreeEntry = 1;
    //}
    int firstFreeEntry = entries[0].reserved;
    printf("\nfirstFree entry in dir: %d", firstFreeEntry);
    strcpy(entries[firstFreeEntry].filename, pathname);
    entries[firstFreeEntry].attribute = 4;

    FAT[vcb->first_free].next = vcb->first_free + 1;  //🐱
    FAT[firstFreeEntry].busy = 1;                    //🐱

    LBAwrite(FAT, 153, 1);
    printf("\nnext:%d", FAT[firstFreeEntry].next);

    entries[0].reserved = firstFreeEntry + 1; //reserved is the first free entry index in that folder
    LBAwrite(entries, 1, current_wd);


    //Setting the data region to the new dirEntry
    dirEntry * newEntryData = (dirEntry *) malloc(512);strcpy(newEntryData[0].filename, pathname);
    newEntryData[0].starting_cluster = firstFreeEntry;
    newEntryData[0].attribute = 4;
    newEntryData[0].reserved = 3;

    //dubDot entry
    strcpy(newEntryData[1].filename, ".");
    newEntryData[1].attribute = 4;
    newEntryData[1].starting_cluster = firstFreeEntry;

    strcpy(newEntryData[2].filename, "..");
    newEntryData[2].attribute = 4;
    newEntryData[2].starting_cluster = current_wd;

    LBAwrite(newEntryData, 1, firstFreeEntry);


    free(vcb);
    free(entries);
    free(FAT);
    free(newEntryData);

    //LBAread(entries, 1, current_wd);
    //printf("\nentries:%d    %s", entries[1].attribute, entries[1].filename);

    return 0;
}


/**
 *  ARCHIVE
 * **/


//int getNextFreeBlock(int FATNum){
//    FileEntry * FAT = getFAT();
//    VolumeControlBlock * vcb = getVCB();

//    for(int i = FATNum; i < vcb->number_of_clusters; i++){
//        if(FAT[i].busy == 0){
//            free(FAT);
//            return i;
//        }
//    }

//}

//int writeVCB(VolumeControlBlock * vcb){
//    return LBAwrite(vcb, 1, 0);
//}
//remember to free :)


/*
 *  The Current Working Directory
 *  The starting cluster of the cwd is stored in the vcb
 *  By default, it is root.
 *
 *  Given a path name, it uses parse path to find that directory
 *  if parsepath returns true, set the working directory to the starting cluster returned by parsepath
 *  if returns false, that directory doesn't exist and return an error
 *
 *  for getCWD(buffer, size), we get the starting cluster of the cwd from the vcb
 *  Goto that cluster, and read the directory name
 *  read the .. dirEntry until you hit root, appending the name and a  '/'
 *  return the entire name into that buffer
 *  verify using the size
 *
 * */


//returns the current working directory
//char *fs_getcwd(char *pathname, size_t size)
//{
//
//    VolumeControlBlock *vcb = (VolumeControlBlock *)malloc(512);
//    char * fname = malloc(sizeof(char) * 11);
//    LBAread(vcb, 1, 0);
//    unsigned int start_wd = vcb->working_directory;
//    printf("\ncwd start:%d\n", start_wd);
//    dirEntry *readBlock = (dirEntry *)malloc(vcb->block_size);
//    //printf("\nsizeof dirEntry:%d\n", sizeof(struct dirEntry));
//    LBAread(readBlock, 1, start_wd);
//    strcpy(fname, readBlock[0].filename);
//    //printf("\n%s\n", fname);
//    free(readBlock);
//    free(vcb);
//
//    //parsepath to return starting cluster of pathname
//    //memcpy(parsepath, pathname, size);
//
//    return *fname;
//}


//int fs_mkdir(const char *pathname, mode_t mode)
//{
//    VolumeControlBlock * vcb = getVCB(); //TODO - again hardcoded, change
//    FileEntry * FAT = getFAT();
//    unsigned int current_wd;
//    int firstFree;
//
//    dirEntry *readBlock = (dirEntry * )malloc(vcb->block_size);    // A buffer to read the block into
//
//    //determine path type
//
//    if(pathname[0] == '/'){
//        printf("relative");
//        current_wd = vcb->data_start;
//    }else{
//        current_wd = vcb->working_directory;
//    }
//
//    ppReturn * ppReturn = parsePath(pathname);
//    int pathCluster = ppReturn->index;
//    printf("PATH NUMBER: %i \n",pathCluster);
//
//    LBAread(readBlock, 1, pathCluster);
//    printf("testing %s", readBlock[0].filename);
//    // check to see if path already exists
//
//    if(pathCluster == -1){
//        //tokenize path
//        char * currentlyParsing = strtok(pathname, "/");
//        while( currentlyParsing != NULL ) {
//            //see if individual paths exist
//            ppReturn = parsePath(currentlyParsing);
//            pathCluster = ppReturn->index;
//            printf("PATH NUMBER: %i \n",pathCluster);
//            printf("%s\n", currentlyParsing);
//            if(pathCluster == -1){
//                //find a block inside directory
//                printf("%s reading blocks\n",readBlock[0].filename);
//                //create new directory entry list
//                dirEntry *new_dir = malloc(sizeof(512));
//                //find a new block to write to
//                int writeLocation = linkFATBlocks(current_wd, FAT);
//                strcpy(new_dir[0].filename, currentlyParsing);
//                new_dir->starting_cluster = writeLocation;
//                printf("%d\n",writeLocation);
//
//                LBAwrite(new_dir, 1,writeLocation );
//
//                //update vcb and directory paths
//                writeFAT(FAT);
//                writeVCB(vcb);
//
//                free(new_dir);
//                printf("create new directory %s\n", currentlyParsing);
//                //set the current directory to be newly created directory
//            }else{
//                //if directory found we can set current directory to the same position
//                printf("directory %s already exists at pos %d\n", currentlyParsing, pathCluster);
//                current_wd = pathCluster;
//                LBAread(readBlock, 1,current_wd);
//                printf("%s \n", readBlock[0].filename);
//                printf("%s\n", "entering existing directory");
//
//            }
//            currentlyParsing = strtok(NULL, "/");
//
//            printf("%s \n", currentlyParsing);
//
//        }
//
//        //we have parsed and created the new directories we can now free the VCB and return the location of our file
//        free(vcb);
//
//        return current_wd;
//    }else{
//        //instance where directory or file with same name already exists
//        free(vcb);
//        return pathCluster;
//    }
//}

fdDir *fs_opendir(const char *pathname)
{
    VolumeControlBlock * vcb = getVCB();
    ppReturn* ret =  parsePath(pathname);
    int path = ret->index;
    if(path != -1){

    }else{
        return NULL;
    }
    return NULL;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{
    return NULL;
}

int fs_closedir(fdDir *dirp)
{
    return 0;
}

//
//✓

int fs_isFile(char *filename)
{

   VolumeControlBlock *vcb = malloc(512);
    LBAread(vcb, 1, 0);
    int wd = vcb->working_directory;
    dirEntry * entry = (dirEntry *) malloc(512);
    for(int i = 0; i < 16; i++){
        if(strcmp(entry[i].filename, filename)){
            if(entry[i].attribute != 4){
                free(vcb);
                return 0;
            } else{
                free(vcb);
                return -1;
            }

        }
    }

}


//check if specified file is a directory
int fs_isDir(char *pathname) {


    VolumeControlBlock *vcb = malloc(512);
    LBAread(vcb, 1, 0);
    int wd = vcb->working_directory;
    dirEntry * entry = (dirEntry *) malloc(512);
    for(int i = 0; i < 16; i++){
        if(strcmp(entry[i].filename, pathname)){
            //check if file attribute is set to directory
            if(entry[i].attribute == 4){
                free(vcb);
                return 0;
            } else{
                free(vcb);
                return -1;
            }

        }
    }
}


// Key directory functions


int fs_rmdir(const char *pathname) { 
    VolumeControlBlock *vcb = malloc(512);
	LBAread(vcb, 1, 0);

    int index = vcb->working_directory;
    dirEntry * entry = (dirEntry *) malloc(512);
    LBAread(entry, 1, index);
    int tester = -1;
    FileEntry  * fileEntry = getFAT();
    for(int i = 0; i < 16; i++){
        if(!strcmp(entry[i].filename, pathname)){
//            strcpy(entry[i].filename[0], '\0');

            entry[i].filename[0] = '\0';
            printf("\nmatch");
            tester = i;
        }
    }
    if(tester != -1){
       return 0;
    }
    return tester;


//delete a file system from memory
int fs_delete(char *filename) { 
    VolumeControlBlock *vcb = malloc(512);
	LBAread(vcb, 1, 0);
    ppReturn* ret =  parsePath(filename);
    int index = ret->index;

    //pointer for directory Entry
    dirEntry *readBlock = malloc(sizeof(dirEntry));;

    //pointer for FAT table
    FileEntry *cluster = malloc(sizeof(FileEntry));
    int pathCluster = 1;


    if (index != -1){
        LBAread(readBlock, 1, index);
        readBlock[index].filename[0] = 0xe5;
        pathCluster = readBlock[index].filesize / vcb->sectors_per_cluster;
        //free clusters to delete file
        for(int i = 0; i++; i < pathCluster){
          cluster[index].busy = 0;
               if(cluster[index].next != 0){
                     index = cluster[index].next;
                }
        }

	}
    //if the file is not found we update VCB

    LBAwrite(vcb, 1, index);
    return -1;
 } //removes a file

 //returns information from VCB
 //function is used by shell
 int  fs_stat(const char *path, struct fs_stat *buf)
{

    struct fs_stat *stat = malloc(sizeof(struct fs_stat));
    VolumeControlBlock *vcb = getVCB();
    ppReturn* ret =  parsePath(path);
    int index = ret->index;


    if(index != -1){
        if (stat == NULL)
        {
            printf("error; malloc failed ");
            return 0;
        }

        stat->st_blocks = vcb->number_of_blocks;
        stat->st_size= vcb->data_size;
        stat->st_blksize= vcb->block_size;
        stat->st_fileCount = vcb->file_count;
        stat->st_modtime = vcb->file_modification_date;
        stat->st_createtime=vcb->file_creation_date;
        stat->st_accesstime=vcb->file_access_date;

        return((int)stat);

    }
    //free VCB and set to NULL
    free(stat);
    stat=NULL;
    return 0;

}
//use to link fat blocks togeather; returns the index of location to write directory entries to
//int linkFATBlocks(int FATNum, FileEntry * FAT){
//    //search and link the files, we should always have blocks
//    while(0==0){
//        if(FAT[FATNum].busy == 1){
//            if(FAT[FATNum].next == -1){
//                int nextBlock = getNextFreeBlock(FATNum);
//                printf("next free block is %d\n", nextBlock);
//                FAT[FATNum].next = nextBlock;
//                FAT[nextBlock].busy = 1;
//                FAT[nextBlock].next = -1;
//                return nextBlock;
//            }else{
//                FATNum = FAT[FATNum].next;
//            }
//        } else{
//            return FATNum;
//        }
//    }
//}

//int writeFAT(FileEntry * FAT) {
//    VolumeControlBlock * vcb = malloc(512);
//    LBAread(vcb, 1, 0);
//    int FAT_size = ((sizeof(FileEntry) * vcb->number_of_clusters )/ vcb->block_size) + 1; //size in blocks/clusters(clusters=blocks for this test case)
//    free(vcb);
//    return LBAwrite(FAT, FAT_size, 1);
//}
//returns n-1 directory if found; else return -1
//returns n-1 directory if found; else return -1

//
//    ppReturn* ret =  parsePath(pathname);
//    int index = ret->index;
//    printf("index! %i \n", index);
//
//    dirEntry *readBlock = malloc(sizeof(dirEntry));;
//    FileEntry * FAT = getFAT();
//    int pathCluster = 1;
//
//    if (index != -1){
//        int start = readBlock[index].starting_cluster;
//        printf("filename index 0: %d \n", readBlock[start].filename[0] );
//        if(readBlock[start].filename[0] == 0 ) {
//
//
//            readBlock[start].filename[0] = '\0';
//            printf("filename index status: %s \n", readBlock[start].filename);
//            pathCluster = readBlock[start].filesize / vcb->sectors_per_cluster;
//            if(pathCluster < 1){
//                pathCluster = 1;
//            }
//
//            printf("cluster block start %i \n", start);
//            printf("cluster block status %i \n", FAT[start].busy);
//            for (int i = 0; i++; i < pathCluster) {
//                printf("cluster block status %i", FAT[start].busy);
//                FAT[start].busy = 0;
//                if (FAT[start].next != 0) {
//                    printf("cluster block %i", FAT[start].next);
//                    start = FAT[start].next;
//                }
//            }
//
//        }
//
//    }else{
//        printf("directory is not empty");
//        return -1;
//    }
//    LBAwrite(vcb, 1, readBlock[index].starting_cluster);
//    return 0;
}

// Directory iteration functions
//fdDir * fs_opendir(const char *pathname){
//};

//struct fs_diriteminfo *fs_readdir(fdDir *dirp);
//x

// Misc directory functions


//x
