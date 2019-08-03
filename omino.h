#include <ostream>
#include <set>
#include <vector>

class Omino
{
public:
    enum class Tile
    {
	Empty,
        Full,
	Neighbour
    };
    enum class CreateMode
    {
	ReflectionsIncluded,
	NoReflections
    };
    static std::set<Omino> createAll(int n, CreateMode mode);
    Omino(const int w, const int h, const std::vector<Tile>& tiles);
    std::set<Omino> transforms(const CreateMode mode) const;
    std::set<Omino> derive(const CreateMode mode) const;
    inline Tile at(int x, int y) const
    {
	return m_tiles.at(x + m_w * y);
    }
    void dump(std::ostream& os) const
    {
	for (int y = 0; y < m_h; ++y)
	{
	    for (int x = 0; x < m_w; ++x)
	    {
		switch (at(x, y))
		{
		case Tile::Empty:
		    os << ' ';
		    break;
		case Tile::Neighbour:
		    os << '.';
		    break;
		case Tile::Full:
		    os << '#';
		    break;
		}
	    }
	    os << '\n';
	}
    }
private:
    enum class Direction
    {
        North,
        South,
        East,
        West
    };

    int m_w;
    int m_h;
    std::vector<Tile> m_tiles;
    void expand(const Direction);
    Tile adjacent(const int x, const int y, const Direction d);
    Omino rotate(const Direction) const;
    Omino reflect() const;
    friend bool operator<(const Omino& lhs, const Omino& rhs);
};

bool operator<(const Omino& lhs, const Omino& rhs);
