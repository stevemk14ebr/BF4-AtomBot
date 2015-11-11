// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include "PolyHook/PolyHook.h"
#include "DXTK/Inc/SimpleMath.h"
namespace SM = DirectX::SimpleMath;
#include "Menu/DaMenu.h"
#include "Engine/BF4Classes.h"
#include "Tools.h"
#include "Hack Core/Core.h"

PLH::VTableSwap* PLHSwapChainhk;
typedef HRESULT(__stdcall* tPresent)(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags);
tPresent oPresent; //our dx hook

typedef HRESULT(__stdcall* tResizeBuffers)(IDXGISwapChain* pThis,UINT BufferCount,UINT Width,
	UINT Height,DXGI_FORMAT NewFormat,UINT SwapChainFlags);
tResizeBuffers oResizeBuffers;

PLH::VTableSwap* PLHPreframehk;
typedef int(__stdcall* tPreFrameUpdate)(float dt);
tPreFrameUpdate oPreFrame;

PLH::Detour* PLHBitBlthk;
typedef BOOL(__stdcall* tBitBlt)(HDC hdcDest,int nXDest,int nYDest,int nWidth,
	int nHeight,HDC hdcSrc,int nXSrc,int nYSrc,DWORD dwRop);
tBitBlt oBitBlt;

PLH::VFuncSwap* PLHUpdatehk;
typedef DWORD_PTR(__stdcall* tUpdate)(DWORD_PTR a1, DWORD_PTR a2, DWORD_PTR a3);
tUpdate oUpdate;
void* pAntVtable = (void*)OFFSET_ANTVTABLE;

DWORD WINAPI InitThread(LPVOID lparam);
HMODULE DllInst = 0;
int Count = 0;
bool Initialized = false;
bool DrawMenu = false;

DXTKRenderer* Renderer;
ElementManager* GUIManager;
WinAPIInputManager* InputManager;
Core m_HackCore;

