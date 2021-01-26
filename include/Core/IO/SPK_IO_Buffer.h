//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////

#ifndef H_SPK_IOBUFFER
#define H_SPK_IOBUFFER

namespace SPK
{
namespace IO
{
	class SPK_PREFIX Buffer
	{
	public :

		Buffer(size_t initialCapacity);
		Buffer(size_t,std::istream& is);
		~Buffer();

		const char* getData() const	{ return buf; }

		////////////////////
		// Size operators //
		////////////////////

		size_t getSize() const				{ return size; }
		size_t getCapacity() const			{ return capacity; }
		size_t getPosition() const			{ return position; }

		void skip(size_t nb) const			{ position += nb; }
		void setPosition(size_t pos) const	{ position = pos; }
		void clear()						{ position = size = 0; }

		bool isAtEnd() const				{ return position >= size; }

		//////////////////////////////
		// Primitive get operations //
		//////////////////////////////

		const char* get(size_t length) const;

		int32 get32() const
		{
			if (USE_LITTLE_ENDIANS)
				return *reinterpret_cast<const int32*>(get(4));
			else
				return swap32(*reinterpret_cast<const int32*>(get(4)));
		}

		template<class T> T get() const
		{
			return GetHelper<T>::get(this);
		}

		//////////////////////////////
		// Primitive put operations //
		//////////////////////////////

		void put(char c);
		void put(const char* c, size_t length);

		void put32(int32 i)
		{
			if (USE_LITTLE_ENDIANS)
				put(reinterpret_cast<char*>(&i),4);
			else
				put(reinterpret_cast<char*>(swap32(i)),4);
		}

		////////////////////////////
		// Generic put operations //
		////////////////////////////

		void put(unsigned char c)	{ put((char)c); }
		void put(signed char c)		{ put((char)c); }
		void put(float f)			{ put32(*reinterpret_cast<int32*>(&f)); }
		void put(size_t i)			{ put32(*reinterpret_cast<int32*>(&i)); }
		void put(uint32 i)			{ put32(*reinterpret_cast<int32*>(&i)); }
		void put(int32 i)			{ put32(i); }
		void put(std::string s)		{ put(s.data(),s.size()); put('\0'); }
		void put(const Vector3D& v)	{ put(v.x); put(v.y); put(v.z); }
		void put(const Color& c)	{ put32(*reinterpret_cast<const int32*>(&c)); }
		void put(bool b)			{ put(static_cast<char>(b ? 0x01 : 0x00)); }

		template<typename T>
		void put(const std::vector<T>& t)
		{
			put(static_cast<uint32>(t.size()));
			for (size_t i = 0; i < t.size(); ++i)
				put(t[i]);
		}

		template<typename T>
		void put(const Pair<T>& value)
		{
			put(value.value1);
			put(value.value2);
		}

		template<typename T>
		void put(const Triplet<T>& value)
		{
			put(value.value1);
			put(value.value2);
			put(value.value3);
		}

		template<typename T>
		void put(const Quadruplet<T>& value)
		{
			put(value.value1);
			put(value.value2);
			put(value.value3);
			put(value.value4);
		}

		void putBuffer(const Buffer& buffer)
		{
			put(buffer.getData(), buffer.getSize());
		}

	private:
		template<typename T> struct GetHelper {};

		template<typename T>
		struct GetHelper<Pair<T> >
		{
			static Pair<T> get(const Buffer* buffer)
			{
				T v1 = buffer->get<T>();
				T v2 = buffer->get<T>();
				return Pair<T>(v1, v2);
			}
		};

		template<typename T>
		struct GetHelper<Triplet<T> >
		{
			static Triplet<T> get(const Buffer* buffer)
			{
				T v1 = buffer->get<T>();
				T v2 = buffer->get<T>();
				T v3 = buffer->get<T>();
				return Triplet<T>(v1, v2, v3);
			}
		};

		template<typename T>
		struct GetHelper<Quadruplet<T> >
		{
			static Quadruplet<T> get(const Buffer* buffer)
			{
				T v1 = buffer->get<T>();
				T v2 = buffer->get<T>();
				T v3 = buffer->get<T>();
				T v4 = buffer->get<T>();
				return Quadruplet<T>(v1, v2, v3, v4);
			}
		};

		template<typename T>
		struct GetHelper<std::vector<T> >
		{
			static std::vector<T> get(const Buffer* buffer)
			{
				unsigned int size = buffer->get<uint32>();
				std::vector<T> tmp;
				for(size_t t = 0; t < size; t++)
					tmp.push_back(buffer->get<T>());
				return tmp;
			}
		};

	private:
		Buffer(const Buffer& buffer);

		static const bool USE_LITTLE_ENDIANS;

		char* buf;
		size_t capacity;
		size_t size;
		mutable size_t position;

		static int32 swap32(int32 i)
		{
			return ((i & 0xFF000000 >> 24) & 0xFF) |
					(i & 0x00FF0000 >> 8) |
					(i & 0x0000FF00 << 8) |
					(i & 0x000000FF << 24) ;
		}

		void updateSize(size_t newPosition);

		static bool isLittleEndians();
	};

	////////////////////////////
	// Generic put operations //
	////////////////////////////
	template<typename T>
	Buffer& operator<<(Buffer& buffer, T value)
	{
		buffer.put(value);
		return buffer;
	}

	template<typename T, int N>
	Buffer& operator<<(Buffer& buffer, T (&value)[N])
	{
		buffer.put((const char*)&value, N * sizeof(T));
		return buffer;
	}

	////////////////////////////
	// Generic get operations //
	////////////////////////////
	template<> inline char Buffer::get() const	{ return *get(1); }
	template<> inline unsigned char Buffer::get() const	{ return *get(1); }
	template<> inline float Buffer::get() const 	{ int32 i = get32(); return *reinterpret_cast<float*>(&i); }
	template<> inline uint32 Buffer::get() const 	{ int32 i = get32(); return *reinterpret_cast<uint32*>(&i); }
	template<> inline int32 Buffer::get() const	{ return get32(); }
	template<> inline Color Buffer::get() const	{ int32 i = get32(); return *reinterpret_cast<Color*>(&i); }
	template<> inline bool Buffer::get() const	{ return get<char>() != 0; }

	template<> SPK_PREFIX std::string Buffer::get<std::string>() const;
	template<> SPK_PREFIX Vector3D Buffer::get<Vector3D>() const;

	template<typename T>
	Buffer& operator>>(Buffer& buffer, T& value)
	{
		value = buffer.get<T>();
		return buffer;
	}
}}

#endif
