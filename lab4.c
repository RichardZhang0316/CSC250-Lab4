/**
 * This program reads a WAV audio file and prints statistics about the audio samples. The file name 
 * is provided using command line arguments. If the file name is not provided or the file is not readable, 
 * the program will exit and provide an error message.
 *
 * @author Richard Zhang {zhank20@wfu.edu}
 * @date Sep. 29, 2022
 * @assignment Lab 4  
 * @course CSC 250
 * 活在这珍贵的人间 Living in this precious world
 * 人类和植物一样幸福 Humans are as happy as plants
 * 爱情和雨水一样幸福 Love is as happy as rain
 * -- 海子 written in 1985 by Chinese peot Haizi
 **/

#include <math.h>  
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>  


int readWavHeader(FILE* inFile, short *sampleSizePtr, int *numSamplesPtr, int *sampleRatePtr, short *numChannelsPtr);
int readWavData(FILE* inFile, short sampleSize, int numSamples, int sampleRate, short numChannels);

/* the main function first checks whether the command line entered meets the requirments and then checks if 
* the WAV file exist or not. Then, it first reads and prints the RIFF and format chunks and also reads and
* prints the start of the data chunk by calling the readWavHeader() function. Then, it prints the rest if info
* by calling readWavData() function, where we also do some calculation there to produce some data about the samples.
*/
int main(int argc, char *argv[]) {
    FILE *inFile;      /* WAV file */
    short sampleSize = 0;  /* size of an audio sample (bits) */
    int sampleRate = 0;    /* sample rate (samples/second) */
    int numSamples = 0;    /* number of audio samples */ 
    int wavOK = 0;     /* 1 if the WAV file si ok, 0 otherwise */
    short numChannels = 0; /* number of channels */

    if(argc < 2) {
        printf("usage: %s wav_file \n", argv[0]);
        return 1;
    }

    inFile = fopen(argv[1], "rbe"); 
    if(!inFile) {
        printf("could not open wav file %s \n", argv[1]);
        return 2;
    }

    wavOK = readWavHeader(inFile, &sampleSize, &numSamples, &sampleRate, &numChannels);
    if(!wavOK) {
       printf("wav file %s has incompatible format \n", argv[1]);   
       return 3;
    }
    else {
        readWavData(inFile, sampleSize, numSamples, sampleRate, numChannels);
    }
    if(inFile) {
        fclose(inFile);
    }
    return 0;
}


/**
 *  function reads the RIFF, fmt, and start of the data chunk. 
 */
