/*************************************************************
*
* Solution   : Mafia 2 Multiplayer
* Project    : Shared Library
* File       : CCommands.cpp
* Developers : AaronLad <aaron@m2-multiplayer.com>
*
***************************************************************/

#ifdef _CLIENT
#include "../Client/BaseInc.h"
#include "../Client/CCore.h"
 #else
#include "../Client/BaseInc.h"
#include "../Server/CCore.h"
#endif

#include "sqvm.h"

#include "Scripting/CSquirrel.h"
#include "Scripting/CScriptingManager.h"
#include "CClientScriptingManager.h"

#include "CCommands.h"


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