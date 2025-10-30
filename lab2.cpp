#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <chrono>
using namespace std;
list<int> numbers;
bool ready = false;     
mutex m;                 
void DataPreparation() {
    int i;
    cout << "write numb ";
    cin >> i;
    while (i != 0) {
        {
            unique_lock<mutex> lock(m);
            numbers.push_back(i);
        }
        cout << "write numb ";
        cin >> i;
    }

    {
        unique_lock<mutex> lock(m);
        ready = true;
    }
}
void DataProcessing() {
    while (true) {
        {
            unique_lock<mutex> lock(m);
            if (ready) break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    unique_lock<mutex> lock(m);
    cout << "Prime numbers: ";
    for (int n : numbers) {
        if (n < 2) continue;

        bool prime = true;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                prime = false;
                break;
            }
        }

        if (prime) cout << n << " ";
    }
}

int main() {
    thread t1(DataPreparation);
    t1.detach();  

    thread t2(DataProcessing);
    t2.join();     

    return 0;
}
/////////////////////////////////////////// 2//////////////////////////
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
using namespace std;

mutex m;
condition_variable cv;

int GlobalCheck = 0;

void Waits(int id) {
    unique_lock<mutex> lock(m);

    cout << "Thread " << id << " is waiting" << endl;

    cv.wait(lock, [] { return GlobalCheck == 1; });

    cout << "Thread " << id << " waiting is finished" << endl;
}

void Awake() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        unique_lock<mutex> lock(m);
        cout << "Awake notifying" << endl;
        cv.notify_all();
    }

    this_thread::sleep_for(chrono::milliseconds(100));

    {
        unique_lock<mutex> lock(m);
        GlobalCheck = 1;
        cout << "Awake notifying again" << endl;
        cv.notify_all();
    }
}
int main() {
    thread t1(Waits, 1);
    thread t2(Waits, 2);
    thread t3(Waits, 3);

    thread tAwake(Awake);

    t1.join();
    t2.join();
    t3.join();
    tAwake.join();

    return 0;
}
/////////////////////////////////////////// 3//////////////////////////
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
using namespace std;

mutex m;
condition_variable cv;
bool GlobalCheck = 0;

void Thread(int n) {
    unique_lock<mutex> lock(m);

    cv.wait(lock, [] { return GlobalCheck == 1; });

    cout << "Messege from thread " << n << endl;
}

void Notify() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        unique_lock<mutex> lock(m);
        GlobalCheck = 1;
        cv.notify_one();       
    }

}
int main() {
    thread Thread1(Thread, 1);
    thread Thread2(Thread, 2);
    thread Thread3(Thread, 3);

    thread noty(Notify);

    Thread1.join();
    Thread2.join();
    Thread3.join();

    noty.join();
    return 0;
}
/////////////////////////////////////////// 4//////////////////////////
#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <chrono>
#include <condition_variable>
using namespace std;

list<int> numbers;
condition_variable cv;
mutex m;

void DataPreparation() {
    int i;
    cout << "write numb ";
    cin >> i;
    while (i != 0) {
        {
            unique_lock<mutex> lock(m);
            numbers.push_back(i);
        }      
        cout << "write numb ";
        cin >> i;
    }
    cv.notify_one();
}

void DataProcessing() {
    unique_lock<mutex> lock(m);
    cv.wait(lock, [] { return !numbers.empty(); });

    cout << "Prime numbers: ";
    for (int n : numbers) {
        if (n < 2) continue;

        bool prime = true;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                prime = false;
                break;
            }
        }

        if (prime) cout << n << " ";
    }
}

int main() {
    thread t1(DataPreparation);
    thread t2(DataProcessing);

    t1.join();
    t2.join();

    return 0;
}
/////////////////////////////////////////// 5//////////////////////////
#include <iostream>
#include <future>
#include <cmath>
using namespace std;

