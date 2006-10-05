#ifndef __OpenViBE_IObject_H__
#define __OpenViBE_IObject_H__

#include "ov_base.h"
#include "ovCIdentifier.h"

namespace OpenViBE
{
	class CMessage;
	class IObjectContext;

	/**
	 * \class IObject
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-06-16
	 * \brief Base class for all the žOpenViBEž platform objects
	 *
	 * Most of the complex objects existing in the žOpenViBEž platform
	 * should derive this base interface. Thus, several operations can
	 * be performed in order to work on the object. The most important
	 * may be the isDerivedFromClass method that allows the caller to
	 * know it the object has specific interface implementation and if
	 * the object could finally be casted in this interface or subclass.
	 * Several interfaces are provided in the žOpenViBEž specficiation
	 * but custom class could also be created. It is the responsability
	 * of the developper to notify the user of what interfaces are
	 * implemented in a concrete class.
	 *
	 * See isDerivedFromClass to have a sample of how this function
	 * could be used.
	 */
	class OV_API IObject
	{
	public:

		/** \name Class identification */
		//@{

		/**
		 * \brief Returns the final class identifier of the concrete class
		 * \return The class identifier of this object.
		 *
		 * This method should return the class identifier of the
		 * concrete instanciated class.
		 */
		virtual OpenViBE::CIdentifier getClassIdentifier(void)=0;
		/**
		 * \brief Checks if this object is compatible with a class identifier
		 * \param rClassIdentifier [in] : the class identifier you want
		 *        to test this object compatibility with
		 * \return \e true if this object si compatible with the given
		 *         class identifier (this means the concrete class
		 *         overloads the class with given class identifier)
		 *         and \e false when this object is not compatible.
		 *
		 * This method should be used to check object compatibility
		 * with super classes and interfaces. For any concrete class
		 * instance derived from OpenViBE::IObject, one can check if
		 * plugin functions are implemented and so on... see
		 * OpenViBE::Plugins::IPluginObject for an example...
		 */
		virtual OpenViBE::boolean isDerivedFromClass(
			const OpenViBE::CIdentifier& rClassIdentifier)
		{
			return (rClassIdentifier==OV_ClassId_Object);
		}

		//@}

	protected:

		virtual ~IObject(void);
	};
};

#define _IsDerivedFromClass_(_SuperClassName_,_ClassIdentifier_) \
	virtual OpenViBE::boolean isDerivedFromClass( \
		const OpenViBE::CIdentifier& rClassIdentifier) \
	{ \
		return ((rClassIdentifier==_ClassIdentifier_) \
		     || _SuperClassName_::isDerivedFromClass(rClassIdentifier)); \
	}

#define _IsDerivedFromClass_Final_(_SuperClassName_,_ClassIdentifier_) \
	_IsDerivedFromClass_(_SuperClassName_,_ClassIdentifier_) \
	virtual OpenViBE::CIdentifier getClassIdentifier(void) \
	{ \
		return _ClassIdentifier_; \
	}

#endif // __OpenViBE_IObject_H__
