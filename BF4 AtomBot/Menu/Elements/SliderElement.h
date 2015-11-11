#pragma once

template<typename T>
class SliderElement : public MenuElement
{
public:
	typedef EventDispatcher<void(const T&)> eValueChanged;

	virtual ~SliderElement() = default;
	virtual void Draw(RenderInterface& Renderer) override;
	virtual ElementType GetType() override;
	eValueChanged& EventValueChanged();

	T GetValue();

	struct Context
	{
		//Required
		Color m_Color;
		Vector2f m_Position;
		Vector2f m_Size;
		T m_Min;
		T m_Max;
		T m_DefaultValue;

		//Not Required
		float m_SliderWidth;
		Color m_TextColor;
		Context()
		{
			m_TextColor = Color::Black();
			m_SliderWidth = 5;
		}
	};
	SliderElement(const Context& Ctx);
protected:
	virtual bool IsPointInMouseDownZone(const Vector2f& Point) override;
	virtual bool IsCursorInElement() override;
	virtual void AddPosition(const Vector2f& NewAmount) override;
	virtual void OnMouseDown(const MouseMessage& Msg) override;
	virtual void OnMouseUp(const MouseMessage& Msg) override;
	virtual void OnMouseMove(const MouseMessage& Msg) override;
private:
	T Re_Range(float x, float inmin, float inmax, T outmin, T outmax)
	{
		double PercentInRange = (x - inmin) / (inmax - inmin);
		double NewVal = PercentInRange*(outmax - outmin) + outmin; //Just algebraic re-arrangement
		return (T)NewVal;
	}

	Context m_Ctx;
	T m_Min;
	T m_Max;
	T m_Value;

	bool m_DefaultSet;

	bool m_MouseDown;
	Vector2f m_SliderPos;
	Vector2f m_SliderOffset;

	float m_SliderLineXStart;
	float m_SliderLineXEnd;

	eValueChanged m_eValueChanged;
};

/*m_Size will not account for the text on top or bottom of the slider*/
template<typename T>
SliderElement<T>::SliderElement(typename const SliderElement<T>::Context& Ctx) :
	MenuElement(Ctx.m_Position, Ctx.m_Size)
{
	m_Ctx = Ctx;
	m_Min = m_Ctx.m_Min;
	m_Max = m_Ctx.m_Max;
	m_Value = m_Ctx.m_DefaultValue;
	m_SliderPos = m_Position;
	m_MouseDown = false;
	m_DefaultSet = false;
}

