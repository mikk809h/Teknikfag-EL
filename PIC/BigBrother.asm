
	list			p=16f688		; list directive to define processor

	#include		<P16F688.inc>		; processor specific variable definitions

	

	__CONFIG 		_FCMEN_OFF&_IESO_OFF&_BOD_OFF&_CPD_OFF&_CP_OFF&_PWRTE_OFF&_WDT_OFF&_INTRC_OSC_CLKOUT&_MCLRE_OFF


	ORG				0x000			;processor reset vector

  	goto			init			;går til opsætning af programmet


init

;Opsætning af porte

		MOVLW	b'00000111'
		MOVWF	CMCON0			;Her slår vi comperatorerne fra for at frigøre alle porte

		BSF		STATUS,RP0      ;Skift til bank#0


;Opsætning af outputs

		BCF		TRISC,4			;Output til TX(UART)


;Opsætning af inputs

		BSF		TRISC,0			;Input for PIR signal
		BCF		ANSEL,4

		BSF		TRISC,1			;Input for ACC. signal
		BCF		ANSEL,5

		BSF		TRISC,2			;Input for Reed signal
		BCF		ANSEL,6

		BCF		STATUS,RP0		;Skifter til bank#0


;Opsætning af variabler

		Delay1	EQU 0x20		;Sætter variablen Delay1 til bank 0x20

		Delay2	EQU 0x21		;Sætter variablen Delay2 til bank 0x21

		Delay3	EQU 0x22		;Sætter variablen Delay3 til bank 0x22


;Opsætning af transmitter

		BcF 	TXSTA,TRMT		; Status bit, kan ikke skrives til. Transmitregister fuld=0 tom=1
		BSF 	TXSTA,BRGH		; sættes til høj da det giver den mindste fejl med 9200 baud rate
		BCF 	TXSTA,SYNC		; Cleares da der skal køres assyncron mode
		BSF 	TXSTA,TXEN		; Når TXEN sættes er porten klar til at sende data

		MOVLW 	D'25'			; Sæt W til decimaltallet, 25
		MOVWF 	SPBRG			; Flyt W til SPBRG så boud rate generator får en error% på 0.16

		BSF		RCSTA,SPEN		; Aktiverer den serielle port

		CLRW					; Clear W, så nyt data kan blive lageret i W

		goto	main			;går til hovedprogrammet


main


		BTFSC	PORTC,0			;Tester om PORTC,0 har input
		GOTO	pir_on			;Går til pir_on
		MOVLW	b'10000000'		;Sætter w til b'10000000'
		MOVWF 	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder på loop, så dataen når at blive sendt inden programmet fortsætter

acc
		BTFSC	PORTC,1			;Tester om PORTC,1 har input
		GOTO	acc_on			;Går til acc_on
		MOVLW	b'10001000'		;Sætter w til b'10001000'
		MOVWF 	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder på loop, så dataen når at blive sendt inden programmet fortsætter

reed
		BTFSC	PORTC,2			;Tester om PORTC,2 har input
		GOTO	reed_on			;Går til reed_on
		MOVLW	b'10010000'		;Sætter w til b'10010000'
		MOVWF 	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder på loop, så dataen når at blive sendt inden programmet fortsætter

		GOTO	main			;Går tilbage til starten af programmet

pir_on
		MOVLW	b'10000001'		;Sætter w til b'10000001'
		MOVWF	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder på loop, så dataen når at blive sendt inden programmet fortsætter
		GOTO	acc				;Fortsætter programmet fra acc

acc_on
		MOVLW	b'10001001'		;Sætter w til b'10001001'
		MOVWF	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder på loop, så dataen når at blive sendt inden programmet fortsætter
		GOTO	reed			;Fortsætter programmet fra reed

reed_on
		MOVLW	b'10010001'		;Sætter w til b'10010001'
		MOVWF	TXREG			;Flytter dataen fra w ind i TXREG
		CALL	loop			;Kalder på loop, så dataen når at blive sendt inden programmet fortsætter
		GOTO	main			;Går tilbage til starten af programmet


loop	
		
		MOVLW	D'150'		; Tidsforsinkelse på ca. 250 ms
		MOVWF	Delay3		;
Loop12	MOVLW	D'10'		; h'73'= 115 tælles op <=> 255-115 =140 der tælles ned
		MOVWF	Delay1		;
		MOVLW	D'10'		; h'DF'= 223 tælles op <=> 255-223 =32 der tælles ned
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
