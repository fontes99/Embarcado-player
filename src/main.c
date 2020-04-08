/**
 *   PEDRO VERO FONTES
 * 
 * 5 semestre - Eng. da Computação - Insper
 * PROFESSOR : Rafael Corsi - rafael.corsi@insper.edu.br
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

#include "SOS.h"
#include "GOT.h"
#include "HP.h"

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
#define BUT1_PRIORITY 2                 // Prioridade 2

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
#define BUT3_PRIORITY 2                 // Prioridade 2

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

// tempo de uma nota, usada para calcular o tempo da nota específica
#define NOTE_TIME    60000 * 4     // 60seg * tempo (4 tempos)

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

volatile char pause;
volatile char comp;
volatile char musica;

void pausaMusica(void)
{
	pause = 1;
}

void alteraMusica(void){

	if (musica == 2){
		musica = 0;
	}
	else{
		musica += 1;
	}	
}

/************************************************************************/
/* Struct                                                              */
/************************************************************************/

typedef struct{
	int *melodia;
	int tempo;
	int n_notas;
} song;

/************************************************************************/
/* Declarar funções                                                     */
/************************************************************************/

void init(void);
void tocarMusica(int tempo, int melody[], int n_notas);

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
	pmc_enable_periph_clk(BUZZ_PIO_ID);
	pio_configure(BUZZ_PIO, PIO_OUTPUT_0, BUZZ_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_1, LED1_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED2_PIO_ID);
	pio_configure(LED2_PIO, PIO_OUTPUT_1, LED2_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED3_PIO_ID);
	pio_configure(LED3_PIO, PIO_OUTPUT_1, LED3_IDX_MASK, PIO_DEFAULT);
	
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	// configura pino ligado ao botão como entrada com um pull-up.
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  	pio_set_debounce_filter(BUT_PIO, BUT_IDX_MASK, 100);

	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_IDX_MASK, 100);
	
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_IDX_MASK, 100);
	
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_IDX_MASK, 100);
	
	// Ativa interrupção
	pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);
	pio_enable_interrupt(BUT1_PIO, BUT1_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_IDX_MASK);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: pausaMusica()
	pio_handler_set(BUT1_PIO,
					BUT1_PIO_ID,
					BUT1_IDX_MASK,
					PIO_IT_RISE_EDGE,
					pausaMusica);

	pio_handler_set(BUT3_PIO,
					BUT3_PIO_ID,
					BUT3_IDX_MASK,
					PIO_IT_RISE_EDGE,
					alteraMusica);

	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, BUT1_PRIORITY); 
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, BUT3_PRIORITY); 
	
	pause = 0;
	comp = 0;
	musica = 0;
	
}

// função para tocar musica. chamada para tocar musicas diferentes

void tocarMusica(int tempo, int melody[], int n_notas){ 

	// this calculates the duration of a whole note in ms
	int wholenote = (NOTE_TIME) / tempo;

	int divider = 0;
	float noteDuration = 0;

	// iterate over the notes of the song.
	// Remember, the array is twice the number of notes (notes + durations)
	for (int thisNote = 0; thisNote < n_notas; thisNote += 2) {

		// checa se deu pause
		if(pause){
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
			pause = 0;
		}
		
		// checa se trocou de música
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
			
			delay_ms(1000*toca*0.1);                    // 10% de pausa para diferenciar notas iguais
		}
	}
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void){ 

	// - *1) - 
	// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
	// there are two values per note (pitch and duration), so for each note there are four bytes
	
	song got;
	got.melodia = &GOTmelody;
	got.tempo = GOT_TEMPO;
	got.n_notas = sizeof(GOTmelody)/sizeof(GOTmelody[0]);  // *1)

	song storms;
	storms.melodia = &SOSmelody;
	storms.tempo = SOS_TEMPO;
	storms.n_notas = sizeof(SOSmelody)/sizeof(SOSmelody[0]);   // *1)

	song hp;
	hp.melodia = &HPmelody;
	hp.tempo = HP_TEMPO;
	hp.n_notas = sizeof(HPmelody)/sizeof(HPmelody[0]);   // *1)

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
				tocarMusica(storms.tempo, storms.melodia, storms.n_notas);
			}
		}
		else if (musica == 0){
			pio_clear(PIOD, LED3_IDX_MASK);
			
			if (pio_get(BUT2_PIO, PIO_DEFAULT, BUT2_IDX_MASK) == 0){	
				tocarMusica(got.tempo, got.melodia, got.n_notas);
			}
		}

		else if (musica == 2){
			pio_clear(PIOD, LED3_IDX_MASK);
			pio_clear(PIOC, LED1_IDX_MASK);
			
			if (pio_get(BUT2_PIO, PIO_DEFAULT, BUT2_IDX_MASK) == 0){	
				tocarMusica(hp.tempo, hp.melodia, hp.n_notas);
			}
		}

		pio_set(PIOC, LED1_IDX_MASK);
		pio_set(PIOD, LED2_IDX_MASK);
		pio_set(PIOD, LED3_IDX_MASK);
		
	}
  
  return 0;
}
