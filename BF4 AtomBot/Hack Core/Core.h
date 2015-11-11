#pragma once


struct ESPSettings
{
	Color m_EnemyNotVisible;
	Color m_EnemyVisible;
	Color m_FriendlyNotVisible;
	Color m_FriendlyVisible;
	bool m_Enabled;
	bool m_ShowFriendlies;
	bool m_SnapLinesEnabled;
	bool m_SkeletonEnabled;

	ESPSettings()
	{
		m_EnemyNotVisible = Color::Red();
		m_EnemyVisible = Color::Green();
		m_FriendlyNotVisible = Color(.88f, .21f, 1.f); //purple
		m_FriendlyVisible = Color::Blue();
		m_Enabled = true;
		m_ShowFriendlies = false;
		m_SnapLinesEnabled = true;
		m_SkeletonEnabled = true;
	}
};
struct AimbotSettings
{
	bool m_Enabled;
	UpdatePoseResultData::BONES m_AimBone;
	float m_MinTimeToTarget;
	float m_MaxTimeToTarget;
	
	AimbotSettings()
	{
		m_Enabled = true;
		m_AimBone = UpdatePoseResultData::BONES::Spine;
		m_MinTimeToTarget = .7f;
		m_MaxTimeToTarget = 1.2f;
	}
};
struct ClassInfos
{
	ClassInfo* m_Vehicles;
	ClassInfo* m_Bullets;
};
#include "Hack Core/BotPlayerManager.h"
#include "Hack Core/AimbotSmoother.h"
#include "Hack Core/AimbotPredictor.h"
#include "Hack Core/RandomGen.h"

class Core
{
public:
	Core();
	~Core();
	void UpdatePresent();
	void UpdatePreFrame();
	void ESP(RenderInterface* Renderer); 

	void DrawShootSpace(ClientPlayer* pLocalPlayer, RenderInterface* Renderer)
	{
		ClientSoldierEntity* pLocalSoldier = pLocalPlayer->GetSoldierEntity();
		if (!PLH::IsValidPtr(pLocalSoldier))
			return;

		SoldierWeaponComponent* pWepComp = pLocalSoldier->m_pWeaponComponent;
		if (!PLH::IsValidPtr(pWepComp))
			return;

		SoldierWeapon* pWeapon = pWepComp->GetActiveSoldierWeapon();
		if (!PLH::IsValidPtr(pWeapon))
			return;

		ClientWeapon* pClientWeapon = pWeapon->m_pWeapon;
		if (!PLH::IsValidPtr(pClientWeapon))
			return;

		
		SM::Vector3 Forward = pClientWeapon->m_ShootSpace.Backward();
		Forward.Normalize();

		SM::Vector3 Pos2 = pClientWeapon->m_ShootSpace.Translation() + (Forward * 10);
		SM::Vector3 Pos1 = pClientWeapon->m_ShootSpace.Translation() + (Forward * 1.1) ;
		if (!WorldToScreen(Pos1))
			return;

		if (!WorldToScreen(Pos2))
			return;

		Renderer->BeginLine();
		Renderer->DrawLine(Vector2f(Pos1.x,Pos1.y), Vector2f(Pos2.x, Pos2.y), Color::Red());
		Renderer->EndLine();
	}

	void Aimbot(float DeltaTime);
	void EntityESP(RenderInterface* Renderer);
	void NoRecoil();

	ESPSettings& GetESPSettings()
	{
		return m_ESPSettings;
	}
	AimbotSettings& GetAimbotSettings()
	{
		return m_AimbotSettings;
	}

	void OnBoneSelectionChanged(const uint32_t Id,const std::string& BoneName);
private:
	void FindClassInfos();
	void DumpBoneIDs();

	ClassInfos m_ClassInfos;
	ESPSettings m_ESPSettings;
	AimbotSettings m_AimbotSettings;
	AimbotSmoother m_AimbotSmoother;
	AimbotPredictor m_AimbotPredictor;
	BotPlayerManager m_PresentPlayerManager;
	BotPlayerManager m_PreFramePlyerManager;
	AimbotTarget m_PreviousTarget;
	RandomGen m_Random;
};

Core::Core()
{
	FindClassInfos();
}

Core::~Core()
{
}

