#include <stdio.h>



void testloops() {
    int tot = 0;
    for(int i=0; i<100; i++) {
        for (int j=0; j<100; j++) {
          for(int k=0; k<5; k++){
            for(int l=0; l<5; l++){
              tot+=1;
	    }
	  }
	}
    }
    fprintf(stdout, "%d\n", tot);
}





int main(){
    testloops();
    //int tot=0;
    //for(int i=0; i<10; i++) {
    //    //tot+=1;
    //}
    return 0;
}
