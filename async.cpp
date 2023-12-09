#include "async.h"



namespace async {

 std::once_flag startFlag;


handle_t connect(std::size_t bulk)
{
    //Эта мапа будет содержать указатели на потоки обработчики сообщений ключ=контекст
    //эта мапа синглтон так как при всех вызовах connect должна использоваться одна и та же мапа
    auto& executor_map=HandleMap<ExecutorThread>::get_instance();

    //Создаем очередь сообщений на логирование
    MyQueue<std::string>& logger_queue=MyQueue<std::string>::get_instance();

    //Создаем очередь сообщений для файловых потоков
    MyQueue<std::pair<std::string,std::string>>& file_queue=MyQueue<std::pair<std::string,std::string>>::get_instance();

    //здесь необходимо создать потоки loger, file1, file2 с помощью call_once (гарантировано единожды)

    std::call_once(startFlag, &ServiceTreads::startServiceThreads, logger_queue, file_queue);

    auto handle=executor_map.find(bulk);
    if (handle==nullptr)
    {
        //Добавляем новый поток обработчик только если такого контекста еще нет в мапе
        SharedExecutorData data;
        data.size=bulk;
        data.logQueue_ptr=&logger_queue;
        data.fileQueue_ptr=&file_queue;



        auto executor=std::shared_ptr<Executor>(new Executor(data));
        std::shared_ptr<StateFactory> factory(new StateFactory);  

        factory->registrate<State_Creator<Simple_Commamd_Queue_State,Executor_ptr>, Executor_ptr> (Simple_Commamd_Queue_State::id, Executor_ptr(executor));
        factory->registrate<State_Creator<Dynamic_Commamd_Queue_State,Executor_ptr>, Executor_ptr> (Dynamic_Commamd_Queue_State::id, Executor_ptr(executor));
        factory->registrate<State_Creator<Dynamic_Commamd_Queue_Nested_Block_State,Executor_ptr>, Executor_ptr> (Dynamic_Commamd_Queue_Nested_Block_State::id, Executor_ptr(executor));

        executor->setFactory(factory);

        //Запускаем поток обработчик
        auto executorthread=std::shared_ptr<ExecutorThread>(new ExecutorThread(executor));
        return executor_map.add(executorthread, bulk);

    }
    return handle;
}


void receive(handle_t handle, const char *data, std::size_t size)
{
    auto& executor_map=HandleMap<ExecutorThread>::get_instance();
    auto ptr=executor_map.get(handle);
    if (ptr==nullptr) return;  //безопасно выходим при негодном дескрипторе
    else ptr->execute(data,size);
}

void disconnect(handle_t handle)
{
    auto& executor_map=HandleMap<ExecutorThread>::get_instance();
    auto ptr=executor_map.get(handle);
    if (ptr==nullptr) return;  //безопасно выходим при негодном дескрипторе
    else
    {
        executor_map.remove(handle);
        ptr->stop(); //останавливаем поток
    }

}

}//end_namespace
