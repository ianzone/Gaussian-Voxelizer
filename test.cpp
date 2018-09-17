#include <iostream>  
#include <vector>
#include <boost/rational.hpp>  
 
using namespace std;  
using namespace boost;  
 
int main()  
{  
    double a = -0.5;
    rational<int> b(a*10, 10);
    rational<int> x, y, z;
    x=b+5;
    y=b*0;
    z=b/3;
    cout << b << endl; 
    cout << x << endl;
    cout << y << endl;
    cout << z << endl; 

    cout << rational<int>(3,5)*5 << endl;
    cout << rational_cast<double>(y) << endl;


    bool i = false, j = true;
    i = i || j;
    if (i)
    {
        cout<<"true"<<endl;
    }

    int k=0;
    vector<int> v(10);
    while ( k <= 5)
        v[k++]=1;
    cout<<endl;
    for (auto u : v)
    {
        cout<<u;
    }
	return 0;
} 