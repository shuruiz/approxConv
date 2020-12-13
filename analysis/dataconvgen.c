#include <stdio.h>


#define IH 1000
#define IW 1000

#define KH 5
#define KW 5

const int outH = IH-KH+1;
const int outW = IW-KW+1;

void testloops(float* input, float* output) {
    
	
	
const float array[5][5] = {
				          	{ 1, 1, 1000, 1, 1},
				          	{ 1, 1,    1, 1, 1},
				          	{ 1, 1,    1, 1, 1},
				          	{ 1, 1,    1, 1, 1},
				          	{ 1, 1,    1, 1, 1}
						 	};
	
    for(int oh=0; oh<outH; oh++) {
      for (int ow=0; ow<outW; ow++) {
        for(int kh=0; kh<KH; kh++){
          for(int kw=0; kw<KW; kw++){
	        output[(oh*outW) + ow] += 
		      (float)input[(oh+kh)*IW + ow+kw] * array[kh][kw];
	      }
	    }
      }
    }
	
}





int main(){
    float input[IH*IW];
    float output[outH*outW];
    for (int i = 0; i < IH*IW; ++i) {
        scanf("%f", input + i);
    }

    testloops(input, output);
    
	
	for (int i = 0; i < outW*outH; ++i) {
    	fprintf(stdout, "%f\n", output[i]);
	}
	
	
	//int tot=0;
    //for(int i=0; i<10; i++) {
    //    //tot+=1;
    //}
    return 0;
}
