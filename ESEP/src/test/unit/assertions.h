#ifndef ESEP_TEST_UNIT_ASSERTIONS_H
#define ESEP_TEST_UNIT_ASSERTIONS_H

#define UNIT_TEST(desc) \
	mWriter.name = desc; \
	mWriter = [&,this](void) -> void


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


#define ASSERT_NOT_EQUAL(a,b) ASSERT_TRUE((a)!=(b))


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


#endif