void AddESPTabElements(TabbedWindowPageElement* ESPTab)
{
	LabelElement::Context EnemyNotVisLabelCtx;
	EnemyNotVisLabelCtx.m_Position = Vector2f(120, 0);
	EnemyNotVisLabelCtx.m_Size = Vector2f(200, 20);
	EnemyNotVisLabelCtx.m_Text = "Enemy Not Visible Color";
	LabelElement* EnemyNotVisLabel = new LabelElement(EnemyNotVisLabelCtx);

	ColorPickerElement::Context EnemyNotVisColorPickerCtx;
	EnemyNotVisColorPickerCtx.m_FillColor = m_HackCore.GetESPSettings().m_EnemyNotVisible;
	EnemyNotVisColorPickerCtx.m_FillColorMouseOver = Color(.7f, .7f, .7f);
	EnemyNotVisColorPickerCtx.m_Position = Vector2f(10, 0);
	EnemyNotVisColorPickerCtx.m_Size = Vector2f(100, 20);
	EnemyNotVisColorPickerCtx.m_ColorSelectorInfo.m_FillColor = Color(.7f, .7f, .7f);
	EnemyNotVisColorPickerCtx.m_ColorSelectorInfo.m_TitleFillColor = Color(.5f, .5f, .5f);
	EnemyNotVisColorPickerCtx.m_ColorSelectorInfo.m_Size = Vector2f(200, 200);
	ColorPickerElement* EnemyNotVisColorPicker = new ColorPickerElement(EnemyNotVisColorPickerCtx);
	EnemyNotVisColorPicker->EventColorChanged() += [&](Color& color, const uint32_t Id) {
		m_HackCore.GetESPSettings().m_EnemyNotVisible = color;
	};

	LabelElement::Context EnemyVisLabelCtx;
	EnemyVisLabelCtx.m_Position = Vector2f(120, 30);
	EnemyVisLabelCtx.m_Size = Vector2f(200, 20);
	EnemyVisLabelCtx.m_Text = "Enemy Visible Color";
	LabelElement* EnemyVisLabel = new LabelElement(EnemyVisLabelCtx);

	ColorPickerElement::Context EnemyVisColorPickerCtx;
	EnemyVisColorPickerCtx.m_FillColor = m_HackCore.GetESPSettings().m_EnemyVisible;
	EnemyVisColorPickerCtx.m_FillColorMouseOver = Color(.7f, .7f, .7f);
	EnemyVisColorPickerCtx.m_Position = Vector2f(10, 30);
	EnemyVisColorPickerCtx.m_Size = Vector2f(100, 20);
	EnemyVisColorPickerCtx.m_ColorSelectorInfo.m_FillColor = Color(.7f, .7f, .7f);
	EnemyVisColorPickerCtx.m_ColorSelectorInfo.m_TitleFillColor = Color(.5f, .5f, .5f);
	EnemyVisColorPickerCtx.m_ColorSelectorInfo.m_Size = Vector2f(200, 200);
	ColorPickerElement* EnemyVisColorPicker = new ColorPickerElement(EnemyVisColorPickerCtx);
	EnemyVisColorPicker->EventColorChanged() += [&](Color& color, const uint32_t Id) {
		m_HackCore.GetESPSettings().m_EnemyVisible = color;
	};

	LabelElement::Context FriendlyNotVisLabelCtx;
	FriendlyNotVisLabelCtx.m_Position = Vector2f(120, 60);
	FriendlyNotVisLabelCtx.m_Size = Vector2f(200, 20);
	FriendlyNotVisLabelCtx.m_Text = "Friendly Not Visible Color";
	LabelElement* FriendlyNotVisLabel = new LabelElement(FriendlyNotVisLabelCtx);

	ColorPickerElement::Context FriendlyNotVisColorPickerCtx;
	FriendlyNotVisColorPickerCtx.m_FillColor = m_HackCore.GetESPSettings().m_FriendlyNotVisible;
	FriendlyNotVisColorPickerCtx.m_FillColorMouseOver = Color(.7f, .7f, .7f);
	FriendlyNotVisColorPickerCtx.m_Position = Vector2f(10, 60);
	FriendlyNotVisColorPickerCtx.m_Size = Vector2f(100, 20);
	FriendlyNotVisColorPickerCtx.m_ColorSelectorInfo.m_FillColor = Color(.7f, .7f, .7f);
	FriendlyNotVisColorPickerCtx.m_ColorSelectorInfo.m_TitleFillColor = Color(.5f, .5f, .5f);
	FriendlyNotVisColorPickerCtx.m_ColorSelectorInfo.m_Size = Vector2f(200, 200);
	ColorPickerElement* FriendlyNotVisColorPicker = new ColorPickerElement(FriendlyNotVisColorPickerCtx);
	FriendlyNotVisColorPicker->EventColorChanged() += [&](Color& color, const uint32_t Id) {
		m_HackCore.GetESPSettings().m_FriendlyNotVisible = color;
	};

	LabelElement::Context FriendlyVisLabelCtx;
	FriendlyVisLabelCtx.m_Position = Vector2f(120, 60);
	FriendlyVisLabelCtx.m_Size = Vector2f(200, 90);
	FriendlyVisLabelCtx.m_Text = "Friendly Visible Color";
	LabelElement* FriendlyVisLabel = new LabelElement(FriendlyVisLabelCtx);

	ColorPickerElement::Context FriendlyVisColorPickerCtx;
	FriendlyVisColorPickerCtx.m_FillColor = m_HackCore.GetESPSettings().m_FriendlyVisible;
	FriendlyVisColorPickerCtx.m_FillColorMouseOver = Color(.7f, .7f, .7f);
	FriendlyVisColorPickerCtx.m_Position = Vector2f(10, 90);
	FriendlyVisColorPickerCtx.m_Size = Vector2f(100, 20);
	FriendlyVisColorPickerCtx.m_ColorSelectorInfo.m_FillColor = Color(.7f, .7f, .7f);
	FriendlyVisColorPickerCtx.m_ColorSelectorInfo.m_TitleFillColor = Color(.5f, .5f, .5f);
	FriendlyVisColorPickerCtx.m_ColorSelectorInfo.m_Size = Vector2f(200, 200);
	ColorPickerElement* FriendlyVisColorPicker = new ColorPickerElement(FriendlyVisColorPickerCtx);
	FriendlyVisColorPicker->EventColorChanged() += [&](Color& color, const uint32_t Id) {
		m_HackCore.GetESPSettings().m_FriendlyVisible = color;
	};

	LabelElement::Context ESPOnLabelCtx;
	ESPOnLabelCtx.m_Position = Vector2f(120, 120);
	ESPOnLabelCtx.m_Size = Vector2f(200, 20);
	ESPOnLabelCtx.m_Text = "Toggle ESP";
	LabelElement* ESPOnLabel = new LabelElement(ESPOnLabelCtx);

	CheckBoxElement::Context ESPOnCheckBoxCtx;
	ESPOnCheckBoxCtx.m_FillColor = Color(.7f, .7f, .7f);
	ESPOnCheckBoxCtx.m_FillColorMouseOver = Color(.5f, .5f, .5f);
	ESPOnCheckBoxCtx.m_FillColorChecked = Color(.4f, .4f, .4f);
	ESPOnCheckBoxCtx.m_Position = Vector2f(10, 120);
	ESPOnCheckBoxCtx.m_Size = Vector2f(20, 20);
	ESPOnCheckBoxCtx.m_DefaultState = m_HackCore.GetESPSettings().m_Enabled;
	CheckBoxElement* ESPOnCheckBox = new CheckBoxElement(ESPOnCheckBoxCtx);
	ESPOnCheckBox->EventValueChanged() += [&](bool Value) {
		m_HackCore.GetESPSettings().m_Enabled = Value;
	};

	LabelElement::Context ShowFriendlyLabelCtx;
	ShowFriendlyLabelCtx.m_Position = Vector2f(120, 150);
	ShowFriendlyLabelCtx.m_Size = Vector2f(200, 20);
	ShowFriendlyLabelCtx.m_Text = "Toggle Show Friendly";
	LabelElement* ShowFriendlyLabel = new LabelElement(ShowFriendlyLabelCtx);

	CheckBoxElement::Context ShowFriendlyCheckBoxCtx;
	ShowFriendlyCheckBoxCtx.m_FillColor = Color(.7f, .7f, .7f);
	ShowFriendlyCheckBoxCtx.m_FillColorMouseOver = Color(.5f, .5f, .5f);
	ShowFriendlyCheckBoxCtx.m_FillColorChecked = Color(.4f, .4f, .4f);
	ShowFriendlyCheckBoxCtx.m_Position = Vector2f(10, 150);
	ShowFriendlyCheckBoxCtx.m_Size = Vector2f(20, 20);
	ShowFriendlyCheckBoxCtx.m_DefaultState = m_HackCore.GetESPSettings().m_ShowFriendlies;
	CheckBoxElement* ShowFriendlyCheckBox = new CheckBoxElement(ShowFriendlyCheckBoxCtx);
	ShowFriendlyCheckBox->EventValueChanged() += [&](bool Value) {
		m_HackCore.GetESPSettings().m_ShowFriendlies = Value;
	};

	LabelElement::Context SnapLinesLabelCtx;
	SnapLinesLabelCtx.m_Position = Vector2f(120, 180);
	SnapLinesLabelCtx.m_Size = Vector2f(200, 20);
	SnapLinesLabelCtx.m_Text = "Toggle Snaplines";
	LabelElement* SnapLinesLabel = new LabelElement(SnapLinesLabelCtx);

	CheckBoxElement::Context SnapLinesCheckBoxCtx;
	SnapLinesCheckBoxCtx.m_FillColor = Color(.7f, .7f, .7f);
	SnapLinesCheckBoxCtx.m_FillColorMouseOver = Color(.5f, .5f, .5f);
	SnapLinesCheckBoxCtx.m_FillColorChecked = Color(.4f, .4f, .4f);
	SnapLinesCheckBoxCtx.m_Position = Vector2f(10, 180);
	SnapLinesCheckBoxCtx.m_Size = Vector2f(20, 20);
	SnapLinesCheckBoxCtx.m_DefaultState = m_HackCore.GetESPSettings().m_SnapLinesEnabled;
	CheckBoxElement* SnapLinesCheckBox = new CheckBoxElement(SnapLinesCheckBoxCtx);
	SnapLinesCheckBox->EventValueChanged() += [&](bool Value) {
		m_HackCore.GetESPSettings().m_SnapLinesEnabled = Value;
	};

	LabelElement::Context SkeletonLabelCtx;
	SkeletonLabelCtx.m_Position = Vector2f(120, 210);
	SkeletonLabelCtx.m_Size = Vector2f(200, 20);
	SkeletonLabelCtx.m_Text = "Toggle Skeleton";
	LabelElement* SkeletonLabel = new LabelElement(SkeletonLabelCtx);

	CheckBoxElement::Context SkeletonCheckBoxCtx;
	SkeletonCheckBoxCtx.m_FillColor = Color(.7f, .7f, .7f);
	SkeletonCheckBoxCtx.m_FillColorMouseOver = Color(.5f, .5f, .5f);
	SkeletonCheckBoxCtx.m_FillColorChecked = Color(.4f, .4f, .4f);
	SkeletonCheckBoxCtx.m_Position = Vector2f(10, 210);
	SkeletonCheckBoxCtx.m_Size = Vector2f(20, 20);
	SkeletonCheckBoxCtx.m_DefaultState = m_HackCore.GetESPSettings().m_SkeletonEnabled;
	CheckBoxElement* SkeletonCheckBox = new CheckBoxElement(SkeletonCheckBoxCtx);
	SkeletonCheckBox->EventValueChanged() += [&](bool Value) {
		m_HackCore.GetESPSettings().m_SkeletonEnabled = Value;
	};

	ESPTab->AddSubElement(EnemyNotVisLabel);
	ESPTab->AddSubElement(EnemyVisLabel);
	ESPTab->AddSubElement(FriendlyVisLabel);
	ESPTab->AddSubElement(FriendlyNotVisLabel);

	ESPTab->AddSubElement(EnemyNotVisColorPicker);
	ESPTab->AddSubElement(EnemyVisColorPicker);
	ESPTab->AddSubElement(FriendlyNotVisColorPicker);
	ESPTab->AddSubElement(FriendlyVisColorPicker);

	ESPTab->AddSubElement(ESPOnLabel);
	ESPTab->AddSubElement(ESPOnCheckBox);

	ESPTab->AddSubElement(ShowFriendlyLabel);
	ESPTab->AddSubElement(ShowFriendlyCheckBox);

	ESPTab->AddSubElement(SnapLinesLabel);
	ESPTab->AddSubElement(SnapLinesCheckBox);

	ESPTab->AddSubElement(SkeletonLabel);
	ESPTab->AddSubElement(SkeletonCheckBox);
}

