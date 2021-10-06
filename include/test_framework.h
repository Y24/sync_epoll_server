#ifndef _TEST_FRAMEWORK_H
#define _TEST_FRAMEWORK_H 1
#include <iostream>
namespace DemoTest {
using std::cout, std::cerr, std::endl;
template <class T>
void assertEqual(T a, T b) {
  if (a != b) {
    cerr << "?_< assertEqual fails!"
         << " a: " << a << " b: " << b << endl;
  } else {
    cout << "^_^ assertEqual Successed! " << endl;
  }
}
void assertTrue(bool target) {
  if (!target) {
    cerr << "?_< assertTrue fails!" << endl;
  } else {
    cout << "^_^ assertTrue Successed! " << endl;
  }
}
}  // namespace DemoTest
#endif