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

long int Conversion(char *arr , long int *ind)
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
    performance[count].uTime = Conversion(line,&ind);
    performance[count].nice = Conversion(line,&ind);
    performance[count].sTime = Conversion(line,&ind);
    performance[count].idle = Conversion(line,&ind);
    performance[count].io_wait = Conversion(line,&ind);
    performance[count].irq = Conversion(line,&ind);
    performance[count].soft_irq = Conversion(line,&ind);
    performance[count].steal = Conversion(line,&ind);
    performance[count].guest = Conversion(line,&ind);
}

void process_MemInfo(char *line,long int count,struct memInfo * performance){
    if(count==0)
        strcpy(performance->MemTotal,line);
    else if(count==1)
        strcpy(performance->MemFree,line);
    else
        strcpy(performance->MemAvailble,line);
};

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



void PercentageAnalysis(struct info *before, struct info *after,long int count)
{
    struct percentage p;
    long int sum ;
    for(long int i=0;i<count;i++)
    {
        //printf("entered\n");
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

int main() {
    long int count = sysconf(_SC_NPROCESSORS_ONLN);
    count++;
    struct info dummy[count],after[count],before[count];
    struct memInfo mem;
    long int s,times;
    scanf("%ld %ld",&s,&times);
    for(long int i=0;i<times;i++)
    {
        if(i==0)
        {
            getInfo(before,&mem);
            sleep(s);
            getInfo(after,&mem);
        }
        else
        {
            for(long int i=0;i<count;i++)
                before[i] = after[i];
            sleep(s);
            getInfo(after,&mem);
        }
        printf("\n");
        PrintMemInfo(mem);
        PercentageAnalysis(before,after,count);
    }
}