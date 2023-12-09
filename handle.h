#ifndef HANDLE_H
#define HANDLE_H
#include <vector>
#include <iostream>
#include <map>
#include <memory>



//Самодельная мапа где для пары ключ/значение есть еще и c-style handler расположенный в памяти на постоянном месте
//она позволяет безопасно работать с хандлером вида void*
//даже если хандлер уже не валиден  (или вообще передано произвольное значение адреса) можно будет его использовать
//в вызовах функций без ситуации неопределенного поведения
template<typename  U>
class HandleMap
{
    using T=std::shared_ptr<U>;
    std::map<size_t, T> map;
    std::vector<size_t> vector;

    void push(size_t key, T t)
    {
        auto res=find(0);
        if (res!=nullptr) *res=key;
        else vector.push_back(key);
        map[key]=t;

    }

    HandleMap(){}
    ~HandleMap(){}
    HandleMap(const HandleMap<U>& ){}
    HandleMap& operator=(const HandleMap<U>& ){}


public:

    static HandleMap<U>& get_instance()
    {
        static HandleMap<U> instance;
        return instance;
    }


    size_t* add(T t, size_t key)
    {
        auto res=find(key);
        if (res!=nullptr) return res;
        else
        {
            push(key,t);
            auto res=find(key);
            if (res!=nullptr) return res;
        }
    }

    size_t* find(size_t key)
    {
        for (auto&  x: vector)
        {
            if (x==key) return &x;
        }
        return nullptr;
    }


    void remove(void* handle)
    {
        for (auto& x: vector)
            if (&x==handle)
            {   
                auto it=map.find(x);
                x=0;
                map.erase(it);
            }
    }

    T get(void* handle)
    {
        if ((handle<vector.data())||(handle>vector.data()+sizeof(size_t)*vector.size())) return nullptr;
        if (map.find(*reinterpret_cast<size_t*>(handle))==map.end()) return nullptr;

        return map[*reinterpret_cast<size_t*>(handle)];
    }

};




#endif // HANDLE_H
