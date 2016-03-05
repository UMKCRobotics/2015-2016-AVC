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

template <typename CommandFunction>
class commandQueue
{
private:
	int maxSize; //max number of functions allowed in the queue
	bool initialized = false; //starts out uninitialized

	//priority queue
	priority_queue<pair<priority, CommandFunction>, vector<pair<priority, CommandFunction>>, function<bool(pair<priority, CommandFunction>, pair<priority, CommandFunction>)>> commands;
	mutex commands_mutex; //mutex for thread locking inner queue access

	void push(priority pri, CommandFunction command)
	{

		lock_guard<mutex> guard(commands_mutex);
		if(commands.size() >= maxSize)
		{
			commands.pop();

		};
		commands.push(make_pair(pri, command));

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

	void initialize(Conf c)
	{
		maxSize = c.data["motorcontroller"]["queue_size"];

		auto compare = [](pair<priority, CommandFunction> a, pair<priority, CommandFunction> b){return a.first < b.first;};
		commands = priority_queue<pair<priority, CommandFunction>, vector<pair<priority, CommandFunction>>, function<bool(pair<priority, CommandFunction>, pair<priority, CommandFunction>)>>(compare);

		initialized = true;
	}

	commandQueue(){};


public:

	static void initQueue(Conf c) //don't forget to initialize!
	{
		commandQueue::getinstance().initialize(c);
	}

	static commandQueue& getinstance() //of this commandqueue
	{
		static commandQueue instance;
		return instance;
	}

	//push a function to the queue
	static void enqueueCommand(priority pri, CommandFunction && command){

		commandQueue::getinstance().push(pri, command);

		return;
	}

	//pop a function from the queue
	static CommandFunction dequeueCommand(){
		CommandFunction command = commandQueue::getinstance().pop();

		return command;

	}

};

#endif