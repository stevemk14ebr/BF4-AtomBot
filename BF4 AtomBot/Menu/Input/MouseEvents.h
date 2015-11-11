#pragma once

class MouseMessage
{
public:
	enum class MouseEvent
	{
		None,
		Move,
		BtnDown,
		BtnUp,
		Scroll
	};
	enum class MouseButton
	{
		None,
		Left,
		Right,
		Middle,
		XButton1,
		XButton2
	};

	MouseMessage():m_Event(MouseEvent::None),
		m_ButtonPressed(MouseButton::None)
	{
		m_Location = Vector2f(0.f, 0.f);
	}
	MouseMessage(const Vector2f& Loc,const MouseEvent event,const MouseButton Btn)
	{
		m_Location = Loc;
		m_Event = event;
		m_ButtonPressed = Btn;
	}

	Vector2f GetLocation() const;
	MouseEvent GetEvent() const;
	MouseButton GetButton() const;
private:
	Vector2f m_Location;
	MouseEvent m_Event;
	MouseButton m_ButtonPressed;
};

Vector2f MouseMessage::GetLocation() const
{
	return m_Location;
}

MouseMessage::MouseEvent MouseMessage::GetEvent() const
{
	return m_Event;
}

MouseMessage::MouseButton MouseMessage::GetButton() const
{
	return m_ButtonPressed;
}