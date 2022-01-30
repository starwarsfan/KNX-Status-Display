//////////////////////////////////////////////////////////////////////////////
//
// KNX-Status-Display Sketch
//
// Y. Schumann - 01/2022
//
//////////////////////////////////////////////////////////////////////////////

#include <KnxTpUart.h>                              // Einbindung der TpUart Library
#include <Nextion.h>                                // Einbindung Nextion Library
#include <SoftwareSerial.h>                         // Einbindung der Software Serial Library, der Pro Micro
                                                    // hat 2 serielle Hardwareschnittstellen, eine wird für
                                                    // den Busankoppler benötigt, die Andere für die
                                                    // Programmierung über USB, das Display wird somit
                                                    // softwareseitig eingebunden

// GAs definieren
String knxGA01 = "3/3/70";                          // KNX -> Display: DPT5 (Byte), ID der anzuzeigenden Seite, "Display aus" mit -1
String knxGA02 = "3/3/71";                          // Display -> KNX: DPT5 (Byte), Nummer vom Keypad
String knxGA03 = "3/3/72";                          // Display -> KNX: DPT1 (Bit), true/false fuer OK/Cancel

// Kommunikation definieren
SoftwareSerial HMISerial(9,8);                      // Festlegung der Pins (RX, TX) für die serielle
                                                    // Verbindung des Displays
KnxTpUart knx(&Serial1, "1.0.54");                  // Festlegung der seriellen Schnittstelle mit welcher
                                                    // der Arduino mit dem Busankoppler kommuniziert
                                                    // sowie der physikalischen Adresse.

// Buttons definieren: NexButton(PageID, ButtonID, ButtonName)
NexButton bNumber1 = NexButton(3,  1, "bNo1");      // "1"
NexButton bNumber2 = NexButton(3,  2, "bNo2");      // "2"
NexButton bNumber3 = NexButton(3,  3, "bNo3");      // "3"
NexButton bNumber4 = NexButton(3,  4, "bNo4");      // "4"
NexButton bNumber5 = NexButton(3,  5, "bNo5");      // "5"
NexButton bNumber6 = NexButton(3,  6, "bNo6");      // "6"
NexButton bNumber7 = NexButton(3,  7, "bNo7");      // "7"
NexButton bNumber8 = NexButton(3,  8, "bNo8");      // "8"
NexButton bNumber9 = NexButton(3,  9, "bNo9");      // "9"
NexButton bNumber0 = NexButton(3, 10, "bNo0");      // "0"
NexButton bCancel  = NexButton(3, 11, "bCancel");   // "Cancel"
NexButton bOK      = NexButton(3, 12, "bOk");       // "OK"

// Liste der Button-Objecte
NexTouch *nex_listen_list[] = {
    &bNumber1,
    &bNumber2,
    &bNumber3,
    &bNumber4,
    &bNumber5,
    &bNumber6,
    &bNumber7,
    &bNumber8,
    &bNumber9,
    &bNumber0,
    &bCancel,
    &bOK,
    NULL
};

void setup() {
    HMISerial.begin(9600);                          // Start der seriellen Verbindung (Display) mit 9600 Baud
    sendToDisplay("dims=40");                       // Festlegen der Displayhelligkeit auf 40%

    nexInit();                                      // Display initialisieren
    bOK.attachPop(bOkPopCallback);                  // Callbacks der Buttons registrieren (attachPop(...) = beim loslassen des Buttons)
    bCancel.attachPop(bCancelPopCallback);
    bNumber1.attachPop(bNumber1PopCallback);
    bNumber2.attachPop(bNumber2PopCallback);
    bNumber3.attachPop(bNumber3PopCallback);
    bNumber4.attachPop(bNumber4PopCallback);
    bNumber5.attachPop(bNumber5PopCallback);
    bNumber6.attachPop(bNumber6PopCallback);
    bNumber7.attachPop(bNumber7PopCallback);
    bNumber8.attachPop(bNumber8PopCallback);
    bNumber9.attachPop(bNumber9PopCallback);
    bNumber0.attachPop(bNumber0PopCallback);

    Serial1.begin(19200, SERIAL_8E1);               // Start der seriellen Verbindung (Busankoppler) mit 19200 Baud
    knx.uartReset();                                // Uart Reset
    knx.addListenGroupAddress(knxGA01);             // Gruppenadressen auf die der Arduino "lauschen" soll
//    knx.addListenGroupAddress(knxGA02);
//    knx.addListenGroupAddress(knxGA03);
}

