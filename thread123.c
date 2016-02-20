/*
 * thr.c
 *
 *  Created on: Jan 30, 2014
 *      Author: crivera
 */


#include "stdio.h"
#include <pthread.h>

#define TRUE 1
#define FALSE 0

pthread_mutex_t g_tLock = PTHREAD_MUTEX_INITIALIZER;;
pthread_mutex_t g_tLockCVar = PTHREAD_MUTEX_INITIALIZER;;
pthread_cond_t g_Cond = PTHREAD_COND_INITIALIZER;
static int g_charNumArray[7]={0,0,0,0,0,0,0};
static int g_numChars=0;
static char g_myChar=EOF;
static char g_myCharString[7]={0,0,0,0,0,0,0};


void * showEyeC(void *p)
{
    int i=0;
    if ( p ) i = (int) p;
    printf("start loop with %d \r\n",i);
	while (TRUE)
	{
		if (i%2)
        {
			//print the last number of characters read before the animation.
			//for performance, dont include the i%2 operation in the lock
			pthread_mutex_lock(&g_tLock);
            printf("%d/ nc = %d",i,g_numChars);
        }
        else
        {
    		//print the last number of characters read before the animation
			//for performance, dont include the i%2 operation in the lock
    		pthread_mutex_lock(&g_tLock);
            printf("\\%d nc = %d",i,g_numChars);
        }
		pthread_mutex_unlock(&g_tLock);
        fflush(stdout);    //needed in ubuntu, otherwise stdout is not updated on the printf
        printf("\r      \r"); //clear the line
        i++;
	}
    return 0;
}

void * Spell(void *p)
{
	int bSearching=TRUE;
	while (bSearching)
	{
		//to print carcmg as the characters appear in the file :-)
		pthread_mutex_lock(&g_tLockCVar);
		while (g_myChar!='c' &&
			   g_myChar!='a' &&
			   g_myChar!='r' &&
			   g_myChar!='c' &&
			   g_myChar!='m' &&
			   g_myChar!='g' )
		{
			pthread_cond_wait(&g_Cond, &g_tLockCVar);
		}
		switch (g_myChar)
		{
		case 'c':
			g_myCharString[0]=g_myChar;
			g_myCharString[3]=g_myChar;
			g_charNumArray[0]=(g_numChars-1);
			g_charNumArray[3]=(g_numChars-1);
			break;
		case 'a':
			g_myCharString[1]=g_myChar;
			g_charNumArray[1]=(g_numChars-1);
			break;
		case 'r':
			g_myCharString[2]=g_myChar;
			g_charNumArray[2]=(g_numChars-1);
			break;
		case 'm':
			g_myCharString[4]=g_myChar;
			g_charNumArray[4]=(g_numChars-1);
			break;
		case 'g':
			g_myCharString[5]=g_myChar;
			g_charNumArray[5]=(g_numChars-1);
			break;
		default:
			break;
		}
		pthread_mutex_unlock(&g_tLockCVar);
		if (!strncmp(g_myCharString,"carcmg",6))
		{
			bSearching=FALSE;
		}
	}
	return 0;
}

#define BYTE_SIZE 8
#define SHIFT_START 7

int rotateBits(unsigned short int byte)
{
	int rotatedByte=0;
	int temp=0,i=0,shift_start=7;
	printf("rotate 0x%2X\n",byte);
	//bit 0
	temp = byte&0x1;
	rotatedByte |= rotatedByte|(temp<<shift_start);
	//bits 1-3
	for (i=1;i<=3;i++)
	{
		shift_start-=2;
		temp = byte&(1<<i); //shift left = 2^i (1,2; 2,4; 3,8)
		rotatedByte |= temp<<(shift_start); //1,5; 2,3; 3,1
	}
	//bits 7-4
	shift_start=7;
	for (i=7;i>=4;i--)
	{
		temp = byte&(1<<i);
		rotatedByte |= temp>>(shift_start);
		shift_start-=2;
	}
	return rotatedByte;
}

int main (int argc, char * argv[])
{
    FILE *fp=0;
    FILE *fp2=0;
    pthread_t myThreadVar;
    pthread_t myThreadVar2;

    if (argc<3)
    {
       printf("threads123 - usage : infilename outfilename 1 (to enable debug prints)\n");
       return FALSE;
    }

    printf("opening file %s\n", argv[1]);
    fp = fopen(argv[1],"r");
    printf("opening file %s\n", argv[2]);
    fp2 = fopen(argv[2],"a+");

    if (fp && fp2)
    {
       printf("success opening files %s and %s\n", argv[1],argv[2]);
       if (argv[3] != NULL)
       {
           pthread_create(&myThreadVar,NULL,showEyeC,atoi(argv[3]));
           pthread_create(&myThreadVar2,NULL,Spell,NULL);
       }
       do
       {
    	  g_myChar = fgetc(fp);
          if (g_myChar!=EOF)
          {
             fputc(g_myChar,fp2);
             if (g_myChar=='\n')
             {
                fputs("\r",fp2);
             }

     		pthread_mutex_lock(&g_tLockCVar);
     		if (g_myChar=='c' ||
       		    g_myChar=='a' ||
     		    g_myChar=='r' ||
     		    g_myChar=='c' ||
     		    g_myChar=='m' ||
     		    g_myChar=='g' )
     		{
     			pthread_cond_signal(&g_Cond);
     		}
     		pthread_mutex_unlock(&g_tLockCVar);

            pthread_mutex_lock(&g_tLock);
            g_numChars ++;
            pthread_mutex_unlock(&g_tLock);
          }
       } while (g_myChar!=EOF);

       fclose(fp);
       fclose(fp2);
    }
    else if ( !fp )
    {
       printf("failed to open file %s\n", argv[1]);
       return FALSE;
    }
    else if ( !fp2 )
    {
       printf("failed to open file %s\n", argv[2]);
       return FALSE;
    }

    if ( argv[3] != NULL )
    {
        pthread_cancel(myThreadVar);
        pthread_cancel(myThreadVar2);
        printf("\nThreads terminated\n");
    }
    printf("\r\r done with %s. numChars = %d. string = %s\n", argv[1],g_numChars,g_myCharString);
    printf("rotated byte = 0x%2X\n",rotateBits(atoi(argv[3])));
    return TRUE;
}
