 Z '   ��     K  � �� ��� �� ��   br.pool.ntp.org	        �?�           �?�                       POPTools	   Tecnequip@   Exemplo de Exemplo
de leitura A/D, que transmite
pela RS-485
   1.5   �>�R            o        adf                              ocs1S`                               seq1b                	              seq2^                                         L   POPTools v2.0 - Tecnequip Tecnologia em Equipamentos Ltda.
Data: 07/01/2014       �   Exemplo de utiliza��o da instru��o de leitura A/D (Conversor Anal�gico -> Digital)
Esta l�gica transmite uma string com o valor atual do A/D pela interface RS-485 a cada dois segundos       �   A linha abaixo serve para criar um rel� que fica alternando a cada 1s.
Assim, ele fica 1 segundo em n�vel alto e mais 1 segundo em n�vel baixo.       `         b      @B    ^      @B    `                V   Agora o valor do A/D � lido.
A cada ciclo o A/D carrega o valor atual na vari�vel AD.    (   f             �   A seguir est� a l�gica de transmiss�o da mensagem.
Quando OSC ligar, o comando seguinte (Escrita de string formatada) faz a transmiss�o do texto pela RS-485.       `       >      valor: %d\r\nf      �Z