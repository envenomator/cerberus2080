/*********************************************/
/**         CERBERUS 2080's BIOS code       **/
/**      Brought to you by The Byte Attic   **/
/**        For the ATmega328p-PU (CAT)      **/
/**     To be compiled in the arduino IDE   **/
/**     (use Arduino Uno as target board)   **/
/** Copyright 2020-2021 by Bernardo Kastrup **/
/**            All rights reserved          **/
/**      Code distributed under license     **/
/*********************************************/

/** Provided AS-IS, without guarantees of any kind                        **/
/** This is NOT a commercial product as has not been exhaustively tested  **/

/** The directive "F()", which tells the compiler to put strings in code memory **/
/** instead of dynamic memory, is used as often as possible to save dynamic     **/
/** memory, the latter being the bottleneck of this code.                       **/

/** Updated By:		Dean Belfield	**/
/** Created:		31/07/2021		**/
/** Last Updated:	23/11/2021 		**/

//	Modinfo:
//	10/08/2021:	All recognised keystrokes sent to mailbox, F12 now returns to BIOS, 50hz NMI starts when code runs
//	11/08/2021:	Memory map now defined in configs, moved code start to 0x0205 to accomodate inbox
//	12/08/2021:	Refactored load, save and delFile. Now handles incoming messages from Z80/6502
//	21/08/2021:	Tweaks for sound command, bug fixes in incoming message handler
//  06/10/2021: Tweaks for cat command
//	23/11/2021:	Moved PS2Keyboard library from Arduino library to src subdirectory

/** These libraries are built into the arduino IDE  **/

#include <SPI.h>
#include <SD.h>
#include <TimerOne.h>
#include <EEPROM.h>

/** For more information about this PS2Keyboard library:    **/
/** http://www.arduino.cc/playground/Main/PS2Keyboard       **/
/** http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html     **/
/** Note that the Arduino managed library is not the latest **/
/** Install from the GitHub project in order to build       **/

#include <PS2KeyAdvanced.h>
#include <PS2KeyMap.h>


/** Compilation defaults **/
#define config_silent 1				// Turn off the startup jingle
#define config_enable_nmi 1			// Turn on the 50hz NMI timer when CPU is running. If set to 0 will only trigger an NMI on keypress
//#define config_default_cpu 0		// 0: 6502, 1: Z80
//#define config_default_speed 1		// 0: 4mhz, 1: 8mhz
#define config_counter_nmi 0xff   // counter addresss - counting up during nmi
#define config_outbox_flag 0x0200	// Outbox flag memory location (byte)
#define config_outbox_data 0x0201	// Outbox data memory location (byte)
#define config_inbox_flag 0x0202	// Inbox flag memory location (byte)
#define config_inbox_data 0x0203	// Inbox data memory location (word)
#define config_code_start 0x0205	// Start location of code

#define ADDR_EEPROM_MODE    0
#define ADDR_EEPROM_SPEED   1

/** Pinout below defined as per Arduino Uno pin IDs **/
/** The next pins go to SPACER **/
#define SI A5     /** Serial Input, pin 28 on CAT **/
#define SO A4     /** Serial Output, pin 27 on CAT **/
#define SC A3     /** Shift Clock, pin 26 on CAT **/
#define AOE A2    /** Address Output Enable, pin 25 on CAT **/
#define RW A1     /** Memory Read/!Write, pin 24 on CAT **/
#define LD A0     /** Latch Data, pin 23 on CAT **/
#define CPUSLC 5  /** CPU SeLeCt, pin 11 on CAT **/
#define CPUIRQ 6  /** CPU Interrupt ReQuest, pin 12 on CAT **/
#define CPUGO 7   /** CPU Go/!Halt, pin 13 on CAT **/
#define CPURST 8  /** CPU ReSeT, pin 14 on CAT **/
#define CPUSPD 9  /** CPU SPeeD, pin 15 on CAT **/
/** The next pins go to I/O devices **/
#define KCLK 2    /** CLK pin connected to PS/2 keyboard (CAT pin 4) **/
#define KDAT 3    /** DATA pin connected to PS/2 keyboard (CAT pin 5) **/
#define SOUND 4   /** Sound output to buzzer, pin 6 on CAT **/
#define CS 10     /** Chip Select for SD Card, pin 16 on CAT **/
/** MISO, MOSI and SCK for SD Card are hardwired in CAT: **/
/** CLK  -> pin 19 on CAT **/
/** MISO -> pin 18 on CAT **/
/** MOSI -> pin 17 on CAT **/

/** Now some stuff required by the libraries in use **/
const int chipSelect = CS;
const int DataPin = KDAT;
const int IRQpin = KCLK;

/* Status constants */
#define STATUS_DEFAULT 0
#define STATUS_BOOT 1
#define STATUS_READY 2
#define STATUS_UNKNOWN_COMMAND 3
#define STATUS_NO_FILE 4
#define STATUS_CANNOT_OPEN 5  
#define STATUS_MISSING_OPERAND 6
#define STATUS_SCROLL_PROMPT 7
#define STATUS_FILE_EXISTS 8
#define STATUS_ADDRESS_ERROR 9
#define STATUS_POWER 10
#define STATUS_EOF 11

/** Next is the string in CAT's internal memory containing the edit line, **/
/** intialized in startup.                              **/
volatile char editLine[38];
volatile char previousEditLine[38];

/** The above is self-explanatory: it allows for repeating previous command **/
volatile byte pos = 1;						/** Position in edit line currently occupied by cursor **/
volatile bool mode;
volatile bool cpurunning = false;			/** true = CPU is running, CAT should not use the buses **/
volatile bool interruptFlag = false;		/** true = Triggered by interrupt **/
volatile bool fast;           /** true = 8 MHz CPU clock, false = 4 MHz CPU clock **/
volatile File cd;							/** Used by BASIC directory commands **/

volatile bool readfromserial = false;
volatile bool exitrequested = false;

