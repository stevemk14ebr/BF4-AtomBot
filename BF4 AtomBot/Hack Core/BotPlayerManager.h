#pragma once
#include <vector>
struct AimbotTarget
{
	ClientPlayer* m_Player;
	SM::Vector3 m_WorldPosition;
	bool m_HasTarget;
	AimbotTarget()
	{
		m_Player = nullptr;
		m_HasTarget = false;
	}
};

class BotPlayerManager
{
public:
	BotPlayerManager();
	void UpdatePlayers();
	std::vector<ClientPlayer*>& GetPlayerList();
	ClientPlayer* GetLocalPlayer();
	AimbotTarget& GetClosestCrosshairPlayer();
	void SetAimbotSettings(AimbotSettings& Settings);
protected:
	void UpdateAllPlayerList(); //general list
	void UpdateClosestCrosshairList();
	float GetScreenDistance(SM::Vector3& EnemyScreen, SM::Vector2 ScreenSize);
private:
	std::vector<ClientPlayer*> m_PlayerList;
	AimbotTarget m_ClosestPlayer;
	AimbotSettings m_BotSettings;
};

BotPlayerManager::BotPlayerManager()
{
	
}

void BotPlayerManager::UpdatePlayers()
{
	UpdateAllPlayerList();
	UpdateClosestCrosshairList();
}

void BotPlayerManager::UpdateAllPlayerList()
{
	Main* pMain = Main::GetInstance();
	if (!PLH::IsValidPtr(pMain))
		return;

	Client* pClient = pMain->m_pClient;
	if (!PLH::IsValidPtr(pClient))
		return;

	ClientGameContext* pGameContext = pClient->m_pGameContext;
	if (!PLH::IsValidPtr(pGameContext))
		return;

	PlayerManager* pPlayerMngr = pGameContext->m_pPlayerManager;
	if (!PLH::IsValidPtr(pPlayerMngr))
		return;

	m_PlayerList.clear();
	for (int i = 0; i < 70; i++)
	{
		ClientPlayer* pPlayer = pPlayerMngr->GetPlayerById(i);
		if(!PLH::IsValidPtr(pPlayer))
			continue;

		m_PlayerList.push_back(pPlayer);
	}
}

void BotPlayerManager::UpdateClosestCrosshairList()
{
	float ClosestDistance = 99999.0f;
	ClientPlayer* ClosestPlayer=nullptr;
	SM::Vector3 ClosestWorldPos;
	bool TargetFoundThisFrame = false;
	m_ClosestPlayer.m_HasTarget = false;

	ClientPlayer* pLocalPlayer = GetLocalPlayer();
	if (!PLH::IsValidPtr(pLocalPlayer))
		return;

	for (ClientPlayer* pPlayer : m_PlayerList)
	{
		if(!PLH::IsValidPtr(pPlayer))
			continue;

		if(pPlayer->m_TeamId == pLocalPlayer->m_TeamId)
			continue;

		ClientSoldierEntity* pSoldier = pPlayer->GetSoldierEntity();
		if(!PLH::IsValidPtr(pSoldier))
			continue;

		if(pSoldier->m_Occluded)
			continue;

		RagdollComponent* pRagdoll = pSoldier->m_pRagdollComponent;
		if(!PLH::IsValidPtr(pRagdoll))
			continue;

		SM::Vector3 HeadVec;
		if(!pRagdoll->GetBone(m_BotSettings.m_AimBone, HeadVec))
			continue;

		SM::Vector3 ScreenVec;
		if(!WorldToScreen(HeadVec,ScreenVec))
			continue;

		SM::Vector2 ScreenSize = GetScreenSize();
		if(ScreenVec.x > ScreenSize.x || ScreenVec.y > ScreenSize.y)
			continue;

		float ScreenDist = GetScreenDistance(ScreenVec, GetScreenSize());
		if (ScreenDist < ClosestDistance)
		{
			ClosestDistance = ScreenDist;
			ClosestPlayer = pPlayer;
			ClosestWorldPos = HeadVec;
			TargetFoundThisFrame = true;
		}
	}
	if (!TargetFoundThisFrame)
		return;

	m_ClosestPlayer.m_HasTarget = true;
	m_ClosestPlayer.m_Player = ClosestPlayer;
	m_ClosestPlayer.m_WorldPosition = ClosestWorldPos;
}

void BotPlayerManager::SetAimbotSettings(AimbotSettings& Settings)
{
	m_BotSettings = Settings;
}

std::vector<ClientPlayer*>& BotPlayerManager::GetPlayerList()
{
	return m_PlayerList;
}

AimbotTarget& BotPlayerManager::GetClosestCrosshairPlayer()
{
	return m_ClosestPlayer;
}

ClientPlayer* BotPlayerManager::GetLocalPlayer()
{
	Main* pMain = Main::GetInstance();
	if (!PLH::IsValidPtr(pMain))
		return nullptr;

	Client* pClient = pMain->m_pClient;
	if (!PLH::IsValidPtr(pClient))
		return nullptr;

	ClientGameContext* pGameContext = pClient->m_pGameContext;
	if (!PLH::IsValidPtr(pGameContext))
		return nullptr;

	PlayerManager* pPlayerMngr = pGameContext->m_pPlayerManager;
	if (!PLH::IsValidPtr(pPlayerMngr))
		return nullptr;

	return pPlayerMngr->m_pLocalPlayer;
}

float BotPlayerManager::GetScreenDistance(SM::Vector3& EnemyScreen, SM::Vector2 ScreenSize)
{
	ScreenSize /= 2;
	SM::Vector2 DistanceFromCenter = SM::Vector2(EnemyScreen.x, EnemyScreen.y) - ScreenSize;
	return DistanceFromCenter.Length();
}