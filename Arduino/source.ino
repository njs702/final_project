#include <dht.h>

dht DHT;

/* PIN NUMBER settings */
int DHT_11_PIN = 3;

/* struct data stores temp & humid info */
typedef struct{
    float humid; 
    float temp;
}temp_humid_data;

static temp_humid_data temp_humid;

/* read temperature & humid from DHT_11 module */
void read_temp_humid(){
    DHT.read11(DHT_11_PIN);
    temp_humid.humid = DHT.humidity;
    temp_humid.temp = DHT.temperature;
}

void setup()
{
	Serial.begin(115200);
}

void loop()
{
	read_temp_humid();
}
