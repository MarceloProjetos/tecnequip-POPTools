Z '   ��    �%  � �� ��� �� ��     br.pool.ntp.org	         �?�           �?�                      POPTools           1.5   �-�S            �         Acrescenta1                                Acrescenta10                                Acrescenta100                                Dividir                                Multiplicar                                RaizQuadrada=                      	          RestoDivisao�                      
          Somar                                Subtrair                                Zerar                             
   ZerarValor                             	   resultado                                valorX
                                           L   POPTools v2.0 - Tecnequip Tecnologia em Equipamentos Ltda.
Data: 10/11/2013       �   Exemplo de utiliza��o de instru��es matem�ticas.
Demonstra-se o uso das instru��es Somar, Subtrair, Multiplicar e Dividir pela implementa��o de uma calculadora.       �   As linhas a seguir acrescentam respectivamente 1, 10 e 100 � vari�vel valor.
Existe o Borda de Subida para que some apenas 1 vez e n�o a cada ciclo enquanto a entrada estiver ligada.                     
       
                                   
       
           
                        
       
           d          �   Abaixo est� a l�gica para zerar as vari�veis resultado e valor.
Quando Zerar for ativado, resultado recebe zero e ativamos ZerarValor para que a vari�vel valor seja zerada tamb�m.                                                            +  As quatro linha abaixo executam respectivamente as opera��es Somar, Subtrair, Multiplicar e Dividir. Tamb�m ativam ZerarValor para que a vari�vel valor seja zerada.
A instru��o Borda de Subida impede que as opera��es sejam executadas continuamente enquanto a entrada correspondente estiver ativada.                                        
                                                            
                                                            
                                                            
                           *   A l�gica abaixo calcula o resto da divis�o       �                :                 
                           �   A l�gica a seguir calcula a raiz quadrada do valor digitado.
Se n�o foi digitado nenhum valor desde a �ltima opera��o, utiliza o �ltimo resultado.       =                    
               C          
              
               C                        �   Nessa l�gica a vari�vel valor � zerada sempre que ZerarValor estiver ativado.
A vari�vel valor recebe 0 e o pr�prio ZerarValor se desativa para que a vari�vel n�o continue sendo zerada a cada ciclo.                                   
               �k