 Z '   ��    �%  � �� ��� �� ��    br.pool.ntp.org	         �?�           �?�                        POPTools	   TecnequipH   Exemplo de utiliza��o 
das instru��es 
Lineariza��o por 
Segmentos.
   1.5   ���R            Z  	      AbsFatorCorr@                               AbsPerimRoda?                               IncFatorCorr>                               IncPerimRoda<                               IncPulsosVolt=                               PosicaoA                           
   Velocidade'                               ZerarEncoderv                             	   novoValorD                                         L   POPTools v2.0 - Tecnequip Tecnologia em Equipamentos Ltda.
Data: 06/01/2014       �   Exemplo de utiliza��o das instru��es Lineariza��o por Segmentos, Leitura de Encoder e Reser de Encoder.
Neste exemplo atualizamos o D/A conforme o valor lido do encoder.       i   A seguir utilizamos uma instru��o Ler Encoder.
O valor atual do encoder � carregado na vari�vel Posicao.    2   A             �   Abaixo existe uma instru��o Reset de Encoder.
Quando ZerarEncoder estiver ativo, a instru��o Reset de Encoder ir� carregar o valor 0 no encoder.       v                D              3   D             y   Abaixo utilizamos a Lineariza��o por Segmentos.
Atrav�s desta instru��o convertermos o valor de Posicao para Velocidade.       �   Neste objeto cadastramos os pontos X,Y de forma a gerar segmentos como em um gr�fico.
A convers�o � realizada utilizando-se a refer�ncia X (variavel Posicao) para gerar o correspondente em Y (variavel Velocidade).    1      '      A             �     �  (   �     �  ����       X   Aqui carregamos no D/A o valor de Velocidade selecionado pela Lineariza��o por Segmentos    6   '         �u