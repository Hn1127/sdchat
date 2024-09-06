#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>
#include <mutex>
#include <memory>

using namespace std;

template<typename T>
class Singleton{
public:
    static shared_ptr<T> GetInstance(){
        static once_flag s_flag;
        call_once(s_flag,[&]{
            _instance = shared_ptr<T>(new T);
        });
        return _instance;
    }
protected:
    Singleton()=default;
    Singleton(const Singleton<T>&)=delete;
    Singleton& operator=(const Singleton<T>& st)=delete;
    static shared_ptr<T> _instance;
};

template<typename T>
shared_ptr<T> Singleton<T>::_instance =nullptr;

#endif // SINGLETON_H
