#include <msp430.h>

#define LED BIT6    // define o pino de saída do LED como P1.0

#define PWM_PERIOD 1000 // Período do sinal PWM (usar 1000 para frequência de 100 Hz)
unsigned int dutyCycle = 0; // Duty cycle inicial

int crem = 1;

int main(void) {

    WDTCTL = WDTPW | WDTHOLD;       //WATCHDOG TIMER PARADO

    P1DIR |= LED;                  //P1.6 IMPLEMENTADO COMO SAÍDA
    P1SEL |= LED;                  //CONFIGURANDO P1.6 COMO PERIFÉRICO TA0.1

    P1DIR &= ~BIT3;                 //P1.3 IMPLEMENTADO COMO ENTRADA
    P1IE  |= BIT3;                  //HABILITA INTERRUPÇÃO EM P1.3
    P1IFG &= ~BIT3;                 //LIMPA FLAG DE INTERRUPÇÃO DO PINO P1.3
    P1REN |= BIT3;                  //HABILITA RESISTOR INTERNO EM P1.3
    P1OUT |= BIT3;                  //RESISTOR DE PULL-UP EM P1.3
    P1IES ^= BIT3;                  //BORDA DE SUBIDA/DESCIDA DO PINO P1.3

    TACCR0 = PWM_PERIOD;            //PERÍODO DO PWM
    TACCTL1 = OUTMOD_7;             //MODO DE SAÍDA DO TIMER0_A: RESET/SET
    TACTL = TASSEL_2 + MC_1;        //Configura o TACTL para controlar o modo de operação e a fonte de clock do Timer_A

    _BIS_SR(GIE + LPM0_bits);       // Habilita interrupções globais e coloca o MSP430 em LPM0

    while(1);
}

#pragma vector = PORT1_VECTOR
__interrupt void port_int(void){

    if (P1IN & BIT3) {
        TA0CCR1 = dutyCycle;                    // Atualiza o valor do duty cycle

        dutyCycle += PWM_PERIOD / 10 * crem;    // Incrementa o duty cycle em 10%

        if (dutyCycle > PWM_PERIOD){
           dutyCycle = PWM_PERIOD;             // Limita o duty cycle máximo
            crem *= -1;
        }
        if (dutyCycle < 1){
            dutyCycle = 0;                      // Limita o duty cycle minimo
            crem *= -1;
        }
    }

    P1IFG &= ~BIT3;     //LIMPA FLAG DE INTERRUPÇÃO DO PINO P1.3
}
