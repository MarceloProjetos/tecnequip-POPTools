#define NULL (void *)(0)

// Funcao para calcular raiz quadrada, tendo como resultado a parte inteira da raiz.
// O ponteiro deve ser carregado com o valor de origem e, ao retornar, tera o resultado.
// O retorno da funcao eh a imprecisao no calculo. Se retornar zero indica raiz exata.
int srint(int *sr)
{
	int resto = 0, result = 0, i = 0, fator;
	if(sr == NULL)
		return 0;

	if(*sr >= 100) {
		result = *sr/100;
		resto = srint(&result)*100 + *sr%100;
	} else {
		resto = *sr;
	}

	fator = result*20;
	while((fator+i)*i <= resto)
		i++;

	i--;
	*sr = result*10 + i;

	return resto - (fator+i)*i;
}
