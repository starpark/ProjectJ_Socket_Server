#pragma once
#include "Inventory.h"

class Scale : public Inventory
{
	enum
	{
		SCALE_MAX_WEIGHT = 999'999'999
	};

public:
	Scale(int index, int operatingWeight, int tolerance, int ivRow, int ivColumn);
	~Scale() override;

	int GetIndex() const { return index_; }
	bool IsOperating();
	int GetOperatingWeight() const { return operatingWeight_; }
	int GetTolerance() const { return tolerance_; }

private:
	int operatingWeight_;
	int tolerance_;
};
