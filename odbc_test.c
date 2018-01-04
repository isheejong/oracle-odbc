#include <windows.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>

// http://www.oracle.com/partners/ko/knowledge-zone/database/index-098976.html
/*
* Defines
*/
#define JOB_LEN 9
#define DATA_LEN 100
#define SQL_STMT_LEN 500

/*
* Procedures
*/
void DisplayError( SWORD HandleType, SQLHANDLE hHandle, char *Module );

/*
* Main Program
*/
int main()
{
      SQLHENV hEnv;
      SQLHDBC hDbc;
      SQLHSTMT hStmt;
      SQLRETURN rc;
      char *DefUserName ="heejong";
      char *DefPassWord ="Welcome1";
      SQLCHAR ServerName[DATA_LEN];
      SQLCHAR *pServerName="//129.150.72.215:1521/pdb1";
      SQLCHAR UserName[DATA_LEN];
      SQLCHAR *pUserName=UserName;
      SQLCHAR PassWord[DATA_LEN];
      SQLCHAR *pPassWord=PassWord;
      char Data[DATA_LEN];
      SQLINTEGER DataLen;
      char error[DATA_LEN];
      char *charptr;
      SQLCHAR SqlStmt[SQL_STMT_LEN];
      SQLCHAR *pSqlStmt=SqlStmt;
      char *pSalesMan = "SALESMAN";
      SQLINTEGER sqlnts=SQL_NTS;

      /*
      * Allocate the Environment Handle
      */
      rc = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv );
      if (rc != SQL_SUCCESS)
      {
            printf( "Cannot Allocate Environment Handle/n");
            printf( "/nHit Return to Exit/n");
            charptr = gets ((char *)error);
            exit(1);
      }

      /*
      * Set the ODBC Version
      */
      rc = SQLSetEnvAttr( hEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
      if (rc != SQL_SUCCESS)
      {
            printf( "Cannot Set ODBC Version/n");
            printf( "/nHit Return to Exit/n");
            charptr = gets ((char *)error);
            exit(1);
      }


      /*
      * Allocate the Connection handle
      */
      rc = SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hDbc );
      if (rc != SQL_SUCCESS)
      {
            printf( "Cannot Allocate Connection Handle/n");
            printf( "/nHit Return to Exit/n");
            charptr = gets ((char *)error);
            exit(1);
      }


      /*
      * Get User Information
      */
      lstrcpy( (char *) pUserName, DefUserName );
      lstrcpy( (char *) pPassWord, DefPassWord );



      /*
      * Data Source name
      */
      printf( "/nEnter the ODBC Data Source Name/n" );
      charptr = gets ((char *) ServerName);



      /*
      * User Name
      */
      printf ( "/nEnter User Name Default [%s]/n", pUserName);
      charptr = gets ((char *) UserName);
      if (*charptr == '/0')
      {
            lstrcpy( (char *) pUserName, (char *) DefUserName );
      }



      /*
      * Password
      */
      printf ( "/nEnter Password Default [%s]/n", pPassWord);
      charptr = gets ((char *)PassWord);
      if (*charptr == '/0')
      {
            lstrcpy( (char *) pPassWord, (char *) DefPassWord );
      }



      /*
      * Connection to the database
      */
      rc = SQLConnect( hDbc, pServerName, (SQLSMALLINT) lstrlen((char *)pServerName), pUserName,
                  (SQLSMALLINT) lstrlen((char *)pUserName),
                  pPassWord,
                  (SQLSMALLINT) lstrlen((char *)pPassWord));
      if (rc != SQL_SUCCESS)
      {
            DisplayError(SQL_HANDLE_DBC, hDbc, "SQLConnect");
      }



      /*
      * Allocate a Statement
      */
      rc = SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt );
      if (rc != SQL_SUCCESS)
      {
            printf( "Cannot Allocate Statement Handle/n");
            printf( "/nHit Return to Exit/n");
            charptr = gets ((char *)error);
            exit(1);
      }


      /*
      * Bind the Parameter
      */
      rc = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT,  SQL_C_CHAR, SQL_CHAR, JOB_LEN, 0,pSalesMan, 0, &sqlnts);

      /*
      * Call the Store Procedure which executes the Result Sets
      */
      lstrcpy( (char *) pSqlStmt, "{CALL ODBCRefCur.EmpCurs(?)}");
      rc = SQLExecDirect(hStmt, pSqlStmt, lstrlen((char *)pSqlStmt));
      if (rc != SQL_SUCCESS)
      {
            DisplayError(SQL_HANDLE_STMT, hStmt, "SQLExecDirect");
      }

      /*
      * Bind the Data
      */
      rc = SQLBindCol( hStmt, 1, SQL_C_CHAR, Data,  sizeof(Data), &DataLen);
      if (rc != SQL_SUCCESS)
      {
            DisplayError(SQL_HANDLE_STMT, hStmt, "SQLBindCol");
      }



      /*
      * Get the data for Result Set 1
      */
      printf( "/nEmployee Names/n/n");
      while ( rc == SQL_SUCCESS ){
            rc = SQLFetch( hStmt );

            if ( rc == SQL_SUCCESS )
            {
                  printf("%s/n", Data);
            }else
            {
                  if (rc != SQL_NO_DATA)
                  {
                        DisplayError(SQL_HANDLE_STMT, hStmt, "SQLFetch");
                  }
            }
      }

      printf( "/nFirst Result Set - Hit Return to Continue/n");
      charptr = gets ((char *)error);

      /*
      * Get the Next Result Set
      */
      rc = SQLMoreResults( hStmt );
      if (rc != SQL_SUCCESS)
      {
            DisplayError(SQL_HANDLE_STMT, hStmt, "SQLMoreResults");
      }

      /*
      * Get the data for Result Set 2
      */
      printf( "/nManagers/n/n");
      while ( rc == SQL_SUCCESS )
      {
            rc = SQLFetch( hStmt );
            if ( rc == SQL_SUCCESS )
            {
                  printf("%s/n", Data);
            }else
            {
                  if (rc != SQL_NO_DATA)
                  {
                        DisplayError(SQL_HANDLE_STMT, hStmt, "SQLFetch");
                  }
            }
      }

      printf( "/nSecond Result Set - Hit Return to Continue/n");
      charptr = gets ((char *)error);

      /*
      * Should Be No More Results Sets
      */
      rc = SQLMoreResults( hStmt );
      if (rc != SQL_NO_DATA)
      {
      DisplayError(SQL_HANDLE_STMT, hStmt, "SQLMoreResults");
      }

      /*
      * Drop the Package
      */
      lstrcpy( (char *) pSqlStmt, "DROP PACKAGE ODBCRefCur");
      rc = SQLExecDirect(hStmt, pSqlStmt, lstrlen((char *)pSqlStmt));

      /*
      * Free handles close connections to the database
      */
      SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
      SQLDisconnect( hDbc );
      SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
      SQLFreeHandle( SQL_HANDLE_ENV, hEnv );

      printf( "/nAll Done - Hit Return to Exit/n");
      charptr = gets ((char *)error);

      return(0);
}



/*
* Display Error Messages
*/
void DisplayError( SWORD HandleType, SQLHANDLE hHandle, char *Module )
{
      SQLCHAR MessageText[255];
      SQLCHAR SQLState[80];
      SQLRETURN rc=SQL_SUCCESS;
      LONG NativeError;
      SWORD RetLen;
      SQLCHAR error[25];
      char *charptr;



      rc = SQLGetDiagRec(HandleType,  hHandle, 1, SQLState, &NativeError, MessageText, 255,  &RetLen);

      printf( "Failure Calling %s/n", Module );
      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
            printf( "/t/t/t State: %s/n", SQLState);
            printf( "/t/t/t Native Error: %d/n", NativeError );
            printf( "/t/t/t Error Message: %s/n", MessageText );
      }

      printf( "/nHit Return to Exit/n");
      charptr = gets ((char *)error);
      exit(1);
}
