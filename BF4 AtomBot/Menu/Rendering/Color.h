#pragma once

struct Color
{
	float R;
	float G;
	float B;
	float A;
	Color(float R_, float G_, float B_, float A_ = 1.0f)
	{
		R = R_;
		G = G_;
		B = B_;
		A = A_;
	}
	Color()
	{
		R = 0.0f;
		G = 0.0f;
		B = 0.0f;
		A = 0.0f;
	}

	static Color Black()
	{
		return Color(0,0,0);
	}
	static Color White()
	{
		return Color(1,1,1);
	}
	static Color Red()
	{
		return Color(1, 0, 0);
	}
	static Color Green()
	{
		return Color(0, 1, 0);
	}
	static Color Blue()
	{
		return Color(0, 0, 1);
	}
};