#ifndef ESEP_LIB_IO_H
#define ESEP_LIB_IO_H

#include <ios>

namespace esep
{
	namespace lib
	{
		class IOStreamFormat
		{
			public:
				IOStreamFormat(std::ios_base::fmtflags flags, std::streamsize precision, std::streamsize width, std::ios::char_type fill)
					: mFlags(flags), mPrecision(precision), mWidth(width), mFill(fill) { }
				std::ios_base::fmtflags flags( ) const { return mFlags; }
				std::streamsize precision( ) const { return mPrecision; }
				std::streamsize width( ) const { return mWidth; }
				std::ios::char_type fill( ) const { return mFill; }
				void flags(std::ios_base::fmtflags flags) { mFlags = flags; }
				void precision(std::streamsize precision) { mPrecision = precision; }
				void width(std::streamsize width) { mWidth = width; }
				void fill(std::ios::char_type fill) { mFill = fill; }
				template<typename T>
					void apply(T&);
			private:
				std::ios_base::fmtflags mFlags;
				std::streamsize mPrecision, mWidth;
				std::ios::char_type mFill;
		};

		template<typename T>
		class IOStreamFormatBackup
		{
			public:
				IOStreamFormatBackup(T& ios)
					: mIOS(ios) , mFormat(ios.flags(), ios.precision(), ios.width(), ios.fill()) { }
				~IOStreamFormatBackup( )
					{ mFormat.apply(mIOS); }
			private:
				T& mIOS;
				IOStreamFormat mFormat;
		};

		template<typename T>
		void IOStreamFormat::apply(T& ios)
		{
			ios.flags(mFlags);
			ios.precision(mPrecision);
			ios.width(mWidth);
			ios.fill(mFill);
		}
	}
}

#endif

