 Z '   ��    �%  � �� ��� �� ��     br.pool.ntp.org	         �?�           �?�                      POPTools	   Tecnequip6   Exemplo de utiliza��o da instru��o 
Tabela de Busca
   1.5                    1        Ciclov                             
   Velocidade'                               idx&                               seq0w                 	              seq1x                                
          L   POPTools v2.0 - Tecnequip Tecnologia em Equipamentos Ltda.
Data: 05/12/2013       �   Exemplo de utiliza��o da instru��o Tabela de Busca
Neste exemplo atualizamos o D/A com o valor indicado na posi��o selecionada.       �   A l�gica abaixo utiliza um rel� interno para criar um sinal liga/desliga.
Esse sinal alterna entre alto e baixo a cada 0,5 segundos.       v          w        �    x        �    v                 �   Agora utilizamos um contador circular para gerar os valores em index.
A vari�vel index ser� utlizada para selecionar uma posi��o na Tabela de Busca.       v          %   &                �   Abaixo utilizamos a Tabela de Busca para carregar a vari�vel Velocidade.
O valor carregado � selecionado pela vari�vel de �ndice (index).    .       '      &      ��������    2   d          P   Aqui carregamos no D/A o valor de Velocidade selecionado pela Tabela de Busca.
    6   '         j