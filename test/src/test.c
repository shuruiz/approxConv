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
float kernel[3][3]={{1.0,0,1.0},{0,1.0,0},{0,0,1.0}};

// float Multiply(){
//     float b=2.2f;
//     float c=4.32f;
//     float d= a*c;
//     return d;
// }


// void conv2d(double in[], int inh, int inw, 
// 	    double k[], double out[]){
//     for(int ih=0; ih<(inh-KH+1); ih++) {
//         for(int iw=0; iw<(inw-KW+1); iw++) {
//             for(int jh=0; jh<5; jh++){
//                 for(int jw=0; jw<5; jw++) {
//                     out[(ih*IW)+iw] += in[((ih+jh)*IW)+iw+jw] * k[(jh*KH)+jw];	
// 		}
// 	    }
//         }
//     }
// }


int main(int argc, char const *argv[])
{


    float target[3][3]={{1.0,0,1.0},{0,1.0,0},{0,0,1.0}};
	float out[3][3]={{0,0,0},{0,0,0},{0,0,0}};
    for(int ih=0; ih<3; ih++) {
        for(int iw=0; iw<3; iw++) {
            out[ih][iw] += kernel[ih][iw] *target[iw][ih];	}}
    // printf(out[1][1]);      
    return 0;
}