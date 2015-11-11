#pragma once

class TabbedWindowElement : public MenuElement
{
public:
	typedef EventDispatcher<void(const uint32_t)> eClosePressed;

	//Returns tab ID
	typedef EventDispatcher<void(const uint32_t)> eTabPressed;
	virtual void Draw(RenderInterface& Renderer) override;
	virtual ElementType GetType() override;

	struct Context
	{
		std::string m_WindowName;
		Color m_TitleFillColor;
		Color m_TabFillColor;
		Color m_TabTitleFillColor;
		Vector2f m_Size;
		Vector2f m_Position;

		//optional
		uint32_t m_DefaultTabFocusIndex;
		float m_TabBarHeight;
		float m_BorderWidth;
		Color m_TabTextColor;
		Color m_DepthHighlightColor;
		Color m_DepthShadeColor;
		Context()
		{
			m_DefaultTabFocusIndex = 0;
			m_TabBarHeight = 25;
			m_BorderWidth = 5;
			m_TabTextColor = Color::Black();
			m_DepthShadeColor = Color::Black();
			m_DepthHighlightColor = Color::White();
		}
	};

	TabbedWindowElement(const Context& Ctx);
	~TabbedWindowElement();

	uint32_t AddTabPage(const std::string& TabName);

	template<typename T>
	T* GetElementById(const uint32_t Id);

	eTabPressed& EventTabPressed();
protected:
	virtual void OnMouseMove(const MouseMessage& Msg) override;
	virtual void OnMouseDown(const MouseMessage& Msg) override;
	virtual void OnPositionChanged(const Vector2f& NewPosition) override;
	virtual void OnMouseUp(const MouseMessage& Msg) override;
	virtual bool PointInRibbon(const Vector2f& Point);
	virtual bool PointInClient(const Vector2f& Point);
	virtual bool IsCursorInElement() override;
	virtual void OnTabPressed(const MouseMessage& Msg, const uint32_t TabIndex);
	virtual void OnClosePressed(const MouseMessage& Msg);
	void ForwardMouseEnterLeave(MenuElement* Element, const MouseMessage &Msg);
private:
	Context m_Ctx;
	std::vector<TabbedWindowPageElement*> m_TabPages;
	std::vector<ButtonElement*> m_TabButtons;
	uint32_t m_TabInFocus;

	Vector2f m_DragOffsetFromPosition;
	bool m_IsMouseDown;
	eTabPressed m_eTabPressed;
	eClosePressed m_eClosePressed;
};

TabbedWindowElement::TabbedWindowElement(const Context& Ctx) :
	MenuElement(Ctx.m_Position, Ctx.m_Size)
{
	m_Ctx = Ctx;
	m_TabInFocus = m_Ctx.m_DefaultTabFocusIndex;
	m_IsMouseDown = false;

}

TabbedWindowElement::~TabbedWindowElement()
{
	for (MenuElement* Element : m_TabPages)
	{
		delete Element;
	}
	m_TabPages.clear();
}

