#include <iostream>

#include <octk_network.h>
#include <octk_logging.hpp>

int main()
{
    std::cout << "exp_network_version start!" << std::endl;
    octk_network_init();
    OCTK_INFO("octk_network_version=%s", octk_network_version());
    return 0;
}
