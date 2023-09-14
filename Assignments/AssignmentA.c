#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int global_variabel = 0;

int main(){
	
	int lokal_variabel = 0;
	
	pid_t pid = vfork(); //Eller vfork()
	
	if(pid == 0){	//Child-process:
		for(int i=0; i<3; i++){
			lokal_variabel++;
			global_variabel++;
			printf("\nChild process: lokal = %d, global = %d", lokal_variabel, global_variabel);
		}
		printf("\n");
		exit(0);
	} else {
		for(int i=0; i<3; i++){
			lokal_variabel++;
			global_variabel++;
			printf("\nParent process: lokal = %d, global = %d", lokal_variabel, global_variabel);
			
		}
		printf("\n");
		exit(0);
	}
	
	return 0;
}