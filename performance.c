#include <stdio.h>
#include<unistd.h>
#include"string.h"

#define max_size 100000

struct info{
    long int uTime;
    long int sTime;
    long int nice;
    long int idle;
    long int io_wait;
    long int irq;
    long int soft_irq;
    long int steal;
    long int guest;
};

struct percentage
{
    float uTime;
    float sTime;
    float nice;
    float idle;
    float io_wait;
    float irq;
    float soft_irq;
    float steal;
    float guest;
};

struct memInfo{
    char MemTotal[max_size];
    char MemFree [max_size];
    char MemAvailble[max_size];
};

struct diskInfo1{
    long writes;
    long reads;
    char dev_name[max_size];
    char dev_number[max_size];
};

struct diskInfo{
    long writes;
    long reads;
    char dev_name[500];
    char dev_number[500];
};



void getInfo(struct info *view,struct memInfo *mem);
long int process_cpuInfo(char *line,long int count,struct info * performance);
void process_MemInfo(char *line,long int count,struct memInfo * performance);
void nonEmpty(char *line,long int* ind);

void PrintPercentage(struct percentage performance)
{
    printf("%.2f\t%.2f\t%.2f\t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%0.2f\n",performance.uTime,performance.nice,performance.sTime,performance.idle,performance.io_wait,performance.irq,performance.soft_irq,performance.steal,performance.guest);
}

void PrintCpuInfo(struct info performance)
{
    printf("%ld %ld %ld %ld %ld %ld %ld %ld %ld\n",performance.uTime,performance.nice,performance.sTime,performance.idle,performance.io_wait,performance.irq,performance.soft_irq,performance.steal,performance.guest);
}

void PrintMemInfo(struct memInfo mem)
{
    printf("MemInfo\n%s%s%sCpuInfo\n",mem.MemTotal,mem.MemFree,mem.MemAvailble);
}

void PrintDiskInfo(struct diskInfo info)
{
    printf("%s\t\t%s\t\t%ld\t%ld\n",info.dev_number,info.dev_name,info.reads,info.writes);
    //printf("%ld\t%ld\n",info.reads,info.writes);
}

void PrintDiskInfoAll(struct diskInfo *info,long int count)
{
    printf("\n\nDiskstats\n");
    printf("Dev number\tDev Name\tReads\tWrites\n");
    for(int i=0;i<count;i++)
        PrintDiskInfo(info[i]);
}

long int ConversionInt(char *arr , long int *ind)
{
    long int i = *ind;
    long int x;
    long int value=0;
    while(1)
    {
        if(arr[i]!=' ')
        {
            x = arr[i]-48;
            value = value *10 + x;
        }
        else
            break;
        i++;
    }
    *ind = ++i;
    return value;
}


void getInfo(struct info *view,struct memInfo *mem)
{
    FILE *fptr;
    char line[max_size];
     
    fptr = fopen("/proc/stat", "r");
    if(fptr == NULL)
        return;
    long int count = 0 ;
    while(fgets(line,max_size,fptr)){
        if(line[0]=='i')
            break;
        process_cpuInfo(line,count,view);
        count++;
        //printf("%s",line);
    }
    fclose(fptr);
     
    fptr = fopen("/proc/meminfo", "r");

    int c = 0;
    if(fptr == NULL)
        return;
    while(fgets(line,max_size,fptr)){
        if(c>=3)
            break;
        process_MemInfo(line,c,mem);
        c++;
    }

    return;
}

long int process_cpuInfo(char *line,long int count,struct info * performance)
{
    long int ind=0;
    while(1)
    {
        if(line[ind]==' ')
        {
            ind++;
            break;
        }
        ind++;
    }
    if(count==0)
        ind++;
    performance[count].uTime = ConversionInt(line,&ind);
    performance[count].nice = ConversionInt(line,&ind);
    performance[count].sTime = ConversionInt(line,&ind);
    performance[count].idle = ConversionInt(line,&ind);
    performance[count].io_wait = ConversionInt(line,&ind);
    performance[count].irq = ConversionInt(line,&ind);
    performance[count].soft_irq = ConversionInt(line,&ind);
    performance[count].steal = ConversionInt(line,&ind);
    performance[count].guest = ConversionInt(line,&ind);
}

void process_MemInfo(char *line,long int count,struct memInfo * performance){
    if(count==0)
        strcpy(performance->MemTotal,line);
    else if(count==1)
        strcpy(performance->MemFree,line);
    else
        strcpy(performance->MemAvailble,line);
};

long Count_devices()
{
    FILE *fptr;
    char line[max_size];
    
    fptr = fopen("/proc/diskstats", "r");
    //fptr = fopen("trial1.txt","r");
    if(fptr == NULL)
        return 0;
    long int count = 0 ;
    while(fgets(line,max_size,fptr)){
        count++;
        //printf("%s",line);
    }
    fclose(fptr);
    //printf("%ld",count);
    return count;

}

