#include <map>
#include <iostream>
#include <cassert>

int main(int argc, char **argv)
{
  std::map<std::string, std::map<int,int>> m;
  std::map<int,int>tmp;
  tmp[1]=2;
  m["hello"] = tmp;
  // check if key is present
  if (m.find("hello") != m.end())
    std::cout << "map contains key world!\n";
  // retrieve
  std::cout << m["hello"][1] << '\n';
  // std::map<std::string, int>::iterator i = m.find("hello");
  // assert(i != m.end());
  // std::cout << "Key: " << i->first << " Value: " << i->second << '\n';
  return 0;
}