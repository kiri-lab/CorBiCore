#include <random>
#include <iostream>
#include <thread>

#include <simpleble/SimpleBLE.h>

#define SLEEP_TIME 50 // Java側のバッファ用。初期値：1(ms)

// cppで書くのほとんど初めてなので、後でキモかったら直してね。
// とりま勉強がてら
int main(int argc, char **argv)
{
    if (!SimpleBLE::Adapter::bluetooth_enabled())
    {
        std::cout << "Bluetooth is not enabled." << std::endl;
        return 1;
    }

    auto adapters = SimpleBLE::Adapter::get_adapters();
    if (adapters.empty())
    {
        std::cout << "No BLE adapters found." << std::endl;
        return 1;
    }

    auto adapter = adapters[0];

    std::cout << "Adapter: " << adapter.identifier() << std::endl;
    std::cout << "Address: " << adapter.address() << std::endl;

    return 0;
}