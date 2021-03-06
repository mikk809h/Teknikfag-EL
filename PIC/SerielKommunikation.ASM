;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		SerielKommunikation.asm
;   Dato:		05.01.2016
;	forfatter:	Mathias Bejlegaard Madsen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;	Seriel kommunikation mellem to PICKITS, hvor den ene er transmitter og den anden er reciever.
;	Transmitteren bestemmer, hvilken data der skal sendes til recieveren.
;	Recieven bestemmer, hvad den modtagede data skal anvendes til.
;	I dette program afg�rer transmitteren, hvilke LEDer p� PORTB, recieveren skal have t�ndt.

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
; ******* OPS�TNING AF PROGRAM POINTERE ***************************************************
			org		0x0000				;Programstart efter et reset
			GOTO	init				;G� til ops�tning
			org		0x0005				;F�rste position efter interrupt-vektor
; ******* INCLUDEREDE FILER ***************************************************************
	#Include 	"SDUboard.LIB"			; SDU PICboard, bibliotek med LCD- og A/D-routiner
	#Include	"Delay.LIB"				; Tilf�jer delay filen
	#Include	"Transmitter.inc"		; Tilf�jer Transmitter filen
	#Include	"Reciever.inc"			; Tilf�jer Reciever filen
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

MAINTRANSMITTER ; PIC skal v�re transmitter (MAINRECIEVER skal udkommenteres)
			;BSF		PORTB,1
			MOVLW	0XAA				; S�t W til et givet data du bestemmer!
			CALL	TRANSMITTER			; Send dataen fra W, ved at k�re TRANSMITTER
			CALL	DELAY				; Vent s� vi sikre, at dette arbejde er f�rdigt
			;BSF		PORTB,2
			MOVLW	0X55				; S�t W til et givet data du bestemmer!	
			CALL	TRANSMITTER			; Send dataen fra W, ved at k�re TRANSMITTER
			CALL 	DELAY				; Vent s� vi sikre, at dette arbejde er f�rdigt	
			BSF		PORTB,3
			CLRF	PORTB
			GOTO	MAINTRANSMITTER		; Loop overst�ende i MAINTRANSMITTER

MAINRECIEVER ; PIC skal v�re reciever (MAINTRANSMITTER skal udkommenteres)
		;	BSF		PORTB,1
			CALL 	RECIEVER			; K�r RECIEVER, for at modtage data
		;	BSF		PORTB,2
			MOVF 	RESULTAT,W			; Flyt det modtagede data fra RECIEVER til W
		;	BSF		PORTB,3
			MOVWF 	PORTB				; Flyt det modtagede data i W til PORTB (Lysdioder)
		;	BSF		PORTB,4
			CALL	DELAY
			GOTO	MAINRECIEVER		; Loop overst�ende i MAINRECIEVER

; ******* PROGRAM AFSLUTTET ***************************************************************		
			END							; Her slutter programmet