void(* resetFunc) (void) = 0;       		/** Software reset fuction at address 0 **/

PS2KeyAdvanced keyboard;
PS2KeyMap keymap;

void setup() {
    mode = EEPROM.read(ADDR_EEPROM_MODE);
    if((mode != 0) && (mode != 1))
    {
      mode = 0;
      EEPROM.write(ADDR_EEPROM_MODE,0);
    }
    fast = EEPROM.read(ADDR_EEPROM_SPEED);
    if((fast != 0) && (fast != 1))
    {
      fast = 0;
      EEPROM.write(ADDR_EEPROM_SPEED,0);
    }
	/** First, declaring all the pins **/
  	pinMode(SO, OUTPUT);
  	pinMode(SI, INPUT);               /** There will be pull-up and pull-down resistors in circuit **/
  	pinMode(SC, OUTPUT);
  	pinMode(AOE, OUTPUT);
  	pinMode(LD, OUTPUT);
  	pinMode(RW, OUTPUT);
  	pinMode(CPUSPD, OUTPUT);
  	pinMode(KCLK, INPUT_PULLUP);      /** But here we need CAT's internal pull-up resistor **/
  	pinMode(KDAT, INPUT_PULLUP);      /** And here too **/
  	pinMode(CPUSLC, OUTPUT);
  	pinMode(CPUIRQ, OUTPUT);
  	pinMode(CPUGO, OUTPUT);
  	pinMode(CPURST, OUTPUT);
  	pinMode(SOUND, OUTPUT);
  	/** Writing default values to some of the output pins **/
  	digitalWrite(RW, HIGH);
  	digitalWrite(SO, LOW);
  	digitalWrite(AOE, LOW);
  	digitalWrite(LD, LOW);
  	digitalWrite(SC, LOW);
  	digitalWrite(CPUSPD, fast);
  	digitalWrite(CPUSLC, mode);
  	digitalWrite(CPUIRQ, LOW);
  	digitalWrite(CPUGO, LOW);
  	digitalWrite(CPURST, LOW);
  	/** Now reset the CPUs **/
  	resetCPUs();
  	/** Clear edit line **/
  	clearEditLine();
  	storePreviousLine();
  	Serial.begin(115200);
	/** Initialize keyboard library **/
  	keyboard.begin(DataPin, IRQpin);
    keyboard.resetKey();          /** essential for modern USB/PS2 keyboards **/
    keyboard.setNoBreak( 1 );
    // and set no repeat on CTRL, ALT, SHIFT, GUI while outputting
    keyboard.setNoRepeat( 1 );
  	/** Now access uSD card and load character definitions so we can put something on the screen **/
  	if (!SD.begin(chipSelect)) {
    	/** SD Card has either failed or is not present **/
    	/** Since the character definitions thus can't be uploaded, accuse error with repeated tone and hang **/
    	while(true) {
      		tone(SOUND, 50, 150);
      		delay(500);
    	}
  	}
  	/** Load character defs into memory **/
  	if(load("chardefs.bin", 0xf000) != STATUS_READY) {
		tone(SOUND, 50, 150);
	  }
  	/**********************************************************/
  	/** Now prepare the screen **/
  	ccls();
  	cprintFrames();
  	/** Load the CERBERUS icon image on the screen ************/
  	int inChar;
  	if(!SD.exists("cerbicon.img")) {
		tone(SOUND, 50, 150); /** Tone out an error if file is not available **/
	}
  	else {
    	File dataFile2 = SD.open("cerbicon.img"); /** Open the image file **/
    	if (!dataFile2) {
			tone(SOUND, 50, 150);     /** Tone out an error if file can't be opened  **/
		}
    	else {
      		for (byte y = 2; y <= 25; y++) {
        		for (byte x = 2; x <= 39; x++) {
	          		String tokenText = "";
          			while (isDigit(inChar = dataFile2.read())) {
						tokenText += char(inChar);
					}
          			cprintChar(x, y, tokenText.toInt());
        		}
			}
      		dataFile2.close();
    	}
  	}
  	/**********************************************************/
  	cprintStatus(STATUS_BOOT);
  	/** Play a little jingle while keyboard finishes initializing **/
  	#if config_silent == 0
  	playJingle();
  	#endif
  	delay(1000);
  	cprintStatus(STATUS_DEFAULT);
  	cprintEditLine();
}

char cerberusKBMap(uint16_t code)
{
  char ascii = keymap.remapKey(code);

  switch(code & 0xFF) // lower byte contains the KEY code, ignore status byte
  {
    // Transform these specific keys back to ascii code, regardless of keyboard layout
    case PS2_KEY_UP_ARROW:
      return 0x0B;
    case PS2_KEY_DN_ARROW:
      return 0x0A;
    case PS2_KEY_R_ARROW:
      return 0x15;
    case PS2_KEY_L_ARROW:
      return 0x08;
    case PS2_KEY_INSERT:
      return 0x02;
    case PS2_KEY_PGUP:
      return 0x19;
    case PS2_KEY_PGDN:
      return 0x1A;
    case PS2_KEY_BS:
    case PS2_KEY_BACK:
      return 0x7F;
    case PS2_KEY_F12:
      return 0x01;
    default:
      break;
  }
  return ascii;
}

