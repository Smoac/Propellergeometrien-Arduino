# Propellergeometrien Arduino
 TU Berlin Projektlabor Projektversuch WS19/20

endprogram.ino ist das genutzte Hauptprogramm für den Arduino. Getestet auf Arduino Leonardo.

erklaerung code:
    Jedes Messgeraet gibt alternerierende spannung als Messwert aus, muss nur konvertiert werden in Messwerte. Dabei wird an vorgegebene Beispiele gehalten mit Datenblatt (Beispielcode fuer konvertieren von Eingangspannung in Volt oder Ampere, getvolt() oder getampere()
    
    Waagenkalibierung vor Versuchsaufbau montiert und Testen nachdem Versuchsaufbau montiert ist. Einmalig Kalibriert mit Kalibrierungswert fuer den Rest der Messungen.
    
    void setspeed()
    umkonvertieren prozentzahl 0-100 in PWM signal was esc lesen kann. (90 minimum, 180 maximum)
    Servo.lib genutzt (Library mit integrierter Funktion)
    
    AS IS, NICHT AS IT WAS INTENDET!!
    void setup() code der einmal láeuft. Serial.begin(19200), hoehere Baudrate als normal fuer hoeheren Datendurchsatz.
    Esc.arm aktiviert die ESC (LOW HIGH LOW PWM SIGNAL)
    Load.cell ist waage
    pinMode definiert Pins am Arduino (eig klar)
    
    void loop() lauft immer immer wieder.
    get ampere, get bla, ist werte aufnehmen und in variable schreiben
    
    if Serial available ist auf manuellen Input warten 
    ++motor schneller
    -- motor langsamer
    a abort
    numbers ist der String der im Serial.monitor ausgegebn wurde
    setSpeed fuer Motoransteuerung verantwortlich