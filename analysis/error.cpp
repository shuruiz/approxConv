#include <iostream>


int main(){
	
	int passval[996];
	int baseval[996];

	int input;
	for(int i=0; i < 996; ++i)
		std::cin >> passval[i];
	for(int i=0; i < 996; ++i)
		std::cin >> baseval[i];

	double sumpercenterror;
	for(int i=0; i < 996; ++i){
		sumpercenterror += (double)(baseval[i] - passval[i])/((double) baseval[i]);
	}
	double avgpercenterror = sumpercenterror/((double)996*996);
	
	std::cout << avgpercenterror << "\n";
}
