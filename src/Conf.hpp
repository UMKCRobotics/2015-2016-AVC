#ifndef CONF_H
#define CONF_H

#include <string>
#include <fstream>
#include <streambuf>

#include "json.hpp"

using namespace std;
using json = nlohmann::json;

//Small configuration wrapper
//uses https://github.com/nlohmann/json

class Conf{
public:
  json data;
  Conf(string filename){
//taken from  https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
   ifstream t(filename);
   string str((istreambuf_iterator<char>(t)),
                 istreambuf_iterator<char>());
   data = json::parse(str);
  }
};
 
#endif
