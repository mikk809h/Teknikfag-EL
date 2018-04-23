
	list			p=16f684		; list directive to define processor

	#include		<P16F684.inc>		; processor specific variable definitions

	

	__CONFIG    	_CP_OFF & _CPD_OFF & _BOD_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT & _MCLRE_ON & _FCMEN_OFF & _IESO_OFF







;**********************************************************************

	ORG				0x000			; processor reset vector

  	goto			Ops�tning				; go to beginning of program





;**********************************************************************







Ops�tning



;	Ops�tning af porte

		

		MOVLW	b'00000111'

		MOVWF	CMCON0			;Her sl�r vi comperatorerne fra for at frig�re alle porte



		BSF		STATUS,RP0      ;Skift til BANK 1 for at lave ops�tning af porte



		BCF		TRISC,RC5		;PortA,5 s�ttes til output



		BSF		TRISC,3			;G�r ben 7 til input

		BSF		ANSEL,7			;G�r ben 7 til analog



;   Ops�tning af Puls With Modulator



		movlw	D'255'	

		movwf	PR2				; PWM perioden s�ttes i PR2,her til 244Hz

		BCF    	STATUS, RP0  	; Der skiftes til bank 0

		movlw	b'00000111'

		movwf	T2CON			; s�tter Timer2 op med 1:16 prescaler

		movlw	b'00001100'		; S�tter pulsbrede modulatoren op med en PWM udgang

		movwf	CCP1CON 	



;	Ops�tning af Analog til Digital converterenADC and PWM configuration



		bsf    	STATUS, RP0 	; Der skiftes til bank 1

		movlw 	b'00000000' 

		movwf 	ADCON1			; set ADC Frc clock

		bcf    	STATUS, RP0 	; Der skiftes til bank 0

		movlw	b'00011101'

		movwf	ADCON0



		delay1	EQU 0x20

		delay2	EQU 0x21

		over	EQU 0x22	;s�tter variablen "over" til bank 0x22

		movlw	d'90'		;s�tter "over"'s v�rdi til 90

		movwf	over		

		under	EQU	0x28	;s�tter variablen "under" til bank 0x28

		movlw	d'80'		;s�tter "under"'s v�rdig til 80

		movwf	under

		goto	main		;g�r til hovedprogrammet


;***********************************************************************


main

		call	ADcheck		;g�r til label ADcheck


		subwf	over,0		;tr�kker v�rdien af "over" fra "w"

		btfss	STATUS,C	;tjekker om v�rdien af "w" er mindre en nul

		GOTO	slow		;g�r til label slow 



		movf	ADRESH,W	;f�r fat i ADRESH igen

		subwf	under,0		;tr�kker "under" fra "w"

		btfsc	STATUS,C	;tj�kker om v�rdien af "w" er mindre en nul
	
		GOTO	slow		;g�r til label slow

		GOTO	fast		;g�r til label fast



slow

		movlw	d'86'		;s�tter v�rdien af CCPR1L til 86, aka 12/255 = 0.047. 0.047*86 = 4.04 volt

		movwf	CCPR1L

		GOTO	main		;g�r tilbage til starten af main



fast

		movlw	d'107'		;s�tter v�rdien af CCPR1L til 107, aka 5.02 volt

		movwf	CCPR1L

		GOTO	main



ADcheck

		bsf		ADCON0,GO	;s�tter ADCON0 til GO



true

		btfsc	ADCON0,GO	;Tjekker om ADCON0 har f�et en v�rdi, skipper n�ste kommando hvis den har

		GOTO	true		;g�r tilbage til label "true"

		movf	ADRESH,W	;S�tter "w" til ADRESH's v�rdi

		return				;g�r tilbage og forts�tter efter ADcheck

END
