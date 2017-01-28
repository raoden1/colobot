// scroll.cpp

#define STRICT
#define D3D_OVERLOADS

#include <windows.h>
#include <stdio.h>
#include <d3d.h>

#include "struct.h"
#include "D3DEngine.h"
#include "math3d.h"
#include "event.h"
#include "misc.h"
#include "iman.h"
#include "button.h"
#include "scroll.h"




// Constructeur de l'objet.

CScroll::CScroll(CInstanceManager* iMan) : CControl(iMan)
{
	CControl::CControl(iMan);

	m_buttonUp   = 0;
	m_buttonDown = 0;

	m_visibleValue = 0.0f;
	m_visibleRatio = 1.0f;
	m_step         = 0.0f;

	m_eventUp   = EVENT_NULL;
	m_eventDown = EVENT_NULL;

	m_bCapture = FALSE;
}

// Destructeur de l'objet.

CScroll::~CScroll()
{
	delete m_buttonUp;
	delete m_buttonDown;

	CControl::~CControl();
}


// Cr�e un nouveau bouton.

BOOL CScroll::Create(FPOINT pos, FPOINT dim, int icon, EventMsg eventMsg)
{
	if ( eventMsg == EVENT_NULL )  eventMsg = GetUniqueEventMsg();
	CControl::Create(pos, dim, icon, eventMsg);

	MoveAdjust();
	return TRUE;
}


void CScroll::SetPos(FPOINT pos)
{
	CControl::SetPos(pos);
	MoveAdjust();
}

void CScroll::SetDim(FPOINT dim)
{
	CControl::SetDim(dim);
	MoveAdjust();
}

// Ajuste les deux boutons.

void CScroll::MoveAdjust()
{
	CButton*	pc;
	FPOINT		pos, dim;

	if ( m_dim.y < m_dim.x*2.0f )  // ascenseur tr�s court ?
	{
		delete m_buttonUp;
		m_buttonUp = 0;

		delete m_buttonDown;
		m_buttonDown = 0;
	}
	else
	{
		if ( m_buttonUp == 0 )
		{
			m_buttonUp = new CButton(m_iMan);
			pc = (CButton*)m_buttonUp;
			pc->Create(FPOINT(0.0f, 0.0f), FPOINT(0.0f, 0.0f), 49, EVENT_NULL);
			pc->SetRepeat(TRUE);
			m_eventUp = pc->RetEventMsg();
		}

		if ( m_buttonDown == 0 )
		{
			m_buttonDown = new CButton(m_iMan);
			pc = (CButton*)m_buttonDown;
			pc->Create(FPOINT(0.0f, 0.0f), FPOINT(0.0f, 0.0f), 50, EVENT_NULL);
			pc->SetRepeat(TRUE);
			m_eventDown = pc->RetEventMsg();
		}
	}

	if ( m_buttonUp != 0 )
	{
		pos.x = m_pos.x;
		pos.y = m_pos.y+m_dim.y-m_dim.x/0.75f;
		dim.x = m_dim.x;
		dim.y = m_dim.x/0.75f;
		m_buttonUp->SetPos(pos);
		m_buttonUp->SetDim(dim);
	}

	if ( m_buttonDown != 0 )
	{
		pos.x = m_pos.x;
		pos.y = m_pos.y;
		dim.x = m_dim.x;
		dim.y = m_dim.x/0.75f;
		m_buttonDown->SetPos(pos);
		m_buttonDown->SetDim(dim);
	}

	AdjustGlint();
}

// Ajuste la position du reflet.

void CScroll::AdjustGlint()
{
	FPOINT	ref;
	float	hButton, h;

	hButton = m_buttonUp?m_dim.x/0.75f:0.0f;
	h = m_dim.y-hButton*2.0f;

	ref.x = m_pos.x;
	ref.y = m_pos.y+hButton+h*m_visibleRatio+0.003f;
	ref.y += h*(1.0f-m_visibleRatio)*(1.0f-m_visibleValue);

	GlintCreate(ref);
}



BOOL CScroll::SetState(int state, BOOL bState)
{
	if ( state & STATE_ENABLE )
	{
		if ( m_buttonUp   != 0 )  m_buttonUp->SetState(state, bState);
		if ( m_buttonDown != 0 )  m_buttonDown->SetState(state, bState);
	}

	return CControl::SetState(state, bState);
}

BOOL CScroll::SetState(int state)
{
	if ( state & STATE_ENABLE )
	{
		if ( m_buttonUp   != 0 )  m_buttonUp->SetState(state);
		if ( m_buttonDown != 0 )  m_buttonDown->SetState(state);
	}

	return CControl::SetState(state);
}

