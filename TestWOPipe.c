#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pigpio.h>
#include "rotary_encoder.h"
#define countElements 20000
#define countBuf 3
#define countBufArray 10

enum workType
{
  Ready,
  Write,
  Pause
};

int n = 0;
char out[countBufArray];
bool State_A, State_B;	
bool Mah = true;	
double Time[countElements];	
int Coord[countElements];
static short Coordinate = 0;	
static short Coordinate2 = 0;	
int count = 0;			
struct timeval start;	
struct timeval timevals[countElements];	
int stopReadFromPipe = 100000;	
char readbuffer[countBuf];	
char bufe[countBuf];	
char Channel = 'o';		
short pendPoint = 10;		
short pendOffsetNow = 0;	
short pendOffset = 10;	
enum workType typeWork = Ready;	

void timevalToDouble(){//перевод из формата timaval в double
for(int i=0;i<=count;i++)
Time[i]=(double)(timevals[i].tv_usec - start.tv_usec) / 1000000 + (double)(timevals[i].tv_sec - start.tv_sec);//вычисляем разность времени для каждого тика
}

void Clear ()
{
  for (int i = 0; i <= count; i++)
    {
      Time[i] = 0;
      Coord[i] = 0;
      timevals[i] = (struct timeval)
      {
      0};
    }
  start = (struct timeval)
  {
  0};
  count = 0;
}

void getCurrentCoordinate(){ //получить текующую координату
n=sprintf(out, "%ld\n", Coordinate);
printf("The integer is: %s\n",out);
printf("The integer is: %d\n",count);
}

void getDataFromSensor(){
if(count<1){
return;
}
timevalToDouble();//преобразование времени

n=sprintf(out, "%ld\n", count);
printf("s\n",out);
  
  
for(int i=0;i<=count;i++){
n=sprintf(out, "%f\n", Time[i]);
printf("s\n",out);

n=sprintf(out, "%ld\n", Coord[i]);
printf("s\n",out);

}
Clear();
}



void callback(int way)
{
   Coordinate2=Coordinate;
   Coordinate += way;
   switch(typeWork){
	    
   case Ready:
   if (abs(Coordinate) > pendPoint){
	    {		
	      if(Coordinate>0)
	      Channel = '+';
	      else
	      Channel = '-';
	      typeWork = Write;
	    }			
	}
   break;
   
   
   
   
   
   case Write:
	      Coord[count] = abs (Coordinate);
	      gettimeofday (&timevals[count], NULL);
	      count++;
	      switch (Channel){
	          case '+':
	          if(Coordinate2>Coordinate){
	              if(Mah==true)
	              pendOffsetNow = Coordinate - pendOffset;
	              if(pendOffsetNow>=Coordinate){
	              typeWork=Pause;
	              Mah=false;
	              }
	          }
	          else
	          {
	              Mah=true;
	          }
	          
	          case '-':
	          if(Coordinate2<Coordinate){
	              if(Mah==true)
	              pendOffsetNow = Coordinate + pendOffset;
	              if(pendOffsetNow<=Coordinate){
	              typeWork=Pause;
	              Mah=false;
	              }
	          }
	          else
	          {
	              Mah=true;
	          }
	      }
	      	  
   
   
   break;
   
   case Pause:
   break;
   
   
   default:
   break;

}
}


int main()
{
  _Renc_t * renc;
  if (gpioInitialise() < 0) return 1;
  renc = Pi_Renc(17, 27, callback);
  readbuffer[0] = '0';
while(1) {

 
   printf( "Enter a value :");
   fgets(readbuffer,sizeof(readbuffer),stdin);

   printf( "\nYou entered: ");
   puts(readbuffer);

if(readbuffer[0]== 'E')exit(0);//Выход
if(readbuffer[0]== 'N'){//получение текущей координаты
getCurrentCoordinate();
}
if(readbuffer[0]== 'W'){//Запись в буфер маха
Clear();
gettimeofday(&start, NULL);
typeWork=Ready;
}
if(readbuffer[0]== 'M'){//Отдать данные в систему
typeWork=Pause;
getDataFromSensor();
typeWork=Ready;
}
if(readbuffer[0]== 'C'){//Сброс
Clear();
Coordinate=0;
Channel='o';
pendPoint=0;
offsetPointMax=0;
}

usleep(stopReadFromPipe);// сон после выполнения операция на пол сек

}
}
