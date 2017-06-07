#include "monitor.h"

#define PROD_COUNT 3  //liczba producentow
#define ELEM_COUNT 5  //liczba produkowanych elementow
#define N 5 // dlugosc bufora

//Bufor wieloelementowy
class Buffer: public Monitor
{
public:
	Buffer() : count( 0 ), indexO(0),indexZ(0) { }
    //~Buffer(){ delete items; }
	void put( int item )
	{
		enter();

		if( count == N )
			wait( fullCond );
        count = count + 1;

        items[indexZ] = item;

		indexZ = (indexZ+1)%N;

        if (count == 1)
            signal( emptyCond );

        leave();
	}

	int get()
	{
		enter();

        if(count == 0)
            wait(emptyCond);

		int res;
        count = count - 1;
		res = items[indexO];
        indexO = (indexO+1)%N;

        if(count == N-1)
            signal(fullCond);
		leave();

		return res;
	}

protected:
	int count;
	int items[N];
	int indexZ,indexO;
	Condition emptyCond, fullCond;
};

Buffer buffer;

void * konsument( void * )
{
	for( int i = 0; i < ELEM_COUNT * PROD_COUNT; ++ i )
	{
		buffer.get();
		sleep( 2 );
        printf( "====== Konsument uruchomiony, zjada %d produkt\n",i );

#ifdef _WIN32
		//Sleep( 3000 );
#else
#endif
	}

	//aby zakonczycz konsumentow
	//for( int i = 0; i < PROD_COUNT; i ++ )
		//buffer.put( 0 );

	return 0;
}

 void * producent( void * nr )
{
	int el;

	for( int i = 0; i < ELEM_COUNT; ++ i )
	{
		buffer.put(i);
        int time = *((int*)(&nr)) + 1;
        sleep(time/4);
        printf( "Producent %d uruchomiony produkuje %i\n",nr,i );

#ifdef _WIN32
		//Sleep( 3000 );
#else

#endif
	}
	return 0;
}

int main(int argc, char* argv[])
{
    //int PROD_COUNT = atoi(argv[1]);
    //int N = atoi(argv[2]);
    //int ELEM_COUNT = atoi(argv[3]);

    int data[4];
	int i;
#ifdef _WIN32
	HANDLE tid[CONS_COUNT+1];
	DWORD id;

	//utworz watek producenta
	tid[0] = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)producent, 0, 0, & id );

	//utworz watki konsumentow
	for ( i = 1; i <= CONS_COUNT; i++)
		tid[i] = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) konsument, (void*)i, 0, & id );

	for ( i = 1; i <= CONS_COUNT; i++)
		WaitForSingleObject( tid[i], INFINITE );

#else
	pthread_t tid[PROD_COUNT+1];

	//utworz watek producenta
	pthread_create(&(tid[0]), NULL, konsument, 0);
//	pthread_create(&(tid[0]), NULL, konsument, *data);

	//utworz watki konsumentow
	for ( i = 1; i <= PROD_COUNT; i++){
	//	pthread_create(&(tid[i]), NULL, producent, *data );
    	pthread_create(&(tid[i]), NULL, producent, (void*)i );
    }
	//czekaj na zakonczenie watkow
	for ( i = 0; i < PROD_COUNT + 1; i++)
	  pthread_join(tid[i],(void **)NULL);
#endif
	return 0;
}

