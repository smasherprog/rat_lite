#include "stdafx.h"
#include "Logging.h"


#ifdef BOOST_NO_EXCEPTIONS
//must provide custom definition to catch the exceptions thrown
namespace boost {
	void throw_exception(std::exception const & e)
	{
		SL_RAT_LOG(e.what(), SL::Remote_Access_Library::Utilities::Logging_Levels::ERROR_log_level);

	}
}
#endif