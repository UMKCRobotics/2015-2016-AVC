#include "GPSNodelist.h"

GPSNodelist::GPSNodelist(string filename) {
	_currentIndex = 0;
	string line;
	ifstream nodefile(filename.c_str());
	while (getline(nodefile, line))
	{
		_Filelines.push_back(line);
	}
}

string GPSNodelist::getNextNodeString() {
	string nextNode;
	if (_currentIndex < _Filelines.size())
	{
		nextNode = _Filelines.at(_currentIndex);
	}
	else
	{
		nextNode = "END_OF_FILE";
	}
	_currentIndex += 1;
	return nextNode;
}

GPSNode GPSNodelist::getNextNode() {
	stringstream ss(getNextNodeString());
	vector<string> string_result;
	while(ss.good())
	{
    	string substr;
    	getline(ss, substr, ',');
    	string_result.push_back(substr);
	}
	long double latitude,longitude;

	stringstream first(string_result[0]);
	stringstream second(string_result[1]);

	first >> latitude;
	second >> longitude;

	return GPSNode(latitude,longitude);
}