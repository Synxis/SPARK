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

#ifndef H_SPK_IO_XMLSAVER
#define H_SPK_IO_XMLSAVER

#include <sstream>

class TiXmlElement;

namespace SPK
{
namespace IO
{
	/** @brief An abstract class to save an entire particle system in a resource */
	class SPK_PREFIX XMLSaver : public Saver
	{
	public :

		virtual XMLSaver* clone() const { return new XMLSaver(*this); }

	private :

		std::string author;

		virtual bool innerSave(std::ostream& os,Graph& graph) const;
		bool writeNode(TiXmlElement& parent,const Node& node,Graph& graph) const;

		template<typename T> static std::string format(const T& value);
		template<typename T> static std::string formatArray(const std::vector<T>& value);
	};

	template<typename T>
	std::string XMLSaver::format(const T& value)
	{
		std::ostringstream os;
		os << std::boolalpha;
		os << value;
		return os.str();
	}

	template<typename T> 
	std::string XMLSaver::formatArray(const std::vector<T>& values)
	{
		std::ostringstream os;
		os << std::boolalpha;
		for (size_t i = 0; i < values.size(); ++i)
		{
			os << values[i];
			if (i != values.size() - 1)
				os << ',';
		}
		return os.str();
	}
}}

#endif
