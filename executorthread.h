#ifndef EXECUTORTHREAD_H
#define EXECUTORTHREAD_H
#include "states.h"
#include <thread>
#include <vector>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <iostream>

//Поток, обработчик исходных сообщений. Оснащен входным буфером.
//Для обработки сообщений использует объект типа  Executor,
//который перекочевал сюда из предыдущей домашки практически без изменений
class ExecutorThread
{
    std::shared_ptr<Executor> m_executor;
    std::thread work_thread;

    std::list<std::string> m_queue;

    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_stop;


    void work()
    {
        //поток будет ждать работы на condition_variable из очереди queue
        try {
           // std::cout<<"\nstart executor thread"<<std::this_thread::get_id()<<std::endl;
            auto func=[&](std::unique_lock<std::mutex>& lock) //функция которая будет вызываться при наличии данных в очереди
            {
                auto line=std::move(m_queue.front());
                m_queue.pop_front();
                lock.unlock();
                //std::cout<<"\nwork_"<<line<<std::endl;
                (m_executor)->execute(line);
            };

        //подключаемся к очереди
        while(true)
        {
            std::unique_lock<std::mutex> lock1(m_mutex);
            if (m_queue.size()>0) func(std::ref(lock1)); //если очередь не пуста, рано вставать на ожидание. работаем!
            else //очередь пуста, встаем на ожидание
            {
                if (m_stop) break;
                else
                {
                    m_cv.wait(lock1, [this]{return (m_queue.size()>0);});
                    func(std::ref(lock1));
                }
            }
        }

        }  catch (...) {
            std::cout<<"There is a big trouble: Unexpected exception in executor tread..."<<std::endl;
        }

    }

public:
    ExecutorThread(std::shared_ptr<Executor> exec) : m_executor(exec)
    {
        m_stop=false;
        work_thread=std::thread([this]{work();});
        work_thread.detach();
    }

    void execute(const char* data, size_t size)
    {
        if (m_stop) return;
        std::string temp;

        if ((size==1)&&(*data==EOF)) temp="EOF";
        else
        {
            std::vector<char> vect;
            vect.resize(size+1);
            std::memcpy(vect.data(), data, size);
            vect[size]='\0';
            temp=std::string(vect.data());
        }
        //std::cout<<"\n___com____ "<<temp<<std::endl;
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push_back(temp);
        m_cv.notify_one();

    }

    void stop()
    {
        m_stop=true;
    }

};






#endif // EXECUTORTHREAD_H
