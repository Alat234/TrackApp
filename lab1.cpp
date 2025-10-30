#include <iostream>
#include <thread>
#include <mutex>
#include <Windows.h>
using namespace std;
void thread1()
{
	cout << 1<<endl;
}
void thread2()
{
	cout << 2 << endl;
}
int main() {
	thread t1(thread1);
	thread t2(thread2);
	return 0;
}
/////////////////////////////////////////// 2//////////////////////////
int main() {
    thread t1(thread1);
    thread t2(thread2);

    t1.detach();
    t2.detach();

    return 0;
}

int main() {
    thread t1(thread1);
    thread t2(thread2);
    Sleep(300);

    t1.detach();
    t2.detach();

    return 0;
}
/////////////////////////////////////////// 3//////////////////////////
#include <iostream>
#include <thread>
#include <list>
#include <algorithm> 
#include <Windows.h>
using namespace std;

list<int> l = { 7, 5, 16, 8 };

void AddToList(int n) {
    l.push_back(n);
    cout << "Added element: " << n << endl;

    for (int i = 1; i <= 9; i++) {
        int val = n + i;
        l.push_back(val);
        cout << "Edded elem: " << val << endl;
        Sleep(50); 
    }
}

void ListContains(int n) {

    for (int i = 0; i < 10; i++) {
        bool found = false;
        for (int x : l) {
            if (x == n) {
                found = true;
                break;
            }
        }
        if (found)
            cout << "Element " << n << " found" << endl;
        else
            cout << "Element " << n << " NOT found" << endl;
        Sleep(50);
    }
}

int main() {
    int n = 10; 

    thread t1(AddToList, n);
    thread t2(ListContains, n);

    Sleep(1000); 

    t1.detach();
    t2.detach();

    return 0;
}
/////////////////////////////////////////// 4//////////////////////////
#include <iostream>
#include <thread>
#include <list>
#include <Windows.h>
#include <mutex>   
using namespace std;

list<int> l = { 7, 5, 16, 8 };
mutex m; 

void AddToList(int n) {
    
    m.lock();
    l.push_back(n);
    cout << "Added element " << n << endl;
    m.unlock();
    

    for (int i = 1; i <10; i++) {
        int val = n + i;
        {
            m.lock();
            l.push_back(val);
            cout << "Edded elem: " << val << endl;
            Sleep(50);
            m.unlock();
        }
    }
}

void ListContains(int n) {
    for (int i = 0; i < 10; i++) {
        bool found = false;
        {
            m.lock();
            for (int x : l) {
                if (x == n) {
                    found = true;
                    break;
                }
            }
        }
        if (found)
            cout << "Element " << n << " found" << endl;
        else
            cout << "Element " << n << " NOT found" << endl;

        Sleep(50);
        m.unlock();
    }
}

int main() {
    int n = 10;

    thread t1(AddToList, n);
    thread t2(ListContains, 15);

    Sleep(2000); 

    t1.detach();
    t2.detach();

    return 0;
}
/////////////////////////////////////////// 5//////////////////////////
#include <iostream>
#include <thread>
#include <list>
#include <Windows.h>
#include <mutex>
using namespace std;

list<int> l = { 7, 5, 16, 8 };
mutex m;

void AddToList(int n) {
    lock_guard<mutex> guard(m);
    l.push_back(n);
    cout << "Added element " << n << endl;
}

void ListContains(int n) {
    lock_guard<mutex> guard(m);
    bool found = false;
    for (int x : l) {
        if (x == n) {
            found = true;
            break;
        }
    }

    if (found)
        cout << "Element " << n << " found" << endl;
    else
        cout << "Element " << n << " NOT found" << endl;
}

int main() {
    int start_value = 10;
    int check_value = 13;

    for (int i = 0; i < 10; i++) {
        thread t1(AddToList, start_value + i);
        thread t2(ListContains, check_value);

        t1.detach();
        t2.detach();

        Sleep(100); 
    }

    Sleep(2000); 

    return 0;
}
/////////////////////////////////////////// 6//////////////////////////\
#include <iostream>
#include <thread>
#include <list>
#include <Windows.h>
#include <mutex>
#include <string>
using namespace std;

class someData {
public:
    string name;
    string surname;
    string address;
    int age;
};

