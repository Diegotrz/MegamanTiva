//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * IE3027: Electrónica Digital 2 - 2019
 */
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7}; 
//Pushbottons
#define PB1 PUSH1
#define PB2 PUSH2
//***************************************************************************************************************************************
// Definición de variables
//***************************************************************************************************************************************
// lectura del estado de pushbuttons
int PB1State = 1;
int PB2State = 1;
int x_1 = 20;
int y_1 = 20;
// variable de antirebote de botones
int pressed1 = 0;
int pressed2 = 0;
// variable para selección de nivel
int level = 0;
// color de llenado según el nivel
int fill_color;
//Variable para el while de animaciones
int pj1 = 0;
//suelos
int xsl1= 0;
int xsl2= 0; 
// posiciones de J1 y J2 en pantalla
int posx1 = 135;
int posy1 = 153;
int posx2 = 60;
int posy2 = 153;
int anclas_aviones[2] = {0, 0};

// Variables para control de estados del juego (jugando, fin del juego, reinicio, etc)
bool jugando = false, ganador = false, apagarControlJ1 = false, apagarControlJ2 = false; 
//Variables para la generacion de los cuadros de la reduccion de la barra de vida
int reducx1 = 0;
int reducx2 = 0;
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);


extern uint8_t fondo[];

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  LCD_Init();
  LCD_Clear(0x00);
  // configuración de botones
  pinMode(PB1, INPUT_PULLUP);
  pinMode(PB2, INPUT_PULLUP);
 
//LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
    
  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
  //LCD_Bitmap(0, 0, 320, 240, fondo);
  LCD_Bitmap(0, 0, 320, 240, fondo);
  String textd = "Diego T.";
  String texta= "Adrian S.";
  String textop2= "Select characters";
  String textop3= "Select map";
  LCD_Print(textd,25 ,190, 1, 0xffff, 0x000);
  LCD_Print(texta,25, 200, 1, 0xffff, 0x000);
  LCD_Print(textop2,110, 190, 1.5, 0xffff, 0x000);
  LCD_Print(textop3,110, 190+22, 1.5, 0xffff, 0x000);
  //String text70 = "*";
  //LCD_Print(text70, 93, 168, 2, 0xffff, 0x421b);
  // FillRect(93, 168, 10, 10, 0x3DC4);
  /*
  for(int x = 0; x <319; x++){
    LCD_Bitmap(x, 52, 16, 16, tile2);
    LCD_Bitmap(x, 68, 16, 16, tile);
    
    LCD_Bitmap(x, 207, 16, 16, tile);
    LCD_Bitmap(x, 223, 16, 16, tile);
    x += 15;
 }
  */
 while(PB1State == 1){
    PB1State = digitalRead(PB1);
    PB2State = digitalRead(PB2);
    for(int x = 0; x <100; x++){
    delay(2);
  int anima = (x/35)%3;
  LCD_Sprite(93,168+level,12,12,bolita,3,anima,0,0);
  }
    
    
    //Serial.println(level);
    if(level == 0){
      FillRect(93, 168+22, 12, 12, 0x0000);
      FillRect(93, 168+44, 12, 12, 0x0000);
      FillRect(93, 168+66, 12, 12, 0x0000);
    }
    if(level == 22){
      FillRect(93, 168, 12, 12, 0x0000);
      FillRect(93, 168+22, 12, 12, 0x0000);
      FillRect(93, 168+44, 12, 12, 0x0000);
      FillRect(93, 168+66, 12, 12, 0x0000);
    }
    if(level > 44){
      level = 0;
      FillRect(93, 168, 12, 12, 0x0000);
      FillRect(93, 168+22, 12, 12, 0x0000);
      FillRect(93, 168+66, 12, 12, 0x0000);
    }
    if (PB2State == HIGH){
      pressed1 = 1;
    }
    if (PB2State == LOW && pressed1 == 1){
      level = level + 22;   
    //  FillRect(105, 125+level, 5, 10, 0x0000);
      pressed1 = 0;
    }
  }
if (level == 0){
    //Serial.println("Mapa 1 seleccionado");
    fill_color = 0x66FC;  // COLOR HEX: 78c6ec
  }

  //-------------------------------------------------
  fondtr();
while (pj1 <=600){
  for(int x = 0; x <400; x++){
    delay(3);
    
  int anim2 = (x/35)%4;
 // LCD_Sprite(135,95,52,47,zerorun,4,anim2,0,1);
  //int anim = (x/35)%3;
  //LCD_Sprite(0,0,320,240,fondmov,3,anim,0,0);
  
  pj1++;
  }
  } 
