// FALoad.cpp
// created: 2018-07-22
// modified: 2018-08-25
// author: Artyom Panfutov

#include "FALoad.h"
#include "SQLException.h"
#include <string>
#include "file.h"
#include <fstream>
#include <vector>
#include "constant.h"
#include "Output.h"


/***********************************************************************
*************** Definiton of methods. Class FALoader *******************
***********************************************************************/

std::string ProccessingLogFileName = "ProcessingLog.txt";
Log ProcLogWriter(ProccessingLogFileName);

// Constructor FALoader()
////////////////////////////////////////////////////////////////////////
FALoad::FALoad(std::string &filename, bool ShowDiag, unsigned long Branch) : Connection() 
{  
	this->ShowDiagInfo = ShowDiag;

	this->BranchID = Branch;

	this->FileName = filename; // For formula's parameter(FileName)
}
// End of constructor FALoader()
// ---------------------------------------------------------------------


// Destructor FALoader()
////////////////////////////////////////////////////////////////////////
FALoad::~FALoad(void) 
{	
	for (std::vector< DSType *>::iterator it = DSTypes.begin(); it != DSTypes.end(); ++it)
	{
		delete *it;
	}
	DSTypes.clear();

	for (std::vector< Param *>::iterator it = SetupFormulaParams.begin(); it != SetupFormulaParams.end(); ++it)
	{
		delete *it;		
	}
	SetupFormulaParams.clear();

	for (std::vector< char *>::iterator it = Columns.begin(); it != Columns.end(); ++it)
	{
		delete *it;
	}
	Columns.clear();

	for (std::vector<int *>::iterator it = ColumnsLen.begin(); it != ColumnsLen.end(); ++it)
	{
		delete *it;
	}
	ColumnsLen.clear();

	for (std::vector< Param *>::iterator it = LastFormulaParams.begin(); it != LastFormulaParams.end(); ++it)
	{
		delete *it;		
	}
	LastFormulaParams.clear();
}
// End of destructor FALoader()
// ---------------------------------------------------------------------

