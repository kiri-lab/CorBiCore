#include <random>
#include <iostream>
#include <thread>
#include <iomanip>

#include <simpleble/SimpleBLE.h>

#define SLEEP_TIME 50 // Java側のバッファ用。初期値：1(ms)

void print_byte_array_hex(SimpleBLE::ByteArray array);
void print_byte_array_float(SimpleBLE::ByteArray array);
void print_byte_array_uint16(SimpleBLE::ByteArray array);
void print_byte_array_int(SimpleBLE::ByteArray array);
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
    SimpleBLE::ByteArray old_data = "";
    for (;;)
    {
        std::vector<std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuids;
        for (SimpleBLE::Service service : CorBiReader.services())
            for (SimpleBLE::Characteristic characteristic : service.characteristics())
            {
                uuids.push_back(std::make_pair(service.uuid(), characteristic.uuid()));
                for (SimpleBLE::Descriptor descriptor : characteristic.descriptors())
                    std::cout << CorBiReader.read(service.uuid(), characteristic.uuid(), descriptor.uuid()) << std::endl; // とりあえず取得テスト
            }
        // FIXME サンプルに従ってUUIDを取得する方法にしてたけど、UUID固定してるなら直接指定でも良いかも。
        SimpleBLE::ByteArray rx_data = CorBiReader.read(uuids[0].first, uuids[0].second);
        SimpleBLE::ByteArray rx_data_RED = CorBiReader.read(uuids[1].first, uuids[1].second);
        SimpleBLE::ByteArray rx_data_IR = CorBiReader.read(uuids[2].first, uuids[2].second);
        if (rx_data_RED != old_data)
        {

            print_byte_array_uint16(rx_data_IR);
            std::cout << "k, ";
            print_byte_array_int(rx_data_RED);
            std::cout << "r, ";
            print_byte_array_int(rx_data);
            std::cout << std::endl;
        }
        old_data = rx_data_RED;
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

void print_byte_array_uint16(SimpleBLE::ByteArray array)
{
    for (size_t i = 0; i < array.size(); i += 2)
    {
        uint16_t number = (static_cast<unsigned char>(array[i]) << 8) | static_cast<unsigned char>(array[i + 1]);
        std::cout << number << ",";
    }
}

void print_byte_array_int(SimpleBLE::ByteArray array)
{
    int i = *reinterpret_cast<int *>(array.data());
    std::cout << i;
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