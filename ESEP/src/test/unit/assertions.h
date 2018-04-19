#ifndef ESEP_TEST_UNIT_ASSERTIONS_H
#define ESEP_TEST_UNIT_ASSERTIONS_H

#include "lib/tml.h"
#include "lib/container_wrapper.h"

#define UNIT_TEST(desc) \
	mWriter.name = desc; \
	mWriter = [this](void) -> void


#define ASSERT_EQUALS(a,b) \
	do { \
		auto va = (a); \
		auto vb = (b); \
		if(va != vb) { \
			MXT_THROW_E(std::logic_error, "Expected ", vb, " [" #b "] but got ", va, " [" #a"] instead!"); \
		} } while(0)


#define ASSERT_TRUE(e) \
	do { \
		if(!(e)) { \
			MXT_THROW_E(std::logic_error, "Expession '" #e "' failed!"); \
		} } while(0)

#define ASSERT_FALSE(e) ASSERT_TRUE(!(e))

#define ASSERT_NOT_EQUAL(a,b) ASSERT_TRUE((a)!=(b))


#define ASSERT_EACH_EQUALS(a,b) \
	do { \
		auto aobj(lib::wrap_container(a)); \
		auto bobj(lib::wrap_container(b)); \
		auto i1 = std::begin(aobj), i2 = std::end(aobj); \
		auto j1 = std::begin(bobj); \
		if(aobj.size() != bobj.size()) \
			MXT_THROW_E(std::logic_error, "Container have different sizes: ", aobj.size(), " and ", bobj.size(), "!"); \
		for(size_t i = 0 ; i1 != i2 ; ++i1, ++j1, ++i) { \
			if(*i1 != *j1) \
				MXT_THROW_E(std::logic_error, "Elements @", i, " differ: Expected ", *j1, " but got ", *i1, " instead!"); \
		} } while(0)


#define ASSERT_ANY_FAILURE(e) \
	do { \
		bool success = true; \
		try { \
			e; \
			success = false; \
		} catch(...) { } \
		if(!success) \
			MXT_THROW_E(std::logic_error, "Expected any exception in expression '" #e "' but got none."); \
	} while(0)

#define ASSERT_FAILURE(e,t) \
	do { \
		bool success = true; \
		try { \
			e; \
			success = false; \
		} catch(const ::esep::tml::DoDecay<t>&) { } \
		if(!success) \
			MXT_THROW_E(std::logic_error, "Expected an excpetion of type " #t " in expression '" #e "' but got none."); \
	} while(0)

#endif