void processKeyAction(uint16_t code, char ascii)
{
  byte i;   /** Just a counter **/
  
  if(cpurunning)
  {
    if((code & 0xFF) == PS2_KEY_F12) stopCode();
    else
    {
      if(ascii > 0)
      {
        cpurunning = false;           /** Just stops interrupts from happening **/
        digitalWrite(CPUGO, LOW);         /** Pause the CPU and tristate its buses to high-Z **/
        byte mode = cpeek(config_outbox_flag);
        cpoke(config_outbox_data, ascii);       /** Put token code of pressed key in the CPU's mailbox, at config_outbox_data **/
        cpoke(config_outbox_flag, 0x01);    /** Flag that there is new mail for the CPU waiting at the mailbox **/
        digitalWrite(CPUGO, HIGH);        /** Let the CPU go **/
        cpurunning = true;
        #if config_enable_nmi == 0
        digitalWrite(CPUIRQ, HIGH); /** Trigger an interrupt **/
        digitalWrite(CPUIRQ, LOW);
        #endif          
      }
    }
  }
  else
  {
    switch(code & 0xFF)
    {
      case PS2_KEY_ENTER:
        enter();
        break;
      case PS2_KEY_UP_ARROW:
        for (i = 0; i < 38; i++) editLine[i] = previousEditLine[i];
        i = 0;
        while (editLine[i] != 0) i++;
        pos = i;
        cprintEditLine();
        break;
      case PS2_KEY_DN_ARROW:
        clearEditLine();
        break;
      case PS2_KEY_BS:
      case PS2_KEY_DELETE:
      case PS2_KEY_L_ARROW:
        editLine[pos] = 32; /** Put an empty space in current cursor position **/
        if (pos > 1) pos--; /** Update cursor position, unless reached left-most position already **/
        editLine[pos] = 0;  /** Put cursor on updated position **/
        cprintEditLine();   /** Print the updated edit line **/
        break;
      case PS2_KEY_R_ARROW: /** ignore these codes in BIOS mode**/
      case PS2_KEY_ESC:
      case PS2_KEY_F12:
      case PS2_KEY_TAB:
      case PS2_KEY_INSERT:
      case PS2_KEY_PGUP:
      case PS2_KEY_PGDN:
        break;
      default:
        if(ascii > 0)
        {
          if(ascii == 0x0d) enter();  // enter by serial ascii, no special control character
          else
          {
            editLine[pos] = ascii;  /** Put new character in current cursor position **/
            if (pos < 37) pos++;    /** Update cursor position **/
            editLine[pos] = 0;      /** Place cursor to the right of new character **/
            if (!readfromserial)
            {
              cprintEditLine();
            }
          }
        }
        break;        
     }   
  }
}

// The main loop
//
void loop() {
  uint16_t code = 0;
  char ascii;

  if(keyboard.available())
  {
    code = keyboard.read();
    ascii = cerberusKBMap(code);
    if((ascii > 0x1a) && (ascii < 0xa8))
    {
      tone(SOUND, 750, 5);              /** Clicking sound for auditive feedback to key presses **/
    }
    if(!readfromserial)
    {
      if(!cpurunning) {
        cprintStatus(STATUS_DEFAULT); /** Update status bar **/ 
      }
    }
    processKeyAction(code, ascii); 
    readfromserial = false;
  }
  else if(Serial.available()) {
    ascii = Serial.read();
    processKeyAction(code, ascii);
    readfromserial = true;
  }      
  if(interruptFlag) {           /** If the interrupt flag is set then **/
    interruptFlag = false;
    messageHandler();         /** Run the inbox message handler **/
  }
  if(exitrequested)
  {
    exitrequested = false;
    stopCode();
  }
}


// CPU Interrupt Routine (50hz)
//
void cpuInterrupt(void) {
  	if(cpurunning) {							// Only run this code if cpu is running 
	   	digitalWrite(CPUIRQ, HIGH);		 		// Trigger an NMI interrupt
	   	digitalWrite(CPUIRQ, LOW);
  	}
	interruptFlag = true;
}

// Inbox message handler
//
void messageHandler(void) {
  	int	flag, status;
  	byte retVal = 0x00;							// Return status; default is OK
  	unsigned int address;						// Pointer for data

 	if(cpurunning) {							// Only run this code if cpu is running 
	 	cpurunning = false;						// Just to prevent interrupts from happening
		digitalWrite(CPUGO, LOW); 				// Pause the CPU and tristate its buses to high-Z
		flag = cpeek(config_inbox_flag);		// Fetch the inbox flag 
		if(flag > 0 && flag < 0x80) {
			address = cpeekW(config_inbox_data);
			switch(flag) {
				case 0x01:
					cmdSound(address);
					break;
				case 0x02: 
					status = cmdLoad(address);
					if(status != STATUS_READY) {
						retVal = (byte)(status + 0x80);
					}
					break;
				case 0x03:
					status = cmdSave(address);
					if(status != STATUS_READY) {
						retVal = (byte)(status + 0x80);
					}
					break;
				case 0x04:
					status = cmdDelFile(address);
					if(status != STATUS_READY) {
						retVal = (byte)(status + 0x80);
					}
					break;
				case 0x05:
					status = cmdCatOpen(address);
					if(status != STATUS_READY) {
						retVal = (byte)(status + 0x80);
					}
					break;
				case 0x06:
					status = cmdCatEntry(address);
					if(status != STATUS_READY) {
						retVal = (byte)(status + 0x80);
					}
					break;
				case 0x7F:
          exitrequested = true;
					break;
			}
			cpoke(config_inbox_flag, retVal);	// Flag we're done - values >= 0x80 are error codes
		}
		digitalWrite(CPUGO, HIGH);   			// Restart the CPU 
		cpurunning = true;
 	}
}

// Handle SOUND command from BASIC
//
void cmdSound(unsigned int address) {
	unsigned int frequency = cpeekW(address);
	unsigned int duration = cpeekW(address + 2) * 50;
	tone(SOUND, frequency, duration);
	delay(duration);
}

// Handle ERASE command from BASIC
//
int cmdDelFile(unsigned int address) {
	cpeekStr(address, editLine, 38);
	return delFile((char *)editLine);	
}

// Handle LOAD command from BASIC
//
int cmdLoad(unsigned int address) {
	unsigned int startAddr = cpeekW(address);
	unsigned int length = cpeekW(address + 2);
	cpeekStr(address + 4, editLine, 38);
	return load((char *)editLine, startAddr);
}

// Handle SAVE command from BASIC
//
int cmdSave(unsigned int address) {
	unsigned int startAddr = cpeekW(address);
	unsigned int length = cpeekW(address + 2);
	cpeekStr(address + 4, editLine, 38);
	return save((char *)editLine, startAddr, startAddr + length - 1);
}

