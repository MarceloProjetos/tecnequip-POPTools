//-----------------------------------------------------------------------------
// Copyright 2007 Jonathan Westhues
//
// This file is part of LDmicro.
// 
// LDmicro is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// LDmicro is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with LDmicro.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// Load/save the circuit from/to a file in a nice ASCII format.
// Jonathan Westhues, Nov 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldmicro.h"

static ElemSubcktSeries *LoadSeriesFromFile(FILE *f, int version);

static const int current_version = 2;

//-----------------------------------------------------------------------------
// Check a line of text from a saved project file to determine whether it
// contains a leaf element (coil, contacts, etc.). If so, create an element
// for and save that in *any and *which, and return TRUE, else return FALSE.
//-----------------------------------------------------------------------------
static BOOL LoadLeafFromFile(char *line, void **any, int *which, int version)
{
    ElemLeaf *l = AllocLeaf();
    int x;

    if(memcmp(line, "COMMENT", 7)==0) {
        char *s = line + 8;
        int i = 0;
        while(*s && *s != '\n') {
            if(*s == '\\') {
                if(s[1] == 'n') {
                    l->d.comment.str[i++] = '\n';
                    s++;
                } else if(s[1] == 'r') {
                    l->d.comment.str[i++] = '\r';
                    s++;
                } else if(s[1] == '\\') {
                    l->d.comment.str[i++] = '\\';
                    s++;
                } else {
                    // that is odd
                }
            } else {
                l->d.comment.str[i++] = *s;
            }
            s++;
        }
        l->d.comment.str[i++] = '\0';
        *which = ELEM_COMMENT;
    } else if(sscanf(line, "CONTACTS %s %d %d", l->d.contacts.name, 
        &l->d.contacts.negated, &l->d.contacts.bit)==3)
    {
        *which = ELEM_CONTACTS;
    } else if(sscanf(line, "COIL %s %d %d %d %d", l->d.coil.name,
		&l->d.coil.negated, &l->d.coil.setOnly, &l->d.coil.resetOnly, &l->d.coil.bit)==5)
    {
        *which = ELEM_COIL;
    } else if(memcmp(line, "PLACEHOLDER", 11)==0) {
        *which = ELEM_PLACEHOLDER;
    } else if(memcmp(line, "SHORT", 5)==0) {
        *which = ELEM_SHORT;
    } else if(memcmp(line, "OPEN", 4)==0) {
        *which = ELEM_OPEN;
    } else if(memcmp(line, "MASTER_RELAY", 12)==0) {
        *which = ELEM_MASTER_RELAY;
    } else if(sscanf(line, "SHIFT_REGISTER %s %d", l->d.shiftRegister.name, &(l->d.shiftRegister.stages))==2) {
        *which = ELEM_SHIFT_REGISTER;
	} else if(sscanf(line, "SET_BIT %s %d %d", l->d.setBit.name, &l->d.setBit.set, &l->d.setBit.bit)==3) {
        *which = ELEM_SET_BIT;
	} else if(sscanf(line, "CHECK_BIT %s %d %d", l->d.checkBit.name, &l->d.checkBit.set, &l->d.checkBit.bit)==3) {
        *which = ELEM_CHECK_BIT;
    } else if(memcmp(line, "OSR", 3)==0) {
        *which = ELEM_ONE_SHOT_RISING;
    } else if(memcmp(line, "OSF", 3)==0) {
        *which = ELEM_ONE_SHOT_FALLING;
    } else if((sscanf(line, "TON %s %d", l->d.timer.name,
        &l->d.timer.delay)==2))
    {
        *which = ELEM_TON;
    } else if((sscanf(line, "TOF %s %d", l->d.timer.name,
        &l->d.timer.delay)==2))
    {
        *which = ELEM_TOF;
    } else if((sscanf(line, "RTO %s %d", l->d.timer.name,
        &l->d.timer.delay)==2))
    {
        *which = ELEM_RTO;
	} else if((sscanf(line, "RTC %d %d/%d/%d %d:%d:%d", &l->d.rtc.wday, &l->d.rtc.mday, &l->d.rtc.month, &l->d.rtc.year,
		&l->d.rtc.hour, &l->d.rtc.minute, &l->d.rtc.second)==7))
    {
        *which = ELEM_RTC;
    } else if((sscanf(line, "CTD %s %d", l->d.counter.name,
        &l->d.counter.max)==2))
    {
        *which = ELEM_CTD;
    } else if((sscanf(line, "CTU %s %d", l->d.counter.name,
        &l->d.counter.max)==2))
    {
        *which = ELEM_CTU;
    } else if((sscanf(line, "CTC %s %d", l->d.counter.name,
        &l->d.counter.max)==2))
    {
        *which = ELEM_CTC;
    } else if(sscanf(line, "RESET_ENC %s", l->d.resetEnc.name)==1) {
        *which = ELEM_RESET_ENC;
    } else if(sscanf(line, "RES %s", l->d.reset.name)==1) {
        *which = ELEM_RES;
    } else if(sscanf(line, "MOVE %s %s", l->d.move.dest, l->d.move.src)==2) {
        *which = ELEM_MOVE;
    } else if(sscanf(line, "EQU %s %s", l->d.cmp.op1, l->d.cmp.op2)==2) {
        *which = ELEM_EQU;
    } else if(sscanf(line, "NEQ %s %s", l->d.cmp.op1, l->d.cmp.op2)==2) {
        *which = ELEM_NEQ;
    } else if(sscanf(line, "GRT %s %s", l->d.cmp.op1, l->d.cmp.op2)==2) {
        *which = ELEM_GRT;
    } else if(sscanf(line, "GEQ %s %s", l->d.cmp.op1, l->d.cmp.op2)==2) {
        *which = ELEM_GEQ;
    } else if(sscanf(line, "LEQ %s %s", l->d.cmp.op1, l->d.cmp.op2)==2) {
        *which = ELEM_LEQ;
    } else if(sscanf(line, "LES %s %s", l->d.cmp.op1, l->d.cmp.op2)==2) {
        *which = ELEM_LES;
    } else if(sscanf(line, "READ_ADC %s", l->d.readAdc.name)==1) {
        *which = ELEM_READ_ADC;
    } else if(sscanf(line, "SET_DA %s", l->d.setDA.name)==1) {
        *which = ELEM_SET_DA;
	} else if(sscanf(line, "MULTISET_DA %s %s %d %d %d %d %d", l->d.multisetDA.name, l->d.multisetDA.name1, &l->d.multisetDA.linear, 
		&l->d.multisetDA.speedup, &l->d.multisetDA.forward, &l->d.multisetDA.time, &l->d.multisetDA.desloc)==7) {
        *which = ELEM_MULTISET_DA;
    } else if(sscanf(line, "READ_ENC %s", l->d.readEnc.name)==1) {
        *which = ELEM_READ_ENC;
	} else if(sscanf(line, "READ_USS %s %d %d %d %d", l->d.readUSS.name, &l->d.readUSS.id, &l->d.readUSS.parameter, &l->d.readUSS.parameter_set, &l->d.readUSS.index)==5) {
        *which = ELEM_READ_USS;
    } else if(sscanf(line, "WRITE_USS %s %d %d %d %d", l->d.writeUSS.name, &l->d.writeUSS.id, &l->d.writeUSS.parameter, &l->d.writeUSS.parameter_set, &l->d.writeUSS.index)==5) {
        *which = ELEM_WRITE_USS;
	} else if(sscanf(line, "READ_MODBUS %s %d %d %d", l->d.readModbus.name, &l->d.readModbus.id, &l->d.readModbus.address, &l->d.readModbus.int32)==4) {
        *which = ELEM_READ_MODBUS;
	} else if(sscanf(line, "WRITE_MODBUS %s %d %d %d", l->d.writeModbus.name, &l->d.writeModbus.id, &l->d.writeModbus.address, &l->d.writeModbus.int32)==4) {
        *which = ELEM_WRITE_MODBUS;
	} else if(sscanf(line, "READ_MODBUS_ETH %s %d %d %d", l->d.readModbusEth.name, &l->d.readModbusEth.id, &l->d.readModbusEth.address, &l->d.readModbusEth.int32)==4) {
        *which = ELEM_READ_MODBUS_ETH;
	} else if(sscanf(line, "WRITE_MODBUS_ETH %s %d %d %d", l->d.writeModbusEth.name, &l->d.writeModbusEth.id, &l->d.writeModbusEth.address, &l->d.writeModbusEth.int32)==4) {
        *which = ELEM_WRITE_MODBUS_ETH;
    } else if(sscanf(line, "SET_PWM %s %d", l->d.setPwm.name, 
        &(l->d.setPwm.targetFreq))==2)
    {
        *which = ELEM_SET_PWM;
    } else if(sscanf(line, "UART_RECV %s", l->d.uart.name)==1) {
        *which = ELEM_UART_RECV;
    } else if(sscanf(line, "UART_SEND %s", l->d.uart.name)==1) {
        *which = ELEM_UART_SEND;
    } else if(sscanf(line, "PERSIST %s", l->d.persist.var)==1) {
        *which = ELEM_PERSIST;
	} else if (sscanf(line, "READ_FORMATTED_STRING %s %[^\t\n]", l->d.fmtdStr.var, l->d.fmtdStr.string)==2) {
		if (_stricmp(l->d.fmtdStr.var,"(empty)") == 0)
			strcpy(l->d.fmtdStr.var, "");
		*which = ELEM_READ_FORMATTED_STRING;
	} else if (sscanf(line, "WRITE_FORMATTED_STRING %s %[^\t\n]", l->d.fmtdStr.var, l->d.fmtdStr.string)==2) {
		if (_stricmp(l->d.fmtdStr.var,"(empty)") == 0)
			strcpy(l->d.fmtdStr.var, "");
		*which = ELEM_WRITE_FORMATTED_STRING;
	} else if (sscanf(line, "READ_SERVO_YASKAWA %s %s %[^\t\n]", l->d.servoYaskawa.id, l->d.servoYaskawa.var, l->d.servoYaskawa.string)==3) {
		if (_stricmp(l->d.servoYaskawa.var,"(empty)") == 0)
			strcpy(l->d.servoYaskawa.var, "");
		*which = ELEM_READ_SERVO_YASKAWA;
	} else if (sscanf(line, "WRITE_SERVO_YASKAWA %s %s %[^\t\n]", l->d.servoYaskawa.id, l->d.servoYaskawa.var, l->d.servoYaskawa.string)==3) {
		if (_stricmp(l->d.servoYaskawa.var,"(empty)") == 0)
			strcpy(l->d.servoYaskawa.var, "");
		*which = ELEM_WRITE_SERVO_YASKAWA;
    } else if(sscanf(line, "FORMATTED_STRING %s %d", l->d.fmtdStr.var, &x)==2)
    {
        if(_stricmp(l->d.fmtdStr.var, "(none)")==0) {
            strcpy(l->d.fmtdStr.var, "");
        }

        char *p = line;
        int i;
        for(i = 0; i < 3; i++) {
            while(!isspace(*p)) p++;
            while( isspace(*p)) p++;
        }
        for(i = 0; i < x; i++) {
            l->d.fmtdStr.string[i] = atoi(p);
            if(l->d.fmtdStr.string[i] < 32) {
                l->d.fmtdStr.string[i] = 'X';
            }
            while(!isspace(*p) && *p) p++;
            while( isspace(*p) && *p) p++;
        }
        l->d.fmtdStr.string[i] = '\0';

        *which = ELEM_FORMATTED_STRING;
    } else if(sscanf(line, "LOOK_UP_TABLE %s %s %d %d", l->d.lookUpTable.dest,
        l->d.lookUpTable.index, &(l->d.lookUpTable.count),
        &(l->d.lookUpTable.editAsString))==4)
    {
        char *p = line;
        int i;
        // First skip over the parts that we already sscanf'd.
        for(i = 0; i < 5; i++) {
            while((!isspace(*p)) && *p)
                p++;
            while(isspace(*p) && *p)
                p++;
        }
        // Then copy over the look-up table entries.
        for(i = 0; i < l->d.lookUpTable.count; i++) {
            l->d.lookUpTable.vals[i] = atoi(p);
            while((!isspace(*p)) && *p)
                p++;
            while(isspace(*p) && *p)
                p++;
        }
        *which = ELEM_LOOK_UP_TABLE;
    } else if(sscanf(line, "PIECEWISE_LINEAR %s %s %d", 
        l->d.piecewiseLinear.dest, l->d.piecewiseLinear.index,
        &(l->d.piecewiseLinear.count))==3)
    {
        char *p = line;
        int i;
        // First skip over the parts that we already sscanf'd.
        for(i = 0; i < 4; i++) {
            while((!isspace(*p)) && *p)
                p++;
            while(isspace(*p) && *p)
                p++;
        }
        // Then copy over the piecewise linear points.
        for(i = 0; i < l->d.piecewiseLinear.count*2; i++) {
            l->d.piecewiseLinear.vals[i] = atoi(p);
            while((!isspace(*p)) && *p)
                p++;
            while(isspace(*p) && *p)
                p++;
        }
        *which = ELEM_PIECEWISE_LINEAR;
    } else if(sscanf(line, "ADD %s %s %s", l->d.math.dest, l->d.math.op1,
        l->d.math.op2)==3)
    {
        *which = ELEM_ADD;
    } else if(sscanf(line, "SUB %s %s %s", l->d.math.dest, l->d.math.op1,
        l->d.math.op2)==3)
    {
        *which = ELEM_SUB;
    } else if(sscanf(line, "MUL %s %s %s", l->d.math.dest, l->d.math.op1,
        l->d.math.op2)==3)
    {
        *which = ELEM_MUL;
    } else if(sscanf(line, "DIV %s %s %s", l->d.math.dest, l->d.math.op1,
        l->d.math.op2)==3)
    {
        *which = ELEM_DIV;
	} else {
        // that's odd; nothing matched
        CheckFree(l);
        return FALSE;
    }
    *any = l;
    return TRUE;
}