void AddAimbotTabElements(TabbedWindowPageElement* AimbotTab)
{
	ComboBoxElement::Context BoneSelectorComboBoxCtx;
	BoneSelectorComboBoxCtx.m_FillColor = Color(.7f, .7f, .7f);
	BoneSelectorComboBoxCtx.m_FillColorMouseDown = Color(.4f, .4f, .4f);
	BoneSelectorComboBoxCtx.m_FillColorMouseOver = Color(.7f, .7f, .7f);
	BoneSelectorComboBoxCtx.m_Position = Vector2f(10, 0);
	BoneSelectorComboBoxCtx.m_Size = Vector2f(100, 20);
	ComboBoxElement* BoneSelectorComboBox = new ComboBoxElement(BoneSelectorComboBoxCtx);
	BoneSelectorComboBox->AddComboButton("Head");
	BoneSelectorComboBox->AddComboButton("Neck");
	BoneSelectorComboBox->AddComboButton("Left Shoulder");
	BoneSelectorComboBox->AddComboButton("Right Shoulder");
	BoneSelectorComboBox->AddComboButton("Left Elbow");
	BoneSelectorComboBox->AddComboButton("Right Elbow");
	BoneSelectorComboBox->AddComboButton("Left Hand");
	BoneSelectorComboBox->AddComboButton("Right Hand");
	BoneSelectorComboBox->AddComboButton("Upper Spine");
	BoneSelectorComboBox->AddComboButton("Lower Spine");
	BoneSelectorComboBox->AddComboButton("Left Knee");
	BoneSelectorComboBox->AddComboButton("Right Knee");
	BoneSelectorComboBox->AddComboButton("Left Foot");
	BoneSelectorComboBox->AddComboButton("Right Foot");
	BoneSelectorComboBox->EventSelectionChanged() += std::bind(&Core::OnBoneSelectionChanged, &m_HackCore, std::placeholders::_1, std::placeholders::_2);

	SliderElement<float>::Context MinTimeSliderCtx;
	MinTimeSliderCtx.m_Color = Color::Black();
	MinTimeSliderCtx.m_DefaultValue = m_HackCore.GetAimbotSettings().m_MinTimeToTarget;
	MinTimeSliderCtx.m_Min = 0.0f;
	MinTimeSliderCtx.m_Max = 2.0f;
	MinTimeSliderCtx.m_Position = Vector2f(10, 30);
	MinTimeSliderCtx.m_Size = Vector2f(200, 20);
	SliderElement<float>* MinTimeSlider = new SliderElement<float>(MinTimeSliderCtx);
	MinTimeSlider->EventValueChanged() += [&](const float& Value) {
		m_HackCore.GetAimbotSettings().m_MinTimeToTarget = Value;
	};

	SliderElement<float>::Context MaxTimeSliderCtx;
	MaxTimeSliderCtx.m_Color = Color::Black();
	MaxTimeSliderCtx.m_DefaultValue = m_HackCore.GetAimbotSettings().m_MaxTimeToTarget;
	MaxTimeSliderCtx.m_Min = 0.0f;
	MaxTimeSliderCtx.m_Max = 2.0f;
	MaxTimeSliderCtx.m_Position = Vector2f(10, 60);
	MaxTimeSliderCtx.m_Size = Vector2f(200, 20);
	SliderElement<float>* MaxTimeSlider = new SliderElement<float>(MaxTimeSliderCtx);
	MaxTimeSlider->EventValueChanged() += [&](const float& Value) {
		m_HackCore.GetAimbotSettings().m_MaxTimeToTarget = Value;
	};

	LabelElement::Context AimbotToggleLabelCtx;
	AimbotToggleLabelCtx.m_Position = Vector2f(120, 90);
	AimbotToggleLabelCtx.m_Size = Vector2f(200, 20);
	AimbotToggleLabelCtx.m_Text = "Toggle Aimbot";
	LabelElement* AimbotToggleLabel = new LabelElement(AimbotToggleLabelCtx);

	CheckBoxElement::Context AimbotCheckboxCtx;
	AimbotCheckboxCtx.m_FillColor = Color(.7f, .7f, .7f);
	AimbotCheckboxCtx.m_FillColorMouseOver = Color(.5f, .5f, .5f);
	AimbotCheckboxCtx.m_FillColorChecked = Color(.4f, .4f, .4f);
	AimbotCheckboxCtx.m_Position = Vector2f(10, 90);
	AimbotCheckboxCtx.m_Size = Vector2f(20, 20);
	AimbotCheckboxCtx.m_DefaultState = m_HackCore.GetESPSettings().m_SkeletonEnabled;
	CheckBoxElement* AimbotCheckBox = new CheckBoxElement(AimbotCheckboxCtx);
	AimbotCheckBox->EventValueChanged() += [&](bool Value) {
		m_HackCore.GetAimbotSettings().m_Enabled = Value;
	};

	AimbotTab->AddSubElement(BoneSelectorComboBox);
	AimbotTab->AddSubElement(MinTimeSlider);
	AimbotTab->AddSubElement(MaxTimeSlider);
	AimbotTab->AddSubElement(AimbotCheckBox);
	AimbotTab->AddSubElement(AimbotToggleLabel);
}

