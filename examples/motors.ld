 Z '   ��     K  � �� ��� �� ��   br.pool.ntp.org	        �?�           �?�                       POPTools	   Tecnequip/   Exemplo de utiliza��o
 da Sa�da PWM e 
Rampa.   1.5   ���R            �        AtivarMotorx                               Largura�                            
   LarguraMax}                               PiscarSaida1�                               UsarPWMz                               ciclo�                               seq4�                	              seq5�                                         L   POPTools v2.0 - Tecnequip Tecnologia em Equipamentos Ltda.
Data: 09/01/2014       �   Exemplo de utiliza��o das instru��es Sa�da PWM e Rampa de Acelera��o/Desacelera��o.
Neste exemplo simulamos a acelera��o de um motor atrav�s de controle anal�gico (Rampa) ou PWM.       �   A linha a seguir utiliza uma instru��o Rampa de Acelera��o/Desacelera��o.
Se AtivaMotor estiver ligado e UsarPWM desligado, Ser� gerada a rampa de acelera��o conforme definido.       x         z       B      
   �               d       X         y   Abaixo carregamos o valor 100 em LarguraMax.
Essa vari�vel � utilizada para checar se estamos excedendo o limite do PWM.       }          d          �   A l�gica a seguir serve para gerar um cilo de 1 segundo.
A vari�vel Largura � ent�o incrementada em 5 a cada intervalo de tempo.       x          z          �         �       �    �       �         �                   �      �                    �   Aqui checamos se o limite foi ultrapassado.
Se Largura for maior que LarguraMax, ent�o Largura ser� carregado com o valor de LarguraMax.       �      }         �                     �   Aqui utilizamos a instru��o Sa�da PWM.
Quando AtivarMotor estiver ligado e UsarPWM tamb�m estiver ligado, ser� gerada uma onda PWM com Largura e a frequ�ncia selecionados na Saida 16.       x          z       )   �  �             <   Envia pela Serial o valor do PWM e pisca a Saida 1 do CLP.
       �       >      valor: %d\n�         �         ��