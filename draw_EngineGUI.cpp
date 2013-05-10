#include "poptools.h"

#include "EngineGUI.h"
#include "EngineRenderD2D.h"

EngineGUI gui(new EngineRenderD2D);

// Is the cursor currently drawn? We XOR it so this gets toggled.
extern BOOL CursorDrawn;

// Colours with which to do syntax highlighting, configurable
extern SyntaxHighlightingColours HighlightColours;
SyntaxHighlightingColours HighlightColoursBrush;

#define INTERF_COLOR_3DLIGHT  0
#define INTERF_COLOR_3DFACE   1
#define INTERF_COLOR_3DSHADOW 2

unsigned int InterfaceColors[3];

void Draw_Init(void)
{
	gui.SetTarget(DrawWindow);
	gui.SetBackgroundColor(HighlightColours.bg);

	HighlightColoursBrush.breakpoint  = gui.CreateBrush(HighlightColours.breakpoint );
	HighlightColoursBrush.bus         = gui.CreateBrush(HighlightColours.bus        );
	HighlightColoursBrush.comment     = gui.CreateBrush(HighlightColours.comment    );
	HighlightColoursBrush.def         = gui.CreateBrush(HighlightColours.def        );
	HighlightColoursBrush.lit         = gui.CreateBrush(HighlightColours.lit        );
	HighlightColoursBrush.name        = gui.CreateBrush(HighlightColours.name       );
	HighlightColoursBrush.op          = gui.CreateBrush(HighlightColours.op         );
	HighlightColoursBrush.punct       = gui.CreateBrush(HighlightColours.punct      );
	HighlightColoursBrush.rungNum     = gui.CreateBrush(HighlightColours.rungNum    );
	HighlightColoursBrush.selected    = gui.CreateBrush(HighlightColours.selected   );
	HighlightColoursBrush.simBusLeft  = gui.CreateBrush(HighlightColours.simBusLeft );
	HighlightColoursBrush.simBusRight = gui.CreateBrush(HighlightColours.simBusRight);
	HighlightColoursBrush.simOff      = gui.CreateBrush(HighlightColours.simOff     );
	HighlightColoursBrush.simOn       = gui.CreateBrush(HighlightColours.simOn      );
	HighlightColoursBrush.simRungNum  = gui.CreateBrush(HighlightColours.simRungNum );

	InterfaceColors[INTERF_COLOR_3DLIGHT ] = gui.CreateBrush(GetSysColor(COLOR_3DLIGHT   ));
	InterfaceColors[INTERF_COLOR_3DFACE  ] = gui.CreateBrush(GetSysColor(COLOR_3DFACE    ));
	InterfaceColors[INTERF_COLOR_3DSHADOW] = gui.CreateBrush(GetSysColor(COLOR_3DDKSHADOW));
}

void Draw_Start(void)
{
	POINT offset = { -ScrollXOffset, 0 };
	gui.SetDrawOffset(offset);
	gui.StartDraw();
/*
	RECT r;
	GetClientRect(DrawWindow, &r);
	r.top += 100;

	gui.DrawText("bus", r, 0, HighlightColoursBrush.bus);
	r.top += 20;

	gui.DrawText("comment", r, 0, HighlightColoursBrush.comment);
	r.top += 20;

	gui.DrawText("def", r, 0, HighlightColoursBrush.def);
	r.top += 20;

	gui.DrawText("lit", r, 0, HighlightColoursBrush.lit);
	r.top += 20;

	gui.DrawText("name", r, 0, HighlightColoursBrush.name);
	r.top += 20;

	gui.DrawText("op", r, 0, HighlightColoursBrush.op);
	r.top += 20;

	gui.DrawText("punct", r, 0, HighlightColoursBrush.punct);
	r.top += 20;

	gui.DrawText("rungNum", r, 0, HighlightColoursBrush.rungNum);
	r.top += 20;

	gui.DrawText("selected", r, 0, HighlightColoursBrush.selected);
	r.top += 20;

	gui.DrawText("simBusLeft", r, 0, HighlightColoursBrush.simBusLeft);
	r.top += 20;

	gui.DrawText("simBusRight", r, 0, HighlightColoursBrush.simBusRight);
	r.top += 20;

	gui.DrawText("simOff", r, 0, HighlightColoursBrush.simOff);
	r.top += 20;

	gui.DrawText("simOn", r, 0, HighlightColoursBrush.simOn);
	r.top += 20;

	gui.DrawText("simRungNum", r, 0, HighlightColoursBrush.simRungNum);
	r.top += 20;*/
}

