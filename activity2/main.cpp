#include "mbed.h"
#include "stdint.h" //This allow the use of integers of a known width
#define LM75_REG_TEMP (0x00) // Temperature Register
#define LM75_REG_CONF (0x01) // Configuration Register
#define LM75_ADDR     (0x90) // LM75 address

#define LM75_REG_TOS (0x03) // TOS Register
#define LM75_REG_THYST (0x02) // THYST Register



I2C i2c(I2C_SDA, I2C_SCL);

DigitalOut myled(LED1);
DigitalOut blue(LED2);
DigitalOut red(LED3);


InterruptIn lm75_int(D7); // Make sure you have the OS line connected to D7

Serial pc(SERIAL_TX, SERIAL_RX);

int16_t i16; // This variable needs to be 16 bits wide for the TOS and THYST conversion to work - can you see why

float* temps = new float[60];
int ptr = 0;
bool rpt = false;

void alarm(int count, bool forever)
{
    if(forever){
        while(true){
            blue=!blue;
            red=!red;
            wait_us(500000);
        }
    } else{
        for(int i = 0; i<count; i++)
        {
            blue=!blue;
            red=!red;
            wait_us(500000);
        }
    }
}
void blue_flip()
{       

        alarm(5, false);
        pc.printf("Interrupt triggered \r\n");
        if(rpt){
            for(int i=ptr; i<60; i++){
                pc.printf("%.3f\r\n", temps[i]);
            }
            for(int i=0; i<ptr; i++){
                pc.printf("%.3f\r\n", temps[i]);
            }
        } else {
            for(int i = 0; i<ptr; i++){
                pc.printf("%.3f\r\n", temps[i]); 
            }
        }
        alarm(0, true);
}
void fill(float t)
{
        if(ptr<60){
            temps[ptr] = t;
        } else {
            ptr = 0;
            rpt = true;
            temps[ptr] = t;
        }
        ptr++;
}

int main()
{
        char data_write[3];
        char data_read[3];

        /* Configure the Temperature sensor device STLM75:
           - Thermostat mode Interrupt
           - Fault tolerance: 0
           - Interrupt mode means that the line will trigger when you exceed TOS and stay triggered until a register is read - see data sheet
        */
        data_write[0] = LM75_REG_CONF;
        data_write[1] = 0x02;
        int status = i2c.write(LM75_ADDR, data_write, 2, 0);
        if (status != 0)
        { // Error
                while (1)
                {
                    myled = !myled;
                    wait(0.2);
                }
        }

        float tos=28; // TOS temperature
        float thyst=26; // THYST tempertuare

        // This section of code sets the TOS register
        data_write[0]=LM75_REG_TOS;
        i16 = (int16_t)(tos*256) & 0xFF80;
        data_write[1]=(i16 >> 8) & 0xff;
        data_write[2]=i16 & 0xff;
        i2c.write(LM75_ADDR, data_write, 3, 0);

        //This section of codes set the THYST register
        data_write[0]=LM75_REG_THYST;
        i16 = (int16_t)(thyst*256) & 0xFF80;
        data_write[1]=(i16 >> 8) & 0xff;
        data_write[2]=i16 & 0xff;
        i2c.write(LM75_ADDR, data_write, 3, 0);

        // This line attaches the interrupt.
        // The interrupt line is active low so we trigger on a falling edge
        lm75_int.fall(&blue_flip);
        float temp = 0;
        while(temp<tos)
        {
                // Read temperature register
                data_write[0] = LM75_REG_TEMP;
                i2c.write(LM75_ADDR, data_write, 1, 1); // no stop
                i2c.read(LM75_ADDR, data_read, 2, 0);

                // Calculate temperature value in Celcius
                int16_t i16 = (data_read[0] << 8) | data_read[1];
                // Read data as twos complement integer so sign is correct
                temp = i16 / 256.0;
                pc.printf("Temperature = %.3f\r\n",temp);
                fill(temp);

                myled = !myled;
                wait(1.0);
        }
        blue_flip();

        delete[] temps;

}