// GetLoadInfo()
////////////////////////////////////////////////////////////////////////
void FALoad::GetLoadInfo()
{
	SQLCHAR SQLGetLoadInfo[] = " select ImportFileID, "
		                              " ltrim(rtrim(Delimiter)), "
                                      " PathInput, "
                                      " PathOutput, "
                                      " ParamList, "
                                      " OEMToANSI, "
                                      " SetupFormula, "
                                      " LastFormula "
                                 " from tImportFile with (nolock) "
                                " where Brief like ? and ImportType = 5 ";
	

	SQLINTEGER
		cbImportFileID,
		cbBrief = SQL_NTS,
		cbDelimiter,
		cbPathInput,
		cbPathOutput,
		cbParamList,
		cbOEMToANSI,
		cbSetupFormula,
		cbLastFormula;

	
	char *BriefParam = (char *)this->LoadBrief.c_str();

	try
	{
		retcode = SQLBindParameter( hstmt, 
			                        1,
			                        SQL_PARAM_INPUT,
			                        SQL_C_CHAR, 
			                        SQL_VARCHAR,
			                        10,                   // Nailed on the DSBRIEFNAME length
			                        0,
			                        BriefParam,
			                        sizeof(BriefParam),
			                        &cbBrief );

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			throw SQLException("\n ERROR: GetLoadInfo failed! SQLBindParameter(Brief)\n", retcode);
		}
		
		retcode = SQLExecDirect( hstmt, 
			                     SQLGetLoadInfo, 
			                     SQL_NTS );
		
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			throw SQLException("\n ERROR: GetLoadInfo failed! SQLExecDirect(SQLGetLoadInfo)\n", retcode);
		}
		
		if (retcode == SQL_SUCCESS)
		{
			while (TRUE)
			{
				retcode = SQLFetch(hstmt);
				if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				{
					throw SQLException("\n ERROR: GetLoadInfo failed! SQLFetch\n", retcode);
				}

				else if (retcode == SQL_SUCCESS)
				{
					// ImportFileID
					retcode = SQLGetData( hstmt, 
						                  1,
						                  SQL_C_DOUBLE,
						                  &ImportFileID,
						                  10,
						                  &cbImportFileID );

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLoadInfo failed! SQLGetData(ImportFileID)\n", retcode);

					// Delimiter										
					retcode = SQLGetData( hstmt,
										  2,
										  SQL_C_CHAR,
										  &Delimiter,
										  sizeof(Delimiter),
										  &cbDelimiter );

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLoadInfo failed! SQLGetData(Delimiter)\n", retcode);

					// PathInput									
					retcode = SQLGetData( hstmt,
						                  3,
						                  SQL_C_CHAR,
						                  &PathInput,
						                  sizeof(PathInput),
						                  &cbPathInput );

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLoadInfo failed! SQLGetData(PathInput)\n", retcode);

					// PathOutput								
					retcode = SQLGetData( hstmt,
						                  4,
						                  SQL_C_CHAR,
						                  &PathOutput,
						                  sizeof(PathOutput),
						                  &cbPathOutput );

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLoadInfo failed! SQLGetData(PathOutput)\n", retcode);

					// ParamList							
					retcode = SQLGetData( hstmt,
						                  5,
						                  SQL_C_CHAR,
						                  &ParamList,
						                  sizeof(ParamList),
						                  &cbParamList );

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLoadInfo failed! SQLGetData(ParamList)\n", retcode);

					// OEMToANSI
					retcode = SQLGetData( hstmt,
						                  6,
						                  SQL_C_DOUBLE,
						                  &OEMToANSI,
						                  10,
						                  &cbOEMToANSI);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLoadInfo failed! SQLGetData(OEMToANSI)\n", retcode);

					// SetupFormula
					retcode = SQLGetData( hstmt,
					                      7,
						                  SQL_C_DOUBLE,
						                  &SetupFormula,
						                  10,
						                  &cbSetupFormula );

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLoadInfo failed! SQLGetData(SetupFormula)\n", retcode);

					// LastFormula
					retcode = SQLGetData( hstmt,
						                  8,
						                  SQL_C_DOUBLE,
						                  &LastFormula,
						                  10,
						                  &cbLastFormula );

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLoadInfo failed! SQLGetData(LastFormula)\n", retcode);

					if (ShowDiagInfo)
					{
						std::cout.setf(std::ios::fixed);
						std::cout << "\n ImportFileID " << ImportFileID;
						std::cout << "\n Delimiter " << Delimiter;
						std::cout << "\n PathInput " << PathInput;
						std::cout << "\n PathOutput " << PathOutput;
						std::cout << "\n ParamList " << ParamList;
						std::cout << "\n OEMToANSI " << OEMToANSI;
						std::cout << "\n SetupFormula " << SetupFormula;
						std::cout << "\n LastFormula" << LastFormula;
					}
				}
				else
				{
				//	std::cout << "\nSQLGetLoadID: No Info!\n";
					break;
				}
			}

		}
		else
			throw SQLException("\n ERROR: GetLoadInfo failed!\n", retcode);

		retcode = SQLFreeStmt(hstmt, SQL_CLOSE);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetLoadInfo failed! SQLFreeStmt failed!\n", retcode);

		this->GetFormulaText(hstmt, SetupFormula, SetupFormulaStr);
		this->GetFormulaText(hstmt, LastFormula, LastFormulaStr);
		
		
		GetTableColumnsFromDB();
		
		if (ShowDiagInfo)
		{
			std::cout << "\n SetupFormulaStr:\n" << SetupFormulaStr;
			std::cout << "\n SetupFormulaStr:\n" << LastFormulaStr;

			std::cout << "\n TableColumns: " << TableColumns;

			for (int i = 0; i < ColumnsLen.size(); i++)
			{
				std::cout << "\n ColumnLen " << i << " " << *ColumnsLen[i];
			}
		}
		
	}
	catch (SQLException &ex)
	{
		ex.ShowMessage(hstmt);
	}
	catch (std::exception &ex)
	{

		std::cout << ex.what();
	}
	
}// End of GetLoadInfo()
//---------------------------------------------------------------------


// SetLoadBrief()
///////////////////////////////////////////////////////////////////////
void FALoad::SetLoadBrief(std::string &Brief)
{
	this->LoadBrief = Brief;
} // End of SetLoadBrief()
//---------------------------------------------------------------------


// ShowLoadBrief()
///////////////////////////////////////////////////////////////////////
inline void FALoad::ShowLoadBrief()
{
	  std::cout << "\n Сокращение загрузки: " << this->LoadBrief;
} // End of ShowLoadBrief()
//--------------------------------------------------------------------


