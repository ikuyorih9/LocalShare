#ifndef I2C_HPP
#define I2C_HPP

#define I2C_ADDRESS 0x27
#define I2C_SDA 21
#define I2C_SCL 22

void onRequest();
void onReceive(int numBytes);

#endif