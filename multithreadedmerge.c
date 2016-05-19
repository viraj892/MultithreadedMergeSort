#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
int* a; //array elements are stored here
int* b; //sorted elements are stored here
int n; //size of the array is stored here

//Reads the elements from a file
void reader(char* filename){
	FILE* f = fopen(filename,"r"); //File stream
	fscanf(f,"%d",&n); // Read the size of input (First line of the input file)
	a = (int*) malloc(sizeof(int)*n); //Heap memory allocated dynamically
	int i;
	//Read the elements from file into the array
	for (i = 0; i < n; ++i)
		fscanf(f,"%d",&a[i]);
	fclose(f);
}
//Prints an array
void print(int* a){
	int i;
	for (i = 0; i < n; ++i)
		printf("%d ",a[i]);
	printf("\n");
}
//Prints the sorted array to a file
void writer(char* filename){
	FILE* f = fopen(filename,"w");
	fprintf(f,"sorted array of size %d :\n",n);
	int i;
	for (i = 0; i < n; ++i)
		fprintf(f,"%d ",b[i]);
	fprintf(f,"\n");
	fclose(f);
}

struct index{int p,r;};

void* merge_sort(void* param){
	struct index* pr = (struct index*) param;
	int p = pr->p,  r = pr->r , ret1,ret2;
	//exit if there is only one element in the partition
	if(p==r)
		pthread_exit(0);

	pthread_t thread1,thread2;
	struct index pr1,pr2; //index pointers both parts of the split set
	int q = (p+r)/2; //split the set into half
	//assign index positions for both the halves
	pr1.p = p;    pr1.r = q;
	pr2.p = q+1;  pr2.r = r;
	
	//Create thread1 to recursively perform merge sort on first half of the array
	ret1 = pthread_create(&thread1,NULL,merge_sort,(void*) &pr1); //returns 0 if thread created sucessfully
	if (ret1>0)
		printf("failed to create new thread 1\n");
	
	//Create thread2 to recursively perform merge sort on second half of the array
	ret2 = pthread_create(&thread2,NULL,merge_sort,(void*) &pr2);
	if (ret2>0)
		printf("failed to create new thread 2\n");

	pthread_join(thread1,NULL); //wait for thread1 to exit/terminate
	pthread_join(thread2,NULL);

	//Sorting every partition and storing into b[]
	int k = p ,i = p ,j = q+1;
	while (i<=q && j<=r){
		if (a[i] < a[j])
			b[k++] = a[i++];
		else
			b[k++] = a[j++];
	}
	for (; i<=q ; i++)
		b[k++] = a[i];
	for (; j<=r ; j++)
		b[k++] = a[j];

	for (i= p ; i <= r ;i++)
		a[i] = b[i];

	pthread_exit(0);
	return NULL;
}

int main(void) {
	reader("input.txt");
	b = (int*)malloc(sizeof(int)*n); //dynamic heap memory for sorted array
	struct index start;
	start.p = 0;    start.r = n-1;
	//Declaring a thread
	pthread_t start_thread;

	struct timeval tv;
	gettimeofday(&tv,NULL); //get timestamp before sorting
	long long t1 = tv.tv_usec;
	
	/*Create the thread named start_thread
	  Passing merge_sort as the starting routine and 
	  the index of the first and last element as the argument
	*/
	pthread_create(&start_thread,NULL,merge_sort,&start);
	pthread_join(start_thread,NULL);

	gettimeofday(&tv,NULL); //get timestamp after sorting
	long long t2 = tv.tv_usec;

	printf("elapsed time = %lld usec\n",t2-t1);
	print(a);
	writer("sorted.txt"); //write output to file
	return 0;
}