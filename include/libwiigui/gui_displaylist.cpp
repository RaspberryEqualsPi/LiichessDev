/****************************************************************************
 * libwiigui
 *
 * Tantric 2009
 *
 * gui_optionbrowser.cpp
 *
 * GUI class definitions
 ***************************************************************************/

// this file has been modified by 26gy2 in order to make it more suitable for a chat box

#include "gui.h"

/**
 * Constructor for the GuiDisplayList class.
 */
#define OBFONTSIZEVALUE 10
#define OBFONTSIZENAME 10
GuiDisplayList::GuiDisplayList(int w, int h, DisplayList * l)
{
	width = w;
	height = h;
	options = l;
	selectable = true;
	listOffset = this->FindMenuItem(-1, 1);
	listChanged = true; // trigger an initial list update
	selectedItem = 0;
	focus = 0; // allow focus

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trig2 = new GuiTrigger;
	trig2->SetSimpleTrigger(-1, WPAD_BUTTON_2, 0);

	btnSoundOver = new GuiSound(button_over_pcm, button_over_pcm_size, SOUND_PCM);
	btnSoundClick = new GuiSound(button_click_pcm, button_click_pcm_size, SOUND_PCM);

	bgOptions = new GuiImageData(bg_chats_png);
	bgOptionsImg = new GuiImage(bgOptions);
	bgOptionsImg->SetParent(this);
	bgOptionsImg->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);

	bgOptionsEntry = new GuiImageData(bg_options_entry_png);

	scrollbar = new GuiImageData(scrollbar_png);
	scrollbarImg = new GuiImage(scrollbar);
	scrollbarImg->SetParent(this);
	scrollbarImg->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	scrollbarImg->SetPosition(0, 30);

	arrowDown = new GuiImageData(scrollbar_arrowdown_png);
	arrowDownImg = new GuiImage(arrowDown);
	arrowDownOver = new GuiImageData(scrollbar_arrowdown_over_png);
	arrowDownOverImg = new GuiImage(arrowDownOver);
	arrowUp = new GuiImageData(scrollbar_arrowup_png);
	arrowUpImg = new GuiImage(arrowUp);
	arrowUpOver = new GuiImageData(scrollbar_arrowup_over_png);
	arrowUpOverImg = new GuiImage(arrowUpOver);

	arrowUpBtn = new GuiButton(arrowUpImg->GetWidth(), arrowUpImg->GetHeight());
	arrowUpBtn->SetParent(this);
	arrowUpBtn->SetImage(arrowUpImg);
	arrowUpBtn->SetImageOver(arrowUpOverImg);
	arrowUpBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	arrowUpBtn->SetPosition(-1, 2);
	arrowUpBtn->SetSelectable(false);
	arrowUpBtn->SetTrigger(trigA);
	arrowUpBtn->SetSoundOver(btnSoundOver);
	arrowUpBtn->SetSoundClick(btnSoundClick);

	arrowDownBtn = new GuiButton(arrowDownImg->GetWidth(), arrowDownImg->GetHeight());
	arrowDownBtn->SetParent(this);
	arrowDownBtn->SetImage(arrowDownImg);
	arrowDownBtn->SetImageOver(arrowDownOverImg);
	arrowDownBtn->SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	arrowDownBtn->SetPosition(-1, -2);
	arrowDownBtn->SetSelectable(false);
	arrowDownBtn->SetTrigger(trigA);
	arrowDownBtn->SetSoundOver(btnSoundOver);
	arrowDownBtn->SetSoundClick(btnSoundClick);

	for(int i=0; i<PAGESIZE; i++)
	{
		optionVal[i] = new GuiText(NULL, OBFONTSIZENAME, (GXColor){0, 0, 0, 255});
		//optionVal[i]->Set
		optionVal[i]->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
		optionVal[i]->SetPosition(8,0);
		optionVal[i]->SetMaxWidth(width - 16);
		//optionVal[i]->SetWrap(true, width - 16);
		//optionVal[i]->SetScroll(true);

		//optionVal[i] = new GuiText(NULL, OBFONTSIZEVALUE, (GXColor){0, 0, 0, 0xff});
		//optionVal[i]->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
		//optionVal[i]->SetPosition(250,0);

		optionBg[i] = new GuiImage(bgOptionsEntry);

		optionBtn[i] = new GuiButton(width,15);
		optionBtn[i]->SetParent(this);
		optionBtn[i]->SetLabel(optionVal[i], 0);
		//optionBtn[i]->SetLabel(optionVal[i], 1);
		//optionBtn[i]->SetImageOver(optionBg[i]);
		optionBtn[i]->SetPosition(2,15*i+6);
		optionBtn[i]->SetTrigger(trigA);
		optionBtn[i]->SetTrigger(trig2);
		optionBtn[i]->SetSoundClick(btnSoundClick);
	}
}

