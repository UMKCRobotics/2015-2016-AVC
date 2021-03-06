#ifndef GPSNODELIST_H
#define GPSNODELIST_H

#include "GPSParser.h"
#include "Conf.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class GPSNodelist {
  private:
    vector<string> _Filelines;
    int _currentIndex;
    string getNextNodeString();
  public:
    GPSNodelist(Conf);
    GPSNode getNextNode();
    bool allNodesVisited(){
      _currentIndex >= _Filelines.size()-1;
    }
};

#endif