void Core::UpdatePresent()
{
	//NOT THREAD SAFE
	m_PresentPlayerManager.UpdatePlayers();
}

void Core::UpdatePreFrame()
{
	m_PreFramePlyerManager.UpdatePlayers();
}

void Core::NoRecoil()
{
	ClientPlayer* pLocalPlayer = m_PreFramePlyerManager.GetLocalPlayer();
	if (!PLH::IsValidPtr(pLocalPlayer))
		return;

	if (pLocalPlayer->InVehicle())
		return;

	ClientSoldierEntity* pLocalSoldier = pLocalPlayer->GetSoldierEntity();
	if (!PLH::IsValidPtr(pLocalSoldier))
		return;

	SoldierWeaponComponent* pWepComp = pLocalSoldier->m_pWeaponComponent;
	if (!PLH::IsValidPtr(pWepComp))
		return;

	SoldierWeapon* pWeapon = pWepComp->GetActiveSoldierWeapon();
	if (!PLH::IsValidPtr(pWeapon))
		return;

	WeaponFiring* pFiring = pWeapon->m_pPrimary;
	if (!PLH::IsValidPtr(pFiring))
		return;

	WeaponSway* pSway= pFiring->m_Sway;
	if (!PLH::IsValidPtr(pSway))
		return;

	GunSwayData* pSwayData = pSway->m_Data;
	if (!PLH::IsValidPtr(pSwayData))
		return;

	pSwayData->m_FirstShotRecoilMultiplier = 0;
	pSwayData->m_ShootingRecoilDecreaseScale = 100;
}

void Core::ESP(RenderInterface* Renderer)
{
	if (!m_ESPSettings.m_Enabled)
		return;

	ClientPlayer* pLocalPlayer = m_PresentPlayerManager.GetLocalPlayer();
	if (!PLH::IsValidPtr(pLocalPlayer))
		return;

	//DrawShootSpace(pLocalPlayer, Renderer);


	for(ClientPlayer* pEnemyPlayer:m_PresentPlayerManager.GetPlayerList())
	{
		if(!PLH::IsValidPtr(pEnemyPlayer))
			continue;

		ClientSoldierEntity* pEnemySoldier = pEnemyPlayer->GetSoldierEntity();
		if (!PLH::IsValidPtr(pEnemySoldier))
			continue;

		bool IsFriendly = pEnemyPlayer->m_TeamId == pLocalPlayer->m_TeamId;
		if (!m_ESPSettings.m_ShowFriendlies && IsFriendly)
			continue;

		Color ESPColor;
		if (IsFriendly)
		{
			ESPColor = pEnemySoldier->m_Occluded ?
				m_ESPSettings.m_FriendlyNotVisible : m_ESPSettings.m_FriendlyVisible;
		}else {
			ESPColor = pEnemySoldier->m_Occluded ?
				m_ESPSettings.m_EnemyNotVisible : m_ESPSettings.m_EnemyVisible;
		}

		if (m_ESPSettings.m_SnapLinesEnabled)
		{
			SM::Vector2 ScreenSz = GetScreenSize();
			if(ScreenSz.x <1.0f || ScreenSz.y <1.0f)
				continue;

			float StartX = ScreenSz.x * .5f;
			float StartY = ScreenSz.y - (ScreenSz.y *.05f);

			SM::Matrix Mat;
			pEnemySoldier->GetTransform(&Mat);
			SM::Vector3 Trans = SM::Vector3(Mat._41, Mat._42, Mat._43);
			if(!WorldToScreen(Trans))
				continue;

			Renderer->BeginLine();
			Renderer->DrawLine(Vector2f(StartX, StartY), Vector2f(Trans.x, Trans.y), ESPColor);
			Renderer->EndLine();
		}

		if (m_ESPSettings.m_SkeletonEnabled)
		{
			RagdollComponent* pEnemyRag = pEnemySoldier->m_pRagdollComponent;
			if(!PLH::IsValidPtr(pEnemyRag))
				continue;

			/*Renderer->BeginText();
			for (int i = 0; i < pEnemyRag->m_AnimationSkeleton->m_BoneCount; i++)
			{
				SM::Vector3 BoneVec;
				if(!pEnemyRag->GetBone((UpdatePoseResultData::BONES)i, BoneVec))
					continue;

				if(!WorldToScreen(BoneVec))
					continue;

				Renderer->RenderText(Vector2f(BoneVec.x, BoneVec.y), ESPColor, "%d", i);
			}
			Renderer->EndText();*/
			Renderer->BeginLine();
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::Head, UpdatePoseResultData::Neck);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::Neck, UpdatePoseResultData::Spine2);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::Spine2, UpdatePoseResultData::Spine1);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::Spine1, UpdatePoseResultData::Spine);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::Neck, UpdatePoseResultData::LeftShoulder);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::LeftShoulder, UpdatePoseResultData::LeftElbowRoll);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::RightShoulder, UpdatePoseResultData::RightElbowRoll);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::LeftElbowRoll, UpdatePoseResultData::LeftHand);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::RightElbowRoll, UpdatePoseResultData::RightHand);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::Spine, UpdatePoseResultData::RightKneeRoll);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::Spine, UpdatePoseResultData::LeftKneeRoll);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::RightKneeRoll, UpdatePoseResultData::RightFoot);
			ConnectBones(Renderer, ESPColor, pEnemyRag, UpdatePoseResultData::LeftKneeRoll, UpdatePoseResultData::LeftFoot);
			Renderer->EndLine();
		}
		TransformDrawAABB(pEnemySoldier, Renderer, ESPColor);
	}
}