// Handle CAT command from BASIC
//
int cmdCatOpen(unsigned int address) {
	cd = SD.open("/");						// Start the process first by opening the directory
	return STATUS_READY;
}

int cmdCatEntry(unsigned int address) {		// Subsequent calls to this will read the directory entries
	File entry;
	entry = cd.openNextFile();				// Open the next file
	if(!entry) {							// If we've read past the last file in the directory
		cd.close();							// Then close the directory
		return STATUS_EOF;					// And return end of file
	}
	cpokeL(address, entry.size());			// First four bytes are the length
	cpokeStr(address + 4, entry.name());	// Followed by the filename, zero terminated
	entry.close();							// Close the directory entry
	return STATUS_READY;					// Return READY
}

/************************************************************************************************/
void enter() {  /** Called when the user presses ENTER, unless a CPU program is being executed **/
/************************************************************************************************/
  unsigned int addr;                /** Memory addresses **/
  byte data;                        /** A byte to be stored in memory **/
  byte i;                           /** Just a counter **/
  String nextWord, nextNextWord, nextNextNextWord; /** General-purpose strings **/
  nextWord = getNextWord(true);     /** Get the first word in the edit line **/
  nextWord.toLowerCase();           /** Ignore capitals **/
  if( nextWord.length() == 0 ) {    /** Ignore empty line **/
    Serial.println(F("OK"));
    return;
  }
  /** MANUAL ENTRY OF OPCODES AND DATA INTO MEMORY *******************************************/
  if ((nextWord.charAt(0) == '0') && (nextWord.charAt(1) == 'x')) { /** The user is entering data into memory **/
    nextWord.remove(0,2);                       /** Removes the "0x" at the beginning of the string to keep only a HEX number **/
    addr = strtol(nextWord.c_str(), NULL, 16);  /** Converts to HEX number type **/
    nextNextWord = getNextWord(false);          /** Get next byte **/
    byte chkA = 1;
    byte chkB = 0;
    while (nextNextWord != "") {                /** For as long as user has typed in a byte, store it **/
      if(nextNextWord.charAt(0) != '#') {
        data = strtol(nextNextWord.c_str(), NULL, 16);/** Converts to HEX number type **/
        while( cpeek(addr) != data ) {          /** Serial comms may cause writes to be missed?? **/
          cpoke(addr, data);
        }
        chkA += data;
        chkB += chkA;
        addr++;
      }
      else {
        nextNextWord.remove(0,1);
        addr = strtol(nextNextWord.c_str(), NULL, 16);
        if( addr != ((chkA << 8) | chkB) ) {
          cprintString(28, 26, nextWord);
          tone(SOUND, 50, 50);
        }
      }
      nextNextWord = getNextWord(false);  /** Get next byte **/
    }
    if(!readfromserial)
    {
      cprintStatus(STATUS_READY);
      cprintString(28, 27, nextWord);
    }
    Serial.print(nextWord);
    Serial.print(' ');
    Serial.println((uint16_t)((chkA << 8) | chkB), HEX);
    
  /** LIST ***********************************************************************************/
  } else if (nextWord == F("list")) {     /** Lists contents of memory in compact format **/
    cls();
    nextWord = getNextWord(false);        /** Get address **/
    list(nextWord);
    cprintStatus(STATUS_READY);
  /** CLS ************************************************************************************/
  } else if (nextWord == F("cls")) {      /** Clear the main window **/
    cls();
    cprintStatus(STATUS_READY);
  /** TESTMEM ********************************************************************************/
  } else if (nextWord == F("testmem")) {  /** Checks whether all four memories can be written to and read from **/
    cls();
    testMem();
    cprintStatus(STATUS_READY);
  /** 6502 ***********************************************************************************/
  } else if (nextWord == F("6502")) {     /** Switches to 6502 mode **/
    mode = false;
    EEPROM.write(0,0);
    digitalWrite(CPUSLC, LOW);            /** Tell CAT of the new mode **/
    cprintStatus(STATUS_READY);
  /** Z80 ***********************************************************************************/
  } else if (nextWord == F("z80")) {      /** Switches to Z80 mode **/
    mode = true;
    EEPROM.write(0,1);
    digitalWrite(CPUSLC, HIGH);           /** Tell CAT of the new mode **/
    cprintStatus(STATUS_READY);
  /** RESET *********************************************************************************/
  } else if (nextWord == F("reset")) {
    resetFunc();						  /** This resets CAT and, therefore, the CPUs too **/
  /** FAST **********************************************************************************/
  } else if (nextWord == F("fast")) {     /** Sets CPU clock at 8 MHz **/
    digitalWrite(CPUSPD, HIGH);
    fast = true;
    EEPROM.write(ADDR_EEPROM_SPEED,1);
    cprintStatus(STATUS_READY);
  /** SLOW **********************************************************************************/
  } else if (nextWord == F("slow")) {     /** Sets CPU clock at 4 MHz **/
    digitalWrite(CPUSPD, LOW);
    fast = false;
    EEPROM.write(ADDR_EEPROM_SPEED,0);
    cprintStatus(STATUS_READY);
  /** DIR ***********************************************************************************/
  } else if (nextWord == F("dir")) {      /** Lists files on uSD card **/
    dir();
  /** DEL ***********************************************************************************/
  } else if (nextWord == F("del")) {      /** Deletes a file on uSD card **/
    nextWord = getNextWord(false);
    catDelFile(nextWord);
  /** LOAD **********************************************************************************/
  } else if (nextWord == F("load")) {     /** Loads a binary file into memory, at specified location **/
    nextWord = getNextWord(false);        /** Get the file name from the edit line **/
    nextNextWord = getNextWord(false);    /** Get memory address **/
    catLoad(nextWord, nextNextWord, false);
  /** RUN ***********************************************************************************/
  } else if ((nextWord == F("run")) || (nextWord == F("r"))) {      /** Runs the code in memory **/
    for (i = 0; i < 38; i++) previousEditLine[i] = editLine[i]; /** Store edit line just executed **/
    runCode();
  /** SAVE **********************************************************************************/
  } else if (nextWord == F("save")) {
    nextWord = getNextWord(false);						/** Start start address **/
    nextNextWord = getNextWord(false);					/** End address **/
    nextNextNextWord = getNextWord(false);				/** Filename **/
    catSave(nextNextNextWord, nextWord, nextNextWord);
  /** MOVE **********************************************************************************/
  } else if (nextWord == F("move")) {
    nextWord = getNextWord(false);
    nextNextWord = getNextWord(false);
    nextNextNextWord = getNextWord(false);
    binMove(nextWord, nextNextWord, nextNextNextWord);
  /** HELP **********************************************************************************/
  } else if ((nextWord == F("help")) || (nextWord == F("?"))) {
    help();
    cprintStatus(STATUS_POWER);
  /** ALL OTHER CASES ***********************************************************************/
  } else cprintStatus(STATUS_UNKNOWN_COMMAND);
  if (!cpurunning) {
    storePreviousLine();
    clearEditLine();                   /** Reset edit line **/
  }
}

