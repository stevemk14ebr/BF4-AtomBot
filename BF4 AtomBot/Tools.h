#pragma once

#define PI       3.14159265358979323846
#define PI_2     1.57079632679489661923

static HANDLE hConsole;

void CreateConsole()
{
	AllocConsole();
	hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
}

void XTrace(char* lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	char szBuffer[512];
	nBuf = _vsnprintf(szBuffer, 511, lpszFormat, args);
	va_end(args);
	DWORD CharsWritten = 0;
	WriteConsole(hConsole, szBuffer, nBuf, &CharsWritten, NULL);
}

bool WorldToScreen(SM::Vector3 vLocVec4, SM::Vector3 &vOut)
{
	GameRenderer *gamerenderer = GameRenderer::GetInstance();
	if (!PLH::IsValidPtr(gamerenderer))
		return false;

	RenderView* pRend = gamerenderer->m_pRenderView;
	if (!PLH::IsValidPtr(pRend))
		return false;

	SM::Matrix vMatrix = pRend->m_View;
	SM::Matrix pMatrix = pRend->m_Projection;

	DxRenderer* pDX = DxRenderer::GetInstance();
	if (!PLH::IsValidPtr(pDX))
		return false;

	Screen* pScreen = pDX->m_pScreen;
	if (!PLH::IsValidPtr(pScreen))
		return false;

	float width = pScreen->m_Width;
	float height = pScreen->m_Height;

	SM::Vector3 vtempOut = DirectX::XMVector3Project(vLocVec4, 0, 0, width, height, 0, 1, pMatrix, vMatrix, 
		DirectX::XMMatrixIdentity());
	vOut = vtempOut;
	if (vtempOut.z < 1.0f)
		return true;
	return false;
}

bool WorldToScreen(SM::Vector3 &vLocVec4)
{
	GameRenderer *gamerenderer = GameRenderer::GetInstance();
	if (!PLH::IsValidPtr(gamerenderer))
		return false;

	RenderView* pRend = gamerenderer->m_pRenderView;
	if (!PLH::IsValidPtr(pRend))
		return false;

	SM::Matrix vMatrix = pRend->m_View;
	SM::Matrix pMatrix = pRend->m_Projection;

	DxRenderer* pDX = DxRenderer::GetInstance();
	if (!PLH::IsValidPtr(pDX))
		return false;

	Screen* pScreen = pDX->m_pScreen;
	if (!PLH::IsValidPtr(pScreen))
		return false;

	float width = pScreen->m_Width;
	float height = pScreen->m_Height;

	SM::Vector3 vOut = DirectX::XMVector3Project(vLocVec4, 0, 0, width, height, 0, 1, pMatrix, vMatrix, DirectX::XMMatrixIdentity());
	vLocVec4 = vOut;
	if (vLocVec4.z < 1.0f)
		return true;
	return false;
}

void ConnectBones(RenderInterface* pRend,const Color& color,RagdollComponent* pRag,UpdatePoseResultData::BONES Bone1,UpdatePoseResultData::BONES Bone2)
{
	if (!PLH::IsValidPtr(pRag))
		return;

	SM::Vector3 Bone1Vec;
	if (!pRag->GetBone(Bone1, Bone1Vec))
		return;

	SM::Vector3 Bone2Vec;
	if (!pRag->GetBone(Bone2, Bone2Vec))
		return;

	if (!WorldToScreen(Bone1Vec))
		return;

	if (!WorldToScreen(Bone2Vec))
		return;

	pRend->DrawLine(Vector2f(Bone1Vec.x, Bone1Vec.y), Vector2f(Bone2Vec.x, Bone2Vec.y), color);
}

ClassInfo* FindClassInfo(const char* ClassName)
{
	TypeInfo* CurrentTypeInfo = TypeInfo::GetFirst();
	if (!PLH::IsValidPtr(CurrentTypeInfo))
		return nullptr;
	do 
	{
		if(_stricmp(CurrentTypeInfo->m_InfoData->m_Name,ClassName) !=0)
			continue;

		if(CurrentTypeInfo->GetTypeCode() != BasicTypesEnum::kTypeCode_Class)
			continue;

		return (ClassInfo*)CurrentTypeInfo;
	} while ((CurrentTypeInfo=CurrentTypeInfo->m_Next) != nullptr);
}

SM::Vector2 GetScreenSize()
{
	DxRenderer* DxRend = DxRenderer::GetInstance();
	if (!PLH::IsValidPtr(DxRend))
		return SM::Vector2(0,0);

	Screen* pScreen=DxRend->m_pScreen;
	if (!PLH::IsValidPtr(pScreen))
		return SM::Vector2(0, 0);

	return SM::Vector2(pScreen->m_Width, pScreen->m_Height);
}

SM::Vector3 MultiplyMat(SM::Vector3& vec, SM::Matrix* mat)
{
	//This just multiplies by the matrix transpose on m[0] to m[2] (skips [m3] as we don't want trans info)
	return SM::Vector3(mat->_11 * vec.x + mat->_21 * vec.y + mat->_31 * vec.z,
		mat->_12 * vec.x + mat->_22 * vec.y + mat->_32 * vec.z,
		mat->_13 * vec.x + mat->_23 * vec.y + mat->_33 * vec.z);
}