void SetupMenu()
{
	XTrace("Initializing Renderer\n");
	DxRenderer* DXRend = DxRenderer::GetInstance();
	if (!PLH::IsValidPtr(DXRend))
		return;

	Renderer = new DXTKRenderer();
	Renderer->Init(DXRend->m_pScreen->m_pSwapChain, DXRend->m_pScreen->m_WindowWidth, DXRend->m_pScreen->m_WindowHeight);
	XTrace("Done...\n");
	XTrace("Initializing Input Manager\n");
	InputManager = new WinAPIInputManager(FindWindow(NULL, "Battlefield 4"));
	XTrace("Done...\n");
	XTrace("Initializing GUI Manager\n");
	GUIManager = new ElementManager(InputManager, Renderer);
	XTrace("Done...\n");
	XTrace("Adding Menu Elements\n");

	TabbedWindowElement::Context TabWinCtx;
	TabWinCtx.m_TabFillColor = Color(.7f, .7f, .7f);
	TabWinCtx.m_TabTitleFillColor = Color(.5f, .5f, .5f);
	TabWinCtx.m_TitleFillColor = Color(.5f, .5f, .5f);
	TabWinCtx.m_WindowName = "SteveBot Settings";
	TabWinCtx.m_Position = Vector2f(300, 100);
	TabWinCtx.m_Size = Vector2f(400, 400);
	TabWinCtx.m_DefaultTabFocusIndex = 0;
	TabbedWindowElement* TabbedWindow = new TabbedWindowElement(TabWinCtx);
	uint32_t Tab1PageId = TabbedWindow->AddTabPage("ESP");
	uint32_t Tab2PageId = TabbedWindow->AddTabPage("Aimbot");
	TabbedWindowPageElement* ESPTab = TabbedWindow->GetElementById<TabbedWindowPageElement>(Tab1PageId);
	TabbedWindowPageElement* AimbotTab = TabbedWindow->GetElementById<TabbedWindowPageElement>(Tab2PageId);
	
	AddESPTabElements(ESPTab);
	AddAimbotTabElements(AimbotTab);
	GUIManager->AddElement(TabbedWindow);
	XTrace("Done...\n");
	Initialized = true;
}