LCD_Clear(0x00);
delay(5);
fondtr();  
pj1 =0;
while (pj1 <=600){
  for(int x = 0; x <400; x++){
    delay(3);
    
  int anim2 = (x/35)%4;
  LCD_Sprite(135,148,40,40,xrun,4,anim2,0,1);
  //int anim = (x/35)%3;
  //LCD_Sprite(0,0,320,240,fondmov,3,anim,0,0);
  
  pj1++;
  }
  } 
  LCD_Clear(0x00);
  fondtr();  
  grafvid();
  reducvid();
  //LCD_Bitmap(150, 105, 34, 35, xsing);
}

//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
PB1State = digitalRead(PB1);
PB2State = digitalRead(PB2);
// Control de movimientos y desactivación de controles cuando un jugador pierde 
  if (PB1State == 0 && !apagarControlJ2){
    anclas_aviones[1] = jump_2(PB1State, 85, 34, 35, xsing);
  } else {
    if (apagarControlJ2){
      FillRect(posx2, anclas_aviones[1], 38, 28, fill_color);
    } else {
      anclas_aviones[1] = fall_2(153, 34, 35, xsing);
    }
  }
  if (PB2State == 0 && !apagarControlJ1){
   anclas_aviones[1]= mov_1(PB2State ,0,282, 34, 35, xsing);
   // anclas_aviones[0] = jump_1(PB2State, 10, 40, 40, xrun);
  } else {
    if (apagarControlJ1){
     // FillRect(posx1, anclas_aviones[0], 38, 28, fill_color);
    } else {
      //anclas_aviones[0] = fall_1(200, 40, 40, xrun);
    }
  }
  








  //Prueba de la bolita de seleccion
  /*
  for(int x = 0; x <320-32; x++){
    delay(15);
  int anima = (x/35)%3;
  LCD_Sprite(93,168,12,12,bolita,3,anima,0,0);
  }
  */
  //for(int x = 0; x <320-32; x++){
  //  delay(15);
 // int anim2 = (x/35)%4;
    
  // LCD_Sprite(x,100,52,47,a,4,anim2,0,1);
    //V_line( x -1, 100, 24, 0x421b);
    
    //LCD_Bitmap(x, 100, 32, 32, prueba);
    
  //  int anim = (x/11)%8;
    
/*
    int anim3 = (x/11)%4;
    
    LCD_Sprite(x, 20, 16, 32, mario,8, anim,1, 0);
    V_line( x -1, 20, 32, 0x421b);
 
    //LCD_Sprite(x,100,32,32,bowser,4,anim3,0,1);
    //V_line( x -1, 100, 32, 0x421b);
 
 
    LCD_Sprite(x, 140, 16, 16, enemy,2, anim2,1, 0);
    V_line( x -1, 140, 16, 0x421b);
  
    LCD_Sprite(x, 175, 16, 32, luigi,8, anim,1, 0);
    V_line( x -1, 175, 32, 0x421b);
 */
  
  /*
  for(int x = 320-32; x >0; x--){
    delay(5);
    int anim = (x/11)%8;
    int anim2 = (x/11)%2;
    
    LCD_Sprite(x,100,16,24,planta,2,anim2,0,0);
    V_line( x + 16, 100, 24, 0x421b);
    
    //LCD_Bitmap(x, 100, 32, 32, prueba);
    
    //LCD_Sprite(x, 140, 16, 16, enemy,2, anim2,0, 0);
    //V_line( x + 16, 140, 16, 0x421b);
    
    //LCD_Sprite(x, 175, 16, 32, luigi,8, anim,0, 0);
    //V_line( x + 16, 175, 32, 0x421b);

    //LCD_Sprite(x, 20, 16, 32, mario,8, anim,0, 0);
    //V_line( x + 16, 20, 32, 0x421b);
  } 
*/
}
//***************************************************************************************************************************************
// Función para inicializar los gráficos de las vidas
//***************************************************************************************************************************************
void grafvid(void){
 FillRect(30, 60, 80, 15, 0x2623);  //Cuadro que genera la primera barra de vida
 FillRect(213, 60, 80, 15, 0x2623); //Cuadro que genera la segunda barra de vida
 String textj1 = "J1"; 
 LCD_Print(textj1,10 ,60, 1, 0xffff,0x66FC ); 
String textj2 = "J2"; 
 LCD_Print(textj2,300 ,60, 1, 0xffff,0x66FC ); 
//Bordes para la primera barra de vida
 FillRect(30, 60, 80, 3, 0xA514); //Borde superior 
 FillRect(30, 75, 83, 3, 0xA514); //Borde inferior
 FillRect(30, 60, 3, 15, 0xA514); //Borde lateral izq
 FillRect(110, 60, 3, 15, 0xA514); //Borde lateral derecho
//Bordes para la segunda barra de vida
 FillRect(213, 60, 80, 3, 0xA514); //Borde superior 
 FillRect(213, 75, 83, 3, 0xA514); //Borde inferior
 FillRect(213, 60, 3, 15, 0xA514); //Borde lateral izq
 FillRect(293, 60, 3, 15, 0xA514); //Borde lateral derecho
 
  }
