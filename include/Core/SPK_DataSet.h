//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2010 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_DATASET
#define H_SPK_DATASET

#include "Core/SPK_DEF.h"

#define SPK_GET_DATA(type,dataSetPtr,index) dynamic_cast<type&>(*(dataSetPtr)->getData(index))

namespace SPK
{
	class Data
	{
	friend class Group;
	friend class DataSet;

	public :

		inline Data();

		inline void setFlag(long int flag);
		inline long int getFlag() const;

	protected :

		virtual inline ~Data() {}

	private :

		long int flag;

		virtual void swap(size_t index0,size_t index1) = 0;
	};

	class SPK_PREFIX DataSet
	{
	friend class Group;

	public :

		inline DataSet();
		virtual inline ~DataSet();

		void init(size_t nbData);

		void setData(size_t index,Data* data);
		inline void setInitialized();

		inline Data* getData(size_t index);
		inline const Data* getData(size_t index) const;

		inline bool isInitialized() const;

		inline void destroyData(size_t index);
		void destroyAllData();

	private :

		Data** dataArray;
		size_t nbData;
		bool initialized;

		inline void swap(size_t index0,size_t index1);
	};

	inline Data::Data() :
		flag(0)
	{}

	inline void Data::setFlag(long int flag)
	{
		this->flag = flag;
	}

	inline long int Data::getFlag() const
	{
		return flag;
	}

	inline DataSet::DataSet() :
		nbData(0),
		initialized(false),
		dataArray(NULL)
	{}

	inline DataSet::~DataSet()
	{
		destroyAllData();
		SPK_DELETE_ARRAY(dataArray);
	}

	inline void DataSet::setInitialized()
	{
		initialized = true;
	}

	inline Data* DataSet::getData(size_t index)
	{
		return dataArray[index];
	}

	inline const Data* DataSet::getData(size_t index) const
	{
		return dataArray[index];
	}

	inline bool DataSet::isInitialized() const
	{
		return initialized;
	}

	inline void DataSet::destroyData(size_t index)
	{
		setData(index,NULL);
	}

	inline void DataSet::swap(size_t index0,size_t index1)
	{
		for (size_t i = 0; i < nbData; ++i)
			dataArray[i]->swap(index0,index1);
	}
};

#endif
