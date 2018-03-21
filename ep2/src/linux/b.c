int main(){
	int i = 0;
	int j = 1;
	for(i=0;i<2000000000;i++){
		j=i*i+i-j*4;
	}
}
