#ifndef ESEP_ANALYSE_V_H
#define ESEP_ANALYSE_V_H

#include <array>
#include <iostream>

#include "lib/utils.h"

#include "lib/analyse/algorithm.h"

namespace esep
{
	namespace analyse
	{
		template<typename T, size_t N>
		class V
		{
			public:
			typedef V<T, N> Self;
			typedef T value_type;
			typedef std::array<value_type, N> container_type;
			typedef value_type *iterator;
			typedef const value_type *const_iterator;

			MXT_DEFINE_E(IndexViolationException);

			public:
				V( ) { for(uint i = 0 ; i < N ; ++i) mV[i] = 0; }
				template<typename TT>
				V(const V<TT, N>& v) { for(uint i = 0 ; i < N ; ++i) mV[i] = v[i]; }
				template<typename ... TT>
				V(value_type v1, TT&& ... v) { setAll(0, v1, std::forward<TT>(v)...); }

				template<typename TT>
				V<T, N>& operator=(const V<TT, N>& v) { for(uint i = 0 ; i < N ; ++i) mV[i] = v[i]; return *this; }

				value_type& operator[](int i) { while(i < 0) i += N; return mV[i % N]; }
				value_type& at(int i) { if(i < 0 || (size_t)i >= N) MXT_THROW_EX(IndexViolationException); return mV[i]; }

				const value_type& operator[](int i) const { while(i < 0) i += N; return mV[i % N]; }
				const value_type& at(int i) const { if(i < 0 || (size_t)i >= N) MXT_THROW_EX(IndexViolationException); return mV[i]; }

				value_type abs_sqr( ) const { T r = 0; for(uint i = 0 ; i < N ; ++i) r += mV[i] * mV[i]; return r; }
				value_type abs( ) const { return sqrt(abs_sqr()); }

				value_type& x( ) { return mV[0]; }
				value_type& y( ) { return mV[1]; }
				value_type& z( ) { return mV[2]; }

				const value_type& x( ) const { return mV[0]; }
				const value_type& y( ) const { return mV[1]; }
				const value_type& z( ) const { return mV[2]; }

				template<typename TT>
				Self& operator+=(const V<TT, N>& v) { for(uint i = 0 ; i < N ; ++i) mV[i] += v[i]; return *this; }

				template<typename TT>
				Self& operator-=(const V<TT, N>& v) { for(uint i = 0 ; i < N ; ++i) mV[i] -= v[i]; return *this; }

				Self& operator/=(const value_type& s) { for(auto& p : *this) p /= s; return *this; }
				Self& operator*=(const value_type& s) { for(auto& p : *this) p *= s; return *this; }

				iterator begin( ) { return mV.begin(); }
				iterator end( ) { return mV.end(); }
				const_iterator begin( ) const { return mV.begin(); }
				const_iterator end( ) const { return mV.end(); }
				const_iterator cbegin( ) const { return begin(); }
				const_iterator cend( ) const { return end(); }

			private:
				template<typename ... TT>
				void setAll(size_t i, value_type v, TT&& ... a)
				{
					mV[i] = v;

					setAll(i + 1, a...);
				}

				void setAll(size_t) { }


			private:
				container_type mV;
		};

		template<typename T1, typename T2, size_t N, typename T = tml::DoPromotion<T1, T2>>
		V<T, N> operator+(const V<T1, N>& v1, const V<T2, N>& v2)
		{
			V<T, N> r(v1);

			return r += v2;
		}

		template<typename T1, typename T2, size_t N, typename T = tml::DoPromotion<T1, T2>>
		V<T, N> operator-(const V<T1, N>& v1, const V<T2, N>& v2)
		{
			V<T, N> r(v1);

			return r -= v2;
		}

		typedef V<double, 2> vec2;
		typedef V<double, 3> vec3;
	}
}

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const esep::analyse::V<T, N>& v)
{
	bool first = true;

	for(const auto& e : v)
	{
		os << (first ? '(' : '/') << e; first = false;
	}

	return os << ')';
}

#endif
