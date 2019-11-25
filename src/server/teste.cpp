#include <iostream>
#include <cstdio>




int main () {
	
	int matric[3][3];

	for (int i = 0; i < 9; ++i) {
		matric[i/3][i%3] = i;
		if (i%3 == 0) printf("\n");
		printf("%d ", matric[i/3][i%3]);
	}
	printf("\n");




	return 0;
}
