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

#ifndef H_SPK_DATAHANDLER
#define H_SPK_DATAHANDLER

#include "Core/SPK_DEF.h"
#include "Core/SPK_Logger.h"
#include "Core/SPK_DataSet.h"

namespace SPK
{
	class DataHandler
	{
	friend class Group;

	public :

		virtual inline ~DataHandler() {}
		inline bool needsDataSet() const;

	protected :

		inline DataHandler(bool NEEDS_DATASET); // abstract class

		inline void prepareData(const Group& group,DataSet* dataSet) const;
		virtual inline void createData(DataSet& dataSet,const Group& group) const {}
		virtual inline void checkData(DataSet& dataSet,const Group& group) const {}

	private :

		const bool NEEDS_DATASET;
	};

	inline DataHandler::DataHandler(bool NEEDS_DATASET) :
		NEEDS_DATASET(NEEDS_DATASET)
	{}

	inline bool DataHandler::needsDataSet() const
	{
		return NEEDS_DATASET;
	}

	inline void DataHandler::prepareData(const Group& group,DataSet* dataSet) const
	{
		if (dataSet != NULL)
		{
			if (!dataSet->isInitialized())
				createData(*dataSet,group);
			else
				checkData(*dataSet,group);

			dataSet->setInitialized();
		}
	}
};

#endif
