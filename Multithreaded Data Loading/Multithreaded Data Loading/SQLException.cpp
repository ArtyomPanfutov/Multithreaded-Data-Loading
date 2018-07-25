// SQLException.cpp
// created: 2018-07-22
// modified:
// author: Artyom Panfutov

#include "SQLException.h"
#include "constant.h"
#include <iostream>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <odbcss.h>

/***********************************************************************
************* Definiton of functions. Class SQLException ***************
***********************************************************************/


// SQLException constructor
///////////////////////////////////////////////////////////////////////
SQLException::SQLException(const char *Message, SQLRETURN RetCode) : std::exception(Message)
{
  this->RetVal = RetCode;
} // End of constructor
//---------------------------------------------------------------------

// ShowMessage()
///////////////////////////////////////////////////////////////////////
void SQLException::ShowMessage(SQLHSTMT hstmt)
{
	SQLLEN numRecs = 0;
	SQLRETURN RetCode;

	SQLGetDiagField( SQL_HANDLE_STMT,
		             hstmt,
		             0,
		             SQL_DIAG_NUMBER,
		             &numRecs, 
		             0, 
		             0);

	// Get the status records.
	auto i = 1;
	while (i <= numRecs && (RetCode = SQLGetDiagRec( SQL_HANDLE_STMT, 
		                                             hstmt, 
		                                             i,
		                                             SQLState,
		                                             &NativeError, 
		                                             Msg,
		                                             sizeof(Msg),
		                                             &MsgLen)) != SQL_NO_DATA )
		i++;

  std::cout << this->what() << std::endl;
  std::cout << "RetVal: " << RetVal << std::endl << std::endl;
  std::cout << "Native Error: " << NativeError << std::endl << "SQL State: " << SQLState << std::endl << Msg << std::endl << std::endl << std::endl;


} // ShowMessage()
//---------------------------------------------------------------------
