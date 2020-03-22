/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 * 
 * Código da música retirado de https://dragaosemchama.com/en/2019/02/songs-for-arduino/ 
 * 
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"


#include "CANTINA.h"
#include "GOT.h"

#include "math.h"



/************************************************************************/
/* defines                                                              */
/************************************************************************/

// LED
#define LED_PIO       PIOC
#define LED_PIO_ID    ID_PIOC
#define LED_IDX       8
#define LED_IDX_MASK  (1 << LED_IDX)

// Botão
#define BUT_PIO       PIOA
#define BUT_PIO_ID    ID_PIOA
#define BUT_IDX       11
#define BUT_IDX_MASK  (1 << BUT_IDX)

// Buzz
#define BUZZ_PIO       PIOA
#define BUZZ_PIO_ID    ID_PIOA
#define BUZZ_IDX       19
#define BUZZ_IDX_MASK  (1 << BUZZ_IDX)

// Botão1 oled
#define BUT1_PIO      PIOA
#define BUT1_PIO_ID   ID_PIOA
#define BUT1_IDX      2
#define BUT1_IDX_MASK (1 << BUT1_IDX)

// Botão2 oled
#define BUT2_PIO      PIOD
#define BUT2_PIO_ID   ID_PIOD
#define BUT2_IDX      30
#define BUT2_IDX_MASK (1 << BUT2_IDX)

// Botão3 oled
#define BUT3_PIO      PIOC
#define BUT3_PIO_ID   ID_PIOC
#define BUT3_IDX      13
#define BUT3_IDX_MASK (1 << BUT3_IDX)

// LED1 oled
#define LED1_PIO      PIOC
#define LED1_PIO_ID   ID_PIOC
#define LED1_IDX      19
#define LED1_IDX_MASK (1 << LED1_IDX)

// LED2 oled
#define LED2_PIO      PIOD
#define LED2_PIO_ID   ID_PIOD
#define LED2_IDX      26
#define LED2_IDX_MASK (1 << LED2_IDX)

// LED3 oled
#define LED3_PIO      PIOD
#define LED3_PIO_ID   ID_PIOD
#define LED3_IDX      11
#define LED3_IDX_MASK (1 << LED3_IDX)

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

volatile char flag;
volatile char comp;
volatile char musica;

void but_callback(void)
{
	flag = 1;
}

void alteraMusica(void)
{	
	musica = !musica;
}

/************************************************************************/
/* constants                                                            */
/************************************************************************/


/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/


/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);
void tocarMusica(int tempo, int melody[], int n_notas); //int melody[260], 

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

// Função de inicialização do uC
void init(void){
	// Initialize the board clock
	sysclk_init();

	// Disativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;

	// Ativa os PIOs que estão conectados (LEDs e BUZZER)
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(BUZZ_PIO_ID);

	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED2_PIO_ID);
	pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED3_PIO_ID);
	pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_IDX_MASK, PIO_DEFAULT);

	//Inicializa PC8 e PA19 como saída
	pio_set_output(LED_PIO, LED_IDX_MASK, 0, 0, 0);
	pio_set_output(LED1_PIO, LED_IDX_MASK, 0, 0, 0);
	pio_set_output(LED2_PIO, LED_IDX_MASK, 1, 0, 0);
	pio_set_output(LED3_PIO, LED_IDX_MASK, 0, 0, 0);
	pio_set_output(BUZZ_PIO, BUZZ_IDX_MASK, 0, 0, 0);
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	// configura pino ligado ao botão como entrada com um pull-up.
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  	pio_set_debounce_filter(BUT_PIO, BUT_IDX_MASK, 50);

	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_IDX_MASK, 50);
	
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_IDX_MASK, 50);
	
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_IDX_MASK, 50);
	
	// Ativa interrupção
	pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);
	pio_enable_interrupt(BUT1_PIO, BUT1_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_IDX_MASK);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUT1_PIO,
					BUT1_PIO_ID,
					BUT1_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but_callback);

	pio_handler_set(BUT3_PIO,
					BUT3_PIO_ID,
					BUT3_IDX_MASK,
					PIO_IT_RISE_EDGE,
					alteraMusica);

	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 2); // Prioridade 2
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 2); // Prioridade 2
	
	flag = 0;
	comp = 0;
	musica = 0;
	
}

void tocarMusica(int tempo, int melody[], int n_notas){ 

	// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
	// there are two values per note (pitch and duration), so for each note there are four bytes
	int notes = n_notas/2; //sizeof(melody) / sizeof(SOSmelody[0]) / 2;

	// this calculates the duration of a whole note in ms
	int wholenote = (60000 * 4) / tempo;

	int divider = 0;
	float noteDuration = 0;

	// iterate over the notes of the song.
	// Remember, the array is twice the number of notes (notes + durations)
	for (int thisNote = 0; thisNote < notes * 2; thisNote += 2) {

		if(flag){
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
			flag = 0;
		}
		
		if(musica != comp){
			comp = musica;
			break;
		}

		// calculates the duration of each note
		divider = melody[thisNote + 1];

		if (divider > 0) {
		// regular note, just proceed
		noteDuration = (wholenote) / divider;
		} 
		
		else if (divider < 0) {
		// dotted notes are represented with negative durations!!
		noteDuration = (wholenote) / abs(divider);
		noteDuration *= 1.5; // increases the duration in half for dotted notes
		}

		float pwm = 1000000/melody[thisNote];
		float toca = noteDuration/pwm;

		// ========== TOCAR NOTA =============
		
		if (melody[thisNote] == 0){
			delay_ms(noteDuration);
		}

		else{
			for (int x = 0; x < 1000*toca*0.9; x++){     //*0.9 para tocar somente 90% do tempo da nota
				pio_clear(PIOD, LED2_IDX_MASK);       
				pio_set(PIOA, BUZZ_IDX_MASK);       
				delay_us(pwm/2.0);

				pio_set(PIOD, LED2_IDX_MASK);
				pio_clear(PIOA, BUZZ_IDX_MASK);
				delay_us(pwm/2.0);				
			}	
			
			delay_ms(1000*toca*0.1);                    // 10% de pausa para diferenciar uma nota da outra
		}
	}
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void){ 

	init();

	pio_set(PIOC, LED1_IDX_MASK);
	pio_set(PIOD, LED2_IDX_MASK);
	pio_set(PIOD, LED3_IDX_MASK);

	// super loop
	// aplicacoes embarcadas não devem sair do while(1).
	// 
	while (1){

		if (musica == 1){
			pio_clear(PIOC, LED1_IDX_MASK);

			if (pio_get(BUT2_PIO, PIO_DEFAULT, BUT2_IDX_MASK) == 0){
				tocarMusica(200, SOSmelody, sizeof(SOSmelody)/sizeof(SOSmelody[0]));
			}
		}
		else if (musica == 0){
			pio_clear(PIOD, LED3_IDX_MASK);
			
			if (pio_get(BUT2_PIO, PIO_DEFAULT, BUT2_IDX_MASK) == 0){	
				tocarMusica(85, GOTmelody, sizeof(GOTmelody)/sizeof(GOTmelody[0]));
			}
		}

		pio_set(PIOC, LED1_IDX_MASK);
		pio_set(PIOD, LED2_IDX_MASK);
		pio_set(PIOD, LED3_IDX_MASK);
	
	}
  
  return 0;
}
