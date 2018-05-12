#ifndef ESEP_EMP_TOKENIZER_H
#define ESEP_EMP_TOKENIZER_H

#include "lib/utils.h"
#include "lib/tml.h"
#include "lib/member_wrapper.h"

namespace esep
{
	namespace emp
	{
		class Tokenizer
		{
			public:
				MXT_DEFINE_E(MalformedLineException);

				template<typename C>
				static void tokenize(C&& out, const std::string& line,
						std::function<void(tml::DoDecay<C>&, const std::string&)> insert =
								[](tml::DoDecay<C>& c, const std::string& s) { c.push_back(s); })
				{
					std::stringstream buf;

					auto push = [&](void) {
						if(!buf.str().empty())
						{
							std::string s = buf.str();

							buf.str(std::string());

							insert(out, s);
						}
					};

					for(const auto& c : line)
					{
						if(c == '\n')
						{
							MXT_THROW_EX(MalformedLineException);
						}

						if(c == '#')
						{
							break;
						}
						else if(c == ' ' || c == '\t')
						{
							push();
						}
						else
						{
							buf << c;
						}
					}

					push();
				}

			private:
				Tokenizer( ) = delete;
				~Tokenizer( ) = delete;
				Tokenizer(const Tokenizer&) = delete;
				Tokenizer& operator=(const Tokenizer&) = delete;
		};
	}
}

#endif
