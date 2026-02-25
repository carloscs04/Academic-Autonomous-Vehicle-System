#include <Bluepad32.h>
#include <ESP32Servo.h>
#include <HardwareSerial.h>
#include "UbidotsEsp32Mqtt.h"
#include <WiFi.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// Pines del ESP32
const int ServoBB = 12;
const int trigPin = 14;
const int echoPin = 27;
const int Forward = 26;
const int Backward = 25;
const int PWM = 33;
#define RXD2 16   // TX (A2) STM32 
#define TXD2 17   // RX (A3) STM32

// Valores Enteros 
int val = 0;
int i = 0;
int i2 = 0;

int brake = 0; 
int throttle;
int estado = 0;
int estadotwo = 0;

// Valores Booleanos
bool autom = false;
bool flagVlc = false;
bool flagSU = false;
bool flagCNE = true;
bool flagPOS = true;
bool flagRUT = false;
bool flagWIFI = false;
bool flagCONB = true;
bool FlagArranqueAuto = false;
bool saltoCorregido = false;
bool flagEspera = false;

// Valores flotantes
long duration;
float error = 0;
float kp = 1;
float referencia = 0;
float correccionP;
float desplazamiento = 0;
float distanceCm;
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

// Arries
float XYZMag[3] = {0.0, 0.0, 0.0};
float Ndis[2] = {};
float Ndeg[2] = {};

// Usa UART2 (pines RX2 = GPIO16, TX2 = GPIO17)
HardwareSerial mySerial(2);
String buffer= "";

// Servomotor
Servo myservo;

// Encoder
const int    C1 = 19; // Entrada de la señal A del encoder.
const int    C2 = 18; // Entrada de la señal B del encoder.

volatile int  n    = 0;
volatile byte ant  = 0;
volatile byte act  = 0;

const float perimetro = 2 * 3.1416 * 0.05; // Reemplaza 'r' por el radio de tu rueda.
const int pasosPorVuelta = 170; // Valor real del encoder.

/* Definimos primero el Token que nos brinda la plataforma Ubidots para hacer la conexión */
const char *UBIDOTS_TOKEN = "BBUS-9UQDRV8xQk2fHnct6vFQN77AuQspzB";
/* Definimos SSID y PASSWORD de nuestra red WiFi */
const char *WIFI_SSID = "Carlos";      
const char *WIFI_PASS = "12345678";     
/* Definimos el nombre de nuestro dispositivo, el cual aparecerá en la plataforma Ubidots */
const char *DEVICE_LABEL = "esp32-conexion";
/* Definimos la variable que se medirá y será publicada en la plataforma Ubidots */
const char *VARIABLE_LABEL_MAG = "esp32-conexion-mag"; 
const char *VARIABLE_LABEL_ENC = "esp32-conexion-enc";
const char *VARIABLE_LABEL_DIS = "esp32-conexion-dis";
/* Definimos la frecuencia de publicación de 5 segundos */
const int PUBLISH_FREQUENCY = 1; 
/* Definimos unas variables extra, que incluye la librería */
unsigned long timer;
uint8_t analogPin = 34; // Pin analógico donde está conectado el potenciómetro
/* Definimos que las variables de la librería UBIDOTS trabajarán con el Token */
Ubidots ubidots(UBIDOTS_TOKEN);

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}

void dumpMouse(ControllerPtr ctl) {
    Serial.printf("idx=%d, buttons: 0x%04x, scrollWheel=0x%04x, delta X: %4d, delta Y: %4d\n",
                   ctl->index(),        // Controller Index
                   ctl->buttons(),      // bitmask of pressed buttons
                   ctl->scrollWheel(),  // Scroll Wheel
                   ctl->deltaX(),       // (-511 - 512) left X Axis
                   ctl->deltaY()        // (-511 - 512) left Y axis
    );
}