void Core::EntityESP(RenderInterface* Renderer)
{
	if (!m_ESPSettings.m_Enabled)
		return;

	Main* pMain = Main::GetInstance();
	if (!PLH::IsValidPtr(pMain))
		return;

	Client* pClient = pMain->m_pClient;
	if (!PLH::IsValidPtr(pClient))
		return;

	ClientGameContext* pGameContext = pClient->m_pGameContext;
	if (!PLH::IsValidPtr(pGameContext))
		return;

	Level* pLevel = pGameContext->m_pLevel;
	if (!PLH::IsValidPtr(pLevel))
		return;

	void* GameWorld =(void*) pLevel->m_pGameWorld;
	if (!PLH::IsValidPtr(GameWorld))
		return;

	EntityIterator<ClientControllableEntity> Vehicles(GameWorld, m_ClassInfos.m_Vehicles);
	if (Vehicles.front())
	{
		do 
		{
			ClientControllableEntity* pVeh = Vehicles.front()->getObject();
			if(!PLH::IsValidPtr(pVeh))
				continue;

			TransformDrawAABB(pVeh,Renderer, Color::White());
		} while (Vehicles.next());
	}

	EntityIterator<ClientControllableEntity> Bullets(GameWorld, m_ClassInfos.m_Bullets);
	if (Bullets.front())
	{
		do 
		{
			ClientControllableEntity* pBullet = Bullets.front()->getObject();
			if (!PLH::IsValidPtr(pBullet))
				continue;
			
			TransformDrawAABB(pBullet,Renderer,Color::Green());
		} while (Bullets.next());
	}
}

