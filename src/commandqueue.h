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

enum priority{CRITICAL = 0, HIGH = 1, MID = 2, LOW = 3};

class commandQueue
{
private:
	int maxSize; //max number of functions allowed in the queue

	//priority queue
	priority_queue<pair<priority, function<void()>>, vector<pair<priority, function<void()>>>, function<bool(pair<priority, function<void()>>, pair<priority, function<void()>>)>> commands;

	mutex commands_mutex; //mutex for thread locking inner queue access

	//Thoughts: Should I just use strings as the commands instead of pushing a function?

	void push(priority pri, function<void()> commandFunction)
	{

		lock_guard<mutex> guard(commands_mutex);
		if(commands.size() >= maxSize)
		{
			commands.pop();

		};
		commands.push(make_pair(pri, commandFunction));

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

	};


public:

	static void initQueue(Conf c) //don't forget to initialize!
	{
		commandQueue::getinstance().maxSize = c.data["motorcontroller"]["queue_size"];

		auto compare = [](pair<priority, function<void()>> a, pair<priority, function<void()>> b){return a.first > b.first;};
		commandQueue::getinstance().commands = priority_queue<pair<priority, function<void()>>, vector<pair<priority, function<void()>>>, function<bool(pair<priority, function<void()>>, pair<priority, function<void()>>)>>(compare);
	}

	static commandQueue& getinstance() //of this commandqueue
	{
		static commandQueue instance;
		return instance;
	}

	//push a function to the queue
	static void pushCommand(priority pri, function<void()> command){

		commandQueue::getinstance().push(pri, command);

		return;
	}

	//pop a function from the queue
	static function<void()> popCommand(){
		function<void()> command = commandQueue::getinstance().pop();

		return command;

	}

};

#endif