void dumpKeyboard(ControllerPtr ctl) {
    static const char* key_names[] = {
        // clang-format off
        // To avoid having too much noise in this file, only a few keys are mapped to strings.
        // Starts with "A", which is offset 4.
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
        "W", "X", "Y", "Z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        // Special keys
        "Enter", "Escape", "Backspace", "Tab", "Spacebar", "Underscore", "Equal", "OpenBracket", "CloseBracket",
        "Backslash", "Tilde", "SemiColon", "Quote", "GraveAccent", "Comma", "Dot", "Slash", "CapsLock",
        // Function keys
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
        // Cursors and others
        "PrintScreen", "ScrollLock", "Pause", "Insert", "Home", "PageUp", "Delete", "End", "PageDown",
        "RightArrow", "LeftArrow", "DownArrow", "UpArrow",
        // clang-format on
    };
    static const char* modifier_names[] = {
        // clang-format off
        // From 0xe0 to 0xe7
        "Left Control", "Left Shift", "Left Alt", "Left Meta",
        "Right Control", "Right Shift", "Right Alt", "Right Meta",
        // clang-format on
    };
    Serial.printf("idx=%d, Pressed keys: ", ctl->index());
    for (int key = Keyboard_A; key <= Keyboard_UpArrow; key++) {
        if (ctl->isKeyPressed(static_cast<KeyboardKey>(key))) {
            const char* keyName = key_names[key-4];
            Serial.printf("%s,", keyName);
       }
    }
    for (int key = Keyboard_LeftControl; key <= Keyboard_RightMeta; key++) {
        if (ctl->isKeyPressed(static_cast<KeyboardKey>(key))) {
            const char* keyName = modifier_names[key-0xe0];
            Serial.printf("%s,", keyName);
        }
    }
    Console.printf("\n");
}

void dumpBalanceBoard(ControllerPtr ctl) {
    Serial.printf("idx=%d,  TL=%u, TR=%u, BL=%u, BR=%u, temperature=%d\n",
                   ctl->index(),        // Controller Index
                   ctl->topLeft(),      // top-left scale
                   ctl->topRight(),     // top-right scale
                   ctl->bottomLeft(),   // bottom-left scale
                   ctl->bottomRight(),  // bottom-right scale
                   ctl->temperature()   // temperature: used to adjust the scale value's precision
    );
}

void processGamepad(ControllerPtr ctl) {
    
    // Medicion de Variables
    val = map(ctl->axisX(),512,-511, 33, 132);
    throttle = map(ctl->throttle(), 0, 1023, 0, 255);
    brake = map(ctl->brake(), 0, 1023, 0, 255);

    // Declaración de estados
    
    if (ctl->b()) {
        autom = !autom;
        if (autom == true){
          Serial.println("Automatico");
          delay(250);
        }
        if (autom == false){
          Serial.println("Control");
          delay(250);
        }
    }

    if (ctl->a()) {
        FlagArranqueAuto = !FlagArranqueAuto;
        if (FlagArranqueAuto == true){
          Serial.println("ArranqueAutoOn");
          delay(250);
        }
        if (FlagArranqueAuto == false){
          Serial.println("FlagArranqueAutoOff");
          delay(250);
        }
    }

    if (ctl->x()) {
        flagWIFI = !flagWIFI;
        if (flagWIFI == true){
          Serial.println("DatosON");
          delay(250);
        }
        if (flagWIFI == false){
          Serial.println("DatosOFF");
          delay(250);
        }
    }
}

void processMouse(ControllerPtr ctl) {
    // This is just an example.
    if (ctl->scrollWheel() > 0) {
        // Do Something
    } else if (ctl->scrollWheel() < 0) {
        // Do something else
    }

    // See "dumpMouse" for possible things to query.
    dumpMouse(ctl);
}

void processKeyboard(ControllerPtr ctl) {
    if (!ctl->isAnyKeyPressed())
        return;

    // This is just an example.
    if (ctl->isKeyPressed(Keyboard_A)) {
        // Do Something
        Serial.println("Key 'A' pressed");
    }

    // Don't do "else" here.
    // Multiple keys can be pressed at the same time.
    if (ctl->isKeyPressed(Keyboard_LeftShift)) {
        // Do something else
        Serial.println("Key 'LEFT SHIFT' pressed");
    }

    // Don't do "else" here.
    // Multiple keys can be pressed at the same time.
    if (ctl->isKeyPressed(Keyboard_LeftArrow)) {
        // Do something else
        Serial.println("Key 'Left Arrow' pressed");
    }

    // See "dumpKeyboard" for possible things to query.
    dumpKeyboard(ctl);
}

