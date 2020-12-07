#include <stdio.h>

int main()
{

  int m1[1000][1000]; 
  int m2[1000][1000]; 
  int i,j;
  FILE* myfile;

  // initialize to make it sparse
  for (i = 0; i < 1000; i++)
  {
    for (j=0;j<1000;j++){
    m1[i] = ((double) rand() / (RAND_MAX));
    m2[i] = ((double) rand() / (RAND_MAX));
    }
  }   
  /// above
  double sum=0;
  for (i = 0; i< 1000; i++){
    //column j
    for (j = 0; j< 1000; j++){ 
    sum += m1[i][j]+m2[j][i] 
    fprintf(sum,"\n");}
  return 1;
}

