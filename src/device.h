#ifndef DEVICES_H
#define DEVICES_H

#include <vector>
#include "logging.h"

class Device {
    std::string Name;
    uint32_t Port;
public:
    Device(std::string name, uint32_t port) : Name(name), Port(port) {}
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void poll() = 0;
    virtual void process() = 0;
    virtual ~Device() {}
    std::string name() const { return Name; }
    uint32_t port() const { return Port; }
};

class DeviceManager { // might not need the class, maybe pure functions are better/simpler
    std::vector<Device&> devices;
public:
    DeviceManager(std::vector<Device&> devices) : devices(devices) {}

    void initializeDevices() {
        if (devices.empty()) {
            log(WARNING, "No devices added");
            return;
        }
        for (auto& device : devices) {
            log(DEBUG, "Initializing device: " + device.name() + ":" + std::to_string(device.port()));
            device.initialize();
        }
        log(INFO, std::to_string(devices.size()) + " devices initialized");
    }

    void shutdownDevices() {
        for (auto& device : devices) {
            log(DEBUG, "Shutting down device: " + device.name() + ":" + std::to_string(device.port()));
            device.shutdown();
        }
        log(INFO, std::to_string(devices.size()) + " devices shut down");
    }

    void resetDevice(int id) {
        if (id < 0 || id >= devices.size()) {
            log(ERROR, "Invalid device id: " + std::to_string(id));
            return;
        }
        log(INFO, "Resetting device: " + devices[id].name());
        devices[id].shutdown();
        devices[id].initialize();
        log(INFO, "Device reset: " + devices[id].name());
    }

    void pollDevices() {
        for (auto& device : devices) {
            device.poll();
        }
    }
};


// TODO: move these elsewhere
class DummyDevice1 : public Device {
public:
    void initialize() override {
        // Dummy initialization code for Device 1)
    }
    
    void shutdown() override {
        // Dummy shutdown code for Device 1
    }
};

class DummyDevice2 : public Device {
public:
    void initialize() override {
        // Dummy initialization code for Device 2
    }
    
    void shutdown() override {
        // Dummy shutdown code for Device 2
    }
};

#endif // DEVICES_H