void processBalanceBoard(ControllerPtr ctl) {
    // This is just an example.
    if (ctl->topLeft() > 10000) {
        // Do Something
    }

    // See "dumpBalanceBoard" for possible things to query.
    dumpBalanceBoard(ctl);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else if (myController->isMouse()) {
                processMouse(myController);
            } else if (myController->isKeyboard()) {
                processKeyboard(myController);
            } else if (myController->isBalanceBoard()) {
                processBalanceBoard(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // "forgetBluetoothKeys()" should be called when the user performs
    // a "device factory reset", or similar.
    // Calling "forgetBluetoothKeys" in setup() just as an example.
    // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
    // But it might also fix some connection / re-connection issues.
    BP32.forgetBluetoothKeys();

    // Enables mouse / touchpad support for gamepads that support them.
    // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
    // - First one: the gamepad
    // - Second one, which is a "virtual device", is a mouse.
    // By default, it is disabled.
    BP32.enableVirtualDevice(false);

    /* Ahora, se incluyen las funciones de conexión de la Plataforma, como la conexión a internet con las credenciales de WiFi */
    // ubidots.setDebug(true);  // Descomentar esto para que los mensajes de depuración estén disponibles
    //ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
    /* Se incluye las funciones de Setup y Reconnect, predefinidas de la librería */
    //ubidots.setup();
    //ubidots.reconnect();
    timer = millis();

    // Declaración de pines
    pinMode(Forward,OUTPUT);
    pinMode(Backward,OUTPUT);
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    ledcAttachPin(PWM, 2);
    ledcSetup(2,8000,8);
    
    // Declaración del servomotor
    myservo.attach(ServoBB);

    // Encoder
    attachInterrupt(digitalPinToInterrupt(C1), encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(C2), encoder, CHANGE);
    
    // Comunicación UART
    mySerial.begin(115200, SERIAL_8N1, RXD2, TXD2);

}

// Arduino loop function. Runs in CPU 1.
void loop() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.
  bool dataUpdated = BP32.update();
  if (dataUpdated)
      processControllers();

  // The main loop must have some kind of "yield to lower priority task" event.
  // Otherwise, the watchdog will get triggered.
  // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
  // Detailed info here:
  // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

  // vTaskDelay(1);

  /* Definimos que, si no se conecta a la plataforma Ubidots, se pasa la función Reconnect() para volver a establecer la conexión */
  if (flagWIFI == true){
    if (flagCONB == true){
      Serial.println("Conectado");
      flagCONB = false;
      ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
      ubidots.setup();
      if (!ubidots.connected()) {
        ubidots.reconnect();
      }
    }
  } else {
    if (flagCONB == false){
      Serial.println("Desconectado");
      flagCONB = true;
      WiFi.disconnect();
    }
  }
  if (autom == true){
    Automatico();
  }

  if (autom == false){
    Control();
  }

  delay(1); 
  ubidots.loop();
}

void Control(){
  // Reinicio de variables
  flagSU = false;
  flagCNE = true;
  flagPOS = true;
  flagRUT = false;
  flagEspera = false;
  estado = 0;
  estadotwo = 0;
  i = 0;
  i2 = 0;

  encoder();
  desplazamiento = (float(n) / pasosPorVuelta) * perimetro;
  Serial.print("desplazamiento :");
  Serial.print(desplazamiento);

  Serial.print(", n :");
  Serial.println(n);
  
  if(WiFi.status() == WL_CONNECTED){
    Sending();
  }
  
  myservo.write(val);
  
  if (throttle > 0  && brake > 0){
    flagVlc = true;
    ledcWrite(2, throttle);
  }
  if (throttle > 0  != brake > 0){
    flagVlc = false;
    ledcWrite(2, brake);
  }
  if (brake == 0 || flagVlc == true){ 
    digitalWrite(Backward,LOW);
    ledcWrite(2, brake);
  }
  if (throttle == 0 || flagVlc == true){
    digitalWrite(Forward,LOW);
    ledcWrite(2, brake);
  }
  if (throttle > 0 && flagVlc == false){
    digitalWrite(Backward,LOW);
    digitalWrite(Forward,HIGH);
    if (throttle > 0 && throttle < 175){
      throttle = 175;
    }
    ledcWrite(2, throttle);
  }
  if (brake > 0 && flagVlc == false){
    digitalWrite(Forward,LOW);
    digitalWrite(Backward,HIGH);
    if (brake > 0 && brake < 175){
      brake = 175;
    }
    ledcWrite(2, brake);
  }
  delay(100);
}

void Automatico(){
  if(WiFi.status() == WL_CONNECTED){
    Sending();
  }

  Ndis[1] = Ndis[0];
  Ndis[0] = Ultrasonic();
  if (Ndis[0] <= 10){
    Ndis[0] = Ndis[1];
  }
  Ndeg[1] = Ndeg[0];
  Ndeg[0] = Magnetometro(XYZMag);
  if (Ndeg[0] == 0){
    Ndeg[0] = Ndeg[1];
  }
  


  if (FlagArranqueAuto == true && flagCNE == true  && flagEspera == false){
    digitalWrite(Forward,HIGH);
    ledcWrite(2, 180);
  } 
    
  if((FlagArranqueAuto != true) || flagEspera == true){
    digitalWrite(Forward,LOW);
    ledcWrite(2, 0);
    estado = 0; 
    i = 0;
    flagSU = false;
    flagCNE = true;
  }

  if (estadotwo == 0){
    Nort();
  }
  if (estadotwo == 1){
    East();
  }
  if (estadotwo == 2){
    South();
  }
  if (estadotwo == 3){
    West();
  } 

  if ((Ndis[0] <= 30 || flagSU == true) && FlagArranqueAuto == true){
    flagSU = true;
    Secuencia();
  }

  if (flagRUT == true  && flagCNE == true){
    Espera();
  }
  Serial.print("Grados: ");
  Serial.print(Ndeg[0]);
  Serial.print(", distancia, ");
  Serial.print(Ndis[0]);
  Serial.print("estadotwo: ");
  Serial.println(estadotwo);
  delay(49);
}

void Espera(){
  i2++;
  Serial.println("Procesando");
  Serial.println(i2);
  if (i2 >= 80){
    flagRUT = false;
    flagPOS = true;
    flagEspera = false;
    estadotwo++;
    i2 = 0;
    Serial.println("Llegada");
    referencia = referencia + 90;
  }
  if (referencia >= 360){
    referencia = 0;
  }
  if (estadotwo > 3){
    estadotwo = 0;
  };
}

void Nort(){
  Serial.println("Norte, ");

  if (Ndeg[0] > 180 &&  Ndeg[0] <= 360){
    error = referencia - (360 - Ndeg[0]) ; 
    correccionP = error*kp + 88;
    if (correccionP <= 33){
      correccionP = 33;
    }

  }

  if (Ndeg[0] > 0 &&  Ndeg[0] <= 180){
    error = referencia - Ndeg[0]; 
    correccionP = -error*kp + 88;
    if (correccionP >= 132){
      correccionP = 132;
    }
  }

  if (correccionP > 80 && correccionP < 100){
    if (flagPOS == true){
      flagPOS = false;
      n = 0;
    }
    encoder();
    desplazamiento = (float(n) / pasosPorVuelta) * perimetro;
    Serial.println("desplazamiento :");
    Serial.println(desplazamiento);
    if (desplazamiento >= 3.5){
      flagRUT = true;
      flagEspera = true;
    }  
  } 

  myservo.write(correccionP);
  Serial.println(correccionP);
}

void East(){
  Serial.println("Este");
  
  if ((Ndeg[0] > 270 &&  Ndeg[0] <= 360) || (Ndeg[0] >= 0 &&  Ndeg[0] < 90)){
    if (Ndeg[0] > 270 &&  Ndeg[0] <= 360){
      error = Ndeg[0] -360 - 90;
      correccionP = error*kp + 88;
      if (correccionP <= 33){
      correccionP = 33;
      }
    }
    if (Ndeg[0] >= 0 &&  Ndeg[0] <= 90){
      error = Ndeg[0] - 90;
      correccionP = error*kp + 88;
      if (correccionP <= 33){
      correccionP = 33;
      }
    }

    if (Ndeg[0] > 90 &&  Ndeg[0] <= 270){
    error = Ndeg[0] - referencia ; 
    correccionP = error*kp + 88;
    if (correccionP <= 33){
      correccionP = 33;
    }
  }
    
  }

  if (Ndeg[0] > 0 &&  Ndeg[0] <= 180){
    error = (Ndeg[0]) - referencia; 
    correccionP = error*kp + 88;
    if (correccionP >= 132){
      correccionP = 132;
    }
  }
  if (correccionP > 80 && correccionP < 100){
    if (flagPOS == true){
      flagPOS = false;
      n = 0;
    }
    encoder();
    desplazamiento = (float(n) / pasosPorVuelta) * perimetro;
    Serial.println("desplazamiento :");
    Serial.println(desplazamiento);
    if (desplazamiento >= 3.5){
      flagRUT = true;
      flagEspera = true;
    }
  } 

  myservo.write(correccionP);
  Serial.println(correccionP);
}

void South(){
  Serial.println("Sur");
  if (Ndeg[0] > 0 &&  Ndeg[0] <= 180){
    error = referencia - (360 - Ndeg[0]) ; 
    correccionP = error*kp + 88;
    if (correccionP <= 33){
      correccionP = 33;
    }

  }

  if (Ndeg[0] > 180 &&  Ndeg[0] <= 360){
    error = referencia - Ndeg[0]; 
    correccionP = -error*kp + 88;
    if (correccionP >= 132){
      correccionP = 132;
    }
  }

  if (correccionP > 80 && correccionP < 100){
    if (flagPOS == true){
      flagPOS = false;
      n = 0;
    }
    encoder();
    desplazamiento = (float(n) / pasosPorVuelta) * perimetro;
    Serial.println("desplazamiento :");
    Serial.println(desplazamiento);
    if (desplazamiento >= 3.5){
      flagRUT = true;
      flagEspera = true;
    }  
  } 

  myservo.write(correccionP);
  Serial.println(correccionP);
}

void West(){
  Serial.println("Oeste");
  if (Ndeg[0] > 90 &&  Ndeg[0] <= 270){
    error = (Ndeg[0] - 270); 
    correccionP = error*kp + 88;
    if (correccionP <= 33){
      correccionP = 33;
    }

  }

   if ((Ndeg[0] > 270 &&  Ndeg[0] <= 360) || (Ndeg[0] >= 0 &&  Ndeg[0] < 90)){
    if (Ndeg[0] > 270 &&  Ndeg[0] <= 360){
      error = Ndeg[0] - 270;
      correccionP = error*kp + 88;
      if (correccionP >= 132){
      correccionP = 132;
    }
    }
    if (Ndeg[0] >= 0 &&  Ndeg[0] <= 90){
      error = Ndeg[0] + 90;
      correccionP = error*kp + 88;
      if (correccionP >= 132){
      correccionP = 132;
      }
    }
   }

  if (correccionP > 80 && correccionP < 100){
    if (flagPOS == true){
      flagPOS = false;
      n = 0;
    }
    encoder();
    desplazamiento = (float(n) / pasosPorVuelta) * perimetro;
    Serial.println("desplazamiento :");
    Serial.println(desplazamiento);
    if (desplazamiento >= 5.5){
      flagRUT = true;
      flagEspera = true;
    }  
  } 

  myservo.write(correccionP);
  Serial.println(correccionP);
}


void Secuencia() {
  switch (estado) {
    case 0: // Encender el LED durante 500 ms
      digitalWrite(Forward,LOW);
      digitalWrite(Backward,LOW);
      myservo.write(88);
      ledcWrite(2, 0);
      i++;
      if (i == 40){
        estado = 1;  
        i = 0;
      }
      if (flagCNE == true){
        flagCNE = false;
      }
      break;
    case 1:
      digitalWrite(Forward,LOW);
      digitalWrite(Backward,HIGH);
      ledcWrite(2, 180);
      i++;
      if (i == 25){
        estado = 2;  
        i = 0;
      }
      break;
    case 2:
      digitalWrite(Forward,LOW);
      digitalWrite(Backward,LOW);
      ledcWrite(2, 0);
      i++;
      if (i == 40){
        estado = 3;  
        i = 0;
      }
      break;
    case 3:
      if (Ndis[0] <= 50){
        estado = 0;
      }
      digitalWrite(Forward,LOW);
      digitalWrite(Backward,LOW);
      ledcWrite(2, 0);
      myservo.write(45);
      i++;
      if (i == 25){
        estado = 4;  
        i = 0;
      }
      break;
    case 4:
      if (Ndis[0] <= 50){
        estado = 0;
      }
      digitalWrite(Backward,LOW);
      digitalWrite(Forward,HIGH);
      ledcWrite(2, 180);
      i++;;
      if (i == 17){
        myservo.write(132);
      }
      if (i == 25){
        estado = 5; 
        i = 0;
      }
      break;
    case 5:
      if (Ndis[0] <= 50){
        estado = 0;
      }
      myservo.write(88);
      i++;
      if (i == 10){
        estado = 6; 
        i = 0;
      }
      break;
    case 6:
      if (Ndis[0] <= 50){
        estado = 0;
      }
      myservo.write(125);
      i++;
      if (i == 20) {
        myservo.write(33);
      }
      if (i == 25){
        estado = 0; 
        i = 0;
        flagSU = false;
        flagCNE = true;
      }
      break;
  }
}

void encoder() {

  ant=act;
  
  if(digitalRead(C1)) bitSet(act,1); else bitClear(act,1);            
  if(digitalRead(C2)) bitSet(act,0); else bitClear(act,0);

  if(ant == 2 && act ==0) {n++;}
  if(ant == 0 && act ==1) {n++;}
  if(ant == 3 && act ==2) {n++;}
  if(ant == 1 && act ==3) {n++;}
  
  // Restar desplazamiento negativo
  //if(ant == 1 && act ==0) n--;
  //if(ant == 3 && act ==1) n--;
  //if(ant == 0 && act ==2) n--;
  //if(ant == 2 && act ==3) n--;    
}

float Ultrasonic(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  
  float distanceCm = duration * SOUND_SPEED/2;

  return distanceCm;
}

float Magnetometro(float arr[]){
  while (mySerial.available()>0){
    char data= mySerial.read();
    if (data >= 32 && data <= 126) { // Solo caracteres imprimibles
        buffer += data;
    }
  }
  if (buffer.length() > 0) { 
    
    // Encontrar las comas y dividir el string
    int firstComma = buffer.indexOf(',');    // Posición de la primera coma
    int secondComma = buffer.indexOf(',', firstComma + 1); // Posición de la segunda coma

    // Extraer las partes
    String part1 = buffer.substring(0, firstComma); // "xxxx"
    String part2 = buffer.substring(firstComma + 1, secondComma); // "yyyy"
    String part3 = buffer.substring(secondComma + 1); // "zzzz"

    int num1 = part1.toInt();
    int num2 = part2.toInt();
    int num3 = part3.toInt();

    XYZMag[1] = num1;
    XYZMag[2] = num2;
    XYZMag[3] = num3;

    float Ndeg = atan2(num2,num1)*(180/3.1416);

    if (Ndeg < 0) {
        Ndeg += 360.0;  // Ajusta los negativos
    }
    
    buffer = ""; // Vacía el buffer después de imprimir

    return Ndeg;
  }
}

void Sending(){
  if (labs(millis() - timer) > PUBLISH_FREQUENCY) {
    /* Leemos el valor del potenciómetro */
    // Magnetometro
    float Ndeg = Magnetometro(XYZMag);
    // Encoder
    encoder();
    desplazamiento = (float(n) / pasosPorVuelta) * perimetro;
    // Ultrasonico
    float distancia = Ultrasonic();
    
    /* Definimos que el valor del potenciómetro será enviado por la variable definida */
    ubidots.add(VARIABLE_LABEL_MAG, Ndeg);
    ubidots.add(VARIABLE_LABEL_ENC, desplazamiento);
    ubidots.add(VARIABLE_LABEL_DIS, distancia);
    /* Hacemos la publicación de los datos en el dispositivo definido */
    ubidots.publish(DEVICE_LABEL);
    /* Para mostrar los datos, los imprimimos en el terminal Serial */
    Serial.println("Enviando los datos a Ubidots: ");
    Serial.println("Valor del magnetometro: " + String(Ndeg));
    Serial.println("Valor del enconder: " + String(desplazamiento));
    Serial.println("Valor del magnetometro: " + String(distancia));
    Serial.println("-----------------------------------------");
    timer = millis();
  }
}