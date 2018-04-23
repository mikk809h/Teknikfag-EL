
	list			p=16f688		; list directive to define processor

	#include		<P16F688.inc>		; processor specific variable definitions

	

	__CONFIG 		_FCMEN_OFF&_IESO_OFF&_BOD_OFF&_CPD_OFF&_CP_OFF&_PWRTE_OFF&_WDT_OFF&_INTRC_OSC_CLKOUT&_MCLRE_OFF


	ORG				0x000			;processor reset vector

  	goto			init			;g�r til ops�tning af programmet


init

;Ops�tning af porte

		MOVLW	b'00000111'
		MOVWF	CMCON0			;Her sl�r vi comperatorerne fra for at frig�re alle porte

		BSF		STATUS,RP0      ;Skift til bank#0


;Ops�tning af outputs

		BCF		TRISC,4			;Output til TX(UART)


;Ops�tning af inputs

		BSF		TRISC,0			;Input for PIR signal
		BCF		ANSEL,4

		BSF		TRISC,1			;Input for ACC. signal
		BCF		ANSEL,5

		BSF		TRISC,2			;Input for Reed signal
		BCF		ANSEL,6

		BCF		STATUS,RP0		;Skifter til bank#0


;Ops�tning af variabler

		Delay1	EQU 0x20		;S�tter variablen Delay1 til bank 0x20

		Delay2	EQU 0x21		;S�tter variablen Delay2 til bank 0x21

		Delay3	EQU 0x22		;S�tter variablen Delay3 til bank 0x22


;Ops�tning af transmitter

		BcF 	TXSTA,TRMT		; Status bit, kan ikke skrives til. Transmitregister fuld=0 tom=1
		BSF 	TXSTA,BRGH		; s�ttes til h�j da det giver den mindste fejl med 9200 baud rate
		BCF 	TXSTA,SYNC		; Cleares da der skal k�res assyncron mode
		BSF 	TXSTA,TXEN		; N�r TXEN s�ttes er porten klar til at sende data

		MOVLW 	D'25'			; S�t W til decimaltallet, 25
		MOVWF 	SPBRG			; Flyt W til SPBRG s� boud rate generator f�r en error% p� 0.16

		BSF		RCSTA,SPEN		; Aktiverer den serielle port

		CLRW					; Clear W, s� nyt data kan blive lageret i W

		goto	main			;g�r til hovedprogrammet


main


		BTFSC	PORTC,0			;Tester om PORTC,0 har input
		GOTO	pir_on			;G�r til pir_on
		MOVLW	b'10000000'		;S�tter w til b'10000000'
		MOVWF 	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder p� loop, s� dataen n�r at blive sendt inden programmet forts�tter

acc
		BTFSC	PORTC,1			;Tester om PORTC,1 har input
		GOTO	acc_on			;G�r til acc_on
		MOVLW	b'10001000'		;S�tter w til b'10001000'
		MOVWF 	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder p� loop, s� dataen n�r at blive sendt inden programmet forts�tter

reed
		BTFSC	PORTC,2			;Tester om PORTC,2 har input
		GOTO	reed_on			;G�r til reed_on
		MOVLW	b'10010000'		;S�tter w til b'10010000'
		MOVWF 	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder p� loop, s� dataen n�r at blive sendt inden programmet forts�tter

		GOTO	main			;G�r tilbage til starten af programmet

pir_on
		MOVLW	b'10000001'		;S�tter w til b'10000001'
		MOVWF	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder p� loop, s� dataen n�r at blive sendt inden programmet forts�tter
		GOTO	acc				;Forts�tter programmet fra acc

acc_on
		MOVLW	b'10001001'		;S�tter w til b'10001001'
		MOVWF	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder p� loop, s� dataen n�r at blive sendt inden programmet forts�tter
		GOTO	reed			;Forts�tter programmet fra reed

reed_on
		MOVLW	b'10010001'		;S�tter w til b'10010001'
		MOVWF	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder p� loop, s� dataen n�r at blive sendt inden programmet forts�tter
		GOTO	main			;G�r tilbage til starten af programmet


loop	
		
		MOVLW	D'150'		; Tidsforsinkelse p� ca. 250 ms
		MOVWF	Delay3		;
Loop12	MOVLW	D'10'		; h'73'= 115 t�lles op <=> 255-115 =140 der t�lles ned
		MOVWF	Delay1		;
		MOVLW	D'10'		; h'DF'= 223 t�lles op <=> 255-223 =32 der t�lles ned
		MOVWF	Delay2		;
Loop1	DECFSZ	Delay1,F	;
		GOTO	Loop1		;
		MOVLW	D'10'		;
		MOVWF	Delay1		;
		DECFSZ	Delay2,F	;
		GOTO	Loop1		;
		DECFSZ	Delay3,F	;
		GOTO	Loop12		;
		RETURN
END
