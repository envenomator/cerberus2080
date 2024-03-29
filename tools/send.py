#!/usr/bin/env python3

DEFAULT_SERIAL_PORT = 'COM11'
DEFAULT_BAUDRATE    = 115200
DEFAULT_ADDRESS     = 0x0205

import sys
import time
import os.path

## syntax
## send.py FILENAME <SERIAL PORT> <START ADDRESS>
## 
## SERIAL PORT defaults to /dev/ttyUSB0
## START ADDRESS defaults to 0x0205

def errorexit(message):
  print(message)
  print('Press ENTER to continue')
  input()
  exit()
  return

try:
  import serial
  import serial.tools.list_ports
except ModuleNotFoundError:
  errorexit('Please install the \'pyserial\' module with pip')

if(os.name == 'posix'): # termios only exists on Linux
  DEFAULT_SERIAL_PORT   = '/dev/ttyUSB0'
  try:
    import termios
  except ModuleNotFoundError:
    errorexit('Please install the \'termios\' module with pip')


if len(sys.argv) == 1:
  sys.exit('Usage: send.py FILENAME <SERIAL_PORT> <START_ADDRESS>')

if not os.path.isfile(sys.argv[1]):
  sys.exit(f'Error: file \'{sys.argv[1]}\' not found')

if len(sys.argv) == 2:
  serialports = serial.tools.list_ports.comports()
  if len(serialports) > 1:
    sys.exit("Multiple serial ports present - cannot automatically select");
  if len(serialports) == 0:
    sys.exit('No serial port available');
  serialport = str(serialports[0]).split(" ")[0]
if len(sys.argv) >= 3:
  serialport = sys.argv[2]

if len(sys.argv) == 4:
  startAddress = sys.argv[3]
else:
  startAddress = DEFAULT_ADDRESS

print(f'Sending {sys.argv[1]}')
print(f'Using port {serialport}')

f = open(sys.argv[1], "rb")

content = f.read()
checkError = False
blockSize = 10

resetPort = False

if(os.name == 'posix'):
  if resetPort == False:
  # to be able to suppress DTR, we need this
    f = open(serialport)
    attrs = termios.tcgetattr(f)
    attrs[2] = attrs[2] & ~termios.HUPCL
    termios.tcsetattr(f, termios.TCSAFLUSH, attrs)
    f.close()
  else:
    f = open(serialport)
    attrs = termios.tcgetattr(f)
    attrs[2] = attrs[2] | termios.HUPCL
    termios.tcsetattr(f, termios.TCSAFLUSH, attrs)
    f.close()

ser = serial.Serial()
ser.baudrate = DEFAULT_BAUDRATE
ser.port = serialport
ser.timeout = 2

# OS-specific serial dtr/rts settings
if(os.name == 'nt'):
  ser.setDTR(False)
  ser.setRTS(False)
if(os.name == 'posix'):
  ser.rtscts = False            # not setting to false prevents communication
  ser.dsrdtr = resetPort        # determines if board resets or not

try:
    ser.open()
    print('Opening serial port...')
    #time.sleep(3) #might need this if a Cerberus keeps resetting after opening the serial
    print('Writing file to serial port')
    blockIndex = 0
    blockBytesRemaining = len(content)
    while (blockBytesRemaining > 0) and (not checkError):
      print(f'{(blockIndex/len(content))*100:3.0f}% - {blockIndex:6d} of {len(content):6d} - ', end="")
      blockSize = min(blockBytesRemaining, blockSize)
      header = f'0x{(startAddress + blockIndex):04x}'
      print(header, end="")
      for byte in header:
          ser.write(str(byte).encode('ascii'))
      checka = 1
      checkb = 0
      for i in range(blockSize):
        data = content[blockIndex]
        blockIndex = blockIndex + 1
        checka = (checka + data) % 256
        checkb = (checkb + checka) % 256
        block = f' {data:02X}'
        blockBytesRemaining = blockBytesRemaining - 1
        print(block, end="")
        for byte in block:
          ser.write(str(byte).encode('ascii'))
      checksum = f'{((checka << 8) | checkb):0X}'
      ser.write(b'\r')
      ret = ser.readline()
      ret = ret.strip()
      if not ret.endswith(bytes(checksum, 'ascii')):
        print(' -- recv error')
        checkError = True
      else:
        print(' -- recv ok')
    ser.write(b'\r')
    if checkError:
      print("Upload error - mismatched checksum")
    else:
      print('\nCerberus command to save data to disk:')
      print(f'SAVE {(startAddress):04x} {(startAddress + blockIndex - 1):04x} FILENAME')
    f.close()
except serial.SerialException:
  print('Error: serial port unavailable, quiting.')
