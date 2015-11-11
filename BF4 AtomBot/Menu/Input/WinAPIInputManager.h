#pragma once

Vector2f GetMousePos()
{
	POINT Pos;
	GetCursorPos(&Pos);
	return Vector2f(Pos.x, Pos.y);
}

class WinAPIInputManager :public InputManagerInterface
{
public:
	virtual void PollKeyboard() override;
	virtual void PollMouse() override;
	virtual Vector2f MapCursorToScreenSpace(const Vector2f& CursorPos) const override;
	Vector2f GetMappedMousePos();
	WinAPIInputManager(HWND hWND):m_OldMouseLeftKeyState(0){
		m_hWnd = hWND;
	}
private:
	SHORT m_OldMouseLeftKeyState;
	SHORT m_OldMouseRightKeyState;
	Vector2f m_OldMousePosition;
	HWND m_hWnd;

	int GetLeftMouseVK()
	{
		return IsMouseSwapped() ? VK_RBUTTON : VK_LBUTTON;
	}
	int GetRightMouseVK()
	{
		return IsMouseSwapped() ? VK_LBUTTON : VK_RBUTTON;
	}
	bool IsKeyDown(SHORT KeyQuery)
	{
		return KeyQuery & 0x8000;
	}
	bool IsMouseSwapped()
	{
		//People can swap left button on mouse to be equal to right button action
		if (GetSystemMetrics(SM_SWAPBUTTON) == TRUE)
			return true;
		return false;
	}
};

void WinAPIInputManager::PollKeyboard()
{

}

void WinAPIInputManager::PollMouse()
{
	POINT mouse;
	if (!GetCursorPos(&mouse))
		return;
	Vector2f CurPos = MapCursorToScreenSpace(Vector2f(mouse.x, mouse.y));

	SHORT MouseLeftKeyState = GetAsyncKeyState(GetLeftMouseVK());
	if (IsKeyDown(MouseLeftKeyState) && !IsKeyDown(m_OldMouseLeftKeyState))
	{
		MouseMessage Msg(CurPos, MouseMessage::MouseEvent::BtnDown,
			MouseMessage::MouseButton::Left);
		m_eProcessMouseMessage.Invoke(Msg);
	}

	if (!IsKeyDown(MouseLeftKeyState) && IsKeyDown(m_OldMouseLeftKeyState))
	{
		MouseMessage Msg(CurPos, MouseMessage::MouseEvent::BtnUp,
			MouseMessage::MouseButton::Left);
		m_eProcessMouseMessage.Invoke(Msg);
	}

	SHORT MouseRightKeyState = GetAsyncKeyState(GetRightMouseVK());
	if (IsKeyDown(MouseRightKeyState) && !IsKeyDown(m_OldMouseRightKeyState))
	{
		MouseMessage Msg(CurPos, MouseMessage::MouseEvent::BtnDown,
			MouseMessage::MouseButton::Right);
		m_eProcessMouseMessage.Invoke(Msg);
	}

	if (!IsKeyDown(MouseRightKeyState) && IsKeyDown(m_OldMouseRightKeyState))
	{
		MouseMessage Msg(CurPos, MouseMessage::MouseEvent::BtnUp,
			MouseMessage::MouseButton::Right);
		m_eProcessMouseMessage.Invoke(Msg);
	}

	if (CurPos != m_OldMousePosition)
	{
		MouseMessage Msg(CurPos, MouseMessage::MouseEvent::Move,
			MouseMessage::MouseButton::None);
		m_eProcessMouseMessage.Invoke(Msg);
	}
	m_OldMousePosition = CurPos;
	m_OldMouseLeftKeyState = MouseLeftKeyState;
	m_OldMouseRightKeyState = MouseRightKeyState;
}

Vector2f WinAPIInputManager::MapCursorToScreenSpace(const Vector2f& CursorPos) const
{
	POINT point;
	point.x = (LONG)CursorPos.x;
	point.y = (LONG)CursorPos.y;
	if (!ScreenToClient(m_hWnd, &point))
	{
		printf("Screen To Client Failed\n");
		return CursorPos;
	}
	return Vector2f(point.x, point.y);
}

Vector2f WinAPIInputManager::GetMappedMousePos()
{
	return MapCursorToScreenSpace(GetMousePos());
}