String getNextWord(bool fromTheBeginning) {
  /** A very simple parser that returns the next word in the edit line **/
  static byte initialPosition;    /** Start parsing from this point in the edit line **/
  byte i, j, k;                   /** General-purpose indices **/
  if (fromTheBeginning) initialPosition = 1; /** Starting from the beginning of the edit line **/
  i = initialPosition;            /** Otherwise, continuing on from where we left off in previous call **/
  while ((editLine[i] == 32) || (editLine[i] == 44)) i++; /** Ignore leading spaces or commas **/
  j = i + 1;                      /** Now start indexing the next word proper **/
  /** Find the end of the word, marked either by a space, a comma or the cursor **/
  while ((editLine[j] != 32) && (editLine[j] != 44) && (editLine[j] != 0)) j++;
  char nextWord[j - i + 1];       /** Create a buffer (the +1 is to make space for null-termination) **/
  for (k = i; k < j; k++) nextWord[k - i] = editLine[k]; /** Transfer the word to the buffer **/
  nextWord[j - i] = 0;            /** Null-termination **/
  initialPosition = j;            /** Next time round, start from here, unless... **/
  return (nextWord);              /** Return the contents of the buffer **/
}

void help() {
  cls();
  cprintString(3, 2,  F("The Byte Attic's CERBERUS 2080 (tm)"));
  cprintString(3, 3,  F("        AVAILABLE COMMANDS:"));
  cprintString(3, 4,  F(" (All numbers must be hexadecimal)"));
  cprintString(3, 6,  F("0xADDR BYTE: Writes BYTE at ADDR"));
  cprintString(3, 7,  F("list ADDR: Lists memory from ADDR"));
  cprintString(3, 8,  F("cls: Clears the screen"));
  cprintString(3, 9,  F("testmem: Reads/writes to memories"));
  cprintString(3, 10, F("6502: Switches to 6502 CPU mode"));
  cprintString(3, 11, F("z80: Switches to Z80 CPU mode"));
  cprintString(3, 12, F("fast: Switches to 8MHz mode"));
  cprintString(3, 13, F("slow: Switches to 4MHz mode"));
  cprintString(3, 14, F("reset: Resets the system"));
  cprintString(3, 15, F("dir: Lists files on uSD card"));
  cprintString(3, 16, F("del FILE: Deletes FILE"));
  cprintString(3, 17, F("load FILE ADDR: Loads FILE at ADDR"));
  cprintString(3, 18, F("save ADDR1 ADDR2 FILE: Saves memory"));
  cprintString(5, 19, F("from ADDR1 to ADDR2 to FILE"));
  cprintString(3, 20, F("run: Executes code in memory"));
  cprintString(3, 21, F("move ADDR1 ADDR2 ADDR3: Moves bytes"));
  cprintString(5, 22, F("between ADDR1 & ADDR2 to ADDR3 on"));
  cprintString(3, 23, F("help / ?: Shows this help screen"));
  cprintString(3, 24, F("F12 key: Quits CPU program"));
}

void binMove(String startAddr, String endAddr, String destAddr) {
  unsigned int start, finish, destination;                /** Memory addresses **/
  unsigned int i;                                         /** Address counter **/
  if (startAddr == "") cprintStatus(STATUS_MISSING_OPERAND);                   /** Missing the file's name **/
  else {
    start = strtol(startAddr.c_str(), NULL, 16);          /** Convert hexadecimal address string to unsigned int **/
    if (endAddr == "") cprintStatus(STATUS_MISSING_OPERAND);                   /** Missing the file's name **/
    else {
      finish = strtol(endAddr.c_str(), NULL, 16);         /** Convert hexadecimal address string to unsigned int **/
      if (destAddr == "") cprintStatus(STATUS_MISSING_OPERAND);                /** Missing the file's name **/
      else {
        destination = strtol(destAddr.c_str(), NULL, 16); /** Convert hexadecimal address string to unsigned int **/
        if (finish < start) cprintStatus(STATUS_ADDRESS_ERROR);              /** Invalid address range **/
        else if ((destination <= finish) && (destination >= start)) cprintStatus(STATUS_ADDRESS_ERROR); /** Destination cannot be within original range **/  
        else {
          for (i = start; i <= finish; i++) {
            cpoke(destination, cpeek(i));
            destination++;
          }
          cprintStatus(STATUS_READY);
        }
      }
    }
  }
}

void list(String address) {
  /** Lists the contents of memory from the given address, in a compact format **/
  byte i, j;                      /** Just counters **/
  unsigned int addr;              /** Memory address **/
  if (address == "") addr = 0;
  else addr = strtol(address.c_str(), NULL, 16); /** Convert hexadecimal address string to unsigned int **/
  for (i = 2; i < 25; i++) {
    cprintString(3, i, "0x");
    cprintString(5, i, String(addr, HEX));
    for (j = 0; j < 8; j++) {
      cprintString(12+(j*3), i, String(cpeek(addr++), HEX)); /** Print bytes in HEX **/
    }
  }
}

