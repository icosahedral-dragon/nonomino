
#include "omino.h"

#include <iostream>

int main()
{
    auto const os = Omino::createAll(4, Omino::CreateMode::ReflectionsIncluded);
    for (auto const& o : os)
    {
	o.dump(std::cout);
	std::cout << std::endl;
    }
    std::cout << os.size() << std::endl;
}