HRESULT __stdcall hkPresent(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags)
{
	*(DWORD_PTR*)PBRecieveSS = 0; //Zero Punkbuster Screenshots
	if (!Initialized)
	{
		//Wait a little to init renderer, crashes otherwise dunno why
		if (Count < 200)
		{
			Count++;
			return oPresent(thisptr, SyncInterval, Flags);
		}
		SetupMenu();
	}

	if (DrawMenu)
	{
		BorderInputNode* pBorderInput = BorderInputNode::GetInstance();
		if (!PLH::IsValidPtr(pBorderInput))
			return 0;

		MouseDevice* pMouse = pBorderInput->m_pMouse->m_pDevice;
		//Tell game to enable mouse for our menu
		GUIManager->UpdateInput();
		GUIManager->Render();

		pMouse->m_EnableCursor = 1;
		pMouse->m_ShowCursor = 1;
	}
	m_HackCore.UpdatePresent();
	m_HackCore.ESP(Renderer);
	m_HackCore.EntityESP(Renderer);
	return oPresent(thisptr, SyncInterval, Flags);
}

HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pThis,UINT BufferCount, UINT Width, UINT Height,DXGI_FORMAT NewFormat,UINT SwapChainFlags)
{
	XTrace("Resizing\n");
	DxRenderer* DXRend = DxRenderer::GetInstance();
	if (!PLH::IsValidPtr(DXRend))
		return S_OK;

	delete GUIManager;
	Initialized = false;
	Count = 0;

	XTrace("Done...\n");
	return oResizeBuffers(pThis, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

int __stdcall hkPreFrameUpdate(float DeltaTime)
{
	//Original clears input buffers, so call first
	int RetVal=oPreFrame(DeltaTime);
	m_HackCore.UpdatePreFrame();
	m_HackCore.Aimbot(DeltaTime);
	m_HackCore.NoRecoil();
	return RetVal;
}

BOOL __stdcall hkBitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth,
	int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
	//Block FairFight screenshots, should send back white SS
	XTrace("!!!!SCREENSHOT ATTEMPTED!!!!\n");
	return oBitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc,WHITENESS);
}

