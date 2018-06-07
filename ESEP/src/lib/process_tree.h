#ifndef ESEP_LIB_PROCESSTREE_H
#define ESEP_LIB_PROCESSTREE_H

#include "lib/tml.h"
#include "lib/function.h"

namespace esep
{
	namespace lib
	{
		namespace data
		{
			template<typename T>
			struct Tagger
			{
				template<T V, typename TT>
				struct Tag
				{
					typedef TT Type;
					static constexpr T Value = V;
				};
			};

			template<typename T, typename ... TT>
			struct Source
			{
			};

			namespace impl
			{
				typedef tml::Nil Nil;

				template<typename T>
				struct Flatten
				{
					typedef tml::TypeList<T, Nil> Type;
				};

				template<typename T, typename ... TT>
				struct Flatten<Source<T, TT...>>
				{
					typedef tml::TypeList<T, typename Flatten<tml::MakeTypeList<TT...>>::Type> Type;
				};

				template<typename H, typename T>
				struct Flatten<tml::TypeList<H, T>>
				{
					typedef tml::list::DoAppend<typename Flatten<H>::Type, typename Flatten<T>::Type> Type;
				};

				template<>
				struct Flatten<Nil>
				{
					typedef Nil Type;
				};
			}

			template
			<
				typename Tag,
				typename Data,
				typename Tree
			>
			class ProcessTree
			{
				typedef tml::Nil Nil;

				template<Tag V, typename T>
				using Tagger = typename Tagger<Tag>::template Tag<V, T>;

				struct Assemble
				{
					template<typename TTag, typename L> struct Filter;
					template<typename TTag, typename H, typename T> struct Filter<TTag, tml::TypeList<H, T>> : Filter<Tag, T> { };
					template<typename TTag> struct Filter<TTag, Nil> : tml::Type2Type<Nil> { };
					template<typename TTag, Tag V, typename T, typename TT>
					struct Filter<
						TTag,
						tml::TypeList<
							Tagger<V, T>,
							TT>>
					{
						typedef tml::TypeList<
							Tagger<V, T>,
							typename Filter<TTag, TT>::Type>
						Type;
					};

					typedef typename impl::Flatten<Tree>::Type flat_t;
					typedef typename Filter<Tag, flat_t>::Type Type;

				};

				typedef typename Assemble::Type tag_map_t;

				template<Tag T>
				struct FindByTag
				{
					template<typename TT> struct Apply : tml::False { };
					template<typename TT> struct Apply<Tagger<T, TT>> : tml::True { };
				};

				template<typename F>
				class Holder
				{
					public:
					typedef FunctionTraits<F> traits_t;
					typedef tml::DoDecay<tml::list::DoGet<typename traits_t::argument_types, 0>> argument_type;
					typedef typename traits_t::return_type value_type;
					typedef std::function<argument_type(void)> load_fn;

					public:
						Holder(load_fn f) : mLoader(std::move(f)) { }
						const value_type& get( )
						{
							if(!static_cast<bool>(mObj))
							{
								mObj.reset(new value_type(F()(mLoader())));
							}

							return *mObj;
						}

					private:
						load_fn mLoader;
						std::unique_ptr<value_type> mObj;
				};

				template<Tag T>
				struct TaggedHolder : Holder<typename tml::list::DoFindFirst<tag_map_t, FindByTag<T>>::Type>
				{
					typedef Holder<typename tml::list::DoFindFirst<tag_map_t, FindByTag<T>>::Type> Super;

					template<typename TT> TaggedHolder(TT&& f) : Super(std::forward<TT>(f)) { }
				};

				template<typename T, typename>
				struct AnonHolder : Holder<T>
				{
					template<typename TT> AnonHolder(TT&& f) : Holder<T>(std::forward<TT>(f)) { }
				};

				template<typename F, typename L> struct HolderHelper { typedef AnonHolder<F, L> Type; };
				template<Tag V, typename T, typename L> struct HolderHelper<Tagger<V, T>, L> { typedef TaggedHolder<V> Type; };

				template<typename L, typename F>
				struct Impl : HolderHelper<F, Impl<L, F>>::Type
				{
					typedef typename HolderHelper<F, Impl<L, F>>::Type Super;
					typedef typename Super::load_fn load_fn;

					Impl(load_fn f) : Super(std::move(f)) { }
				};

				template<typename L, typename F, typename ... T>
				struct Impl<L, Source<F, T...>> : HolderHelper<F, Impl<L, Source<F, T...>>>::Type, Impl<tml::TypeList<F, L>, T>...
				{
					typedef typename HolderHelper<F, Impl<L, Source<F, T...>>>::Type Super;
					typedef typename Super::load_fn load_fn;
					typedef std::function<typename Super::value_type(void)> forward_fn;

					Impl(load_fn f) : Super(std::move(f)), Impl<tml::TypeList<F, L>, T>(genCB())... { }

					forward_fn genCB( ) { return [this](void) { return Super::get(); }; }
				};

				typedef Impl<Nil, Tree> container_t;

				public:
					ProcessTree(const Data& v) : mData(v), mContainer([this](void) { return mData; }) { }
					template<Tag T>
					const typename TaggedHolder<T>::value_type& get( ) { return mContainer.TaggedHolder<T>::get(); }
				private:
					Data mData;
					container_t mContainer;
			};
		}
	}
}

#endif
