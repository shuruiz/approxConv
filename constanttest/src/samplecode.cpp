// samplecode.cpp
#include "stdio.h"
#include <iostream>

int main() {
	
	const int array[] = { 0.1, 1, 1, 3, 0.1, 5 };
	
	int temp = 0;
	for (int i = 0; i < 6; ++i) {
		int input;
		std::cin >> input;
		temp += array[i] * input;	
	}

	printf("%d\n", temp);
	
	return 0;
}
