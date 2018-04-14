#ifndef ESEP_TEST_UNIT_H
#define ESEP_TEST_UNIT_H

#include <string>
#include <functional>
#include <vector>
#include <stdexcept>

#include "lib/utils.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class TestSuite
			{
				struct TestWriter
				{
					TestWriter(TestSuite *ts) : ts(ts) { }
					template<typename T>
						void operator=(T&& f)
							{ ts->addTest(name, f); }
					TestSuite * const ts;
					std::string name;
				};
				public:
				enum class Result
				{
					SUCCESS,
					FAILURE
				};

				typedef std::function<void(void)> cb_fn;
				typedef std::vector<std::pair<Result, std::string>> result_t;

				public:
					TestSuite(const std::string& name) : mWriter(this), mName(name) { }
					virtual ~TestSuite( ) { }
					const std::string& name( ) const { return mName; }
					const result_t& results( ) const { return mResults; }
					uint testCount( ) const { return mTests.size(); }
					void doTest( );
				protected:
					virtual void setup( ) { }
					virtual void teardown( ) { }
					virtual void define( ) = 0;
					TestWriter mWriter;
				private:
					void addTest(const std::string& s, cb_fn f)
						{ mTests.push_back(std::make_pair(s, f)); }

				private:
					const std::string mName;
					std::vector<std::pair<std::string, cb_fn>> mTests;
					result_t mResults;

					friend class TestWriter;
			};
		}
	}
}

#endif