void Core::Aimbot(float DeltaTime)
{
	if (!m_AimbotSettings.m_Enabled)
		return;

	m_PreFramePlyerManager.SetAimbotSettings(m_AimbotSettings);

	if (!GetAsyncKeyState(VK_RBUTTON))
		return;

	ClientPlayer* pLocalPlayer = m_PreFramePlyerManager.GetLocalPlayer();
	if (!PLH::IsValidPtr(pLocalPlayer))
		return;

	if (pLocalPlayer->InVehicle())
		return;

	ClientSoldierEntity* pLocalSoldier = pLocalPlayer->GetSoldierEntity();
	if (!PLH::IsValidPtr(pLocalSoldier))
		return;

	SoldierWeaponComponent* pWepComp = pLocalSoldier->m_pWeaponComponent;
	if (!PLH::IsValidPtr(pWepComp))
		return;

	SoldierWeapon* pWeapon = pWepComp->GetActiveSoldierWeapon();
	if (!PLH::IsValidPtr(pWeapon))
		return;

	ClientWeapon* pClientWeapon = pWeapon->m_pWeapon;
	if (!PLH::IsValidPtr(pClientWeapon))
		return;

	SoldierAimingSimulation* pAimSim = pWeapon->m_pAuthoritativeAiming;
	if (!PLH::IsValidPtr(pAimSim))
		return;

	AimAssist* pAimAssist = pAimSim->m_pFPSAimer;
	if (!PLH::IsValidPtr(pAimAssist))
		return;

	SM::Matrix ShootSpaceMat = pClientWeapon->m_ShootSpace;

	AimbotTarget Target = m_PreFramePlyerManager.GetClosestCrosshairPlayer();
	if (!PLH::IsValidPtr(Target.m_Player))
		return;

	if (!Target.m_HasTarget)
		return;

	SM::Vector3 TempAim = Target.m_WorldPosition;
	float ZeroThetaOffset = m_AimbotPredictor.PredictLocation(pLocalSoldier, Target.m_Player->GetSoldierEntity(),
		TempAim, ShootSpaceMat);
	Target.m_WorldPosition = TempAim;

	if (Target.m_Player != m_PreviousTarget.m_Player)
		m_AimbotSmoother.ResetTimes(
			SM::Vector2(m_Random.GetRandom<float>(m_AimbotSettings.m_MinTimeToTarget, m_AimbotSettings.m_MaxTimeToTarget,false),
				m_Random.GetRandom<float>(m_AimbotSettings.m_MinTimeToTarget, m_AimbotSettings.m_MaxTimeToTarget, false)));

	m_AimbotSmoother.Update(DeltaTime);

	SM::Vector3 vDir = Target.m_WorldPosition - ShootSpaceMat.Translation();
	vDir.Normalize();

	SM::Vector2 BotAngles = {
		-atan2(vDir.x, vDir.z),
		atan2(vDir.y,Length2D(vDir))
	};

	BotAngles -= pAimSim->m_Sway;
	BotAngles.y -= ZeroThetaOffset;
	m_AimbotSmoother.SmoothAngles(pAimAssist->m_AimAngles, BotAngles);
	pAimAssist->m_AimAngles = BotAngles;
	m_PreviousTarget = Target;
}

void Core::FindClassInfos()
{
	m_ClassInfos.m_Vehicles = FindClassInfo("ClientVehicleEntity");
	m_ClassInfos.m_Bullets = FindClassInfo("VeniceClientBulletEntity");
}

void Core::DumpBoneIDs()
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

	ClientPlayer* pLocalPlayer = pPlayerMngr->m_pLocalPlayer;
	if (!PLH::IsValidPtr(pLocalPlayer))
		return;

	ClientSoldierEntity* pSoldier = pLocalPlayer->GetSoldierEntity();
	if (!PLH::IsValidPtr(pSoldier))
		return;

	RagdollComponent* pRagDoll = pSoldier->m_pRagdollComponent;
	XTrace("rag %I64X\n", pRagDoll);

	AnimationSkeleton* pSkeleton = pRagDoll->m_AnimationSkeleton;
	if (!PLH::IsValidPtr(pSkeleton))
		return;
	XTrace("skel %I64X\n", pSkeleton);

	for (int i = 0; i < pSkeleton->m_BoneCount; i++)
	{
		char* name = pSkeleton->GetBoneNameAt(i);
		XTrace("%s = %d,\n", name, i);
	}
}

void Core::OnBoneSelectionChanged(const uint32_t Id, const std::string& BoneName)
{
	if (BoneName == "Head")
	{
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::Head;
	}
	else if (BoneName == "Neck") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::Neck;
	}
	else if (BoneName == "Left Shoulder") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::LeftShoulder;
	}
	else if (BoneName == "Right Shoulder") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::RightShoulder;
	}
	else if (BoneName == "Left Elbow") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::LeftElbowRoll;
	}
	else if (BoneName == "Right Elbow") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::RightElbowRoll;
	}
	else if (BoneName == "Left Hand") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::LeftHand;
	}
	else if (BoneName == "Right Hand") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::RightHand;
	}
	else if (BoneName == "Upper Spine") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::Spine2;
	}
	else if (BoneName == "Lower Spine") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::Spine;
	}
	else if (BoneName == "Left Knee") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::LeftKneeRoll;
	}
	else if (BoneName == "Right Knee") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::RightKneeRoll;
	}
	else if (BoneName == "Left Foot") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::LeftFoot;
	}
	else if (BoneName == "Right Foot") {
		m_AimbotSettings.m_AimBone = UpdatePoseResultData::BONES::RightFoot;
	}
}