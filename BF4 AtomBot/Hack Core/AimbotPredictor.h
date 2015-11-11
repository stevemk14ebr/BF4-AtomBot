#pragma once

class AimbotPredictor
{
public:
	float PredictLocation(ClientSoldierEntity* pLocalEnt, ClientControllableEntity* pEnemy,SM::Vector3& AimPoint,const SM::Matrix& ShootSpace);
private:
	float DoPrediction(const SM::Vector3& ShootSpace, SM::Vector3& AimPoint, const SM::Vector3& MyVelocity, const SM::Vector3& EnemyVelocity, const SM::Vector3& BulletSpeed,const float Gravity,const WeaponZeroingEntry& Zero);
};

float AimbotPredictor::PredictLocation(ClientSoldierEntity* pLocalEnt, ClientControllableEntity* pEnemy, SM::Vector3& AimPoint,const SM::Matrix& ShootSpace)
{
	if (!PLH::IsValidPtr(pLocalEnt))
		return 0;

	if (!PLH::IsValidPtr(pEnemy))
		return 0;

	SoldierWeaponComponent* pWepComp = pLocalEnt->m_pWeaponComponent;
	if (!PLH::IsValidPtr(pWepComp))
		return 0;

	SoldierWeapon* pWeapon = pWepComp->GetActiveSoldierWeapon();
	if (!PLH::IsValidPtr(pWeapon))
		return 0;

	ClientWeapon* pClientWeapon = pWeapon->m_pWeapon;
	if (!PLH::IsValidPtr(pClientWeapon))
		return 0;

	WeaponFiring* pWepFiring = *(WeaponFiring**)OFFSET_PPCURRENTWEAPONFIRING;
	if (!PLH::IsValidPtr(pWepFiring))
		return 0;

	PrimaryFire* pFiring = pWepFiring->m_pPrimaryFire;
	if (!PLH::IsValidPtr(pFiring))
		return 0;

	FiringFunctionData* pFFD = pFiring->m_FiringData;
	if (!PLH::IsValidPtr(pFFD))
		return 0;

	WeaponModifier* pWepModifier = pClientWeapon->m_pWeaponModifier;
	WeaponZeroingEntry ZeroEntry = WeaponZeroingEntry(-1.0f, -1.0f);
	if (PLH::IsValidPtr(pWepModifier))
	{
		ZeroingModifier* pZeroing = pWepModifier->m_ZeroingModifier;
		if (PLH::IsValidPtr(pZeroing))
		{
			int m_ZeroLevelIndex = pWepComp->m_ZeroingDistanceLevel;
			ZeroEntry = pZeroing->GetZeroLevelAt(m_ZeroLevelIndex);
		}
	}

	SM::Vector4 SpawnOffset4 = pFFD->m_ShotConfigData.m_PositionOffset;
	SM::Vector3 SpawnOffset = SM::Vector3(SpawnOffset4.x, SpawnOffset4.y, SpawnOffset4.z);
	SM::Vector4 InitialSpeed4 = pFFD->m_ShotConfigData.m_Speed;
	SM::Vector3 InitialSpeed = SM::Vector3(InitialSpeed4.x, InitialSpeed4.y, InitialSpeed4.z);

	BulletEntityData* pBullet = pFFD->m_ShotConfigData.m_ProjectileData;
	if (!PLH::IsValidPtr(pBullet))
		return 0;

	float Gravity=pBullet->m_Gravity;

	SM::Vector3 MyVelocity = *pLocalEnt->GetVelocity();
	SM::Vector3 EnemyVelocity = *pEnemy->GetVelocity();
	return DoPrediction(ShootSpace.Translation()+SpawnOffset, AimPoint, MyVelocity, EnemyVelocity, InitialSpeed, Gravity,ZeroEntry);
}

float AimbotPredictor::DoPrediction(const SM::Vector3& ShootSpace, SM::Vector3& AimPoint, const SM::Vector3& MyVelocity, const SM::Vector3& EnemyVelocity, const SM::Vector3& BulletSpeed, const float Gravity, const WeaponZeroingEntry& Zero)
{
	SM::Vector3 Dist = (AimPoint - ShootSpace);
	float Distance = Dist.Length();
	float AirTime = Distance / fabs(BulletSpeed.z);
	AimPoint += EnemyVelocity*AirTime;

	float HorizDistance = Length2D(Dist);
	float HorizontalAirTime = HorizDistance / fabs(BulletSpeed.z);
	float Drop = (.5f*fabs(Gravity)*HorizontalAirTime*HorizontalAirTime);
	AimPoint.y += Drop;

	if (Zero.m_ZeroDistance == -1.0f)
		return 0.0f;

	float ZeroAirTime = Zero.m_ZeroDistance / fabs(BulletSpeed.z);
	float ZeroDrop = (.5f*fabs(Gravity)*ZeroAirTime*ZeroAirTime);
	float Theta = atan2(ZeroDrop, Zero.m_ZeroDistance);
	return Theta;
}

