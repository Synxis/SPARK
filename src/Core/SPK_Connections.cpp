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

#include <SPARK_Core.h>

namespace SPK
{
	ConnectionStatus connect(const Ref<SPKObject>& sender, const std::string& control,
		const Ref<SPKObject>& receiver, const std::string& attribute,
		unsigned int id, const std::string& field)
	{
		if(!sender)
			return CONNECTION_STATUS_INVALID_SENDER;

		ConnectionParameters cp =
		{
			control,
			receiver,
			attribute,
			ToSPKType<void>::type,
			id,
			field,
			0
		};

		ConnectableDescription* desc = sender->getConnectableDescription();
		return desc ? desc->connect(cp, sender) : CONNECTION_STATUS_INVALID_SENDER;
	}

	void disconnect(const Ref<SPKObject>& sender, const std::string& control,
		const Ref<SPKObject>& receiver, const std::string& attribute,
		unsigned int id, const std::string& field)
	{
		if(!sender)
			return;

		ConnectionParameters cp =
		{
			control,
			receiver,
			attribute,
			ToSPKType<void>::type,
			id,
			field,
			0
		};

		ConnectableDescription* desc = receiver->getConnectableDescription();
		if(desc) desc->disconnect(cp);
	}

	void ConnectionItem::removeConnection()
	{
		if(previousConnection)
			previousConnection->nextConnection = nextConnection;
		if(nextConnection)
			nextConnection->previousConnection = previousConnection;
		previousConnection = nextConnection = 0;
	}

	bool ConnectionItem::insertAfterConnection(ConnectionItem* item)
	{
		if(!item) return false;

		previousConnection = item;
		nextConnection = item->nextConnection;
		item->nextConnection = this;
		if(nextConnection) nextConnection->previousConnection = this;
		return true;
	}
}
