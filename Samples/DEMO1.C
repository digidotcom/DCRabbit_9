/*****************************************************

     demo1.c
     Z-World, 2000

		Sample program for Dynamic C Premier tutorial
******************************************************/

main() {

	int i, j;

	i = 0;
	
	while (1) {
		i++;

		for (j=0; j<20000; j++);
		
		printf("i = %d\n", i);
	}
}