bool TransformDrawAABB(ClientControllableEntity* pEnt, RenderInterface* Renderer, Color color)
{
	TransformAABBStruct TransAABB;
	SM::Matrix EntityMatrix;
	pEnt->GetAABB(&TransAABB);
	pEnt->GetTransform(&EntityMatrix);

	SM::Vector3 pos = EntityMatrix.Translation();

	SM::Vector3 min = SM::Vector3(TransAABB.AABB.m_Min.x, TransAABB.AABB.m_Min.y, TransAABB.AABB.m_Min.z);
	SM::Vector3 max = SM::Vector3(TransAABB.AABB.m_Max.x, TransAABB.AABB.m_Max.y, TransAABB.AABB.m_Max.z);
	SM::Vector3 crnr2 = pos + MultiplyMat(SM::Vector3(max.x, min.y, min.z), &EntityMatrix);
	SM::Vector3 crnr3 = pos + MultiplyMat(SM::Vector3(max.x, min.y, max.z), &EntityMatrix);
	SM::Vector3 crnr4 = pos + MultiplyMat(SM::Vector3(min.x, min.y, max.z), &EntityMatrix);
	SM::Vector3 crnr5 = pos + MultiplyMat(SM::Vector3(min.x, max.y, max.z), &EntityMatrix);
	SM::Vector3 crnr6 = pos + MultiplyMat(SM::Vector3(min.x, max.y, min.z), &EntityMatrix);
	SM::Vector3 crnr7 = pos + MultiplyMat(SM::Vector3(max.x, max.y, min.z), &EntityMatrix);
	min = pos + MultiplyMat(min, &EntityMatrix);
	max = pos + MultiplyMat(max, &EntityMatrix);

	if (!WorldToScreen(min))
		return false;
	if (!WorldToScreen(max))
		return false;
	if (!WorldToScreen(crnr2))
		return false;
	if (!WorldToScreen(crnr3))
		return false;
	if (!WorldToScreen(crnr4))
		return false;
	if (!WorldToScreen(crnr5))
		return false;
	if (!WorldToScreen(crnr6))
		return false;
	if (!WorldToScreen(crnr7))
		return false;

	Renderer->BeginLine();

	Renderer->DrawLine(Vector2f(min.x, min.y), Vector2f(crnr2.x, crnr2.y), color);
	Renderer->DrawLine(Vector2f(min.x, min.y), Vector2f(crnr4.x, crnr4.y), color);
	Renderer->DrawLine(Vector2f(min.x, min.y), Vector2f(crnr6.x, crnr6.y), color);

	//max to 5,7,3
	Renderer->DrawLine(Vector2f(max.x, max.y), Vector2f(crnr5.x, crnr5.y), color);
	Renderer->DrawLine(Vector2f(max.x, max.y), Vector2f(crnr7.x, crnr7.y), color);
	Renderer->DrawLine(Vector2f(max.x, max.y), Vector2f(crnr3.x, crnr3.y), color);

	//2 to 7,3
	Renderer->DrawLine(Vector2f(crnr2.x, crnr2.y), Vector2f(crnr3.x, crnr3.y), color);
	Renderer->DrawLine(Vector2f(crnr2.x, crnr2.y), Vector2f(crnr7.x, crnr7.y), color);

	//4 to 5,3
	Renderer->DrawLine(Vector2f(crnr4.x, crnr4.y), Vector2f(crnr5.x, crnr5.y), color);
	Renderer->DrawLine(Vector2f(crnr4.x, crnr4.y), Vector2f(crnr3.x, crnr3.y), color);

	//6 to 5,7
	Renderer->DrawLine(Vector2f(crnr6.x, crnr6.y), Vector2f(crnr5.x, crnr5.y), color);
	Renderer->DrawLine(Vector2f(crnr6.x, crnr6.y), Vector2f(crnr7.x, crnr7.y), color);

	Renderer->EndLine();
}

void NormalizeAngle(SM::Vector2& Angle)
{
	if (Angle.x <= -PI)
		Angle.x += 2 * PI;
	if (Angle.x > PI)
		Angle.x -= 2 * PI;
	if (Angle.y <= -PI_2)
		Angle.y += PI;
	if (Angle.y > PI_2)
		Angle.y -= PI;
}

void LerpAngle(SM::Vector2& From, SM::Vector2& To, float Step)
{
	SM::Vector2 Delta = (To - From);
	NormalizeAngle(Delta);
	To = (From + Step*Delta);
	NormalizeAngle(To);
}

void CerpAngle(SM::Vector2& From, SM::Vector2& To, float StepX, float StepY)
{
	float CubicStepX = (1 - cos(StepX*PI)) / 2;
	float CubicStepY = (1 - cos(StepY*PI)) / 2;
	SM::Vector2 Delta = (To - From);
	NormalizeAngle(Delta);
	To.x = (From.x + CubicStepX * Delta.x);
	To.y = (From.y + CubicStepY * Delta.y);
	NormalizeAngle(To);
}

float Length2D(SM::Vector3& Vec)
{
	return sqrtf(Vec.x*Vec.x + Vec.z*Vec.z);
}

void GetEyeLocation(SM::Matrix& Mat)
{
	GameRenderer *gamerenderer = GameRenderer::GetInstance();
	if (!PLH::IsValidPtr(gamerenderer))
		return;

	RenderView* pRend = gamerenderer->m_pRenderView;
	if (!PLH::IsValidPtr(pRend))
		return;

	Mat=pRend->m_ViewInverse;
}