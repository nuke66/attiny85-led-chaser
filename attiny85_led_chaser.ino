/*
 * This version for the ATTiny85 - does not support serial!!!
 * 
 * Board: ATTiny25/45/85
 * Processor: ATTiny85
 * Clock: Internal 1 Mhz
 * Programmer: USBtinyISP
 * 
 * 
 * LED chaser built on the 74H595 shift register
 * 
 * Pin out for 74H595
 * 
 * ------+------------------------------
 * Pin     Description
 * ------+------------------------------
 * ST_CP |  Latch
 * SH_CP |  Clock
 * DS    |  Data Serial (in)
 * MR    |  Master reset (low enable)  => Tie to high (5V)
 * OE    |  Output Enable (low enable) => Tie to Gnd
 * Q7*   |  Serial Out (for chaining multiple chips)
 * ------+------------------------------
 * 
 * Daisy chaining (multiple 595 chips): 
 * -----------------------------------
 * 
 * - Output of serial out (Q7*) is connected to serial in (DS) of next chip 
 * - Common Clock and latch pin connection for all chips
 * - Remember decoupling capacitors on Vcc to Gnd (0.1uF)
 * 
 */

#define NUM_SR  1  // Number of shift registers   <==???? do I need this ???
#define NUM_LED 16  // Total number of LEDs

// ATTiny Arduino pins
#define SR_LATCH   0  // Connect to ST_CP (latch)
#define SR_DATA    1  // Connect to DS (Serial in) 
#define SR_CLOCK   2  // Connect to SH_CP (clock)

const int LED_MAX_VALUE = (1 << (NUM_LED)) - 1; // byte value of number of LEDs 

void setup() {
  //Setup SR output pins
  pinMode(SR_LATCH, OUTPUT);
  pinMode(SR_CLOCK, OUTPUT);
  pinMode(SR_DATA,  OUTPUT);
}

void loop() {
  sweep_bi_direction(100);
  sweep_bi_direction(100);
  sweep_bi_direction(100);
  sweep_bi_direction(100);
  sweep_bi_direction(100);
  sweep_bi_direction(100);

  one_in_two_delayed(1000);
  writeSR(0,200);
  
  sweep(80);
  sweep(80);
  writeSR(0,200);

  one_in_two(1000);
  writeSR(0,200);
  
  sweep_and_stack(50,false);
  sweep_and_stack(50,true);
  writeSR(0,100);

  one_in_three(200);
  writeSR(0,200);
  
  blink_all(500);
  writeSR(0, 200);
  
  //binary_count(1);
}

/*   
 * Write out to a 74HC595 shift register 
 */
void writeSR(byte value, int nDelay)
{
  // if (bReverse) value = ~value;

  digitalWrite(SR_LATCH, LOW);      // Set latch pin low
  shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, value); // Shift out the byte value
  digitalWrite(SR_LATCH, HIGH);     // Set latch pin high 
  
  if (nDelay != 0) delay(nDelay);
}

/*   
 * Write out to a 74HC595 shift register 
 */
void writeSR(byte value, int nDelay, bool bReverse)
{
  digitalWrite(SR_LATCH, LOW);      // Set latch pin low

  if (!bReverse)
     shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, value);
  else
     shiftOut(SR_DATA, SR_CLOCK, LSBFIRST, value);
     
  digitalWrite(SR_LATCH, HIGH);     // Set latch pin high 
  
  if (nDelay != 0) delay(nDelay);
}



/*
 * Sweep sequence, forward and back again
 */
void sweep(int nDelay)
{
    for(int n = 0; n < NUM_LED; n++) 
    {
      writeSR((1 << n), nDelay);
    }

    for( int n = (NUM_LED-1); n >= 0 ; n--) 
    {
      writeSR((1 << n), nDelay);
    }
}

/*
 * Display binary count, counting up from 0 to LED_NUM-1
 */
