;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		Transmitter.inc
;   Dato:		05.01.2016
;	forfatter:	Mathias Bejlegaard Madsen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;	Seriel kommunikation mellem to PICKITS, hvor den ene er transmitter og den anden
;	er reciever. Transmitteren bestemmer, hvilken data der skal sendes til recieveren.
;	Recieven bestemmer, hvad den modtagede data skal anvendes til.
;	I dette program afg�rer transmitteren, hvilke LEDer p� PORTB, recieveren skal t�nde.

; ******* HOVEDPROGRAM ********************************************************************

TRANSMITTERSETUP
	; s�t de forskellige bits i bank 1, som er associeret med "asynchronous transmission"
	BSF		STATUS,RP0	; Der skiftes til bank1

	BcF 	TXSTA,TX9D	; 9ende bit s�ttes til 0, men anvendes ikke i dette program
	BcF 	TXSTA,TRMT	; Status bit, kan ikke skrives til. Transmitregister fuld=0 tom=1
	BSF 	TXSTA,BRGH	; s�ttes til h�j da det giver den mindste fejl med 9200 baud rate
			; Bit 3 	  anvendes ikke 
	BCF 	TXSTA,SYNC	; Cleares da der skal k�res assyncron mode
	BSF 	TXSTA,TXEN	; N�r TXEN s�ttes er porten klar til at sende data
	BSF 	TXSTA,TX9	; TX9 cleares da vi bare �nsker at sende 8 bit
			; Bit 7 	  s�ttes ikke da den er ligegyldig i assyncrone mode

	MOVLW 	D'64'		; S�t W til decimaltallet, 64
	MOVWF 	SPBRG		; Flyt W til SPBRG s� boud rate generator f�r en error% p� 0.16
	
	BcF 	INTCON,GIE	; G�r alle interupts mulige
	BcF 	INTCON,PEIE	; giver lavstatus interrupts adgang
	
	BcF 	PIE1,TXIE	; s�tter interuptet op s� det kan anvendes
	BCF		STATUS,RP0	; Der skiftes til bank0
	BCF 	PIR1,TXIF	; s�tter interuptflaget til 0, 1 betyder byte modtaget / sendt????

	BSF		RCSTA,SPEN	; Aktiverer den serielle port

	CLRW				; Clear W, s� nyt data kan blive lageret i W
	RETURN				; Setup er f�rdig, vend tilbage til hovedprogram

TRANSMITTER
; Afsendelse af data fra transmitter til reciever
	BTFSC PIR1,TXIF		; Hvis interuptflaget er sat, udf�r nedenst�ende kode
	GOTO TRANSMITTER	; Hvis interuptflaget ikke er sat, tjek igen
		MOVWF TXREG		; Flyt dataen fra W til sender registret: TXREG
		CLRW			; Clear dataen fra W, s� nyt data kan blive lageret i W
		BCF PIR1,TXIF	; Clear interuptflaget
		RETURN			; Returner n�r dataen er blevet sendt til recieveren

; ******* PROGRAM AFSLUTTET ***************************************************************