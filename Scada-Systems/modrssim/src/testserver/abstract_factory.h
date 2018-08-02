///////////////////////////////////////////////////////////////////////////////
///  abstract_factory.h
///  Allows a user to register types and a key for the type(can be enums ints 
///  strings). There are two register functions one which takes a create 
///  function which must match the function signature:
///  std::auto_ptr<Base> (*func)()
///  this allows the user to be able to construct the object in the manner 
///  which they require . ie if the constructor for the class needs a certain
///  parameter then the user create function could pass it to the class,
///  via a function getter call or some other means. I have never used this functionality
///  of the class since its design :) Maybe I should just remove it ??
///  If a create function is not suppplied then the default will be used which 
///  just returns an auto pointer to the base class.
///
///  @License
///  This work is licenced under a Creative Commons Licence.
///  see:
///  http://creativecommons.org/licenses/by-nc-sa/3.0/
///  and:
///  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
///  For more details.
///  
///  @author Liam Devine
///  @email: see www.liamdevine.co.uk for contact details.
///////////////////////////////////////////////////////////////////////////////

#ifndef ABSTRCT_FACTORY_H_
#	define ABSTRCT_FACTORY_H_

#	include <map>
#	include <string>
#	include <algorithm>
#	include <memory>
#	include <stdexcept>


namespace LVD
{
	template<typename Base, typename Class>
		inline std::auto_ptr<Base> abstract_creator()
	{
		return std::auto_ptr<Base>(new Class);
	}

	template<typename Base,typename Id>
	class Abstract_factory
	{
	public:
		typedef std::auto_ptr<Base> (*func)();

		Abstract_factory(){}
		//search the map and look for the create function for this key, if found return the return value
		//of the function otherwise return null pointer 
		//LVD update: this does seem to be liked so instead it is throwing
		std::auto_ptr<Base> create(Id key)
		{
			iter i;
			//return ( ( i = m_table.find(key) ) != m_table.end() ) ? (i->second)() : std::auto_ptr<Base>(0);
			if(( i = m_table.find(key) ) != m_table.end() ){ return  i->second(); }
			else
			{
				throw std::runtime_error("could not find the key in the factory");
			}
		}
		//register a key and a create function for this key
		bool register_key(Id key, func function)
		{
			iter i;
			if ( ( i = m_table.find(key) ) == m_table.end() )
			{
				m_table[key] = function;
				return true;
			}
			else return false;
		}
		//register a key and supplies the type of class which this key relates to 
		//this is then used by the default creator to create the required class
		//when called
		template <typename Type> bool register_key(Id key)
		{
			iter i;
			if ( ( i = m_table.find(key) ) == m_table.end() )
			{
				m_table[key] = &abstract_creator<Base,Type>;
				return true;
			}
			else return false;
		}


		//remove a key and the create function from the map
		bool unregister_key(Id key)
		{
			iter i;
			if ( ( i = m_table.find(key) ) != m_table.end() )
			{
				m_table.erase(i);
				return true;
			}
			else return false;
		}

	private:
		typedef std::map< Id , func > Table;
		typedef typename Table::iterator iter;
		Table m_table;

	};

}


#endif//FACTORY_H_
