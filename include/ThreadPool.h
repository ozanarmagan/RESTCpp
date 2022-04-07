#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>

namespace restcpp
{

    /**
     * @brief Thread pool implementation to support multiple clients by multithreading
     * 
     */
    class ThreadPool
    {
        public:
            ThreadPool(int n) { m_threads.resize(n); for(int i = 0;i < n;i++) m_threads[i] = std::thread(PoolWorker(this)); }
            template <typename T,typename... Args>
            auto enqueue(T f,Args&&... args)
            {
                using funcType = decltype(f(args...))();
                std::function<funcType> func = std::bind<void>(std::forward<T>(f), std::forward<Args>(args)...);
                auto taskPtr = std::make_shared<std::packaged_task<funcType>>(func);
                if(!m_shutDown)
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_jobs.emplace([taskPtr]() {(*taskPtr)(); });
                    m_condition.notify_one();
                }
                return taskPtr->get_future();
            }
            void shutDown() { m_shutDown = true;}
        private:
            class PoolWorker
            {
                public:
                    PoolWorker(ThreadPool* pool) : m_pool(pool) {    }
                    void operator()() 
                    {
                        
                        while(!m_pool->m_shutDown)
                        {
                            std::function<void()> f;
                            {
                                std::unique_lock<std::mutex> lock(m_pool->m_mutex);
                                if(m_pool->m_jobs.empty())
                                {
                                    if(m_pool->m_shutDown)
                                        break;
                                    m_pool->m_condition.wait(lock);
                                }
                                f = std::move(m_pool->m_jobs.front());
                                m_pool->m_jobs.pop();
                            }
                            f();
                        }
                    }
                private:
                    ThreadPool* m_pool;
            };
            friend class PoolWorker;
            bool m_shutDown = false;
            std::mutex m_mutex;
            std::condition_variable m_condition;
            std::vector<std::thread> m_threads;
            std::queue<std::function<void()>> m_jobs;
    };
    }