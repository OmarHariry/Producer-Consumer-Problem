#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include<bits/stdc++.h>
using namespace std;
int MAX;
queue<int>buffer;				//buffer queue
sem_t counter_sem; 				//counter semaphore used by threads/monitor to change the mssgcounter variable
sem_t n,bufferSize,buffer_sem ; 
//n is number of items in buffer ,used by collector to check if there are items in the buffer 
//full is used to check if producer can add into the buffer
//buffer_sem used by producer/consumer(moniter/collector) to enter the critical section (use the buffer)

int mssg_counter;

void* producer(void*);
void* consumer(void*);
void* change_counter(void*);
int rand_sleep(); // generates random number for sleep intervals

 
int main(){
    int n_mCounter;					// N mCounter threads
    cout << "Enter mCounter count: ";
    cin >> n_mCounter;
    cout << "Enter size of buffer: "; 		// size of buffer
    cin >> MAX;
    mssg_counter = 0; 					// global shared resource  (CRITICAL RESOURCE) between threads and monitor
    pthread_t mMonitor;
    pthread_t mCollector;
    pthread_t mCounter[n_mCounter];
    //initialize counter_sem , buffer_sem to zero
    //and number of items "n" in buffer to zero
    //and buffersize to max "as taken from user"
    sem_init(&counter_sem,0,1); 
    sem_init(&buffer_sem,0,1); 
    sem_init(&n,0,0);
    sem_init(&bufferSize,0,MAX);
    pthread_create(&mMonitor,NULL,producer,NULL); 	// creating Monitor thread
    
    for(int i = 0 ; i < n_mCounter ; ++i){		// creating the mCounter threads
        int *arg = (int*)malloc(sizeof(*arg));
        *arg = i+1;
        pthread_create(&mCounter[i],NULL,change_counter,arg);
    }
    
    pthread_create(&mCollector,NULL,consumer,NULL);	// creating the consumer thread
    								
    for(int i = 0 ; i < n_mCounter ; ++i) 	// joining the threads mCounter threads
         pthread_join(mCounter[i],NULL);
        						
    pthread_join(mMonitor,NULL);			// and finally joining the monitor and consumer threads
    pthread_join(mCollector,NULL);
	

}
void* producer(void* arg)
{
    sleep(rand_sleep()); 					
    int temp; 							//to store the value of messageCounter before changing it "temp"
    while(true){   						// produce
        int status = sem_trywait(&counter_sem);		//returns -1 if cant enter critical section
        if(status == -1) 
        {
            cout << "Monitor thread: waiting to read counter" << endl;
            sleep(1);
            sem_wait(&counter_sem);
        }
        // Counter critical section
        temp = mssg_counter;
        mssg_counter = 0;
        cout << "Monitor thread: reading a count value of " << temp << endl;
        sem_post(&counter_sem);
        int status2 = sem_trywait(&bufferSize);		//returns -1 if the buffer is full
        if(status2 == -1) 
        {
            cout << "Monitor thread: Buffer full!!" << endl;
            sleep(rand_sleep());
            sem_wait(&bufferSize);
        }
        sem_wait(&buffer_sem);				//checks if the collector is using the buffer
        // buffer critical section
        cout << "Monitor thread: writing to buffer at position " << buffer.size() << " value: " << temp <<endl;
        buffer.push(temp); 					//insert into buffer "producing element"
        sleep(rand_sleep());
        sem_post(&buffer_sem);				//semSignal(buffer) //if collector is waiting it may enter now
        sem_post(&n);						//semSignal(number of items in buffer)
    }
    return NULL;
    
}
void* change_counter(void* arg)
{
        sleep(rand_sleep());					// inteval of time t1
        int thread_id = *((int*)arg);
        cout << endl <<"Counter thread " << thread_id << ": received a message" << endl; 
        sleep(1);
        int status = sem_trywait(&counter_sem);
        if(status == -1)
        {
            cout << endl << "Counter thread " << thread_id << ": waiting to write" << endl; 
            sleep(1);
            sem_wait(&counter_sem);
        }
        // critical section of (message counter)
        ++mssg_counter;
        cout << "Counter thread "<< thread_id <<": now adding to counter, counter value="<< mssg_counter << endl;
        sleep(rand_sleep());
        sem_post(&counter_sem);
        free(arg);
        return NULL;
}
void* consumer(void* arg)
{  
    sleep(rand_sleep()); 
    while(true){
        int status = sem_trywait(&n);
        if(status == -1) 					// returns -1 if buffer is empty (n=0)
        {
            cout << "Collector thread: nothing is in the buffer!" << endl;
            sleep(rand_sleep());
            sem_wait(&n);
        }
        sem_wait(&buffer_sem);
        // critical section
        int temp= buffer.front();				//retrieve the front from the buffer queue
        cout << "Collector thread: reading from buffer value: " << temp << endl;
        buffer.pop();						// "Consuming" the element 
        sleep(rand_sleep());
        sem_post(&buffer_sem);
        sem_post(&bufferSize);
    }
    
}
int rand_sleep()
{
    return (rand() % 10) + 1;
}
