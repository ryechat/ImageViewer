#pragma once
#ifndef GUID_D634731E0CE3493DAA94DF5577BC181B
#define GUID_D634731E0CE3493DAA94DF5577BC181B

#ifndef STDAFX_H
#include <wtypes.h>
#include <functional>
#endif

namespace basis {

/*! Thread.
    The thread suspends itself after it created.
    The Memeber function, addTask(), lets the thread work on the task.
    If the thread already had have another task, addTask function stores
    a task to inner list. The thread then keep on working until it 
    finished all of tasks.
    
    The thread will alive until the memberfunction, join(), would be called.
    Destructor will call join() in any way.

    All member functions are thread-safe.
*/
class CThread {
public:
    CThread();
    CThread(CThread&) = delete;
    CThread& operator=(CThread&) = delete;
    CThread(CThread&&) = default;
    CThread& operator=(CThread&&) = default;
    ~CThread();

    // Whether the thread is alive.
    explicit operator bool() { return impl != nullptr; }

    //! Original return code for threads of this class.
    static unsigned constexpr EXITCODE = 1u;
    
    using TaskTy = std::function<void(void)>;

    /*! Adds a task for the thread.
        If the thread was not busy, it starts to work on the task.
        @sa CThread
    */
    void addTask(TaskTy f);
    
    /*! Tells the thread to end.
        @note
        All functions despite destructor will go to    not work;
        but it's safe to call, once this function returned.
        @sa CThread
    */
    void join();
    
    /*! Waits the thread suspended.
        This function returns true immediately
        if the thread is not in busy or already joined.
        It also returns true if the thread finished
        all tasks before specified time passed.
        If the thread still has been working 
        after specified time passed,
        then this function returns false.
        The thread suspend itself only when finished all tasks.
        @param timeToWait Maximum time to wait. If this is set
        to be negative value, this function returns control
        when the thread is suspended regardless the time.
    */
    bool wait(int timeToWait) const noexcept;

    //! Returns true if the thread is running.
    //! This function returns !wait(0).
    bool isBusy() noexcept { return !wait(0); }

    /*!    Returns a handle for waiting this thread suspended.
        Returned handle can be used as a parameter of WaitForMultipleObjects().
    */
    HANDLE getWaitHandle();

    /*!    Returns a handle of this thread.
        The handle this function returns can be used to wait thead terminated.
    */
    HANDLE getThreadHandle();

private:
    class Impl;
    Impl *impl;
};

}  // namespace

#endif