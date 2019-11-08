#include <stdio.h>
#include <iostream>
#include "elrabbit/common/class_util.h"

using namespace std;


class Demon1
{
public:
    void set(int a) { a_ = a;}
    int get() const { return a_; }

private:
    ELRABBIT_SINGLETON(Demon1)

    int a_ = 0;
};

Demon1::Demon1() {}
Demon1::~Demon1() {}

int main()
{
	cout <<"test called zjw" << endl;
    Demon1::getInstance()->set(1000);
    cout << Demon1::getInstance()->get() << endl;
    return 0;
}