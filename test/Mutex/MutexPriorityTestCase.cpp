/**
 * \file
 * \brief MutexPriorityTestCase class implementation
 *
 * \author Copyright (C) 2014 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * \date 2014-10-27
 */

#include "MutexPriorityTestCase.hpp"

#include "priorityTestPhases.hpp"
#include "SequenceAsserter.hpp"

#include "distortos/StaticThread.hpp"
#include "distortos/Mutex.hpp"

namespace distortos
{

namespace test
{

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| local constants
+---------------------------------------------------------------------------------------------------------------------*/

/// size of stack for test thread, bytes
constexpr size_t testThreadStackSize {256};

/*---------------------------------------------------------------------------------------------------------------------+
| local functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

void thread(SequenceAsserter& sequenceAsserter, unsigned int sequencePoint, Mutex& mutex);

/*---------------------------------------------------------------------------------------------------------------------+
| local types
+---------------------------------------------------------------------------------------------------------------------*/

/// type of test thread
using TestThread = decltype(scheduler::makeStaticThread<testThreadStackSize>({}, thread,
		std::ref(std::declval<SequenceAsserter&>()), std::declval<unsigned int>(), std::ref(std::declval<Mutex&>())));

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Test thread.
 *
 * Locks the mutex, marks the sequence point in SequenceAsserter and unlocks the mutex.
 *
 * \param [in] sequenceAsserter is a reference to SequenceAsserter shared object
 * \param [in] sequencePoint is the sequence point of this instance
 * \param [in] mutex is a reference to shared mutex
 */

void thread(SequenceAsserter& sequenceAsserter, const unsigned int sequencePoint, Mutex& mutex)
{
	mutex.lock();
	sequenceAsserter.sequencePoint(sequencePoint);
	mutex.unlock();
}

/**
 * \brief Builder of TestThread objects.
 *
 * \param [in] threadParameters is a reference to ThreadParameters object
 * \param [in] sequenceAsserter is a reference to SequenceAsserter shared object
 * \param [in] mutex is a reference to shared mutex
 *
 * \return constructed TestThread object
 */

TestThread makeTestThread(const ThreadParameters& threadParameters, SequenceAsserter& sequenceAsserter, Mutex& mutex)
{
	return scheduler::makeStaticThread<testThreadStackSize>(threadParameters.first, thread,
			std::ref(sequenceAsserter), static_cast<unsigned int>(threadParameters.second), std::ref(mutex));
}

}	// namespace

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

bool MutexPriorityTestCase::run_() const
{
	for (const auto& phase : priorityTestPhases)
	{
		SequenceAsserter sequenceAsserter;
		Mutex mutex;

		std::array<TestThread, totalThreads> threads
		{{
				makeTestThread(phase.first[phase.second[0]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[1]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[2]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[3]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[4]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[5]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[6]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[7]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[8]], sequenceAsserter, mutex),
				makeTestThread(phase.first[phase.second[9]], sequenceAsserter, mutex),
		}};

		mutex.lock();

		for (auto& thread : threads)
			thread.start();

		mutex.unlock();

		for (auto& thread : threads)
			thread.join();

		if (sequenceAsserter.assertSequence(totalThreads) == false)
			return false;
	}

	return true;
}

}	// namespace test

}	// namespace distortos
