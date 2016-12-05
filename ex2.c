#include <stdio.h>
#include <memory.h>
#include <stdlib.h>


/**
 * Haim_Rubinstein
 * 203405386
 * ex2
 */


//declare the function's
int checkLittle(char* buffer);
void copyFile(FILE* des,FILE* src);
int copyToNewEncoding(FILE* des,FILE* src, char* oldEncoding, char* newEncoding);
unsigned int chengeFromUnix( char* newEncoding, char* buffer, int isLitte);
unsigned int chengeFromWin(FILE* src, char* newEncoding, char* buffer, int isLitte);
unsigned int chengeFromMac( char* newEncoding, char* buffer, int isLitte);
void chengeByteOrder(FILE* des,FILE* src, char* oldEncode,char* newEncoding, char* flag);


/**
 * the main function
 * runs the program
 *
 * @param argc - num of arguments to the main
 * @param argv - the arguments to the main
 *
 */
int main(int argc,char* argv[]) {

    //creating new files according to the given path to the main
    FILE* desFile =fopen(argv[2],"wb");
    FILE* srcFile = fopen(argv[1],"rb");
    //checking if the file openning filed
    if((desFile==NULL)||(srcFile==NULL)){
        return 0;
    }

    //checking the number of flags and acting accordingly
    switch(argc) {
        //only copy the file
        case 3:
            copyFile(desFile,srcFile);
            break;
        //copy to another encoding
        case 5:
            copyToNewEncoding(desFile,srcFile,argv[3],argv[4]);
            break;
        //copy to another encoding and switching bytes
        case 6:
            chengeByteOrder(desFile,srcFile,argv[3],argv[4],argv[5]);
            break;
        default:
            break;
    }

    //close the files
    if(desFile != NULL){
        fclose(desFile);
    }
    if(srcFile != NULL){ //todo check why it's always true
        fclose(srcFile);
    }

    //end of main func
    return 0;
}

/**
 *a function that check on wich machine we are
 * working on
 * big or little endian
 *
 * @return 1 if little 0 if big
 */
int checkLittle(char* buffer){

    //recives an array containing the first charactar of
    //a utf-16 file and check if it's created on a big or littte endian
    //machine according to the fffe/feff charactar
    char c = *buffer;
    if(c == (char)0xff){
        return 1;
    }
    return 0;
}

/**
 * taking one file and copy it to a new file
 * as is. without changing anything in the way
 *
 * @param des - the destnation file
 * @param src - the source file
 */
void copyFile(FILE* des,FILE* src){

    //declare the variables
    long int numWriten;
    //define a array to read into
    char buffer[2] ;//todo do i have to use hex????
    //reading the next 2 chars from the src file
    unsigned long int numRead = fread(buffer, sizeof(char), 2, src);;

    //a loop that runs until the file ends and there in nothing
    //left to read
    while (numRead == 2) {

        //writing the charactar to the dest file
        numWriten = fwrite(buffer, sizeof(char), numRead, des);
        //reading the next 2 chars from the src file
        numRead = fread(buffer, sizeof(char), 2, src);
    }
}

/**
 * getting 2 files,source and detination, the first one(source) in one encoding
 * and the second one(destination) currently empty.
 * then copy the src to the des file with a new encodind chosen by the user
 * and given as a flag.
 * @param des - destination file
 * @param src - source file
 * @param oldEncoding - old encoding
 * @param newEncoding - new encoding
 * @return //todo check !! to return error?
 */
int copyToNewEncoding(FILE* des,FILE* src, char* oldEncoding, char* newEncoding){
    //checking from witch system to witch other system to replace

    //if the lod and new encoding are equals then just copy
    if(strcmp(oldEncoding,newEncoding)==0){
        copyFile(des,src);
    }

    //define a array to read into
    char* buffer = (char*) malloc(4*sizeof(char));
    //checking if the allocation succeed
    if(buffer == NULL){
        return -1;
    }

    //reading to first chars
    long int numRead = fread(buffer, sizeof(char),2,src);
    //checking on wich machine the file was created
    int isLittle = 0;
    if(numRead != 0){
        isLittle = checkLittle(buffer);
    }

    unsigned int numWriten = 2;
    //a loop that runs until the file ends and there in nothing
    //left to read
    while (numRead == 2){
        //checking if the chars are the new line chars
        //if they are then change them to the new encoding new line char
        //changing from windows encoding
        if(strcmp(oldEncoding,"-win")==0){
            numWriten = chengeFromWin(src,newEncoding,buffer,isLittle);
        }
            //changing from unix encoding
        else if(strcmp(oldEncoding,"-unix")==0){
            numWriten = chengeFromUnix(newEncoding,buffer,isLittle);
        }
            //changing from mac encoding
        else if(strcmp(oldEncoding,"-mac")==0){
            numWriten = chengeFromMac(newEncoding,buffer,isLittle);
        }
        //write the chars to the dest file
        fwrite(buffer, sizeof(char),numWriten,des);
        //read the next chars
        numRead = fread(buffer, sizeof(char),2,src);
    }
    //free the allocated buffer
    free(buffer);
    //end of function
    return 0;  //todo return -1 it error accrued
}