BOOL CScroll::ClearState(int state)
{
	if ( state & STATE_ENABLE )
	{
		if ( m_buttonUp   != 0 )  m_buttonUp->ClearState(state);
		if ( m_buttonDown != 0 )  m_buttonDown->ClearState(state);
	}

	return CControl::ClearState(state);
}


// Gestion d'un �v�nement.

BOOL CScroll::EventProcess(const Event &event)
{
	FPOINT	pos, dim;
	float	hButton, h, value;

	CControl::EventProcess(event);

	if ( m_buttonUp != 0 && !m_bCapture )
	{
		if ( !m_buttonUp->EventProcess(event) )  return FALSE;
	}
	if ( m_buttonDown != 0 && !m_bCapture )
	{
		if ( !m_buttonDown->EventProcess(event) )  return FALSE;
	}

	if ( event.event == m_eventUp && m_step > 0.0f )
	{
		m_visibleValue -= m_step;
		if ( m_visibleValue < 0.0f )  m_visibleValue = 0.0f;
		AdjustGlint();

		Event newEvent = event;
		newEvent.event = m_eventMsg;
		m_event->AddEvent(newEvent);
	}

	if ( event.event == m_eventDown && m_step > 0.0f )
	{
		m_visibleValue += m_step;
		if ( m_visibleValue > 1.0f )  m_visibleValue = 1.0f;
		AdjustGlint();

		Event newEvent = event;
		newEvent.event = m_eventMsg;
		m_event->AddEvent(newEvent);
	}

	hButton = m_buttonUp?m_dim.x/0.75f:0.0f;

	if ( event.event == EVENT_LBUTTONDOWN &&
		 (m_state & STATE_VISIBLE)        &&
		 (m_state & STATE_ENABLE)         )
	{
		if ( CControl::Detect(event.pos) )
		{
			pos.y = m_pos.y+hButton;
			dim.y = m_dim.y-hButton*2.0f;
			pos.y += dim.y*(1.0f-m_visibleRatio)*(1.0f-m_visibleValue);
			dim.y *= m_visibleRatio;
			if ( event.pos.y < pos.y       ||
				 event.pos.y > pos.y+dim.y )  // clic hors cabine ?
			{
				h = (m_dim.y-hButton*2.0f)*(1.0f-m_visibleRatio);
				value = 1.0f-(event.pos.y-(m_pos.y+hButton+dim.y*0.5f))/h;
				if ( value < 0.0f )  value = 0.0f;
				if ( value > 1.0f )  value = 1.0f;
				m_visibleValue = value;
				AdjustGlint();

				Event newEvent = event;
				newEvent.event = m_eventMsg;
				m_event->AddEvent(newEvent);
			}
			m_bCapture = TRUE;
			m_pressPos = event.pos;
			m_pressValue = m_visibleValue;
		}
	}

	if ( event.event == EVENT_MOUSEMOVE && m_bCapture )
	{
		h = (m_dim.y-hButton*2.0f)*(1.0f-m_visibleRatio);
		if ( h != 0 )
		{
			value = m_pressValue - (event.pos.y-m_pressPos.y)/h;
			if ( value < 0.0f )  value = 0.0f;
			if ( value > 1.0f )  value = 1.0f;

			if ( value != m_visibleValue )
			{
				m_visibleValue = value;
				AdjustGlint();

				Event newEvent = event;
				newEvent.event = m_eventMsg;
				m_event->AddEvent(newEvent);
			}
		}
	}

	if ( event.event == EVENT_LBUTTONUP && m_bCapture )
	{
		m_bCapture = FALSE;
	}

	if ( event.event == EVENT_KEYDOWN &&
		 event.param == VK_WHEELUP    &&
		 Detect(event.pos)            &&
		 m_buttonUp != 0              )
	{
		Event newEvent = event;
		newEvent.event = m_buttonUp->RetEventMsg();
		m_event->AddEvent(newEvent);
	}
	if ( event.event == EVENT_KEYDOWN &&
		 event.param == VK_WHEELDOWN  &&
		 Detect(event.pos)            &&
		 m_buttonDown != 0            )
	{
		Event newEvent = event;
		newEvent.event = m_buttonDown->RetEventMsg();
		m_event->AddEvent(newEvent);
	}

	return TRUE;
}


// Dessine le bouton.

