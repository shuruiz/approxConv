#include <iostream>

#define outH 991
#define outW 991

int main(){
	
	int passval[outW*outH];
	int baseval[outW*outH];

	int input;
	for(int i=0; i < outW*outH; ++i) {
		std::cin >> input;
		passval[i] = input;
	}
		
	for(int i=0; i < outW*outH; ++i) {
		std::cin >> input;
		baseval[i] = input;
	}

	double sumpercenterror = 0;
	for(int i=0; i <outW*outH; ++i){
		if (baseval[i] == 0) std::cout << "Zero\n";
		sumpercenterror += (double)(baseval[i] - passval[i])/((double) baseval[i]);
	}
	double avgpercenterror = sumpercenterror/((double)outW*outH);
	
	std::cout << avgpercenterror << "\n";
}
