#ifndef ncutil_NCEXCEPTION_HPP_INCLUDED
#define ncutil_NCEXCEPTION_HPP_INCLUDED 1

#include <exception>
#include <string>
namespace ncutil {

	class NCException : public std::exception
	{
	public:
		NCException( const std::string &msg ) : m_what(msg)
		{
		}
		virtual ~NCException() throw()
		{
		}

		virtual const char * what() const throw()
		{
			return this->m_what.empty() ? "programmer error: no exception description." : this->m_what.c_str();
		}
	private:
		std::string m_what;
	};

} // namespace ncutil


#endif // ncutil_NCEXCEPTION_HPP_INCLUDED
