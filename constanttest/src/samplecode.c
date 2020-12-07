// samplecode.c
#include "stdio.h"

int main() {
	
	const int array[] = { 0, 1, 0, 3, 0, 5 };
	
	int temp = 0;
	temp += array[0];
	/*for (int i = 0; i < 6; ++i) {
		temp += array[i];	
	}*/

	printf("%d\n", temp);
	
	return 0;
}
