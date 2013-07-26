#include "poptools.h"

unsigned int FindAndReplace(string search_text, string new_text, eSearchAndReplaceMode mode)
{
	if(search_text.size() == 0) {
		if(!strlen(POPSettings.last_search_text)) {
			return 0;
		} else {
			search_text = POPSettings.last_search_text;
		}
	} else {
		strcpy(POPSettings.last_search_text, search_text.c_str());
	}

	if(new_text.size() == 0) {
		new_text = POPSettings.last_new_text;
	} else {
		strcpy(POPSettings.last_new_text, new_text.c_str());
	}

	int ret = ladder->SearchAndReplace(search_text, new_text, mode);
	if(ret < 0) {
		return 0;
	} else {
		return ret;
	}
}

//-----------------------------------------------------------------------------
// Move the cursor in response to a keyboard command (arrow keys). Basically
// we move the cursor within the currently selected element until we hit
// the edge (e.g. left edge for VK_LEFT), and then we see if we can select
// a new item that would let us keep going.
//-----------------------------------------------------------------------------
void MoveCursorKeyboard(int keyCode, BOOL shiftPressed)
{
	eMoveCursor moveto;

    switch(keyCode) {
		case VK_HOME:
			if(shiftPressed) {
				moveto = eMoveCursor_DiagramHome;
			} else {
				moveto = eMoveCursor_RungHome;
			}
			break;

        case VK_LEFT:
			moveto = eMoveCursor_Left;
            break;

		case VK_END:
			if(shiftPressed) {
				moveto = eMoveCursor_DiagramEnd;
			} else {
				moveto = eMoveCursor_RungEnd;
			}
			break;

		case VK_RIGHT:
			moveto = eMoveCursor_Right;
            break;

        case VK_UP:
			moveto = eMoveCursor_Up;
            break;

        case VK_DOWN:
			moveto = eMoveCursor_Down;
            break;

		default:
			return;
	}

	ladder->MoveCursor(moveto);
}

//-----------------------------------------------------------------------------
// Negate the selected item, if this is meaningful.
//-----------------------------------------------------------------------------
bool NegateSelected(void)
{
	LadderContext context = ladder->getContext();
	if(context.SelectedElem == nullptr) return false; // nenhum objeto selecionado

	switch(context.SelectedElem->getWhich()) {
        case ELEM_CONTACTS: {
			LadderElemContactProp *prop = (LadderElemContactProp *)context.SelectedElem->getProperties();
            if(prop->negated == false) {
				prop->negated = true;
				context.SelectedElem->setProperties(context, prop);
				ladder->updateContext();
			} else {
				delete prop;
				return false;
			}
            break;
		}

        case ELEM_COIL: {
			LadderElemCoilProp *prop = (LadderElemCoilProp *)context.SelectedElem->getProperties();
            if(!prop->negated || prop->setOnly || prop->resetOnly) {
				prop->negated   = true;
				prop->setOnly   = false;
				prop->resetOnly = false;
				context.SelectedElem->setProperties(context, prop);
				ladder->updateContext();
			} else {
				delete prop;
				return false;
			}
            break;
        }
        default:
			return false;
            break;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Make the item selected normal: not negated, not set/reset only.
//-----------------------------------------------------------------------------
bool MakeNormalSelected(void)
{
	LadderContext context = ladder->getContext();
	if(context.SelectedElem == nullptr) return false; // nenhum objeto selecionado

	switch(context.SelectedElem->getWhich()) {
        case ELEM_CONTACTS: {
			LadderElemContactProp *prop = (LadderElemContactProp *)context.SelectedElem->getProperties();
            if(prop->negated == true) {
				prop->negated = false;
				context.SelectedElem->setProperties(context, prop);
				ladder->updateContext();
			} else {
				delete prop;
				return false;
			}
            break;
		}

        case ELEM_COIL: {
			LadderElemCoilProp *prop = (LadderElemCoilProp *)context.SelectedElem->getProperties();
            if(prop->negated || prop->setOnly || prop->resetOnly) {
				prop->negated   = false;
				prop->setOnly   = false;
				prop->resetOnly = false;
				context.SelectedElem->setProperties(context, prop);
				ladder->updateContext();
			} else {
				delete prop;
				return false;
			}
            break;
        }
        default:
			return false;
            break;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Make the selected item set-only, if it is a coil.
//-----------------------------------------------------------------------------
bool MakeSetOnlySelected(void)
{
	LadderContext context = ladder->getContext();
	if(context.SelectedElem == nullptr) return false; // nenhum objeto selecionado

	if(context.SelectedElem->getWhich() == ELEM_COIL) {
		LadderElemCoilProp *prop = (LadderElemCoilProp *)context.SelectedElem->getProperties();
        if(prop->negated || !prop->setOnly || prop->resetOnly) {
			prop->negated   = false;
			prop->setOnly   = true;
			prop->resetOnly = false;
			context.SelectedElem->setProperties(context, prop);
			ladder->updateContext();
		} else {
			delete prop;
			return false; // nao alterado
		}
    }

	return true;
}

//-----------------------------------------------------------------------------
// Make the selected item reset-only, if it is a coil.
//-----------------------------------------------------------------------------
bool MakeResetOnlySelected(void)
{
	LadderContext context = ladder->getContext();
	if(context.SelectedElem == nullptr) return false; // nenhum objeto selecionado

	if(context.SelectedElem->getWhich() == ELEM_COIL) {
		LadderElemCoilProp *prop = (LadderElemCoilProp *)context.SelectedElem->getProperties();
        if(prop->negated || prop->setOnly || !prop->resetOnly) {
			prop->negated   = false;
			prop->setOnly   = false;
			prop->resetOnly = true;
			context.SelectedElem->setProperties(context, prop);
			ladder->updateContext();
		} else {
			delete prop;
			return false; // nao alterado
		}
    }

	return true;
}