int readWavHeader(FILE* inFile, short *sampleSizePtr, int *numSamplesPtr, int *sampleRatePtr, short *numChannelsPtr) {
    char chunkId[] = "    ";  /* chunk id, note initialize as a C-string */
    char data[] = "    ";      /* chunk data */
    int chunkSize = 0;        /* number of bytes remaining in chunk */
    short audioFormat = 0;    /* audio format type, PCM = 1 */
    short numChannels = 0;    /* number of audio channels */ 
    int sampleRate = 0;       /* Audio samples per second */
    short bitsPerSample = 0; /* Number of bits used for an audio sample. */
    int audioSamples = 0; /* This is the number of bytes of data (audio samples) */

    /* first chunk is the RIFF chunk, let's read that info */  
    fread(chunkId, 1, 4, inFile);
    fread(&chunkSize, 1, 4, inFile);
    printf("chunk: %s \n", chunkId);
    fread(data, 1, 4, inFile);
    printf("  data: %s \n", data);

    /* let's try to read the next chunk, it always starts with an id */
    fread(chunkId, 1, 4, inFile);
    /* if the next chunk is not "fmt " then let's skip over it */  
    while(strcmp(chunkId, "fmt ") != 0) {
        fread(&chunkSize, 1, 4, inFile);
        /* skip to the end of this chunk */  
        fseek(inFile, chunkSize, SEEK_CUR);
        /* read the id of the next chuck */  
        fread(chunkId, 1, 4, inFile);
    }  

    /* if we are here, then we must have the fmt chunk, now read that data */  
    fread(&chunkSize, 1, 4, inFile);
    fread(&audioFormat, 1,  sizeof(audioFormat), inFile);
    fread(&numChannels, 1,  sizeof(numChannels), inFile);
    /* you'll need more reads here, hear? */ 
    fread(&sampleRate, 1,  sizeof(sampleRate), inFile);
    /* skip byte rate and block align to get to position of bits per sample*/
    fseek(inFile, 4, SEEK_CUR);
    fseek(inFile, 2, SEEK_CUR);
    fread(&bitsPerSample, 1, sizeof(bitsPerSample), inFile); 

    /* print the information we got in the fmt chunk */
    printf("chunk: %s \n", chunkId);
    printf(" audio format: %d \n", audioFormat);
    printf(" num channels: %d \n", numChannels);
    printf(" sample rate: %d \n", sampleRate);
    printf(" bits per sample: %d \n", bitsPerSample);

    /* read the data chunk next, use another while loop (like above) */
    /* visit http://goo.gl/rxnHB1 for helpful advice */
    /* let's try to read the data chunk to get the date chunk id and data size (num samples) */
    fread(chunkId, 1, 4, inFile);
    /* if the next chunk is not "data" then let's skip over it */  
    while(strcmp(chunkId, "data") != 0) {
        fread(&chunkSize, 1, 4, inFile);
        /* skip to the end of this chunk */  
        fseek(inFile, chunkSize, SEEK_CUR);
        /* read the id of the next chuck */  
        fread(chunkId, 1, 4, inFile);
    }

    /* if we are here, then we must have the fmt chunk, now read that data */  
    fread(&chunkSize, 1, 4, inFile);
    fread(&audioSamples, 1,  sizeof(audioSamples), inFile);
    /* print the information we got in the data chunk */
    printf("chunk: %s \n", chunkId);

    *sampleSizePtr = bitsPerSample;
    if (numChannels == 2){
        *numSamplesPtr = (chunkSize * 8 / bitsPerSample) / 2;
        printf(" num samples: %d \n", *numSamplesPtr);
    }
    else{
        *numSamplesPtr = chunkSize * 8 / bitsPerSample;
        printf(" num samples: %d \n", *numSamplesPtr);
    }
    *sampleRatePtr = sampleRate;
    *numChannelsPtr = numChannels;
    return (audioFormat == 1);
}


/**
 *  function reads the WAV audio data (last part of the data chunk)
 */
int readWavData(FILE* inFile, short sampleSize, int numSamples, int sampleRate, short numChannels) {
    float duration = 0.0;
    int i = 0;
    int current = 0;
    int currentLeft = 0;
    int currentRight = 0;
    int maxMono = 0;
    int maxLeft = 0;
    int maxRight = 0;

    /* calculate and print duration */
    duration = (float) numSamples / sampleRate;
    printf(" duration: %f (sec) \n", duration);

    /* if the WAV file only has one channel, then we only need to print the max abs mono sample */
    if (numChannels == 1) {
        for (i = 0; i < numSamples; i++){
            fread(&current, 1, (sampleSize/8), inFile);
            /* the size of the sample will depend on how the audio was encoded (typically, 1, 2, or 4 bytes) */
            if (sampleSize / 8 == 4){
                current = abs((int) current);
            }
            else if (sampleSize / 8 == 2){
                current = abs((short) current);
            }
            if (maxMono < current){
                maxMono = current;
            }
        }
        printf(" max abs mono sample: %d \n", maxMono);
    }
    else {
         /* if the WAV file has two channels, then we  need to print the max abs left sample and max abs right sample */
        for (i = 0; i < numSamples; i++){
            fread(&currentLeft, 1, (sampleSize/8), inFile);
            fread(&currentRight, 1, (sampleSize/8), inFile);
            /* compute the absolute values of the samples based on how many bytes a sample is encoded in */
            if (sampleSize / 8 == 4){
                currentLeft = abs((int) currentLeft);
                currentRight = abs((int) currentRight);
            }
            else if (sampleSize / 8 == 2){
                currentLeft = abs((short) currentLeft);
                currentRight = abs((short) currentRight);
            }
            if (maxLeft < currentLeft){
                maxLeft = currentLeft;
            }
            if (maxRight < currentRight){
                maxRight = currentRight;
            }
        }
        printf(" max abs left sample: %d \n", maxLeft);
        printf(" max abs right sample: %d \n", maxRight);
    }
    return 1;
}



