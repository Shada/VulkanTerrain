#include <cstdio>
#include <iostream>

#include "version.hpp"
#include "MyMath/mymath.hpp"
#include "VulkanEngine/Model/VertexLayout.hpp"
#include "VulkanEngine/Model/VertexComponent.hpp"

int main()
{
    std::cout << "Hello World" << std::endl;
    int bob = 3;

    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_POSITION,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_COLOR,
    });

    std::cout
        << "sqrt(3) = " << mymath::sqrt(3) << std::endl;

    getchar();
}