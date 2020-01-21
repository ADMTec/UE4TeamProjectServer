#include "Constants/ServerConstants.hpp"
#include "Constants/GameConstants.hpp"
#include "Provider/ZoneDataProvider.hpp"
#include <iostream>



int main()
{
    try {
        const_cast<ServerConstants&>(ServerConstants::Instance()).Initialize();
        const_cast<GameConstants&>(GameConstants::Instance()).Initialize();
        const_cast<ZoneDataProvider&>(ZoneDataProvider::Instance()).Initialize();
    } catch (const std::exception & e) {
        std::cout << e.what();
    }



    return 0;
}