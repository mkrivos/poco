//
// TaskManager.cpp
//
// Library: Foundation
// Package: Tasks
// Module:  Tasks
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/TaskManager.h"
#include "Poco/TaskNotification.h"


namespace Poco {


const int TaskManager::MIN_PROGRESS_NOTIFICATION_INTERVAL = 100000; // 100 milliseconds


TaskManager::TaskManager(ThreadPool::ThreadAffinityPolicy affinityPolicy):
	_threadPool(ThreadPool::defaultPool(affinityPolicy))
{
}


TaskManager::TaskManager(ThreadPool& pool):
	_threadPool(pool)
{
}


TaskManager::~TaskManager()
{
}


void TaskManager::start(Task* pTask, int cpu)
{
	TaskPtr pAutoTask(pTask); // take ownership immediately
	FastMutex::ScopedLock lock(_mutex);

	pAutoTask->setOwner(this);
	pAutoTask->setState(Task::TASK_STARTING);
	_taskList.push_back(pAutoTask);
	try
	{
		_threadPool.start(*pAutoTask, pAutoTask->name(), cpu);
	}
	catch (...)
	{
		// Make sure that we don't act like we own the task since
		// we never started it.  If we leave the task on our task
		// list, the size of the list is incorrect.
		_taskList.pop_back();
		throw;
	}
}


void TaskManager::startSync(Task* pTask)
{
	TaskPtr pAutoTask(pTask); // take ownership immediately
	ScopedLockWithUnlock<FastMutex> lock(_mutex);

	pAutoTask->setOwner(this);
	pAutoTask->setState(Task::TASK_STARTING);
	_taskList.push_back(pAutoTask);
	lock.unlock();
	try
	{
		pAutoTask->run();
	}
	catch (...)
	{
		FastMutex::ScopedLock miniLock(_mutex);

		// Make sure that we don't act like we own the task since
		// we never started it.  If we leave the task on our task
		// list, the size of the list is incorrect.
		_taskList.pop_back();
		throw;
	}
}


void TaskManager::cancelAll()
{
	FastMutex::ScopedLock lock(_mutex);

	for (TaskList::iterator it = _taskList.begin(); it != _taskList.end(); ++it)
	{
		(*it)->cancel();
	}
}


void TaskManager::joinAll()
{
	_threadPool.joinAll();
}


TaskManager::TaskList TaskManager::taskList() const
{
	FastMutex::ScopedLock lock(_mutex);
	
	return _taskList;
}


void TaskManager::addObserver(const AbstractObserver& observer)
{
	_nc.addObserver(observer);
}


void TaskManager::removeObserver(const AbstractObserver& observer)
{
	_nc.removeObserver(observer);
}


void TaskManager::postNotification(const Notification::Ptr& pNf)
{
	_nc.postNotification(pNf);
}


void TaskManager::taskStarted(Task* pTask)
{
	_nc.postNotification(AutoPtr<TaskStartedNotification>(new TaskStartedNotification(pTask)));
}


void TaskManager::taskProgress(Task* pTask, float progress)
{
	ScopedLockWithUnlock<FastMutex> lock(_mutex);

	if (_lastProgressNotification.isElapsed(MIN_PROGRESS_NOTIFICATION_INTERVAL))
	{
		_lastProgressNotification.update();
		lock.unlock();
		_nc.postNotification(AutoPtr<TaskProgressNotification>(new TaskProgressNotification(pTask, progress)));
	}
}


void TaskManager::taskCancelled(Task* pTask)
{
	_nc.postNotification(AutoPtr<TaskCancelledNotification>(new TaskCancelledNotification(pTask)));
}


void TaskManager::taskFinished(Task* pTask)
{
	TaskPtr currentTask;
	ScopedLockWithUnlock<FastMutex> lock(_mutex);
	
	for (TaskList::iterator it = _taskList.begin(); it != _taskList.end(); ++it)
	{
		if (*it == pTask)
		{
			currentTask = *it;
			_taskList.erase(it);
			break;
		}
	}
	lock.unlock();

	_nc.postNotification(AutoPtr<TaskFinishedNotification>(new TaskFinishedNotification(pTask)));
}


void TaskManager::taskFailed(Task* pTask, const Exception& exc)
{
	_nc.postNotification(AutoPtr<TaskFailedNotification>(new TaskFailedNotification(pTask, exc)));
}


} // namespace Poco
