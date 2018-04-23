
	list			p=16f684		; list directive to define processor

	#include		<P16F684.inc>		; processor specific variable definitions


	__CONFIG 		_FCMEN_OFF&_IESO_OFF&_BOD_OFF&_CPD_OFF&_CP_OFF&_PWRTE_OFF&_WDT_OFF&_INTRC_OSC_CLKOUT&_MCLRE_OFF


	ORG				0x000			; processor reset vector

  	goto			Ops�tning		; go to beginning of program


Ops�tning

;Ops�tning af porte

		MOVLW	b'00000111'
		MOVWF	CMCON0			;Her sl�r vi comperatorerne fra for at frig�re alle porte

		BSF		STATUS,RP0      ;Skift til BANK 1 for at lave ops�tning af porte


;Ops�tning af outputs

		BCF		TRISC,5			;PORTC,5 s�ttes til output (ACC)

		BCF		TRISC,4			;PORTA,2 s�ttes til output (PIR sensor)

		BCF		TRISC,3			;PORTC,0 s�ttes til output (REED relay)


;Ops�tning af inputs

		BSF		TRISC,0			;G�r PORTC,0 til input (ACC)
		BSF		ANSEL,4			;G�r PORTC,0 til analog (ACC)

		BSF		TRISC,1			;G�r PORTC,1 til input (REED relay)
		BCF		ANSEL,5			;G�r PORTC,1 til digital (REED relay)

		BSF		TRISC,2			;G�r PORTC,2 til input (PIR sensor)
		BCF 	ANSEL,6			;G�r PORTC,2 til digital (PIR sensor)


;Ops�tning af Puls With Modulator

		MOVLW	D'255'			;S�tter w til 255

		MOVWF	PR2				;PWM perioden s�ttes i PR2,her til 244Hz

		BCF    	STATUS, RP0  	;Skifter til bank#0

		MOVLW	b'00000111'		

		MOVWF	T2CON			; s�tter Timer2 op med 1:16 prescaler

		MOVLW	b'00001100'		; S�tter pulsbrede modulatoren op med en PWM udgang

		MOVWF	CCP1CON 	


;Ops�tning af Analog til Digital converterenADC and PWM configuration

		BSF    	STATUS, RP0 	; Der skiftes til bank 1

		MOVLW 	b'00000000' 

		MOVWF 	ADCON1			; set ADC Frc clock

		BCF    	STATUS, RP0 	; Der skiftes til bank 0

		MOVLW	b'00010001'		;S�tter ADCON til PORTC,0

		MOVWF	ADCON0


		Delay1	EQU 0x20

		Delay2	EQU 0x21

		Delay3	EQU 0x22

		over	EQU 0x23	;s�tter variablen "over" til bank 0x23

		MOVLW	d'170'		;s�tter "over"'s v�rdi til 170

		MOVWF	over		

		under	EQU	0x28	;s�tter variablen "under" til bank 0x28

		MOVLW	d'50'		;s�tter "under"'s v�rdig til 50

		MOVWF	under

		GOTO	main		;g�r til hovedprogrammet


main

		BTFSS	PORTC,2
		BCF		PORTC,4		;Clear PORTA,2 (PIR sensor)

		BTFSC	PORTC,2		;Tester om PORTC,2 har input (PIR sensor)
		BSF		PORTC,4		;�bner PORTC,4 (PIR sensor)


		BTFSS	PORTC,1
		BCF		PORTC,3		;Clear PORTC,1 (REED relay)

		BTFSC	PORTC,1		;Tester om PORTC,0 har input (REED relay)
		BSF		PORTC,3		;�bner PORTC,1 (REED relay)


		CALL	ADcheck		;g�r til label ADcheck


		SUBWF	over,0		;tr�kker v�rdien af "over" fra "w"

		BTFSS	STATUS,C	;tjekker om v�rdien af "w" er mindre en nul
		GOTO	on			;g�r til label on


		MOVF	ADRESH,W	;f�r fat i ADRESH igen

		SUBWF	under,0		;tr�kker "under" fra "w"

		BTFSC	STATUS,C	;tj�kker om v�rdien af "w" er mindre en nul
		GOTO	on			;g�r til label on

		GOTO	off			;g�r til label off


on

		MOVLW	d'106'		;s�tter v�rdien af CCPR1L til 106, aka 5.0 volt
		MOVWF	CCPR1L

		GOTO	loop

		GOTO	main		;g�r tilbage til starten af main


off

		MOVLW	d'0'		;s�tter v�rdien af CCPR1L til 0, 0 volt
		MOVWF	CCPR1L

		GOTO	main


ADcheck

		BSF		ADCON0,GO	;s�tter ADCON0 til GO


true

		BTFSC	ADCON0,GO	;Tjekker om ADCON0 har f�et en v�rdi
		GOTO	true		;g�r tilbage til label "true"

		MOVF	ADRESH,W	;S�tter "w" til ADRESH's v�rdi

		RETURN				;g�r tilbage og forts�tter efter ADcheck

loop
	
			MOVLW	D'100'				; Tidsforsinkelse p� ca. 250 ms
			MOVWF	Delay3				;
Loop12		MOVLW	D'100'				; h'73'= 115 t�lles op <=> 255-115 =140 der t�lles ned
			MOVWF	Delay1				;
			MOVLW	D'100'				; h'DF'= 223 t�lles op <=> 255-223 =32 der t�lles ned
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
