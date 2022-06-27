#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

#define Max_Size 20 //max core
#define  MAX_LEN 300 // max character in for a line

FILE* FlPtr;
pthread_mutex_t Lock_Collection;
pthread_cond_t Cond_Collection;
double buffer;
int user[2], nices[2] ,systems[2], idel[2], iowait[2], irq[2] ,softirq[2],  steal[2];
int Idel[2],NonIdle[2], Total[2];


typedef struct{

    size_t Total_Cores[2]; //number of cores existed on the system
    char Contents[2][Max_Size][MAX_LEN]; // the content of every cpu 
    double *Core_Usage; // array of core persentage

} Collection ;

Collection* Col;


double persentage_calculate()
{
    double CPU_Persentage;
    for(int i=0;i<2;i++)
    {
        NonIdle[i] = user[i] + nices[i] + systems[i] + irq[i] + softirq[i] + steal[i];
        Idel[i] = idel[i], iowait[i];
    }
    CPU_Persentage =(double) ((Idel[1] + NonIdle[1] -(Idel[0] + NonIdle[0])) -(Idel[1]- Idel[0]))/((Idel[1] + NonIdle[1] -(Idel[0] + NonIdle[0])));
    return CPU_Persentage;
}
void Assignvalues(int seq, int index, int value)
{
    switch(index)
    {
        
        case 1:
            user[seq] = value;
            break;
        
        case 2:
            nices[seq] = value;
            break;

        case 3:
            systems[seq] = value;
            break;
        case 4:
            idel[seq] = value;
            break;
        case 5:
            iowait[seq] = value;
            break;

        case 6:
            irq[seq] = value;
            break;

        case 7:
            softirq[seq] = value;
            break;
        
        case 8:
            steal[seq] = value;
            break;
        
    }
}

void* PrinterFun(void* passvalue) // the print thread 
{
  Collection *Colp;
  Colp = Col;
  sleep(2);
pthread_mutex_lock(&Lock_Collection); 
    
 pthread_cond_signal(&Cond_Collection); 
  int i =0;
  while(i <= Colp->Total_Cores[0])
  {
    if(i == 0)
        printf("Total CPU :%.3f\n",*Colp->Core_Usage*100);
    else
        printf("CPU_Core_%d : %.3f\n",i,*++Colp->Core_Usage*100);
   i++;
  }
pthread_mutex_unlock(&Lock_Collection); 

}

void* AnalyserFun(void* arg) // the Analyser thread 
{
    sleep(1);
    
    Collection* Cols;
    char* ptr, delim[] = " ";
    int count = 0, att = 0;

    Cols = Col;

    pthread_mutex_lock(&Lock_Collection); 
    
    pthread_cond_signal(&Cond_Collection); 
    while(count<=Cols->Total_Cores[0])
    {
        att = 0;
        for(int i=0;i<2;i++)
        {      
              att = 0;
              ptr =  strtok(Cols->Contents[i][count], delim);
                //convert array pointers to string 
            while (ptr != NULL)
	        { 
                
                if(att > 0)
	    	        Assignvalues(i, att, strtol(ptr,NULL, 16));
		        ptr = strtok(NULL, delim);
                att++;
	        }                
       }
    buffer =persentage_calculate();
    Cols->Core_Usage = & buffer;
    
    count++;
    Cols->Core_Usage+count;
    }
     pthread_mutex_unlock(&Lock_Collection); 
    
}

void* ReaderFun(void* value) // the Read thread
{
    
    Col =(Collection*) malloc(sizeof(Collection));
   
   pthread_mutex_lock(&Lock_Collection);  
   for(int i=0;i<2;i++)
   {
   
    int counter = 0;
    FlPtr  = fopen("/proc/stat","r");
    if (!FlPtr) //assert -  Program exits if the file pointer returns NULL.
        {exit(EXIT_FAILURE);}

    while (!feof(FlPtr) && !ferror(FlPtr))
       
        if (fgets(Col->Contents[i][counter],MAX_LEN, FlPtr) != NULL)
        { 
            if (strstr(Col->Contents[i][counter],"intr"))
               break;
            
            counter++;
       }
    Col->Total_Cores[i] = --counter; // all the core plus one for the total one 
    fclose(FlPtr);
    
    sleep(1);
    }
    
    pthread_cond_wait(&Cond_Collection, &Lock_Collection);
    pthread_mutex_unlock(&Lock_Collection); 

}

int main()
{
   
    pthread_t TrdRead, TrdAnalaysis,TrdPrint;
    pthread_mutex_init(&Lock_Collection,NULL);
    pthread_cond_init(&Cond_Collection,NULL);
    //pthread_cond_init(&Cond_Collection2,NULL);
    if( 0 == pthread_create(&TrdRead,NULL,ReaderFun,NULL))
             
            if( 0 == pthread_create(&TrdAnalaysis,NULL,AnalyserFun,NULL))
            
                if( 0 == pthread_create(&TrdPrint,NULL,PrinterFun,NULL))
                {}
              
    
    if(0 == pthread_join(TrdRead,NULL))
        
         if(0 == pthread_join(TrdAnalaysis,NULL))
            if(0 == pthread_join(TrdPrint,NULL))
            {}
             
        
    pthread_mutex_destroy(&Lock_Collection);
    pthread_cond_destroy(&Cond_Collection);
    //pthread_cond_init(&Cond_Collection2,NULL);
    free(Col);
    exit(EXIT_SUCCESS);
}