//***************************************************************************************************************************************
// Función para la reducción de la vida
//***************************************************************************************************************************************
void reducvid (void){
 reducx1 = 90;
 reducx2 =  273;
 FillRect(reducx1, 63, 20, 12, 0xE100); 
 FillRect(reducx1, 63, 20, 12, 0xE100);
 
  
  
  
  }
//***************************************************************************************************************************************
// Función para inicializar los fondos
//***************************************************************************************************************************************
void fondtr(void){
xsl1 =0;
xsl2 = 0;
  
 FillRect(0, 0, 320, 240, fill_color); 


for(int r = 0; r <13  ; r++){

LCD_Bitmap(xsl1, 188, 25, 26, suelo1);

xsl1= xsl1+25;

}
for(int r2 = 0; r2 <17; r2++){
//LCD_Bitmap(xsl2, 170, 19, 18, suelo2);  
//LCD_Bitmap(xsl2, 188, 19, 18, suelo2);
LCD_Bitmap(xsl2, 206, 19, 18, suelo2);
LCD_Bitmap(xsl2, 222, 19, 18, suelo2);
xsl2= xsl2+19;  
  }
LCD_Bitmap(0, 0, 33, 25, nube1);
LCD_Bitmap(60, 35, 33, 25, nube1);
LCD_Bitmap(125, 13, 33, 25, nube1);
LCD_Bitmap(200, 20, 33, 25, nube1);
LCD_Bitmap(270,28, 33, 25, nube1);



  
  }
//****************************************
// Caída
//****************************************

// Jugador 1
int fall_1(int ylim2, int width, int height, unsigned char bitmap[]) {
  int anim = (posy1 / 35) % 2;
  if (posy1 < ylim2) {
    posy1 = posy1 + 2;
  }
  LCD_Bitmap(posx1, posy1, width, height, bitmap);
  
  H_line(posx1, posy1 - 1, width, fill_color);
  H_line(posx1, posy1 - 1 - 1, width, fill_color);
  return posy1;
};

// Jugador 2
int fall_2(int ylim2, int width, int height, unsigned char bitmap[]) {
  
  if (posy2 < ylim2) {
    delay(18);
    posy2 = posy2 + 2;
  }
  LCD_Bitmap(posx2, posy2, width, height, bitmap);
  
  H_line(posx2, posy2 - 1, width, fill_color);
  H_line(posx2, posy2 - 1 - 1, width, fill_color);
  return posy2;
};

//****************************************
// Salto
//****************************************
// Jugador 1
int jump_1(int buttonState, int ylim1, int width, int height, unsigned char bitmap[]) {
  
  if (buttonState == 0 & posy1 - 10 > ylim1) {
    posy1 = posy1 - 10;
    FillRect(posx1, posy1 + height, width, height, fill_color);
   // H_line(posx1, posy1 + height, width, fill_color);
    LCD_Bitmap(posx1, posy1, width, height, bitmap);
   
    }
  
  
  return posy1;
};

// Jugador 2
int jump_2(int buttonState, int ylim1, int width, int height, unsigned char bitmap[]) {
  
  if (buttonState == 0 & posy2 - 10 > ylim1) {
    posy2 = posy2 - 10;
    FillRect(posx2, posy2 + 10, width, height, fill_color);
   // H_line(posx2, posy2 + height, width, fill_color);
    LCD_Bitmap(posx2, posy2, width, height, bitmap);
    
    }
  
  
  return posy2;
};
//****************************************
// Movimiento
//****************************************
//---------------------------------------------Movimiento a la derecha------------------------------------------
//----------------------------------------------------Jugador 2------------------------------------------------------
int mov_1(int buttonState, int xlim1,int xlim2, int width, int height, unsigned char bitmap[]) {
  
  if (buttonState == 0  & posx2  < xlim2)  {
    delay(20);
    posx2 = posx2 + 5;
    FillRect(posx2-5, posy2 , width, height, fill_color);
   // H_line(posx2, posy2 + height, width, fill_color);
    LCD_Bitmap(posx2, posy2, width, height, bitmap);
    
    }
  
  
  return posy2;
};


//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+w;
  y2 = y+h;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = w*h*2-1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
      
      //LCD_DATA(bitmap[k]);    
      k = k - 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
  for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width -1 - offset)*2;
      k = k+width*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k - 2;
     } 
  }
  }else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
  }
    
    
    }
  digitalWrite(LCD_CS, HIGH);
}
