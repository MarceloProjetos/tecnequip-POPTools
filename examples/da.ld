 Z '   ��     K  � �� ��� �� ��   br.pool.ntp.org	        �?�           �?�                       POPTools	   TecnequipA   Exemplo de utiliza��o
 do D/A 
(Conversor Digital -> Anal�gico)   1.5   ,��R            �        DAs                               ocs1S`                              seq1b                	              seq2^                                         L   POPTools v2.0 - Tecnequip Tecnologia em Equipamentos Ltda.
Data: 07/01/2014       �   Exemplo de utiliza��o do D/A (Conversor Digital -> Anal�gico).
Atrav�s de vari�vel que � incrementada a cada 100 ms, o D/A oscila entre -10V e +10V.       h   A l�gica a seguir oscila a sa�da a cada 100ms
Essa � a base de tempo utilizada para atualiza��o do D/A.       `         b      @B    ^      @B    `                �   Quando a sa�da S1 � ativada, a vari�vel DA � acrescida de 250.
Para que a vari�vel n�o seja atualizada a cada ciclo enquanto S1 estiver ativada, usa-se o detector de borda de subida.       `             s      s          �          �   O D/A da POP-7 pode variar entre -10V e +10V.
Assim verificamos se o limite superior foi ultrapassado e, nesse caso, atualizamos a vari�vel para o limite inferior.        s          '     s          ����       �   A seguir o D/A � atualizado.
Ele est� configurado para receber o valor em mV, realizando a convers�o para o valor direto de 12 bits internamente.    6   s                 �   A seguir est� a l�gica de transmiss�o da mensagem.
Quando OSC ligar, o comando seguinte (Escrita de string formatada) faz a transmiss�o do texto pela RS-485.       `       >      valor: %d\r\ns      ��