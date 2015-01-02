#include <iostream>

using namespace std;

enum class A { win, lose };

int main()
{
    A a = A::win;
    if (a == A::win)
	cout << "win" << endl;
    else if (a == A::lose)
	cout << "lose" << endl;
    else
	cout << "unknow" << endl;

    return 0;
}
