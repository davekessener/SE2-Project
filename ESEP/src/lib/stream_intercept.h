#ifndef ESEP_LIB_STREAMINTERCEPT_H
#define ESEP_LIB_STREAMINTERCEPT_H

#include <iostream>
#include <sstream>

namespace esep
{
	namespace lib
	{
		class StreamIntercept : public std::streambuf
		{
			typedef std::function<void(std::streambuf *)> restore_fn;

			public:
				template<typename T>
					StreamIntercept(std::basic_ios<T>&);
				~StreamIntercept( );
				std::string getBuffer( ) { return mSS.str(); }
			protected:
				int_type overflow(int_type = traits_type::eof());

			private:
				std::streambuf *mTarget;
				restore_fn mRestore;
				std::streambuf *mSink;
				std::stringstream mSS;

			private:
				StreamIntercept(const StreamIntercept&) = delete;
				StreamIntercept& operator=(const StreamIntercept&) = delete;
		};

		template<typename T>
		StreamIntercept::StreamIntercept(std::basic_ios<T>& s)
		{
			mRestore = [&s](std::streambuf *p) { s.rdbuf(p); };
			mSink = s.rdbuf(this);
			mTarget = mSS.rdbuf();
		}
	}
}

#endif