void runCode() {
  byte runL = config_code_start & 0xFF;
  byte runH = config_code_start >> 8;

  ccls();
  /** REMEMBER:                           **/
  /** Byte at config_outbox_flag is the new mail flag **/
  /** Byte at config_outbox_data is the mail box      **/
  cpoke(config_outbox_flag, 0x00);	/** Reset outbox mail flag	**/
  cpoke(config_outbox_data, 0x00);	/** Reset outbox mail data	**/
  cpoke(config_inbox_flag, 0x00);	/** Reset inbox mail flag	**/
  if (!mode) {            /** We are in 6502 mode **/
    /** Non-maskable interrupt vector points to 0xFCB0, just after video area **/
    cpoke(0xFFFA, 0xB0);
    cpoke(0xFFFB, 0xFC);
    // NMI interrupt routine, can be overridden by the CPU later
    cpoke(0xFCB0, 0xE6);    // INC zeropage address
    cpoke(0xFCB1, config_counter_nmi);    // address of the counter that increments every NMI
    cpoke(0xFCB2, 0x40);    // RTI
    /** Set reset vector to config_code_start, the beginning of the code area **/
    cpoke(0xFFFC, runL);
    cpoke(0xFFFD, runH);
  } else {                /** We are in Z80 mode **/
    /** The NMI service routine of the Z80 is at 0x0066 **/
    /** It simply returns **/
    // 0066   ED 45                  RETN 
    cpoke(0x0066, 0xED);
    cpoke(0x0067, 0x45);
    /** The Z80 fetches the first instruction from 0x0000, so put a jump to the code area there **/
    // 0000   C3 ll hh               JP   config_code_start
	#if config_code_start != 0x0000
    cpoke(0x0000, 0xC3);
    cpoke(0x0001, runL);
    cpoke(0x0002, runH);
	#endif
  }
  cpurunning = true;
  digitalWrite(CPURST, HIGH); /** Reset the CPU **/
  digitalWrite(CPUGO, HIGH);  /** Enable CPU buses and clock **/
  delay(50);
  digitalWrite(CPURST, LOW);  /** CPU should now initialize and then go to its reset vector **/
  #if config_enable_nmi == 1
  Timer1.initialize(20000);
  Timer1.attachInterrupt(cpuInterrupt); /** Interrupt every 0.02 seconds - 50Hz **/
  #endif
}

void stopCode() {
    cpurunning = false;         /** Reset this flag **/
    Timer1.detachInterrupt();
    digitalWrite(CPURST, HIGH); /** Reset the CPU to bring its output signals back to original states **/ 
    digitalWrite(CPUGO, LOW);   /** Tristate its buses to high-Z **/
    delay(50);                   /** Give it some time **/
    digitalWrite(CPURST, LOW);  /** Finish reset cycle **/

    load("chardefs.bin", 0xf000);/** Reset the character definitions in case the CPU changed them **/
    ccls();                     /** Clear screen completely **/
    cprintFrames();             /** Reprint the wire frame in case the CPU code messed with it **/
    cprintStatus(STATUS_DEFAULT);            /** Update status bar **/
    clearEditLine();            /** Clear and display the edit line **/
}

void dir() {
  /** Lists the files in the root directory of uSD card, if available **/
  byte y = 2;                     /** Screen line **/
  byte x = 0;                     /** Screen column **/
  File root;                      /** Root directory of uSD card **/
  File entry;                     /** A file on the uSD card **/
  cls();
  root = SD.open("/");            /** Go to the root directory of uSD card **/
  while (true) {
    entry = root.openNextFile();  /** Open next file **/
    if (!entry) {                 /** No more files on the uSD card **/
      root.close();               /** Close root directory **/
      cprintStatus(STATUS_READY);            /** Announce completion **/
      break;                      /** Get out of this otherwise infinite while() loop **/
    }
    cprintString(3, y, entry.name());
    cprintString(20, y, String(entry.size(), DEC));
    entry.close();                /** Close file as soon as it is no longer needed **/
    if (y < 24) y++;              /** Go to the next screen line **/
    else {
      cprintStatus(STATUS_SCROLL_PROMPT);            /** End of screen has been reached, needs to scrow down **/
      for (x = 2; x < 40; x++) cprintChar(x, 29, ' '); /** Hide editline while waiting for key press **/
      while (!keyboard.available());/** Wait for a key to be pressed **/
      if ((keyboard.read() & 0xFF) == PS2_KEY_ESC) { /** If the user pressed ESC, break and exit **/
        tone(SOUND, 750, 5);      /** Clicking sound for auditive feedback to key press **/
        root.close();             /** Close the directory before exiting **/
        cprintStatus(STATUS_READY);
        break;
      } else {
        tone(SOUND, 750, 5);      /** Clicking sound for auditive feedback to key press **/
        cls();                    /** Clear the screen and... **/
        y = 2;                    /** ...go back tot he top of the screen **/
      }
    }
  }
}

void catDelFile(String filename) {
	cprintStatus(delFile(filename));
}

int delFile(String filename) {
	int status = STATUS_DEFAULT;
  	/** Deletes a file from the uSD card **/
  	if (!SD.exists(filename)) {
		status = STATUS_NO_FILE;		/** The file doesn't exist, so stop with error **/
	}
  	else {
	    SD.remove(filename);          /** Delete the file **/
	    status = STATUS_READY;
  	}
	return status;
}