class exchangePerson {
    someData sd;
    mutex m;
public:

    static void JohnDoe(exchangePerson& ep) {
        lock_guard<mutex> guard(ep.m);
        ep.sd.name = "John";
        ep.sd.surname = "Doe";
        ep.sd.address = "Unknown";
        ep.sd.age = 120;
    }
    static void JacobSmith(exchangePerson& ep) {
        lock_guard<mutex> guard(ep.m);
        ep.sd.name = "Jacob";
        ep.sd.surname = "Smith";
        ep.sd.address = "Known";
        ep.sd.age = 1;
    }

    static void Swap(exchangePerson& ep1, exchangePerson& ep2) {
        if (&ep1 == &ep2) return;

        lock(ep1.m, ep2.m);

        lock_guard<mutex> lock1(ep1.m, adopt_lock);
        lock_guard<mutex> lock2(ep2.m, adopt_lock);

        swap(ep1.sd, ep2.sd);
    }


    void printData(const string& title) {
        lock_guard<mutex> guard(m);
        cout << title << ":" << endl;
        cout << "  Name: " << sd.name << endl;
        cout << "  Surname: " << sd.surname << endl;
        cout << "  Address: " << sd.address << endl;
        cout << "  Age: " << sd.age << endl << endl;
    }
};

int main() {
    exchangePerson ep1;
    exchangePerson ep2;

    
    ep1.printData("Object 1 before");
    ep2.printData("Object 2 before");

    thread t1(exchangePerson::JohnDoe, ref(ep1));
    thread t2(exchangePerson::JacobSmith, ref(ep2));

    t1.detach();
    t2.detach();

    Sleep(100); 

    ep1.printData("Object 1 after JohnDoe");
    ep2.printData("Object 2 after JacobSmith");

    thread t3(exchangePerson::Swap, ref(ep1), ref(ep2));
    t3.join(); 

    ep1.printData("Object 1 after all");
    ep2.printData("Object 2 after all");

    return 0;
}
/////////////////////////////////////////// 7//////////////////////////
#include <iostream>
#include <thread>
#include <list>
#include <Windows.h>
#include <mutex>
#include <string>
using namespace std;

class someData {
public:
    string name;
    string surname;
    string address;
    int age;
};

class exchangePerson {
    someData sd;
    mutex m;
public:

    static void JohnDoe(exchangePerson& ep) {
        lock_guard<mutex> guard(ep.m);
        ep.sd.name = "John";
        ep.sd.surname = "Doe";
        ep.sd.address = "Unknown";
        ep.sd.age = 120;
    }
    static void JacobSmith(exchangePerson& ep) {
        lock_guard<mutex> guard(ep.m);
        ep.sd.name = "Jacob";
        ep.sd.surname = "Smith";
        ep.sd.address = "Known";
        ep.sd.age = 1;
    }

    static void Swap(exchangePerson& ep1, exchangePerson& ep2) {
        if (&ep1 == &ep2) return;

        unique_lock<mutex> lock1(ep1.m, defer_lock);
        unique_lock<mutex> lock2(ep2.m, defer_lock);

        lock(lock1, lock2);

        swap(ep1.sd, ep2.sd);
    }


    void printData(const string& title) {
        lock_guard<mutex> guard(m);
        cout << title << ":" << endl;
        cout << "  Name: " << sd.name << endl;
        cout << "  Surname: " << sd.surname << endl;
        cout << "  Address: " << sd.address << endl;
        cout << "  Age: " << sd.age << endl << endl;
    }
};

int main() {
    exchangePerson ep1;
    exchangePerson ep2;

    
    ep1.printData("Object 1 before");
    ep2.printData("Object 2 before");

    thread t1(exchangePerson::JohnDoe, ref(ep1));
    thread t2(exchangePerson::JacobSmith, ref(ep2));

    t1.detach();
    t2.detach();

    Sleep(100); 

    ep1.printData("Object 1 after JohnDoe");
    ep2.printData("Object 2 after JacobSmith");

    thread t3(exchangePerson::Swap, ref(ep1), ref(ep2));
    t3.join(); 

    ep1.printData("Object 1 after all");
    ep2.printData("Object 2 after all");


    return 0;
}


