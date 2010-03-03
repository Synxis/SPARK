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

#include <ctime>
#include <limits>

#include "Core/SPK_Logger.h"
#include "Extensions/Zones/SPK_Point.h"

namespace SPK
{
	SPKMain SPKMain::instance = SPKMain();

	// This allows SPARK initialization at application start up
	SPKMain::SPKMain()
	{
		// Inits the random seed
		randomSeed = static_cast<unsigned int>(std::time(NULL));
		// little tweak to ensure the randomSeed is uniformly distributed along all the range
		for (size_t i = 0; i < 2; ++i)
			randomSeed = generateRandom(static_cast<unsigned int>(1),std::numeric_limits<unsigned int>::max());

		// Creates the zone by default
		defaultZone = Point::create();
		defaultZone->setShared(true);
		defaultZone->setDestroyable(false);
	}

	// This allows SPARK finalization at application exit
	SPKMain::~SPKMain()
	{
		// Singletons are destroyed
		Logger::destroyInstance();
	}
}
