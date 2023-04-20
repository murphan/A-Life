//
// Created by Emmet on 11/10/2022.
//

#include "body.h"
#include "rotation.h"

Body::Cell::Cell(u32 value): value(value) {}


auto Body::Cell::bodyPart() const -> BodyPart {
	return (BodyPart)(value & 0xff);
}

auto Body::Cell::data() const -> i32 {
	return (i32)((value >> 8) & 0xff);
}

auto Body::Cell::make(BodyPart bodyPart, i32 data) -> Cell {
	return Cell((data << 8) | bodyPart);
}

auto Body::Cell::modify(i32 modifier) -> void {
	value |= ((modifier + 1) << 16);
}

auto Body::Cell::makeEmpty() -> Body::Cell {
	return Cell(0_u32);
}

auto Body::Cell::isModified() const -> bool {
	return ((value >> 16) & 0xff) != 0;
}

auto Body::Cell::modifier() const -> i32 {
	return (i32)((value >> 16) & 0xff) - 1;
}

BodyBuilder::BodyBuilder() :
	currentX(0),
	currentY(0),
	anchors {
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
	} {}

auto BodyBuilder::getNextCellofType(BodyPart bodyPart, i32 & start) -> std::optional<Util::Coord> {
	for (auto i = start; i < insertedOrder.size(); ++i) {
		auto [current, x, y] = insertedOrder[i];
		if (current == bodyPart) {
			start = i;
			return std::make_optional<Util::Coord>(x, y);
		}
	}

	return std::nullopt;
}

auto BodyBuilder::add(BodyPart bodyPart, i32 x, i32 y) -> void {
	insertedOrder.push_back({ bodyPart, x, y });
}

auto canvasIndex(i32 width, i32 x, i32 y) -> i32 {
	return y * width + x;
}

auto Body::expand(i32 expandX, i32 expandY) -> void {
	auto newWidth = width;
	auto newHeight = height;
	auto newOriginX = originX;
	auto newOriginY = originY;
	auto offX = 0;
	auto offY = 0;

	if (expandX < 0) {
		newWidth += -expandX;
		newOriginX += -expandX;
		offX = -expandX;

	} else if (expandX > 0) {
		newWidth += expandX;
	}

	if (expandY < 0) {
		newHeight += -expandY;
		newOriginY += -expandY;
		offY = -expandY;

	} else if (expandY > 0) {
		newHeight += expandY;
	}

	auto newCanvas = std::vector<Cell>(newWidth * newHeight, Cell::makeEmpty());

	for (auto y = 0; y < height; ++y) {
		for (auto x = 0; x < width; ++x) {
			newCanvas[canvasIndex(newWidth, x + offX, y + offY)] = canvas[canvasIndex(width, x, y)];
		}
	}

	width = newWidth;
	height = newHeight;
	originX = newOriginX;
	originY = newOriginY;
	canvas = std::move(newCanvas);
}

auto Body::indexOf(i32 x, i32 y) const -> i32 {
	return (y + originY) * width + (x + originX);
}

auto Body::canvasLeft() const -> i32 {
    return -originX;
}

auto Body::canvasRight() const -> i32 {
    return (width - 1) - originX;
}

auto Body::canvasDown() const -> i32 {
    return -originY;
}

auto Body::canvasUp() const -> i32 {
    return (height - 1) - originY;
}

Body::Body(i32 edge):
    width(edge * 2 + 1), height(edge * 2 + 1),
	originX(edge), originY(edge),
	canvas(width * height, Cell::makeEmpty()),
    left(0), right(0), down(0), up(0), numCells(0)
{}

/**
 * @param builder the builder for the body creation process
 * @param direction direction to add the body part in
 * @param part the body segment to add
 * @param jumpAnchor use -1 for no usingAnchor, otherwise 0,1,2,3 for anchors A,B,C,D
 *
 * modifies the builder's current direction and current position
 */
auto Body::addCell(BodyBuilder & builder, Direction direction, Cell cell, i32 jumpAnchor) -> void {
    /* move from current position unless anchored, then jump */
	auto baseX = builder.currentX;
	auto baseY = builder.currentY;

	if (jumpAnchor > -1) {
		baseX = builder.anchors[jumpAnchor].x;
		baseY = builder.anchors[jumpAnchor].y;
	}

    /* keep moving in direction until finding an empty space */
    auto newX = baseX;
    auto newY = baseY;
    do {
        newX += direction.x();
        newY += direction.y();
    } while (accessExpand(newX, newY, 5).bodyPart() != BodyPart::NONE);

	builder.currentX = newX;
	builder.currentY = newY;

	directAddCell(builder, cell, newX, newY);
}

