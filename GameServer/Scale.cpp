#include "pch.h"
#include "Scale.h"
#include "Player.h"
#include "Match.h"

Scale::Scale(int index, int operatingWeight, int tolerance, int ivRow, int ivColumn)
	: Inventory(index, ivRow, ivColumn, SCALE_MAX_WEIGHT), isOpen_(false), operatingWeight_(operatingWeight), tolerance_(tolerance)
{
}

Scale::~Scale()
{
}

bool Scale::IsOperating()
{
	isOpen_.store(operatingWeight_ - tolerance_ <= GetCurrentWeight() && GetCurrentWeight() <= operatingWeight_ + tolerance_);

	return isOpen_.load();
}

void Scale::CalculateAttribution()
{
	for (auto item : owningItems_)
	{
		if (auto addedPlayerIndex = item.second->prevOwnedPlayerIndex)
		{
			addedPlayerIndex->AddScore(Match::ACTIVATING_ESCAPE_CONTRIBUTION_SCORE);
		}
	}
}
