# EdgeMqttDevX

EdgeMqttDevX is a lightweight IoT development framework focused on MQTT communication and edge device functionality. This is a fork of EdgeDevX with Azure IoT Hub/Central support removed to create a more focused, MQTT-centric library.

## Features

- **MQTT Communication**: Built-in MQTT client support via MQTT-C library
- **Cross-Platform**: Support for Linux, macOS, and embedded systems
- **GPIO Support**: Hardware abstraction for GPIO operations on supported platforms
- **JSON Serialization**: Lightweight JSON handling with Parson
- **Timer Management**: Asynchronous timer functionality
- **Utility Functions**: Common utility functions for edge development
- **OpenAI Integration**: Optional OpenAI API integration

## Key Changes from EdgeDevX

- **Removed Azure IoT Dependencies**: All Azure IoT Hub/Central specific code has been removed
- **Stub Implementation**: Azure IoT API functions are stubbed for compatibility but perform no operations
- **MQTT Focus**: Emphasis on direct MQTT communication rather than Azure-specific protocols
- **Lighter Weight**: Significantly reduced dependencies and binary size

## Building

### Prerequisites

- CMake 3.10 or higher
- C compiler (GCC, Clang, or MSVC)
- For macOS: Homebrew packages (libuv, curl, uuid, openssl)

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

### macOS Dependencies

```bash
# Install required dependencies via Homebrew
brew install libuv curl openssl ossp-uuid
```

## Library Structure

- `src/` - Core implementation files
- `include/` - Header files
- `MQTT-C/` - MQTT client library (submodule)

## Usage

Link against the `edge_devx` library and include the necessary headers:

```c
#include "dx_mqtt.h"
#include "dx_timer.h"
#include "dx_utilities.h"
```

## Migration from Azure IoT

If migrating from Azure IoT enabled EdgeDevX:

1. Azure IoT functions (`dx_azurePublish`, `dx_deviceTwinReportValue`, etc.) will compile but perform no operations
2. Replace Azure IoT telemetry with direct MQTT publishing using `dx_mqtt.h` functions
3. Use standard MQTT topic structures instead of Azure IoT device twin properties

## License

This project maintains the same license as the original EdgeDevX project.

## Contributing

Contributions are welcome! Please ensure all changes maintain compatibility with the existing API surface.
