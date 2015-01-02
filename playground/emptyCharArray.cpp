#include <iostream>
#include <cstring>

using namespace std;

typedef char TThostFtdcUserIDType[16];

struct MyStruct
{
    char first[20];
};

int main()
{
    TThostFtdcUserIDType ActiveUserID;

    cout << ActiveUserID << endl;
    cout << strlen(ActiveUserID) << endl;
    cout << (ActiveUserID[0] == 0) << endl;
    cout << (ActiveUserID[0] == '\0') << endl;

    ActiveUserID[0] = 0;
    cout << ActiveUserID << endl;
    cout << strlen(ActiveUserID) << endl;
    cout << (ActiveUserID[0] == 0) << endl;
    cout << (ActiveUserID[0] == '\0') << endl;
    
    ActiveUserID[0] = '\0';
    cout << ActiveUserID << endl;
    cout << strlen(ActiveUserID) << endl;
    cout << (ActiveUserID[0] == 0) << endl;
    cout << (ActiveUserID[0] == '\0') << endl;

    char array1 [] = "2160";
    char array2 [] = "2158";
    char array3 [] = "2160";

    cout << (array1 == array2) << endl;
    cout << (array2 == array3) << endl;
    cout << (array1 == array3) << endl;

    cout << memcmp(array1, array2, 5) << endl;
    cout << memcmp(array2, array3, 5) << endl;
    cout << memcmp(array1, array3, 5) << endl;

    MyStruct struct1, struct2;
    strcpy(struct1.first, "foo");
    strcpy(struct2.first, "foo");
    cout << memcmp(&struct1, &struct2, sizeof(MyStruct)) << endl;

    if (0 || -1) {
	cout << "aha" << endl;
    }

    return 0;
}