/**
 * Destructor for the GuiDisplayList class.
 */
GuiDisplayList::~GuiDisplayList()
{
	delete arrowUpBtn;
	delete arrowDownBtn;

	delete bgOptionsImg;
	delete scrollbarImg;
	delete arrowDownImg;
	delete arrowDownOverImg;
	delete arrowUpImg;
	delete arrowUpOverImg;

	delete bgOptions;
	delete bgOptionsEntry;
	delete scrollbar;
	delete arrowDown;
	delete arrowDownOver;
	delete arrowUp;
	delete arrowUpOver;

	delete trigA;
	delete trig2;
	delete btnSoundOver;
	delete btnSoundClick;

	for(int i=0; i<PAGESIZE; i++)
	{
		//delete optionTxt[i];
		delete optionVal[i];
		delete optionBg[i];
		delete optionBtn[i];
	}
}

void GuiDisplayList::SetColPosition(int x)
{
	for(int i=0; i<PAGESIZE; i++)
		optionVal[i]->SetPosition(x,0);
}

void GuiDisplayList::SetFocus(int f)
{
	focus = f;

	for(int i=0; i<PAGESIZE; i++)
		optionBtn[i]->ResetState();

	if(f == 1)
		optionBtn[selectedItem]->SetState(STATE_SELECTED);
}

void GuiDisplayList::ResetState()
{
	if(state != STATE_DISABLED)
	{
		state = STATE_DEFAULT;
		stateChan = -1;
	}

	for(int i=0; i<PAGESIZE; i++)
	{
		optionBtn[i]->ResetState();
	}
}

int GuiDisplayList::GetClickedOption()
{
	int found = -1;
	for(int i=0; i<PAGESIZE; i++)
	{
		if(optionBtn[i]->GetState() == STATE_CLICKED)
		{
			optionBtn[i]->SetState(STATE_SELECTED);
			found = optionIndex[i];
			break;
		}
	}
	return found;
}

/****************************************************************************
 * FindMenuItem
 *
 * Help function to find the next visible menu item on the list
 ***************************************************************************/

int GuiDisplayList::FindMenuItem(int currentItem, int direction)
{
	int nextItem = currentItem + direction;

	if(nextItem < 0 || nextItem >= options->fields.size())
		return -1;

	if(options->fields[nextItem].size() > 0)
		return nextItem;
	else
		return FindMenuItem(nextItem, direction);
}

/**
 * Draw the button on screen
 */
void GuiDisplayList::Draw()
{
	if(!this->IsVisible())
		return;

	bgOptionsImg->Draw();

	int next = listOffset;

	for(int i=0; i<PAGESIZE; ++i)
	{
		if(next >= 0)
		{
			optionBtn[i]->Draw();
			next = this->FindMenuItem(next, 1);
		}
		else
			break;
	}

	scrollbarImg->Draw();
	arrowUpBtn->Draw();
	arrowDownBtn->Draw();

	this->UpdateEffects();
}

void GuiDisplayList::TriggerUpdate()
{

	listOffset = (options->fields.size() >= PAGESIZE) ? (options->fields.size() - PAGESIZE) : 0; // autoscroll

		/*if(next >= 0)
		{
			if(selectedItem == PAGESIZE-1)
			{
				// move list down by 1
				listOffset = this->FindMenuItem(listOffset, 1);
				listChanged = true;
			}
			else if(optionBtn[selectedItem+1]->IsVisible())
			{
				optionBtn[selectedItem]->ResetState();
				//optionBtn[selectedItem+1]->SetState(STATE_SELECTED, t->chan);
				++selectedItem;
			}
		}*/
	
	listChanged = true;
}

