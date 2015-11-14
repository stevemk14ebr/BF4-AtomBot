#pragma once
#include "Hack Core/PolynomialSolver.h"

class AimbotPredictor
{
public:
	float PredictLocation(ClientSoldierEntity* pLocalEnt, ClientControllableEntity* pEnemy,SM::Vector3& AimPoint,const SM::Matrix& ShootSpace);
private:
	float DoPrediction(const SM::Vector3& ShootSpace, SM::Vector3& AimPoint, const SM::Vector3& MyVelocity, const SM::Vector3& EnemyVelocity, const SM::Vector3& BulletSpeed,const float Gravity,const WeaponZeroingEntry& Zero);
	PolynomialSolver m_Solver;
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
	SM::Vector3 RelativePos = (AimPoint-ShootSpace);
	SM::Vector3 GravityVec = SM::Vector3(0, fabs(Gravity),0);
	auto fApproxPos = [](SM::Vector3& CurPos,const SM::Vector3& Velocity,const SM::Vector3& Accel,const float Time)->SM::Vector3 {
		return CurPos + Velocity*Time + .5f*Accel*Time*Time;                               
	};                                                                     

	//http://playtechs.blogspot.com/2007/04/aiming-at-moving-target.html
	double a = .25f * GravityVec.Dot(GravityVec);
	double b = EnemyVelocity.Dot(GravityVec);
	double c = RelativePos.Dot(GravityVec) + EnemyVelocity.Dot(EnemyVelocity) - (BulletSpeed.z * BulletSpeed.z);
	double d = 2.0f*(RelativePos.Dot(EnemyVelocity));
	double e = RelativePos.Dot(RelativePos);

	//Calculate time projectile is in air
	std::vector<double> Solutions;
	int NumSol=m_Solver.SolveQuartic(a, b, c, d, e, Solutions);

	//find smallest non-negative real root
	float ShortestAirTime = 99999.0f;
	for (int i = 0; i < NumSol; i++)
	{
		float AirTime = Solutions[i];
		if(AirTime<0)
			continue;

		if (AirTime < ShortestAirTime)
			ShortestAirTime = AirTime;
	}
	//Extrapolate position on velocity, and account for bullet drop
	AimPoint = fApproxPos(AimPoint,EnemyVelocity,GravityVec,ShortestAirTime);

	if (Zero.m_ZeroDistance == -1.0f)
		return 0.0f;

	//This is still an approximation, fix later
	float ZeroAirTime = Zero.m_ZeroDistance / fabs(BulletSpeed.z);
	float ZeroDrop = (.5f*fabs(Gravity)*ZeroAirTime*ZeroAirTime);
	float Theta = atan2(ZeroDrop, Zero.m_ZeroDistance);
	return Theta;
}

