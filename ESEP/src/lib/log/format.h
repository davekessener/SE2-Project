#ifndef ESEP_LOG_FORMAT_H
#define ESEP_LOG_FORMAT_H

#include <string>
#include <vector>
#include <sstream>
#include <functional>

#include "lib/utils.h"
#include "lib/tml.h"
#include "lib/tuple.h"
#include "lib/function.h"

namespace esep
{
	namespace log
	{
		namespace format
		{
			template<typename ... L>
			class Collection
			{
				template<typename T>
				struct MakeFormatter
				{
					typedef std::function<std::string(const tml::DoDecay<T>&)> Type;
				};

				typedef lib::Tuple<tml::DoApply<tml::MakeTypeList<L...>, MakeFormatter>> tuple_t;

				public:
					template<typename T>
					void set(T&& f)
					{
						typedef lib::FunctionTraits<tml::DoDecay<T>> function_traits;
						typedef tml::DoGet<typename function_traits::argument_types, 0> argument_type;
						typedef typename MakeFormatter<argument_type>::Type reference_type;

						mFormatters.template get<reference_type>() = f;
					}

					template<typename T>
					std::string apply(T&& a)
					{
						typedef typename MakeFormatter<T>::Type reference_type;

						return mFormatters.template get<reference_type>()(a);
					}

					template<typename T>
					std::string format(size_t i, const T& o)
					{
						if(i)
							MXT_THROW("Format collection has ", i, " too few types.");

						return apply(o);
					}

					template<typename T, typename ... TT>
					std::string format(size_t i, const T& o, const TT& ... a)
					{
						return i ? format(i - 1, a...) : apply(o);
					}
				private:
					tuple_t mFormatters;
			};

			class Parser
			{
				enum class State
				{
					IDLE,
					ANTICIPATING,
					SPACING,
					PROCESSING
				};

				public:
					template<size_t N>
						Parser(const char * const (&t)[N])
							: mTable(t), mN(N), mState(State::IDLE), mSpacing(0) { }
					void parse(char);
					template<typename T>
						void parseAll(T&& o)
							{ for(const char *i = &o[0] ; *i ; ++i) parse(*i); parse('\0'); }

					std::vector<std::string> literals;
					std::vector<std::pair<int, uint>> format;
				private:
					State onIdle(char);
					State onAnticipating(char);
					State onSpacing(char);
					State onProcessing(char);
					void push( );
				private:
					const char * const * const mTable;
					const size_t mN;
					State mState;
					uint mSpacing;
					std::stringstream mSS, mBak, mID;
			};
		}
	}
}

#endif