void GuiDisplayList::ResetText()
{
	int next = listOffset;

	for(int i=0; i<PAGESIZE; i++)
	{
		if(next >= 0)
		{
			optionBtn[i]->ResetText();
			next = this->FindMenuItem(next, 1);
		}
		else
			break;
	}
}

void GuiDisplayList::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t)
		return;

	int next, prev;

	arrowUpBtn->Update(t);
	arrowDownBtn->Update(t);

	next = listOffset;

	if(listChanged)
	{
		listChanged = false;
		for(int i=0; i<PAGESIZE; ++i)
		{
			if(next >= 0)
			{
				if(optionBtn[i]->GetState() == STATE_DISABLED)
				{
					optionBtn[i]->SetVisible(true);
					optionBtn[i]->SetState(STATE_DEFAULT);
				}

				//optionTxt[i]->SetText(options->name[next]);
				optionVal[i]->SetText(options->fields[next].c_str());
				optionIndex[i] = next;
				next = this->FindMenuItem(next, 1);
			}
			else
			{
				optionBtn[i]->SetVisible(false);
				optionBtn[i]->SetState(STATE_DISABLED);
			}
		}
	}

	for(int i=0; i<PAGESIZE; ++i)
	{
		if(i != selectedItem && optionBtn[i]->GetState() == STATE_SELECTED)
			optionBtn[i]->ResetState();
		//else if(focus && i == selectedItem && optionBtn[i]->GetState() == STATE_DEFAULT)
		//	optionBtn[selectedItem]->SetState(STATE_SELECTED, t->chan);

		int currChan = t->chan;

		if(t->wpad->ir.valid && !optionBtn[i]->IsInside(t->wpad->ir.x, t->wpad->ir.y))
			t->chan = -1;

		optionBtn[i]->Update(t);
		t->chan = currChan;

		//if(optionBtn[i]->GetState() == STATE_SELECTED)
		//	selectedItem = i;
	}

	// pad/joystick navigation
	if(!focus)
		return; // skip navigation

	if(t->Down() || arrowDownBtn->GetState() == STATE_CLICKED)
	{
		/*next = this->FindMenuItem(optionIndex[selectedItem], 1);

		if(next >= 0)
		{
			if(selectedItem == PAGESIZE-1)
			{
				// move list down by 1
				listOffset = this->FindMenuItem(listOffset, 1);
				listChanged = true;
			}
			else if(optionBtn[selectedItem+1]->IsVisible())
			{
				optionBtn[selectedItem]->ResetState();
				optionBtn[selectedItem+1]->SetState(STATE_SELECTED, t->chan);
				++selectedItem;
			}
		}*/
		next = this->FindMenuItem(listOffset + PAGESIZE - 1, 1);
		if (next >= 0){
			listOffset = next - PAGESIZE + 1;
			listChanged = true;
			//selectedItem++;
		}
		arrowDownBtn->ResetState();
		//listChanged = true;
	}
	else if(t->Up() || arrowUpBtn->GetState() == STATE_CLICKED)
	{
		/*prev = this->FindMenuItem(optionIndex[selectedItem], -1);

		if(prev >= 0)
		{
			if(selectedItem == 0)
			{
				// move list up by 1
				listOffset = prev;
				listChanged = true;
			}
			else
			{
				optionBtn[selectedItem]->ResetState();
				optionBtn[selectedItem-1]->SetState(STATE_SELECTED, t->chan);
				--selectedItem;
			}
		}*/
		prev = this->FindMenuItem(listOffset + PAGESIZE - 1, -1);
		if (prev >= 0 && listOffset > 0) {
			listOffset = prev - PAGESIZE + 1;
			listChanged = true;
			//selectedItem--;
		}
		arrowUpBtn->ResetState();
	}

	if(updateCB)
		updateCB(this);
}
