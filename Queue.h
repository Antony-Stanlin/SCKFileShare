#pragma once

#include <queue>
#include <vector>

#include <mutex>
#include <condition_variable>


class Queue
{

private:

    std::queue<std::vector<char>> queue;

    std::mutex mutex;

    std::condition_variable condition;

    bool finished = false;


public:

    void Push(std::vector<char> block);

    bool Pop(std::vector<char>& block);

    void SetFinished();

    bool IsEmpty();

};