void ConversionString(char *arr,char *line,long int *ind,long int *ind1)
{
    nonEmpty(line,ind);
    long int i = *ind;
    long int x=*ind1;
    while(1)
    {
        if(line[i]!=' ')
        {
            if(x>=max_size)
                break;
            arr[x]=line[i];
            x++;
        }
        else
            break;
        i++;
    }
    *ind = ++i;
    *ind1 =x;
}

void nonEmpty(char *line,long int* ind)
{
    long int i=*ind;
    while(1)
    {
        if(line[i]!=' ')
            break;
        i++;
    }
    *ind = i;
}

void process_diskInfo(char* line,struct diskInfo *info,long int count)
{
    long int ind = 0;
    long int x = 0;
    char dummy[max_size];
    ConversionString(info[count].dev_number,line,&ind,&x);
    info[count].dev_number[x] = '/';
    x++;
    ConversionString(info[count].dev_number,line,&ind,&x);x=0;
    ConversionString(info[count].dev_name,line,&ind,&x);x=0;
    info[count].reads = ConversionInt(line,&ind);
    ConversionString(dummy,line,&ind,&x);x=0;
    ConversionString(dummy,line,&ind,&x);x=0;
    ConversionString(dummy,line,&ind,&x);x=0;
    info[count].writes = ConversionInt(line,&ind);
    //printf("read - %ld, writes- %ld\n",info[count].reads,info[count].writes);
}


void diskInfo(struct diskInfo *info)
{
    FILE *fptr;
    char line[max_size];
    
    fptr = fopen("/proc/diskstats", "r");
    //fptr = fopen("trial1.txt","r");

    if(fptr == NULL)
        return;
    long int count = 0 ;
    while(fgets(line,max_size,fptr)){
        process_diskInfo(line,info,count);
        count++;
        //printf("%s",line);
    }
    fclose(fptr);

}

void PercentageAnalysis(struct info *before, struct info *after,long int count)
{
    struct percentage p;
    long int sum ;
    for(long int i=0;i<count;i++)
    {
        sum=0;
        p.uTime = after[i].uTime - before[i].uTime;
        p.nice = after[i].nice - before[i].nice;
        p.sTime = after[i].sTime - before[i].sTime;
        p.idle = after[i].idle - before[i].idle;
        p.io_wait = after[i].io_wait - before[i].io_wait;
        p.irq = after[i].irq - before[i].irq;
        p.soft_irq = after[i].soft_irq - before[i].soft_irq;
        p.steal = after[i].steal - before[i].steal;
        p.guest = after[i].guest - before[i].guest;
        sum = p.uTime+p.guest+p.idle+p.io_wait+p.irq+p.nice+p.soft_irq+p.steal+p.sTime;
        p.uTime = ((p.uTime*100.0)/sum);
        p.nice = ((p.nice*100.0)/sum);
        p.sTime = (p.sTime*100.0)/sum;
        p.idle = (p.idle*100.0)/sum;
        p.io_wait = (p.io_wait*100.0)/sum;
        p.irq = (p.irq*100.0)/sum;
        p.soft_irq = (p.soft_irq*100.0)/sum;
        p.steal = (p.steal*100.0)/sum;
        p.guest = (p.guest*100.0)/sum;
        if(i==0)
        {
            printf("     uTime%%\tnice%%\tsTime%%\tidle%%\tio_w%%\tirq%%\ts_irq%%\tsteal%%\tguest%%\n");
            printf("cpu  ");
        }
        else
        {
            printf("cpu%ld ",i-1);
        }
        PrintPercentage(p);
    }
}

void Analysis(struct diskInfo *before, struct diskInfo *after,struct diskInfo *analysis,long int count)
{
    //PrintDiskInfoAll(before,count);
    for(int i=0;i<count;i++)
    {
        strcpy(analysis[i].dev_number,before[i].dev_number);
        strcpy(analysis[i].dev_name,before[i].dev_name);
        analysis[i].reads = after[i].reads -  before[i].reads;
        analysis[i].writes = after[i].writes - before[i].writes;
    }
    PrintDiskInfoAll(analysis,count);
}


int main() {
    long int count = sysconf(_SC_NPROCESSORS_ONLN);
    count++;
    long int number = Count_devices();
    //printf("%ld",number);
    struct info dummy[count],after[count],before[count];
    struct diskInfo1 f;
    struct diskInfo1 fk[2];
    struct diskInfo lol;
    struct diskInfo after_interval[number],before_interval[number],analysis[number];
    struct memInfo mem;
    long int s,times;
    scanf("%ld %ld",&s,&times);
    for(long int i=0;i<times;i++)
    {
        if(i==0)
        {
            getInfo(before,&mem);
            diskInfo(before_interval);
            sleep(s);
            getInfo(after,&mem);
            diskInfo(after_interval);
        }
        else
        {
            for(long int i=0;i<count;i++)
                before[i] = after[i];
            for(long int i=0;i<number;i++)
                before_interval[i] = after_interval[i];
            sleep(s);
            getInfo(after,&mem);
            diskInfo(after_interval);
        }
        printf("\n");
        PrintMemInfo(mem);
        PercentageAnalysis(before,after,count);
        Analysis(before_interval,after_interval,analysis,number);
    }
}