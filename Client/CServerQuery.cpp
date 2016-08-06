/*************************************************************
*
* Solution   : Mafia 2 Multiplayer
* Project    : Client
* File       : CServerQuery.cpp
* Developers : AaronLad <aaron@m2-multiplayer.com>
*
***************************************************************/

#include	"BaseInc.h"
#include	<Threading/CThread.h>
#include	<RakPeerInterface.h>
#include	<BitStream.h>
#include	<MessageIdentifiers.h>
#include	<RPC4Plugin.h>

#include	"CCore.h"

#include	"CGUI.h"
#include	"CServerBrowser.h"
#include	"CServerList.h"
#include	"CServerQuery.h"

#include	"SharedUtility.h"

#include	"CServerQuery.h"

void CServerQuery::WorkerThread ( CThread * pCreator )
{
	while ( pCreator->GetUserData < bool > () )
	{
		CServerQuery * pServerQuery = CCore::Instance()->GetGUI()->GetServerBrowser()->GetServerQuery ();
		if ( pServerQuery )
		{
			for ( std::list< CServerListItem* >::iterator iter = pServerQuery->GetQueue().begin (); iter != pServerQuery->GetQueue().end (); )
			{
				CServerListItem * pServerQueryItem = *iter;
				if ( (SharedUtility::GetTime () - pServerQueryItem->ulQueryStart) > 10000 )
				{
					CLogFile::Printf ( "Deleting stale query." );
					delete pServerQueryItem;
					pServerQuery->Remove ( pServerQueryItem );
				}
				else
					iter++;
			}

			sockaddr_in address;
			memset ( &address, 0, sizeof ( sockaddr_in ) );
			int iFromLen = sizeof ( sockaddr_in );
			static char szBuffer [ 2048 ];

			int iBytesRead = -1;
			while ( (iBytesRead = recvfrom ( pServerQuery->GetSocket (), szBuffer, sizeof ( szBuffer ), NULL, (sockaddr *)&address, &iFromLen )) != -1 )
			{
				// Is this query for M2Online?
				if ( szBuffer [ 0 ] == 'M' && szBuffer [ 1 ] == '2' && szBuffer [ 2 ] == 'O' && szBuffer [ 3 ] == 'n' && szBuffer[4] == 'l' && szBuffer[5] == 'i' && szBuffer[6] == 'n' && szBuffer[7] == 'e')
				{
					char szIpAddress [ 64 ];
					SharedUtility::GetIPFromSocketAddress(address.sin_family, &address.sin_addr, szIpAddress, sizeof(szIpAddress));
					unsigned short usPort = ntohs ( address.sin_port );

					CServerListItem * pServerItem = pServerQuery->GetQueryItem ( szIpAddress, (usPort - 1) );
					if ( !pServerItem )
						continue;

					pServerItem->Parse ( szBuffer, strlen ( szBuffer ) );

					pServerQuery->Remove ( pServerItem );
				}
				else {
					CLogFile::Printf("Received query but ignored it (%s)", szBuffer);
				}
			}
		}
		Sleep ( 50 );
	}
}

CServerQuery::CServerQuery ( void )
{
	WSADATA wsaData;
	WSAStartup ( MAKEWORD( 2, 2 ), &wsaData );

	m_iSocket = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	unsigned long sockopt = 1;
	ioctlsocket ( m_iSocket, FIONBIO, &sockopt );
}

CServerQuery::~CServerQuery ( void )
{
	Reset ();
	if ( m_iSocket != -1 )
		closesocket ( m_iSocket );
	WSACleanup ();
}

bool CServerQuery::Query ( CServerListItem * pServerItem )
{
	CLogFile::Printf("Querying... %s", pServerItem->strHost.Get());
	sockaddr_in address;
	memset ( &address, 0, sizeof ( sockaddr_in ) );
	address.sin_family = AF_INET;
	address.sin_port = htons ( pServerItem->usGamePort + 1 );
	address.sin_addr.s_addr = inet_addr ( pServerItem->strHost.Get () );

#ifdef _DEBUG
	CLogFile::Printf ( "Sending query to %s:%d...", pServerItem->strHost.Get(), pServerItem->usGamePort );
#endif

	int sentChars = sendto(m_iSocket, "M2Online", 8, NULL, (sockaddr *)&address, sizeof(sockaddr_in));
	if ( sentChars == 8 )
	{
		pServerItem->ulQueryStart = SharedUtility::GetTime ();
		m_queryQueue.push_back ( pServerItem );

		CLogFile::Printf ( "CServerQuery::Query () - Queue size: %d", m_queryQueue.size () );

		if ( !m_workerThread.IsRunning () )
		{
			m_workerThread.SetUserData< bool > ( true );
			m_workerThread.Start ( WorkerThread );
		}
		return true;
	}
	else {
#ifdef _DEBUG
		CLogFile::Printf("Sent %d chars", sentChars);
#endif
	}

	return false;
}

void CServerQuery::Remove ( CServerListItem * pServerItem )
{
	for ( std::list< CServerListItem* >::iterator iter = m_queryQueue.begin (); iter != m_queryQueue.end (); iter ++ )
	{
		if ( pServerItem == *iter )
		{
			m_queryQueue.erase ( iter );
			break;
		}
	}

	CLogFile::Printf ( "CServerQuery::Remove () - Queue size: %d", m_queryQueue.size () );

	if ( !m_queryQueue.size () )
	{
		Reset ();
	}
}

void CServerQuery::Reset ( void )
{
	m_queryQueue.clear ();
	if ( m_workerThread.IsRunning () )
	{
		m_workerThread.SetUserData< bool > ( false );
		m_workerThread.Stop ( false, true );
	}
}

CServerListItem * CServerQuery::GetQueryItem ( const char * szHost, unsigned short usPort )
{
	for ( std::list< CServerListItem* >::iterator iter = m_queryQueue.begin (); iter != m_queryQueue.end (); iter ++ )
	{
		if ( !(*iter)->strHost.Compare ( szHost ) && (*iter)->usGamePort == usPort )
			return *iter;
	}

	return NULL;
}

void CServerQuery::Process ( void )
{
	if ( !m_queryQueue.size () )
		return;

	for ( std::list< CServerListItem* >::iterator iter = m_queryQueue.begin (); iter != m_queryQueue.end (); iter ++ )
	{
		if ( (SharedUtility::GetTime () - (*iter)->ulQueryStart) > 10000 )
		{
			iter = m_queryQueue.erase ( iter );
		}
		else
			iter ++;
	}
}