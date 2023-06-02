///////////////////////////////////////////////////////////////////////////////
//
//                Temporizador con Termostato versión 1
// 
//    El objetivo de este equipo es controlar la temperatura del horno.
//
// Con el uso de un sensor de temperatura LM35/LM335 se capta la Temperatura
// real (Temperatura) del horno y se hace una comparación con la Temperatura
// deseada (Grados) teniendo en cuenta la Tolerancia.
//
// Si Temperatura < (Grados - Tolerancia),  se activa la resistencia que calienta el horno.
// Si Temperatura > (Grados + Tolerancia),  se desactiva la resistencia.
// De esta manera se mantiene la temperatura del horno en un rango seleccionado.
//
// Al correr el sistema se van guardando los datos en la EEPROM de manera que
// si el área se queda sin electricidad, se mostrarán los ultimos datos
// guardados en el inicio del sistema.
//
///////////////////////////////////////////////////////////////////////////////

#include <18F1220.h>
#device ADC=10
#use delay(internal=4MHz)
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOMCLR                   //Master Clear pin used for I/O
#FUSES PROTECT                  //Code protected from reads
#FUSES CPD                      //Data EEPROM Code Protected

#define LCD_DATA_PORT getenv("SFR:PORTB")
#define LCD_ENABLE_PIN PIN_B0
#define LCD_RS_PIN PIN_B1
#define LCD_RW_PIN PIN_A2
#include <lcd.c>

#bit BotonA     = 0xF80.4
#bit BotonB     = 0xF80.5
#bit PediluvioA = 0xF80.6
#bit PediluvioB = 0xF80.7
#bit Buzzer     = 0xF81.2

int16 L1,L2;
float AmperajeA=0,AmperajeB=0;
int C1=0,S1=0,M1=0;
int C2=0,S2=0,M2=0;
int1 OperandoA=0,OperandoB=0,ExcesoA=0,ExcesoB=0;

#int_timer0
void timer() 
{
   if(OperandoA)
   {
      set_timer0(58);
      if(C1==0)
      {
         C1=20;
         if(S1==0)
         {
            S1=60;
            if(M1==0)
            {
               M1=60;
            }
            M1--;
         }
         S1--;
         if(!S1 && !M1)
         {
            PediluvioA=0;
            OperandoA=0;
            lcd_gotoxy(1,1);
            printf(lcd_putc," FIN DE SESION");
         }
      }
      C1--;
   }
   if(OperandoB)
   {
      set_timer0(58);
      if(C2==0)
      {
         C2=20;
         if(S2==0)
         {
            S2=60;
            if(M2==0)
            {
               M2=60;
            }
            M2--;
         }
         S2--;
         if(!S2 && !M2)
         {
            PediluvioB=0;
            OperandoB=0;
            lcd_gotoxy(1,1);
            printf(lcd_putc," FIN DE SESION");
         }
      }
      C2--;
   }
}
void LeerTemp()
{
   set_adc_channel(0);
   delay_us(10);
   L1=read_adc();
   AmperajeA=(L1*5.0)/1024.0;
   set_adc_channel(1);
   delay_us(10);
   L2=read_adc();
   AmperajeB=(L2*5.0)/1024.0;
}
void Estado()
{
   if(!ExcesoA)
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"  %02u:%02u  %1.2fA"M1,S1,AmperajeA);
   }
   if(!ExcesoB)
   {
      lcd_gotoxy(1,2);
      printf(lcd_putc,"  %02u:%02u  %1.2fA"M2,S2,AmperajeB);
   }
}
void Operatividad()
{
   LeerTemp();
   Estado();
   if(OperandoA)
   {
      if(AmperajeA>=1.5)
      {
         PediluvioA=0;
         ExcesoA=1;
         lcd_gotoxy(1,1);
         printf(lcd_putc," EXCESO DE SAL");
      }
   }
   if(OperandoB)
   {
      if(AmperajeB>=1.5)
      {
         PediluvioB=0;
         ExcesoB=1;
         lcd_gotoxy(1,2);
         printf(lcd_putc,"EXCESO DE SAL");
      }
   }
}
void main()
{
   set_tris_a(0b00111111);
   set_tris_b(0x00);
   output_b(0x00);
   enable_interrupts(global|int_timer0);
   setup_timer_0(T0_INTERNAL|T0_DIV_256|T0_8_BIT);
   setup_adc_ports(sAN0|sAN1);
   setup_adc(ADC_CLOCK_DIV_2|ADC_TAD_MUL_0);
   set_timer0(58);
   lcd_init();
   PediluvioA=PediluvioB=1;
   while(TRUE)
   {
      Operatividad();
      if(!BotonA)     // Pediluvio A
      {
         while(!BotonA){delay_ms(200);}
         if(OperandoA==0)
         {
            OperandoA=1;
         }else{
            OperandoA=0;
         }
      }
      if(!BotonB)     // Pediluvio B
      {
         while(!BotonB){delay_ms(200);}
         if(OperandoB==0)
         {
            OperandoB=1;
         }else{
            OperandoB=0;
         }
      }
   }
}