DWORD_PTR hkUpdate(DWORD_PTR a1, DWORD_PTR a2, DWORD_PTR a3)//ICY
{
	if (a1)
		*(bool*)(a1 + 0xEC) = true;

	if (a2)
		*(bool*)(a2 + 0x8A) = true;

	return oUpdate(a1, a2, a3);
}

void Uninject()
{
	PLHSwapChainhk->UnHook();
	PLHBitBlthk->UnHook();
	PLHUpdatehk->UnHook();
	PLHPreframehk->UnHook();
	delete GUIManager;
	TerminateThread(InitThread, 0);
	Sleep(500);
	FreeConsole();
	FreeLibraryAndExitThread(DllInst, 0);
}

DWORD WINAPI InitThread(LPVOID lparam)
{
	DxRenderer* DXRend=DxRenderer::GetInstance();
	if (!PLH::IsValidPtr(DXRend))
		return 0;

	PLHSwapChainhk = new PLH::VTableSwap();
	PLHSwapChainhk->SetupHook((BYTE*)DXRend->m_pScreen->m_pSwapChain, 8, (BYTE*)&hkPresent);
	PLHSwapChainhk->Hook();
	oPresent = PLHSwapChainhk->GetOriginal<tPresent>();
	oResizeBuffers = PLHSwapChainhk->HookAdditional<tResizeBuffers>(13, (BYTE*)&hkResizeBuffers);

	PLHBitBlthk = new PLH::Detour();
	PLHBitBlthk->SetupHook((BYTE*)GetProcAddress(GetModuleHandle("Gdi32.dll"), "BitBlt"), (BYTE*)&hkBitBlt);
	PLHBitBlthk->Hook();
	oBitBlt=PLHBitBlthk->GetOriginal<tBitBlt>();

	PLHUpdatehk = new PLH::VFuncSwap();
	PLHUpdatehk->SetupHook((BYTE**)pAntVtable, 11, (BYTE*)&hkUpdate);
	PLHUpdatehk->Hook();
	oUpdate = PLHUpdatehk->GetOriginal<tUpdate>();

	PLHPreframehk = new PLH::VTableSwap();
	PLHPreframehk->SetupHook((BYTE*)BorderInputNode::GetInstance()->m_Vtable, 3, (BYTE*)&hkPreFrameUpdate);
	PLHPreframehk->Hook();
	oPreFrame = PLHPreframehk->GetOriginal<tPreFrameUpdate>();

	while (1)
	{
		if (GetAsyncKeyState(VK_F10) & 0x8000)
		{
			Sleep(100);
			Uninject();
			break;
		}
		if (GetAsyncKeyState(VK_F11) & 0x8000)
		{
			Sleep(100);
			DrawMenu = !DrawMenu;
		}
		Sleep(20);
	}
	return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule,DWORD Reason,LPVOID lpReserved)
{
	if (Reason == DLL_PROCESS_ATTACH) 
	{
		DisableThreadLibraryCalls(hModule);
		DllInst = hModule;
		CreateConsole();
		HANDLE hThread = CreateThread(NULL, NULL, InitThread, NULL, NULL, NULL);
		CloseHandle(hThread);
		XTrace("Injected\n");
	}
	return TRUE;
}

