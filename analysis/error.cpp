#include <iostream>


int main(){
	
	float passval[996*996];
	float baseval[996*996];

	int input;
	for(int i=0; i < 996*996; ++i)
		std::cin >> passval[i];
	for(int i=0; i < 996*996; ++i)
		std::cin >> baseval[i];

	double sumpercenterror = 0;
	for(int i=0; i < 10; ++i){
		sumpercenterror += (double)(baseval[i] - passval[i])/((double) baseval[i]);
	}
	double avgpercenterror = sumpercenterror/((double)996*996);
	
	std::cout << avgpercenterror << "\n";
}
