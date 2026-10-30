// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicCameraTypes.h"

#include "Curves/CurveFloat.h"

float FCameraBlend::Evaluate(float NormalizedTime) const
{
	const float T = FMath::Clamp(NormalizedTime, 0.f, 1.f);
	const float Exp = FMath::Max(Exponent, 1.f);

	switch (Type)
	{
	case ECameraBlendType::Linear:
		return T;
	case ECameraBlendType::EaseIn:
		return FMath::InterpEaseIn(0.f, 1.f, T, Exp);
	case ECameraBlendType::EaseOut:
		return FMath::InterpEaseOut(0.f, 1.f, T, Exp);
	case ECameraBlendType::EaseInOut:
		return FMath::InterpEaseInOut(0.f, 1.f, T, Exp);
	case ECameraBlendType::Custom:
		return Curve ? FMath::Clamp(Curve->GetFloatValue(T), 0.f, 1.f) : T;
	}
	return T;
}