#include "DHT.h"
#include "Wire.h"
#include "BMP085.h"

// include files for mysql
#include "SPI.h"
#include "Ethernet.h"
#include "sha1.h"
#include "mysql.h"

/* Setup for Ethernet Library */
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(192,168,1,1);
IPAddress ip(192,168,1,177);
IPAddress gateway(192,168,1, 1);
IPAddress subnet(255, 255, 255, 0);
char sqlbuf[100];  //128
char sqlDbase[] = " USE dcmsDB";
char pStr[8];     //10
char htStr[6];    //10
char tmpStr[6];   //10
/*Setup for MySQL  */
unsigned int mysqlPort=3306;
Connector my_conn; // The Connector/Arduino reference
char user[] = "root2";
char password[] = "root2";
boolean sqlconnect= false;
float  ht = 84.50;
float  tmp = 26.78;
long  p = 101325.0;


// Nastaveni cidla tlaku
BMP085 dps = BMP085();

//Setup for DHT
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(115200);//Set Baud Rate to 9600 bps
    Ethernet.begin(mac_addr, ip, gateway, subnet);
    Wire.begin();
    delay(1000);
    mysqlBegin();
    delay(1000);
    dps.init(MODE_STANDARD, 31800, true);
}
 
// Cidlo tlaku
float pressure()
{
    dps.getPressure(&p);
//    Serial.print("  Tlak(Pa):");
//    Serial.print(p);
  return p; 
}


// Cidlo vlhkosti
float humidity()
{
    float h = dht.readHumidity();
//    Serial.print("Humidity: "); 
//    Serial.println(h);
  return h;
}

// Cidlo teploty
float temperature()
{
    float t = dht.readTemperature();
//    Serial.print("Temperature: "); 
//    Serial.println(t);
  return t; 
}

// Start MySQL
void mysqlBegin()
{
 
  delay(350);
    Serial.println("connecting MySQL");
    
     if (my_conn.mysql_connect(server_addr, mysqlPort, user, password))
      {
       sqlconnect=true;
       Serial.println("Success!");
        delay(150);
        my_conn.cmd_query(sqlDbase);  
       }
     else
      {
       Serial.println("Failed.");
      }
   
}
// Zaslani dat do MySQL
void mysqldata()
{
  // uncomment to use the given functions
  tmp = temperature();
  ht  = humidity();
  p   = pressure();

  
if (sqlconnect==true)
  {
   delay(150);
   dtostrf(ht,1,1,htStr);
   dtostrf(tmp,1,1,tmpStr);
   dtostrf(p,1,0,pStr);
   sprintf(sqlbuf, "INSERT INTO dcmsData (Humidity, Pressure, Temperature, DayTime) values ('%s', '%s', '%s', NOW())", htStr, pStr, tmpStr) ;
   my_conn.cmd_query(sqlbuf);
   Serial.println("Stored!");
   }

} // end mysql...!


//-----Main program
void loop()
{
    
    mysqldata();//
    delay(5000);
}