void CScroll::Draw()
{
	FPOINT	pos, dim, ppos, ddim;
	POINT	wdim;
	float	hButton, h;
	int		icon, i, n;

	if ( m_state & STATE_SHADOW )
	{
		DrawShadow(m_pos, m_dim);
	}

	hButton = m_buttonUp?m_dim.x/0.75f:0.0f;

	// Dessine le fond.
	pos.x = m_pos.x;
	pos.y = m_pos.y+hButton;
	dim.x = m_dim.x;
	dim.y = m_dim.y-hButton*2.0f;
	if ( m_state & STATE_ENABLE )  icon = 0;
	else                           icon = 1;
	DrawVertex(pos, dim, icon);

	// Dessine la cabine.
	if ( m_visibleRatio < 1.0f && (m_state & STATE_ENABLE) )
	{
		pos.x += 0.004f;  // ch'tite marge
		pos.y += 0.004f;
		dim.x -= 0.008f;
		dim.y -= 0.008f;
		pos.y += dim.y*(1.0f-m_visibleRatio)*(1.0f-m_visibleValue);
		dim.y *= m_visibleRatio;
		DrawVertex(pos, dim, 2);

		wdim = m_engine->RetDim();
		if ( wdim.y <= 768.0f )  // 1024x768 ou moins ?
		{
			h = 1.0f/wdim.y;  // 1 pixel
		}
		else
		{
			h = 2.0f/wdim.y;  // 2 pixels
		}

		n = (int)(dim.y*0.8f/(h*3.0f));
		if ( n < 1 )  n = 1;
		if ( n > 5 )  n = 5;

		ppos.x = pos.x+0.003f;
		ppos.y = pos.y+(dim.y-(n-1)*(h*3.0f)-(h*2.0f))/2.0f;
		ddim.x = dim.x-0.006f;
		ddim.y = (h*2.0f);
		for ( i=0 ; i<n ; i++ )
		{
			DrawVertex(ppos, ddim, 3);  // barre horizontale
			ppos.y += (h*3.0f);
		}
	}

	if ( m_buttonUp != 0 )
	{
		m_buttonUp->Draw();
	}
	if ( m_buttonDown != 0 )
	{
		m_buttonDown->Draw();
	}
}

// Dessine un rectangle.

void CScroll::DrawVertex(FPOINT pos, FPOINT dim, int icon)
{
	FPOINT		uv1, uv2;
	float		ex, dp;

	if ( icon == 0 )
	{
		m_engine->SetTexture("button1.tga");
		m_engine->SetState(D3DSTATENORMAL);
//?		m_engine->SetState(D3DSTATETTb);
		uv1.x = 128.0f/256.0f;  // rectangle jaune
		uv1.y =  16.0f/256.0f;
		uv2.x = 144.0f/256.0f;
		uv2.y =  32.0f/256.0f;
		ex = 4.0f/256.0f;
	}
	else if ( icon == 1 )
	{
		m_engine->SetTexture("button1.tga");
//?		m_engine->SetState(D3DSTATENORMAL);
		m_engine->SetState(D3DSTATETTb);
		uv1.x = 224.0f/256.0f;  // rectangle gris
		uv1.y =   0.0f/256.0f;
		uv2.x = 256.0f/256.0f;
		uv2.y =  32.0f/256.0f;
		ex = 8.0f/256.0f;
	}
	else if ( icon == 2 )
	{
		m_engine->SetTexture("button1.tga");
//?		m_engine->SetState(D3DSTATENORMAL);
		m_engine->SetState(D3DSTATETTb);
		uv1.x = 224.0f/256.0f;  // rectangle bleu
		uv1.y =  96.0f/256.0f;
		uv2.x = 256.0f/256.0f;
		uv2.y = 128.0f/256.0f;
		ex = 8.0f/256.0f;
	}
	else
	{
		m_engine->SetTexture("button1.tga");
//?		m_engine->SetState(D3DSTATENORMAL);
		m_engine->SetState(D3DSTATETTb);
		uv1.x =   8.0f/256.0f;  // ligne -
		uv1.y =  44.0f/256.0f;
		uv2.x =  24.0f/256.0f;
		uv2.y =  48.0f/256.0f;
		ex = 0.0f;
	}

	dp = 0.5f/256.0f;
	uv1.x += dp;
	uv1.y += dp;
	uv2.x -= dp;
	uv2.y -= dp;

	DrawIcon(pos, dim, uv1, uv2, ex);
}


void CScroll::SetVisibleValue(float value)
{
	if ( value < 0.0 )  value = 0.0f;
	if ( value > 1.0 )  value = 1.0f;
	m_visibleValue = value;
	AdjustGlint();
}

float CScroll::RetVisibleValue()
{
	return m_visibleValue;
}


void CScroll::SetVisibleRatio(float value)
{
	if ( value < 0.1 )  value = 0.1f;
	if ( value > 1.0 )  value = 1.0f;
	m_visibleRatio = value;
	AdjustGlint();
}

float CScroll::RetVisibleRatio()
{
	return m_visibleRatio;
}


void CScroll::SetArrowStep(float step)
{
	m_step = step;
}

float CScroll::RetArrowStep()
{
	return m_step;
}
