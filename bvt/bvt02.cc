#include <memblock.h>
#include <stdlib.h>
#include <memory.h>
#include <fdostream.h>
#include <mistream.h>
#include <uios.h>
using namespace ustl;

void WriteCML (const memblock& l)
{
    cout << "memblock{" << l.size() << "}: ";
    const char* pc = reinterpret_cast<const char*>(l.cdata());
    size_t nc = l.size();
    while (nc && pc[nc - 1] == 0)
	-- nc;
    cout.write (l, nc);
    cout << endl;
}

void TestMB (void)
{
    char strTest[] = "abcdefghijklmnopqrstuvwxyz";
    const size_t strTestLen = strlen(strTest);
    const char* cstrTest = strTest;

    memblock a, b;
    a.link (strTest, strTestLen);
    if (a.begin().base() != strTest)
	cout << "begin() failed on memblock" << endl;
    if (a.begin() + 5 != &strTest[5])
	cout << "begin() + 5 failed on memblock" << endl;
    if (0 != memcmp (a, strTest, strTestLen))
	cout << "memcmp failed on memblock" << endl;
    WriteCML (a);
    b.link (cstrTest, strTestLen);
    if (b.begin().base() != NULL)
	cout << "begin() of const failed on memblock" << endl;
    if (b.cmemlink::begin().base() != cstrTest)
	cout << "cmemlink::begin() failed on memblock" << endl;
    WriteCML (b);
    if (!(a == b))
	cout << "operator== failed on memblock" << endl;
    b.resize (strTestLen - 2);
    a = b;
    if (a.begin() == b.begin())
	cout << "Assignment does not copy a link" << endl;
    a.deallocate();
    a.link (strTest, strTestLen);
    WriteCML (a);
    a.insert (a.begin() + 5, 9);
    a.fill (a.begin() + 5, "-", 1, 9);
    WriteCML (a);
    a.erase (a.begin() + 2, 7);
    a.fill (a.end() - 7, "=", 1, 7);
    WriteCML (a);
    a.fill (a.begin() + 5, "TEST", 4, 3); 
    WriteCML (a);

    a.unlink();
    a.resize (a.size() + strTestLen);
    a.copy (cstrTest, strTestLen);
    WriteCML (a);
    a.resize (a.size() + strTestLen);
    a.fill (a.begin() + strTestLen, "-+=", 3, strTestLen / 3);
    WriteCML (a);
    a.resize (0);
    WriteCML (a);
    a.resize (strTestLen + strTestLen / 2);
    WriteCML (a);
}

int main (void)
{
    TestMB();
    return (0);
}
