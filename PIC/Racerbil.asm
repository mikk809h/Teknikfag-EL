
	list			p=16f684		; list directive to define processor

	#include		<P16F684.inc>		; processor specific variable definitions

	

	__CONFIG    	_CP_OFF & _CPD_OFF & _BOD_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT & _MCLRE_ON & _FCMEN_OFF & _IESO_OFF







;**********************************************************************

	ORG				0x000			; processor reset vector

  	goto			Opsætning				; go to beginning of program





;**********************************************************************







Opsætning



;	Opsætning af porte

		

		MOVLW	b'00000111'

		MOVWF	CMCON0			;Her slår vi comperatorerne fra for at frigøre alle porte



		BSF		STATUS,RP0      ;Skift til BANK 1 for at lave opsætning af porte



		BCF		TRISC,RC5		;PortA,5 sættes til output



		BSF		TRISC,3			;Gør ben 7 til input

		BSF		ANSEL,7			;Gør ben 7 til analog



;   Opsætning af Puls With Modulator



		movlw	D'255'	

		movwf	PR2				; PWM perioden sættes i PR2,her til 244Hz

		BCF    	STATUS, RP0  	; Der skiftes til bank 0

		movlw	b'00000111'

		movwf	T2CON			; sætter Timer2 op med 1:16 prescaler

		movlw	b'00001100'		; Sætter pulsbrede modulatoren op med en PWM udgang

		movwf	CCP1CON 	



;	Opsætning af Analog til Digital converterenADC and PWM configuration



		bsf    	STATUS, RP0 	; Der skiftes til bank 1

		movlw 	b'00000000' 

		movwf 	ADCON1			; set ADC Frc clock

		bcf    	STATUS, RP0 	; Der skiftes til bank 0

		movlw	b'00011101'

		movwf	ADCON0



		delay1	EQU 0x20

		delay2	EQU 0x21

		over	EQU 0x22	;sætter variablen "over" til bank 0x22

		movlw	d'90'		;sætter "over"'s værdi til 90

		movwf	over		

		under	EQU	0x28	;sætter variablen "under" til bank 0x28

		movlw	d'80'		;sætter "under"'s værdig til 80

		movwf	under

		goto	main		;går til hovedprogrammet


;***********************************************************************


main

		call	ADcheck		;går til label ADcheck


		subwf	over,0		;trækker værdien af "over" fra "w"

		btfss	STATUS,C	;tjekker om værdien af "w" er mindre en nul

		GOTO	slow		;går til label slow 



		movf	ADRESH,W	;får fat i ADRESH igen

		subwf	under,0		;trækker "under" fra "w"

		btfsc	STATUS,C	;tjækker om værdien af "w" er mindre en nul
	
		GOTO	slow		;går til label slow

		GOTO	fast		;går til label fast



slow

		movlw	d'86'		;sætter værdien af CCPR1L til 86, aka 12/255 = 0.047. 0.047*86 = 4.04 volt

		movwf	CCPR1L

		GOTO	main		;går tilbage til starten af main



fast

		movlw	d'107'		;sætter værdien af CCPR1L til 107, aka 5.02 volt

		movwf	CCPR1L

		GOTO	main



ADcheck

		bsf		ADCON0,GO	;sætter ADCON0 til GO



true

		btfsc	ADCON0,GO	;Tjekker om ADCON0 har fået en værdi, skipper næste kommando hvis den har

		GOTO	true		;går tilbage til label "true"

		movf	ADRESH,W	;Sætter "w" til ADRESH's værdi

		return				;går tilbage og fortsætter efter ADcheck

END