//-----------------------------------------------------------------------------
// Load a parallel subcircuit from a file. We look for leaf nodes using
// LoadLeafFromFile, which we can put directly into the parallel circuit
// that we're building up, or series subcircuits that we can pass to
// LoadSeriesFromFile. Returns the parallel subcircuit built up, or NULL if
// something goes wrong.
//-----------------------------------------------------------------------------
static ElemSubcktParallel *LoadParallelFromFile(FILE *f, int version)
{
    char line[512];
    void *any;
    int which;

    ElemSubcktParallel *ret = AllocSubcktParallel();
    int cnt = 0;
    
    for(;;) {
        if(!fgets(line, sizeof(line), f)) return NULL;
        char *s = line;
        while(isspace(*s)) s++;

        if(_stricmp(s, "SERIES\n")==0) {
            which = ELEM_SERIES_SUBCKT;
            any = LoadSeriesFromFile(f, version);
            if(!any) return NULL;

        } else if(LoadLeafFromFile(s, &any, &which, version)) {
            // got it
        } else if(_stricmp(s, "END\n")==0) {
            ret->count = cnt;
            return ret;
        } else {
            return NULL;
        }
        ret->contents[cnt].which = which;
        ret->contents[cnt].d.any = any;
        cnt++;
        if(cnt >= MAX_ELEMENTS_IN_SUBCKT) return NULL;
    }
}

