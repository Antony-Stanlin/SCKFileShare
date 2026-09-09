#pragma once

#include <thread>


class Thread
{

private:

    std::thread thread;


public:

    template <typename Function, typename Object>
    void Start(Function function,Object* object){

        thread = std::thread(function,object);
    }


    void Join(){

        if (thread.joinable())
            thread.join();
    }

};