//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 :                                                    //
//  - Julien Fryer - julienfryer@gmail.com				                        //
//  - Thibault Lescoat - info-tibo@orange.fr                                    //
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

#ifndef H_SPK_CONTROLLER
#define H_SPK_CONTROLLER

namespace SPK
{
	/**
	* @brief Component for controlling values automatically
	*
	* All classes can have controls, but only Controllers will have
	* automatic update of the controls value. These are updated before any
	* particle update; it allows high reusability of the code, and in a
	* transparent manner for the user.
	*
	* To create a new Controller, you just have to subclass it and add it
	* to the system you want (and connect it also).
	*/
	class Controller : public SPKObject
	{
	public:
		/** @brief Destructor */
		virtual ~Controller() {}

		spark_description(Controller, SPKObject)
		(
		);

	protected:
		friend class System;

		/**
		* @brief Updates the value of the controls of the class
		*/
		virtual void updateValues(float deltaTime) = 0;
	};
}

#endif
