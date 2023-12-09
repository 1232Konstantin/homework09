#ifndef EXECUTER_H
#define EXECUTER_H
#include <string>
#include <memory>
#include <list>
#include <iostream>
#include "statefactory.h"
#include <chrono>
#include <ctime>

#include "myqueue.h"

using CommandList_ptr=std::shared_ptr<std::list<std::pair<std::string,std::string>>>;

//Данные executor необходимые для кастомизации состояний
//Меняются в зависимости от протокола информационного обмена
typedef struct
{
    size_t size; //размер блока
    MyQueue<std::string> *logQueue_ptr; //адрес очереди сообщений для потока loger
    MyQueue<std::pair<std::string, std::string>> *fileQueue_ptr; //адрес очереди сообщений для потоков file1 и file2
} SharedExecutorData;


//Класс исполнитель команд
//остается неизменным при кастомизации состояний, но исходное состояние должно называться "FIRST_STATE"
class Executor
{
   std::unique_ptr<BaseState> m_state;
   std::shared_ptr<StateFactory> m_factory;

   CommandList_ptr m_command_list;
   SharedExecutorData m_data;

public:
   Executor(SharedExecutorData data): m_data(data)
   {
       m_command_list=CommandList_ptr(new std::list<std::pair<std::string, std::string>>());
   }

   void setFactory(std::shared_ptr<StateFactory>& factory)
   {
       m_factory=std::move(factory);
       setState("FIRST_STATE");
   }

   void setState(std::string state)
   {
        m_state=m_factory->create(state);

   }
   CommandList_ptr getCommandList()
   {
       return m_command_list;
   }

   SharedExecutorData& getExecutorData() {return m_data;}

   void  execute(std::string command) {
       //суперское преобразование текущего момента времени в строку
       auto now=std::chrono::steady_clock::now();
       auto timeSinceEpoch=now.time_since_epoch();
       auto seconds=std::chrono::duration_cast<std::chrono::seconds>(timeSinceEpoch);
       auto nanoseconds=std::chrono::duration_cast<std::chrono::nanoseconds>(timeSinceEpoch-seconds);
       std::string timeString=std::to_string(seconds.count())+std::to_string(nanoseconds.count());
       m_state->execute(command, timeString);

   }

   ~Executor()=default;
};

using Executor_ptr=std::weak_ptr<Executor>;





#endif // EXECUTER_H
