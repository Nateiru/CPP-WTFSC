#include <iostream>

class Person
{
public:
  Person(const char* name):name_(name) {};

  Person(const Person& person) {
    name_ = person.name_;
    std::cout << "拷贝构造函数" << std::endl;
  }

  Person(Person&& person) {
    name_ = person.name_;
    std::cout << "移动构造函数"<< std::endl;
  }

  Person& operator=(const Person& person) {
    name_ = person.name_;
    std::cout << "拷贝赋值运算符" << std::endl;
    return *this;
  }

  Person& operator=(Person&& person) {
    if (this != &person) {
      name_ = person.name_;
      person.name_ = nullptr;
      std::cout << "移动赋值运算符" << std::endl;
    }
    return *this;
  }
private:
  const char*  name_;
};
 
Person getPerson() {
  Person person("person in func");
  return person;
}

int main() {
  Person person1("xiaohong");
  Person person2("xiaoming");
  Person person3(getPerson());//移动构造函数
  Person person4(std::move(person1));//移动构造函数
  Person person5(person2);//拷贝构造函数

  Person person6("xiaolan");
  person6 = std::move(person3);//移动赋值运算
  Person person7("xiaoqi");
  person7 = Person("xiaoqi");//移动赋值运算
  Person person8("xiaoba");
  person8 = person1;//拷贝赋值运算符
  return 0;
}