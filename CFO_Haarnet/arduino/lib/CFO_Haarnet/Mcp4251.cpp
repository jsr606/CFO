
// MCP4251 2-channel Digital Potentiometer
// ww1.microchip.com/downloads/en/DeviceDoc/22059b.pdf

// The default SPI Control Register - SPCR = B01010000;
// interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
// sample on leading edge of clk,system clock/4 rate (fastest).
// Enable the digital pins 11-13 for SPI (the MOSI,MISO,SPICLK)
#include <Mcp4251.h>

//---------- constructor ----------------------------------------------------

MCP4251::MCP4251(uint8_t slave_select_pin, float rAB_ohms)
{
  setup_ss(slave_select_pin);
	setup_resistance(rAB_ohms, 0.0); //rW_ohms_typical);
}

MCP4251::MCP4251(uint8_t slave_select_pin, float rAB_ohms, float rW_ohms)
{
  setup_ss(slave_select_pin);
  setup_resistance(rAB_ohms, rW_ohms);
}

//------------------ protected -----------------------------------------------

uint16_t MCP4251::byte2uint16(byte high_byte, byte low_byte)
{
  return (uint16_t)high_byte<<8 | (uint16_t)low_byte;
}

byte MCP4251::uint16_high_byte(uint16_t uint16)
{
  return (byte)(uint16>>8);
}

byte MCP4251::uint16_low_byte(uint16_t uint16)
{
  return (byte)(uint16 & 0x00FF);
}

void MCP4251::setup_ss(uint8_t slave_select_pin)
{
  // Set slave select (Chip Select) pin for SPI Bus, and start high (disabled)
  ::pinMode(slave_select_pin,OUTPUT);
  ::digitalWrite(slave_select_pin,HIGH);
  this->slave_select_pin = slave_select_pin;
}

void MCP4251::setup_resistance(float rAB_ohms, float rW_ohms)
{
  this->rAB_ohms             = rAB_ohms;
  this->rW_ohms              = rW_ohms;
  this->rAW_ohms_max         = rAB_ohms - rW_ohms;
  this->scale                = rAW_ohms_max;
}

float MCP4251::step_increment()
{
  return (rAW_ohms_max - rW_ohms) / resolution;
}

unsigned int MCP4251::ohms2wiper_pos(float ohms)
{
  if(ohms <= 0.0)
    return 0;
  else if(scale != rAW_ohms_max)
    ohms = ohms * rAW_ohms_max / scale;

  return (unsigned int)((ohms - rW_ohms) / step_increment() ) + 0.5;
}

float MCP4251::wiper_pos2ohms(unsigned int wiper_pos)
{
  float ohms =  rW_ohms + ( (float)wiper_pos * step_increment() );

  if(scale != rAW_ohms_max)
    ohms = ohms * scale / rAW_ohms_max;
  
  return ohms;
}

void MCP4251::write(byte cmd_byte, byte data_byte)
{
  cmd_byte |= kCMD_WRITE;
  ::digitalWriteFast(slave_select_pin, LOW);
//	byte high_byte = spi4teensy3::send(cmd_byte);
//	byte low_byte  = spi4teensy3::send(data_byte);
	spi4teensy3::send(cmd_byte);
	spi4teensy3::send(data_byte);
  ::digitalWriteFast(slave_select_pin, HIGH);
  //bool result = ~low_byte;
}

uint16_t MCP4251::read(byte cmd_byte)
{
  cmd_byte |= kCMD_READ;
  ::digitalWrite(slave_select_pin, LOW);
//	byte high_byte = spi4teensy3::receive(cmd_byte);
//	byte low_byte  = spi4teensy3::receive(0xFF);
	spi4teensy3::send(cmd_byte);
	spi4teensy3::send(0xFF);
  ::digitalWrite(slave_select_pin, HIGH);
//  return byte2uint16(high_byte, low_byte);
}

void MCP4251::wiper_pos(byte pot, unsigned int wiper_pos)
{
  byte cmd_byte    = 0x00;
  byte data_byte   = 0x00;
  cmd_byte        |= pot;

  // Calculate the 9-bit data value to send
  if(wiper_pos > 255)
    cmd_byte      |= B00000001; // Table 5-1 (page 36)
  else
    data_byte      = (byte)(wiper_pos & 0x00FF);

  write(cmd_byte|kADR_VOLATILE, data_byte);

  if(non_volatile)
  {
    // EEPROM write cycles take 4ms each. So we block with delay(5); after any NV Writes
    write(cmd_byte|kADR_NON_VOLATILE, data_byte);
    delay(5);
  }
}

//---------- public ----------------------------------------------------

float MCP4251::wiper0()
{
  return wiper_pos2ohms( wiper0_pos() );
}

float MCP4251::wiper1()
{
  return wiper_pos2ohms( wiper1_pos() );
}

unsigned int MCP4251::wiper0_pos()
{
  return (unsigned int)( 0x01FF & this->read(kADR_WIPER0|kADR_VOLATILE) );
}

unsigned int MCP4251::wiper1_pos()
{
  return 0x01FF & this->read(kADR_WIPER1|kADR_VOLATILE);
}

void MCP4251::wiper0(float ohms)
{
  wiper0_pos( ohms2wiper_pos(ohms) );
}

void MCP4251::wiper1(float ohms)
{
  wiper1_pos( ohms2wiper_pos(ohms) );
}

void MCP4251::wiper0_pos(unsigned int wiper_pos)
{
  this->wiper_pos(kADR_WIPER0, wiper_pos);
}

void MCP4251::wiper1_pos(unsigned int wiper_pos)
{
  this->wiper_pos(kADR_WIPER1, wiper_pos);
}


// // Not implemented
// bool MCP4251::pot0_connected(bool terminal_a, bool wiper, bool terminal_b)
// {
//   
// }
// 
// bool MCP4251::pot1_connected(bool terminal_a, bool wiper, bool terminal_b)
// {
//   
// }
// 
// void MCP4251::pot0_connect(bool terminal_a, bool wiper, bool terminal_b)
// {
//   
// }
// 
// void MCP4251::pot1_connect(bool terminal_a, bool wiper, bool terminal_b)
// {
//   
// }
// 
// bool MCP4251::pot0_shutdown()
// {
//   
// }
// 
// bool MCP4251::pot1_shutdown()
// {
//   
// }
// 
// void MCP4251::pot0_shutdown(bool shutdown)
// {
//   
// }
// 
// void MCP4251::pot1_shutdown(bool shutdown)
// {
//   
// }
// 
// bool MCP4251::hw_shutdown()
// {
//   
// }


