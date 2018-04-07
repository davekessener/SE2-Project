#ifndef ESEP_TEST_UNIT_H
#define ESEP_TEST_UNIT_H

#include <string>
#include <functional>
#include <vector>
#include <stdexcept>

#define UNIT_TEST(desc) \
	mWriter.name = desc; \
	mWriter = [this](void) -> void

#define ASSERT_EQUALS(a,b) \
	do { \
		auto va = (a); \
		auto vb = (b); \
		if(va != vb) { \
			MXT_THROW_E(std::logic_error, "Expected ", vb, " but got ", va, " instead!"); \
		} } while(0)

#define ASSERT_EACH_EQUALS(a,b) \
	do { \
		auto i1 = std::begin(a), i2 = std::end(a); \
		auto j1 = std::begin(b), j2 = std::end(b); \
		size_t l1 = 0, l2 = 0; \
		for(auto i = i1 ; i != i2 ; ++i) ++l1; \
		for(auto j = j1 ; j != j2 ; ++j) ++l2; \
		if(l1 != l2) \
			MXT_THROW_E(std::logic_error, "Container have different sizes: ", l1, " and ", l2, "!"); \
		for(size_t i = 0 ; i1 != i2 ; ++i1, ++j1, ++i) { \
			if(*i1 != *j1) \
				MXT_THROW_E(std::logic_error, "Elements @", i, " differ: Expected ", *j1, " but got ", *i1, " instead!"); \
		} } while(0)

#define ASSERT_FAILURE(e) \
	do { \
		bool success = true; \
		try { \
			e; \
			success = false; \
		} catch(...) { } \
		if(!success) \
			MXT_THROW_E(std::logic_error, "Expected an exception but got none."); \
	} while(0)

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
				typedef std::function<void(Result)> result_fn;
				typedef std::vector<std::string> errors_t;

				public:
					TestSuite(const std::string& name) : mWriter(this), mName(name) { }
					virtual ~TestSuite( ) { }
					const std::string& name( ) const { return mName; }
					const errors_t& errors( ) const { return mErrors; }
					void doTest(result_fn);
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
					errors_t mErrors;

					friend class TestWriter;
			};
		}
	}
}

#endif

