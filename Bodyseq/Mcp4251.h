
// MCP4251 2-channel Digital Potentiometer
// ww1.microchip.com/downloads/en/DeviceDoc/22059b.pdf

#include <Arduino.h>
#include <spi4teensy3.h>

#ifndef Mcp4251_h
#define Mcp4251_h

class MCP4251
{
  public:
    // You must at least specify the slave select pin and the rated resistance
    MCP4251(uint8_t slave_select, float rAB_ohms);

    // If you have measured wiper resistance, rW
    MCP4251(uint8_t slave_select, float rAB_ohms, float rW_ohms);

    // The resistance scaling, defaults to rAB_ohms
    float scale;

    // Read potentiometer values
    float wiper0();
    float wiper1();
    unsigned int wiper0_pos();
    unsigned int wiper1_pos();

    // Write potentiometer values
    void wiper0(float ohms);
    void wiper1(float ohms);

    void wiper0_pos(unsigned int wiper_pos);
    void wiper1_pos(unsigned int wiper_pos);

    // // Not implemented
    // // Connect / disconnect potentiometers
    // bool pot0_connected(bool terminal_a, bool wiper, bool terminal_b);
    // bool pot1_connected(bool terminal_a, bool wiper, bool terminal_b);
    // void pot0_connect(bool terminal_a, bool wiper, bool terminal_b);
    // void pot1_connect(bool terminal_a, bool wiper, bool terminal_b);
    // 
    // bool pot0_shutdown();
    // bool pot1_shutdown();
    // void pot0_shutdown(bool shutdown);
    // void pot1_shutdown(bool shutdown);
    // 
    // bool hw_shutdown();

  protected:
//    const static float rW_ohms_typical = 117.50f;
    const static unsigned int resolution_7bit = 128;
    const static unsigned int resolution_8bit = 256;

    // Other devices can be configured below vv as per the device numbering scheme:
    // MCP4N-- N=1 single pot, N=2 dual pot
    // MCP4--N N=1 potentiometer, N=2 rheostat
    // MCP4-N- N=3 7-bit volatile, N=4 7-bit non-volatile, N=5 8-bit volatile, N=6 8-bit non-volatile
    const static bool         non_volatile    = false;
    const static unsigned int resolution      = resolution_8bit;

    float rW_ohms;
    float rAB_ohms;
    float rAW_ohms_max;

    uint8_t slave_select_pin;

    const static uint8_t kADR_WIPER0       = B00000000;
    const static uint8_t kADR_WIPER1       = B00010000;

    const static uint8_t kCMD_READ         = B00001100;
    const static uint8_t kCMD_WRITE        = B00000000;

    const static uint8_t kADR_VOLATILE     = B00000000;
    const static uint8_t kADR_NON_VOLATILE = B00100000;

    const static uint8_t kTCON_REGISTER    = B01000000;
    const static uint8_t kSTATUS_REGISTER  = B01010000;

    uint16_t byte2uint16(byte high_byte, byte low_byte);
    byte uint16_high_byte(uint16_t uint16);
    byte uint16_low_byte(uint16_t uint16);

    void setup_ss(uint8_t slave_select_pin);
    void setup_resistance(float rAB_ohms, float rW_ohms);

    float step_increment();
    unsigned int ohms2wiper_pos(float ohms);
    float wiper_pos2ohms(unsigned int wiper_pos);

    uint16_t read(byte cmd_byte);
    void write(byte cmd_byte, byte data_byte);
    void wiper_pos(byte pot, unsigned int wiper_pos);
};

#endif // Mcp4251_h



