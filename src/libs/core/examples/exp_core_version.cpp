#include <iostream>

#include <octk_core.h>
#include <octk_logging.hpp>

int main()
{
    std::cout << "exp_core_version start!" << std::endl;
    octk_core_init();
    OCTK_INFO("octk_core_version=%s", octk_core_version());
    return 0;
}
