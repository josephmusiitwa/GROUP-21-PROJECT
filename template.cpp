#include <iostream>
using namespace std;

template<typename T, typename U ,typename M>
class Triple {
    T first;
    U second;
    M third;

public:
    Triple(T a, U b, M c) : first(a), second(b) , third(c){}

    void display() const {
        cout<<format("First: {}\n", first);
        cout<<format("Second: {}\n", second);
        cout<<format("Third: {}\n", third);
    }
};

int main() {
    Triple<int, string, char> t1(10, "Steven",'M');
    Triple<double, string, char> t2(21.5,"Jose",'M');
    

    t1.display();
    cout<< "   "<<endl; 
    t2.display();
}