#include "tobi.hpp"

#include <iostream>


int main(int argc, char** argv)
{
    tobi::Tobi t = tobi::createTobi();
    
    std::cout << t.getPosition().x << std::endl;
    
    return 0;
}