// GetDSTypesFromDB()
//////////////////////////////////////////////////////////////////////
void FALoad::GetDSTypesFromDB()
{
  SQLCHAR SQLGetDSTypes[] = 
	                       " select s.name, "
                                  " s.length, "
                                  " s.prec "
	                         " from systypes s with (nolock) "
                            " inner join tDSType d with (nolock) "
                                   "  on s.name = d.typename ";
	  
  SQLINTEGER cbName,
	         cbLength,
	         cbPrec;
 
  try 
  {
    retcode = SQLExecDirect( hstmt,
	                         SQLGetDSTypes,
	                         SQL_NTS );

    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	  throw SQLException("\n ERROR: GetDSTypesFromDB faled! SQLExecDirect(SQLGetDSTypes)\n", retcode);
    }

    if (retcode == SQL_SUCCESS)
    {
	    while (TRUE)
	    {
		  retcode = SQLFetch(hstmt);
		  if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
		  {
			  throw SQLException("\n ERROR: GetDSTypesFromDB failed! SQLFetch\n", retcode);
		  }

		  else if (retcode == SQL_SUCCESS)
		  {
			  DSTypes.push_back(new DSType);

			  // Name
			  retcode = SQLGetData( hstmt,
				                    1,
				                    SQL_C_CHAR,
				                    DSTypes.back()->Name,
				                    sizeof(DSTypes.back()->Name),
				                    &cbName );

			  if (retcode != SQL_SUCCESS)
				  throw SQLException("\n ERROR: GetDSTypesFromDB failed! SQLGetData(Name)\n", retcode);

			  // Length
			  retcode = SQLGetData( hstmt,
				                    2,
				                    SQL_C_ULONG,
				                    &DSTypes.back()->Length,
				                    sizeof(&DSTypes.back()->Length),
				                    &cbLength );

			  if (retcode != SQL_SUCCESS)
				  throw SQLException("\n ERROR: GetDSTypesFromDB failed! SQLGetData(Length)\n", retcode);

			  // Precision
			  retcode = SQLGetData( hstmt,
				                    3,
				                    SQL_C_ULONG,
				                    &DSTypes.back()->Precision,
				                    sizeof(&DSTypes.back()->Precision),
				                    &cbPrec );

			  if (retcode != SQL_SUCCESS)
				  throw SQLException("\n ERROR: GetDSTypesFromDB failed! SQLGetData(Precision)\n", retcode);

			  /*std::cout.setf(std::ios::fixed);
			  std::cout << "\n Name " << DSTypes.back()->Name;
			  std::cout << "\n Length " << DSTypes.back()->Length;
			  std::cout << "\n Precision " << DSTypes.back()->Precision;*/
		  }
		  else
		  {
			  break;
		  }
	    }

    }
    else
	  throw SQLException("\n ERROR: GetLoadInfo failed!\n", retcode);


    retcode = SQLFreeStmt(hstmt, SQL_CLOSE);

    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	  throw SQLException("\n ERROR: GetDSTypeFromDB failed! SQLFreeStmt failed!\n", retcode);
  }
  catch (SQLException &ex)
  {
	ex.ShowMessage(hstmt);
  }

  // Write into variables
  try
  {
	  for (auto i = 0; i < DSTypes.size(); i++)
	  {
		  if (strcmp(DSTypes[i]->Name, "DSBRIEFNAME") == 0)
		  {
			  DSBRIEFNAME_Length = DSTypes[i]->Length;
		  }
		  else if (strcmp(DSTypes[i]->Name, "DSCOMMENT") == 0)
		  {
			  DSCOMMENT_Length = DSTypes[i]->Length;
		  }
	  }

	  if (DSBRIEFNAME_Length < 0)
		  throw std::exception("\n ERROR:  Type DSBRIEFNAME has incorrect length!\n");

	  if (DSCOMMENT_Length < 0)
		  throw std::exception("\n ERROR: Type DSCOMMENT has incorrect length!\n");
  }
  catch (std::exception &ex)
  {
	  std::cout << ex.what();
  }

} // End of GetDSTypesFromDB()
//---------------------------------------------------------------------------------


// GetType()
///////////////////////////////////////////////////////////////////////////////////
inline char * DSType::GetType()
{
	return Name;
} // End of GetType()
//--------------------------------------------------------------------------------


// GetFormulaText()
///////////////////////////////////////////////////////////////////////////////////
void FALoad::GetFormulaText(SQLHSTMT &hstmt, double &FormulaID, std::string &QueryStr)
{
	SQLCHAR SQLGetQueryStr[] = " select QueryStr "
                                 " from tQueryLine with (nolock index = XPKtQueryLine) "
 		                        " where Type = 1 "
		                          " and ContextID = ? ";

	SQLINTEGER
		cbFormulaID = SQL_NTS,
		cbQueryStr;

	SQLCHAR SingleStr[256];


	try
	{
		retcode = SQLBindParameter(
			hstmt,
			1,
			SQL_PARAM_INPUT, 
			SQL_C_DOUBLE,
			SQL_DOUBLE,
			10,
			0,
			&FormulaID,
			sizeof(FormulaID),
			&cbFormulaID);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			throw SQLException("\n ERROR: GetFormulaText failed! SQLBindParameter(FormulaID)\n", retcode);
		}

		retcode = SQLExecDirect(
			hstmt,
			SQLGetQueryStr,
			SQL_NTS);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			throw SQLException("\n ERROR: GetFormulaText failed! SQLExecDirect(SQLGetQueryStr)\n", retcode);
		}

		if (retcode == SQL_SUCCESS)
		{
			while (TRUE)
			{
				retcode = SQLFetch(hstmt);
				if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				{
					throw SQLException("\n ERROR: GetFormulaText failed! SQLFetch\n", retcode);
				}

				else if (retcode == SQL_SUCCESS)
				{
					// QueryStr										
					retcode = SQLGetData(
						hstmt,
						1,
						SQL_C_CHAR,
						&SingleStr,
						sizeof(SingleStr),
						&cbQueryStr);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetFormulaText failed! SQLGetData(QueryStr)\n", retcode);


					if (strcmp((const char *)SingleStr, "SQL:") != 0) // Ingore first row "SQL:" from formula's text
					{
						QueryStr += "\n";
						QueryStr += (const char*)SingleStr;
					}
				}
				else
					break;
			}
		}

		retcode = SQLFreeStmt(hstmt, SQL_CLOSE);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetFormulaText failed! SQLFreeStmt failed!\n", retcode);
	}
	catch (SQLException &ex)
	{
		ex.ShowMessage(hstmt);
	}
} // End of GetFormulaText()
//---------------------------------------------------------------------------------



