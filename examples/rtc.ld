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
    Sirene at 1 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 13/08/2012
END
RUNG
    COMMENT Exemplo de utilização da instrução Agendador de Tarefa.\r\nNeste exemplo simulamos o funcionamento de uma sirene, alertando o horário de início e final de expediente e o horário de almoço.
END
RUNG
    COMMENT Aqui configuramos um agendamento para ocorrer todos os dias às 00:00 horas.\r\nServe para limpar o relé interno que indica que o dia atual é um feriado.
END
RUNG
    RTC 255 0/0/0 0:0:0
    COIL Feriado 0 0 1 0
END
RUNG
    COMMENT Alguns agendamentos em paralelo ativados em datas específicas.\r\nServe para ativar o relé interno que indica que o dia de hoje é um feriado.
END
RUNG
    PARALLEL
        RTC 2 1/5/0 0:0:0
        RTC 127 7/9/0 0:0:0
        RTC 127 15/11/0 0:0:0
        RTC 127 25/12/0 0:0:0
        RTC 127 31/12/0 0:0:0
    END
    COIL Feriado 0 1 0 0
END
RUNG
    COMMENT Abaixo está a lógica que ativa a sirene.\r\nSe não for feriado, quando o agendamento ativar será disparada a sirene por 1,5 segundo.
END
RUNG
    CONTACTS Feriado 1 0
    PARALLEL
        RTC 190 0/0/0 7:0:0
        RTC 190 0/0/0 12:0:0
        RTC 190 0/0/0 13:30:0
        RTC 190 0/0/0 17:0:0
    END
    TOF EsperaSirene 1500000
    COIL Sirene 0 0 0 0
END
