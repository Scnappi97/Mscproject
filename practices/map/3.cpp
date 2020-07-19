
#include <map>
#include <string>
#include <iostream>
using namespace std;
using std::map;
map<string, string> namemap;

int main(){
namemap["a"]="aa";
namemap["b"]="bb";
namemap["c"]="cc";
if(namemap.find("d") != namemap.end()) { 
    std::cout<<namemap["a"];
}
}