// GetSetupParamsFromDB
///////////////////////////////////////////////////////////////////////////////////
void FALoad::GetSetupParamsFromDB()
{
	SQLCHAR SQLGetSetupFormulaParams[] = " select Number, "
                                                " CalcField,"
                                                " CalcProperty "
                                           " from tParam with (nolock index = XPKtParam) "
 		                                   " where FormulaID = ? ";

	SQLINTEGER
		cbFormulaID = SQL_NTS,
		cbNumber,
		cbCalcField,
		cbCalcProperty;

	try
	{
		retcode = SQLBindParameter(
			hstmt,
			1,
			SQL_PARAM_INPUT,
			SQL_C_DOUBLE,
			SQL_DOUBLE,
			15,
			0,
			&SetupFormula,
			sizeof(SetupFormula),
			&cbFormulaID);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			throw SQLException("\n ERROR:  GetSetupParamsFromDB failed! SQLBindParameter(FormulaID)\n", retcode);
		}

		retcode = SQLExecDirect(
			hstmt,
			SQLGetSetupFormulaParams,
			SQL_NTS);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			throw SQLException("\n ERROR: GetSetupParamsFromDB failed! SQLExecDirect(SQLGetFormulaParams)\n", retcode);
		}

		if (retcode == SQL_SUCCESS)
		{
			while (TRUE)
			{
				retcode = SQLFetch(hstmt);
				if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				{
					throw SQLException("\n ERROR: GetSetupParamsFromDB failed! SQLFetch\n", retcode);
				}

				else if (retcode == SQL_SUCCESS)
				{
					SetupFormulaParams.push_back(new Param);

					// Number									
					retcode = SQLGetData(
						hstmt,
						1,
						SQL_C_DOUBLE,
						&SetupFormulaParams.back()->Number,
						sizeof(SetupFormulaParams.back()->Number),
						&cbNumber);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetSetupParamsFromDB failed! SQLGetData(Number)\n", retcode);

					// CalcField								
					retcode = SQLGetData(
						hstmt,
						2,
						SQL_C_CHAR,
						&SetupFormulaParams.back()->CalcField,
						sizeof(SetupFormulaParams.back()->CalcField),
						&cbCalcField);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetSetupParamsFromDB failed! SQLGetData(CalcField)\n", retcode);

					// CalcProperty
					retcode = SQLGetData(
						hstmt,
						3,
						SQL_C_ULONG,
						&SetupFormulaParams.back()->CalcProperty,
						sizeof(SetupFormulaParams.back()->CalcProperty),
						&cbCalcProperty);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetSetupParamsFromDB failed! SQLGetData(CalcProperty)\n", retcode);
			
				}
				else
					break;
			}
		}

		retcode = SQLFreeStmt(hstmt, SQL_CLOSE);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetSetupParamsFromDB failed! SQLFreeStmt failed!\n", retcode);
	}
	catch (SQLException &ex)
	{
		ex.ShowMessage(hstmt);
	}

} // End of GetSetupParamsFromDB


  // GetLastParamsFromDB
  ///////////////////////////////////////////////////////////////////////////////////
void FALoad::GetLastParamsFromDB()
{
	SQLCHAR SQLGetLastFormulaParams[] = " select Number, "
                                               " CalcField, "
                                               " CalcProperty "
                                         " from tParam with (nolock index = XPKtParam) "
 		                                " where FormulaID = ? ";

	SQLINTEGER
		cbFormulaID = SQL_NTS,
		cbNumber,
		cbCalcField,
		cbCalcProperty;

	try
	{
		retcode = SQLBindParameter(
			hstmt,
			1,
			SQL_PARAM_INPUT,
			SQL_C_DOUBLE,
			SQL_DOUBLE,
			15,
			0,
			&LastFormula,
			sizeof(LastFormula),
			&cbFormulaID);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			throw SQLException("\n ERROR: GetLastParamsFromDB failed! SQLBindParameter(FormulaID)\n", retcode);
		}

		retcode = SQLExecDirect(
			hstmt,
			SQLGetLastFormulaParams,
			SQL_NTS);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			throw SQLException("\n ERROR: GetLastParamsFromDB failed! SQLExecDirect(SQLGetFormulaParams)\n", retcode);
		}

		if (retcode == SQL_SUCCESS)
		{
			while (TRUE)
			{
				retcode = SQLFetch(hstmt);
				if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				{
					throw SQLException("\n ERROR: GetLastParamsFromDB failed! SQLFetch\n", retcode);
				}

				else if (retcode == SQL_SUCCESS)
				{
					LastFormulaParams.push_back(new Param);

					// Number									
					retcode = SQLGetData(
						hstmt,
						1,
						SQL_C_DOUBLE,
						&LastFormulaParams.back()->Number,
						sizeof(LastFormulaParams.back()->Number),
						&cbNumber);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLastParamsFromDB failed! SQLGetData(Number)\n", retcode);

					// CalcField								
					retcode = SQLGetData(
						hstmt,
						2,
						SQL_C_CHAR,
						&LastFormulaParams.back()->CalcField,
						sizeof(LastFormulaParams.back()->CalcField),
						&cbCalcField);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLastParamsFromDB failed! SQLGetData(CalcField)\n", retcode);

					// CalcProperty
					retcode = SQLGetData(
						hstmt,
						3,
						SQL_C_ULONG,
						&LastFormulaParams.back()->CalcProperty,
						sizeof(LastFormulaParams.back()->CalcProperty),
						&cbCalcProperty);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetLastParamsFromDB failed! SQLGetData(CalcProperty)\n", retcode);

				}
				else
					break;
			}
		}

		retcode = SQLFreeStmt(hstmt, SQL_CLOSE);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetLastParamsFromDB failed! SQLFreeStmt failed!\n", retcode);
	}
	catch (SQLException &ex)
	{
		ex.ShowMessage(hstmt);
	}

} // End of GetLastParamsFromDB
//--------------------------------------------------------------------------------


