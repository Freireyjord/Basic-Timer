#include <msp430.h>

#define PWM_PERIOD 1000 // Período do sinal PWM (usar 1000 para frequência de 100 Hz)
unsigned int dutyCycle = 0; // Duty cycle inicial
int crem = 1;
int main(void) {

    WDTCTL = WDTPW | WDTHOLD;       //WATCHDOG TIMER PARADO

    DCOCTL = 0;                     //
    BCSCTL1 = CALBC1_1MHZ;          //CONFIGURA CLOCK EM 1 MHZ
    DCOCTL = CALDCO_1MHZ;

    P1DIR |= 0x40;                  //P1.6 IMPLEMENTADO COMO SAÍDA
    P1SEL |= 0x40;                  //CONFIGURANDO P1.6 COMO PERIFÉRICO TA0.1

    P1DIR &= ~0x08;                 //P1.3 IMPLEMENTADO COMO ENTRADA
    P1IE  |= 0X08;                  //HABILITA INTERRUPÇÃO EM P1.3
    P1IFG &= ~0X08;                 //LIMPA FLAG DE INTERRUPÇÃO DO PINO P1.3
    P1REN |= 0X08;                  //HABILITA RESISTOR INTERNO EM P1.3
    P1OUT |= 0X08;                  //RESISTOR DE PULL-UP EM P1.3
    P1IES ^= 0x08;                  //BORDA DE SUBIDA/DESCIDA DO PINO P1.3

    TACCR0 = PWM_PERIOD;            //PERÍODO DO PWM
    TACCTL1 = OUTMOD_7;             //MODO DE SAÍDA DO TIMER0_A: RESET/SET
    TACCR1 = dutyCycle;             //DUTY CYCLE DO PWM EM 10%
    TACTL = TASSEL_2 + MC_1;        //TASSEL_2 -> CLOCK SOURCE: MCLK    MC_1 ->                                   

    _BIS_SR(GIE + LPM0_bits); // Habilita interrupções globais e coloca o MSP430 em LPM0

    while(1);
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void timer_int(void){

    TACCTL1 &= ~CCIFG;

}

#pragma vector = PORT1_VECTOR
__interrupt void port_int(void){


    TA0CCR1 = dutyCycle; // Atualiza o valor do duty cycle
    dutyCycle += PWM_PERIOD / 10 * crem; // Incrementa o duty cycle em 10%


    if (dutyCycle > PWM_PERIOD){
        dutyCycle = PWM_PERIOD; // Limita o duty cycle máximo
        crem *= -1;
    }
    if (dutyCycle < 1){
        dutyCycle = 0; // Limita o duty cycle minimo
        crem *= -1;
    }

    P1IFG &= ~0x08;     //LIMPA FLAG DE INTERRUPÇÃO DO PINO P1.3
}
