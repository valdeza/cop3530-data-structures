/**
 * testcase_autogen.cpp
 * Generates test cases for use on that thing due Friday (29 Jan)!
 * Initial author: C. Martin
 *
 */
 
//TODO
// 1. Comment code and figure out workflow.
// 1.5. Write a README.md. Don't forget to mention that we make no guarantees that this autogen will generate test cases that can be processed in this century.
// 1.8. Perform naive benchmarking to estimate when the worst test case *might* finish.
// 2. Restructure as necessary.
// 3. Change file output to console output
//  (This way, if people want to make a file, they can just redirect output to their own specified file.)
// 4. Implement program arguments to allow user to specify maximum end list.

#include<unistd.h>
#include<fstream>
#include<stdlib.h>
#include<iostream>
int main(int, char**){
srand ( time(NULL) );
std::ofstream myfile;
  myfile.open("input.txt");
int size = 0;
while(size<=0){
size = rand()% 100;
}
myfile<<size<<std::endl;

int *list=new int[size];

 
for(int i = 0 ; i < size;i++){
if(i+1<size){
list[i]= rand()%200;
myfile<<list[i]<<" ";

}
else
{
list[i]= rand()%200;
myfile<<list[i];
}
}

int addi=0;
while(addi <= 0){
addi = rand()%20;
}
myfile<<std::endl<<addi;
int *list1=new int[addi];

int step =0;
while (step <=0)
{
step= rand()%4;

}
//myfile<<step<<std::endl;
int sizeaddi=0;
while(sizeaddi <= 0 &&(step * addi)<size ){
sizeaddi = rand()%19;
}
//myfile<<sizeaddi<<std::endl;
for(int i = 0; i<addi; i++){
myfile<<std::endl<<step<<std::endl;


myfile<<sizeaddi<<std::endl;
for(int j = 0 ; j < sizeaddi;j++){
if(j+1<sizeaddi ){
list1[j]=rand()%200;
myfile<<list1[j]<<" ";
} else
{
list1[j]=rand()%200;
myfile<<list1[j];

}
}


}
myfile<<std::endl;
myfile.close();
}
