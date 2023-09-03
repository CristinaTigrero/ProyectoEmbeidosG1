#include <Adafruit_Fingerprint.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27,16,2);
// declaracion de pines de los pulsadores
//Sensor de huella
SoftwareSerial SERIE(2,3);
Adafruit_Fingerprint DEDO = Adafruit_Fingerprint(&SERIE);
//Rele
#define relay 4
int id;
//Teclado
#include <Keypad.h>
const byte FILAS =4;
const byte COLUMNAS =4;
char keys[FILAS][COLUMNAS]={
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte filaspines[FILAS]= {8,7,6,5};
byte columnaspines[COLUMNAS]= {12,11,10,9};
Keypad teclado= Keypad(makeKeymap(keys),filaspines,columnaspines,FILAS, COLUMNAS);
char key;

//Para comprobar que la huella existe
int getFingerPrint() 
{
  int p = DEDO.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = DEDO.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  p = DEDO.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  // found a match!
  return DEDO.fingerID;

}
//Funciones de los botones

//Boton 1 escanear huella
void press1(){
  Serial.println("f1");
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("Sensor activado");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("Escaneando");
  delay(3000);
  if ( getFingerPrint() != -1)
  {
    digitalWrite(relay, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Puerta abierta");
       
  }  
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No registrado");
  }
  delay(3000);
  lcd.clear();
}
void press2(){
  //segundo botón es CERRAR que cierra la puerta
  lcd.setCursor(0,0);
  Serial.println("f2"); 
  lcd.print("Cerrando puerta");
  digitalWrite(relay, HIGH); 
  delay(3000);
  lcd.clear();
}
//Encontrar el id de la huella 
int detectaHuella(){
uint8_t p = DEDO.getImage(); //Toma una imagen de la huella.
if(p != FINGERPRINT_OK) return -2;
p = DEDO.image2Tz(); //Convierte la imagen en una plantilla.
if(p != FINGERPRINT_OK) return -1;
p = DEDO.fingerFastSearch();
if(p != FINGERPRINT_OK) return -1;
Serial.print("ID encontrado #"); Serial.print(DEDO.fingerID);
Serial.print(" con una confianza de "); Serial.println(DEDO.confidence);
return DEDO.fingerID; 
}
//
void press3(){
  Serial.println("f"); 
  lcd.setCursor(0,0);
  lcd.print("Agregar Usuario");
  delay(3000);
  lcd.setCursor(0,1);
  lcd.print("Huella administrador");
  delay(3000);
  if(detectaHuella()==1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ingrese el ID");
    Serial.println("Listo para registrar una huella digital.");
    Serial.println("Escriba el número de ID (del 1 al 162) que desea guardar en este dedo.");
    id = leeNumero();
    lcd.setCursor(14,0);
    lcd.print(id);
    if(!id){ //El id tiene que ser distinto de cero.
    return;}
    Serial.print("Inscripción del ID ");
    Serial.println(id);
    registrarHuella();
    lcd.setCursor(0,0);
    lcd.print("Usuario Agregado");
   //while(1);
  delay(2000);
  lcd.clear();
  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No coincide");
    delay(2000);
    lcd.clear();
  }


}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  DEDO.begin(57600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Bienvenido");
  delay(3000);
  lcd.clear();
  pinMode(relay,OUTPUT);
  digitalWrite(relay, HIGH);
  if(DEDO.verifyPassword()){
  Serial.println("Sensor de huella encontrado.");
  }
  else{
  Serial.println("No se encontró un sensor de huella");
  while(1){delay(1);}
  }
  

}
int leeNumero(void)
{
  int second=0;
  while( 1 )
  {
    key = teclado.getKey();
    if(key >= '0' && key <= '9')
    {
      second = second * 10 + (key - '0');
    }

    if(key == 'D') break;  
  }
 return second; 
}


void loop() {
  // put your main code here, to run repeatedly:
  key = teclado.getKey();
  lcd.setCursor(0,0);
  lcd.print("Escoja una opcion");
  lcd.setCursor(0,1);
  lcd.print("A  B  C");
  if(key){
    lcd.clear();
    delay(500);
    if(key=='A'){
      Serial.println("OPCION A");
      press1();
    }
    if(key=='B'){
      Serial.println("OPCION B");
      press2();
    }
    if(key=='C'){
      Serial.println("OPCION C");
      press3();
    }

  }
 
  delay(1000);
  key=NULL;


} 

uint8_t registrarHuella() {
int p = -1;
Serial.print("Ingrese su dedo para poder registrarlo en el ID "); Serial.println(id);
while(p != FINGERPRINT_OK){
/*
La función getImage() pide al sensor que tome una imagen del dedo presionado en la superficie.
La función retorna:
FINGERPRINT_OK en caso de éxito.
FINGERPRINT_NOFINGER si no se detecta ningún dedo.
FINGERPRINT_PACKETRECIEVERR en error de comunicación.
FINGERPRINT_IMAGEFAIL en error de imagen.
*/
p = DEDO.getImage();
switch(p){
case FINGERPRINT_OK: Serial.println("Imagen tomada."); break;
case FINGERPRINT_NOFINGER: Serial.println("."); break;
case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Error de comunicación."); break;
case FINGERPRINT_IMAGEFAIL: Serial.println("Error de imagen."); break;
default: Serial.println("Error desconocido."); break;
}
}
/*
La función image2Tz() pide al sensor que convierta la imagen en una plantilla.
Recibe como parámetro la ubicación de la ranura para colocar la plantilla de 
características (se coloca una en 1 y otra en 2 para verificación para crear el modelo).
La función retorna:
FINGERPRINT_OK en caso de éxito.
FINGERPRINT_IMAGEMESS si la imagen está demasiado desordenada.
FINGERPRINT_PACKETRECIEVERR en error de comunicación.
FINGERPRINT_FEATUREFAIL al no poder identificar las características de las huellas dactilares.
FINGERPRINT_INVALIDIMAGE al no poder identificar las características de las huellas dactilares.
*/
p = DEDO.image2Tz(1);
switch(p){
case FINGERPRINT_OK: Serial.println("Imagen convertida."); break;
case FINGERPRINT_IMAGEMESS: Serial.println("Imagen demasiado desordenada."); return p;
case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Error de comunicación."); return p;
case FINGERPRINT_FEATUREFAIL: Serial.println("No se pudieron encontrar las características de la huella digital."); return p;
case FINGERPRINT_INVALIDIMAGE: Serial.println("No se pudieron encontrar las características de la huella digital."); return p;
default: Serial.println("Error desconocido."); return p;
}
Serial.println("Quite el dedo.");
lcd.setCursor(0,1);
lcd.print("Otra vez su huella");
delay(3000);
lcd.clear();
delay(2000);
p = 0;
while(p != FINGERPRINT_NOFINGER){ //Se queda en el while siempre y cuando no se retire el dedo del sensor.
p = DEDO.getImage(); //Obtiene la imagen de la huella digital.
}
Serial.print("ID "); Serial.println(id);
p = -1;
Serial.println("Vuelva a colocar el mismo dedo.");
while(p != FINGERPRINT_OK){ //Se queda en el while si no se coloca el dedo.
p = DEDO.getImage(); //Obtiene la imagen de la huella digital.
switch(p){
case FINGERPRINT_OK: Serial.println("Imagen tomada"); break;
case FINGERPRINT_NOFINGER: Serial.print("."); break;
case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Error de comunicación."); break;
case FINGERPRINT_IMAGEFAIL: Serial.println("Error de imagen."); break;
default: Serial.println("Error desconocido."); break;
}
}
p = DEDO.image2Tz(2); //Pide al sensor que convierta la imagen en una plantilla.
switch(p){
case FINGERPRINT_OK: Serial.println("Imagen convertida."); break;
case FINGERPRINT_IMAGEMESS: Serial.println("Imagen demasiado desordenada."); return p;
case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Error de comunicación."); return p;
case FINGERPRINT_FEATUREFAIL: Serial.println("No se pudieron encontrar las características de la huella digital."); return p;
case FINGERPRINT_INVALIDIMAGE: Serial.println("No se pudieron encontrar las características de la huella digital."); return p;
default: Serial.println("Error desconocido."); return p;
}
Serial.print("Creando modelo para el id ");  Serial.println(id);
/*
La función createModel pide al sensor que tome dos plantillas de funciones de impresión y cree un modelo.
La función retorna:
FINGERPRINT_OK en caso de éxito.
FINGERPRINT_PACKETRECIEVERR en error de comunicación.
FINGERPRINT_ENROLLMISMATCH en discrepancia de huellas dactilares.
*/
p = DEDO.createModel();
switch(p){
case FINGERPRINT_OK: Serial.println("Impresiones emparejadas"); break;
case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Error de comunicación"); return p;
case FINGERPRINT_ENROLLMISMATCH: Serial.println("Las huellas dactilares no coinciden."); return p;
default: Serial.println("Error desconocido."); return p;
}
Serial.print("ID "); Serial.println(id);
/*
La función storeModel pide al sensor que almacene el modelo calculado para su posterior comparación.
La función recibe como parámetro la ubicación del id.
La función retorna:
FINGERPRINT_OK en caso de éxito.
FINGERPRINT_BADLOCATION si la ubicación no es válida.
FINGERPRINT_FLASHERR si el modelo no se pudo escribir en la memoria flash.
FINGERPRINT_PACKETRECIEVERR en error de comunicación.
*/
p = DEDO.storeModel(id);
switch(p){
case FINGERPRINT_OK: Serial.println("Huella almacenada."); break;
case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Error de comunicación."); return p;
case FINGERPRINT_BADLOCATION: Serial.println("No se pudo almacenar en esa ubicación."); return p;
case FINGERPRINT_FLASHERR: Serial.println("Error de escritura en la memoria flash."); return p;
default: Serial.println("Error desconocido."); return p;
}
}