// GetInfo
///////////////////////////////////////////////////////////////////////////////////
inline void GetInfo(std::string &CalcField, unsigned int &CalcProperty)
{
	
 // What is it?

} // End of GetInfo
//---------------------------------------------------------------------------------


// GetTableColumnsFromDB()
///////////////////////////////////////////////////////////////////////////////////
void FALoad::GetTableColumnsFromDB()
{
	SQLCHAR SQLGetColumns[] = "select count(*) "
                                "from syscolumns with (nolock) "
                               "where id = object_id(?) ";

	SQLCHAR SQLGetLengthColumn[] = 
		"select c.prec, "
		       "c.xscale "
          "from syscolumns c with (nolock) "
         "where c.id = object_id(?) "
         "order by c.colorder asc ";


	SQLINTEGER
		cbParamList = SQL_NTS,
		cbTableColumns,
		cbColumnLength,
		cbColumnScale;

	long ColumnScale;

	try
	{
		// 1. Get count of columns
		retcode = SQLBindParameter(
			hstmt,
			1,
			SQL_PARAM_INPUT,
			SQL_C_CHAR,
			SQL_VARCHAR,
			256,
			0,
			ParamList,
			sizeof(ParamList),
			&cbParamList);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLBindParameter(ParamList)\n", retcode);
	

		retcode = SQLExecDirect(
			hstmt,
			SQLGetColumns,
			SQL_NTS);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLExecDirect(SQLGetColumns)\n", retcode);
		

		if (retcode == SQL_SUCCESS)
		{
			while (TRUE)
			{
				retcode = SQLFetch(hstmt);
				if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				{
					throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLFetch\n", retcode);
				}

				else if (retcode == SQL_SUCCESS)
				{
					// Count									
					retcode = SQLGetData(
						hstmt,
						1,
						SQL_C_ULONG,
						&TableColumns,
						0,
						&cbTableColumns);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLGetData(Number)\n", retcode);
				}
				else
					break;
			}
		}

		retcode = SQLFreeStmt(hstmt, SQL_CLOSE);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLFreeStmt failed!\n", retcode);


		// 2. Get length for each column
		retcode = SQLBindParameter(
			hstmt,
			1,
			SQL_PARAM_INPUT,
			SQL_C_CHAR,
			SQL_VARCHAR,
			256,
			0,
			ParamList,
			sizeof(ParamList),
			&cbParamList);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLBindParameter(ParamList)\n", retcode);


		retcode = SQLExecDirect(
			hstmt,
			SQLGetLengthColumn,
			SQL_NTS);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLExecDirect(SQLGetLengthColumn)\n", retcode);

		if (retcode == SQL_SUCCESS)
		{    
			int i = 0;

			Columns.erase(Columns.cbegin(), Columns.cend());
			IsNumeric.erase(IsNumeric.cbegin(), IsNumeric.cend());

			while (TRUE)
			{
				ColumnsLen.push_back(new int);
				IsNumeric.push_back(new bool);

				retcode = SQLFetch(hstmt);
				if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				{
					throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLFetch\n", retcode);
				}

				else if (retcode == SQL_SUCCESS)
				{
					// Length
					retcode = SQLGetData(
						hstmt,
						1,
						SQL_C_ULONG,
						ColumnsLen[i],
						0,
						&cbTableColumns);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLGetData(Length)\n", retcode);

					// Scale
					retcode = SQLGetData(
						hstmt,
						2,
						SQL_C_ULONG,
						&ColumnScale,
						0,
						&cbColumnScale);

					if (retcode != SQL_SUCCESS)
						throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLGetData(Length)\n", retcode);

					if (ColumnScale == 0)
						IsNumeric[i] = false;
					else
						IsNumeric[i] = true;

					i++;
				}
				else
					break;
			}
		}

		retcode = SQLFreeStmt(hstmt, SQL_CLOSE);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			throw SQLException("\n ERROR: GetTableColumnsFromDB failed! SQLFreeStmt failed!\n", retcode);
		
	}
	catch (SQLException &ex)
	{
		ex.ShowMessage(hstmt);
	}

} // End of GetTableColumnsFromDB
  //---------------------------------------------------------------------------------



