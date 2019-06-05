/*****************************************************************************/
/* Based on Craig Peacock's  code                                            */
/* Supports HAF 440Hz-1660Hz  (Octaves 4 and 5)                              */
/*****************************************************************************/

#include <pic.h>

#define TONE    RB4
#define HEARTBEAT RB0

void InitTimer(void);
void delayms(unsigned char cnt);
void PlayNote(unsigned short note, unsigned char octave, unsigned int duration);

unsigned char beep;
unsigned char preloadTMR1L;
unsigned char preloadTMR1H;
unsigned short TMR0Count;
unsigned char beat_speed;

void main(void)
{
    unsigned int pointer = 0;
    unsigned int octave = 0;
    unsigned int duration = 0;
    unsigned short note = 0;
    unsigned int defaultoctave = 0;
    unsigned int defaultduration = 0;

/*<!--viki-begin-tone-->*/

	const unsigned char static Melody[] = {""};
        defaultoctave = ;
        defaultduration = ;
        beat_speed = ; 
/*<!--viki-end-tone-->*/
	
	TRISB4 = 0;     /* Make TONE an output */
	TRISB0 = 0;		/* Make HEARTBEAT as output */	
	HEARTBEAT = 1;
    beep = 0;

    InitTimer();
    PEIE = 1;
    GIE = 1;      /* Enable General Purpose Interrupts */

    do {

        octave = defaultoctave; 	/* Set Default Octave */

        if ((Melody[pointer] == '3') && (Melody[pointer+1] == '2')) {
            duration = 32;
            pointer += 2;
        }
        else if ((Melody[pointer] == '1') && (Melody[pointer+1] == '6')) {
            duration = 16;
            pointer += 2;
        }
        else if (Melody[pointer] == '8') {
            duration = 8;
            pointer++;
        }
        else if (Melody[pointer] == '4') {
            duration = 4;
            pointer++;
        }
        else if (Melody[pointer] == '2') {
            duration = 2;
            pointer++;
        }
        else if (Melody[pointer] == '1') {
            duration = 1;
            pointer++;
        } else duration = defaultduration;

        if (Melody[pointer + 1] == '#') {

            /* Process Sharps */

            switch (Melody[pointer]) {
	    case 'a' : note = 2145;
	      break;
	    case 'c' : note = 1803;
	      break;
	    case 'd' : note = 1607;
	      break;
	    case 'f' : note = 1351;
	      break;
	    case 'g' : note = 1204;
	      break;
            }
            pointer +=2;

        } else {

            switch (Melody[pointer]) {
	    case 'a' : note = 2272;
	      break;
	    case 'b' : note = 2024;
	      break;
	    case 'c' : note = 1911;
	      break;
	    case 'd' : note = 1702;
	      break;
	    case 'e' : note = 1516;
	      break;
	    case 'f' : note = 1431;
	      break;
	    case 'g' : note = 1275;
	      break;
	    case 'p' : note = 0;
	      break;
            }
            pointer++;
        }

        if (Melody[pointer] == '.') {
            /* Duration 1.5x */
            duration = duration + 128;
            pointer++;
        }

        if (Melody[pointer] == '4') {
            octave = 4;
            pointer++;
        } else if (Melody[pointer] == '5') {
            octave = 5;
            pointer++;
        } else     if (Melody[pointer] == '6') {
            octave = 6;
            pointer++;
        } else     if (Melody[pointer] == '7') {
            octave = 7;
            pointer++;
        } 

        if (Melody[pointer] == '.') {
            /* Duration 1.5x */
            duration = duration + 128;
            pointer++;
        }
	
	/* Divide by 4 to take care of 4Mhz */
	PlayNote(note, octave, duration);       

     } while (Melody[pointer++] == ',');
    
    /* Wait until last note has played */
    while(TMR0Count) { };
    beep = 0;

    /* Loop */
    while(1) {};
}

void PlayNote(unsigned short note, unsigned char octave, unsigned int duration)
{

    /* Process octave */
    switch (octave) {
        case 4 : /* Do noting */
             break;
        case 5 : /* %2 */
             note = note >> 1; 
             break;
        case 6 : /* %4 */
	  /* note = note >> 2; */
             break;
        case 7 : /* %8 */
	  /* note = note >> 4; */
             break;
    }

    /* Wait until last note has played */
    while(TMR0Count) { };
    beep = 0;

    /* Process New Note Frequency */
    if (note) {
        note = ~note;
        preloadTMR1L = (note & 0xFF);
        preloadTMR1H = ((note & 0xFF00) >> 8);
    }
    
    /* Process Note Duration */    
    TMR0Count = 255/(duration & 0x7F);

    /* If duration is 1.5x add .5 to duration */
    if (duration & 0x80) TMR0Count = (TMR0Count + (TMR0Count >> 1));    

    if (note) beep = 1;
}

void InitTimer(void)
{
    /* Initialise Timer 0 */
    OPTION = 0b11010111;  /* Set TMR0 to Internal CLk, 1:256 */
    T0IF = 0;             /* Clear TMR0 Flag, ready for use */
    T0IE = 1;             /* Enable Timer Overflow Interrupt */

    /* Initialise Timer 1 */
    T1CON = 0b00000001;   /* Counter Enabled, Using Ext Pin 1:1 Prescaler */
    TMR1IF = 0;           /* Clear Flag */
    TMR1IE = 1;           /* Enable Interrupt */
}

void interrupt interr(void)
{
    if (T0IF) {
        TMR0 = beat_speed;
        if (TMR0Count) TMR0Count--;
        T0IF = 0;
    }
    if (TMR1IF) {
        if (beep) TONE = !TONE;
        else      TONE = 0;
        TMR1H = preloadTMR1H;
        TMR1L = preloadTMR1L;
        TMR1IF = 0; /* Clear Flag */
    }
}