void loop() {
    nexLoop(nex_listen_list);                       // Loop um Display-Events auszuwerten
    delay(10);                                      // Pause in ms
}

void serialEvent1() {
    KnxTpUartSerialEventType eType = knx.serialEvent();
  
    if (eType == KNX_TELEGRAM) {
        KnxTelegram* telegram = knx.getReceivedTelegram();         // Telegrammauswertung auf KNX (bei Empfang immer notwendig)

        String target =                                            // Zusammensetzen der Gruppenadresse
        String(0 + telegram->getTargetMainGroup())   + "/" +
        String(0 + telegram->getTargetMiddleGroup()) + "/" +
        String(0 + telegram->getTargetSubGroup());

        if (telegram->getCommand() == KNX_COMMAND_WRITE) {         // Auswertung des empfangenen KNX-Telegrammes
             if (target.equals(knxGA01)) {                         // Wenn die Gruppenadresse knxGA01 empfangen wird...
                int pageID = telegram->get1ByteIntValue();         // Hole Integer-Wert aus Telegram
                if (pageID <= 0) {                                 // Wenn 0 oder kleiner ...
                    sendToDisplay("sleep=1");                      // ... schalte das Display aus (1=Display Aus)
                } else {                                           // Ansonsten ...
                    sendToDisplay("sleep=0");                      // ...Wecke das Display auf (0=Display Ein)
                    sendToDisplay("page " + String(pageID - 1));   // ...Rufe uebergebene Seite auf
                }
//             } else if (target == knxGA04) {                     // Wenn die Gruppenadresse knxGA04 empfangen wird...
//                 if (telegram->getBool()) {                      // ...Wenn der Befehl "Ein" lautet...
//                     sendToDisplay("sleep=0");                   // ...Wecke das Display auf...
//                     sendToDisplay("page 1");                    // ...Rufe Seite 2 auf
//                 } else {                                        // ...Ansonsten...
//                     sendToDisplay("sleep=1");                   // ...Schalte das Display aus
//                 }
            }
        }
    }
}

/**
    Uebergebenen String an Display schicken,
    fuer Seitenwechsel bspw. "page 3"
*/
void sendToDisplay(String string) {
    HMISerial.print(string);
    HMISerial.print("\xFF\xFF\xFF");
}

/**
    Je Button muss ein Callback definiert werden, da es leider nicht moeglich ist,
    die ID eines Buttons zu ermitteln. Die eigentlichen Funktionen zum senden auf
    den Bus werden aus diesen Callbacks heraus mit entsprechenden Parametern
    aufgerufen.
*/
void bOkPopCallback(void *ptr) {
    sendBoolToKNX(knxGA03, true);
}
void bCancelPopCallback(void *ptr) {
    sendBoolToKNX(knxGA03, false);
}
void bNumber1PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 1);
}
void bNumber2PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 2);
}
void bNumber3PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 3);
}
void bNumber4PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 4);
}
void bNumber5PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 5);
}
void bNumber6PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 6);
}
void bNumber7PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 7);
}
void bNumber8PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 8);
}
void bNumber9PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 9);
}
void bNumber0PopCallback(void *ptr) {
    sendIntToKNX(knxGA02, 0);
}

/**
    Hilfsfunktionen zum senden auf den Bus
*/
void sendBoolToKNX(String knxGA, bool boolean) {
    bool success = knx.groupWriteBool(knxGA, boolean);
}
void sendIntToKNX(String knxGA, int integer) {
    bool success = knx.groupWrite1ByteInt(knxGA, integer);
}
