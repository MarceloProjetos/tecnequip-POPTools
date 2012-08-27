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
    Amarelo at 2 0 type 2
    EsperaAmarelo at 21 0 type 2048
    EsperaVerde at 22 0 type 2048
    EsperaVermelho at 20 0 type 2048
    ForcarPiscante at 1 0 type 1
    Verde at 3 0 type 2
    Vermelho at 1 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 13/08/2012
END
RUNG
    COMMENT Exemplo de Aplicação - Semáforo com Modo Piscante.\r\nOs tempos de Verde, Amarelo e Vermelho são configuráveis por ModBUS.
END
RUNG
    COMMENT Aqui configuramos o valor inicial de cada variável e o modo inicial de operação.\r\nAs variáveis EsperaVerde, EsperaAmarelo e EsperaVermelho são associadas a registradores ModBUS (M0, M1 e M2 respectivamente) de forma que podem ser alteradas.
END
RUNG
    OSR
    PARALLEL
        MOVE EsperaVerde 20
        MOVE EsperaAmarelo 3
        MOVE EsperaVermelho 30
        COIL AmareloAtivo 0 1 0 0
        MOVE Tempo EsperaAmarelo
    END
END
RUNG
    COMMENT Aqui geramos o ciclo de operação, alternando entre ligado e desligado a cada 1 segundo.\r\nÉ a bsae de tempo de funcionamento do semáforo.
END
RUNG
    CONTACTS Ciclo 1 0
    TON ON 500000
    TOF OFF 500000
    COIL Ciclo 0 0 0 0
END
RUNG
    COMMENT Abaixo existe um agendamento para que todos os dias, a partir de 23 horas, o semáforo entre no Modo Piscante.\r\nNesse modo o sinal Amarelo ficará piscando e não haverá transições entre o Verde e o Vermelho. É possível forçar este modo ativando-se ForcarPiscante.
END
RUNG
    PARALLEL
        RTC 255 0/0/0 23:0:0
        SERIES
            CONTACTS ForcarPiscante 0 0
            OSR
        END
    END
    PARALLEL
        COIL ModoPiscante 0 1 0 0
        COIL VerdeAtivo 0 0 1 0
        COIL AmareloAtivo 0 0 1 0
        COIL VermelhoAtivo 0 0 1 0
    END
END
RUNG
    COMMENT Este outro agendamento desativa o Modo Piscante todos os dias às 6 horas da manhã.\r\nNeste momento é ativado o sinal Amarelo e a contagem é então iniciada normalmente.
END
RUNG
    PARALLEL
        RTC 255 0/0/0 6:0:0
        SERIES
            CONTACTS ForcarPiscante 0 0
            OSF
        END
    END
    PARALLEL
        COIL ModoPiscante 0 0 1 0
        COIL AmareloAtivo 0 1 0 0
        MOVE Tempo EsperaAmarelo
    END
END
RUNG
    COMMENT A lógica a seguir é complexa e a principal lógica do programa. É aqui que o estado do semáforo é atualizado.\r\nSempre que Ciclo é ativado, o contador utilizando a variável Tempo é decrementado. Quando Tempo atinge zero, um novo estado é selecionado.
END
RUNG
    COMMENT De acordo com o modo atualmente ativo decide-se qual é o novo estado. A sequência, como era de se esperar, é Verde -> Amarelo -> Vermelho.\r\nAo atualizar o estado, Tempo é carregada com o tempo a aguardar até mudar novamente de estado.
END
RUNG
    COMMENT Existe uma particularidade ao checar se o modo Verde é o ativo: testamos também se o tempo é zero.\r\nIsso acontece pois o estado Vermelho pode ter ativado o Verde como novo estado, causando erro na lógica.
END
RUNG
    CONTACTS Ciclo 0 0
    CTD Tempo 0
    PARALLEL
        SERIES
            CONTACTS VermelhoAtivo 0 0
            PARALLEL
                COIL VerdeAtivo 0 1 0 0
                COIL VermelhoAtivo 0 0 1 0
                MOVE Tempo EsperaVerde
            END
        END
        SERIES
            CONTACTS AmareloAtivo 0 0
            PARALLEL
                COIL VermelhoAtivo 0 1 0 0
                COIL AmareloAtivo 0 0 1 0
                MOVE Tempo EsperaVermelho
            END
        END
        SERIES
            EQU Tempo 0
            CONTACTS VerdeAtivo 0 0
            PARALLEL
                COIL AmareloAtivo 0 1 0 0
                COIL VerdeAtivo 0 0 1 0
                MOVE Tempo EsperaAmarelo
            END
        END
    END
END
RUNG
    COMMENT A linha abaixo ativa o sinal Verde sempre que o estado ativo seja Verde.\r\nEnquanto o tempo restante for maior que 5, o sinal verde ficará aceso continuamente. Se menor, ficará piscando intermitentemente.
END
RUNG
    CONTACTS VerdeAtivo 0 0
    PARALLEL
        GRT Tempo 5
        CONTACTS Ciclo 0 0
    END
    COIL Verde 0 0 0 0
END
RUNG
    COMMENT Abaixo ativamos o sinal Amarelo. Se ativo o estado Amarelo, ele ficará aceso continuamente.\r\nJá se ativado o Modo Piscante, ele ficará piscando de acordo com o tempo de Ciclo.
END
RUNG
    PARALLEL
        CONTACTS AmareloAtivo 0 0
        SERIES
            CONTACTS ModoPiscante 0 0
            CONTACTS Ciclo 0 0
        END
    END
    COIL Amarelo 0 0 0 0
END
RUNG
    COMMENT Abaixo é a lógica que ativa o sinal Vermelho.\r\nEle funciona apenas de uma forma: se o estado vermelho estiver ativado, o sinal Vermelho ficará aceso continuamente.
END
RUNG
    CONTACTS VermelhoAtivo 0 0
    COIL Vermelho 0 0 0 0
END
