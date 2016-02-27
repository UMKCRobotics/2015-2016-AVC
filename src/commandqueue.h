#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

//#include "logger.h"
#include "Conf.hpp"

#include <iostream>
#include <queue>
#include <functional>
#include <vector>
#include <memory>
#include <algorithm>
#include <map>
#include <utility>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>

using namespace std;

class commandQueue
{
private:
	int maxSize; //max number of functions allowed in the queue

	//priority queue
	priority_queue<pair<int, function<void()>>, vector<pair<int, function<void()>>>, function<bool(pair<int, function<void()>>, pair<int, function<void()>>)>> commands; //define this
	mutex commands_mutex; //mutex for thread locking inner queue access

	//Thoughts: Should I just use strings as the commands instead of pushing a function?

	void push(int priority, function<void()> commandFunction)
	{

		lock_guard<mutex> guard(commands_mutex);
		if(commands.size() >= maxSize)
		{
			commands.pop();

		};
		commands.push(make_pair(priority, commandFunction));

	};

	function<void()> pop()
	{

		lock_guard<mutex> guard(commands_mutex);
		if(commands.size() == 0)	{
			return [](){};
		}

		function<void()> topC = commands.top().second;
		commands.pop();
		return topC;
		

		
	};//dont return void

	commandQueue()
	{
		//do stuff with the config
		maxSize = 4; //set this from the config file

		auto compare = [](pair<int, function<void()>> a, pair<int, function<void()>> b){return a.first > b.first;};
		commands = priority_queue<pair<int, function<void()>>, vector<pair<int, function<void()>>>, function<bool(pair<int, function<void()>>, pair<int, function<void()>>)>>(compare);
	}


public:

	static commandQueue& getinstance() //of this commandqueue
	{
		static commandQueue instance;
		return instance;
	}

	//push a function to the queue
	static void pushCommand(int priority, function<void()> command){

		commandQueue::getinstance().push(priority, command);

		return;
	}

	//pop a function from the queue
	static function<void()> popCommand(){
		function<void()> command = commandQueue::getinstance().pop();

		return command;

	}

};

#endif