void Draw_End(void)
{
	gui.EndDraw();
}

//-----------------------------------------------------------------------------
// Convenience functions for making the text colors pretty, for DrawElement.
//-----------------------------------------------------------------------------
COLORREF currColor = HighlightColoursBrush.bus;

void NormText(void)
{
    currColor = InSimulationMode ? HighlightColoursBrush.simOff :
        HighlightColoursBrush.def;
}
void EmphText(void)
{
    currColor = InSimulationMode ? HighlightColoursBrush.simOn :
        HighlightColoursBrush.selected;
}
void NameText(void)
{
    if(!InSimulationMode && !ThisHighlighted) {
        currColor = HighlightColoursBrush.name;
    }
}
void BodyText(void)
{
    if(!InSimulationMode && !ThisHighlighted) {
        currColor = HighlightColoursBrush.def;
    }
}

void DrawLine(RECT r)
{
	int x, y;

    x = r.left*FONT_WIDTH + X_PADDING;
    y = (r.top - ScrollYOffset*POS_HEIGHT)*FONT_HEIGHT + Y_PADDING;
	POINT start = { x, y + FONT_HEIGHT / 2 };

    x = r.right*FONT_WIDTH + X_PADDING;
    y = (r.bottom - ScrollYOffset*POS_HEIGHT)*FONT_HEIGHT + Y_PADDING;
	POINT end = { x + FONT_WIDTH, y + FONT_HEIGHT / 2 };

	gui.DrawLine(start, end, currColor);
}

//-----------------------------------------------------------------------------
// Output a string to the screen at a particular location, in character-
// sized units.
//-----------------------------------------------------------------------------
void DrawCharsToEngineGUI(int cx, int cy, char *str)
{
	char ch[2] = { 0, 0 };
	RECT r = { 0, 0, 0, 0 };
	r.right = max(POS_WIDTH*FONT_WIDTH*(DISPLAY_MATRIX_X_SIZE+1), GetSystemMetrics(SM_CXVIRTUALSCREEN) + 300);
	r.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN) + 300;

	cy -= ScrollYOffset*POS_HEIGHT;
    if(cy < -2) return;
    if(cy*FONT_HEIGHT + Y_PADDING + (int)RibbonHeight > IoListTop) return;

	COLORREF prevColor = currColor;
    BOOL firstTime = TRUE;
    BOOL inNumber = FALSE;
    BOOL inComment = FALSE;
    int inBrace = 0;
    for(; *str; str++, cx++) {
        int x = cx*FONT_WIDTH + X_PADDING;
        int y = cy*FONT_HEIGHT + Y_PADDING;

        BOOL hiOk = !(InSimulationMode || ThisHighlighted);

        if(strchr("{}[]", *str) && hiOk && !inComment)  {
            if(*str == '{' || *str == '[') inBrace++;
            if(inBrace == 1) {
				r.left = x;
				r.top = y;
				ch[0] = *str;
				gui.DrawText(ch, r, 0, HighlightColoursBrush.punct);
            } else {
				r.left = x;
				r.top = y;
				ch[0] = *str;
				gui.DrawText(ch, r, 0, currColor);
            }
            if(*str == ']' || *str == '}') inBrace--;
        } else if((
            ((*str > 0 ? isdigit(*str) : false) && (firstTime || isspace(str[-1]) 
                || str[-1] == ':' || str[-1] == '[')) ||
            (*str == '-' && (*str > 0 ? isdigit(str[1]) : false))) && hiOk && !inComment)
        {
				r.left = x;
				r.top = y;
				ch[0] = *str;
				gui.DrawText(ch, r, 0, HighlightColoursBrush.lit);
            inNumber = TRUE;	
        } else if(*str == '\x01') {
            cx--;
            if(hiOk) {
				prevColor = currColor;
				currColor = HighlightColoursBrush.op;
            }
        } else if(*str == '\x02') {
            cx--;
            if(hiOk) {
				currColor = prevColor;
                inComment = FALSE;
            }
        } else if(*str == '\x03') {
            cx--;
            if(hiOk) {
				prevColor = currColor;
				currColor = HighlightColoursBrush.comment;
                inComment = TRUE;
            }
        } else if(inNumber) {
            if((*str > 0 ? isdigit(*str) : false) || *str == '.') {
				r.left = x;
				r.top = y;
				ch[0] = *str;
				gui.DrawText(ch, r, 0, HighlightColoursBrush.lit);
            } else {
				r.left = x;
				r.top = y;
				ch[0] = *str;
				gui.DrawText(ch, r, 0, currColor);
                inNumber = FALSE;
            }
		} else if(*str == '-') {
			POINT start = { x, y + FONT_HEIGHT / 2 }, end = { x + FONT_WIDTH, y + FONT_HEIGHT / 2 };
			gui.DrawLine(start, end, currColor);
		} else if(*str == '+') {
			if (x > X_PADDING - FONT_WIDTH + 3)
			{
				POINT start = { x, y + FONT_HEIGHT / 2 }, end = { x + FONT_WIDTH, y + FONT_HEIGHT / 2 };
				gui.DrawLine(start, end, currColor);

				start.x = x + FONT_WIDTH / 2;
				start.y = y + FONT_HEIGHT / 4;
				end.x = x + FONT_WIDTH / 2;
				end.y = y + ((FONT_HEIGHT / 4) * 3) + 1;
				gui.DrawLine(start, end, currColor);
			}
		} else if(*str == '|') {
			if (x > X_PADDING - FONT_WIDTH + 3)
			{
				POINT start = { x + FONT_WIDTH / 2, y }, end = { x + FONT_WIDTH / 2, y + FONT_HEIGHT };
				gui.DrawLine(start, end, currColor);
			}
        } else {
				r.left = x;
				r.top = y;
				ch[0] = *str;
				gui.DrawText(ch, r, 0, currColor);
        }

        firstTime = FALSE;
    }
}

