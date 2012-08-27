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
    LigarBombaHidr at 1 0 type 2
    ModoAutomatico at 1 0 type 1
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 09/08/2012
END
RUNG
    COMMENT Exemplo de utilização das instruções de Borda de Subida e Borda de Descida.\r\nEste é um exemplo de aplicação dessas instruções, demonstrando uma situação onde a sua utilização é necessária.
END
RUNG
    COMMENT Ao ativar ModoAutomatico, liga IniciarModoAuto.\r\nDevido à instrução de borda de subida, IniciarModoAuto somente será ativado no exato momento em que ModoAutomatico for ativado
END
RUNG
    CONTACTS ModoAutomatico 0 0
    OSR
    COIL IniciarModoAuto 0 1 0 0
END
RUNG
    COMMENT Quando IniciarAuto é ativado, ele ativa LigarBombaHidr. Depois de 3s, IniciarProducao é ativado e o próprio IniciarModoAuto é desativado.\r\nNote que se não fosse utilizada a instrução de borda de subida na linha acima IniciarModoAuto seria novamente ativado no ciclo seguinte.
END
RUNG
    CONTACTS IniciarModoAuto 0 0
    PARALLEL
        COIL LigarBombaHidr 0 1 0 0
        SERIES
            TON EsperaIniciar 3000000
            PARALLEL
                COIL IniciarModoAuto 0 0 1 0
                COIL IniciarProducao 0 1 0 0
            END
        END
    END
END
RUNG
    COMMENT Quando ModoAutomatico for desativado, IniciarModoAuto e IniciarProducao também serão desativados.\r\nPerceba que sem a instrução de borda de descida eles continuariam a ser desligados a cada ciclo.
END
RUNG
    CONTACTS ModoAutomatico 0 0
    OSF
    PARALLEL
        COIL IniciarModoAuto 0 0 1 0
        COIL IniciarProducao 0 0 1 0
    END
END
