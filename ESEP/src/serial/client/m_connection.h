#ifndef ESEP_SERIAL_CLIENT_MODULE_CONNECTION_H
#define ESEP_SERIAL_CLIENT_MODULE_CONNECTION_H

#include "lib/sync/monitor.h"
#include "serial/connection.h"

#include "types.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Base_Connection
			{
				typedef sync::Monitor<Connection *> sync_t;

				public:
					Base_Connection(Connection *c) : mConnection(c) { }
					virtual ~Base_Connection( ) { }
				protected:
					sync_t& get( ) { return mConnection; }
				private:
					sync_t mConnection;
			};

			class Out_Connection : public virtual Base_Connection
			{
				public:
					Out_Connection(Connection *c) : Base_Connection(c) { }
					virtual void write(const types::buffer_t&);
			};

			class In_Connection : public virtual Base_Connection
			{
				public:
					In_Connection(Connection *c) : Base_Connection(c) { }
					virtual void read(types::buffer_t&, size_t);
					types::buffer_t read(size_t s)
						{ types::buffer_t o; read(o, s); return o; }
					template<typename T>
						T read( )
							{ types::buffer_t o; T r; read(o, sizeof(T)); o >> r; return r; }
			};

			class InOut_Connection : public In_Connection, public Out_Connection
			{
				public:
					InOut_Connection(Connection *c) : Base_Connection(c), In_Connection(c), Out_Connection(c) { }
				private:
			};

			class SeperatedConnection : public InOut_Connection
			{
				public:
					SeperatedConnection(In_Connection& in, Out_Connection& out)
						: Base_Connection(nullptr), InOut_Connection(nullptr), mIn(in), mOut(out) { }
					virtual void write(const types::buffer_t& o) { mOut.write(o); }
					virtual void read(types::buffer_t& b, size_t l) { mIn.read(b, l); }
				private:
					In_Connection& mIn;
					Out_Connection& mOut;
			};
		}
	}
}

#endif
