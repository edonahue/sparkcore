//Much thanks to user apassemard and others on spark.io forum thread: https://community.spark.io/t/interface-spark-core-with-xively/475/5

#define FEED_ID "1746473078"
#define XIVELY_API_KEY ""

TCPClient client;


int lightPin = 0;  //define a pin for Photo resistor
int ledPin=11;     //define a pin for LED
double light = 0.0;
double total_light = 0.0;
int ledD = D7;

int count = 0;
unsigned long LastUpTime = 0;
unsigned long LastCloudCheck = 0;
char whichApp[64] = "Read Light with XIVELY";


void setup() {
    Serial.begin(9600);  //Begin serial communcation
    Spark.variable("light", &light, DOUBLE);
    pinMode( ledPin, OUTPUT );
	Spark.variable("whichapp", &whichApp, STRING);	
}

void loop() {
  int reading = 0;
  double voltage = 0.0;
  // Keep reading the sensor value so when we make an API
  // call to read its value, we have the latest one
  reading = analogRead(lightPin);
  // The returned value from the Core is going to be in the range from 0 to 4095
  // Calculate the voltage from the sensor reading
  voltage = (reading * 3.3) / 4095;
  // Calculate the temperature and update our static variable
  light = (voltage - 0.5) * 100;  
       if (millis()-LastUpTime>2000)
       {
          if (count <= 5) {
              total_light += light; 
              count++;
          }
          else {          
                  xivelyTemp(total_light/count); //Send the average of the last 5 readings
                  count = 0;
                  total_light = 0;
          }
          LastUpTime = millis();
       }

       if (millis()-LastCloudCheck > 1000*60*2) { //check every 5 min to see if the connection still exists
           if(!Spark.connected()) Spark.connect();
           LastCloudCheck = millis();
       }
  
}


void xivelyTemp(double lux) {

  ledStatus(5, 100); 
   //Serial.println("Connecting to server...");
    if (client.connect("api.xively.com", 8081)) 
    {
        // Connection succesful, update datastreams
        client.print("{");
        client.print("  \"method\" : \"put\",");
        client.print("  \"resource\" : \"/feeds/");
        client.print(FEED_ID);
        client.print("\",");
        client.print("  \"params\" : {},");
        client.print("  \"headers\" : {\"X-ApiKey\":\"");
        client.print(XIVELY_API_KEY);
        client.print("\"},");
        client.print("  \"body\" :");
        client.print("    {");
        client.print("      \"version\" : \"1.0.0\",");
        client.print("      \"datastreams\" : [");
        client.print("        {");
        client.print("          \"id\" : \"light\",");
        client.print("          \"current_value\" : \"");
        client.print(lux); //adjustment for some weird reason..
        client.print("\"");
        client.print("        }");
        client.print("      ]");
        client.print("    },");
        client.print("  \"token\" : \"0x123abc\"");
        client.print("}");
        client.println();

        ledStatus(3, 1000);        
    } 
    else 
    {
        // Connection failed
        //Serial.println("connection failed");
        ledStatus(3, 2000);
    }
    client.flush();
    client.stop();
}

void ledStatus(int x, int t)
{
    for (int j = 0; j <= x-1; j++)
    {
        digitalWrite(ledD, HIGH);
        delay(t);
        digitalWrite(ledD, LOW);
        delay(t); 
   }
}