//-----------------------------------------------------------------------------
// Same as LoadParallelFromFile, but for a series subcircuit. Thus builds
// a series circuit out of parallel circuits and leaf elements.
//-----------------------------------------------------------------------------
static ElemSubcktSeries *LoadSeriesFromFile(FILE *f, int version)
{
    char line[512];
    void *any;
    int which;

    ElemSubcktSeries *ret = AllocSubcktSeries();
    int cnt = 0;
    
    for(;;) {
        if(!fgets(line, sizeof(line), f)) return NULL;
        char *s = line;
        while(isspace(*s)) s++;

        if(_stricmp(s, "PARALLEL\n")==0) {
            which = ELEM_PARALLEL_SUBCKT;
            any = LoadParallelFromFile(f, version);
            if(!any) return NULL;

        } else if(LoadLeafFromFile(s, &any, &which, version)) {
            // got it
        } else if(_stricmp(s, "END\n")==0) {
            ret->count = cnt;
            return ret;
        } else {
            return NULL;
        }
        ret->contents[cnt].which = which;
        ret->contents[cnt].d.any = any;
        cnt++;
        if(cnt >= MAX_ELEMENTS_IN_SUBCKT) return NULL;
    }
}

//-----------------------------------------------------------------------------
// Load a project from a saved project description files. This describes the
// program, the target processor, plus certain configuration settings (cycle
// time, processor clock, etc.). Return TRUE for success, FALSE if anything
// went wrong.
//-----------------------------------------------------------------------------
BOOL LoadProjectFromFile(char *filename)
{
    FreeEntireProgram();
    strcpy(CurrentCompileFile, "");

    FILE *f = fopen(filename, "r");
    if(!f) return FALSE;

    char line[512];
    int version, crystal, cycle, baud, comPort, UART, ip[4], mask[4], gw[4], dns[4];
	char sntp[126];

	version = 0;

    while(fgets(line, sizeof(line), f)) {
        if(_stricmp(line, "IO LIST\n")==0) {
            if(!LoadIoListFromFile(f, version)) {
                fclose(f);
                return FALSE;
            }
		} else if(sscanf(line, "LDmicro0.%d", &version)) {
			if (version < current_version)
				Error(_("Aviso: O arquivo deste projeto � de uma vers�o anterior e ser� atualizado para a vers�o atual quando for gravado !"));
			else if (version > current_version)
			{
				Error(_("Aviso: Este projeto n�o pode ser aberto porque foi gravado com uma vers�o mais nova do programa POPTools !"));
				fclose(f);
                return FALSE;
			}
			//Prog.version = version;
		} else if(sscanf(line, "POPTools:1.%d", &version)) {
			if (version < current_version)
				Error(_("Aviso: O arquivo deste projeto � de uma vers�o anterior e ser� atualizado para a vers�o atual quando for gravado !"));
			else if (version > current_version)
			{
				Error(_("Aviso: Este projeto n�o pode ser aberto porque foi gravado com uma vers�o mais nova do programa POPTools !"));
				fclose(f);
                return FALSE;
			}
			//Prog.version = version;
        } else if(sscanf(line, "CRYSTAL=%d", &crystal)) {
            Prog.mcuClock = crystal;
        } else if(sscanf(line, "CYCLE=%d", &cycle)) {
            Prog.cycleTime = cycle;
        } else if(sscanf(line, "BAUD=%d", &baud)) {
            Prog.baudRate = baud;
        } else if(sscanf(line, "PARITY=%d", &UART)) {
			Prog.UART = UART;
        } else if(sscanf(line, "COM=%d", &comPort)) {
            Prog.comPort = comPort;
        } else if(sscanf(line, "IP=%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3])) {
            Prog.ip[0] = ip[0];
            Prog.ip[1] = ip[1];
            Prog.ip[2] = ip[2];
            Prog.ip[3] = ip[3];
        } else if(sscanf(line, "MASK=%d.%d.%d.%d", &mask[0], &mask[1], &mask[2], &mask[3])) {
            Prog.mask[0] = mask[0];
            Prog.mask[1] = mask[1];
            Prog.mask[2] = mask[2];
            Prog.mask[3] = mask[3];
        } else if(sscanf(line, "GW=%d.%d.%d.%d", &gw[0], &gw[1], &gw[2], &gw[3])) {
            Prog.gw[0] = gw[0];
            Prog.gw[1] = gw[1];
            Prog.gw[2] = gw[2];
            Prog.gw[3] = gw[3];
        } else if(sscanf(line, "DNS=%d.%d.%d.%d", &dns[0], &dns[1], &dns[2], &dns[3])) {
            Prog.dns[0] = dns[0];
            Prog.dns[1] = dns[1];
            Prog.dns[2] = dns[2];
            Prog.dns[3] = dns[3];
		} else if(sscanf(line, "SNTP=%d-%d:%s", &Prog.gmt, &Prog.dailysave, &sntp)) {
			strncpy(Prog.sntp, sntp, sizeof(Prog.sntp));
        } else if(memcmp(line, "COMPILED=", 9)==0) {
            //line[strlen(line)-1] = '\0';
            //strcpy(CurrentCompileFile, line+9);
        } else if(memcmp(line, "MICRO=", 6)==0) {
            line[strlen(line)-1] = '\0';
            int i;
            for(i = 0; i < NUM_SUPPORTED_MCUS; i++) {
                if(_stricmp(SupportedMcus[i].mcuName, line+6)==0) {
                    Prog.mcu = &SupportedMcus[i];
                    break;
                }
            }
            if(i == NUM_SUPPORTED_MCUS) {
                Error(_("Microcontroller '%s' not supported.\r\n\r\n"
                    "Defaulting to no selected MCU."), line+6);
            }
        } else if(_stricmp(line, "PROGRAM\n")==0) {
            break;
        }
    }
    if(_stricmp(line, "PROGRAM\n") != 0) goto failed;

    int rung;
    for(rung = 0;;) {
        if(!fgets(line, sizeof(line), f)) break;
        if(_stricmp(line, "RUNG\n")!=0) goto failed;

        Prog.rungs[rung] = LoadSeriesFromFile(f, version);
        if(!Prog.rungs[rung]) goto failed;
        rung++;
    }
    Prog.numRungs = rung;

    fclose(f);
    return TRUE;

failed:
    fclose(f);
    NewProgram();
    Error(_("File format error; perhaps this program is for a newer version "
            "of POPTools?"));
    return FALSE;
}

//-----------------------------------------------------------------------------
// Helper routine for outputting hierarchical representation of the ladder
// logic: indent on file f, by depth*4 spaces.
//-----------------------------------------------------------------------------
static void Indent(FILE *f, int depth)
{
    int i;
    for(i = 0; i < depth; i++) {
        fprintf(f, "    ");
    }
}

//-----------------------------------------------------------------------------
// Save an element to a file. If it is a leaf, then output a single line
// describing it and return. If it is a subcircuit, call ourselves
// recursively (with depth+1, so that the indentation is right) to handle
// the members of the subcircuit. Special case for depth=0: we do not
// output the SERIES/END delimiters. This is because the root is delimited
// by RUNG/END markers output elsewhere.
//-----------------------------------------------------------------------------
static void SaveElemToFile(FILE *f, int which, void *any, int depth)
{
    ElemLeaf *l = (ElemLeaf *)any;
    char *s;

    Indent(f, depth);

    switch(which) {
        case ELEM_PLACEHOLDER:
            fprintf(f, "PLACEHOLDER\n");
            break;

        case ELEM_COMMENT: {
            fprintf(f, "COMMENT ");
            char *s = l->d.comment.str;
            for(; *s; s++) {
                if(*s == '\\') {
                    fprintf(f, "\\\\");
                } else if(*s == '\n') {
                    fprintf(f, "\\n");
                } else if(*s == '\r') {
                    fprintf(f, "\\r");
                } else {
                    fprintf(f, "%c", *s);
                }
            }
            fprintf(f, "\n");
            break;
        }
        case ELEM_OPEN:
            fprintf(f, "OPEN\n");
            break;

        case ELEM_SHORT:
            fprintf(f, "SHORT\n");
            break;

        case ELEM_MASTER_RELAY:
            fprintf(f, "MASTER_RELAY\n");
            break;
        
        case ELEM_SET_BIT:
            fprintf(f, "SET_BIT %s %d %d\n", l->d.setBit.name, l->d.setBit.set, l->d.setBit.bit);
            break;

        case ELEM_CHECK_BIT:
            fprintf(f, "CHECK_BIT %s %d %d\n", l->d.checkBit.name, l->d.checkBit.set, l->d.checkBit.bit);
            break;

        case ELEM_SHIFT_REGISTER:
            fprintf(f, "SHIFT_REGISTER %s %d\n", l->d.shiftRegister.name,
                l->d.shiftRegister.stages);
            break;

        case ELEM_CONTACTS:
            fprintf(f, "CONTACTS %s %d %d\n", l->d.contacts.name,
				l->d.contacts.negated, l->d.contacts.bit);
            break;

        case ELEM_COIL:
            fprintf(f, "COIL %s %d %d %d %d\n", l->d.coil.name, l->d.coil.negated,
                l->d.coil.setOnly, l->d.coil.resetOnly, l->d.coil.bit);
            break;

		case ELEM_MULTISET_DA:
			fprintf(f, "MULTISET_DA %s %s %d %d %d %d %d\n", l->d.multisetDA.name, l->d.multisetDA.name1, l->d.multisetDA.linear, 
		l->d.multisetDA.speedup, l->d.multisetDA.forward, l->d.multisetDA.time, l->d.multisetDA.desloc);
            break;

        case ELEM_TON:
            s = "TON"; goto timer;
        case ELEM_TOF:
            s = "TOF"; goto timer;
        case ELEM_RTO:
            s = "RTO"; goto timer;

timer:
            fprintf(f, "%s %s %d\n", s, l->d.timer.name, l->d.timer.delay);
            break;
		case ELEM_RTC:
			fprintf(f, "RTC %d %d/%d/%d %d:%d:%d\n", l->d.rtc.wday, l->d.rtc.mday, l->d.rtc.month, l->d.rtc.year,
												l->d.rtc.hour, l->d.rtc.minute, l->d.rtc.second);
			break;
        case ELEM_CTU:
            s = "CTU"; goto counter;
        case ELEM_CTD:
            s = "CTD"; goto counter;
        case ELEM_CTC:
            s = "CTC"; goto counter;

counter:
            fprintf(f, "%s %s %d\n", s, l->d.counter.name, l->d.counter.max);
            break;

        case ELEM_RES:
            fprintf(f, "RES %s\n", l->d.reset.name);
            break;

        case ELEM_MOVE:
            fprintf(f, "MOVE %s %s\n", l->d.move.dest, l->d.move.src);
            break;

        case ELEM_ADD: s = "ADD"; goto math;
        case ELEM_SUB: s = "SUB"; goto math;
        case ELEM_MUL: s = "MUL"; goto math;
        case ELEM_DIV: s = "DIV"; goto math;
math:
            fprintf(f, "%s %s %s %s\n", s, l->d.math.dest, l->d.math.op1,
                l->d.math.op2);
            break;

        case ELEM_EQU: s = "EQU"; goto cmp;
        case ELEM_NEQ: s = "NEQ"; goto cmp;
        case ELEM_GRT: s = "GRT"; goto cmp;
        case ELEM_GEQ: s = "GEQ"; goto cmp;
        case ELEM_LES: s = "LES"; goto cmp;
        case ELEM_LEQ: s = "LEQ"; goto cmp;
cmp:
            fprintf(f, "%s %s %s\n", s, l->d.cmp.op1, l->d.cmp.op2);
            break;

        case ELEM_ONE_SHOT_RISING:
            fprintf(f, "OSR\n");
            break;

        case ELEM_ONE_SHOT_FALLING:
            fprintf(f, "OSF\n");
            break;

        case ELEM_READ_ADC:
            fprintf(f, "READ_ADC %s\n", l->d.readAdc.name);
            break;

        case ELEM_SET_DA:
			fprintf(f, "SET_DA %s\n", l->d.setDA.name);
            break;

        case ELEM_READ_ENC:
            fprintf(f, "READ_ENC %s\n", l->d.readEnc.name);
            break;

        case ELEM_RESET_ENC:
            fprintf(f, "RESET_ENC %s\n", l->d.resetEnc.name);
            break;

        case ELEM_READ_USS:
            fprintf(f, "READ_USS %s %d %d %d %d\n", l->d.readUSS.name, l->d.readUSS.id, l->d.readUSS.parameter, l->d.readUSS.parameter_set, l->d.readUSS.index);
            break;

        case ELEM_WRITE_USS:
            fprintf(f, "WRITE_USS %s %d %d %d %d\n", l->d.writeUSS.name, l->d.writeUSS.id, l->d.writeUSS.parameter, l->d.writeUSS.parameter_set, l->d.writeUSS.index);
            break;

        case ELEM_READ_MODBUS:
			fprintf(f, "READ_MODBUS %s %d %d %d\n", l->d.readModbus.name, l->d.readModbus.id, l->d.writeModbus.address, l->d.readModbus.int32);
            break;

        case ELEM_WRITE_MODBUS:
			fprintf(f, "WRITE_MODBUS %s %d %d %d\n", l->d.writeModbus.name, l->d.writeModbus.id, l->d.writeModbus.address, l->d.writeModbus.int32);
            break;

        case ELEM_READ_MODBUS_ETH:
            fprintf(f, "READ_MODBUS_ETH %s %d %d %d\n", l->d.readModbusEth.name, l->d.readModbusEth.id, l->d.readModbusEth.address, l->d.readModbusEth.int32);
            break;

        case ELEM_WRITE_MODBUS_ETH:
			fprintf(f, "WRITE_MODBUS_ETH %s %d %d %d\n", l->d.writeModbusEth.name, l->d.writeModbusEth.id, l->d.writeModbusEth.address, l->d.writeModbusEth.int32);
            break;

        case ELEM_SET_PWM:
            fprintf(f, "SET_PWM %s %d\n", l->d.setPwm.name,
                l->d.setPwm.targetFreq);
            break;

        case ELEM_UART_RECV:
            fprintf(f, "UART_RECV %s\n", l->d.uart.name);
            break;

        case ELEM_UART_SEND:
            fprintf(f, "UART_SEND %s\n", l->d.uart.name);
            break;

        case ELEM_PERSIST:
            fprintf(f, "PERSIST %s\n", l->d.persist.var);
            break;

        case ELEM_READ_FORMATTED_STRING:
			{
			char s[128];
			if (strlen(l->d.fmtdStr.var) > 0)
				strcpy(s, l->d.fmtdStr.var);
			else
				strcpy(s, "(empty)");

			fprintf(f, "READ_FORMATTED_STRING %s %s\n", s, l->d.fmtdStr.string);
            break;
			}
        case ELEM_WRITE_FORMATTED_STRING:
			{
			char s[128];
			if (strlen(l->d.fmtdStr.var) > 0)
				strcpy(s, l->d.fmtdStr.var);
			else
				strcpy(s, "(empty)");

			fprintf(f, "WRITE_FORMATTED_STRING %s %s\n", s, l->d.fmtdStr.string);
            break;
			}
        case ELEM_READ_SERVO_YASKAWA:
			{
			char s[128];
			if (strlen(l->d.servoYaskawa.var) > 0)
				strcpy(s, l->d.servoYaskawa.var);
			else
				strcpy(s, "(empty)");

			fprintf(f, "READ_SERVO_YASKAWA %s %s %s\n", l->d.servoYaskawa.id, s, l->d.servoYaskawa.string);
            break;
			}
        case ELEM_WRITE_SERVO_YASKAWA:
			{
			char s[128];
			if (strlen(l->d.servoYaskawa.var) > 0)
				strcpy(s, l->d.servoYaskawa.var);
			else
				strcpy(s, "(empty)");

			fprintf(f, "WRITE_SERVO_YASKAWA %s %s %s\n", l->d.servoYaskawa.id, s, l->d.servoYaskawa.string);
            break;
			}
		case ELEM_FORMATTED_STRING: {
            int i;
            fprintf(f, "FORMATTED_STRING ");
            if(*(l->d.fmtdStr.var)) {
                fprintf(f, "%s", l->d.fmtdStr.var);
            } else {
                fprintf(f, "(none)");
            }
            fprintf(f, " %d", strlen(l->d.fmtdStr.string));
            for(i = 0; i < (int)strlen(l->d.fmtdStr.string); i++) {
                fprintf(f, " %d", l->d.fmtdStr.string[i]);
            }
            fprintf(f, "\n");
            break;
        }
        case ELEM_LOOK_UP_TABLE: {
            int i;
            fprintf(f, "LOOK_UP_TABLE %s %s %d %d", l->d.lookUpTable.dest,
                l->d.lookUpTable.index, l->d.lookUpTable.count,
                l->d.lookUpTable.editAsString);
            for(i = 0; i < l->d.lookUpTable.count; i++) {
                fprintf(f, " %d", l->d.lookUpTable.vals[i]);
            }
            fprintf(f, "\n");
            break;
        }
        case ELEM_PIECEWISE_LINEAR: {
            int i;
            fprintf(f, "PIECEWISE_LINEAR %s %s %d", l->d.piecewiseLinear.dest,
                l->d.piecewiseLinear.index, l->d.piecewiseLinear.count);
            for(i = 0; i < l->d.piecewiseLinear.count*2; i++) {
                fprintf(f, " %d", l->d.piecewiseLinear.vals[i]);
            }
            fprintf(f, "\n");
            break;
        }

        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            if(depth == 0) {
                fprintf(f, "RUNG\n");
            } else {
                fprintf(f, "SERIES\n");
            }
            for(i = 0; i < s->count; i++) {
                SaveElemToFile(f, s->contents[i].which, s->contents[i].d.any,
                    depth+1);
            }
            Indent(f, depth);
            fprintf(f, "END\n");
            break;
        }

        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *s = (ElemSubcktParallel *)any;
            int i;
            fprintf(f, "PARALLEL\n");
            for(i = 0; i < s->count; i++) {
                SaveElemToFile(f, s->contents[i].which, s->contents[i].d.any,
                    depth+1);
            }
            Indent(f, depth);
            fprintf(f, "END\n");
            break;
        }

        default:
            oops();
            break;
    }
}

