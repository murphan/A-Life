
#ifndef ALIFE_MAPCELL_H
#define ALIFE_MAPCELL_H

#include <vector>
#include <optional>

#include "types.h"

#include "food.h"
#include "factor.h"

/**
 * Map Cells are the grid that cmake up the map
 * they provide locations for the organisims to move through
 * and contain the information that organisims need to make decisions
 */
class MapCell {
private:
    i8 factors[3];
	bool hasWall;
    std::optional<Food> food;

public:
	MapCell();
	MapCell(i8 factor0, i8 factor1, i8 factor2);
	explicit MapCell(i8 factors[3]);

    auto getFactor(Factor factor) const -> i8;
	auto setFactor(Factor factor, i8 value) -> void;

    auto setFood(Food newFood) -> void;

	auto getHasWall() const -> bool;
	auto getHasFood() const -> bool;

	auto getFood() -> Food &;

    auto removeFood() -> void;
};

#endif //ALIFE_MAPCELL_H
