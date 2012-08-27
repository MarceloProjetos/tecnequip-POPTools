POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=100000000
BAUD=9600
PARITY=0
MODBUSID=0
COM=0
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
DNS=192.168.0.1
SNTP=9-0:br.pool.ntp.org
X4=1
CANSAVE=0

IO LIST
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 13/08/2012
END
RUNG
    COMMENT Exemplo de utilização das instruções Ler String Formatada e Escrever String Formatada.\r\nA cada 2 segundos enviamos o valor de uma variável pela serial e, no intervalo, verificamos por comando para incrementar esta variável.
END
RUNG
    COMMENT A seguir utilizamos a instrução Escrever String Formatada. O %d no texto será substituído pelo conteúdo de Valor.\r\nApós a transmissão será feita uma pausa de 2 segundos antes de enviar novamente. A cada envio, Valor é decrementado em 1.
END
RUNG
    CONTACTS LerValor 1 0
    WRITE_FORMATTED_STRING Valor Valor: %d\r\n
    TOF ON 1000000
    TON OFF 1000000
    PARALLEL
        COIL LerValor 0 0 0 0
        SERIES
            OSR
            PARALLEL
                SUB Valor Valor 1
                MOVE Incremento 0
            END
        END
    END
END
RUNG
    COMMENT Entre os envios (enquanto LerValor estiver ativo) realizamos a leitura de uma string através da instrução Ler String Formatada.\r\nSe recebermos uma string que coincide com o padrão, o valor constante em %d será armazenado em Incremento. Adicionamos então Incremento à variável Valor.
END
RUNG
    CONTACTS LerValor 0 0
    READ_FORMATTED_STRING Incremento INC %d\r\n
    ADD Valor Valor Incremento
END
