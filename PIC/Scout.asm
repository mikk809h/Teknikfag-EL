
	list			p=16f684		; list directive to define processor

	#include		<P16F684.inc>		; processor specific variable definitions


	__CONFIG 		_FCMEN_OFF&_IESO_OFF&_BOD_OFF&_CPD_OFF&_CP_OFF&_PWRTE_OFF&_WDT_OFF&_INTRC_OSC_CLKOUT&_MCLRE_OFF


	ORG				0x000			; processor reset vector

  	goto			Opsætning		; go to beginning of program


Opsætning

;Opsætning af porte

		MOVLW	b'00000111'
		MOVWF	CMCON0			;Her slår vi comperatorerne fra for at frigøre alle porte

		BSF		STATUS,RP0      ;Skift til BANK 1 for at lave opsætning af porte


;Opsætning af outputs

		BCF		TRISC,5			;PORTC,5 sættes til output (ACC)

		BCF		TRISC,4			;PORTA,2 sættes til output (PIR sensor)

		BCF		TRISC,3			;PORTC,0 sættes til output (REED relay)


;Opsætning af inputs

		BSF		TRISC,0			;Gør PORTC,0 til input (ACC)
		BSF		ANSEL,4			;Gør PORTC,0 til analog (ACC)

		BSF		TRISC,1			;Gør PORTC,1 til input (REED relay)
		BCF		ANSEL,5			;Gør PORTC,1 til digital (REED relay)

		BSF		TRISC,2			;Gør PORTC,2 til input (PIR sensor)
		BCF 	ANSEL,6			;Gør PORTC,2 til digital (PIR sensor)


;Opsætning af Puls With Modulator

		MOVLW	D'255'			;Sætter w til 255

		MOVWF	PR2				;PWM perioden sættes i PR2,her til 244Hz

		BCF    	STATUS, RP0  	;Skifter til bank#0

		MOVLW	b'00000111'		

		MOVWF	T2CON			; sætter Timer2 op med 1:16 prescaler

		MOVLW	b'00001100'		; Sætter pulsbrede modulatoren op med en PWM udgang

		MOVWF	CCP1CON 	


;Opsætning af Analog til Digital converterenADC and PWM configuration

		BSF    	STATUS, RP0 	; Der skiftes til bank 1

		MOVLW 	b'00000000' 

		MOVWF 	ADCON1			; set ADC Frc clock

		BCF    	STATUS, RP0 	; Der skiftes til bank 0

		MOVLW	b'00010001'		;Sætter ADCON til PORTC,0

		MOVWF	ADCON0


		Delay1	EQU 0x20

		Delay2	EQU 0x21

		Delay3	EQU 0x22

		over	EQU 0x23	;sætter variablen "over" til bank 0x23

		MOVLW	d'170'		;sætter "over"'s værdi til 170

		MOVWF	over		

		under	EQU	0x28	;sætter variablen "under" til bank 0x28

		MOVLW	d'50'		;sætter "under"'s værdig til 50

		MOVWF	under

		GOTO	main		;går til hovedprogrammet


main

		BTFSS	PORTC,2
		BCF		PORTC,4		;Clear PORTA,2 (PIR sensor)

		BTFSC	PORTC,2		;Tester om PORTC,2 har input (PIR sensor)
		BSF		PORTC,4		;Åbner PORTC,4 (PIR sensor)


		BTFSS	PORTC,1
		BCF		PORTC,3		;Clear PORTC,1 (REED relay)

		BTFSC	PORTC,1		;Tester om PORTC,0 har input (REED relay)
		BSF		PORTC,3		;Åbner PORTC,1 (REED relay)


		CALL	ADcheck		;går til label ADcheck


		SUBWF	over,0		;trækker værdien af "over" fra "w"

		BTFSS	STATUS,C	;tjekker om værdien af "w" er mindre en nul
		GOTO	on			;går til label on


		MOVF	ADRESH,W	;får fat i ADRESH igen

		SUBWF	under,0		;trækker "under" fra "w"

		BTFSC	STATUS,C	;tjækker om værdien af "w" er mindre en nul
		GOTO	on			;går til label on

		GOTO	off			;går til label off


on

		MOVLW	d'106'		;sætter værdien af CCPR1L til 106, aka 5.0 volt
		MOVWF	CCPR1L

		GOTO	loop

		GOTO	main		;går tilbage til starten af main


off

		MOVLW	d'0'		;sætter værdien af CCPR1L til 0, 0 volt
		MOVWF	CCPR1L

		GOTO	main


ADcheck

		BSF		ADCON0,GO	;sætter ADCON0 til GO


true

		BTFSC	ADCON0,GO	;Tjekker om ADCON0 har fået en værdi
		GOTO	true		;går tilbage til label "true"

		MOVF	ADRESH,W	;Sætter "w" til ADRESH's værdi

		RETURN				;går tilbage og fortsætter efter ADcheck

loop
	
			MOVLW	D'100'				; Tidsforsinkelse på ca. 250 ms
			MOVWF	Delay3				;
Loop12		MOVLW	D'100'				; h'73'= 115 tælles op <=> 255-115 =140 der tælles ned
			MOVWF	Delay1				;
			MOVLW	D'100'				; h'DF'= 223 tælles op <=> 255-223 =32 der tælles ned
			MOVWF	Delay2				;
Loop1		DECFSZ	Delay1,F			;
			GOTO	Loop1				;
			MOVLW	D'100'				;
			MOVWF	Delay1				;
			DECFSZ	Delay2,F			;
			GOTO	Loop1				;
			DECFSZ	Delay3,F			;
			GOTO	Loop12				;
			
			GOTO	main
END
