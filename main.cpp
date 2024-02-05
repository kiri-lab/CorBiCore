#include <random>
#include <iostream>
#include <thread>
#include <iomanip>

#include <simpleble/SimpleBLE.h>

#define SLEEP_TIME 50 // Java側のバッファ用。初期値：1(ms)

void print_byte_array_hex(SimpleBLE::ByteArray array);
void print_byte_array_float(SimpleBLE::ByteArray array);
SimpleBLE::Peripheral findCorBi(SimpleBLE::Adapter adaper);
// TODO 接続周りはコールバックに変更。
// TODO エラーハンドリングしっかり。
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

    // std::cout << "Adapter: " << adapter.identifier() << std::endl;
    // std::cout << "Address: " << adapter.address() << std::endl;

    SimpleBLE::Peripheral CorBiReader = findCorBi(adapter);
    if (CorBiReader.is_connectable())
        CorBiReader.connect();
    else
        return 1;
    if (!CorBiReader.is_connected())
        return 1;

    for (;;)
    {
        std::vector<std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuids;
        for (SimpleBLE::Service service : CorBiReader.services())
            for (SimpleBLE::Characteristic characteristic : service.characteristics())
                uuids.push_back(std::make_pair(service.uuid(), characteristic.uuid()));

        SimpleBLE::ByteArray rx_data = CorBiReader.read(uuids[0].first, uuids[0].second);
        print_byte_array_float(rx_data);
        // print_byte_array_hex(rx_data);
    }

    // std::cout << "Connected to CorBi." << std::endl;

    return 0;
}

void print_byte_array_hex(SimpleBLE::ByteArray array)
{
    std::reverse(array.begin(), array.end());
    for (auto byte : array)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (uint32_t)((uint8_t)byte) << " ";
    std::cout << std::endl;
}

void print_byte_array_float(SimpleBLE::ByteArray array)
{
    float f = *reinterpret_cast<float *>(array.data());
    std::cout << f << std::endl;
}

// FIXME タイムアウトとか実装した方が良さそうだぞ
// bool Peripheral.ininitialized()があるっぽ
SimpleBLE::Peripheral findCorBi(SimpleBLE::Adapter adapter)
{
    for (;;)
    {
        SimpleBLE::Peripheral CorBiReader = SimpleBLE::Peripheral();
        adapter.scan_for(500);
        std::vector<SimpleBLE::Peripheral> peripherals = adapter.scan_get_results();
        for (auto peripheral : peripherals)
        {
            if (peripheral.identifier() == "CorBi")
            {
                // std::cout << "CorBi is found." << std::endl;
                CorBiReader = peripheral;
                if (CorBiReader.is_connectable())
                    return CorBiReader;
            }
            // std::cout << "Peripheral: " << peripheral.identifier() << std::endl;
            // std::cout << "Address: " << peripheral.address() << std::endl;
        }
    }
}