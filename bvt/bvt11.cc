#include <uset.h>
#include <umultiset.h>
#include <fdostream.h>
#include <mistream.h>
#include <uios.h>
#include <uctralgo.h>
using namespace ustl;

void PrintVector (const int* first, const int* last)
{
    cout << "{";
    while (first < last)
	cout << ' ' << *first++;
    cout << " }" << endl;
}

int main (void)
{
    const int vv[] = { 1, 8, 9, 2, 3, 1, 1, 4, 6, 1, 3, 4 };
    set<int> v (vv, vv + VectorSize(vv));
    multiset<int> mv (vv, vv + VectorSize(vv));
    cout << "set:\t\t";
    PrintVector (v.begin(), v.end());
    cout << "erase(3):\t";
    v.erase (3);
    PrintVector (v.begin(), v.end());
    cout << "multiset:\t";
    PrintVector (mv.begin(), mv.end());
    cout << "count(1) = " << mv.count(1) << endl;
    cout << "find(4) = " << binary_search (mv, 4) - mv.begin() << endl;
    cout << "find(5) = " << binary_search (mv, 5) - mv.begin() << endl;
    cout << "erase(3):\t";
    mv.erase (3);
    PrintVector (mv.begin(), mv.end());
}