long long nthPrime(int n) {
    if (n < 1) return -1;
    int count = 0;
    long long num = 1;

    while (count < n) {
        num++;
        bool isPrime = true;
        for (long long i = 2; i * i <= num; i++) {
            if (num % i == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) count++;
    }
    return num;
}

int main() {

    //1---------------------------------------
    int n;
    cout << "Enter n: ";
    cin >> n;

    future<long long> primeFuture = async(launch::deferred, nthPrime, n);

    cout << "Choose function to apply on " << n << " (1 - sqrt, 2 - sin, 3 - ln): ";
    int choice;
    cin >> choice;

    double value = 0.0;
    switch (choice) {
    case 1: value = sqrt(n); break;
    case 2: value = sin(n); break;
    case 3: value = log(n); break;
    default: cout << "Invalid choice!" << endl;
    }
    cout << "Result of chosen function = " << value << endl;

    long long prime = primeFuture.get();
    cout << n << "-th prime number is: " << prime << endl;
///2
    int n;
cout << "Enter n: ";
cin >> n;
future<long long> primeFuture = async(launch::async, nthPrime, n);

cout << "Choose function to apply on " << n
    << " (1 - sqrt, 2 - sin, 3 - ln): ";
int choice;
cin >> choice;

double value = 0.0;
switch (choice) {
case 1: value = sqrt(n); break;
case 2: value = sin(n); break;
case 3: value = log(n); break;
default: cout << "Invalid choice!" << endl;
}
cout << "Result of chosen function = " << value << endl;

auto start = high_resolution_clock::now();

long long prime = primeFuture.get();

auto end = high_resolution_clock::now();
auto duration = duration_cast<milliseconds>(end - start);

cout << n << "-th prime number is: " << prime << endl;
cout << "Time taken to compute prime: " << duration.count() << " ms" << endl;
}
/////////////////////////////////////////// 6//////////////////////////
#include <iostream>
#include <thread>
#include <deque>
#include <future>
#include <mutex>
#include <condition_variable>
#include <cmath>
using namespace std;

deque<packaged_task<int()>> taskDeque;
deque<int> nDeque;
mutex mtx;
mutex coutMutex;
condition_variable cv;
bool finishedInput = false;
bool a = false;

bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i <= sqrt(num); i++)
        if (num % i == 0) return false;
    return true;
}

int nthPrime(int n) {
    int count = 0;
    int num = 1;
    while (count < n) {
        num++;
        if (isPrime(num)) count++;
    }
    return num;
}

void workerThread() {
    while (true) {
        packaged_task<int()> task;
        int n = 0;

        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return !taskDeque.empty() || finishedInput; });

            if (taskDeque.empty() && finishedInput) break;

            task = move(taskDeque.front());
            taskDeque.pop_front();
            n = nDeque.front();
            nDeque.pop_front();
        }

        future<int> fut = task.get_future();
        task();  
        int result = fut.get();

        {
            lock_guard<mutex> lock(coutMutex);
            cout << "The " << n << "-th prime number is " << result << endl;
        }

    }
}


int main() {
    thread worker(workerThread);

    while (true) {
        int n;
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Enter n (or 0 to stop): " << flush;
            cin >> n;
        }
                
        if (n <= 0) break;

        packaged_task<int()> task([n] { return nthPrime(n); });

        {
            lock_guard<mutex> lock(mtx);
            taskDeque.push_back(move(task));
            nDeque.push_back(n);
        }
        cv.notify_all();
    }
 
    {
        lock_guard<mutex> lock(mtx);
        finishedInput = true;
    }
    cv.notify_one();

    worker.join();
    cout << "All tasks completed. Exiting." << endl;
}
/////////////////////////////////////////// 7//////////////////////////
#include <iostream>
#include <thread>
#include <deque>
#include <future>
#include <mutex>
#include <condition_variable>
#include <cmath>
using namespace std;

mutex m;

bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i <= sqrt(num); i++)
        if (num % i == 0) return false;
    return true;
}
int nthPrime(int n) {
    int count = 0;
    int num = 1;
    while (count < n) {
        num++;
        if (isPrime(num)) count++;
    }
    return num;
}

void Thread1(int n, promise<int> nth, promise<bool> f, promise<int> m10) {
    unique_lock<mutex> lock(m);

    int Nth = nthPrime(n);
    nth.set_value(Nth);
    f.set_value(true);
    m10.set_value(Nth * 10);

}

void Thread2(shared_future<int> nth, shared_future<bool> f) {
    if (f.get()) {
        unique_lock<mutex> lock(m);
        std::this_thread::sleep_for(2s);
        cout << "Squir of nth is " << sqrt(nth.get()) << endl;
    }
}

int main() {
    while (true) {
        int n;
        cout << "Enter n (or 0 to stop): ";
        cin >> n;

        if (n == 0) break;

        promise<int> p_nth;
        promise<bool> flag;
        promise<int> multy10;

        shared_future<int> f_nth = p_nth.get_future().share();
        future<bool> f_flag = flag.get_future();
        future<int> f_multy10 = multy10.get_future();

        thread t1(Thread1, n, move(p_nth), move(flag), move(multy10));
        thread t2(Thread2, f_nth, move(f_flag));

        cout << "The " << n << "-th prime number is " << f_nth.get() << endl;
        cout << "The " << n << "-th prime number * 10 is " << f_multy10.get() << endl;

        t1.join();
        t2.join();
    }
    return 0;
}




