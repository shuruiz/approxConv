// #include <stdio.h>

// int main()
// {

  
//   float i=10.0f;
//   float j=20.1f;
//  // FILE* myfile;
//   float c = i+j;
// //  printf("%s\n",c );
//   return 1;
// }

 #include <stdio.h>
// Global Constant value
float a=1.4f;

float Multiply(){
    float b=2.2f;
    float c=4.32f;
    float d= a*c;
    return d;
}

int main(int argc, char const *argv[])
{

    printf("%f\n",Multiply());

    return 0;
}