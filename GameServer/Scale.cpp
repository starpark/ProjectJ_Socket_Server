#include "pch.h"
#include "Scale.h"

Scale::Scale(int index, int operatingWeight, int tolerance, int ivRow, int ivColumn)
	: Inventory(ivRow, ivColumn, SCALE_MAX_WEIGHT), index_(index), operatingWeight_(operatingWeight), tolerance_(tolerance)
{
}

Scale::~Scale()
{
}

bool Scale::IsOperating()
{
	return operatingWeight_ - tolerance_ <= GetCurrentWeight() && GetCurrentWeight() <= operatingWeight_ + tolerance_;
}