// StartBCP
/////////////////////////////////////////////////////////////////////////////////////
unsigned long FALoad::StartBCP(std::string &fieldterm_str, std::string &rowterm_str, unsigned long int &firstrow, unsigned long int &lastrow, std::vector<std::string> &file_str)
{
	int cbCol = 0;

//	std::ifstream file(filename);
	std::string cur_str, copy_field;

	unsigned long it = 0,
		cur_pos = 0,
		last_pos = 0,
		str_num = 1;

	char *chColumn;  
	char terminator = '\0';

	DBINT cRowsDone = 0;

	RETCODE SendRet;

	// Create objects for each column.
	for (int i = 0; i < TableColumns; i++)
       Columns.push_back(new char[*ColumnsLen[i]]);

	// Set SPID Column. Temp table always has the first column "SPID".
	sprintf_s(Columns[0], *ColumnsLen[0], "%d", SPID);
	
	try
	{
		// Initialize the bulk copy.  
		retcode = bcp_init(hdbc, (const char*)ParamList, NULL, "bcp_log.txt", DB_IN);
		if (retcode != SUCCEED)
			throw SQLException("\n ERROR: StartBCP failed! bcp_init\n", retcode);


		// Set code page.
		if (this->OEMToANSI == 1)
			retcode = bcp_control(hdbc, BCPFILECP, (void *)1251);
		else 
			retcode = bcp_control(hdbc, BCPFILECP, (void *)BCPFILECP_OEMCP);

		if (retcode == FAIL)
			throw SQLException("\n ERROR: StartBCP failed! bcp_control(CP)\n", retcode);
		
		// Bind a variables to a table columns.
		for (int i = 0; i < TableColumns; i++)
		{
			if (!IsNumeric[i])
				retcode = bcp_bind(
					hdbc,
					(BYTE *)Columns[i],
					NULL,
					SQL_VARLEN_DATA,
					(UCHAR *)&terminator,
					1,
					SQLCHARACTER,
					i + 1);
			else 
				retcode = bcp_bind(
					hdbc,
					(BYTE *)Columns[i],
					NULL,
					SQL_VARLEN_DATA,
					(UCHAR *)&terminator,
					1,
					SQLNUMERICN,
					i + 1);

			if ((retcode != SUCCEED))
				throw SQLException("\n ERROR: StartBCP failed! bcp_bind\n", retcode);
		}


		for (unsigned long i = firstrow; i <= lastrow; i++)
		{
			cur_str = file_str[i];

			// Set num column.
			sprintf_s(Columns[1], *ColumnsLen[1], "%d", str_num);

			if (ShowDiagInfo)
			{
				std::cout << "\n Col " << 0 << " " << Columns[0];
				std::cout << "\n Col " << 1 << " " << Columns[1];
			}

			it = 2, cur_pos = 0, last_pos = 0;

			if (it < TableColumns - 1)
			{
				while ((cur_pos = cur_str.find(fieldterm_str, cur_pos)) != std::string::npos)
				{
					if (it > Columns.size() - 1)
						break;
					else
					{
						copy_field = cur_str.substr(last_pos, cur_pos - last_pos);

						// For those case when a field has a bigger length than a table column length
						if (copy_field.length() > *ColumnsLen[it])
							strcpy_s(Columns[it], *ColumnsLen[it] + 1, (cur_str.substr(last_pos, *ColumnsLen[it] )).c_str());
						else
							strcpy_s(Columns[it], *ColumnsLen[it] + 1, (copy_field).c_str());

						if (ShowDiagInfo)
						{
							std::cout << "\n Col " << it << " " << Columns[it];
						}

						last_pos = cur_pos + 1;
						cur_pos++;
						it++;
					}
				}
			}
			if (it == TableColumns - 1)
			{
				cur_pos = cur_str.find(rowterm_str, cur_pos);
				strcpy_s(Columns[it], *ColumnsLen[it], (cur_str.substr(last_pos, cur_pos - last_pos)).c_str());

				if (ShowDiagInfo)
				{
					std::cout << "\n Col " << it << " " << Columns[it];
				}

				last_pos = cur_pos - 1;
				it++;
			}

			// For those case when not all columns exists in the input file.
			if (it < TableColumns - 1)
			{
				while (it <= TableColumns - 1)
				{
					strcpy_s(Columns[it], *ColumnsLen[it], " ");

					if (ShowDiagInfo)
					{
						std::cout << "\n Col " << it << " " << Columns[it];
					}

					it++;
				}
			}

			// And send this row to the table.
			SendRet = bcp_sendrow(hdbc);
			if (SendRet != SUCCEED)
			{
				//throw SQLException("\n bcp_sendrow failed! retcode:", SendRet);
				std::cout << "\n ERROR:  bcp_sendrow failed! retcode: " << SendRet;
			}	
			str_num++;
		}
		     
		// Signal the end of the bulk copy operation.  
		cRowsDone = bcp_done(hdbc);
		if ((cRowsDone == -1)) 
		{
			std::cout << "\n ERROR:  bcp_done(hdbc) Failed\n\n";			
		}

		std::string LogMessage = " Rows copied: " + std::to_string(cRowsDone);
		ProcLogWriter.Push(LogMessage, INFO_MESSAGE, 1);
	}
	catch (SQLException &ex)
	{
		ex.ShowMessage(hstmt);
	}

	return cRowsDone;
} // End of StartBCP
//-----------------------------------------------------------------------------------


