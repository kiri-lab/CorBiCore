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

    std::vector<SimpleBLE::Adapter> adapters = SimpleBLE::Adapter::get_adapters();
    if (adapters.empty())
    {
        std::cout << "No BLE adapters found." << std::endl;
        return 1;
    }

    SimpleBLE::Adapter adapter = adapters[0];

    std::cout << "Adapter: " << adapter.identifier() << std::endl;
    std::cout << "Address: " << adapter.address() << std::endl;

    adapter.scan_for(500);

    std::vector<SimpleBLE::Peripheral> peripherals = adapter.scan_get_results();

    for (auto peripheral : peripherals)
    {
        std::cout << "Peripheral: " << peripheral.identifier() << std::endl;
        std::cout << "Address: " << peripheral.address() << std::endl;
    }

    return 0;
}