void binary_count(int nDelay)
{
  for (byte n = 0; n < LED_MAX_VALUE; n++) {
    writeSR(n, 100);
  }
}

/*
 * Cycle through every 1 in 2 LEDs 
 */
void one_in_two(int nDelay)
{
  int count = 0; 
  int res = 0; 
  
  for (int temp = NUM_LED; temp > 0; temp--) {
    
    // if bit is odd, then generate 
    // number and or with res 
    if (count % 2 == 0) 
      res |= (1 << count); 
        
    count++; 
  }

 
  writeSR(res, nDelay);
  writeSR(res<<1, nDelay);
  writeSR(res, nDelay);
  writeSR(res<<1, nDelay);
  writeSR(0,500);
}


/*
 * Cycle through every 1 in 2 LEDs but with a gap between transitions 
 */
void one_in_two_delayed(int nDelay)
{
  int count = 0; 
  int res = 0; 
  
  for (int temp = NUM_LED; temp > 0; temp--) {
    
    // if bit is odd, then generate 
    // number and or with res 
    if (count % 2 == 0) 
      res |= (1 << count); 
        
    count++; 
  }

  writeSR(res, nDelay);
  writeSR(0,500);
  writeSR(res<<1, nDelay);
  writeSR(0,500);
  writeSR(res, nDelay);
  writeSR(0,500);
  writeSR(res<<1, nDelay);
  writeSR(0,500);
  
  writeSR(res, nDelay);
  writeSR(res<<1, nDelay);
  writeSR(res, nDelay);
  writeSR(res<<1, nDelay);
  writeSR(0,500);
}

 /*
  * Cycle through every 1 in 3 LEDs
  */
void one_in_three(int nDelay)
{
  int count = 0; 
  
  // res for store 010101.. number 
  int res = 0; 
  
  for (int temp = NUM_LED; temp > 0; temp--) {
    
    // if bit is odd, then generate 
    // number and or with res 
    if (count % 3 == 0) 
      res |= (1 << count); 
        
    count++; 
  }

  for(int i=1;i<=8;i++)
  {
    writeSR(res, nDelay);
    writeSR(res<<1, nDelay);
    writeSR(res<<2, nDelay);
  }  
}

/*
 * Blink all LEDs on and off
 */
void blink_all(int nDelay)
{ 
  for(int n=0;n<4;n++){
    writeSR(LED_MAX_VALUE,nDelay);
    writeSR(0,nDelay);
  }
}


/*
 * Sweep with LED stacking 
 */
 void sweep_and_stack(int nDelay,bool bReverse)
 {
    int curStack = 0;   // holds which LEDs are currently in the stack

    // track which LED position that will be 'stacked' next
    for(int stackPos = NUM_LED-1; stackPos > 0; stackPos--)
    {
        // sweep towards stack position
        for(int n = 0; n < stackPos; n++) 
        {
            writeSR((1 << n)|curStack, nDelay, bReverse);
        }

        // capture which LED has been added to the stack
        curStack |= (1 << stackPos);

        for( int n = (stackPos-1); n >= 0 ; n--) 
        {
            writeSR((1 << n)|curStack, nDelay, bReverse);
        }
    }
}

/*
 *  Sweep from both ends at the same time.  Sweep LEDs pass each other.
 */
void sweep_bi_direction(int nDelay)
{
     int n = 0;            // increasing sweep position
     int i = (NUM_LED)-1;  // decreasing sweep position
  
     for(; n < NUM_LED;) 
     {
       writeSR((1 << n)|(1<< i), nDelay); // OR both sweeps together to get one valuek
       n++; 
       i--;
     }
}

/*
 * from middle point sweep in both directions (left and right from middle) to end.  Stack when you hit the end.
 */
//void sweep_and_stack_middle()
//{
  
//}

/*
 * snake, basically the same as sweep() but using more than one LED
 */
// void snake()
// {
  
// }