// PrepareFormula()
/////////////////////////////////////////////////////////////////////////////////////
void FALoad::PrepareFormula(std::string &Formula, std::vector<Param *> &Params)
{
	try
	{ 
		// Delete comments from the formula.
		size_t
			BeginCommentSymPos = 0,
			EndCommentSymPos = 0,
			OneLineCommentSymPos = 0,
			EndLinePos = 0;

		std::string
			BeginCommentSym = "/*",
			EndCommentSym = "*/",
			OneLineCommentSym = "--";

		while ((BeginCommentSymPos = Formula.find(BeginCommentSym, BeginCommentSymPos)) != std::string::npos)
		{
			EndCommentSymPos = BeginCommentSymPos;

			while ((EndCommentSymPos = Formula.find(EndCommentSym, EndCommentSymPos)) != std::string::npos)
			{				
				Formula.erase(BeginCommentSymPos, EndCommentSymPos - BeginCommentSymPos + 2);
				BeginCommentSymPos += EndCommentSymPos;

				break;
			}

			if (BeginCommentSymPos == EndCommentSymPos)
				EndCommentSymPos = Formula.length() - 1;

			BeginCommentSymPos++;
		}


		while ((OneLineCommentSymPos = Formula.find(OneLineCommentSym, OneLineCommentSymPos)) != std::string::npos)
		{
			EndLinePos = 0;
			while ((EndLinePos = Formula.find("\n", OneLineCommentSymPos)) != std::string::npos)
			{
				Formula.erase(OneLineCommentSymPos, EndLinePos - OneLineCommentSymPos);
				OneLineCommentSymPos = EndLinePos;
				break;
			}
			
			OneLineCommentSymPos++;
		}

		for (int i = 0; i < Params.size(); i++)
		{
			if (strcmp(Params[i]->CalcField, "DealProtocolID") == 0)
			{
				SQLCHAR SQLGetNewImportProtocolID[] =
					" set nocount on " 
					" declare @RetVal int, @ID DSIDENTIFIER, @CurDate DSOPERDAY "
					" select @CurDate = CONVERT (date, GETDATE())  "
					" exec @RetVal = ImportProtocol_Insert @ID out, ?, @CurDate, @CurDate " 
					" select @RetVal, @ID ";

				SQLINTEGER
					cbRetVal = 0,
					cbImportProtocolID = 0,
					cbImportFileID = 0,
					RetVal = 0;

				double ImportProtocolID, ImportID;

				ImportID = this->ImportFileID;

				// Bind ImportFileID
				retcode = SQLBindParameter(
					hstmt,
					1,
					SQL_PARAM_INPUT,
					SQL_C_DOUBLE,
					SQL_DOUBLE,
					15,
					0,
					&ImportID,
					sizeof(ImportID),
					&cbImportFileID);

				if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
				{
					throw SQLException("\n ERROR: GetLoadInfo failed! SQLBindParameter(Brief)\n", retcode);
				}

				retcode = SQLExecDirect(
					hstmt,
					SQLGetNewImportProtocolID,
					SQL_NTS);

				if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO && retcode != SQL_NEED_DATA)
					throw SQLException("\n ERROR: PrepareFormula failed! SQLExecDirect(SQLGetNewImportProtocolID)\n", retcode);
			

				if (retcode == SQL_SUCCESS)
				{
					while (TRUE)
					{
						retcode = SQLFetch(hstmt);
						if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
						{
							throw SQLException("\n ERROR: PrepareFormula failed! SQLFetch\n", retcode);
						}

						else if (retcode == SQL_SUCCESS)
						{
							// RetVal								
							retcode = SQLGetData(
								hstmt,
								1,
								SQL_C_ULONG,
								&RetVal,
								0,
								&cbRetVal);

							if (retcode != SQL_SUCCESS)
								throw SQLException("\n ERROR: PrepareFormula failed! SQLGetData(RetVal)\n", retcode);							

							// ImportProtocolID							
							retcode = SQLGetData(
								hstmt,
								2,
								SQL_C_DOUBLE,
								&ImportProtocolID,
								0,
								&cbImportProtocolID);

							if (retcode != SQL_SUCCESS)
								throw SQLException("\n ERROR: PrepareFormula failed! SQLGetData(ImportProtocolID)\n", retcode);
						}
						else
							break;
					}
				}

				SQLRETURN closeCursRet = SQLCloseCursor(hstmt);
				if (closeCursRet != SQL_SUCCESS)
					throw SQLException("\n ERROR: PrepareFormumla failed! SQLCloseCursor\n", retcode);

				retcode = SQLFreeStmt(hstmt, SQL_CLOSE);

				if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
					throw SQLException("\n ERROR: PrepareFormula failed! SQLFreeStmt failed!\n", retcode);

				// Check @RetVal 
				if (RetVal == 0)
				{	
					Params[i]->Value = std::to_string(ImportProtocolID);
					int pos = 0;
					pos = Params[i]->Value.find(",", pos);
					Params[i]->Value.erase(pos, Params[i]->Value.length() - pos);
				}
				else
					throw SQLException("\n ERROR: ImportProtocol_Insert returned retval!\n", retcode);

			
			}
			else if (strcmp(Params[i]->CalcField, "Branch") == 0)
			{

				if (Params[i]->CalcProperty == 0)
				  Params[i]->Value = std::to_string(this->BranchID);
				else 
					Params[i]->Value = std::string("'") + std::to_string(BranchID) + std::string("'");
			}
			else if (strcmp(Params[i]->CalcField, "FileName") == 0)
			{
				Params[i]->Value = std::string("'") + FileName + std::string("'");
			}
			else
			{ 
				// Requests value from user, if it is not initialized 
				if (Params[i]->Value == "")
				{
					std::string Value;
					std::cout << "\n Enter " << Params[i]->CalcField << ": ";
					std::cin >> Value;

					if (Params[i]->CalcProperty == 0)
						Params[i]->Value = Value;
					else
						Params[i]->Value = std::string("'") + Value + std::string("'");
				}
			}

			// Change formula's text
			std::string finding_str;

			finding_str = std::string("%") + std::to_string(static_cast<int>(Params[i]->Number)) + std::string("!");

			if (ShowDiagInfo)
			  std::cout << "\n finding str " << finding_str;

			int cur_pos = 0;

			while ((cur_pos = Formula.find(finding_str, cur_pos)) != std::string::npos)
			{
				Formula.replace(cur_pos, finding_str.length(), Params[i]->Value);
				cur_pos++;
			}

			cur_pos = 0;

            // Set nocount setting, if not setted.
			std::string nocount_on_str = " set nocount on ";
			if (cur_pos = Formula.find(nocount_on_str, cur_pos) == std::string::npos)
				Formula.insert(0, nocount_on_str);

		}
		if (ShowDiagInfo)
		  std::cout << "\nPrepared formula's text: \n" << Formula;

	}
	catch (SQLException &ex)
	{
		ex.ShowMessage(hstmt);
	}
} // End of PrepareFormula
//----------------------------------------------------------------------------------


