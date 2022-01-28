#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

using std::function;
using std::vector;
using std::thread;
using std::queue;
using std::mutex;
using std::condition_variable;

class ThreadPool
{
    public:
        ThreadPool(int n) { mThreads.resize(n); for(int i = 0;i < n;i++) mThreads[i] = thread(PoolWorker(this)); }
        template <typename T>
        void enqueue(T f)
        {
            mJobs.push(f);
            mCondition.notify_one();
        }
    private:
        class PoolWorker
        {
            public:
                PoolWorker(ThreadPool* pool) : mPool(pool) {    }
                void operator()() 
                {
                    function<void()> f;
                    while(!mPool->mShutDown)
                    {
                        std::unique_lock<mutex> lock(mPool->mMutex);
                        if(mPool->mJobs.empty())
                            mPool->mCondition.wait(lock);
                        f = mPool->mJobs.front();
                        mPool->mJobs.pop();
                        f();
                    }
                }
            private:
                ThreadPool* mPool;
        };
        friend class PoolWorker;
        bool mShutDown = false;
        mutex mMutex;
        condition_variable mCondition;
        vector<thread> mThreads;
        queue<function<void()>> mJobs;
};