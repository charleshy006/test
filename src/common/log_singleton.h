#ifndef LOG_SINGLETON_H_
#define LOG_SINGLETON_H_


#include <stdio.h>
#include <iostream>
#include <pthread.h>

namespace uuzu{ namespace logsdk{

template<typename T>
class Singleton
{
public:
	//创建实例
    static T& createInstance()
	{
		return getInstance();
	}
	//获取实例
	static T& getInstance()
	{
        pthread_mutex_lock(&mutex);
        if (NULL == ms_pInstance)
		{
            //garbo;
            ms_pInstance = new T();
		}
        pthread_mutex_unlock(&mutex);

		return *ms_pInstance;
	}
	//销毁实例
	static void destroyInstance()
	{
        pthread_mutex_lock(&mutex);
        if (NULL != ms_pInstance)
		{
			delete ms_pInstance;
			ms_pInstance = NULL;
		}
        pthread_mutex_unlock(&mutex);
	}

	//将用新的实例将旧的实例换出来
	static T* swapInstance(T* new_one)
	{
        pthread_mutex_lock(&mutex);
        T* old_one = ms_pInstance;
		ms_pInstance = new_one;
        pthread_mutex_unlock(&mutex);
		return old_one;
	}

protected:
	static T	*ms_pInstance;
    static pthread_mutex_t mutex;

    class Garbo
    {
    public:
        ~Garbo()
        {
            pthread_mutex_lock(&mutex);
            if (Singleton::ms_pInstance)
            {
                //std::cout << "Garbo dtor" << std::endl;
                delete Singleton::ms_pInstance;
            }
            pthread_mutex_unlock(&mutex);
        }
    };

    static Garbo garbo;

protected:
	Singleton()
	{
	}
	virtual ~Singleton()
	{
	}

private:
    // Prohibited actions
    Singleton(const Singleton &);
    Singleton& operator = (const Singleton &);

};

template<typename T>
T* Singleton<T>::ms_pInstance = NULL;

template<typename T>
pthread_mutex_t Singleton<T>::mutex;

template<typename T>
typename Singleton<T>::Garbo Singleton<T>::garbo;

}}
#endif /* LOG_SINGLETON_H_ */
