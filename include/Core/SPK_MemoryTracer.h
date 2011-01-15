//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_MEMORYTRACER
#define H_SPK_MEMORYTRACER

#ifdef _DEBUG
#define SPK_TRACE_MEMORY
#endif

#ifndef SPK_TRACE_MEMORY
#define SPK_NEW(name,...) new name(__VA_ARGS__)
#define SPK_NEW_ARRAY(name,nb) new name[nb]
#define SPK_DELETE(name) delete name
#define SPK_DELETE_ARRAY(name) delete[] name
#define SPK_DUMP_MEMORY {}
#else
#define SPK_NEW(name,...) (name*)SPK::SPKMemoryTracer::getInstance().registerAllocation(new name(__VA_ARGS__),sizeof(name),#name,__FILE__,__LINE__)
#define SPK_NEW_ARRAY(name,nb) (name*)SPK::SPKMemoryTracer::getInstance().registerAllocation(new name[nb],sizeof(name) * nb,std::string(#name).append("[]"),__FILE__,__LINE__)
#define SPK_DELETE(name) { SPK::SPKMemoryTracer::getInstance().unregisterAllocation(name); delete name; }
#define SPK_DELETE_ARRAY(name) { SPK::SPKMemoryTracer::getInstance().unregisterAllocation(name); delete[] name; }
#define SPK_DUMP_MEMORY { SPK::SPKMemoryTracer::getInstance().dumpMemory(); }

#include <string>
#include <set>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

namespace SPK
{
	class SPK_PREFIX SPKMemoryTracer
	{
	struct BlockInfo;

	friend bool operator==(const BlockInfo&,const BlockInfo&);
	friend bool operator<(const BlockInfo&,const BlockInfo&);
	friend bool compareAllocTime(const BlockInfo&,const BlockInfo&);

	public :

		static SPKMemoryTracer& getInstance();

		void* registerAllocation(void* position,size_t size,const std::string& type,const std::string& file,size_t line)
		{
			if (position == NULL)
				return NULL;

			BlockInfo info(position);
			info.size = size;
			info.type = type;
			info.fileName = file;
			info.lineNb = line;
			info.time = static_cast<float>(clock()) / CLOCKS_PER_SEC;
			info.index = nextIndex++;

			blocks.insert(info);

			totalMemorySize += size;
			if (totalMemorySize > maxMemorySize)
				maxMemorySize = totalMemorySize;

			return position;
		}

		void unregisterAllocation(void* position)
		{
			std::set<BlockInfo>::iterator it = blocks.find(BlockInfo(position));
			if (it != blocks.end())
			{
				totalMemorySize -= it->size;
				blocks.erase(it);
			}
		}

		void dumpMemory()
		{
			std::ofstream file("SPARK_Memory_Dump.txt",std::ios::out | std::ios::app);

			if (file)
			{
				std::vector<BlockInfo> sortedBlocks(blocks.begin(),blocks.end());
				std::sort(sortedBlocks.begin(),sortedBlocks.end(),compareAllocTime);

				time_t currentTime = time(NULL);
				tm* timeinfo = localtime(&currentTime);
				file << "-----------------------------------------------------------------------------------------------\n";
				file << "SPARK MEMORY DUMP - " << asctime(timeinfo) << "\n\n";
				file.precision(3);
				file << "Dynamic memory used: " << totalMemorySize << " bytes allocated (" << totalMemorySize / (1024.0f * 1024.0f) << " mb) in " << blocks.size() << " blocks\n";
				file << "Maximum dynamic memory allocated: " << maxMemorySize << " bytes ("<< maxMemorySize / (1024.0f * 1024.0f) << " mb)\n";
				file << "Total number of allocated blocks: " << nextIndex << "\n\n";

				std::vector<BlockInfo>::const_iterator it = sortedBlocks.begin();
				std::vector<BlockInfo>::const_iterator end = sortedBlocks.end();
				for (; it != end; ++it)
				{
					file << it->position << " - ";

					file.width(10);
					file << std::right << it->size << " bytes";

					file.width(32);
					std::ostringstream typeStr;
					typeStr << " of " << it->type;
					file << std::right << typeStr.str();

					file.width(18);
					std::ostringstream timeStr;
					timeStr << " at " << it->time << " sec";
					file << std::right << timeStr.str();
					file << "\t(" << it->fileName << " - line " << it->lineNb << ")\n";
				}
				file << "-----------------------------------------------------------------------------------------------\n\n";
				file.close();
			}
		}

	private :

		struct BlockInfo
		{
			void* position;
			size_t size;
			std::string type;
			std::string fileName;
			size_t lineNb;
			float time;
			unsigned long int index;

			BlockInfo(void* position) : position(position) {}
		};

		SPKMemoryTracer() :
			nextIndex(0),
			totalMemorySize(0),
			maxMemorySize(0)
		{}

		SPKMemoryTracer(const SPKMemoryTracer&); // Not used
		SPKMemoryTracer& operator=(const SPKMemoryTracer&); // Not used

		unsigned long int nextIndex;

		unsigned long int totalMemorySize;
		unsigned long int maxMemorySize;

		std::set<BlockInfo> blocks;
	};

	inline bool operator==(const SPKMemoryTracer::BlockInfo& block0,const SPKMemoryTracer::BlockInfo& block1)
	{
		return block0.position == block1.position;
	}

	inline bool operator<(const SPKMemoryTracer::BlockInfo& block0,const SPKMemoryTracer::BlockInfo& block1)
	{
		return block0.position < block1.position;
	}

	inline bool compareAllocTime(const SPKMemoryTracer::BlockInfo& block0,const SPKMemoryTracer::BlockInfo& block1)
	{
		return block0.index < block1.index;
	}
}
#endif
#endif