//todo do it is ok to use malloc???

/**
 *a function that recives a char array(buffer) and that olds a
 * charactar in utf-16 and check if it's fitting the new
 * line character in unix system.
 * if it is a match then change it(the buffer array) to the new system new line
 * charactar according to the flag.and return it
 * @param newEncoding - the flag to wich system to change
 * @param buffer - the array that olds the charactar
 * @return - the number of bytes to write in the des file,2 for mac,4 for win.the charactars was already
 *            changed in the buffer.
 */
unsigned int chengeFromUnix(char* newEncoding, char* buffer,int isLittle){
    //declare the variables
    int i =0x0;
    unsigned int toWrite = 2;
    //checking if the charactar in the buffer is a mach to
    //the unix new line charactar
    if((isLittle == 1)&&((buffer[i] == 0x0a)&&(buffer[i+1])==0x00)||
            ((isLittle == 0)&&((buffer[i+1] == 0x0a)))&&(buffer[i]=0x00)) {
        //if the new encoding system is for a mac system
        if (strcmp(newEncoding, "-mac") == 0) {
            //the file is from little endian machine
            if (isLittle == 1) { // todo stay like this or change the if to if(checkEndian() )
                buffer[i] = 0x0d;
                //the file is from big endian machine
            } else {
                buffer[i + 1] = 0x0d;
            }
            //if the new encoding system is for a win system
        } else {
            //if the new system is a win system then the new kine charactar is 2 charactars
            //means 4 bytes in utf-16
            toWrite = 4;
            //the file is from little endian machine
            if (isLittle == 1) {
                buffer[i] = 0x0d;
                buffer[i+1] = 0x0;
                buffer[i+2] = 0x0a;
                buffer[i+3] =0x0;
                //the file is from big endian machine
            } else {
                buffer[i] = 0x0;
                buffer[i+1] = 0x0d;
                buffer[i+2] =0x0;
                buffer[i+3] = 0x0a;
            }
        }
    }
    return toWrite;
}

/**
 *a function that recives a char array(buffer) and that olds a
 * charactar in utf-16 and check if it's fitting the new
 * line character in mac system.
 * if it is a match then change it(the buffer array) to the new system new line
 * charactar according to the flag.and return it
 * @param newEncoding - the flag to wich system to change
 * @param buffer - the array that olds the charactar
 * @return -  the number of bytes to write in the des file,2 for unix,4 for win.the charactars was already
 *            changed in the buffer.
 */
unsigned int chengeFromMac(char* newEncoding, char* buffer,int isLittle){

    //declare the variables
    int i=0x0;
    unsigned int toWrite = 2;
    //checking if the charactar in the buffer is a mach to
    //the mac new line charactar
    if((isLittle == 1)&&((buffer[i] == 0x0d)&&(buffer[i+1] == 0x00))
       ||((isLittle == 0)&&((buffer[i+1] == 0x0d))&&(buffer[i] == 0x00))) { //todo what happend if a i am on little but getting a file that was created on big , the checkEndian()wont work
        //if the new encoding system is for a unix system
        if (strcmp(newEncoding, "-unix") == 0) {
            //the file is from little endian machine
            if (isLittle == 1) {
                buffer[i] = 0x0a;
                //the file is from big endian machine
            } else {
                buffer[i + 1] = 0x0a;
            }
            //if the new encoding system is for a win system
        } else {
            //if the new system is a win system then the new kine charactar is 2 charactars
            //means 4 bytes in utf-16
            toWrite = 4;
            //the file is from little endian machine
            if (isLittle == 1) {
                buffer[i] = 0x0d;
                buffer[i+1] = 0x0;
                buffer[i+2] = 0x0a;
                buffer[i+3] =0x0;
                //the file is from big endian machine
            } else {
                buffer[i] = 0x0;
                buffer[i+1] = 0x0d;
                buffer[i+2] =0x0;
                buffer[i+3] = 0x0a;
            }
        }
    }
    return toWrite;
}

/**
 *a function that recives a char array(buffer) and that olds a
 * charactar in utf-16 and check if it's fitting the new
 * line character in win system.
 * if it is a match then change it(the buffer array) to the new system new line
 * charactar according to the flag.and return it
 * @param newEncoding - the flag to wich system to change
 * @param buffer - the array that olds the charactar
 * @return -  the number of bytes to write in the des file,2 for unix and mac,4 for win.the charactars was already
 *            changed in the buffer.
 */
