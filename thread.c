#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

#define Max_Size 20 //max core
#define  MAX_LEN 300 // max character in for a line
// PrevIdle = previdle + previowait
// Idle = idle + iowait

// PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
// NonIdle = user + nice + system + irq + softirq + steal

// PrevTotal = PrevIdle + PrevNonIdle
// Total = Idle + NonIdle

// # differentiate: actual value minus the previous one
// totald = Total - PrevTotal
// idled = Idle - PrevIdle

// CPU_Percentage = (totald - idled)/totald


FILE* FlPtr;
pthread_mutex_t LockVar;

int user[2], nices[2] ,systems[2], idel[2], iowait[2], irq[2] ,softirq[2],  steal[2];
int Idel[2],NonIdle[2], Total[2];
size_t CPU_Persentage;
typedef struct{
    size_t *Core_Usage;
} Results;

typedef struct{

    size_t Total_Cores[2]; //number of cores existed on the system
    char Contents[2][Max_Size][MAX_LEN];

} Collection ;

Collection* Col; 
Results *Rs;
size_t persentage_calculate()
{
    

     for(int i=0;i<2;i++)
   {
    NonIdle[i] = user[i] + nices[i] + systems[i] + irq[i] + softirq[i] + steal[i];
    Idel[i] = idel[i], iowait[i];
   }
   CPU_Persentage = ((Idel[1] + NonIdle[1] -(Idel[0] + NonIdle[0])) -(Idel[1]- Idel[0]))/((Idel[1] + NonIdle[1] -(Idel[0] + NonIdle[0])));
    
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
void* PrinterFun(void* passvalue)
{
  Collection *Colp;
  Results* RSp;
  RSp = Rs;
  Colp = Col;
  int i =0;
  while(i <= Colp->Total_Cores[0])
  {
    
  }
}

void* AnalyserFun(void*value)
{
    sleep(4);
    
    Collection* Cols;
    char* ptr, delim[] = " ";
    Cols = Col;
    char* Contents[2];
    int count = 0, size = 0;
    while(count<=Cols->Total_Cores[0])
    {
        
    for (int j=0; j<2;j++)
    {
        size =0;
        ptr =  strtok(Cols->Contents[j][count], delim);
        while (ptr != NULL)
	{
        if(size >0)
	    	Assignvalues(j, size, strtol(ptr,NULL, 10));
		//printf("'%s'\n", ptr);
		ptr = strtok(NULL, delim);
        size++;
	}
    }
    persentage_calculate();
    Rs->Core_Usage = &CPU_Persentage;
    Rs->Core_Usage++;
    count++;
    }
   for(int i=0;i<2;i++)

    printf("******** %s\n",Cols->Contents[i][4]);
    
    
}
void* ReaderFun(void* value)
{
    
    Col =(Collection*) malloc(sizeof(Collection));
   pthread_mutex_lock(&LockVar);
   for(int i=0;i<2;i++)
   {
    // pthread_mutex_lock(&LockVar);
    int counter = 0;
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
    Col->Total_Cores[i] = --counter;
    fclose(FlPtr); 
     sleep(1);
    printf(" %ld\n",Col->Total_Cores[i]);
    }
    
    pthread_mutex_destroy(&LockVar);
    sleep(1);
}

int main()
{   
        
    pthread_t TrdRead,TrdAnalaysis;
    
    pthread_mutex_init(&LockVar,NULL);
    pthread_create(&TrdRead,NULL,ReaderFun,NULL);
    pthread_create(&TrdAnalaysis,NULL,AnalyserFun,NULL);
   
    pthread_join(TrdRead,NULL);
    pthread_join(TrdAnalaysis,NULL);
    
    pthread_mutex_destroy(&LockVar);
    
    free(Col);
    exit(EXIT_SUCCESS);

    
}
