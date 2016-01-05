
int main_led(void)
{
	volatile unsigned int counter,
		*pconp = (volatile unsigned int *)0x400fc0c4,
		*gpio_cpu_iocon = (volatile unsigned int *)0x4002c0d8,
		*gpio_p1_dir = (volatile unsigned int *)0x20098020,
		*gpio_p1_msk = (volatile unsigned int *)0x20098030,
		*gpio_p1_set = (volatile unsigned int *)0x20098038,
		*gpio_p1_clr = (volatile unsigned int *)0x2009803c;

	// Ativa os GPIOs
	*pconp       = *pconp       | (1 << 15);

	// Configura o pino do led de CPU para funcionar como GPIO. Pull-up e histerese habilitados
	*gpio_cpu_iocon = 0x30;

	// Configura o pino do led de CPU para Saida
	*gpio_p1_dir = *gpio_p1_dir | (1 << 22);

	// Configura o registrador de mascara para usar o pino do led de CPU
	*gpio_p1_msk = *gpio_p1_msk | ~(1 << 22);

    while(1)
    {
    	// Seleciona nivel alto no pino do led de CPU
    	*gpio_p1_set = *gpio_p1_set | (1 << 22);

    	for(counter = 0; counter < 500000; counter++);

    	// Seleciona nivel baixo no pino do led de CPU
    	*gpio_p1_clr = *gpio_p1_clr | (1 << 22);

    	for(counter = 0; counter < 500000; counter++);
    }
}
