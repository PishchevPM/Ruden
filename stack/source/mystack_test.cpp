#include "mystack.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;
using namespace nsp;

int main ()
{
    MyStack<int, int> stack;
    vector <int> res;

    while (!cin.eof ())
    {
        char c;
        int v, pr;
        cin >> c;

        if (c == 'q' || cin.eof ())                //quit from program
            break;

        if (c == 'e')                              //erase element
            stack.pop_back ();
        else if (c == 'p')                         //print stack
            cout << stack;
        else if (c == 't')                         //print total count of elements
            cout << stack.total_size () << endl;   
        else if (c == 'h')                         //print count of priorities
            cout << stack.priors_size () << endl;  
        else if (c == 'i')                         //push a element
        {
            cin >> v >> pr;
            stack.push_back (v, pr);
        }
        else if (c == 'o')                        //print kth element's priority and value
        {
            cin >> v;
            if (stack[v] != stack.end ())
                cout << stack[v].get_priority () << " " << *stack[v] << std::endl;         
        }
    }

    return 0;
}