/*************************************************************
*
* Solution   : Mafia 2 Multiplayer
* Project    : Client
* File       : CM2Hud.cpp
* Developers : AaronLad <aaron@m2-multiplayer.com>
*
***************************************************************/

#include "BaseInc.h"

#include "CM2Hud.h"

#include "COffsets.h"
#include "CPatcher.h"

#include "CLua.h"

CM2HudTimer::CM2HudTimer( M2HudTimer * pHudTimer )
{
#ifdef _DEBUG
	CLogFile::Printf( "CM2HudTimer::CM2HudTimer( 0x%p )", pHudTimer );
#endif

	// Set the hud timer
	SetHudTimer( pHudTimer );
}

CM2HudTimer::~CM2HudTimer( void )
{

}

void CM2HudTimer::Toggle( bool bToggle )
{
	// Get the hud component
	void * pHudComponent = m_pHudTimer->m_pHudComponent;

	_asm
	{
		push bToggle;
		mov ecx, pHudComponent;
		call COffsets::FUNC_CHudComponent__Show;
	}
}

void CM2HudTimer::SetTime( float fTime )
{
	// Is the hud timer instance valid?
	if( m_pHudTimer )
	{
		// Set the timer time
		m_pHudTimer->m_fTime = fTime;

		// Set the timer remaining time
		m_pHudTimer->m_fRemainingTime = fTime;
	}
}

float CM2HudTimer::GetTime( void )
{
	// Is the hud timer instance valid?
	if( m_pHudTimer )
		return m_pHudTimer->m_fTime;

	return 0.0f;
}

float CM2HudTimer::GetRemainingTime( void )
{
	// Is the hud timer instance valid?
	if( m_pHudTimer )
		return m_pHudTimer->m_fRemainingTime;

	return 0.0f;
}

void CM2HudTimer::Start( void )
{
	// Is the hud timer instance valid?
	if( m_pHudTimer )
	{
		// Mark the timer as running
		m_pHudTimer->m_bIsRunning = true;

		// Mark the timer as not stopped
		m_pHudTimer->m_bIsStopped = false;
	}
}

void CM2HudTimer::Stop( void )
{
	// Is the hud timer instance valid?
	if( m_pHudTimer )
	{
		// Mark the timer as not running
		m_pHudTimer->m_bIsRunning = false;

		// Mark the timer as stopped
		m_pHudTimer->m_bIsStopped = true;
	}
}

bool CM2HudTimer::IsRunning( void )
{
	// Is the hud timer instance valid?
	if( m_pHudTimer )
		return m_pHudTimer->m_bIsRunning;

	return false;
}

bool CM2HudTimer::IsStopped( void )
{
	// Is the hud timer instance valid?
	if( m_pHudTimer )
		return m_pHudTimer->m_bIsStopped;
	
	return false;
}

CM2Hud::CM2Hud( M2Hud * pHud )
{
#ifdef _DEBUG
	CLogFile::Printf( "CM2Hud::CM2Hud( 0x%p )", pHud );
#endif

	// Set the hud
	SetHud( pHud );

	// Set the hud timer
	SetHudTimer( new CM2HudTimer( pHud->m_pTimer ) );

	// Set the hud fader
	SetHudFader( *(M2HudFader **)COffsets::VAR_CHudFader );

	// Reset
	m_bShowing = true;
	m_drunkLevel = 0;
	m_wantedLevel = 0;
	m_bTargetGPS = false;
}

CM2Hud::~CM2Hud( void )
{
	// Is the hud timer instance valid?
	if( m_pHudTimer )
	{
		// Delete the hud timer instance
		SAFE_DELETE( m_pHudTimer );
	}
}

void CM2Hud::FadeIn( float fTime )
{
	/*// Is the hud valid?
	if( m_pHud )
	{
		// Fade the screen in
		void * pScreen = m_pHudFader->m_pScreen;
		fTime *= 0.001000000047497451;

		_asm
		{
			push		0
			push		1
			lea			ecx, fTime
			push		ecx
			lea			ecx, fTime
			push		ecx
			mov			ecx, pScreen
			call		COffsets::FUNC_CHud__FaderFadeIn
		}
	}*/

	CLua::Executef( "game.hud:FaderFadeIn(%d)", (int)fTime );
}

void CM2Hud::FadeOut( float fTime )
{
	/*
	// Is the hud fader instance valid?
	if( m_pHudFader )
	{
		// Fade the screen out
		void * pScreen = m_pHudFader->m_pScreen;
		fTime *= 0.001000000047497451;
		void * pSyncObject = NULL;

		_asm
		{
			push		0
			push		1
			push		fTime
			lea			ecx, pSyncObject
			push		ecx
			mov			ecx, pScreen
			call		COffsets::FUNC_CHud__FaderFadeOut
		}
	}*/

	CLua::Executef( "game.hud:FaderFadeOut(%d)", (int)fTime );
}

void CM2Hud::Show( bool bShow )
{
	// Toggle the radar
	RadarShow( bShow );

	// Toggle the subtitles
	SubtitlesShow( bShow );

	// Toggle the speedo
	SpeedoShow( bShow );

	// Toggle the action buttons
	ActionButtonsShow( bShow );

	// Toggle the message area
	MessageAreaShow( bShow );

	// Toggle the info area
	InfoAreaShow( bShow );

	// Save the state
	m_bShowing = bShow;
}

void CM2Hud::RadarShow( bool bShow )
{
	// Is the hud instance valid?
	if( m_pHud )
	{
		// Toggle the radar
		void* pRadar = m_pHud->m_pRadar;

		_asm
		{
			push bShow;
			mov ecx, pRadar;
			call COffsets::FUNC_CHud__RadarShow;
		}
	}
}