//-----------------------------------------------------------------------------
// Save the program in memory to the given file. Returns TRUE for success,
// FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL SaveProjectToFile(char *filename)
{
    FILE *f = fopen(filename, "w");
    if(!f) return FALSE;

    fprintf(f, "POPTools:1.%d\n", current_version);
    if(Prog.mcu) {
        fprintf(f, "MICRO=%s\n", Prog.mcu->mcuName);
    }
    fprintf(f, "CYCLE=%d\n", Prog.cycleTime);
    fprintf(f, "CRYSTAL=%d\n", Prog.mcuClock);
    fprintf(f, "BAUD=%d\n", Prog.baudRate);
	fprintf(f, "PARITY=%d\n", Prog.UART);
	fprintf(f, "COM=%d\n", Prog.comPort);
    fprintf(f, "IP=%d.%d.%d.%d\n", Prog.ip[0], Prog.ip[1], Prog.ip[2], Prog.ip[3]);
    fprintf(f, "MASK=%d.%d.%d.%d\n", Prog.mask[0], Prog.mask[1], Prog.mask[2], Prog.mask[3]);
    fprintf(f, "GW=%d.%d.%d.%d\n", Prog.gw[0], Prog.gw[1], Prog.gw[2], Prog.gw[3]);
	fprintf(f, "DNS=%d.%d.%d.%d\n", Prog.dns[0], Prog.dns[1], Prog.dns[2], Prog.dns[3]);
	fprintf(f, "SNTP=%d-%d:%s\n", Prog.gmt, Prog.dailysave, Prog.sntp);
    if(strlen(CurrentCompileFile) > 0) {
        //fprintf(f, "COMPILED=%s\n", CurrentCompileFile);
    }

    fprintf(f, "\n");
    // list extracted from schematic, but the pin assignments are not
    fprintf(f, "IO LIST\n", Prog.mcuClock);
    SaveIoListToFile(f);
    fprintf(f, "END\n", Prog.mcuClock);

    fprintf(f, "\n", Prog.mcuClock);
    fprintf(f, "PROGRAM\n", Prog.mcuClock);

    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        SaveElemToFile(f, ELEM_SERIES_SUBCKT, Prog.rungs[i], 0);
    }

    fclose(f);
    return TRUE;
}
