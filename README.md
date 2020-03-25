# Buzzer 8-bit player Embarcado
Projeto de Computação Embarcada do 5° semestre de Engenharia de Computação - INSPER.

 O projeto consiste em um player de músicas 8-bit feito em uma placa SAM-E70 com algumas funcionalidades

## Descrição do projeto

Para a realização do projeto, foram utilizados três componentes:

 - Placa ATSAME70-XPLD (SAM-E70 Atmel)
	 - LED principal, botão de reset e um botão para uso geral
 - Tone Buzzer
 - 
 - Placa secundaria OLED XPLAINED PRO
	 - 3 LEDs e 3 botões para uso genérico

O Buzzer e a Placa secundaria estão conectados na Placa SAM-E70

### Funcionalidades

O projeto contem três funcionalidades:

 - Botão 1 do OLED
	 - Botão de pause
- Botão 2 do OLED
	 - Botão para alternar músicas
- Botão 3 do OLED
	 - Botão para iniciar uma música do começo (usado apenas para começar a tocar depois que se acaba, pela primeira vez depois de compilado ou para iniciar depois de se trocar de música)



## Configurando os componentes e ambiente

### Ligações elétricas
Só precisamos nos preocupar com três conexões na hora de configurar os componentes:

 - Qualquer um dos dois pinos do Tone Buzzer será conectado no GND da SAM-E70 

 - O outro pino do Buzzer será conectado na entrada PA19 

 - A placa OLED será conectada na EXT2 da SAM-E70, envolvendo todos os pinos (as ligações deles se encontram no código)

### Ambiente de compilação
Para passar o código para a placa, usamos o [Atmel Studio](https://www.microchip.com/mplab/avr-support/atmel-studio-7). Ele será o responsável pela compilação do código na placa

Baixe o Atmel Studio e abra o arquivo com extenção "solução", plugue a placa e depois clique em 'Star Without Debugging'

A placa esta pronta para ser usada


## Utilização 

Clone este repositório em sua máquina
	
	git clone https://github.com/fontes99/Embarcado-player.git



#
### Referências
As listas das músicas e a lógica de como utilizá-las foram baseadas da lógica apresentada no site [Dragão sem Chama](https://dragaosemchama.com/en/2019/02/songs-for-arduino/)


<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE0MjE3ODkzMSwxNDkwOTA0OTQ4LDE5MT
A0NDkzMjAsMzMxNDk5ODExLC0xMTc5MTAwOTMzLC0yOTI0Mjk5
OTMsMTIyMjY3Nzk2MywxODA0NjcyMTExLDE3OTIyMTA0ODBdfQ
==
-->