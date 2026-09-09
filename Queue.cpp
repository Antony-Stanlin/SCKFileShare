#include "Queue.h"


void Queue::Push(std::vector<char> block){

    {
        std::lock_guard<std::mutex> lock(mutex);

        queue.push(block);
    }

    condition.notify_one();
}

bool Queue::Pop(std::vector<char>& block){

    std::unique_lock<std::mutex> lock(mutex);

    condition.wait(
        lock,
        [this]()
        {
            return !queue.empty() || finished;
        }
    );

    if (queue.empty() && finished)
        return false;

    block = queue.front();

    queue.pop();

    return true;
}

void Queue::SetFinished(){

    {
        std::lock_guard<std::mutex> lock(mutex);

        finished = true;
    }

    condition.notify_all();
}

bool Queue::IsEmpty(){

    std::lock_guard<std::mutex> lock(mutex);

    return queue.empty();
}