#include <avr/cpufunc.h>
#include <avr/io.h>
#include <util/delay.h>

const int STEP = 1;
const int MAX = 255;
const int MIN = 0;
const int BUTTON_DELAY = 10;
const int BIG_DELAY = 100;

enum STATES {UP, DOWN, FLAT, FIXED};

int check_button(int* state, int* delay_press)
{
  if (*delay_press)
  {
	(*delay_press)--;
	return 0;
  }
  if (!(PINB & 0b00001000))
  {
	*delay_press = BUTTON_DELAY;
	if(*state == FIXED)
	  *state = DOWN;
	else
	  *state = FIXED;
	return 1;
  }
  return 0;
}

int main(void)
{
  // все ножки на выход
  DDRB =  0b00000011; // пины 0 и 1 на выход, остальные на вход
  PORTB = 0b11111100; // пины 2 - 7 подтянуть к +5 В
  TCCR0A = 128 + 32 + 3; // режим работы каналов А и В: FastPWM. Сброс при совпадении.
  TCCR0B = 1; // Без делителя таймера
  OCR0A = MIN;
  OCR0B = MAX;
  int direction = STEP;
  int state = UP;
  int bigdelay = 1;
  int delay = 10;
  int button_delay = 0;
  int enable_button = 1;
  int first_press = 0;
  int delay_press = 0;
  while (1)
  {
	OCR0A += direction;
	OCR0B = MAX - OCR0A;
	for(int i=0; i< bigdelay; i++)
	{
	  if(check_button(&state, &delay_press))
		break;
	  _delay_ms(60);
	}
	switch(state) {
	  case UP:
		if (OCR0A >= 253)
		{
		  state = DOWN;
		  bigdelay = BIG_DELAY;
		}
		else
		{
		  bigdelay = 1;
		  direction = STEP;
		}
		break;
	  case DOWN:
		if (OCR0A <= 2)
		{
		  state = UP;
		  bigdelay = BIG_DELAY;
		}
		else
		{
		  bigdelay = 1;
		  direction = -STEP;
		}
		break;
	  case FIXED:
		bigdelay = 1;
		OCR0A = MAX;
		OCR0B = MIN;
		direction = 0;
	}
  }
}

