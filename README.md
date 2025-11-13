# LinuxCanbusTool

A comprehensive Linux CAN bus tool with DBC file parsing, CAN message transceiver, data recording and database storage capabilities.

## 🚀 Features

### Core Functions
- **DBC File Parsing** - Support for reading and parsing standard DBC format files
- **CAN Message Parsing** - Automatic parsing of CAN message signals based on DBC definitions
- **CAN Message Packaging** - Package signal values into CAN messages according to DBC definitions
- **Bidirectional Communication** - Support for both CAN message transmission and reception
- **Real-time Monitoring** - Real-time display of CAN signal value changes

### Data Processing
- **CSV Data Recording** - Real-time CAN data recording and saving in CSV format
- **MySQL Database** - Save CAN data to MySQL database
- **Python Integration** - Support for calling Python scripts for data processing
- **Multi-interface Support** - Support for multiple CAN interfaces like can0, can1, etc.

### Testing Features
- **Loopback Testing** - Loopback functionality testing between can0 and can1 interfaces
- **Timed Transmission** - Support for different transmission cycles (10ms, 20ms, 50ms, 100ms)
- **Performance Statistics** - Statistics for message parsing, packaging, and transmission/reception timing

## 📋 Requirements

- **Operating System**: Linux (with SocketCAN support)
- **Compiler**: GCC with C99 support
- **Dependencies**:
  - pthread (for multi-threading support)
  - MySQL client library (optional, for database functionality)
  - Python development headers (optional, for Python integration)

## 🔧 Build

### Using Code::Blocks
The project includes Code::Blocks project files (`*.cbp`) that can be opened and compiled directly in Code::Blocks.

### Manual Compilation with GCC
```bash
gcc -o dbc_load_tool main.c canbus.c pharse.c package.c BO_Index.c \
    dbc_timer.c GetTime.c MySql.c PyCall.c menu.c \
    -lpthread -lmysqlclient -lpython3.x
```

## 📖 Usage

### Basic Usage
```bash
./dbc_load_tool <dbc_file>
```

### Example
```bash
./dbc_load_tool Huanghonglei.dbc
```

### Operating Modes
After starting the program, a menu will be displayed with the following options:

1. **Canbus MySql** - CAN data reception and storage to MySQL database
2. **Fork** - Dual-interface loopback test mode for can0 and can1  
3. **Test Mode** - Timed transmission test mode
4. **CSV + Python** - CSV recording mode with Python script integration
5. **Test** - Python integration functionality testing

### DBC File Format
Supports standard DBC file format including:
- `BO_` message definitions
- `SG_` signal definitions  
- Signal properties (start bit, length, byte order, sign, factor, offset, etc.)

## 📁 Project Structure

```
LinuxCanbusTool/
├── main.c              # Main program entry point
├── canbus.c/.h         # CAN bus interface wrapper
├── pharse.c/.h         # DBC parsing functionality
├── package.c/.h        # CAN message packaging functionality
├── BO.h                # CAN message and signal data structure definitions
├── BO_Index.c/.h       # DBC file loading and indexing
├── dbc_timer.c/.h      # Timed transmission functionality
├── MySql.c/.h          # MySQL database interface
├── PyCall.c/.h         # Python integration interface
├── GetTime.c/.h        # Time statistics utility
├── menu.c/.h           # User interface menu
├── *.dbc               # DBC file examples
└── bin/Debug/          # Compilation output directory
```

## 🔧 Configuration

### CAN Interface Setup
Ensure the CAN interface is properly configured:
```bash
# Setup CAN interface
sudo ip link set can0 type can bitrate 500000
sudo ip link set up can0
```

### MySQL Configuration
If using database functionality, ensure MySQL service is running and configure the appropriate database connection parameters.

### Python Environment
If using Python integration features, ensure Python development environment is installed.

## 📊 Performance

- **Real-time Performance** - Supports real-time processing of high-frequency CAN messages
- **Multi-threading** - Multi-threaded architecture with separated transmission/reception and display
- **Memory Management** - Dynamic memory allocation supporting large DBC files
- **Time Statistics** - Built-in performance monitoring with processing time statistics

## 🤝 Contributing

Issues and Pull Requests are welcome to improve the project.

## 📄 License

Please check the LICENSE file in the project root directory for license information.

## 👨‍💻 Author

- **Huang Honglei** - Intern Project 2018/4/23

## 📞 Support

For questions or suggestions, please contact via:
- Submit GitHub Issues
- Email project maintainers

---

*Note: This tool is primarily designed for automotive electronics CAN bus development and testing, suitable for vehicle ECU communication debugging, CAN protocol analysis, and similar scenarios.*