void catSave(String filename, String startAddress, String endAddress) {
	unsigned int startAddr;
	unsigned int endAddr;
	int status = STATUS_DEFAULT;
   	if (startAddress == "") {
		status = STATUS_MISSING_OPERAND;               /** Missing operand **/
	}
	else {
		startAddr = strtol(startAddress.c_str(), NULL, 16);
		if(endAddress == "") {
			status = STATUS_MISSING_OPERAND;
		}
		else {
			endAddr = strtol(endAddress.c_str(), NULL, 16);
			status = save(filename, startAddr, endAddr);
		}
	}
	cprintStatus(status);
}

int save(String filename, unsigned int startAddress, unsigned int endAddress) {
  	/** Saves contents of a region of memory to a file on uSD card **/
	int status = STATUS_DEFAULT;
  	unsigned int i;                                     /** Memory address counter **/
  	byte data;                                          /** Data from memory **/
  	File dataFile;                                      /** File to be created and written to **/
	if (endAddress < startAddress) {
		status = STATUS_ADDRESS_ERROR;            		/** Invalid address range **/
	}
	else {
		if (filename == "") {
			status = STATUS_MISSING_OPERAND;          	/** Missing the file's name **/
		}
		else {
			if (SD.exists(filename)) {
				status = STATUS_FILE_EXISTS;   				/** The file already exists, so stop with error **/
			}
			else {
				dataFile = SD.open(filename, FILE_WRITE); /** Try to create the file **/
				if (!dataFile) {
					status = STATUS_CANNOT_OPEN;           /** Cannot create the file **/
				}
				else {                                    /** Now we can finally write into the created file **/
					for(i = startAddress; i <= endAddress; i++) {
						data = cpeek(i);
						dataFile.write(data);
					}
					dataFile.close();
					status = STATUS_READY;
				}
			}
		}
	}
	return status;
}

void catLoad(String filename, String startAddress, bool silent) {
	unsigned int startAddr;
	int status = STATUS_DEFAULT;
	if (startAddress == "") {
		startAddr = config_code_start;	/** If not otherwise specified, load file into start of code area **/
	}
	else {
		startAddr = strtol(startAddress.c_str(), NULL, 16);	/** Convert address string to hexadecimal number **/
	}
	status = load(filename, startAddr);
	if(!silent) {
		cprintStatus(status);
	}
}

int load(String filename, unsigned int startAddr) {
  /** Loads a binary file from the uSD card into memory **/
  File dataFile;                                /** File for reading from on SD Card, if present **/
  unsigned int addr = startAddr;                /** Address where to load the file into memory **/
  int status = STATUS_DEFAULT;
  if (filename == "") {
	  status = STATUS_MISSING_OPERAND;
  }
  else {
    if (!SD.exists(filename)) {
		status = STATUS_NO_FILE;				/** The file does not exist, so stop with error **/
	} 
    else {
      	dataFile = SD.open(filename);           /** Open the binary file **/
      	if (!dataFile) {
			status = STATUS_CANNOT_OPEN; 		/** Cannot open the file **/
	  	}
      	else {
        	while (dataFile.available()) {		/** While there is data to be read... **/
          	cpoke(addr++, dataFile.read());     /** Read data from file and store it in memory **/
          	if (addr == 0) {                    /** Break if address wraps around to the start of memory **/
            	dataFile.close();
            	break;
          	}
        }
        dataFile.close();
		status = STATUS_READY;
      }
    }
  }
  return status;
}

void cprintEditLine () {
  	byte i;
  	for (i = 0; i < 38; i++) cprintChar(i + 2, 29, editLine[i]);
}

void clearEditLine() {
  	/** Resets the contents of edit line and reprints it **/
  	byte i;
  	editLine[0] = 62;
  	editLine[1] = 0;
  	for (i = 2; i < 38; i++) editLine[i] = 32;
  	pos = 1;
  	cprintEditLine();
}

void storePreviousLine() {
	for (byte i = 0; i < 38; i++) previousEditLine[i] = editLine[i]; /** Store edit line just executed **/
}

void cprintStatus(byte status) {
  	/** REMEMBER: The macro "F()" simply tells the compiler to put the string in code memory, so to save dynamic memory **/
  	switch( status ) {
    	case STATUS_BOOT:
      		center(F("Here we go! Hang on..."));
      		break;
    	case STATUS_READY:
      		center(F("Alright, done!"));
      		break;
    	case STATUS_UNKNOWN_COMMAND:
      		center(F("Darn, unrecognized command"));
      		tone(SOUND, 50, 150);
      		break;
    	case STATUS_NO_FILE:
      		center(F("Oops, file doesn't seem to exist"));
      		tone(SOUND, 50, 150);
      		break;
    	case STATUS_CANNOT_OPEN:
      		center(F("Oops, couldn't open the file"));
      		tone(SOUND, 50, 150);
      		break;
    	case STATUS_MISSING_OPERAND:
      		center(F("Oops, missing an operand!!"));
      		tone(SOUND, 50, 150);
      		break;
    	case STATUS_SCROLL_PROMPT:
      		center(F("Press a key to scroll, ESC to stop"));
      		break;
    	case STATUS_FILE_EXISTS:
      		center(F("The file already exists!"));
      	break;
    		case STATUS_ADDRESS_ERROR:
      	center(F("Oops, invalid address range!"));
      		break;
    	case STATUS_POWER:
      		center(F("Feel the power of Dutch design!!"));
      		break;
    	default:
      		cprintString(2, 27, F("      CERBERUS 2080: "));
      		if (mode) cprintString(23, 27, F(" Z80, "));
      		else cprintString(23, 27, F("6502, "));
      		if (fast) cprintString(29, 27, F("8 MHz"));
      		else cprintString(29, 27, F("4 MHz"));
      		cprintString(34, 27, F("     "));
  	}
}

void center(String text) {
  	clearLine(27);
  	cprintString(2+(38-text.length())/2, 27, text);
}

void playJingle() {
  	delay(500);           /** Wait for possible preceding keyboard click to end **/
  	tone(SOUND, 261, 50);
  	delay(150);
  	tone(SOUND, 277, 50);
  	delay(150);
  	tone(SOUND, 261, 50);
  	delay(150);
  	tone(SOUND, 349, 500);
  	delay(250);
  	tone(SOUND, 261, 50);
  	delay(150);
  	tone(SOUND, 349, 900);
}

