#include <stdio.h>
#include <iostream>
#include <memory>
#include "elrabbit/common/class_util.h"
#include "elrabbit/common/string_piece.h"

using namespace std;
using elrabbit::common::StringPiece;

class Demon1
{
public:
    void set(int a) { a_ = a;}
    int get() const { return a_; }

private:
    ELRABBIT_SINGLETON(Demon1)

    int a_ = 0;
};

class Demon2
{
public:
    Demon2(int a) : a_(a) {}
    void set(int a) { a_ = a;}
    int get() const { return a_; }

private:
    int a_ = 0;
};

Demon1::Demon1() {}
Demon1::~Demon1() {}

int main()
{
	cout <<"test called zjw" << endl;
    Demon1::getInstance()->set(1000);
    cout << Demon1::getInstance()->get() << endl;

    unique_ptr<Demon2> demo1(new Demon2(200));
    cout << demo1->get() << endl;

    unique_ptr<StringPiece> piece(new StringPiece("zjw"));
    cout << piece->data() << endl;
    return 0;
}