uint32_t TabbedWindowElement::AddTabPage(const std::string& TabName)
{
	TabbedWindowPageElement::Context TabCtx;
	TabCtx.m_WindowName = TabName;
	TabCtx.m_FillColor = m_Ctx.m_TabFillColor;
	TabCtx.m_TitleFillColor = m_Ctx.m_TabTitleFillColor;
	TabCtx.m_Position = Vector2f(m_Position.x + m_Ctx.m_BorderWidth, m_Position.y + m_Ctx.m_TabBarHeight * 2);
	TabCtx.m_Size = Vector2f(m_Size.x - (m_Ctx.m_BorderWidth * 2), m_Size.y - (m_Ctx.m_TabBarHeight * 2) - m_Ctx.m_BorderWidth);
	TabCtx.m_TitleBarHeight = m_Ctx.m_TabBarHeight;
	TabCtx.m_TextColor = m_Ctx.m_TabTextColor;
	TabbedWindowPageElement* TabPage = new TabbedWindowPageElement(TabCtx);
	m_TabPages.push_back(TabPage);

	ButtonElement::Context BtnCtx;
	BtnCtx.m_ButtonText = TabName;
	BtnCtx.m_FillColor = m_Ctx.m_TabFillColor;
	BtnCtx.m_FillColorMouseDown = Color::White();
	BtnCtx.m_FillColorMouseOver = Color::Black();
	//+2 to account for zero index and +1 more for the button being added
	float WidthPerBtn = (m_Size.x - (m_Ctx.m_BorderWidth * 2)) / (m_TabButtons.size() + 2);
	int BtnIndex = 0;
	for (ButtonElement* Btn : m_TabButtons)
	{
		Btn->SetSize(Vector2f(WidthPerBtn, m_Ctx.m_TabBarHeight));
		Btn->SetPosition(Vector2f(WidthPerBtn*BtnIndex + m_Position.x + m_Ctx.m_BorderWidth, Btn->GetPosition().y));
		BtnIndex++;
	}
	BtnCtx.m_Size = Vector2f(WidthPerBtn, m_Ctx.m_TabBarHeight);
	BtnCtx.m_Position = Vector2f(WidthPerBtn*BtnIndex + m_Ctx.m_BorderWidth, m_Ctx.m_TabBarHeight);
	ButtonElement* TabBtn = new ButtonElement(BtnCtx);
	TabBtn->EventMouseDown() += std::bind(&TabbedWindowElement::OnTabPressed,
		this, std::placeholders::_1, BtnIndex);

	TabBtn->AddPosition(m_Position); //make relative to window
	m_TabButtons.push_back(TabBtn);
	return TabPage->GetId();
}

void TabbedWindowElement::Draw(RenderInterface& Renderer)
{
	Renderer.BeginLine();
	Renderer.DrawFilledBox(m_Position, m_Size, m_Ctx.m_TitleFillColor);
	Renderer.DrawLineBox(m_Position, m_Size, Color::Black());

	Vector2f TopLeft = m_Position + Vector2f(1, 1);
	Vector2f TopRight = Renderer.GetRectPoint(m_Position, m_Size, RenderInterface::RectPoint::TopRight) + Vector2f(-1, 1);
	Vector2f BottomRight = Renderer.GetRectPoint(m_Position, m_Size, RenderInterface::RectPoint::BottomRight) + Vector2f(-1, -1);
	Vector2f BottomLeft = Renderer.GetRectPoint(m_Position, m_Size, RenderInterface::RectPoint::BottomLeft) + Vector2f(1, -1);
	Renderer.DrawLine(TopLeft, TopRight, m_Ctx.m_DepthHighlightColor);
	Renderer.DrawLine(TopLeft, BottomLeft, m_Ctx.m_DepthHighlightColor);
	Renderer.DrawLine(TopRight, BottomRight, m_Ctx.m_DepthShadeColor);
	Renderer.DrawLine(BottomLeft, BottomRight, m_Ctx.m_DepthShadeColor);
	Renderer.EndLine();

	Vector2f WndNameSz = Renderer.MeasureString("%s", m_Ctx.m_WindowName.c_str());
	float DeltaHeight = m_Ctx.m_TabBarHeight - WndNameSz.y;
	DeltaHeight /= 2;
	Renderer.BeginText();
	Renderer.RenderText(Vector2f(m_Position.x + m_Ctx.m_BorderWidth, m_Position.y + DeltaHeight), m_Ctx.m_TabTextColor, "%s", m_Ctx.m_WindowName.c_str());
	Renderer.EndText();

	for (ButtonElement* Btn : m_TabButtons)
	{
		Btn->Draw(Renderer);
	}
	if (m_TabInFocus > m_TabPages.size())
		return;

	m_TabPages.at(m_TabInFocus)->Draw(Renderer);
}

ElementType TabbedWindowElement::GetType()
{
	return ElementType::TabbedWindow;
}

void TabbedWindowElement::OnMouseMove(const MouseMessage& Msg)
{
	MenuElement::OnMouseMove(Msg);
	m_TabPages.at(m_TabInFocus)->OnMouseMove(Msg);

	if (m_IsMouseDown)
	{
		Vector2f NewPosition = Msg.GetLocation() + m_DragOffsetFromPosition;
		OnPositionChanged(NewPosition);
	}

	for (ButtonElement* Button : m_TabButtons)
	{
		ForwardMouseEnterLeave(Button, Msg);
	}
}