void cls() {
  	/** This clears the screen only WITHIN the main frame **/
  	unsigned int y;
  	for (y = 2; y <= 25; y++) {
    	clearLine(y);
	}
}

void clearLine(byte y) {
  	unsigned int x;
  	for (x = 2; x <= 39; x++) {
    	cprintChar(x, y, 32);
	}
}

void ccls() {
  	/** This clears the entire screen **/
  	unsigned int x;
  	for (x = 0; x < 1200; x++) {
	    cpoke(0xF800 + x, 32);        /** Video memory addresses start at 0XF800 **/
	}
}

void cprintFrames() {
  	unsigned int x;
  	unsigned int y;
  	/** First print horizontal bars **/
  	for (x = 2; x <= 39; x++) {
	    cprintChar(x, 1, 3);
	    cprintChar(x, 30, 131);
	    cprintChar(x, 26, 3);
  	}
  	/** Now print vertical bars **/
  	for (y = 1; y <= 30; y++) {
	    cprintChar(1, y, 133);
	    cprintChar(40, y, 5);
  	}
}

void cprintString(byte x, byte y, String text) {
  	unsigned int i;
  	for (i = 0; i < text.length(); i++) {
	    if (((x + i) > 1) && ((x + i) < 40)) {
			cprintChar(x + i, y, text[i]);
		}
  	}
}

void cprintChar(byte x, byte y, byte token) {
  	/** First, calculate address **/
  	unsigned int address = 0xF800 + ((y - 1) * 40) + (x - 1); /** Video memory addresses start at 0XF800 **/
  	cpoke(address, token);
}

void testMem() {
  	/** Tests that all four memories are accessible for reading and writing **/
  	unsigned int x;
  	byte i = 0;
    for (x = 0; x < 874; x++) {
    	cpoke(x, i);                                           /** Write to low memory **/
    	cpoke(0x8000 + x, cpeek(x));                           /** Read from low memory and write to high memory **/
    	cpoke(addressTranslate(0xF800 + x), cpeek(0x8000 + x));/** Read from high mem, write to VMEM, read from character mem **/
    	if (i < 255) i++;
    	else i = 0;
  	}
}

unsigned int addressTranslate (unsigned int virtualAddress) {
  	byte numberVirtualRows;
  	numberVirtualRows = (virtualAddress - 0xF800) / 38;
  	return((virtualAddress + 43) + (2 * (numberVirtualRows - 1)));
}

void resetCPUs() {            	/** Self-explanatory **/
  	digitalWrite(CPURST, LOW);
  	digitalWrite(CPUSLC, LOW);  /** First reset the 6502 **/
  	digitalWrite(CPUGO, HIGH);
  	delay(50);
  	digitalWrite(CPURST, HIGH);
  	digitalWrite(CPUGO, LOW);
  	delay(50);
  	digitalWrite(CPURST, LOW);
  	digitalWrite(CPUSLC, HIGH); /** Now reset the Z80 **/
  	digitalWrite(CPUGO, HIGH);
  	delay(50);
  	digitalWrite(CPURST, HIGH);
  	digitalWrite(CPUGO, LOW);
  	delay(50);
  	digitalWrite(CPURST, LOW);
  	if (!mode) {
		  digitalWrite(CPUSLC, LOW);
	}
}

byte readShiftRegister() {
  	byte data;
  	data = shiftIn(SI, SC, MSBFIRST);
  	return data;
}

void setShiftRegister(unsigned int address, byte data) { 
  	shiftOut(SO, SC, LSBFIRST, address);      /** First 8 bits of address **/
  	shiftOut(SO, SC, LSBFIRST, address >> 8); /** Then the remaining 8 bits **/
  	shiftOut(SO, SC, LSBFIRST, data);         /** Finally, a byte of data **/
}

void cpoke(unsigned int address, byte data) {
  	setShiftRegister(address, data);
  	digitalWrite(AOE, HIGH);      /** Enable address onto bus **/
  	digitalWrite(RW, LOW);        /** Begin writing **/
  	digitalWrite(RW, HIGH);       /** Finish up**/
  	digitalWrite(AOE, LOW);
}

void cpokeW(unsigned int address, unsigned int data) {
	cpoke(address, data & 0xFF);
	cpoke(address + 1, (data >> 8) & 0xFF);
}

void cpokeL(unsigned int address, unsigned long data) {
	cpoke(address, data & 0xFF);
	cpoke(address + 1, (data >> 8) & 0xFF);
	cpoke(address + 2, (data >> 16) & 0xFF);
	cpoke(address + 3, (data >> 24) & 0xFF);
}

boolean cpokeStr(unsigned int address, String text) {
	unsigned int i;
	for(i = 0; i < text.length(); i++) {
		cpoke(address + i, text[i]);
	}
	cpoke(address + i, 0);
	return true;
}

byte cpeek(unsigned int address) {
  	byte data = 0;
  	setShiftRegister(address, data);
  	digitalWrite(AOE, HIGH);      /** Enable address onto us **/
  	/** This time we do NOT enable the data outputs of the shift register, as we are reading **/
  	digitalWrite(LD, HIGH);       /** Prepare to latch byte from data bus into shift register **/
  	digitalWrite(SC, HIGH);       /** Now the clock tics, so the byte is actually latched **/
  	digitalWrite(LD, LOW);
  	digitalWrite(AOE, LOW);
  	data = readShiftRegister();
  	return data;
}

unsigned int cpeekW(unsigned int address) {
	return cpeek(address) + (256 * cpeek(address+1));
}

boolean cpeekStr(unsigned int address, byte * dest, int max) {
	unsigned int i;
	byte c;
	for(i = 0; i < max; i++) {
		c = cpeek(address + i);
		dest[i] = c;
		if(c == 0) return true;
	}
	return false;
}
