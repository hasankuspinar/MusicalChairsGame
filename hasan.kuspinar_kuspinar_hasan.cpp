#include <iostream>
#include <mutex>
#include <thread>
#include "IntQueueHW6.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
using namespace std;
mutex myMutex, coutMutex;

void player_function(IntQueueHW6 & myQueue, int player_no, struct tm *ptm, int & out) // player thread
{
    this_thread::sleep_until(chrono::system_clock::from_time_t(mktime(ptm))); // sleeping the player until 2 seconds later
    myMutex.lock(); // locking the thread for synchronization
    if(!myQueue.isFull()) // if the player has captured a chair
    {
        myQueue.enqueue(player_no); // enqueueing the player no to check the player who captured a chair
        myMutex.unlock();
        coutMutex.lock();
        cout << "Player " << player_no << " captured a chair at " << put_time(ptm, "%X") << "." << endl;
        coutMutex.unlock();
    }
    else // if the player could not capture a chair
    {
        out = player_no; // for keeping the ID of the player who could not capture a chair
        myMutex.unlock();
    }
}

bool vector_find(int num, vector<int> & vector1) // a function that checks if the given num is in the vector or not
{
    bool result = false;
    for(int i = 0; i < vector1.size(); i++)
    {
        if(num == vector1[i]) // checking if num is equal to the element in the ith index
        {
            result = true;
        }
    }
    return result;
}

int main()
{
    int num_Players;
    cout << "Welcome to Musical Chairs game!\nEnter the number of players in the game: " << endl;
    cin >> num_Players;
    cout << "Game Start!" << endl << endl;
    const int num1 = num_Players; // a constant that holds number of players to go over the threads array and check which of those still remain in the game
    int num_Chairs = num_Players - 1;
    int num_Rounds = num_Chairs;
    vector<int> IDs; // vector that includes the players remaining in the game
    for(int k = 0; k < num_Players; k++)
    {
        IDs.push_back(k);
    }
    thread myThreads[num_Players];
    for(int a = 0; a < num_Rounds; a++) // iterating rounds times
    {
        int out;
        //thread myThreads[num_Players]; // creating the player threads
        IntQueueHW6 myQueue(num_Chairs); // creating a queue with number of rounds elements
        time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
        struct tm *ptm = new struct tm;
        localtime_s(ptm, &tt);
        cout << "Time is now " << put_time(ptm,"%X")<<endl;
        if(ptm->tm_sec >= 58) // checking the time overflow of seconds after sleeping 2 seconds
        {
            ptm->tm_sec = 0 + (ptm->tm_sec+2)%60;
            ptm->tm_min += 1;
        }
        else // checking the time overflow of seconds after sleeping 2 seconds
        {
            ptm->tm_sec += 2;
        }
        for(int i = 0; i < num1; i++)
        {
            if (vector_find(i, IDs)) // checking if the indexes are in the list to not include the player who could not capture a chair
            {
                myThreads[i] = thread(&player_function, ref(myQueue), i, ptm, ref(out)); // creating the threads
            }
        }
        for(int i = 0; i < num1; i++)
        {
            if (vector_find(i, IDs)) // checking if the indexes are in the list to not include the player who could not capture a chair
            {
                myThreads[i].join(); // joining the threads
            }
        }
        IDs.clear();
        for(int b = 0; b < num_Chairs; b++) // updating IDs vector with the players that captured a chair
        {
            int num;
            myQueue.dequeue(num);
            IDs.push_back(num); // pushing the num from the queue to the vector
        }
        cout << "Player " << out << " couldn't capture a chair." << endl; // displaying the player who could not capture a chair
        cout << "Remaining players are as follows: ";
        for(int b = 0; b < num_Chairs; b++) // displaying the remaining players
        {
            cout << IDs[b] << " ";
        }
        num_Chairs--; // decrementing number of chairs
        num_Players--; // decrementing number of players
        cout << endl << endl;
    }
    cout << "Game over!" << endl;
    cout << "Winner is Player " << IDs[0] << "." << endl;

    return 0;
}