void TabbedWindowElement::OnPositionChanged(const Vector2f& NewPosition)
{
	Vector2f DeltaPosition = NewPosition - m_Position;
	m_Position = NewPosition;
	m_ePositionChanged.Invoke(m_Position);

	Vector2f NewTabPosition = Vector2f(m_Position.x + m_Ctx.m_BorderWidth, m_Position.y + m_Ctx.m_TabBarHeight * 2);
	for (TabbedWindowPageElement* Element : m_TabPages)
	{
		Element->OnPositionChanged(NewTabPosition);
	}

	for (ButtonElement* Button : m_TabButtons)
	{
		Button->AddPosition(DeltaPosition);
	}
}

void TabbedWindowElement::OnMouseDown(const MouseMessage& Msg)
{
	MenuElement::OnMouseDown(Msg);
	if (Msg.GetButton() == MouseMessage::MouseButton::Left && PointInRibbon(Msg.GetLocation()))
	{
		m_IsMouseDown = true;
		m_DragOffsetFromPosition = m_Position - Msg.GetLocation();
		m_eZOrderChanged.Invoke(GetId());
	}

	for (MenuElement* Button : m_TabButtons)
	{
		if (Button->IsPointInMouseDownZone(Msg.GetLocation()))
			Button->OnMouseDown(Msg);
	}
	m_TabPages.at(m_TabInFocus)->OnMouseDown(Msg);
}

void TabbedWindowElement::OnMouseUp(const MouseMessage& Msg)
{
	MenuElement::OnMouseUp(Msg);
	if (Msg.GetButton() == MouseMessage::MouseButton::Left)
		m_IsMouseDown = false;

	m_TabPages.at(m_TabInFocus)->OnMouseUp(Msg);
	for (MenuElement* Button : m_TabButtons)
	{
		Button->OnMouseUp(Msg);
	}
}

void TabbedWindowElement::OnClosePressed(const MouseMessage& Msg)
{
	m_eClosePressed.Invoke(GetId());
	printf("Closed\n");
}

void TabbedWindowElement::ForwardMouseEnterLeave(MenuElement* Element, const MouseMessage &Msg)
{
	if (!Element->IsPointInControl(Msg.GetLocation()) &&
		Element->IsCursorInElement())
	{
		Element->OnMouseLeave(Msg);
	}
	else if (Element->IsPointInControl(Msg.GetLocation()) &&
		!Element->IsCursorInElement())
	{
		Element->OnMouseEnter(Msg);
	}

	if (Element->IsCursorInElement())
	{
		Element->OnMouseMove(Msg);
	}
}

bool TabbedWindowElement::PointInRibbon(const Vector2f& Point)
{
	Vector2f TabRowBegin = Vector2f(m_Position.x + m_Ctx.m_BorderWidth, m_Position.y + m_Ctx.m_TabBarHeight);
	if (Point.x > m_Position.x && Point.x < (m_Position.x + m_Size.x) &&
		Point.y > m_Position.y && Point.y < TabRowBegin.y)
		return true;
	return false;
}

bool TabbedWindowElement::PointInClient(const Vector2f& Point)
{
	return m_TabPages.at(m_TabInFocus)->PointInClient(Point);
}

bool TabbedWindowElement::IsCursorInElement()
{
	if (m_IsMouseDown)
		return true;
	return MenuElement::IsCursorInElement();
}

void TabbedWindowElement::OnTabPressed(const MouseMessage& Msg, const uint32_t TabIndex)
{
	if (TabIndex > m_TabPages.size())
		return;
	m_TabInFocus = TabIndex;
	m_eTabPressed.Invoke(m_TabPages.at(m_TabInFocus)->GetId());
}

TabbedWindowElement::eTabPressed& TabbedWindowElement::EventTabPressed()
{
	return m_eTabPressed;
}

template<typename T>
T* TabbedWindowElement::GetElementById(const uint32_t Id)
{
	for (MenuElement* Element : m_TabPages)
	{
		if (Element->GetId() == Id)
			return dynamic_cast<T*>(Element);

		if (Element->GetType() == ElementType::Window)
		{
			WindowElement* WindowElem = dynamic_cast<WindowElement*>(Element);
			if (WindowElem == nullptr)
				continue;

			T* RetElement = WindowElem->GetElementById<T>(Id);
			if (RetElement == nullptr)
				continue;
			return RetElement;
		}
	}
	return nullptr;
}