HRESULT InitRibbon(HWND hWindowFrame);
HRESULT UpdateRibbonHeight(void);

//-----------------------------------------------------------------------------
// Paint the ladder logic program to the screen. Also figure out where the
// cursor should go and fill in coordinates for BlinkCursor. Not allowed to
// draw deeper than IoListTop, as we would run in to the I/O listbox.
//-----------------------------------------------------------------------------
void PaintWindow(void)
{
	static bool first = true;

    ok();

    RECT r;
    GetClientRect(DrawWindow, &r);
    int bw = r.right;
    int bh = IoListTop;

	// Update Ribbon Height
	if(!RibbonHeight || first) {
		first = false;
		UpdateRibbonHeight();
	}

	gui.SetBackgroundColor(InSimulationMode ? HighlightColours.simBg : HighlightColours.bg);

    // now figure out how we should draw the ladder logic
    ColsAvailable = ProgCountWidestRow();
    if(ColsAvailable < ScreenColsAvailable()) {
        ColsAvailable = ScreenColsAvailable();
    }
    memset(DisplayMatrix, 0, sizeof(DisplayMatrix));
    SelectionActive = FALSE;
    memset(&Cursor, 0, sizeof(Cursor));

    DrawChars = DrawCharsToEngineGUI;

    int i;
    int cy = 0;
    int rowsAvailable = ScreenRowsAvailable();
    for(i = 0; i < Prog.numRungs; i++) {
        int thisHeight = POS_HEIGHT*CountHeightOfElement(ELEM_SERIES_SUBCKT,
            Prog.rungs[i]);

        // For speed, there is no need to draw everything all the time, but
        // we still must draw a bit above and below so that the DisplayMatrix
        // is filled in enough to make it possible to reselect using the
        // cursor keys.
        if(((cy + thisHeight) >= (ScrollYOffset - 8)*POS_HEIGHT) &&
            (cy < (ScrollYOffset + rowsAvailable + 8)*POS_HEIGHT))
        {
            int rung = i + 1;
            int y = Y_PADDING + FONT_HEIGHT*cy;
            int yp = y + FONT_HEIGHT*(POS_HEIGHT/2) - 
                POS_HEIGHT*FONT_HEIGHT*ScrollYOffset;

			if(Prog.rungHasBreakPoint[i]) {
				int center = (X_PADDING - FONT_WIDTH + 3)/2;
				r.left   = center-X_PADDING/4;
				r.right  = center+X_PADDING/4;
				r.top    = yp+FONT_HEIGHT;
				r.bottom = yp+FONT_HEIGHT+X_PADDING/2;
				gui.DrawEllipse(r, HighlightColoursBrush.breakpoint);
			}

		    GetClientRect(DrawWindow, &r);
			char ch[4] = { 0, 0, 0, 0 };
            if(rung < 10) {
                ch[0] = rung + '0';
				r.left = 5 + FONT_WIDTH; r.top = yp;
			} else if(rung < 100) {
                ch[0] = (rung / 10) + '0';
				ch[1] = (rung % 10) + '0';
				r.left = 5; r.top = yp;
            } else {
				_itoa(rung, ch, 10);
				r.left = 5; r.top = yp;
            }
			gui.DrawText(ch, r, 0, InSimulationMode ? HighlightColoursBrush.simRungNum : HighlightColoursBrush.rungNum);

            int cx = 0;
			// If DrawElement returned TRUE and has breakpoint for this line, pause simulation.
            if(DrawElement(ELEM_SERIES_SUBCKT, Prog.rungs[i], &cx, &cy, 
                Prog.rungPowered[i]) && RealTimeSimulationRunning && Prog.rungHasBreakPoint[i]) {
					PauseSimulation();
					Error(_("Simulação interrompida na linha %d"), i+1);
			}
        }

        cy += thisHeight;
        cy += POS_HEIGHT;
    }
    cy -= 2;
    DrawEndRung(0, cy);

    if(SelectedGxAfterNextPaint >= 0) {
        MoveCursorNear(SelectedGxAfterNextPaint, SelectedGyAfterNextPaint);
        InvalidateRect(MainWindow, NULL, FALSE);
        SelectedGxAfterNextPaint = -1;
        SelectedGyAfterNextPaint = -1;
    } else if(ScrollSelectedIntoViewAfterNextPaint && Selected) {
        SelectElement(-1, -1, Selected->selectedState);
        ScrollSelectedIntoViewAfterNextPaint = FALSE;
        InvalidateRect(MainWindow, NULL, FALSE);
    } else {
        if(!SelectionActive) {
            if(Prog.numRungs > 0) {
                if(MoveCursorTopLeft()) {
                    InvalidateRect(MainWindow, NULL, FALSE);
                }
            }
        }
    }

    // draw the `buses' at either side of the screen
    r.left = X_PADDING - FONT_WIDTH + 3;
    r.top = 0;
    r.right = r.left + 4;
    r.bottom = IoListTop;
	gui.DrawRectangle(r, InSimulationMode ? HighlightColoursBrush.simBusLeft : HighlightColoursBrush.bus);

    r.left += POS_WIDTH*FONT_WIDTH*ColsAvailable + 3;
    r.right += POS_WIDTH*FONT_WIDTH*ColsAvailable + 3;
	gui.DrawRectangle(r, InSimulationMode ? HighlightColoursBrush.simBusRight : HighlightColoursBrush.bus);
 
    CursorDrawn = FALSE;

    if(InSimulationMode) {
        KillTimer(MainWindow, TIMER_BLINK_CURSOR);
    } else {
        KillTimer(MainWindow, TIMER_BLINK_CURSOR);
        BlinkCursor(NULL, 0, NULL, 0);
        SetTimer(MainWindow, TIMER_BLINK_CURSOR, 800, BlinkCursor);
    }

    ok();
}

void PaintScrollAndSplitter(void)
{
	RECT r;
	// Fill around the scroll bars
	if(NeedHoriz) {
		r.top = IoListTop - ScrollHeight - 2;
		r.bottom = IoListTop - 2;
		gui.DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DFACE]);
	}
	GetClientRect(MainWindow, &r);
	r.right += ScrollXOffset;
	r.left = r.right - ScrollWidth - 2;
	gui.DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DFACE]);

	// Draw the splitter thing to grab to resize the I/O listview.
	GetClientRect(DrawWindow, &r);
	r.left  += ScrollXOffset;
	r.right += ScrollXOffset;

	r.top = IoListTop - 2 - RibbonHeight;
	r.bottom = IoListTop - RibbonHeight;
	gui.DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DFACE  ]);
	r.top = IoListTop - 2 - RibbonHeight;
	r.bottom = IoListTop - 1 - RibbonHeight;
	gui.DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DLIGHT ]);
	r.top = IoListTop - RibbonHeight;
	r.bottom = IoListTop + 1 - RibbonHeight;
	gui.DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DSHADOW]);
}