void CM2Hud::ScoreShow( bool bShow )
{
	// Is the hud valid?
	if( m_pHud )
	{
		void* pScore = m_pHud->m_pScore;

		_asm
		{
			push bShow;
			mov ecx, pScore;
			call COffsets::FUNC_CHud__ScoreShow;
		}
	}
}

void CM2Hud::SubtitlesShow( bool bShow )
{
	// Is the hud valid?
	if( m_pHud )
	{
		void* pSubtitles = m_pHud->m_pSubtitles;
		bool bFix = !bShow;

		_asm
		{
			push bFix;
			mov ecx, pSubtitles;
			call COffsets::FUNC_CHud__SubtitlesShow;
		}
	}
}

void CM2Hud::ActionButtonsShow( bool bShow )
{
	// Is the hud valid?
	if( m_pHud )
	{
		void* pActionButtons = m_pHud->m_pMessageArea->m_pActionButtons;

		_asm
		{
			push bShow;
			mov ecx, pActionButtons;
			call COffsets::FUNC_CHudComponent__Show;
		}
	}
}

void CM2Hud::MessageAreaShow( bool bShow )
{
	// Is the hud valid?
	if( m_pHud )
	{
		void* pMessageArea = m_pHud->m_pMessageArea->m_pMessageArea;
		void* pMessageArea2 = m_pHud->m_pMessageArea->m_pMessageArea2;

		_asm
		{
			push bShow;
			mov ecx, pMessageArea;
			call COffsets::FUNC_CHudComponent__Show;
		}

		_asm
		{
			push bShow;
			mov ecx, pMessageArea2;
			call COffsets::FUNC_CHudComponent__Show;
		}
	}
}

void CM2Hud::InfoAreaShow( bool bShow )
{
	// Is the hud valid?
	if( m_pHud )
	{
		void* pInfoArea = m_pHud->m_pMessageArea->m_pInfoArea;

		_asm
		{
			push bShow;
			mov ecx, pInfoArea;
			call COffsets::FUNC_CHudComponent__Show;
		}
	}
}

void CM2Hud::ShowLowHealthFX( bool bShow )
{
	// Is the hud valid?
	if( m_pHud )
		m_pHud->m_pHealthFX->m_bShow = bShow;
}

void CM2Hud::SpeedoShow ( bool bShow )
{
	// Is the hud valid?
	if ( m_pHud )
	{
		// C_HudSpeedo::Show
		DWORD C_HudSpeedo__Show = 0x8818D0;
		M2HudSpeedo * pSpeedo = m_pHud->m_pSpeedo;

		_asm
		{
			push bShow;
			mov ecx, pSpeedo;
			call C_HudSpeedo__Show;
		}
	}
}

void CM2Hud::ShowMessage(int position, int showMode, const char * text, float delay)
{
	/*
	Pos 0 : top left corner
	Pos 1 : bottom left corner
	Pos 2 : Just beside the radar
	Pos 3 : Center of the screen
	
	Mode 0 : Grey
	Mode 1 : Red/Yellow
	Mode 2 : Blue
	Mode 3 : Fluo blue/green
	*/
	if (m_pHud)
	{
		/*
		This workig with textId
		*/
		DWORD dwFunc = 0x08EAB80;
		__asm
		{
			push 0;
			push delay;
			push showMode;
			push 0;
			push 0;
			push 0;
			push 0;
			push text;
			push position;
			call dwFunc;
		}

		
		/*DWORD dwFunc = 0x8CC740;

		__asm
		{
			push 0;
			push 0;
			push delay;
			push showMode; // Showmode
			push text;
			push position; // Position
			call dwFunc;
		};*/
	}
}

void CM2Hud::SetDrunkLevel(int level)
{
	if (level > 0){
		CLua::Execute("game.hud:EffectAlcoholHit()"); // Initiate drunk effect
	}
	CLua::Executef( "game.hud:EffectAlcoholDrunk(\"%d\")", level ); // Set drunk level

	m_drunkLevel = level;
}

void CM2Hud::SetWantedLevel(int level, float size)
{
	/* Works but quickly reset */
	if (m_pHud)
	{
		DWORD dwFunc = 0x883860;
		void * wanted = m_pHud->m_pWantedLevel;
		float value = 1.0f; // Still unknow
		float value2 = 1.0f; // Size of the pic
		_asm
		{
			push value2;
			push value;
			push level;
			mov ecx, wanted;
			call dwFunc;
		}
	}
}

void CM2Hud::StartGPS(float fX, float fY)
{
	if (!m_pHud)
		return;

	if (m_bTargetGPS)
		return;

	CLua::Execute("l_8_1 = game.quests:NewQuest(5, 12, true, \"0056000005\", \"0056000005\")");
	CLua::Executef("pos = game.navigation:RegisterObjectivePos(%f, %f, l_8_1:AddObjective(\"0551010006\"))", fX, fY);
	CLua::Executef("game.navigation:SetFakeTargetForGPS(pos, %f, %f)", fX, fY);

	m_bTargetGPS = true;
}

void CM2Hud::StopGPS()
{
	if (!m_pHud)
		return;

	if (!m_bTargetGPS)
		return;

	CLua::Execute("game.navigation:UnregisterObjectivePos(pos)");
	m_bTargetGPS = false;
}

void CM2Hud::EnableFPV(bool bEnable)
{
	if (bEnable){
		CLua::Execute("game.cameramanager:GetPlayerMainCamera(0):EnableFPV(game.game:GetActivePlayer(),true)");
	}
	else {
		CLua::Execute("game.cameramanager:GetPlayerMainCamera(0):EnableFPV(game.game:GetActivePlayer(),false)");
	}
}