/*  ArduinoIO - An open source program to allow an Arduino to be used as
 *  a generic I/O module.
 *  Copyright (c) 2010 Phil Birkelbach
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* This is configuration stuff that should be changed for different
 * sized Arduinos */
#define HALF_DUPLEX    // undefine this if using full duplex point to point

int const BUFF_SIZE   = 12;   // Size of the receive buffer
int const BAUDRATE    = 9600;
char address[2] = {'A', 'A'};  // Two character hex address

/* If a pin number is not in this array it is not considered a valid
   pin on the Arduino.  This would changed for different Arduino's */

#ifdef HALF_DUPLEX
  int const NUM_PINS = 17;
  byte PinNumbers[] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
#else
  int const NUM_PINS = 18;
  byte PinNumbers[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
#endif
byte PinConfig[NUM_PINS];

/* These correspond to A0, A1, A2 .... etc */
int const NUM_ANALOG_PINS = 6;
byte AllowedAIPins[] = {14,15,16,17,18,19};
//byte AnalogIO[NUM_ANALOG_PINS];

/* These are the pin numbers that can be used as PWM Outputs */
int const NUM_PWM_PINS = 6;
byte AllowedPWMPins[] = {3,5,6,9,10,11};
//byte PWMIO[NUM_PWM_PINS];

/* These are flags defs for PinConfig[] */
int const BIT_DIGITAL = 0; // First four bits are type
int const BIT_AI      = 1;
int const BIT_PWM     = 2;
int const BIT_LAST    = 4; // Last read value
int const BIT_DIR     = 5; // Digital I/O Direction 1 = Input
int const BIT_PUR     = 6; // Pull Up Resistor Configure
int const BIT_NOTIFY  = 7;

byte buff[BUFF_SIZE];  // Serial receive buffer
byte bptr;             // buffer pointer
byte next;             // index used for parsing commands

void setup() 
{ 
#ifdef HALF_DUPLEX
    pinMode(2, OUTPUT);
#endif
    Serial.begin(BAUDRATE); 
    
}

void loop() 
{ 
    if(Serial.available()) {
        buff[bptr] = Serial.read();
        if(buff[bptr] == '\r') {
            next = 1;
            parse_command();
            //Serial.print("\n"); //Just for testing.  Should be removed.
            bptr = 0;
        } else {
            bptr++;
            if(buff[0] != '$' || bptr > BUFF_SIZE) {
                bptr = 0;
            }
        }
    }
}

void parse_command()
{
  byte n;
  // First convert to all uppercase
  for(n=0; n < BUFF_SIZE && buff[n] != '\r'; n++) {
    if(buff[n] >= 'a' && buff[n] <= 'z')
      buff[n] &= ~(0x20);
  }
  /* Check to see if it's us */
  if(buff[next] != address[0] || buff[next+1] != address[1])
      return;
  if(buff[next+2] != '&') {
      respond("E1");
      return;
  }
  next = 4;
  if(buff[next]=='C') {
      if(buff[next+1]=='D')      config_digital();
      else if(buff[next+1]=='A') config_analog();
      else if(buff[next+1]=='S') config_spi();
      else if(buff[next+1]=='M') config_module();
  } else if(buff[next]=='R') {
      if(buff[next+1]=='D')      read_digital();
      else if(buff[next+1]=='A') read_analog();
      else if(buff[next+1]=='S') read_spi();
  } else if(buff[next]=='W') {
      if(buff[next+1]=='D')      write_digital();
      else if(buff[next+1]=='A') write_analog();
      else if(buff[next+1]=='S') write_spi();
  } else {
      respond("E1");
  }
}

void respond(char *s)
{
#ifdef HALF_DUPLEX
  digitalWrite(2, 1);
#endif
  Serial.print("%");
  Serial.print(address[0]);
  Serial.print(address[1]);
  Serial.print("&");
  Serial.print(s);
  Serial.print("\r");
#ifdef HALF_DUPLEX
/* This delay is required because we have to wait until the byte has been fully sent
 * before we turn the RS485 enable/disable pin off */
  delay(3);  //TODO: Change to actually watch the register in the serial port 
  digitalWrite(2, 0);
#endif
}

void config_digital()
{
  byte pin,n;
  char set, arg;
  pin = get_pin_number(next+2);
  for(n=0; n<NUM_PINS && PinNumbers[n]!=pin; n++) ;
  if(n==NUM_PINS) { /* If it's not in the array it's not a valid pin number */
      respond("E3");
  } else {
      set = buff[next];
      arg = buff[next +1];
      if(set == 'I') {
          PinConfig[n] &= 0xF0; // Reset the type
          bitSet(PinConfig[n], BIT_DIGITAL);
          bitSet(PinConfig[n], BIT_DIR); 
          pinMode(pin, INPUT);
          respond("OK");
          //Serial.print("OK-Set to Input pin = ");
      } else if(set == 'O') {
          PinConfig[n] &= 0xF0; // Reset the type
          bitSet(PinConfig[n], BIT_DIGITAL);
          bitClear(PinConfig[n], BIT_DIR);
          pinMode(pin, OUTPUT);
          respond("OK");
          //Serial.print("OK-Set to Output pin = ");
      } else if(set == 'R') {
          if(bitRead(PinConfig[n], BIT_DIGITAL) == 0 ||
             bitRead(PinConfig[n], BIT_DIR) == 0    ) {
                respond("E3");
                return;
          }
          if(arg == '1')
              digitalWrite(pin, 1);
          else if(arg == '0')
              digitalWrite(pin, 0);
          else {
              respond("E1");
              return;
          }
          respond("OK");
      } else if(set == 'N') {
          respond("E0");  
          return;
      } else {
          respond("E1");
          return;
      }
    }
}

void config_analog()
{
  respond("E0");    
}

void config_spi()
{
  respond("E0");    
}

void config_module()
{
  respond("E0");    
}

void read_digital()
{
    byte pin,n;

    pin = get_pin_number(next+2);
    for(n=0; n<NUM_PINS && PinNumbers[n]!=pin; n++) ;
    if(n==NUM_PINS) { /* If it's not in the array it's not a valid pin number */
        respond("E3");
    } else {
        if(bitRead(PinConfig[n], BIT_DIGITAL) == 0 ||
           bitRead(PinConfig[n], BIT_DIR) == 0    ) {
              respond("E3");
              return;
        }
        if(digitalRead(pin)) respond("1");
        else                 respond("0");
    }
}

void read_analog()
{
  respond("E0");    
}

void read_spi()
{
  respond("E0");    
}

void write_digital()
{
    byte pin,n;
    char arg;
    
    pin = get_pin_number(next+2);
    for(n=0; n<NUM_PINS && PinNumbers[n]!=pin; n++) ;
    if(n==NUM_PINS) { /* If it's not in the array it's not a valid pin number */
        respond("E3");
    } else {
        if(bitRead(PinConfig[n], BIT_DIGITAL) == 0 ||
           bitRead(PinConfig[n], BIT_DIR) != 0    ) {
              respond("E3");
              return;
        }
        if(buff[next] != ',') {
            respond("E1");
            return;
        }
        arg = buff[next+1];
        if(arg == '1') {
            digitalWrite(pin, 1);
            respond("OK");
        } else if(arg == '0') {
            digitalWrite(pin, 0);
            respond("OK");
        } else {
            respond("E1");
            return;
        }
    }
}

void write_analog()
{
  respond("E0");    
}

void write_spi()
{
  respond("E0");    
}

/* The index points to the place in the global buff[] where we want to
 * find a two digit number.  Set's the global 'next' to the next index
 * in buff that has data */
byte get_pin_number(byte index)
{
  byte digits = 0;
  
  if(buff[index] >= '0' && buff[index] <= '9') digits++;
  if(buff[index+1] >= '0' && buff[index+1] <= '9') digits++;
  
  if(digits == 1) {
    next = index+1;
    return buff[index] - '0';
  } else if(digits == 2) {
    next = index + 2;
    return (buff[index] - '0')*10 + (buff[index+1] - '0');
  }
  return 0xFF;
}
