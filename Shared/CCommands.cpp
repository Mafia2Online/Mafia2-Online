/*************************************************************
*
* Solution   : Mafia 2 Multiplayer
* Project    : Shared Library
* File       : CCommands.cpp
* Developers : AaronLad <aaron@m2-multiplayer.com>
*
***************************************************************/

#include	"CString.h"

#ifdef _CLIENT
#include	"../Client/BaseInc.h"
#include	"../Client/CCore.h"
#include	"CClientScriptingManager.h"
 #else
#include	"../Client/BaseInc.h"
#include	"../Server/CCore.h"
#endif

#include	"../../Libraries/squirrel/squirrel.h"
#include	"../../Libraries/squirrel/sqvm.h"

#include	"Scripting/CSquirrel.h"
#include	"Scripting/CScriptingManager.h"

#include	"CCommands.h"


void CSquirrelCommandHandler::Call( CSquirrelArguments * pArguments )
{
	// Find the script
#ifndef _CLIENT
	CSquirrel * pScript = CCore::Instance()->GetScriptingManager()->Get( m_pVM );
#else
	CSquirrel * pScript = CCore::Instance()->GetClientScriptingManager()->GetScriptingManager()->Get(m_pVM);
#endif

	if( pScript )
	{
		// Call the function
		pScript->Call( m_pFunction, pArguments );
	}
}