auto Body::directAddCell(BodyBuilder & builder, Cell cell, i32 x, i32 y) -> void {
	canvas[indexOf(x, y)] = cell;

	builder.add(cell.bodyPart(), x, y);

	/* update bounds */
	if (x < left) left = x;
	else if (x > right) right = x;

	if (y < down) down = y;
	else if (y > up) up = y;

	++numCells;
}

/** may resize the canvas if out of bounds */
auto Body::accessExpand(i32 x, i32 y, i32 expandBy) -> Cell {
    auto expandX = 0;
    auto expandY = 0;

    if (x < canvasLeft()) {
        expandX = -expandBy;
    } else if (x > canvasRight()) {
        expandX = expandBy;
    }
    if (y < canvasDown()) {
        expandY = -expandBy;
    } else if (y > canvasUp()) {
        expandY = expandBy;
    }
    if (expandX != 0 || expandY != 0) expand(expandX, expandY);

    return canvas[indexOf(x, y)];
}

auto Body::safeAccess(i32 x, i32 y) const -> Cell {
	if (x < canvasLeft() || x > canvasRight() || y < canvasDown() || y > canvasUp()) return Cell::makeEmpty();
	return canvas[indexOf(x, y)];
}

auto Body::directAccess(i32 x, i32 y) -> Body::Cell & {
	return canvas[indexOf(x, y)];
}

auto Body::access(i32 x, i32 y, Direction rotation) const -> Cell {
	auto [accessX, accessY] = Rotation::rotate({ x, y }, rotation);
	return safeAccess(accessX, accessY);
}

auto Body::debugToString() const -> std::string {
    auto string = std::string();
    string.reserve((width + 1) * height - 1);

    for (auto y = height - 1; y >= 0; --y) {
        for (auto x = 0; x < width; ++x) {
            string.push_back(canvas[canvasIndex(width, x, y)].bodyPart() + '0');
        }
        if (y > down) string.push_back('\n');
    }

    return string;
}

auto Body::getNumCells() const -> i32 {
	return numCells;
}

auto Body::getWidth() const -> i32 {
	return right - left + 1;
}

auto Body::getHeight() const -> i32 {
	return up - down + 1;
}

inline auto diagonalLength(i32 length0, i32 length1) {
	return Util::outer(sqrtf((f32)(length0 * length0 + length1 * length1)));
}

auto Body::getRight(Direction rotation) const -> i32 {
	switch (rotation.value()) {
		case Direction::RIGHT: return right;
		case Direction::RIGHT_UP: return diagonalLength(right, down);
		case Direction::UP: return -down;
		case Direction::LEFT_UP: return diagonalLength(down, left);
		case Direction::LEFT: return -left;
		case Direction::LEFT_DOWN: return diagonalLength(left, up);
		case Direction::DOWN: return up;
		case Direction::RIGHT_DOWN: return diagonalLength(up, right);
	}
}

auto Body::getUp(Direction rotation) const -> i32 {
	switch (rotation.value()) {
		case Direction::RIGHT: return up;
		case Direction::RIGHT_UP: return diagonalLength(up, right);
		case Direction::UP: return right;
		case Direction::LEFT_UP: return diagonalLength(right, down);
		case Direction::LEFT: return -down;
		case Direction::LEFT_DOWN: return diagonalLength(down, left);
		case Direction::DOWN: return -left;
		case Direction::RIGHT_DOWN: return diagonalLength(left, up);
	}
}

auto Body::getLeft(Direction rotation) const -> i32 {
	switch (rotation.value()) {
		case Direction::RIGHT: return left;
		case Direction::RIGHT_UP: return -diagonalLength(left, up);
		case Direction::UP: return -up;
		case Direction::LEFT_UP: return -diagonalLength(up, right);
		case Direction::LEFT: return -right;
		case Direction::LEFT_DOWN: return -diagonalLength(right, down);
		case Direction::DOWN: return down;
		case Direction::RIGHT_DOWN: return -diagonalLength(down, left);
	}
}

auto Body::getDown(Direction rotation) const -> i32 {
	switch (rotation.value()) {
		case Direction::RIGHT: return down;
		case Direction::RIGHT_UP: return -diagonalLength(down, left);
		case Direction::UP: return left;
		case Direction::LEFT_UP: return -diagonalLength(left, up);
		case Direction::LEFT: return -up;
		case Direction::LEFT_DOWN: return -diagonalLength(up, right);
		case Direction::DOWN: return -right;
		case Direction::RIGHT_DOWN: return -diagonalLength(right, down);
	}
}
