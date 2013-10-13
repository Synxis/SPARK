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

#ifndef H_SPK_IO_XMLSAVER
#define H_SPK_IO_XMLSAVER

#ifndef SPK_NO_XML

namespace pugi
{
	class xml_node;
	class xml_attribute;
}

namespace SPK
{
namespace IO
{
	class XMLSerializer;

	/** @brief A struct that defines the layout of a XML document to save */
	struct XMLLayout
	{
		std::string indent;					/**< @brief the string used for an indent ("\t" by default) */
		bool lineBreak;						/**< @brief true to include line breaks in the output */

		/** @brief Default constructor */
		XMLLayout() :
			indent("\t"),
			lineBreak("\n")
		{}
	};

	/** @brief A class to serialize a System in an XML format */
	class SPK_PREFIX XMLSaver : public Saver
	{
	public:
		/** @brief Constructor */
		XMLSaver();

		/** @brief Constructor */
		~XMLSaver();

		/**
		* @brief Sets the author of the systems to save
		* The author will be written at the beginning of the XML document as a comment.
		* @param author : The author of the systems to save
		*/
		void setAuthor(const std::string& author)	{ this->author = author; }

		/**
		* @brief Sets the layout of the saver
		* The layout will be used each time a document is written.
		* @param layout : the layout rules
		*/
		void setLayout(const XMLLayout& layout)		{ this->layout = layout; }

		/**
		* @brief Gets the layout of the saver
		* @return the layout rules
		*/
		XMLLayout& getLayout()						{ return layout; }

		/**
		* @brief Gets the layout of the saver
		* Const version of getLayout()
		* @return the layout rules
		*/
		const XMLLayout& getLayout() const			{ return layout; }

		/** Reimplementation */
		void beginSave(std::ostream& os, const std::vector<SPKObject*>& objRef);

		/** Reimplementation */
		void serializeConnection(const Ref<SPKObject>& sender, const std::string& ctrl,
			const Ref<SPKObject>& receiver, const std::string& attr, unsigned int fieldId, const std::string& field);

		/** Reimplementation */
		bool endSave();

	protected:
		/** Reimplementation */
		Serializer* getSerializer();

	private:
		friend class XMLSerializer;
		struct SaveContext;

		XMLLayout layout;
		std::string author;
		SaveContext* context;
		XMLSerializer* serializer;
		static const int VERSION = 2;
	};
}
}

#endif
#endif
