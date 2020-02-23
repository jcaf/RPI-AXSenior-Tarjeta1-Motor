#ifndef RPM_H_
#define RPM_H_

    #define DDRxINT0    (DDRD)
    #define PORTWxINT0	(PORTD)
    #define PORTRxINT0	(PIND)
    #define PIN_INT0 	(2)

    extern volatile uint16_t rpm_ISR;
    extern uint16_t RPM;

    void RPM_process(void);

#endif // RPM_H_