unsigned int chengeFromWin(FILE* src,char* newEncoding, char* buffer,int isLittle){

    //declare the variables
    int i =0x0;
    unsigned int toWrite= 2;
    char buff[2];

    //checking if the charactar in the buffer is a mach to
    //the win new line charactar
    if ((isLittle == 1)&&(((buffer[i]==0x0d)&&(buffer[i+1]==0x0)))
        ||((isLittle == 0)&&((buffer[i]==0x0)&&(buffer[i+1]==0x0d)))) {  //todo in this order? +do i need a flag if i already// switched?
        //the first charactar was /n now checking for /r
        //reading the next 2 bytes
        fread(buff, sizeof(char),2,src);
        if((isLittle == 1)&&(((buff[i]==0x0a)&&(buff[i+1]==0x0)))||
                ((isLittle == 0)&&(((buff[i]==0x0)&&(buff[i+1]==0x0a))))) {
            //if the new encoding system is for a unix system
            if (strcmp(newEncoding, "-unix") == 0) {
                //the file is from little endian machine
                if (isLittle == 1) {
                    buffer[i] = 0x0a;
                    buffer[i + 1] = 0x0;
                    //the file is from big endian machine
                } else {
                    buffer[i + 1] = 0x0a;
                    buffer[i] = 0x0;
                }
                //if the new encoding system is for a mac system
            } else {
                //the file is from little endian machine
                if (isLittle == 1) {
                    buffer[i] = 0x0d;
                    buffer[i + 1] = 0x0;
                    //the file is from big endian machine
                } else {
                    buffer[i + 1] = 0x0d;
                    buffer[i] = 0x0;
                }
            }
            //there was no mach in the second charactar,then copy the next  2 bytes
            //to the buffer
        } else{
            buffer[i+2] = buff[i];
            buffer[i+3] = buff[i+1];
            toWrite +=2;
        }

    }
    return toWrite;
}


/**
 * getting 2 files,source and detination, the first one(source) in one encoding
 * and the second one(destination) currently empty.
 * then copy the src to the des file with a new encodind chosen by the user
 * and given as a flag.
 * the user is also passing a flag that says if to change every 2 bytes
 * (from big to little endien to the other way)
 * @param des - the destination file
 * @param src -  the source file
 * @param oldEncode - the old encoding
 * @param newEncoding -the old encoding
 * @param flag - the flag if to change bytes
 * //todo how to return error?
 */
void chengeByteOrder(FILE* des,FILE* src, char* oldEncode,char* newEncoding, char* flag){

    //checking the swap/keep flag

    //if it's swap flag
    if(strcmp(flag, "-swap")==0){
        //allocate array to write into
        char* buffer = (char*) malloc(2*sizeof(char));
        //check if the allocation succeeded
        if(buffer == NULL){
            return;
        }

        //read the first 2 bytes
        unsigned long int numRead  = fread(buffer, sizeof(char),2,src);
        int i =0x0;

        int isLittle;
        if(numRead != 0){
            isLittle = checkLittle(buffer);
        }
        //a loop that runs until the file ends and there in nothing
        //left to read
        while (numRead == 2){
            unsigned long int toWrite = 2;
            //the old encoding is win system
            if((strcmp(oldEncode,"-win")==0)&&(strcmp(oldEncode,newEncoding)!=0)){
                toWrite = chengeFromWin(src,newEncoding,buffer,isLittle);
                //the old encoding is mac system
            }else if((strcmp(oldEncode,"-mac")==0)&&(strcmp(oldEncode,newEncoding)!=0)){
                toWrite = chengeFromMac(newEncoding,buffer,isLittle);
                //the old encoding is unix system
            }else if((strcmp(oldEncode,"-unix")==0)&&(strcmp(oldEncode,newEncoding)!=0)){
               toWrite = chengeFromUnix(newEncoding,buffer,isLittle);
            }

            //change every 2 bytes in the buffer
            char tempChar = buffer[i];
            buffer[i] = buffer[i+1];
            buffer[i+1] = tempChar;
            if(toWrite == 4){
                tempChar = buffer[i+2];
                buffer[i+2] = buffer[i+3];
                buffer[i+3] = tempChar;
            }
            //and write the buffer into the des file
            fwrite(buffer, sizeof(char),toWrite,des);
            //read the next 2 bytes
            numRead =  fread(buffer, sizeof(char),2,src);

        }
        //free the buffer
        free(buffer);
    // if it's keep flag
    }else if(strcmp(flag, "-keep")==0){
        copyToNewEncoding(des,src,oldEncode,newEncoding);
    }
}

