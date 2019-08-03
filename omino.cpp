#include "omino.h"

#include <map>

namespace
{

static Omino TheMonomino =
{
    3,
    3,
    { Omino::Tile::Empty, Omino::Tile::Neighbour, Omino::Tile::Empty,
      Omino::Tile::Neighbour, Omino::Tile::Full, Omino::Tile::Neighbour,
      Omino::Tile::Empty, Omino::Tile::Neighbour, Omino::Tile::Empty
    }
};

}

std::set<Omino> Omino::createAll(int n, CreateMode mode)
{
    if (n <= 1)
    {
	return { TheMonomino };
    }
    std::set<Omino> os;
    for (auto const& o : createAll(n-1, mode))
    {
	auto const ds = o.derive(mode);
	std::copy(ds.begin(), ds.end(), std::inserter(os, os.begin()));
    }
    return os;
}

Omino::Omino(const int w, const int h, const std::vector<Tile>& tiles)
    : m_w(w), m_h(h), m_tiles(tiles)
{
    // Embiggen where necessary so that no full tile is adjacent the edge.
    std::set<Direction> expandDirs;
    for (int x = 0; x < m_w; ++x)
    {
	if (at(x, 0) == Tile::Full) expandDirs.insert(Direction::North);
	if (at(x, m_h - 1) == Tile::Full) expandDirs.insert(Direction::South);
    }
    for (int y = 0; y < m_h; ++y)
    {
	if (at(0, y) == Tile::Full) expandDirs.insert(Direction::West);
	if (at(m_w - 1, y) == Tile::Full) expandDirs.insert(Direction::East);
    }
    for (auto const d : expandDirs)
    {
	expand(d);
    }
    // Find and mark neighbours of full tiles.
    for (int y = 0; y < m_h; ++y)
    {
	for (int x = 0; x < m_w; ++x)
	{
	    if (at(x, y) == Tile::Empty &&
		(adjacent(x, y, Direction::North) == Tile::Full ||
		 adjacent(x, y, Direction::South) == Tile::Full ||
		 adjacent(x, y, Direction::East) == Tile::Full ||
		 adjacent(x, y, Direction::West) == Tile::Full))
	    {
		m_tiles[x + m_w * y] = Tile::Neighbour;
	    }
	}
    }
}

std::set<Omino> Omino::transforms(const CreateMode mode) const
{
    std::set<Omino> os;
    os.insert(rotate(Direction::North));
    os.insert(rotate(Direction::South));
    os.insert(rotate(Direction::East));
    os.insert(rotate(Direction::West));
    if (mode == CreateMode::ReflectionsIncluded)
    {
	os.insert(rotate(Direction::North).reflect());
	os.insert(rotate(Direction::South).reflect());
	os.insert(rotate(Direction::East).reflect());
	os.insert(rotate(Direction::West).reflect());
    }
    return os;
}

std::set<Omino> Omino::derive(const CreateMode mode) const
{
    // Note that the mode used in canonicalising the resultant omino is the
    // opposite of the derive mode.
    const CreateMode transformMode = (mode == CreateMode::ReflectionsIncluded) ?
	CreateMode::NoReflections : CreateMode::ReflectionsIncluded;
    std::set<Omino> os;
    for (int x = 0; x < m_w; ++x)
    {
	for (int y = 0; y < m_h; ++y)
	{
	    if (at(x, y) == Tile::Neighbour)
	    {
		std::vector<Tile> nt = m_tiles;
		nt[x + m_w * y] = Tile::Full;
		Omino o(m_w, m_h, nt);
		os.insert(*o.transforms(transformMode).begin());
	    }
	}
    }
    return os;
}

void Omino::expand(const Direction d)
{
    static const std::map<Direction, std::pair<int, int>> expandBy = {
	{ Direction::North, { 0, 1 } },
	{ Direction::South, { 0, 1 } },
	{ Direction::East, { 1, 0 } },
	{ Direction::West, { 1, 0 } }
    };
    static const std::map<Direction, std::pair<int, int>> shiftBy = {
	{ Direction::North, { 0, 1 } },
	{ Direction::South, { 0, 0 } },
	{ Direction::East, { 0, 0 } },
	{ Direction::West, { 1, 0 } }
    };
    int nw = m_w + expandBy.at(d).first;
    int nh = m_h + expandBy.at(d).second;
    std::vector<Tile> newTiles(nw * nh, Tile::Empty);
    for (int y = 0; y < m_h; ++y)
    {
	for (int x = 0; x < m_w; ++x)
	{
	    newTiles[x + shiftBy.at(d).first + nw * (y + shiftBy.at(d).second)]
		= at(x, y);
	}
    }
    std::swap(m_w, nw);
    std::swap(m_h, nh);
    std::swap(m_tiles, newTiles);
}

Omino::Tile Omino::adjacent(const int x, const int y, const Direction d)
{
    static const std::map<Direction, std::pair<int, int>> moveBy = {
	{ Direction::North, { 0, -1 } },
	{ Direction::South, { 0, 1 } },
	{ Direction::East, { 1, 0 } },
	{ Direction::West, { -1, 0 } }
    };
    if ((x <= 0 && d == Direction::West) ||
	(x >= m_w - 1 && d == Direction::East) ||
	(y <= 0 && d == Direction::North) ||
	(y >= m_h - 1 && d == Direction::South))
    {
	return Tile::Empty;
    }
    return at(x + moveBy.at(d).first, y + moveBy.at(d).second);
}

Omino Omino::rotate(const Direction d) const
{
    int nw = (d == Direction::North || d == Direction::South) ? m_w : m_h;
    int nh = (d == Direction::North || d == Direction::South) ? m_h : m_w;
    std::vector<Tile> nt = m_tiles;
    for (int x = 0; x < m_w; ++x)
    {
	for (int y = 0; y < m_h; ++y)
	{
	    int dx = x, dy = y;
	    switch (d)
	    {
	    case Direction::North:
		break;
	    case Direction::South:
		dx = nw - 1 - x;
		dy = nh - 1 - y;
		break;
	    case Direction::East:
		dx = nw - 1 - y;
		dy = x;
		break;
	    case Direction::West:
		dx = y;
		dy = nh - 1 - x;
		break;
	    }
	    nt[dx + nw * dy] = at(x, y);
	}
    }
    return Omino(nw, nh, nt);
}

Omino Omino::reflect() const
{
    Omino o(*this);
    for (int y = 0; y < m_h; ++y)
    {
	for (int x = 0; x < m_w; ++x)
	{
	    o.m_tiles[x + m_w * y] = at(m_w - 1 - x, y);
	}
    }
    return o;
}

bool operator<(const Omino& lhs, const Omino& rhs)
{
    if (lhs.m_w != rhs.m_w)
    {
	return (lhs.m_w < rhs.m_w);
    }
    if (lhs.m_h != rhs.m_h)
    {
	return (lhs.m_h < rhs.m_h);
    }
    return (lhs.m_tiles < rhs.m_tiles);
}

