#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void write_command(unsigned char command);
void write_data(unsigned char character);
void write_string(unsigned char *s);

//int flag=0;

int i1,i2;

ISR(PCINT0_vect){

	if((PINB & (1<<PB4))<=0){

		//PORTB ^= (1<<PB2);
		i1++;  //increment count1

		EIMSK &= ~(1<<INT0|1<<INT1);   //turn off int0,1
		PCICR &= ~(1<<PCIE0|1<<PCIE1);   //turn off pcint0,1
		TCNT1 = 0 ;                     //set t1 = 0
		TCCR1B |= (1<<CS12);            //enable timer

		TIMSK1 |= ( 1 << OCIE1A ); // enable timer interrupt mask register for OCR1A
		
	}

	//sei();


}

ISR(PCINT1_vect){

	if((PINC & (1<<PC5))<=0){

		//PORTB ^= (1<<PB3);

		i2--;  //increment count1
		
		//cli();

		//_delay_ms(200);
		
		EIMSK &= ~(1<<INT0);
		EIMSK &= ~(1<<INT1);
		PCICR &= ~(1<<PCIE0|1<<PCIE1);
		TCNT1 = 0 ;
		TCCR1B |= (1<<CS12);
		TIMSK1 |= ( 1 << OCIE1A ); // enable timer interrupt mask register for OCR1A
	}

}


ISR(INT0_vect){

	PORTB ^= (1<<PB2); //toggle led1
	//cli();
	//_delay_ms(200);

	EIMSK &= ~(1<<INT0);
	EIMSK &= ~(1<<INT1);
	PCICR &= ~(1<<PCIE0|1<<PCIE1);
	TCNT1 = 0 ;
	TCCR1B |= (1<<CS12);
	TIMSK1 |= ( 1 << OCIE1A ); // enable timer interrupt mask register for OCR1A
	


	//sei();
	//flag=1;
}


ISR(INT1_vect){

	PORTB ^= (1<<PB3); //toggle led1
	
	EIMSK &= ~(1<<INT0);
	EIMSK &= ~(1<<INT1);
	PCICR &= ~(1<<PCIE0|1<<PCIE1);
	TCNT1 = 0 ;
	TCCR1B |= (1<<CS12);
	TIMSK1 |= ( 1 << OCIE1A ); // enable timer interrupt mask register for OCR1A

}


ISR(TIMER1_COMPA_vect){
		//PORTB ^= (1 << PB2); // toggles the led
		TIMSK1 &= ~( 1 << OCIE1A ); // enable timer interrupt mask register for OCR1A
		EIMSK |= (1<<INT0 | 1<<INT1);
		PCICR |= (1<<PCIE0|1<<PCIE1);
		TCCR1B &= ~(1<<CS12);

}	
	
//sei = set interrupts
//cli = clear interrupts



int main(void){

	DDRB |= (1<<DDB1 | 1<<DDB0);  //READWRITE connected to ground, write mode for enable and rs
	DDRD |= (1<<DDD4 | 1<<DDD5 | 1<<DDD6 | 1<<DDD7);  //write mode for data/command bits (4)



	//CONNECTION FOR LED AND BUTTON
	DDRB |= (1<<DDB2 | 1<<DDB3); //pb2,3 - led output
	PORTB |= (1<<PB2 | 1<<PB3); //LED OUT HIGH

	DDRD &= ~(1<<DDD2 | 1<<DDD3); //int0&1 
	PORTD |= (1<<PD2|1<<PD3);  //INT0&1 


	EIMSK |= (1<<INT0|1<<INT1);


	DDRB &= ~(1<<DDB4); //PCINT4 input
	PORTB |= (1<<PB4);  //PCINT4 high
	DDRC &= ~(1<<DDC5); //PCINT13 input
	PORTC |= (1<<PC5);  //PCINT13 high


	PCICR |= (1<<PCIE0|1<<PCIE1);    //enable pcint 
	PCMSK1 |= (1<<PCINT13); 	 	//pc int13 enable
	PCMSK0 |= (1<<PCINT4); 	 	//pc int4 enable


	TCCR1B |= ( 1 << WGM12 ); // initialising 16-bit counter in CTC mode with prescaler of 256
	OCR1A = 12499; // 16MHz / (256*2) = x -->> x*2/5 
	TCNT1 = 0; // initialise counter

	

	sei();


	_delay_ms(15);

	write_command(0x28);  //set interface - 4-Bits, 2-lines
	write_command(0x08);  //enable display
	write_command(0x01);  //clear and home
	write_command(0x06);  //move cursor right
	write_command(0x0C);  //turrn on display

	i1=0,i2=0;
	char c[100];



	while(1){

		write_command(0x80);
		itoa(i1,c,10);
		write_string(c);
		//_delay_ms(1000);

		write_command(0xC0);
		itoa(i2,c,10);
		write_string(c);
		//_delay_ms(1000);

	}


}

void write_string(unsigned char *s){

	int i=0;

	for(;s[i]!='\0';i++){
		write_data(s[i]); //write char by char
		//_delay_ms(100);   //to create a typing effect
	}    

}

void write_command(unsigned char command){

	PORTB &= ~(1<<PB0);  //en low
	PORTB &= ~(1<<PB1);  //rs low

	PORTD = (command & 0xf0); //high byte
	
	PORTB |= (1<<PB0); //en high
	_delay_us(1); // delay 450ns
	PORTB &= ~(1<<PB0);  //en low
	_delay_ms(5); // delay 5ms

	PORTD = ((command<<4) & 0xf0); //lower byte
	PORTB |= (1<<PB0); //en high 
	_delay_us(1); // delay 450ns
	PORTB &= ~(1<<PB0);  //en low
	_delay_ms(5); // delay 5ms


}

void write_data(unsigned char character){

	PORTB &= ~(1<<PB0);  //en low
	PORTB |= (1<<PB1);  //rs high

	PORTD = (character & 0xf0); //high byte
	PORTB |= (1<<PB0); //en high
	_delay_us(1); // delay 450ns
	PORTB &= ~(1<<PB0);  //en low
	_delay_us(200); // delay 5ms

	PORTD = ((character<<4) & 0xf0); //lower byte
	PORTB |= (1<<PB0); //en high  
	_delay_us(1); // delay 450ns
	PORTB &= ~(1<<PB0);  //en low
	_delay_us(200); // delay 5ms


}





