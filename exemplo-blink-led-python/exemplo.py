from pyfirmata import Arduino, util
import time

pinLed = 13

board = Arduino('COM8') # Verifica a porta COM no caso de linux /dev/tty..


def blinkLed(blynkTime = 1):
    while True:
        board.digital[pinLed].write(0)
        time.sleep(1)
        board.digital[pinLed].write(1)
        time.sleep(1)

if __name__ == '__main__': # Executa a função
    blinkLed()
