;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		SerielKommunikation.asm
;   Dato:		05.01.2016
;	forfatter:	Mathias Bejlegaard Madsen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;	Seriel kommunikation mellem to PICKITS, hvor den ene er transmitter og den anden er reciever.
;	Transmitteren bestemmer, hvilken data der skal sendes til recieveren.
;	Recieven bestemmer, hvad den modtagede data skal anvendes til.
;	I dette program afgører transmitteren, hvilke LEDer på PORTB, recieveren skal have tændt.

; ******* PROCESSOR DEFINITIONER **********************************************************
	processor	16f877a					;Sets processor
	#include 	p16f877a.inc
	errorlevel -302						;fjerner meddelser om forkerte banker fra fejl listen
	errorlevel -305						;fjerner meddelser om forkerte banker fra fejl listen
	errorlevel -307
	errorlevel -207
; ******* COMPILER configuration bits *****************************************************
	__config	_HS_OSC & _PWRTE_OFF & _WDT_OFF & _CP_OFF & _CPD_OFF & _LVP_OFF

; ******* DEFFINITION AF VARIABLE *********************************************************
	#include	"SDUboard.var"			;Variable som bruges af SDUborad (0x79 - 0x7F)
	RESULTAT	EQU 0X20				; Definer RESULTAT til lagring af modtaget data 
; ******* OPSÆTNING AF PROGRAM POINTERE ***************************************************
			org		0x0000				;Programstart efter et reset
			GOTO	init				;Gå til opsætning
			org		0x0005				;Første position efter interrupt-vektor
; ******* INCLUDEREDE FILER ***************************************************************
	#Include 	"SDUboard.LIB"			; SDU PICboard, bibliotek med LCD- og A/D-routiner
	#Include	"Delay.LIB"				; Tilføjer delay filen
	#Include	"Transmitter.inc"		; Tilføjer Transmitter filen
	#Include	"Reciever.inc"			; Tilføjer Reciever filen
; ******* INITIALISERING AF CONTROLLER *****************************************************
init		
			BSF		STATUS,RP0			; definer porte - start (bank1)
			CLRF	TRISB				; lav port C til udgange
			BCF		TRISC,6				; RC6/TX/CK
			BSF		TRISC,7				; RC7/RX/DT
			BCF		STATUS,RP0			; definer porte - slut (bank0)
;			CLRF	PORTB				; sluk alle lysdioder
			BSF		PORTB,0
;			CALL 	TRANSMITTERSETUP	; transmitter (Udkommenter RECIEVERSETUP)
;			GOTO	MAINTRANSMITTER		; transmitter (Udkommenter MAINRECIEVER)
			CALL	RECIEVERSETUP		; reciever (Udkommenter TRANSMITTERSETUP)
			GOTO	MAINRECIEVER		; reciever (Udkommenter MAINTRANSMITTER)

; ******* HOVEDPROGRAM ********************************************************************

MAINTRANSMITTER ; PIC skal være transmitter (MAINRECIEVER skal udkommenteres)
			;BSF		PORTB,1
			MOVLW	0XAA				; Sæt W til et givet data du bestemmer!
			CALL	TRANSMITTER			; Send dataen fra W, ved at køre TRANSMITTER
			CALL	DELAY				; Vent så vi sikre, at dette arbejde er færdigt
			;BSF		PORTB,2
			MOVLW	0X55				; Sæt W til et givet data du bestemmer!	
			CALL	TRANSMITTER			; Send dataen fra W, ved at køre TRANSMITTER
			CALL 	DELAY				; Vent så vi sikre, at dette arbejde er færdigt	
			BSF		PORTB,3
			CLRF	PORTB
			GOTO	MAINTRANSMITTER		; Loop overstående i MAINTRANSMITTER

MAINRECIEVER ; PIC skal være reciever (MAINTRANSMITTER skal udkommenteres)
		;	BSF		PORTB,1
			CALL 	RECIEVER			; Kør RECIEVER, for at modtage data
		;	BSF		PORTB,2
			MOVF 	RESULTAT,W			; Flyt det modtagede data fra RECIEVER til W
		;	BSF		PORTB,3
			MOVWF 	PORTB				; Flyt det modtagede data i W til PORTB (Lysdioder)
		;	BSF		PORTB,4
			CALL	DELAY
			GOTO	MAINRECIEVER		; Loop overstående i MAINRECIEVER

; ******* PROGRAM AFSLUTTET ***************************************************************		
			END							; Her slutter programmet