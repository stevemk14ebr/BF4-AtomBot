#pragma once

class AimbotSmoother
{
public:
	AimbotSmoother();
	void Update(float DeltaTime);
	void ResetTimes(SM::Vector2& Times);
	void SmoothAngles(SM::Vector2& CurrentAngles, SM::Vector2& TargetAngles);
private:
	SM::Vector2 m_TimeToTarget;
	SM::Vector2 m_CurAimTime;
};

AimbotSmoother::AimbotSmoother()
{

}

void AimbotSmoother::Update(float DeltaTime)
{
	m_CurAimTime += SM::Vector2(DeltaTime, DeltaTime);
	if(m_CurAimTime.x > m_TimeToTarget.x)
		m_CurAimTime.x = m_TimeToTarget.x;

	if(m_CurAimTime.y > m_TimeToTarget.y)
		m_CurAimTime.y = m_TimeToTarget.y;
}

void AimbotSmoother::ResetTimes(SM::Vector2& NewAimTimes)
{
	m_TimeToTarget = NewAimTimes;
	m_CurAimTime = SM::Vector2(0, 0);
}

void AimbotSmoother::SmoothAngles(SM::Vector2& CurrentAngles,SM::Vector2& TargetAngles)
{
	SM::Vector2 AimPercent = m_CurAimTime / m_TimeToTarget;
	CerpAngle(CurrentAngles, TargetAngles, AimPercent.x, AimPercent.y);
}