template<typename T>
void SliderElement<T>::Draw(RenderInterface& Renderer)
{

	/*                      Slider Name
	MinStr[5 pixel gap] ---------||---------[5 pixel gap]MaxStr
	CurrentStr
	*/
	Renderer.BeginText();
	Vector2f m_MinStrSz;
	Vector2f m_MaxStrSz;
	if (std::is_integral<T>::value)
	{
		m_MinStrSz = Renderer.MeasureString("%d", m_Min);
		Renderer.RenderText(m_Position, m_Ctx.m_TextColor, "%d", m_Min);

		m_MaxStrSz = Renderer.MeasureString("%d", m_Max);
		Renderer.RenderText(Vector2f(m_Position.x + m_Size.x - m_MaxStrSz.x, m_Position.y), m_Ctx.m_TextColor, "%d", m_Max);

		Vector2f ValStrSz = Renderer.MeasureString("%d", m_Value);
		Renderer.RenderText(Vector2f(m_SliderPos.x - ValStrSz.x / 2, m_SliderPos.y + m_Size.y), m_Ctx.m_TextColor, "%d", m_Value);
	}
	else if (std::is_floating_point<T>::value) {
		m_MinStrSz = Renderer.MeasureString("%.1f", m_Min);
		Renderer.RenderText(Vector2f(m_Position.x, m_Position.y), m_Ctx.m_TextColor, "%.1f", m_Min);

		m_MaxStrSz = Renderer.MeasureString("%.1f", m_Max);
		Renderer.RenderText(Vector2f(m_Position.x + m_Size.x - m_MaxStrSz.x, m_Position.y), m_Ctx.m_TextColor, "%.1f", m_Max);

		Vector2f ValStrSz = Renderer.MeasureString("%.1f", m_Value);
		Renderer.RenderText(Vector2f(m_SliderPos.x - ValStrSz.x / 2, m_SliderPos.y + m_Size.y), m_Ctx.m_TextColor, "%.1f", m_Value);
	}
	Renderer.EndText();
	Renderer.BeginLine();
	Renderer.DrawFilledBox(m_SliderPos, Vector2f(m_Ctx.m_SliderWidth, m_Size.y), m_Ctx.m_Color);

	m_SliderLineXStart = m_Position.x + m_MinStrSz.x + 5;
	m_SliderLineXEnd = m_Position.x + m_Size.x - 5 - m_MaxStrSz.x;
	Renderer.DrawLine(Vector2f(m_SliderLineXStart, m_Position.y + (m_Size.y / 2)), Vector2f(m_SliderLineXEnd, m_Position.y + (m_Size.y / 2)), Color::Black());
	Renderer.EndLine();

	//Have to wait for one draw call before setting default to know text sizes and where the line is
	if (!m_DefaultSet)
	{
		m_SliderPos.x = Re_Range(m_Ctx.m_DefaultValue, m_Min, m_Max, m_SliderLineXStart, m_SliderLineXEnd);
		m_DefaultSet = true;
	}
}

template<typename T>
bool SliderElement<T>::IsPointInMouseDownZone(const Vector2f& Point)
{
	if (Point.x > m_SliderPos.x && Point.x < (m_SliderPos.x + m_Ctx.m_SliderWidth) &&
		Point.y > m_SliderPos.y && Point.y < (m_SliderPos.y + m_Ctx.m_Size.y))
		return true;
	return false;
}

template<typename T>
bool SliderElement<T>::IsCursorInElement()
{
	if (m_MouseDown)
		return true;
	return MenuElement::IsCursorInElement();
}

template<typename T>
void SliderElement<T>::AddPosition(const Vector2f& NewAmount)
{
	MenuElement::AddPosition(NewAmount);
	m_SliderPos += NewAmount;
}

template<typename T>
void SliderElement<T>::OnMouseDown(const MouseMessage& Msg)
{
	MenuElement::OnMouseDown(Msg);
	m_MouseDown = true;
	m_SliderOffset = m_SliderPos - Msg.GetLocation();
}

template<typename T>
void SliderElement<T>::OnMouseUp(const MouseMessage& Msg)
{
	MenuElement::OnMouseUp(Msg);
	m_MouseDown = false;
}

template<typename T>
void SliderElement<T>::OnMouseMove(const MouseMessage& Msg)
{
	MenuElement::OnMouseMove(Msg);
	if (m_MouseDown)
	{
		Vector2f MousePos = Msg.GetLocation() + m_SliderOffset;
		if (MousePos.x > m_SliderLineXStart && MousePos.x < m_SliderLineXEnd)
		{
			m_SliderPos.x = MousePos.x;
			m_Value = Re_Range(m_SliderPos.x, m_SliderLineXStart, m_SliderLineXEnd, m_Min, m_Max);
		}
		else if (MousePos.x <= m_SliderLineXStart) {
			m_Value = m_Min;
		}
		else if (MousePos.y >= m_SliderLineXEnd) {
			m_Value = m_Max;
		}
	}
	m_eValueChanged.Invoke(m_Value);
}

template<typename T>
T SliderElement<T>::GetValue()
{
	return m_Value;
}

template<typename T>
typename SliderElement<T>::eValueChanged& SliderElement<T>::EventValueChanged()
{
	return m_eValueChanged;
}

template<typename T>
ElementType SliderElement<T>::GetType()
{
	return ElementType::Slider;
}
