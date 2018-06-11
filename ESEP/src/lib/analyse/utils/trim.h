#ifndef ESEP_ANALYSE_UTILS_TRIM_H
#define ESEP_ANALYSE_UTILS_TRIM_H

namespace esep
{
	namespace analyse
	{
		namespace utils
		{
			template<typename I>
			void trim(I& i1, I& i2, double t, size_t n)
			{
				typedef decltype(i1->second) value_type;

				std::deque<value_type> max_buf;

				if(i1 == i2) return;

				for(auto i = i1 ; i != i2 ; ++i)
				{
					if(max_buf.empty()) max_buf.push_back(i->second);
					else if(i->second > max_buf.back())
					{
						for(auto j1 = max_buf.begin(), j2 = max_buf.end() ; j1 != j2 ; ++j1)
						{
							if(i->second > *j1)
							{
								max_buf.insert(j1, i->second);
								while(max_buf.size() > n) max_buf.pop_back();
								break;
							}
						}
					}
				}

				value_type max = 0, rem = 0;

				for(const auto& e : max_buf)
				{
					rem += e % n;
					max += rem / n;
					rem %= n;
					max += e / n;
				}

				if(rem > n / 2) ++max;

				max *= t;

				auto i = i2;

				while(i1 != i2) while(i1->second < max) ++i1;
				while(i1 != i2) while((--i)->second < max) i2 = i;
			}
		}
	}
}

#endif
