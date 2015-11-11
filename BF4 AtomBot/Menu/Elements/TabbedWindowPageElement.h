#pragma once

class TabbedWindowPageElement : public WindowElement
{
public:
	friend class TabbedWindowElement;
	TabbedWindowPageElement(const Context& Ctx);
	virtual ~TabbedWindowPageElement() = default;
	virtual void Draw(RenderInterface& Renderer) override;
protected:
	virtual bool PointInClient(const Vector2f& Point) override;
	virtual bool PointInRibbon(const Vector2f& Point) override;
	virtual void OnMouseMove(const MouseMessage& Msg) override;
};

TabbedWindowPageElement::TabbedWindowPageElement(const Context& Ctx) :
	WindowElement(Ctx)
{
	//Don't need a close button
	delete m_CloseButton;
	m_SubElements.clear();
}

void TabbedWindowPageElement::Draw(RenderInterface& Renderer)
{
	Renderer.BeginLine();
	Renderer.DrawFilledBox(m_Position, m_Size, m_Ctx.m_FillColor);
	Renderer.DrawLineBox(m_Position, m_Size, Color::Black());
	Renderer.EndLine();
	for (auto i = m_SubElements.rbegin(); i != m_SubElements.rend(); i++)
	{
		(*i)->Draw(Renderer);
	}
}

void TabbedWindowPageElement::OnMouseMove(const MouseMessage& Msg)
{
	MenuElement::OnMouseMove(Msg);
	for (MenuElement* Element : m_SubElements)
	{
		if (!Element->IsPointInControl(Msg.GetLocation()) &&
			Element->IsCursorInElement())
		{
			Element->OnMouseLeave(Msg);
			return;
		}
		else if (Element->IsPointInControl(Msg.GetLocation()) &&
			!Element->IsCursorInElement())
		{
			Element->OnMouseEnter(Msg);
			return;
		}

		if (Element->IsCursorInElement())
		{
			Element->OnMouseMove(Msg);
			return;
		}
	}
}

bool TabbedWindowPageElement::PointInClient(const Vector2f& Point)
{
	return MenuElement::IsPointInControl(Point);
}

bool TabbedWindowPageElement::PointInRibbon(const Vector2f& Point)
{
	//No ribbon, so always false
	return false;
}


