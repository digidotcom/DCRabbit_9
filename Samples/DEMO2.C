/*****************************************************

     demo2.c
     Z-World, 2000

		Sample program for Dynamic C Premier tutorial
******************************************************/

main() {

	int i, j, k;

	i = 0;
	k = 0;
		
	while (1) {
		i++;

		k = k + i;
		
		for (j=0; j<20000; j++);
		
		printf("i = %d\n", i);
	}
}