// ExecuteFormula
////////////////////////////////////////////////////////////////////////////////////
void FALoad::ExecuteFormula(std::string &Formula)
{
	SQLCHAR *SQLFormula = (unsigned char *)Formula.c_str();	

	SQLRETURN mrRetcode;

	SQLINTEGER cbRetVal;

	unsigned int RetVal = 0; // For returning value from the formula

	try
	{
		retcode = SQLExecDirect(
			hstmt,
			SQLFormula,
			SQL_NTS);

		if (retcode != SQL_SUCCESS && retcode != SQL_NO_DATA)
			throw SQLException("\n ERROR: ExecuteFormula failed! SQLExecDirect(SQLFormula)\n", retcode);	
	
		do 
		{  
			if (retcode == SQL_SUCCESS)
			{	
				while (TRUE)
				{
					retcode = SQLFetch(hstmt);

					if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
						throw SQLException("\n ERROR: ExecuteFormula failed! SQLFetch\n", retcode);
					else if (retcode == SQL_SUCCESS)
					{
						// RetVal
						retcode = SQLGetData(
							hstmt,
							1,
							SQL_C_ULONG,
							&RetVal,
							10,
							&cbRetVal);

						if (retcode != SQL_SUCCESS)
							throw SQLException("\n ERROR: ExecuteFormula failed! SQLGetData(RetVal)\n", retcode);
					}
					else
						break;
				}

			}
		} while (mrRetcode = SQLMoreResults == SQL_SUCCESS);

		//std::cout << "\n RetVal " << RetVal;

		retcode = SQLFreeStmt(hstmt, SQL_CLOSE);
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		  throw SQLException("\n ERROR: ExecuteFormula failed! SQLFreeStmt failed!\n", retcode);

	}
	catch (SQLException &ex)
	{
		ex.ShowMessage(hstmt);
	}	
	
} // End of ExecuteFormula
//----------------------------------------------------------------------------------


// SetProcessingRange
/////////////////////////////////////////////////////////////////////////////////////
void FALoad::SetProcessingRange(unsigned long &first, unsigned long&last)
{
	this->FirstRow = first;
	this->LastRow = last;

} // End of SetProcessingRange
//-----------------------------------------------------------------------------------