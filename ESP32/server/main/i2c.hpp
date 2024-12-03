#ifndef I2C_HPP
#define I2C_HPP

#define I2C_ADDRESS 0x27

void onRequest();
void onReceive(int numBytes);

#endif
