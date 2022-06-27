#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

#define Max_Size 20 //max core
#define  MAX_LEN 300 // max character in for a line

FILE* FlPtr;

typedef struct{

    size_t Total_Cores[2]; //number of cores existed on the system
    char Contents[2][Max_Size][MAX_LEN]; // the content of every cpu 

} Collection ;

Collection* Col;
void* ReaderFun(void* value)
{
    
    Col =(Collection*) malloc(sizeof(Collection));
    int counter;
   
   for(int i=0;i<2;i++)
   {
   
    counter = 0;
    FlPtr  = fopen("/proc/stat","r");
    
    if (!FlPtr) //assert -  Program exits if the file pointer returns NULL.
        {exit(EXIT_FAILURE);}
    Col->Total_Cores[i] = 0;
    while (!feof(FlPtr) && !ferror(FlPtr))
       
        if (fgets(Col->Contents[i][counter],MAX_LEN, FlPtr) != NULL)
        { 
            if (strstr(Col->Contents[i][counter],"intr"))
               break;
            
            counter++;
       }
    Col->Total_Cores[i] = --counter; // all the core plus one for the total one 
     sleep(1);
    
    }
    
   
    sleep(1);
}

int main()
{
    pthread_t TrdRead;
    if( 0 == pthread_create(&TrdRead,NULL,ReaderFun,NULL))
        {

        }
    
    if(0 == pthread_join(TrdRead,NULL))
        {

        }

    free(Col);
    exit(EXIT_SUCCESS);
}