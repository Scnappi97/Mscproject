#include<map>
#include<stdio.h>
#include<string>
#include <iostream>
#include <functional>
using namespace std;

map<string, string> maps;
hash<string> str_hash;
void sig(string x, string y){
    maps[x]=str_hash(y);
}
int main(){
    sig("a","aa");
    sig("b","aa");
    std::cout<<maps["a"];
    //std::cout << "str1 and str2: " << (str_hash(maps[1])==str_hash(maps[2])) << '\n';
    std::cout << "str1 and str2: " << (maps["1"]==maps["2"]) << '\n';
    return 0;
}