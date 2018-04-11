#ifndef ESEP_LIB_STREAMINTERCEPT_H
#define ESEP_LIB_STREAMINTERCEPT_H

#include <iostream>

namespace esep
{
	namespace lib
	{
		class StreamIntercept : public std::streambuf
		{
			typedef std::function<void(std::streambuf *)> restore_fn;

			public:
				template<typename T>
					StreamIntercept(std::basic_ios<T>&, std::basic_ios<T>&);
				~StreamIntercept( );
			protected:
				int_type overflow(int_type = traits_type::eof());

			private:
				std::streambuf *mTarget;
				restore_fn mRestore;
				std::streambuf *mSink;

			private:
				StreamIntercept(const StreamIntercept&) = delete;
				StreamIntercept& operator=(const StreamIntercept&) = delete;
		};

		template<typename T>
		StreamIntercept::StreamIntercept(std::basic_ios<T>& t, std::basic_ios<T>& s)
			: mTarget(t.rdbuf())
		{
			mRestore = [&s](std::streambuf *p) { s.rdbuf(p); };
			mSink = s.rdbuf(this);
		}
	}
}

#endif
