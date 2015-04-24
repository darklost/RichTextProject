#include "ui/rtw_ui.h"
//********************************************************************
//	created:	2010.04.08 10:00
//	filename: 	rtw_text.cpp
//	author:		ldr123
//	purpose:	文本输入框
//  other:		局部重构添加注释
//********************************************************************

RT_IMPLEMENT_DYNAMIC(RtwTextBox, RtwWidget, 0, "ui")

//item内部可分割的个数
//比如文字可以按照每个文字进行分割，而图片则是不可分割的
int RtwTextBox::SItem::getColCount()
{
	int count = 0;
	switch (type)
	{
	case eit_Text:
		count += (int)text.size();
		break;
	case eit_Image:
		count++;
		break;
	default:
		break;
	}

	return count;
}
int	RtwTextBox::SItem::getWidthTrue()
{
	int nWidth = 0;
	switch(type)
	{
	case eit_Text:
	case eit_Name:
		nWidth = g_workspace.GetUiRender()->GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(nFontID), text.c_str()) + 2;
		break;
	case eit_Image:
		nWidth = imgSize.width;
		break;
	case eit_Enter:
		nWidth = 1;
		break;
	default:
		break;
	}

	return nWidth;
}

//获取一项
RtwTextBox::SItem* RtwTextBox::SLine::getItem(int index)
{
	if (index >= (int)lineitems.size())
		return NULL;

	std::list<RtwTextBox::SItem>::iterator iterItem = lineitems.begin();
	for (int i=0; i<index; ++i)
		iterItem++;

	return &(*iterItem);
}

//获取item在行内的索引
int RtwTextBox::SLine::getItemIndex(const SItem* pItem)
{
	int index = 0;
	foreach(std::list<RtwTextBox::SItem>, iterItem, lineitems)
	{
		if (&(*iterItem) == pItem)
			return index;

		index ++;
	}
	return -1;
}

//获取一行内可分割的item的个数总和
int RtwTextBox::SLine::getColCount()
{
	int count = 0;
	foreach(std::list<RtwTextBox::SItem>, iterItem, lineitems)
	{
		RtwTextBox::SItem& item = *iterItem;
		count += item.getColCount();
	}
	return count;
}

//插入一列
RtwTextBox::SItem* RtwTextBox::SLine::InsertItem(RtwTextBox::SItem& item, int index/* = -1*/)
{
	if (index > (int)lineitems.size())
		return NULL;

	int iHeight = item.rect.getHeight();
	int iWidth = 0;

	if (item.imgSize.height == 0)
	{
		iHeight	= g_workspace.GetUiRender()->GetFontHeight(g_workspace.GetUiRender()->GetNativeFontId(item.nFontID));
		iWidth  = g_workspace.GetUiRender()->GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(item.nFontID), item.text.c_str());
	}
	else
	{
		iHeight = item.imgSize.height;
		iWidth  = item.imgSize.width;
	}

	//调整行的高度
	if (height < iHeight)
	{
		height		= iHeight;
		rect.bottom = rect.top + height;
	}

	rect.right = rect.left + rect.getWidth() + iWidth;

	if (index < 0)
	{
		lineitems.push_back(item);
		std::list<RtwTextBox::SItem>::iterator iterItem = lineitems.end();
		return &(*(--iterItem));
	}
	else 
	{
		std::list<RtwTextBox::SItem>::iterator iterItem = lineitems.begin();
		for (int i=0; i<index; ++i)
		{
			iterItem ++;
		}
		lineitems.insert(iterItem, item);
		return &(*(--iterItem));
	}
}

//删除指定的item
bool RtwTextBox::SLine::RemoveItem(int index)
{
	if ((int)lineitems.size() <= index)
		return false;

	std::list<RtwTextBox::SItem>::iterator iterItem = lineitems.begin();
	for (int i=0; i<index; ++i)
	{
		iterItem ++;
	}

	lineitems.erase(iterItem);

	return true;
}

bool RtwTextBox::SLine::RemoveItem(RtwTextBox::SItem* pItem)
{
	foreach (std::list<RtwTextBox::SItem>, iterItem, lineitems)
	{
		if (&(*iterItem) == pItem)
		{
			lineitems.erase(iterItem);
			return true;
		}
	}

	return false;
}

int	RtwTextBox::SLine::getWidthTrue()
{
	int nWidth = 0;
	foreach (std::list<RtwTextBox::SItem>, iterItem, lineitems)
	{
		switch(iterItem->type)
		{
		case eit_Text:
		case eit_Name:
			nWidth += g_workspace.GetUiRender()->GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(iterItem->nFontID), iterItem->text.c_str()) + 2;
			break;
		case eit_Image:
			nWidth += iterItem->imgSize.width;
			break;
		case eit_Enter:
			nWidth += 1;
			break;
		}
	}
	return nWidth;
}

RtwTextBox::CheckValid*	RtwTextBox::m_pChatCheckValidFunc	= NULL;
RtwTextBox::CheckValid*	RtwTextBox::m_pCheckValidFunc		= NULL;
RtwEventDispatcher RtwTextBox::EvGameClickEvent;

RtwTextBox::RtwTextBox()
:m_LeftMargin(0), m_RightMargin(0), m_BottomMargin(0), m_TopMargin(0), m_addface(false), m_bEnableInput(true),
m_bWordWrap(false), m_bMultiLine(false), m_bPassword(false), m_bNumeric(false), m_Capacity(0),m_ViewOffsetLine(0),
m_ViewOffsetCol(0), m_ViewLastLine(0), m_CursorPosLine(0), m_CursorPosCol(0),m_CursorColor(RtwPixel(0xffccaaff)), 
m_CursorPosColBak(0), m_SelStartLine(0), m_SelStartCol(0), m_SelEndLine(0),	m_SelEndCol(0), m_Password_ShowChar('*'), 
m_nLineNum(200), m_bNameColorChange(true), m_bCanPenetrate(true),m_bEnableMenu(false), m_bChatCheckValid(false),
m_bCheckValid(false), m_bMatheMatics(true), m_bIsRoll(false),m_nMaxWidth(0),m_bUseGameClickEvent(false),m_nShowCount(3),m_strDefaultText(""),
m_bShowDefaultText(false)
{
	m_pAutoScrollV		= NULL;
	m_bAutoScrollV		= false;
	m_WidgetType		= wtEditBox;
	m_pRMenu			= NULL;
	m_pRichTextLoader	= RT_NEW CUiRichText();
	m_pHtmlTextLoader	= RT_NEW CUiHtmlText();

	m_Lines.clear();
	memset(m_acDoubleByte, 0, 2);
}

RtwTextBox::~RtwTextBox()
{
	DEL_ONE(m_pRichTextLoader);
	DEL_ONE(m_pHtmlTextLoader);

	if (m_pRMenu)
	{
		g_workspace.RemoveWidget(m_pRMenu);
	}
}

void RtwTextBox::Destroy()
{
	_ResetSelf();

	RtwWidget::Destroy();
}

void RtwTextBox::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwTextBox::SetAutoScrollV(bool bAutoScrollV)
{
	RtwWidget::SetAutoScrollV(bAutoScrollV);
	if (bAutoScrollV && m_pAutoScrollV)
	{
		m_pAutoScrollV->EvScroll += RTW_CALLBACK(this, RtwTextBox, OnScroll_ScrollBarV);
	}
}

void RtwTextBox::SetAutoScrollH(bool bAutoScrollH){}

bool RtwTextBox::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	EvMouseMove	+= RTW_CALLBACK(this, RtwTextBox, OnMouseMove_This);
	EvMouseLDown+= RTW_CALLBACK(this, RtwTextBox, OnMouseLDown_This);
	EvMouseRDown+= RTW_CALLBACK(this, RtwTextBox, OnMouseRDown_This);
	EvKey		+= RTW_CALLBACK(this, RtwTextBox, OnKeyDown_This);
	EvKeyChar	+= RTW_CALLBACK(this, RtwTextBox, OnKeyChar_This);
	EvLClick	+= RTW_CALLBACK(this, RtwTextBox, OnLClick_This);
	EvHyperLink	+= RTW_CALLBACK(this, RtwTextBox, OnEvHyperLink);

	return true;
}

//移动
void RtwTextBox::OnMove(const RtwRect& OrigFrameRect)
{
	RtwWidget::OnMove(OrigFrameRect);

	//每行每列都得移动
	SSize osize(m_rcFrame.left-OrigFrameRect.left, m_rcFrame.top-OrigFrameRect.top);

	foreach (std::list<SLine>, iterLine, m_Lines)
	{
		SLine& line = *iterLine;
		line.rect.Offset(osize);

		foreach (std::list<SItem>, iterItem, line.lineitems)
		{
			SItem& item = *iterItem;
			item.rect.Offset(osize);
		}
	}

	RefreshCursorPosition();
	RefreshScrollBarVData();
}

void RtwTextBox::OnOffset(const SSize& size)
{
	RtwWidget::OnOffset(size);

	m_ViewRect.Offset(size);
	m_CursorRect.Offset(size);

	for (std::list<SLine>::iterator iterLine=m_Lines.begin(); iterLine!=m_Lines.end(); ++iterLine)
	{
		SLine& line = *iterLine;
		line.rect.Offset(size);

		for (std::list<SItem>::iterator iterItem=line.lineitems.begin(); iterItem!=line.lineitems.end(); ++iterItem)
		{
			iterItem->rect.Offset(size);
		}
	}

	RefreshCursorPosition();
	RefreshScrollBarVData();
}

bool RtwTextBox::CloneFrom(RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwTextBox::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwTextBox* pOther = (RtwTextBox*)pWidget;

	m_bEnableInput	= pOther->m_bEnableInput;
	m_bWordWrap		= pOther->m_bWordWrap;
	m_bMultiLine	= pOther->m_bMultiLine;
	m_bAutoScrollV	= pOther->m_bAutoScrollV;

	RemoveAllItems(false);
	m_Lines = pOther->m_Lines;

	m_ViewOffsetLine	= pOther->m_ViewOffsetLine;
	m_ViewLastLine		= pOther->m_ViewLastLine;
	m_CursorPosLine		= pOther->m_CursorPosLine;
	m_CursorPosCol		= pOther->m_CursorPosCol;
	m_CursorPosColBak	= pOther->m_CursorPosColBak;
	m_SelStartLine		= pOther->m_SelStartLine;
	m_SelStartCol		= pOther->m_SelStartCol;
	m_SelEndLine		= pOther->m_SelEndLine;
	m_SelEndCol			= pOther->m_SelEndCol;
	m_SelText			= pOther->m_SelText;
	m_nLineNum			= pOther->m_nLineNum;
	m_bUseGameClickEvent= pOther->m_bUseGameClickEvent;

	m_CursorRect = pOther->m_CursorRect;

	return true;
}

//设置光标到line行col列
void RtwTextBox::SetCursorPos(int line, int col)
{
	SLine* pLine = getLine(line);
	UI_ENSURE(pLine);

	m_CursorPosLine = line;
	if (col < 0)
	{
		m_CursorPosCol = 0;
	}
	else if (col > (int)pLine->getColCount())
	{
		m_CursorPosCol = (int)pLine->getColCount();
	}
	else 
	{
		m_CursorPosCol = col;
	}

	RefreshCursorPosition();
}

void RtwTextBox::ClearText()
{
	RemoveAllItems(false);
	m_ViewOffsetLine	= 0;
	m_ViewLastLine		= 0;
	m_CursorPosLine		= 0;
	m_CursorPosCol		= 0;
	m_CursorPosColBak	= 0;
	m_SelStartLine		= 0;
	m_SelStartCol		= 0;
	m_SelEndLine		= 0;
	m_SelEndCol			= 0;
	m_SelText			= "";
	SetCursorPos(0, 0);
}

void RtwTextBox::SetText(const std::string& text)
{
	if (text.empty())
		m_bShowDefaultText = true;
	std::string checkdString = text;
	CheckValidString(checkdString);

	ClearText();

	// 添加新行
	SLine line;
	line.height = g_workspace.GetUiRender()->GetFontHeight(g_workspace.GetUiRender()->GetNativeFontId(m_nFontID))+2;
	SItem item;
	item.type			= eit_Text;
	item.color			= m_TextColor;
	item.bordercolor	= m_TextBorderColor;
	item.text			= checkdString;
	item.nFontID		= m_nFontID;

	line.lineitems.push_back(item);
	InsertLine(line);

	SetCursorPos(0, (int)checkdString.size());
}

//追加文本
void RtwTextBox::AddText(const std::string &text, RtwPixel color)
{
	std::string checkdString = text;
	CheckValidString(checkdString);

	SLine *pLine = getLine(m_CursorPosLine);
	if (!pLine)
	{
		SetText(text);
	}
	else
	{
		SItem item;
		item.type	= eit_Text;
		item.color	= color;
		item.text	= checkdString;
		pLine->lineitems.push_back(item);
		m_CursorPosCol += (int)checkdString.size();
	}

	CalcLinePosition(m_CursorPosLine);
	AutoAdjectCursor();
}

//添加超链接
void RtwTextBox::AddHyperLinkText(const std::string &text,const std::string &link,  RtwPixel color)
{
	SHyperLink lLink;
	lLink.Text = text;
	lLink.Type = hlinkClient;
	AddHyperLinkText(text, lLink, color);
}

//获取超链接个数
int RtwTextBox::GetHyperLinkNumber()
{
	int nResult = 0;
	SItem* nextItem = NULL;

	for (std::list<SLine>::iterator li=m_Lines.begin(); li!=m_Lines.end(); ++li)
	{
		for (std::list<SItem>::iterator sli=li->lineitems.begin(); sli!=li->lineitems.end(); ++sli)
		{
			if (sli->hyperLink.Type != hlinkChildInvalid)
			{
				if (nextItem && nextItem==&(*sli))
				{
					continue;
				}

				nextItem = sli->nextItem;
				nResult++;
			}
		}
	}

	return nResult;
}

void RtwTextBox::AddHyperLinkText(const std::string &text, const SHyperLink &hyperLink, RtwPixel color)
{
	if(m_Capacity>0 && m_Capacity<=getColCount())
	{
		return;
	}

	SItem item;
	item.type	= eit_Text;
	item.color	= color;
	item.text	= text;
	item.hyperLink = hyperLink;
	SLine *pLine = getLine(m_CursorPosLine);
	if (!pLine)
	{
		SLine line;
		InsertLine(line, -1);
		m_CursorPosCol = 0;
		getLine(m_CursorPosLine)->InsertItem(item);
	}
	else
	{
		SItem* _pItem = PickItem(SPoint(m_CursorRect.left, m_CursorRect.top));
		if (!_pItem || _pItem->type!=eit_Text)
		{
			std::list<SItem>::iterator sli = pLine->lineitems.begin();
			int nTmpIndex = 0;
			while (sli != pLine->lineitems.end())
			{
				if (&(*sli) == _pItem)
				{
					break;
				}

				nTmpIndex++;
				++sli;
			}

			pLine->InsertItem(item, _pItem?nTmpIndex+1:0);
		}
		else
		{
			std::list<SItem>::iterator sli = pLine->lineitems.begin();
			int nTmpIndex = 0;
			int nTmpPos = 0;
			while (sli != pLine->lineitems.end())
			{
				if (&(*sli) == _pItem)
				{
					break;
				}

				nTmpPos++;
				nTmpIndex += sli->getColCount();
				++sli;
			}

			int _InsertIndex = m_CursorPosCol-nTmpIndex;
			int nText = (int)_pItem->text.size();
			if (m_CursorPosCol > nText+nTmpIndex)
			{
				_InsertIndex = nText;
			}

			if(m_Capacity>0 && nText>=m_Capacity)
			{
				return;
			}

			SItem newItem;
			_DivideTextItem(*_pItem, _InsertIndex, newItem);
			pLine->InsertItem(item, ++nTmpPos);
			if (!newItem.text.empty())
			{
				pLine->InsertItem(newItem, ++nTmpPos);
			}
		}
	}

	m_CursorPosCol += (int)text.size();
	CalcLinePosition(m_CursorPosLine);
	RefreshCursorPosition();
	AutoAdjectCursor();
}

//获取文本
std::string RtwTextBox::getText() const 
{
	char szBuf[1024] = "\0";
	std::string str = "";
	for (
		std::list<SLine>::const_iterator iterLine = m_Lines.begin(); 
		iterLine != m_Lines.end(); iterLine++
		)
	{
		const SLine& line = *iterLine;
		for (
			std::list<SItem>::const_iterator iterItem = line.lineitems.begin();
			iterItem != line.lineitems.end(); iterItem++
			)
		{
			const SItem& item = *iterItem;
			if (item.type == eit_Text || item.type == eit_Name||item.type==eit_Image)
			{
				if (item.hyperLink.childType == hlinkInvalid)
				{
					str += item.text;
				}
				else if (item.hyperLink.childType == hlinkTask)
				{
					str += item.text;
				}
				else if (item.hyperLink.childType == hlinkItem)
				{
					if ((int)item.hyperLink.vecParam.size() == 3 && item.lastItem == NULL)
					{
						rt2_format(szBuf, 1024, "item[%llu, %llu, %llu]meti", item.hyperLink.vecParam[0], item.hyperLink.vecParam[1], item.hyperLink.vecParam[2]);
						str += szBuf;
					}
				}
				else
				{
					str += item.text;
				}
			}
			else if (item.type == eit_Enter)
			{
				str += "#[intercept]";
			}
		}
	}
	return str;
}

RtwTextBox::SLine* RtwTextBox::getLine(int index) 
{
	int nLstLine = (int)m_Lines.size();
	if (index < 0 || m_Lines.empty() || index>=nLstLine)
		return NULL;

	list<SLine>::iterator it = m_Lines.begin();
	advance(it, index);

	if (it == m_Lines.end())
		return NULL;

	return &(*it);
}

//绘制前台
void RtwTextBox::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwRect rcAfterClip = m_ViewRect;
	if (pClipRect)
		rcAfterClip.ClipBy(*pClipRect);

	m_bCanPenetrate = true;

	bool bClick , bHover; //item是否有鼠标事件
	std::string strHyperLinkTmp = "";
	DWORD nHyperColor = 0;
	if (m_bShowDefaultText && !m_strDefaultText.empty() && getEnableInput())
	{
		g_workspace.GetUiRender()->DrawText(
			g_workspace.GetUiRender()->GetNativeFontId(m_nFontID), 
			m_strDefaultText.c_str(), 
			m_rcFrame, 
			SETCOLORALPHA(RtwPixel(0xffffffff), m_Transparency),
			SETCOLORALPHA(RtwPixel(0xff000000), m_Transparency),
			wtFontRender_Bottom_Align,
			0,
			&rcAfterClip
			);
	}
	else
	// 内容
	foreach (std::list<SLine>, iterLine, m_Lines)
	{
		SLine& line = *iterLine;
		if (!rcAfterClip.IsIntersect(line.rect))
			continue;

		int nMinLeft =-1;
		foreach (std::list<SItem>, iterItem, line.lineitems)
		{
			bClick = false;
			bHover = false;

			SItem& item = *iterItem;

			RtwRect tmpClipRect = rcAfterClip;
			if (!item.bIsRoll)
			{
				nMinLeft = item.rect.right + 2;
			}
			else if (item.bIsRoll && nMinLeft!=-1)
			{
				if (nMinLeft> item.rcFrame.left)
				{
					nMinLeft= item.rcFrame.left;
				}
				tmpClipRect.left = nMinLeft;
			}

			if (!rcAfterClip.IsIntersect(item.rect))
				continue;
			RtwRect rcRender = item.rect;
			SPoint curPoint = g_workspace.getMousePos();

			if (g_workspace.getMouseCapture() == this)
			{
				if (PickItem(curPoint) == &item)
				{
					m_bCanPenetrate = false;
					bClick = true;
				}
			}
			else if (g_workspace.getMouseHover() == this)
			{
				if (PickItem(curPoint) == &item)
				{
					m_bCanPenetrate = false;
					bHover = true;
				}
			}

			wtFontRenderOption fro = wtFontRender_Bottom_Align;

			if (item.bUnderLine)
				fro |= wtFontRender_Underline;

			switch (item.type)
			{
			case eit_Name:
			case eit_Text:
				{
					//文本
					RtwPixel txtColor = item.color;
					RtwPixel txtBoderColor = item.bordercolor;
					//密码形式显示
					if (!m_bPassword)
					{
						//支持链接颜色
						if (!item.hyperLink.Text.empty())
						{
							if (item.hyperLink.normalColor != -1)
							{
								txtColor = item.hyperLink.normalColor;
							}

							if (bHover)
							{
								item.hyperLink.cState= 1;
								if (item.hyperLink.hilghtColor != -1)
								{
									txtColor = item.hyperLink.hilghtColor;
								}
								else
								{
									txtColor = g_workspace.getHyperLinkDefaultStyle().HighlightColor;
								}

								nHyperColor = txtColor;
							}
							else if (bClick)
							{
								item.hyperLink.cState= 2;
								if (item.hyperLink.pushedColor != -1)
								{
									txtColor = item.hyperLink.pushedColor;
								}

								nHyperColor = txtColor;
							}
							else
							{
								// 判断同一超链其它地方是不是处于按下或者悬浮状态
								item.hyperLink.cState= 0;
								if (item.lastItem)
								{
									if(item.lastItem->hyperLink.cState== 1)
									{
										if (item.lastItem->hyperLink.hilghtColor != -1)
										{
											txtColor = item.lastItem->hyperLink.hilghtColor;
										}
										else
										{
											txtColor = g_workspace.getHyperLinkDefaultStyle().HighlightColor;
										}
									}else if(item.lastItem->hyperLink.cState== 2)
									{
										if (item.lastItem->hyperLink.pushedColor != -1)
										{
											txtColor = item.lastItem->hyperLink.pushedColor;
										}
									}
								}
								if (item.nextItem)
								{
									if(item.nextItem->hyperLink.cState== 1)
									{
										if (item.nextItem->hyperLink.hilghtColor != -1)
										{
											txtColor = item.nextItem->hyperLink.hilghtColor;
										}
										else
										{
											txtColor = g_workspace.getHyperLinkDefaultStyle().HighlightColor;
										}
									}else if(item.nextItem->hyperLink.cState== 2)
									{
										if (item.nextItem->hyperLink.pushedColor != -1)
										{
											txtColor = item.nextItem->hyperLink.pushedColor;
										}
									}
								}
							}
						}

						if (item.type == eit_Name)
						{
							//名字变色
							if (bHover && m_bNameColorChange)
							{
								txtColor = 0xffff0000;
							}
						}

						g_workspace.GetUiRender()->DrawText(
							g_workspace.GetUiRender()->GetNativeFontId(item.nFontID), 
							item.text.c_str(), 
							rcRender, 
							SETCOLORALPHA(txtColor, m_Transparency),
							SETCOLORALPHA(txtBoderColor, m_Transparency),
							fro,
							0,
							&tmpClipRect
							);
					}
					else 
					{
						std::string strPass;
						for (int i = 0; i < (int)item.text.size(); ++i)
							strPass.push_back(m_Password_ShowChar);

						g_workspace.GetUiRender()->DrawText(
							g_workspace.GetUiRender()->GetNativeFontId(item.nFontID), 
							strPass.c_str(), 
							rcRender, 
							SETCOLORALPHA(txtColor, m_Transparency),
							SETCOLORALPHA(txtBoderColor, m_Transparency),
							fro,
							0,
							&tmpClipRect
							);
					}

					if (item.bDraw)
					{
						RtwRect selectrect = item.rect;
						selectrect.left		+= 1;
						selectrect.top		+= 1;
						selectrect.bottom	-= 1;
						g_workspace.GetUiRender()->FillRectangle(selectrect, 0xb24060f0);
					}

					RtwRect _rcSelect = item.rcSelect;
					_rcSelect.ClipBy(rcAfterClip);
					if (_rcSelect.isValid())
					{
						g_workspace.GetUiRender()->FillRectangle(_rcSelect, RtwPixel(0x500000ff));
					}
				}
				break;
			case eit_Image:
				{
					//图片
					rcRender.bottom = rcRender.top + item.imgSize.height;
					rcRender.right  = rcRender.left+ item.imgSize.width;

					RtwImagePtr pCurrentImage = item.image;
					if(pCurrentImage)
						g_workspace.GetUiRender()->DrawImage(pCurrentImage, rcRender, 0, m_Transparency, &tmpClipRect);
				}
				break;
			default:
				break;
			}
		}
	}

	RtwWidget::DrawForeground(pClipRect);

	// 画光标位置 @@@@@
	if (g_workspace.getFocusWidget() == this && getEnableInput())
	{
		if ( (rtMilliseconds() / 500) & 1 )
		{
			if (m_CursorRect.top >= rcAfterClip.top && m_CursorRect.bottom <= rcAfterClip.bottom)
				g_workspace.GetUiRender()->FillRectangle(m_CursorRect, SETCOLORALPHA(m_CursorColor, m_Transparency));
		}
	}
}

//绘制背景
void RtwTextBox::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

//计算矩形
void RtwTextBox::CalcClient()
{
	m_rcClient = m_rcFrame;
	//自动纵向滚动条
	if (m_bAutoScrollV)
	{
		m_rcClient.right -= g_workspace.getDefaultScrollVWidth() + 2;
	}

	m_ViewRect.left		= m_rcClient.left	+ 1;
	m_ViewRect.right	= m_rcClient.right	- 1;
	m_ViewRect.top		= m_rcClient.top	+ 1;
	m_ViewRect.bottom	= m_rcClient.bottom - 1;

	if (m_pAutoScrollV)
	{
		m_pAutoScrollV->Move(RtwRect(m_rcFrame.right-g_workspace.getDefaultScrollVWidth(), m_rcFrame.top, m_rcFrame.right, m_rcFrame.bottom));
		m_pAutoScrollV->Offset(SSize(-m_rcFrame.left, -m_rcFrame.top));
	}
}

//光标自适应位置
void RtwTextBox::AutoAdjectCursor()
{
	if (m_bAutoSizeV || m_bAutoSizeH)
		return ;

	while (!m_ViewRect.IsContain(m_CursorRect))
	{
		bool bProcessed = false;
		if (m_ViewRect.left >= m_CursorRect.right)
		{
			if( false == m_bWordWrap )
			{
				OffsetItemPosition(SSize(48, 0), 0);
				bProcessed = true;
			}else
			{
				m_CursorPosLine -= 1;
				m_CursorPosLine  = m_CursorPosLine < 0 ? 0 : m_CursorPosLine;
				m_CursorPosCol   = 0;
				RefreshCursorPosition();			
			}
		}
		else if (m_ViewRect.right <= m_CursorRect.left)
		{
			if( false == m_bWordWrap )
			{
				OffsetItemPosition(SSize(-48, 0), 0);
				bProcessed = true;
			}else
			{
				m_CursorPosLine += 1;
				SLine* pLine     = getLine( m_CursorPosLine );
				if( NULL == pLine )
				{
					SLine line;
					InsertLine(line, m_CursorPosLine );
				}
				m_CursorPosCol   = 0;
				RefreshCursorPosition();
				ScrollToBottom( );
			}
		}
		if (!bProcessed)
			break;
	}
}

bool RtwTextBox::_DivideTextItem(SItem& ItemToDivide, int DivideIndexFrom, SItem& outNewItem)
{
	//分割字符串
	UI_ENSURE_B(ItemToDivide.type == eit_Text);
	UI_ENSURE_B(DivideIndexFrom>=0 && DivideIndexFrom<(int)ItemToDivide.text.size());

	outNewItem			= ItemToDivide;
	outNewItem.text		= outNewItem.text.substr(DivideIndexFrom);
	ItemToDivide.text	= ItemToDivide.text.substr(0, DivideIndexFrom);

	return true;
}

//合并临近的item
bool RtwTextBox::_MergeNeighborItem(SItem* pItem1, SItem* pItem2)
{
	if (!pItem1 || !pItem2 || pItem1->type!=eit_Text || pItem2->type!=eit_Text || pItem1->color!=pItem2->color)
		return false;

	pItem1->text += pItem2->text;
	return true;
}

//合并两个item
bool RtwTextBox::_MergeItem(SItem* pItem1, SItem* pItem2)
{
	if (!pItem1 || !pItem2 || pItem1->type != eit_Text || pItem2->type != eit_Text)
		return false;

	pItem1->text += pItem2->text;
	return true;
}

//当前行和下一行合并
bool RtwTextBox::_MergeWrapedLine(int line)
{
	//得到行
	SLine* pLine = getLine(line);

	int MergeLineIndex = line + 1;
	SLine* pNextLine = getLine(MergeLineIndex);

	if (
		!pLine || pLine->lineitems.empty()		//存在此行,且内容不是空
		|| !pNextLine || !pNextLine->bWordWrapTail //存在下一行,
		)
	{
		return false;
	}

	SItem* pItem = &(pLine->lineitems.back());	//第一行的最后一个item
	while (pNextLine && pNextLine->bWordWrapTail)
	{
		if (pNextLine->lineitems.empty())
			return false;

		SItem* _pItem = &(pNextLine->lineitems.front());	//下一行的第一个item

		UI_CHECK_B(_pItem->type == eit_Text);
		UI_CHECK_B(_MergeItem(pItem, _pItem));

		//依次合并
		pNextLine->lineitems.pop_front();
		foreach(std::list<SItem>, iterItem, pNextLine->lineitems)
		{
			_pItem = &(*iterItem);
			pLine->lineitems.push_back(*_pItem);
		}

		//合并完了就删掉
		UI_CHECK_B(RemoveLine(MergeLineIndex));
		pNextLine = getLine(MergeLineIndex);
	}
	return true;
}

bool RtwTextBox::RemoveFristLine()
{
	if (m_Lines.empty())
		return false;

	int height = m_Lines.front().rect.getHeight();
	m_Lines.pop_front();

	OffsetItemPosition(SSize(0, height + m_LineSpace), 0);

	return true;
}

bool RtwTextBox::RemoveLine(int line,bool bDown /*= true*/)
{
	if (line < 0 || m_Lines.empty())
		return false;

	list<SLine>::iterator it = m_Lines.begin();
	advance(it, line);

	if (it == m_Lines.end())
		return false;

	int height = it->rect.getHeight();
	m_Lines.erase(it);

	if (bDown)
	{
		OffsetItemPosition(SSize(0, height + m_LineSpace), line);
	}
	else
	{
		OffsetItemPosition(SSize(0, -height - m_LineSpace), line);
	}

	return true;
}

//删除所以行
bool RtwTextBox::RemoveAllLines()
{
	return RemoveAllItems();
}

//删除所有item
bool RtwTextBox::RemoveAllItems(bool bRefreshItemPos/* = true*/)
{
	while (m_Lines.size())
	{
		UI_ENSURE_B(RemoveLine(0));
	}

	Refresh();

	return true;
}

//根据\r\n分割字符串
void ParseString(const string &str, vector<string> &vecOut)
{
	vecOut.clear();

	string tmp = "";
	int nPos = 0;
	while (nPos < (int)str.size())
	{
		if (str[nPos] == 13) //\r
		{
			if (nPos < (int)str.size()-1)
			{
				if (str[nPos+1] == 10) //\n
				{
					nPos += 2;
					if (!tmp.empty())
					{
						vecOut.push_back(tmp);
						tmp = "";

						continue;
					}
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			tmp += str[nPos];
		}

		nPos++;
	}

	if (!tmp.empty())
	{
		vecOut.push_back(tmp);
	}
}

//在光标位置插入字符串
bool RtwTextBox::InsertStringToCurrentCursor(const std::string& str, bool bEnter)
{
	bool bProcessed = false;

	if (m_Lines.empty())
	{
		// 如果还没有行，就添加一个
		SLine line;
		InsertLine(line, -1);
	}

	if(m_Capacity>0 && m_Capacity<=getColCount())
	{
		return false;
	}
	//end

	vector<std::string> vecString; //按照\r\n分割的string
	ParseString(str, vecString);

	for (vector<string>::iterator vi=vecString.begin(); vi!=vecString.end(); ++vi)
	{
		string strTmp = *vi;

		SLine* pLine = getLine(m_CursorPosLine);
		if (!pLine)
		{
			SLine lineTmp;
			InsertLine(lineTmp, m_CursorPosLine-1);
			pLine = getLine(m_CursorPosLine);
		}

		//行内没有item
		if (pLine && pLine->lineitems.empty())
		{
			//默认字体
			SItem item;
			item.text	= strTmp;
			item.type	= eit_Text;
			item.color	= m_TextColor;
			item.nFontID= m_nFontID;
			pLine->InsertItem(item, 0);
			bProcessed = true;
		}
		else 
		{	
			SLine *pLine = getLine(m_CursorPosLine);
			if (!pLine)
			{
				return false;
			}

			SItem* _pItem = PickItem(SPoint(m_CursorRect.left, m_CursorRect.top));
			if (!_pItem || _pItem->type!=eit_Text || _pItem->hyperLink.bIsLink())
			{
				SItem pItem;
				pItem.type=eit_Text;
				pItem.text = strTmp;
				pItem.nFontID= m_nFontID;

				if (!_pItem)
				{
					pLine->InsertItem(pItem, 0);
				}
				else
				{
					std::list<SItem>::iterator sli = pLine->lineitems.begin();
					int nTmpIndex = 0;
					while (sli != pLine->lineitems.end())
					{
						if (&(*sli) == _pItem)
						{
							break;
						}

						nTmpIndex++;
						++sli;
					}

					if (m_CursorPosCol == 0)
					{
						nTmpIndex = -1;
					}

					pLine->InsertItem(pItem, nTmpIndex+1);
				}
			}
			else
			{
				std::list<SItem>::iterator sli = pLine->lineitems.begin();
				int nTmpIndex = 0;
				while (sli != pLine->lineitems.end())
				{
					if (&(*sli) == _pItem)
					{
						break;
					}

					nTmpIndex += sli->getColCount();
					++sli;
				}

				int _InsertIndex = m_CursorPosCol-nTmpIndex;
				int nText = (int)_pItem->text.size();
				if (m_CursorPosCol > nText+nTmpIndex)
				{
					_InsertIndex = nText;
				}

				if(m_Capacity>0 && nText>=m_Capacity)
				{
					return false;
				}

				size_t nSize = strTmp.size();
				for (size_t i=0; i<nSize; ++i)
				{
					_pItem->text.insert(_InsertIndex+i, 1, strTmp[i]);
				}

			}

			bProcessed = true;
		}

		if (bEnter)
			m_CursorPosCol = (int)strTmp.size();
		else
			m_CursorPosCol += (int)strTmp.size();

		if (bProcessed)
		{
			CalcLinePosition(m_CursorPosLine);
			if (bEnter) _Intercept_Enter(m_CursorPosLine, m_CursorPosCol);
			RefreshCursorPosition();
			AutoAdjectCursor();
		}
	}

	return bProcessed;
}

bool RtwTextBox::InsertCharacterToCurrentCursor(const char character)
{
	static bool bChinese = false;

	bool bProcessed = false;

	if (m_Lines.empty())
	{
		// 如果还没有行，就添加一个
		SLine line;
		InsertLine(line, -1);
	}

	//功能键排除
	//begin
	if (character == vkBack)
		return false;

	if (character == vkEscape)
		return false;

	else if (character == vkEnter)
		return false;

	int nTextSize = getColCount();

	//gb2312编码
	if((character&0xff) >= 0xa1)
	{
		if (!bChinese)
		{
			if( m_Capacity>0 && nTextSize+2 > m_Capacity)
			{
				return false;
			}

			bChinese = true;
		}
		else
		{
			bChinese = false;
		}
	}
	else
	{
		if( m_Capacity>0 && nTextSize+1 > m_Capacity)
		{
			return false;
		}
	}


	SLine* pLine = getLine(m_CursorPosLine);
	UI_ENSURE_B(pLine);

	//行内没有item
	if (pLine->lineitems.empty())
	{
		//默认字体
		SItem item;
		item.text	= character;
		item.type	= eit_Text;
		item.color	= m_TextColor;
		item.nFontID= m_nFontID;
		pLine->InsertItem(item, 0);
		bProcessed = true;
	}
	else 
	{	
		SLine *pLine = getLine(m_CursorPosLine);
		if (!pLine)
		{
			return false;
		}

		SItem* _pItem = PickItem(SPoint(m_CursorRect.left, m_CursorRect.top));
		if (!_pItem || _pItem->type!=eit_Text || _pItem->hyperLink.bIsLink())
		{
			SItem pItem;
			pItem.type=eit_Text;
			pItem.nFontID= m_nFontID;
			pItem.text.insert(0, 1, character);
			if (!_pItem)
			{
				pLine->InsertItem(pItem, 0);
			}
			else
			{
				std::list<SItem>::iterator sli = pLine->lineitems.begin();
				int nTmpIndex = 0;
				while (sli != pLine->lineitems.end())
				{
					if (&(*sli) == _pItem)
					{
						break;
					}

					nTmpIndex++;
					++sli;
				}

				if (m_CursorPosCol == 0)
				{
					nTmpIndex = -1;
				}

				pLine->InsertItem(pItem, nTmpIndex+1);
			}
		}
		else
		{
			std::list<SItem>::iterator sli = pLine->lineitems.begin();
			int nTmpIndex = 0;
			while (sli != pLine->lineitems.end())
			{
				if (&(*sli) == _pItem)
				{
					break;
				}

				nTmpIndex += sli->getColCount();
				++sli;
			}

			int _InsertIndex = m_CursorPosCol-nTmpIndex;
			int nText = (int)_pItem->text.size();
			if (m_CursorPosCol > nText+nTmpIndex)
			{
				_InsertIndex = nText;
			}

			if(m_Capacity>0 && nText>=m_Capacity)
			{
				return false;
			}

			_pItem->text.insert(_InsertIndex, 1, character);
		}

		bProcessed = true;
	}

	if (bProcessed)
	{
		m_CursorPosCol++;
	}

	return bProcessed;
}

bool RtwTextBox::InsertItemToCurrentCursor(SItem& item)
{
	return false;
}

RtwTextBox::SItem* RtwTextBox::GetLinkItemByText(const char* szText)
{
	std::list<SLine>::iterator iterLine = m_Lines.begin();
	for (; iterLine != m_Lines.end(); ++iterLine)
	{
		SLine& line = *iterLine;

		foreach (std::list<SItem>, iterItem, line.lineitems)
		{
			SItem& item = *iterItem;

			if (item.type == eit_Text)
			{
				if (item.text.find(szText) != string::npos && !item.hyperLink.Text.empty())
				{
					return &item;
				}
			}
		}
	}

	return NULL;
}

//捡取item
bool RtwTextBox::PickItem(const SPoint& point, int& outLine, int& outCol, int& outItemIndex, bool& bDoubleByte, bool& outNear, bool& bhlink)
{
	outLine = 0;
	outCol = 0;
	outItemIndex = 0;
	std::list<SLine>::iterator iterLine = m_Lines.begin();
	for (; iterLine != m_Lines.end(); ++iterLine)
	{
		SLine& line = *iterLine;
		int j = 0;
		outCol = 0;

		// 根据高度判断
		if ( (point.y >= line.rect.top) && (point.y <= line.rect.bottom) )
		{
			foreach (std::list<SItem>, iterItem, line.lineitems)
			{
				SItem& item = *iterItem;
				if (item.rect.IsContain(point))
				{
					switch (item.type)
					{
					case eit_Name:
					case eit_Text:
						{
							// 							int iIndex = 0;
							bool bNear = true;

							wtFontTextRect rcFontText;
							rcFontText.x = item.rect.left;
							rcFontText.y = item.rect.top;
							rcFontText.z = item.rect.right;
							rcFontText.w = item.rect.bottom;

							wtFontPickPos pickPos;
							int result = g_workspace.GetUiRender()->PickText(
								g_workspace.GetUiRender()->GetNativeFontId(item.nFontID),
								item.text.c_str(), 
								rcFontText, 
								wtFontRender_Default,
								point.x,
								point.y,
								&pickPos
								);
							if (result != wtPickIndex_Invalid)
							{
								outItemIndex	=  j;
								if (item.hyperLink.bIsLink())
								{
									if (result >= (float)item.text.size()/2)
									{
										outCol			+= item.text.size();
										bNear			= false;
									}
									else
									{
										bNear			= true;
									}

									bhlink			= true;
								}
								else
								{
									if (result != wtPickIndex_TxBegin)
									{
										outCol += result;
										if (item.text.size()>result+1 && (item.text.substr(result, 1)[0] & 0x80))
										{	
											outCol++;
										}
										outCol++;
									}
									else
									{
										outCol = 0;
									}

									bhlink			= false;
								}
							}
						}
						return true;
					case eit_Image:
						outItemIndex = j;
						if (item.rect.getCenter().x > point.x)
							outNear = true;
						else
							outNear = false;
						return true;
					default:
						break;
					}
				}
				else 
				{
					RtwRect rcTmp = item.rect;
					rcTmp.left = 0;
					if (rcTmp.IsContain(point))
					{
						outNear = true;
						outCol	= 0;
						return true;
					}

					outCol += item.getColCount();
				}
				j++;
			}

			return true;
		}

		outLine ++;
	}

	return false;
}

//返回捡取到得item
RtwTextBox::SItem* RtwTextBox::PickItem(const SPoint& point)
{
	for (std::list<SLine>::iterator iterLine = m_Lines.begin(); iterLine != m_Lines.end(); ++iterLine)
	{
		SLine& line = *iterLine;

		// 根据高度判断
		if ( (point.y >= line.rect.top) && (point.y <= line.rect.bottom) )
		{
			foreach (std::list<SItem>, iterItem, line.lineitems)
			{
				SItem& item = *iterItem;
				if (item.rect.IsContain(point))
				{
					return &item;
				}
			}
			return NULL;
		}
	}

	return NULL;
}

//mousemove
void RtwTextBox::OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (!getEnableInput())
		return;

	if (g_workspace.getMouseCapture() != this)
	{
		return;
	}

	int line, col, itemIndex;
	bool bDoubleByte;
	bool bNear;
	bool bHlink= false;

	if (!m_Lines.empty())
	{
		//捡取item
		if (PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y), line, col, itemIndex, bDoubleByte, bNear, bHlink))
		{
			// 光标
			m_CursorPosLine	= line;
			m_CursorPosCol	= col;
		}
		// 重置选中字符
		_SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
		RefreshSelectionRect();
	}
	RefreshCursorPosition();
}

//点击事件
void RtwTextBox::OnLClick_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	// 处理超链接
	SItem* pItem = PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y));
	if (pItem && pItem->type == eit_Text && pItem->hyperLink.Type != hlinkInvalid)
	{
		if (m_bUseGameClickEvent)
		{
			RtwTextBox::EvGameClickEvent(pWidget, *pEvent);
		}
	}
}

void RtwTextBox::OnEvHyperLink(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
}

void RtwTextBox::OnRMenu(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwPopupMenu* pMenu = pWidget->ConverTo<RtwPopupMenu>();

	assert(pMenu);

	int iSelectItem = pMenu->GetLastSelectItem();
	switch (iSelectItem)
	{
	case 0:	//复制
		_OnCopy();
		break;
	case 1:	//剪切
		_OnCut();
		break;
	case 2:	//粘贴
		_OnPaste();
		break;
	case 3: //全选
		_OnSelectAll();
		break;
	default:
		break;
	}

	if (iSelectItem!=3)
	{
		_SetSelectionStart(0, 0);
		_SetSelectionEnd(0, 0);
		RefreshSelectionRect();
	}
}

void RtwTextBox::OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	_SetSelectionStart(0, 0);
	_SetSelectionEnd(0, 0);

	int line, col, itemIndex;
	bool bDoubleByte;
	bool bNear;
	bool bHlink= false;

	if (!m_Lines.empty())
	{
		//捡取item
		if (PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y), line, col, itemIndex, bDoubleByte, bNear, bHlink))
		{
			// 光标
			m_CursorPosLine	= line;
			m_CursorPosCol	= col;
		}
		else 
		{
			//没捡取到,默认位置
			int textsize  = 0;
			int imagesize = 0;

			m_CursorPosLine		= (int)m_Lines.size() - 1;
			SLine* pLastLine	= getLastLine();

			if (pLastLine && !pLastLine->lineitems.empty())
			{
				foreach(std::list<SItem>, iterItem, pLastLine->lineitems)
				{
					SItem &item = *iterItem;
					if (item.type==eit_Text)
					{
						textsize+=(int)item.text.size();
					}else if(item.type==eit_Image)
					{
						imagesize++;
					}

				}
				m_CursorPosCol = textsize+imagesize;
			}
			else 
			{
				m_CursorPosCol = 0;
			}
		}
		// 重置选中字符
		_SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
		_SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
	}
	RefreshSelectionRect();
	RefreshCursorPosition();
}

void RtwTextBox::OnMouseRDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (!m_bEnableMenu)
	{
		return;
	}

	int SelStartLine, SelEndLine, SelStartCol, SelEndCol;
	_getSelectionStartEnd(SelStartLine, SelStartCol, SelEndLine, SelEndCol);
	bool bSelected = (SelStartCol!=SelEndCol || SelStartLine!=SelEndLine);

	if (m_pRMenu == NULL)
	{
		m_pRMenu = g_workspace.GetWidgetFactory()->CreateWidget<RtwPopupMenu>();
		m_pRMenu->SetBackground(0xD800272D);
		m_pRMenu->SetBorderColor(0xD822272D);
		m_pRMenu->EvMenuSelect += RTW_CALLBACK(this, RtwTextBox, OnRMenu);
		m_pRMenu->EvUnFocus += RTW_CALLBACK(this, RtwTextBox, OnHideMenu);
		m_pRMenu->Hide();
		m_pRMenu->UpdateOriginalFlags();
		g_workspace.AddWidget(m_pRMenu);
	}

	m_pRMenu->RemoveAllItems();
	m_pRMenu->AddMenuItem("复制", !m_bPassword&&m_bEnableInput&&bSelected);
	m_pRMenu->AddMenuItem("剪切", !m_bPassword&&m_bEnableInput&&bSelected);
	m_pRMenu->AddMenuItem("粘贴", !m_bPassword&&m_bEnableInput);
	m_pRMenu->AddMenuItem("全选", !m_bPassword&&m_bEnableInput);

	g_workspace.ShowPopupMenu(m_pRMenu, NULL, g_workspace.GetMousePosX(), g_workspace.GetMousePosY());
}

void RtwTextBox::OnHideMenu(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
}

void RtwTextBox::MergeAllLine()
{
	int nLine = (int)m_Lines.size();
	SLine newLine;
	SItem *pTmpTextItem = NULL;
	for (int i=0; i<nLine; ++i)
	{
		SLine *pTmpLine = getLine(i);
		if (!pTmpLine)
		{
			continue;
		}

		for (std::list<RtwTextBox::SItem>::iterator srsi=pTmpLine->lineitems.begin(); srsi!=pTmpLine->lineitems.end(); ++srsi)
		{
			if (srsi->type == eit_Text)
			{
				// 如果纯文本，直接文字组合
				// 超链也是直接文字组合，前提是两个item是因为换行分开的
				if ((!pTmpTextItem||srsi->hyperLink.Type!=hlinkInvalid) || (pTmpTextItem && pTmpTextItem->hyperLink.Type!=hlinkInvalid))
				{
					if (srsi->nextItem && !srsi->lastItem && srsi->hyperLink.childType == hlinkItem)
					{
						// 开始合并道具超链
						newLine.lineitems.push_back(*srsi);
						pTmpTextItem = &(newLine.lineitems.back());
					}
					else if (srsi->lastItem && srsi->hyperLink.childType == hlinkItem)
					{
						// 同属性道具超链，只合并文字
						pTmpTextItem->text += srsi->text;
					}
					else
					{
						newLine.lineitems.push_back(*srsi);
						pTmpTextItem = &(newLine.lineitems.back());
					}
				}
				else
				{
					pTmpTextItem->text += srsi->text;
				}

				continue;
			}

			newLine.lineitems.push_back(*srsi);
			pTmpTextItem = NULL;
		}
	}

	m_Lines.clear();
	InsertLine(newLine);
}


void RtwTextBox::_OnCopy()
{
	if (g_workspace.isKeyPressed(vkCtrl) && g_workspace.getClipboard() && !m_bPassword)
	{
		g_workspace.getClipboard()->Copy(getSelectText());
	}
}

void RtwTextBox::_OnCut()
{
	if (g_workspace.getClipboard() && !m_bPassword && getEnableInput())
	{
		g_workspace.getClipboard()->Copy(getSelectText());

		int oldCursorLine = m_CursorPosLine;
		int oldCursorCol  = m_CursorPosCol;

		RemoveSelect();
		MergeAllLine();
		SetCursorPos(oldCursorLine, oldCursorCol);
		AutoAdjectCursor();
	}
}

void RtwTextBox::_OnPaste()
{
	if (g_workspace.getClipboard() && getEnableInput())
	{
		std::string strClipboard;
		g_workspace.getClipboard()->Paste(strClipboard);
		if (!strClipboard.empty())
		{
			//不能输入数字?
			if (m_bNumeric) //只允许数字
			{
				if (!m_bMatheMatics && strClipboard.find("-") != string::npos)
				{
					return; //不允许输入负数
				}

				for (int i=0; i<(int)strClipboard.size(); ++i)
				{
					char c = strClipboard[i]; //是数字就不能有汉字
					if((c<48 || c>57))
					{
						return;
					}
				}
			}

			RemoveSelect();

			std::replace(strClipboard.begin(), strClipboard.end(), '\r', '\n');
			std::replace(strClipboard.begin(), strClipboard.end(), '\n', ' ');
			int nNeed = 0;
			if (m_Capacity > 0)
			{
				nNeed = m_Capacity-getColCount();
			}
			else 
			{
				nNeed = (int)strClipboard.size();
			}

			nNeed = min(nNeed, (int)strClipboard.size());
			if (nNeed <= 0)
			{
				return;
			}

			int nChar = 0;
			for (int i=0; i<nNeed; ++i)
			{
				if (strClipboard[i] & 0x80)
				{
					if (i==nNeed-1)
					{
						break;
					}
					else
					{
						nChar++;
						i++;
					}
				}

				nChar++;
			}

			if (nChar <= 0)
			{
				return;
			}
			std::string  strNew = strClipboard.substr(0, nChar);

			InsertStringToCurrentCursor(strNew, false);
			int oldCursorLine = m_CursorPosLine;
			int oldCursorCol  = m_CursorPosCol;
			MergeAllLine();
			m_CursorPosCol = oldCursorCol;
			m_CursorPosLine = oldCursorLine;

			SLine *pLine = getLine(m_CursorPosLine);
			if (pLine)
			{
				int lineCol = pLine->getColCount();
				while (m_CursorPosCol > lineCol)
				{
					m_CursorPosCol = m_CursorPosCol-lineCol;
					m_CursorPosLine++;

					SLine *pLine = getLine(m_CursorPosLine);
					if (!pLine)
					{
						break;
					}

					lineCol = pLine->getColCount();
				}
			}

			RefreshCursorPosition();
			AutoAdjectCursor();
		}
		return;
	}
}

void RtwTextBox::_OnSelectAll()
{
	if (!m_bPassword)
	{
		int nLineNum = getLineCount()-1;
		SLine *pLine = getLine(nLineNum);
		if (!pLine)
		{
			return;
		}
		_SetSelectionStart(0, 0);
		_SetSelectionEnd(nLineNum, pLine->getColCount());
		RefreshSelectionRect();
	}
}

//键盘按下
void RtwTextBox::OnKeyDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (!getEnableInput())
		return;

	pEvent->cancle = false;
	switch (pEvent->key.code)
	{
	case vkEnter:
		{
			if (!getMultiLine())
			{
				return;
			}

			bool bKeyCtrl = (GetKeyState(VK_CONTROL)&0xFF00)!=0;

			if (!bKeyCtrl)
			{
				// 				//换行
				// 				_Intercept(m_CursorPosLine, m_CursorPosCol);
				return;
			}

			EvUpdateText(this, weUpdateText);
		}
		break;
	case vkTab:
		return;
	case vkLeft:
		{
			//移动
			if (m_CursorPosCol > 0)
			{
				SItem* pItemIn = NULL;
				SItem* pItemBetweenLeft = NULL;
				SItem* pItemBetweenRight = NULL;
				int nItemIndex = 0;
				if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex, &pItemBetweenLeft, &pItemBetweenRight))
				{
					if (pItemIn)
					{
						if (pItemIn->type == eit_Text)
						{
							// 判断前一个字符是不是双字节字符
							if (nItemIndex <= 1)
								m_CursorPosCol -= 1;
							else 
							{
								char p = pItemIn->text[nItemIndex-2];
								if (p & 0x80)
								{
									char q = pItemIn->text[nItemIndex-1];
									if (q & 0x80)
										m_CursorPosCol -= 2;
									else 
										m_CursorPosCol -= 1;
								}
								else 
								{
									m_CursorPosCol -= 1;
								}
							}
						}
						else 
						{
							m_CursorPosCol -= 1;
						}
					}
					else if (pItemBetweenLeft)
					{
						// 前后响应可能还有问题
						if (pItemBetweenLeft->hyperLink.bIsLink())
						{
							int size= pItemBetweenLeft->getWidth();
							if (pItemBetweenLeft->lastItem)
							{
								size+= pItemBetweenLeft->lastItem->getWidth();
							}

							m_CursorPosCol		-= size/GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(pItemBetweenLeft->nFontID), "a", m_bPassword);
							m_CursorRect.left	-= size;
							m_CursorRect.right	-= size;
						}else if (pItemBetweenLeft->type == eit_Text)
						{
							// 判断前一个字符是不是双字节字符
							int strsize = (int)pItemBetweenLeft->text.size();
							if (strsize <= 1)
								m_CursorPosCol -= 1;
							else 
							{
								char p = pItemBetweenLeft->text[strsize-2];
								if (p & 0x80)
								{
									char q = pItemBetweenLeft->text[strsize-1];
									if (q & 0x80)
										m_CursorPosCol -= 2;
									else 
										m_CursorPosCol -= 1;
								}
								else 
								{
									m_CursorPosCol -= 1;
								}
							}
						}
						else
						{
							m_CursorPosCol		-= 1;
						}
					}
				}
				if (m_CursorPosCol < 0)
					m_CursorPosCol = 0;

				// 重置选中字符
				_SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
				_SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
				RefreshSelectionRect();
				RefreshCursorPosition();
				AutoAdjectCursor();
			}
		}
		break;
	case vkRight:
		{
			SItem* pItemIn = NULL;
			int nItemIndex = 0;
			SItem* pItemBetweenLeft = NULL;
			SItem* pItemBetweenRight = NULL;
			if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex, &pItemBetweenLeft, &pItemBetweenRight))
			{
				if (pItemIn)
				{
					if (pItemIn->type == eit_Text)
					{
						char p = pItemIn->text[nItemIndex];
						if (p & 0x80)
							m_CursorPosCol += 2;
						else 
							m_CursorPosCol += 1;
					}
					else 
					{
						m_CursorPosCol		+=	1;
					}
				}
				else if (pItemBetweenRight)
				{
					// 前后响应可能还有问题
					if (pItemBetweenRight->hyperLink.bIsLink())
					{
						int size= pItemBetweenRight->getWidth();
						if (pItemBetweenRight->lastItem)
						{
							size+= pItemBetweenRight->lastItem->getWidth();
						}
						m_CursorPosCol		+= size/GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(pItemBetweenRight->nFontID), "a", m_bPassword);
						m_CursorRect.left	+= size;
						m_CursorRect.right	+= size;
					}else if (pItemBetweenRight->type == eit_Text)
					{
						if (!pItemBetweenRight->text.empty())
						{
							char p = pItemBetweenRight->text[0];
							if (p & 0x80)
								m_CursorPosCol += 2;
							else 
								m_CursorPosCol += 1;
						}
					}
					else 
					{
						m_CursorPosCol += 1;
					}
				}
			}
			if (m_CursorPosCol < 0)
				m_CursorPosCol = 0;

			// 重置选中字符
			_SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
			_SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
			RefreshSelectionRect();
			RefreshCursorPosition();
			AutoAdjectCursor();
		}
		break;
	case vkUp:
		if (m_CursorPosLine > 0)
		{
			SItem* pItemIn = NULL;
			int nItemIndex = 0;
			SItem* pItemBetweenLeft = NULL;
			SItem* pItemBetweenRight = NULL;
			if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex, &pItemBetweenLeft, &pItemBetweenRight))
			{
				if (pItemIn)
				{
					if (pItemIn->type == eit_Text)
					{
						char p = pItemIn->text[nItemIndex];
						if (p & 0x80)
						{
							// @@@@@
						}
					}
				}
			}

			// 光标位置
			m_CursorPosLine --;

			// 重置选中字符
			_SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
			_SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
			RefreshSelectionRect();
			RefreshCursorPosition();
		}
		break;
	case vkDown:
		if (m_CursorPosLine < (int)m_Lines.size() - 1)
		{
			SItem* pItemIn = NULL;
			int nItemIndex = 0;
			SItem* pItemBetweenLeft = NULL;
			SItem* pItemBetweenRight = NULL;
			if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex, &pItemBetweenLeft, &pItemBetweenRight))
			{
				if (pItemIn)
				{
					if (pItemIn->type == eit_Text)
					{
						char p = pItemIn->text[nItemIndex];
						if (p & 0x80)
						{
							// @@@@@
						}
					}
				}
			}

			// 光标位置
			m_CursorPosLine ++;

			// 重置选中字符
			_SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
			_SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
			RefreshSelectionRect();
			RefreshCursorPosition();
		}
		break;
	case vkHome:
		if (m_CursorPosCol > 0)
		{
			m_CursorPosCol = 0;

			// 重置选中字符
			_SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
			_SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
			RefreshSelectionRect();
			RefreshCursorPosition();
			AutoAdjectCursor();
		}
		break;
	case vkEnd:
		{
			SLine* pLine = getLine(m_CursorPosLine);
			if (pLine && !pLine->lineitems.empty())
			{
				SItem* pItem = pLine->getItem(0);
				m_CursorPosCol = pItem->getColCount();

				// 重置选中字符
				_SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
				_SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
				RefreshSelectionRect();
				RefreshCursorPosition();
				AutoAdjectCursor();
			}
		}
		break;
	case vkDelete:
		{
			if (m_SelStartLine == m_SelEndLine && m_SelStartCol == m_SelEndCol)
			{
				SLine* pLine = getLine(m_CursorPosLine);
				UI_ENSURE(pLine);
				SItem* pItemIn = NULL;
				int nItemIndex = 0;
				SItem* pItemBetweenLeft = NULL;
				SItem* pItemBetweenRight = NULL;
				if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex, &pItemBetweenLeft, &pItemBetweenRight))
				{
					if (pItemIn)
					{
						if (pItemIn->type == eit_Text)
						{
							char p = pItemIn->text[nItemIndex];
							if (p & 0x80)
								pItemIn->text.erase(nItemIndex, 2);
							else 
								pItemIn->text.erase(nItemIndex, 1);
						}
					}
					else if (pItemBetweenRight)
					{
						if (pItemBetweenRight->hyperLink.bIsLink())
						{
							// 前后响应可能还有问题
							if (pItemBetweenRight->lastItem)
							{
								SLine* pLastLine = getLine(m_CursorPosLine- 1);
								pLastLine->RemoveItem(pItemBetweenRight->lastItem);
							}

							if (pItemBetweenRight->nextItem)
							{
								SLine* pNextLine = getLine(m_CursorPosLine+ 1);
								pNextLine->RemoveItem(pItemBetweenRight->nextItem);
							}
							pLine->RemoveItem(pItemBetweenRight);
						}
						else if (pItemBetweenRight->type == eit_Text)
						{
							char p = pItemBetweenRight->text[0];
							if (p & 0x80)
								pItemBetweenRight->text.erase(nItemIndex, 2);
							else 
								pItemBetweenRight->text.erase(nItemIndex, 1);

							if (pItemBetweenRight->text.empty())
								pLine->RemoveItem(pItemBetweenRight);
						}
						else
						{
							pLine->RemoveItem(pItemBetweenRight);
						}
					}

					int oldCursorLine = m_CursorPosLine;
					int oldCursorCol  = m_CursorPosCol;

					MergeAllLine();
					SetCursorPos(oldCursorLine, oldCursorCol);
					EvInputChar(this, etInputChar);
					if (getText().empty())
						m_bShowDefaultText =true;
					else
						m_bShowDefaultText = false;
				}
			}
			else 
			{
				RemoveSelect();
			}

			AutoAdjectCursor();
		}
		break;
	case vkBack:
		{
			if (m_CursorPosLine>0 && m_CursorPosCol<=0)
			{
				m_CursorPosLine--;
				m_CursorPosCol = getLine(m_CursorPosLine)->getColCount();
			}

			if (m_SelStartLine == m_SelEndLine && m_SelStartCol == m_SelEndCol)
			{
				SLine* pLine = getLine(m_CursorPosLine);
				UI_ENSURE(pLine);
				SItem* pItemIn = NULL;
				int nItemIndex = 0;
				SItem* pItemBetweenLeft = NULL;
				SItem* pItemBetweenRight = NULL;
				if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex, &pItemBetweenLeft, &pItemBetweenRight))
				{
					if (pItemIn && pItemIn->type==eit_Text && pItemIn->text.size()>0 && nItemIndex>0)
					{
						if (pItemIn->hyperLink.bIsLink())
						{
							if (pItemIn->lastItem)
							{
								SLine* pLastLine = getLine(m_CursorPosLine-1);
								pLastLine->RemoveItem(pItemIn->lastItem);
							}

							if (pItemIn->nextItem)
							{
								SLine* pNextLine = getLine(m_CursorPosLine+1);
								pNextLine->RemoveItem(pItemIn->nextItem);
							}

							pLine->RemoveItem(pItemIn);
							m_CursorPosCol -= 1;
						}
						else
						{
							// 判断前一个字符是不是双字节字符
							if (nItemIndex == 1)
							{
								pItemIn->text.erase(0, 1);
								m_CursorPosCol -= 1;
							}
							else 
							{
								char p = pItemIn->text[nItemIndex-2];
								if (p & 0x80)
								{
									char q = pItemIn->text[nItemIndex-1];
									if (q & 0x80)
									{
										pItemIn->text.erase(nItemIndex-2, 2);
										m_CursorPosCol -= 2;
									}
									else 
									{
										pItemIn->text.erase(nItemIndex-1, 1);
										m_CursorPosCol -= 1;
									}
								}
								else 
								{
									pItemIn->text.erase(nItemIndex-1, 1);
									m_CursorPosCol -= 1;
								}
							}
						}
					}
					else if (pItemBetweenLeft)
					{
						// 前后响应可能还有问题
						if (pItemBetweenLeft->hyperLink.bIsLink())
						{
							if (pItemBetweenLeft->lastItem)
							{
								SLine* pLastLine = getLine(m_CursorPosLine-1);
								pLastLine->RemoveItem(pItemBetweenLeft->lastItem);
							}

							if (pItemBetweenLeft->nextItem)
							{
								SLine* pNextLine = getLine(m_CursorPosLine+1);
								pNextLine->RemoveItem(pItemBetweenLeft->nextItem);
							}

							pLine->RemoveItem(pItemBetweenLeft);
							m_CursorPosCol -= 1;
						}
						else if (pItemBetweenLeft->type == eit_Text)
						{
							// 判断前一个字符是不是双字节字符
							int strsize = (int)pItemBetweenLeft->text.size();
							if (strsize == 1)
							{
								pItemBetweenLeft->text.erase(0, 1);
								m_CursorPosCol -= 1;
							}
							else 
							{
								char p = pItemBetweenLeft->text[strsize-2];
								if (p & 0x80)
								{
									char q = pItemBetweenLeft->text[strsize-1];
									if (q & 0x80)
									{
										pItemBetweenLeft->text.erase(strsize-2, 2);
										m_CursorPosCol -= 2;
									}
									else 
									{
										pItemBetweenLeft->text.erase(strsize-1, 1);
										m_CursorPosCol -= 1;
									}
								}
								else 
								{
									pItemBetweenLeft->text.erase(strsize-1, 1);
									m_CursorPosCol -= 1;
								}
							}

							if (pItemBetweenLeft->text.empty())
								pLine->RemoveItem(pItemBetweenLeft);
						}
						else 
						{
							pLine->RemoveItem(pItemBetweenLeft);
							m_CursorPosCol -= 1;
						}
					}

					if (m_CursorPosLine>0 && m_CursorPosCol<=0)
					{
						m_CursorPosLine--;
						m_CursorPosCol = getLine(m_CursorPosLine)->getColCount();
					}

					int oldCursorLine = m_CursorPosLine;
					int oldCursorCol  = m_CursorPosCol;

					MergeAllLine();
					SetCursorPos(oldCursorLine, oldCursorCol);
					EvInputChar(this, etInputChar);
					if (getText().empty())
						m_bShowDefaultText =true;
					else
						m_bShowDefaultText = false;
				}
			}
			else 
			{
				RemoveSelect();
			}		

			AutoAdjectCursor();
		}
		break;
	case 'c':
	case 'C':
		if (g_workspace.isKeyPressed(vkCtrl))
		{
			_OnCopy();
		}
		break;
	case 'v':
	case 'V':
		if (g_workspace.isKeyPressed(vkCtrl))
		{
			_OnPaste();
		}
		break;
	case 'x':
	case 'X':
		if (g_workspace.isKeyPressed(vkCtrl))
		{
			_OnCut();
		}
		break;
	case 'a':
	case 'A':
		{
			if (g_workspace.isKeyPressed(vkCtrl))
			{
				_OnSelectAll();
			}
		}
		break;
	default:
		break;
	}
}

bool RtwTextBox::_PrepareEnterOk(bool bChinese)
{
	if (!m_bWordWrap || !m_bMultiLine || !m_bEnableInput)
	{
		return false;
	}

	SLine* pLine = getLine(m_CursorPosLine);
	if (!pLine)
	{
		return false;
	}

	int nStep = m_ViewRect.right - pLine->rect.right;
	int nEnglish = GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(m_nFontID), "a", m_bPassword);
	bool bOk = bChinese?nStep<nEnglish*2:nStep<nEnglish;
	if (bOk)
	{
		SItem* pItemIn = NULL;
		int nItemIndex = 0;
		SItem* pItemBetweenLeft = NULL;
		SItem* pItemBetweenRight = NULL;
		if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex, &pItemBetweenLeft, &pItemBetweenRight))
		{
			if (pItemBetweenRight)
			{
				return false;
			}

			int nLineCol = pLine->getColCount();
			if (m_CursorPosCol < nLineCol)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

int RtwTextBox::GetStringNumFromWidth(int nFontID, string str, int nWidth)
{
	int nSize = (int)str.size();
	if (nSize<2 || nWidth<=0)
	{
		return 0;
	}

	int i = 0;
	bool bChinese = false;
	while (i<nSize)
	{
		bChinese = false;
		if ((str[i]&0x80) && (str[i+1]&0x80))
		{
			bChinese = true;
			i++;
		}

		i++;

		if (GetStringWidth(nFontID, str.substr(0, i), m_bPassword) > nWidth)
		{
			if (bChinese)
			{
				i--;
			}

			i--;

			return i;
		}
	}

	return 0;
}

//键盘输入
void RtwTextBox::OnKeyChar_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (!getEnableInput())
		return;

	if (g_workspace.isKeyPressed(vkCtrl) || g_workspace.isKeyPressed(vkTab))
		return;

	//输入的文本
	char character = pEvent->key.code;
	if (character==vkEnter && !g_workspace.IsImeKeyDown())
	{
		if (!m_bMultiLine)
		{
			EvUpdateText(this, weUpdateText);
		}
	}

	//取消选择
	RemoveSelect();

	//不能输入数字?
	if (m_bNumeric) //只允许数字
	{
		if (!m_bMatheMatics)
		{
			if (character=='-')
			{
				return; //不允许输入负数
			}
		}
		else
		{
			string tmp = getText();
			if (!tmp.empty())
			{
				if (tmp[0] == '-')
				{
					if (m_CursorPosCol == 0)
					{
						return;
					}

					if (character == '-')
					{
						return;
					}
				}
			}

			if ((character == '-') && (m_CursorPosCol > 0))
			{
				return;
			}
		}

		//不是数字
		if (character != '-')
		{
			if((character<48 || character>57))
			{
				return;
			}
		}
	}

	//汉字?
	if(!m_acDoubleByte[0] && (character & 0x80))
	{
		m_acDoubleByte[0]=character;
		return;
	}

	if(m_acDoubleByte[0])
		m_acDoubleByte[1]=character;

	bool bChinese = !!(m_acDoubleByte[0] && m_acDoubleByte[1]);	
	if(bChinese)
	{
		int iHead = m_acDoubleByte[0] & 0xff;  
		int iTail = m_acDoubleByte[1] & 0xff;  
		//gb2312编码
		if(iHead>=0xa1 && iHead<=0xf7 && iTail>=0xa1 && iTail<=0xfe) 
		{
			char buf[3] = "\0";
			memcpy(buf, m_acDoubleByte, 2);
			InsertStringToCurrentCursor(buf, false);
		}
		else
		{
			memset(m_acDoubleByte,0,sizeof(char)*2);
			return;
		}
	}
	else
	{
		InsertCharacterToCurrentCursor(character);
	}

	int oldCursorLine = m_CursorPosLine;
	int oldCursorCol  = m_CursorPosCol;
	MergeAllLine();
	m_CursorPosCol = oldCursorCol;
	m_CursorPosLine = oldCursorLine;

	SLine *pLine = getLine(m_CursorPosLine);
	if (pLine)
	{
		int lineCol = pLine->getColCount();
		if (m_CursorPosCol > lineCol)
		{
			//越界喽
			m_CursorPosCol = m_CursorPosCol-lineCol;
			m_CursorPosLine++;
		}
	}

	RefreshCursorPosition();
	AutoAdjectCursor();

	memset(m_acDoubleByte,0, 2);
	EvInputChar(this, etInputChar);
	if (getText().empty())
		m_bShowDefaultText =true;
	else
		m_bShowDefaultText = false;
}

//重置
void RtwTextBox::_ResetSelf()
{
	m_WidgetType		= wtEditBox;
	m_Flag				= m_DefaultFlag;
	m_fontRenderOption	= wtFontRender_Normal;
	m_bEnableInput		= true;
	m_bWordWrap			= false;
	m_bMultiLine		= false;
	m_bAutoScrollV		= false;
	m_ViewOffsetLine	= 0;
	m_CursorPosLine		= 0;
	m_CursorPosCol		= 0;
	m_SelStartLine		= 0;
	m_SelStartCol		= 0;
	m_SelEndLine		= 0;
	m_SelEndCol			= 0;
	m_SelText			= "";
	m_bAutoSizeV		= false;
	m_bAutoSizeH		= false;

	m_Lines.clear();
}

//添加line,名字是错的. 在某个index添加line
bool RtwTextBox::AddItem(SLine& line, int indexBefore/* = -1*/, bool bRefreshItemPos/* = true*/)
{
	if (indexBefore > (int)m_Lines.size())
		return false;

	if (indexBefore < 0)
	{
		m_Lines.push_back(line);
	}
	else 
	{
		std::list<SLine>::iterator iterLine = m_Lines.begin();
		for (int i=0; i<indexBefore; ++i)
		{
			iterLine++;
		}
		m_Lines.insert(iterLine, line);
	}

	return true;
}

//获取line的索引
int RtwTextBox::getLineIndex(SLine* pLine)
{
	int index = 0;
	foreach (std::list<SLine>, iterLine, m_Lines)
	{
		if (&(*iterLine) == pLine)
		{
			return index;
		}

		index ++;
	}
	return -1;
}

//获取item的索引
int RtwTextBox::getItemIndex(SLine* pLine, SItem* pItem)
{
	if (!pLine || !pItem)
		return 0;

	int index = 0;
	foreach (std::list<SItem>, iterItem, pLine->lineitems)
	{
		if (&(*iterItem) == pItem)
		{
			return index;
		}

		index ++;
	}

	return -1;
}

int RtwTextBox::getColCount()
{
	int nResult = 0;
	foreach (std::list<SLine>, iterLine, m_Lines)
	{
		SLine& line = *iterLine;
		foreach (std::list<SItem>, iterItem, line.lineitems)
		{
			SItem& item = *iterItem;
			if (item.type == eit_Image)
			{
				nResult += 2;
			}
			else
			{
				nResult += item.getColCount();
			}
		}
	}

	return nResult;
}

//获取某行某列的item
bool RtwTextBox::_GetItemByLineCol(int line, int col, SItem** ppItemIn, int* pTextItemIndex, SItem** ppItemBetweenLeft, SItem** ppItemBetweenRight)
{
	*ppItemIn			= NULL;
	*pTextItemIndex		= 0;
	*ppItemBetweenLeft	= NULL;
	*ppItemBetweenRight = NULL;

	SLine* pLine = getLine(line);
	UI_ENSURE_B(pLine);

	if (col==0)
	{
		*ppItemIn			= NULL;
		*pTextItemIndex		= 0;
		*ppItemBetweenLeft	= NULL;
		*ppItemBetweenRight = pLine->lineitems.empty() ? NULL : &(pLine->lineitems.front());
		return true;
	}

	bool bFound = false;
	int index = 0;
	foreach(std::list<SItem>, iterItem, pLine->lineitems)
	{
		SItem& item = *iterItem;
		switch (item.type)
		{
		case eit_Text:
			if (index + (int)item.text.size() > col)
			{
				bFound = true;
				int delta			= col - index;
				*ppItemIn			= &item;
				*pTextItemIndex		= delta;
				*ppItemBetweenLeft	= NULL;
				*ppItemBetweenRight = NULL;
			}
			else if (index + (int)item.text.size() == col)
			{
				bFound				= true;
				*ppItemIn			= NULL;
				*pTextItemIndex		= 0;
				*ppItemBetweenLeft	= &item;
				iterItem++;
				if (iterItem == pLine->lineitems.end())
					*ppItemBetweenRight = NULL;
				else 
					*ppItemBetweenRight = &(*iterItem);
			}
			else 
			{
				index += (int)item.text.size();
			}
			break;
		case eit_Image:
			if (index + 1 == col)
			{
				bFound				= true;
				*ppItemIn			= NULL;
				*pTextItemIndex		= 0;
				*ppItemBetweenLeft	= &item;
				iterItem++;
				if (iterItem == pLine->lineitems.end())
					*ppItemBetweenRight = NULL;
				else 
					*ppItemBetweenRight = &(*iterItem);
			}
			else
			{
				index++;
			}
			break;
		default:
			break;
		}

		if (bFound)
			break;
	}

	return bFound;
}

//滚动条移动
void RtwTextBox::OnScroll_ScrollBarV(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	SLine* pFirstLine = getFirstLine();
	UI_ENSURE(pFirstLine);

	SPoint DefaultStartPoint(m_ViewRect.left, m_ViewRect.top);
	SSize OrigOffset(pFirstLine->rect.left - DefaultStartPoint.x, pFirstLine->rect.top - DefaultStartPoint.y);

	int OrigValue = -OrigOffset.height;
	int value = m_pAutoScrollV->getValue();

	OffsetItemPosition(SSize(0, OrigValue - value), 0);
}

bool RtwTextBox::_Intercept(int line, int col, bool bWordWrap)
{
	int itemIndex, itemCol;
	SItem* pItemIn, *pItemBetweenLeft, *pItemBetweenRight;
	SLine* pLine = getLine(line);
	UI_ENSURE_B(pLine);
	UI_ENSURE_B(_GetItemByLineCol(line, col, &pItemIn, &itemCol, &pItemBetweenLeft, &pItemBetweenRight));
	if (pItemIn)
	{
		itemIndex = pLine->getItemIndex(pItemIn);
		return _Intercept(line, itemIndex, itemCol, bWordWrap);
	}
	else 
	{
		if (pItemBetweenLeft)
		{
			itemIndex = pLine->getItemIndex(pItemBetweenLeft);
			return _Intercept(line, itemIndex, pItemBetweenLeft->getColCount(), bWordWrap);
		}
		else if (pItemBetweenRight)
		{
			itemIndex = pLine->getItemIndex(pItemBetweenRight);
			return _Intercept(line, itemIndex, 0, bWordWrap);
		}
		else 
		{
			return false;
		}
	}
}

bool RtwTextBox::_Intercept(int line, int itemIndex, int itemCol, bool bWordWrap)
{
	SLine* pLine = getLine(line);
	UI_ENSURE_B(pLine);
	SItem* pItem = pLine->getItem(itemIndex);
	UI_ENSURE_B(pItem);
	switch (pItem->type)
	{
	case eit_Text:
		{
			int _StartItemIndex = 0;

			SLine* pNewLine = NULL;
			SItem* pItemInsertInto = NULL;
			if (bWordWrap)
			{
				SLine* pNextLine = getLine(line + 1);
				if (pNextLine && pNextLine->bWordWrapTail)
				{
					pItemInsertInto = pNextLine->getItem(0);
					pNewLine = pNextLine;
				}
			}

			if (!pNewLine) 
			{
				SLine NewLine;
				NewLine.height = pLine->height;
				UI_ENSURE_B(AddItem(NewLine, line + 1, false));
				pNewLine = getLine(line + 1);
			}

			// 处理当前Item
			if (itemCol == 0)
			{
				_StartItemIndex = itemIndex;
			}
			else 
			{
				_StartItemIndex = itemIndex + 1;

				if (itemCol >= (int)pItem->text.size())
				{
					itemCol = (int)pItem->text.size();
				}
				std::string textLeft = pItem->text.substr(itemCol);
				pItem->text = pItem->text.substr(0, itemCol);
				if (pItemInsertInto)
				{
					pItemInsertInto->text.insert(0, textLeft);
				}
				else
				{
					SItem NewItem;
					NewItem.type = eit_Text;
					NewItem.color = pItem->color;
					NewItem.bUnderLine = pItem->bUnderLine;
					NewItem.text = textLeft;
					pNewLine->InsertItem(NewItem, 0);
				}
			}
			// 处理后续Item
			for (int i = _StartItemIndex; i < (int)pLine->lineitems.size(); ++i)
			{
				SItem* _pItem = pLine->getItem(i);
				if (!_pItem)
					break;
				pNewLine->InsertItem(*_pItem);
			}
			for (int i = _StartItemIndex; i < (int)pLine->lineitems.size(); ++i)
			{
				pLine->RemoveItem(i);
			}
		}
		break;
	case eit_Image:
		break;
	default:
		break;
	}

	return true;
}

bool RtwTextBox::_Intercept(int line, int col)
{
	int itemIndex, itemCol;
	SItem* pItemIn, *pItemBetweenLeft, *pItemBetweenRight;
	SLine* pLine = getLine(line);
	UI_ENSURE_B(pLine);

	UI_ENSURE_B(_GetItemByLineCol(line, col, &pItemIn, &itemCol, &pItemBetweenLeft, &pItemBetweenRight));
	if (pItemIn)
	{
		UI_ENSURE_B(pItemIn);
		itemIndex = pLine->getItemIndex(pItemIn);

		std::string textLeft = pItemIn->text.substr(itemCol);
		pItemIn->text = pItemIn->text.substr(0, itemCol);

		SItem NewItem;
		NewItem.type = eit_Text;
		NewItem.color = pItemIn->color;
		NewItem.bUnderLine = pItemIn->bUnderLine;
		NewItem.text = textLeft;
		pLine->InsertItem(NewItem, itemIndex+1);

		CalcLinePosition(line);

		return true;
	}
	else 
	{
		return true;
	}
}

//处理回车
bool RtwTextBox::_Intercept_Enter(int line, int col)
{
	int itemIndex, itemCol;
	SItem* pItemIn, *pItemBetweenLeft, *pItemBetweenRight;
	SLine* pLine = getLine(line);
	UI_ENSURE_B(pLine);
	CalcLinePosition(line);

	UI_ENSURE_B(_GetItemByLineCol(line, col, &pItemIn, &itemCol, &pItemBetweenLeft, &pItemBetweenRight));
	if (pItemIn)
	{
		itemIndex = pLine->getItemIndex(pItemIn);

		std::string textLeft = pItemIn->text.substr(itemCol);
		pItemIn->text = pItemIn->text.substr(0, itemCol);

		SLine NewLine;
		SItem NewItem;
		NewItem.type = eit_Text;
		NewItem.color = pItemIn->color;
		NewItem.bUnderLine = pItemIn->bUnderLine;
		NewItem.text = textLeft;
		NewLine.InsertItem(NewItem);
		InsertLine(NewLine, line + 1);

		_MoveTailItemToNextLine(line, itemIndex + 1);

		m_CursorPosLine ++;
		m_CursorPosCol = 0;

		CalcLinePosition(line);
		CalcLinePosition(line + 1);

		AutoAdjectCursor();
	}
	else if (pItemBetweenRight)
	{
		itemIndex = pLine->getItemIndex(pItemBetweenRight);

		SLine NewLine;
		InsertLine(NewLine, line + 1);
		_MoveTailItemToNextLine(line, itemIndex);

		m_CursorPosLine ++;
		m_CursorPosCol = 0;

		CalcLinePosition(line);
		CalcLinePosition(line + 1);

		AutoAdjectCursor();
	}
	else 
	{
		SLine NewLine;
		InsertLine(NewLine, line + 1);

		m_CursorPosLine ++;
		m_CursorPosCol = 0;

		CalcLinePosition(line);
		CalcLinePosition(line + 1);

		AutoAdjectCursor();
	}
	return true;
}

//得到下一个item
RtwTextBox::SItem* RtwTextBox::getNextItem(SLine* pLine, SItem* pItem)
{
	UI_ENSURE_P(pLine && pItem);

	std::list<SItem>::iterator iterItem = pLine->lineitems.begin();
	for (; iterItem != pLine->lineitems.end(); ++iterItem)
	{
		if (&(*iterItem) == pItem)
		{
			iterItem++;
			if (iterItem != pLine->lineitems.end())
				return &(*iterItem);
			else 
				return NULL;
		}
	}

	return NULL;
}

//设置开始选择的位置;
void RtwTextBox::_SetSelectionStart(int line, int col)
{
	m_SelStartLine	= line;
	m_SelStartCol	= col;
}

//设置结束开始的位置
void RtwTextBox::_SetSelectionEnd(int line, int col)
{
	m_SelEndLine	= line;
	m_SelEndCol		= col;
}

//获取被选中的行,列,起始位置
void RtwTextBox::_getSelectionStartEnd(int& lineStart, int& colStart, int& lineEnd, int& colEnd)
{
	if (m_SelEndLine<m_SelStartLine || ((m_SelEndLine==m_SelStartLine) && (m_SelEndCol<m_SelStartCol)))
	{
		lineStart	= m_SelEndLine;
		colStart	= m_SelEndCol;
		lineEnd		= m_SelStartLine;
		colEnd		= m_SelStartCol;
	}
	else 
	{
		lineStart	= m_SelStartLine;
		colStart	= m_SelStartCol;
		lineEnd		= m_SelEndLine;
		colEnd		= m_SelEndCol;
	}
}

//选择所有
void RtwTextBox::SelectAll()
{
	if (!m_Lines.empty())
	{
		SLine* pLastLine = &(m_Lines.back());
		_SetSelectionStart(0, 0);
		_SetSelectionEnd((int)m_Lines.size()-1, pLastLine->getColCount());
	}
}

//删除选择的item
void RtwTextBox::RemoveSelect()
{
	if (m_SelStartLine == m_SelEndLine && m_SelStartCol == m_SelEndCol)
		return;

	int CursorLine	= 0;
	int CursorCol	= 0;
	int nLine		= 0;
	int nCol		= 0;

	bool bIsThis = true;
	SItem *pItem;
	foreach (std::list<SLine>, iterLine, m_Lines)
	{
		for (std::list<SItem>::iterator iterItem=iterLine->lineitems.begin(); iterItem!=iterLine->lineitems.end();)
		{
			pItem = &(*iterItem);
			if (pItem->SelectBegin==0 && pItem->SelectEnd==0)
			{
				++iterItem;
				continue;
			}

			switch (pItem->type)
			{
			case eit_Text:
				{
					if (bIsThis)
					{
						CursorLine = nLine;
					}

					if (pItem->SelectBegin==0 && pItem->SelectEnd==pItem->getColCount())
					{
						if (bIsThis)
						{
							CursorCol = nCol;
							bIsThis = false;
						}

						iterItem = iterLine->lineitems.erase(iterItem);
						continue;
					}
					else
					{
						if (bIsThis)
						{
							CursorCol = nCol+pItem->SelectBegin;
							bIsThis = false;
						}
						pItem->text.erase(pItem->SelectBegin, pItem->SelectEnd-pItem->SelectBegin);
					}
				}
				break;
			case eit_Image:
				if (bIsThis)
				{
					CursorLine = nLine;
					CursorCol = nCol;
					bIsThis = false;
				}

				iterItem = iterLine->lineitems.erase(iterItem);
				continue;
			default:
				break;
			}

			nCol += pItem->getColCount();
			++iterItem;
		}
		nCol = 0;
		++nLine;
	}

	MergeAllLine();
	_SetSelectionStart(0, 0);
	_SetSelectionEnd(0, 0);
	RefreshSelectionRect();
	m_CursorPosLine	= CursorLine;
	m_CursorPosCol	= CursorCol;
	RefreshCursorPosition();
}

void RtwTextBox::ScrollToTop()
{
	if (m_pAutoScrollV)
	{
		m_pAutoScrollV->ScrollToTop();
	}
}

void RtwTextBox::ScrollToBottom()
{
	if (m_pAutoScrollV)
	{
		m_pAutoScrollV->ScrollToBottom();
	}
}

//获取被选中的文本
std::string RtwTextBox::getSelectText()
{
	std::string strSelection = "";
	int SelStartLine, SelStartCol, SelEndLine, SelEndCol;
	_getSelectionStartEnd(SelStartLine, SelStartCol, SelEndLine, SelEndCol);

	for (int lineIndex = SelStartLine; lineIndex <= SelEndLine; ++lineIndex)
	{
		SLine* pLine = getLine(lineIndex);
		if (!pLine)
			continue;

		int _SelStart		= 0;
		int _SelEnd			= 0;
		int nTmpStart		= 0;
		int nTmpEnd			= 0;
		int nTemp			= 0;
		std::string strTmp	= "";

		if (lineIndex == SelStartLine)
		{
			nTemp = nTmpStart = nTmpEnd = 0; 
			for (std::list<SItem>::iterator iterList=pLine->lineitems.begin(); iterList!=pLine->lineitems.end(); ++iterList)
			{
				nTemp		=	iterList->getColCount();
				_SelStart	+=	nTemp;
				strTmp		=	iterList->text;

				if (_SelStart>=SelStartCol)
				{
					//找到开始的位置了
					if (nTmpStart<=SelStartCol)
					{
						strSelection += strTmp.substr(SelStartCol-nTmpStart);
					}
					else
					{
						strSelection += strTmp;
					}
				}

				if (_SelStart >= SelEndCol && lineIndex == SelEndLine)
				{
					//同行结束了
					return strSelection;
				}

				nTmpStart	+= nTemp;
				nTmpEnd     += nTemp;
			}
		}
		else if (lineIndex == SelEndLine)
		{
			nTmpEnd = 0;
			//结束位置肯定不是同一行
			for (std::list<SItem>::iterator iterList=pLine->lineitems.begin(); iterList!=pLine->lineitems.end(); ++iterList)
			{
				nTemp = iterList->getColCount();
				strTmp = iterList->text;
				_SelEnd += nTemp;
				if (_SelEnd >= SelEndCol)
				{
					//同行结束了
					if (nTmpEnd<=SelStartCol)
					{
						strSelection += strTmp.substr(0, SelEndCol-nTmpEnd);
					}
					return strSelection;
				}
				else
				{
					strSelection += iterList->text;
				}

				nTmpEnd += nTmpEnd;			
			}
		}
		else
		{
			for (std::list<SItem>::iterator iterList=pLine->lineitems.begin(); iterList!=pLine->lineitems.end(); ++iterList)
			{
				strSelection += iterList->text;
			}
		}
	}

	return strSelection;
}

//删除子控件
void RtwTextBox::OnRemoveChild(RtwWidget* pWidget)
{

	if (pWidget == m_pScrollBarV)
		m_pScrollBarV = NULL;

}

//是否只能输入文本
void RtwTextBox::SetNumberOnly(bool bNum)
{
	m_bNumeric = bNum;
}

//富文本
void RtwTextBox::SetRichText(const std::string& richtext)
{
	if (!m_pRichTextLoader->LoadFromString(richtext))
		return;

	for (std::list<SLine>::iterator iterLine = m_pRichTextLoader->m_Lines.begin(); iterLine != m_pRichTextLoader->m_Lines.end(); iterLine++)
	{
		SLine& _Line = *iterLine;
		InsertLine(_Line);
	}

	SetCursorPos(0, 0);
}

bool RtwTextBox::LoadHtmlFile(const std::string& htmlFile)
{
	if(!m_pHtmlTextLoader->LoadFromFile(htmlFile))
		return false;

	RemoveAllLines();
	m_CursorPosLine	= 0;
	m_CursorPosCol	= 0;
	m_SelStartLine	= 0;
	m_SelStartCol	= 0;
	m_SelEndLine	= 0;
	m_SelEndCol		= 0;

	for(std::list<SLine>::iterator iterLine = m_pHtmlTextLoader->m_Lines.begin(); iterLine != m_pHtmlTextLoader->m_Lines.end(); ++iterLine)
	{
		SLine& _Line = *iterLine;

		InsertLine(_Line);
	}

	SetCursorPos(0,0);

	return true;
}

//设置html文本
void RtwTextBox::SetHtmlText(const std::string& htmltext, bool bWordWrap, bool bMultiLine)
{
	// 删除所有行，并重置相关标志
	RemoveAllLines();
	m_CursorPosLine	= 0;
	m_CursorPosCol	= 0;
	m_SelStartLine	= 0;
	m_SelStartCol	= 0;
	m_SelEndLine	= 0;
	m_SelEndCol		= 0;

	if(!m_pHtmlTextLoader->LoadFromString(htmltext))
		return ;

	SetWordWrap(bWordWrap);
	SetMultiLine(bMultiLine);

	for(std::list<SLine>::iterator iterLine = m_pHtmlTextLoader->m_Lines.begin(); iterLine != m_pHtmlTextLoader->m_Lines.end(); ++iterLine)
	{
		SLine& _Line = *iterLine;
		
		InsertLine(_Line);
	}
	SetCursorPos(0,0);
}

//从文件里加载
bool RtwTextBox::LoadFromFile(const char* szFileName)
{
	if (!m_pRichTextLoader->LoadFromFile(szFileName))
		return false;

	for (std::list<SLine>::iterator iterLine = m_pRichTextLoader->m_Lines.begin(); iterLine != m_pRichTextLoader->m_Lines.end(); ++iterLine)
	{
		SLine& _Line = *iterLine;
		InsertLine(_Line);
	}

	SetCursorPos(0, 0);

	return true;
}

//计算行的位置
void RtwTextBox::CalcLinePosition(int LineIndex, const SPoint& StartPoint)
{
	SLine *pLine = getLine(LineIndex); //获取LineIndex那行的line
	if (!pLine) return;

	RtwRect OrigLineRect = pLine->rect; //备份line之前的rect

	//重置line
	pLine->rect.left	= StartPoint.x + pLine->slleft;
	pLine->rect.top		= StartPoint.y + pLine->sltop;
	pLine->rect.right	= pLine->rect.left;
	pLine->rect.bottom	= pLine->rect.top;

	SPoint CurrPoint(pLine->rect.left, pLine->rect.top); //line的起始位置

	int nImageHeight= 0;

	//取得图片的最大高度
	foreach (std::list<SItem>, iterItem, pLine->lineitems)
	{
		SItem& item = *iterItem;
		if (item.getImage())
		{
			switch (item.type)
			{
			case eit_Image:
				{
					int height = item.getImage() ? item.imgSize.height: 0;
					if (nImageHeight < height)
					{
						nImageHeight = height;
					}
				}
				break;
			}
		}
	}

	int nItemIndex	= -1;
	foreach (std::list<SItem>, iterItem, pLine->lineitems)
	{
		SItem& item = *iterItem;
		nItemIndex ++;

		item.rect.left		= CurrPoint.x;
		item.rect.top		= CurrPoint.y;

		switch (item.type)
		{
		case eit_Image:
			{
				int width, height;
				width = height = 0;
				RtwImagePtr pImage =  item.getImage();

				if (pImage)
				{
					width	= item.imgSize.width;
					height	= item.imgSize.height;
				}

				item.rect.right		= item.rect.left+width;
				item.rect.bottom	= item.rect.top +height;

				// 如果图片超过最大宽度时，换行到下一行显示
				if (m_nMaxWidth && item.rect.right > m_ViewRect.left + m_nMaxWidth)
				{
					if (m_bWordWrap/* && !m_bAutoSizeH*/)
					{
						item.rect.right		= item.rect.left;
						item.rect.bottom	= item.rect.top;

						_MoveTailItemToNextLine(LineIndex, nItemIndex + 1,pLine->sltop);

						// 不再向下执行
						break;
					}
				}

				if (item.rect.right >= m_ViewRect.right-pLine->slright)
				{
					if (getWidgetType() == wtHtmlView) //如果是htmlview,则当图片没显示完的时候不换行
					{
						int nLineWidth = pLine->rect.right;
						if (nLineWidth <= m_ViewRect.right)
						{
							break;
						}
					}

					if (m_bWordWrap && !m_bAutoSizeH)
					{
						item.rect.right		= item.rect.left;
						item.rect.bottom	= item.rect.top;

						_MoveTailItemToNextLine(LineIndex, nItemIndex + 1,pLine->sltop);
					}
				}
			}
			break;
		case eit_Name:
		case eit_Text:
			{
				int width  = GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(item.nFontID), item.text.c_str(), m_bPassword);
				int height = max(nImageHeight, g_workspace.GetUiRender()->GetFontHeight(g_workspace.GetUiRender()->GetNativeFontId(item.nFontID)));

				item.rect.right	=  item.rect.left + width;
				item.rect.bottom = item.rect.top  + height;

				// 如果超过最大宽度时，换行到下一行显示
				if (m_nMaxWidth && item.rect.right > m_ViewRect.left + m_nMaxWidth)
				{
					if (m_bWordWrap/* && !m_bAutoSizeH && !item.text.empty()*/)
					{
						int nTextNum	= GetStringNumFromWidth(g_workspace.GetUiRender()->GetNativeFontId(item.nFontID), item.text, m_ViewRect.left + m_nMaxWidth-item.rect.left);
						item.rect.right = m_ViewRect.left + m_nMaxWidth+pLine->slright;

						SItem NewItem;
						NewItem			= item;
						item.text		= item.text.substr(0, nTextNum);
						NewItem.text	= NewItem.text.substr(nTextNum);

						_MoveTailItemToNextLine(LineIndex, nItemIndex + 1,pLine->sltop);
						SLine* pNextLine = getLine(LineIndex + 1);
						UI_CHECK(pNextLine);
						if (!pNextLine->lineitems.empty())
						{
							// 与下一行的第一个Item合并
							SItem& _ItemFirst = pNextLine->lineitems.front();
							if (_ItemFirst.type == eit_Text && NewItem.color == _ItemFirst.color && NewItem.hyperLink.childType==_ItemFirst.hyperLink.childType)
							{
								_MergeNeighborItem(&NewItem, &_ItemFirst);
								pNextLine->lineitems.pop_front();
							}
						}
						NewItem.rect.top +=pLine->sltop;
						pNextLine->lineitems.push_front(NewItem);
						std::list<SItem>::iterator iterNext = pNextLine->lineitems.begin();
						SItem& nextItem = *(iterNext);
						if (item.hyperLink.childType == hlinkItem)
						{
							// 如果是超链，那么建立指针的联系
							item.nextItem = &nextItem;
							nextItem.lastItem = &item;
						}
						break;
					}
				}

				if (item.rect.right > m_ViewRect.right-pLine->slright)
				{
					if (m_bWordWrap && !m_bAutoSizeH && !item.text.empty())
					{
						int nTextNum	= GetStringNumFromWidth(g_workspace.GetUiRender()->GetNativeFontId(item.nFontID), item.text, m_ViewRect.right-item.rect.left);
						item.rect.right = m_ViewRect.right;

						SItem NewItem;
						NewItem			= item;
						item.text		= item.text.substr(0, nTextNum);
						NewItem.text	= NewItem.text.substr(nTextNum);

						_MoveTailItemToNextLine(LineIndex, nItemIndex + 1,pLine->sltop);
						SLine* pNextLine = getLine(LineIndex + 1);
						UI_CHECK(pNextLine);
						if (!pNextLine->lineitems.empty())
						{
							// 与下一行的第一个Item合并
							SItem& _ItemFirst = pNextLine->lineitems.front();
							if (_ItemFirst.type == eit_Text && NewItem.color == _ItemFirst.color && NewItem.hyperLink.childType==_ItemFirst.hyperLink.childType)
							{
								_MergeNeighborItem(&NewItem, &_ItemFirst);
								pNextLine->lineitems.pop_front();
							}
						}
						NewItem.rect.top +=pLine->sltop;
						pNextLine->lineitems.push_front(NewItem);
						std::list<SItem>::iterator iterNext = pNextLine->lineitems.begin();
						SItem& nextItem = *(iterNext);
						if (item.hyperLink.childType == hlinkItem)
						{
							// 如果是超链，那么建立指针的联系
							item.nextItem = &nextItem;
							nextItem.lastItem = &item;
						}
					}
				}
			}
			break;
		default:
			break;
		}

		item.rect.bottom += m_LineSpace;

		CurrPoint.x = item.rect.right;
		CurrPoint.y = item.rect.top;

		if (item.rect.right>pLine->rect.right)
		{
			pLine->rect.right = item.rect.right;
		}

		if (item.rect.bottom > pLine->rect.bottom)
		{
			pLine->rect.bottom = item.rect.bottom;
		}
		//
		item.rcFrame = item.rect;
	}

	// 移动后面的所有行
	SSize DeltaOffset(pLine->rect.left - OrigLineRect.left, pLine->rect.bottom-OrigLineRect.bottom);
	OffsetItemPosition(DeltaOffset, LineIndex+1);

	// 计算后面的Wrap行
	SLine* pNextLine = getLine(LineIndex+1);
	if (pNextLine && pNextLine->bWordWrapTail)
	{
		SPoint _Point(StartPoint.x, pLine->rect.bottom + m_LineSpace);
		CalcLinePosition(LineIndex + 1, _Point);
	}

	// 调整ScrollBarV的数据
	RefreshScrollBarVData();

	// 处理自动调整大小
	if (m_bAutoSizeV || m_bAutoSizeH)
	{
		RtwRect rcAuto;
		rcAuto.left		= m_rcFrame.left;
		rcAuto.top		= m_rcFrame.top;
		rcAuto.right	= m_rcFrame.left;
		rcAuto.bottom	= m_rcFrame.top;

		for (std::list<SLine>::iterator iterTmp = m_Lines.begin(); iterTmp != m_Lines.end(); ++iterTmp)
		{
			SLine& line = *iterTmp;
			rcAuto.Expand(line.rect);
		}

		rcAuto.right  += m_LeftSpace+m_LeftMargin;
		rcAuto.bottom += m_LineSpace+m_BottomMargin;

		if (!m_bAutoSizeV)
		{
			rcAuto.bottom = m_rcFrame.bottom;
			rcAuto.top    = m_rcFrame.top;
		}
		if (!m_bAutoSizeH)
		{
			rcAuto.right = m_rcFrame.right;
			rcAuto.left  = m_rcFrame.left;
		}

		if (m_rcFrame != rcAuto)
		{
			Move(rcAuto, true);
		}
	}
}

//刷新滚动条数据
void RtwTextBox::RefreshScrollBarVData()
{
	if (m_pAutoScrollV)
	{
		SLine* pFirstLine = getFirstLine();
		SLine* pLastLine = getLastLine();
		UI_CHECK(pFirstLine);
		UI_CHECK(pLastLine);

		int LineTotalHeight = 0;
		LineTotalHeight = pLastLine->getBottom() - pFirstLine->getTop();
		LineTotalHeight += 12; //增加12像素的空余

		if (LineTotalHeight < m_ViewRect.getHeight())
		{
			m_pAutoScrollV->SetData(0, 0, 0);
		}
		else 
		{
			int Delta = LineTotalHeight - m_ViewRect.getHeight();
			int _Value = m_pAutoScrollV->getValue();
			if (_Value > Delta)
				_Value = Delta;
			else if (_Value < 0)
				_Value = 0;

			Real _Rate = (Real)m_ViewRect.getHeight() / LineTotalHeight;
			m_pAutoScrollV->SetData(0, Delta, _Value, _Rate);
			if (getCursorLine() >= getLineCount()-1)
			{
				ScrollToBottom();
			}
		}
	}
}

//移动item到下一行
bool RtwTextBox::_MoveTailItemToNextLine(int LineIndex, int ItemIndexFrom,int top ,int bottom)
{
	SLine* pLine = getLine(LineIndex);
	UI_ENSURE_B(pLine);

	SLine* pNextLine = getLine(LineIndex + 1);
	if (!pNextLine || !pNextLine->bWordWrapTail) // @@@@@ 外层判断bWordWrapTail
	{
		SLine NewLine;
		NewLine.bWordWrapTail = true;
		if (top>0)
		{
			NewLine.m_type=1;
		}
		NewLine.sltop=top;
		InsertLine(NewLine, LineIndex + 1);
		pNextLine = getLine(LineIndex + 1);
	}

	if (pNextLine)
	{
		// 把余下的Item转移到下一行
		int ItemCount = (int)pLine->lineitems.size();
		for (int iCut = ItemIndexFrom; iCut < ItemCount; ++iCut)
		{
			SItem& _ItemLast = pLine->lineitems.back();
			if (!pNextLine->lineitems.empty())
			{
				// 与下一行的第一个Item合并
				SItem& _ItemFirst = pNextLine->lineitems.front();
				if (_ItemLast.type==eit_Text && _ItemFirst.type==eit_Text && _ItemLast.color==_ItemFirst.color && _ItemFirst.hyperLink.childType==_ItemLast.hyperLink.childType)
				{
					_MergeNeighborItem(&_ItemLast, &_ItemFirst);
					pNextLine->lineitems.pop_front();
				}
			}
			pNextLine->lineitems.push_front(_ItemLast);
			pLine->lineitems.pop_back();
		}
	}

	return true;
}

//计算行位置
void RtwTextBox::CalcLinePosition(int LineIndex)
{
	SLine* pLine = getLine(LineIndex);
	UI_ENSURE(pLine);

	SPoint point(m_ViewRect.left, m_ViewRect.top);
	SLine* pPreLine = getLine(LineIndex-1);
	if (pPreLine)
	{
		point.y = pPreLine->rect.bottom + m_LineSpace;
	}

	CalcLinePosition(LineIndex, point);
}

void RtwTextBox::RefreshPosition()
{
	int LineIndex = 0;
	SPoint StartPoint(m_ViewRect.left, m_ViewRect.top);
	SLine* pLine = getLine(LineIndex);
	if (pLine)
	{
		CalcLinePosition(LineIndex, StartPoint);
	}

	RefreshCursorPosition();
	RefreshSelectionRect();
}

void RtwTextBox::SetScrollPosition(int pos)
{
	SLine *pFirstLine = getFirstLine();
	UI_ENSURE(pFirstLine);

	SLine *pLastLine =  getLine((int)m_Lines.size()-1);
	UI_ENSURE(pLastLine);

	int nHeight = m_ViewRect.getHeight();
	int nChildHeight = pLastLine->rect.bottom - pFirstLine->rect.top;
	int nLength = m_ViewRect.top-pFirstLine->rect.top;

	int nStep = nChildHeight-nHeight;
	int nValueOff = 0;

	if (nStep <= 0)
	{
		nValueOff = nLength;
	}
	else
	{
		int nNewLength = (nStep*pos)/100;
		nValueOff = nLength-nNewLength;
	}

	if (nValueOff != 0)
	{
		OffsetItemPosition(SSize(0, nValueOff), 0);
	}
}

//移动item
void RtwTextBox::OffsetItemPosition(const SSize& Delta, int LineFrom/* = 0*/)
{
	if (LineFrom < 0 || Delta.IsZero())
		return;

	list<SLine>::iterator it = m_Lines.begin();
	advance(it, LineFrom);

	while (it != m_Lines.end())
	{
		it->Offset(Delta);
		++it;
	}

	RefreshCursorPosition();
	RefreshSelectionRect();
}

void RtwTextBox::RefreshCursorPosition()
{
	UI_ENSURE(m_bEnableInput);

	int nDefaultHeight = g_workspace.GetUiRender()->GetFontHeight(g_workspace.GetUiRender()->GetNativeFontId(m_nFontID));
	SLine* pLine = getLine(m_CursorPosLine);
	if (!pLine)
	{
		m_CursorRect.left	= m_ViewRect.left;
		m_CursorRect.right	= m_CursorRect.left + 1;
		m_CursorRect.top	= m_ViewRect.top;
		m_CursorRect.bottom = m_CursorRect.top + nDefaultHeight;
		return;
	}
	else if (pLine->lineitems.empty())
	{
		m_CursorRect.left	= m_ViewRect.left;
		m_CursorRect.right	= m_CursorRect.left + 1;
		m_CursorRect.top	= pLine->rect.top;
		m_CursorRect.bottom = m_CursorRect.top + nDefaultHeight;
		return;
	}

	int textsize	= 0;
	SItem* pTmpItem = 0;
	int nHeight		= 0;
	int nTextWidth	= 0;

	foreach(std::list<SItem>, iterItem, pLine->lineitems)
	{
		SItem &item = *iterItem;
		if (item.type==eit_Text)
		{
			nHeight = max(nHeight, g_workspace.GetUiRender()->GetFontHeight(g_workspace.GetUiRender()->GetNativeFontId(item.nFontID)));
			textsize+=(int)item.text.size();
		}
		else if (item.type==eit_Image)
		{
			textsize++;
			if (textsize<=m_CursorPosCol)
			{
				if (item.imgSize.height > nHeight)
				{
					nHeight = max(nHeight, item.imgSize.height);
				}
			}
		}

		if (textsize >= m_CursorPosCol)
		{
			pTmpItem = &(*iterItem);
			break;
		}
	}

	if (pTmpItem)
	{
		if (pTmpItem->type == eit_Text)
		{
			if (pTmpItem->hyperLink.bIsLink())
			{
				nTextWidth = GetStringWidth(
					g_workspace.GetUiRender()->GetNativeFontId(pTmpItem->nFontID), 
					pTmpItem->text.substr(0, (int)pTmpItem->text.size()+m_CursorPosCol).c_str(),
					m_bPassword
					);
			}
			else
			{
				nTextWidth = GetStringWidth(
					g_workspace.GetUiRender()->GetNativeFontId(pTmpItem->nFontID), 
					pTmpItem->text.substr(0, (int)pTmpItem->text.size()-textsize+m_CursorPosCol).c_str(),
					m_bPassword
					);
			}
		}
		else if (pTmpItem->type == eit_Image) 
		{
			nTextWidth =pTmpItem->imgSize.width;
			if (m_CursorPosCol == 0)
			{
				nTextWidth = 0;
			}
		}

		m_CursorRect.left	= pTmpItem->rect.left + nTextWidth;
		m_CursorRect.right	= m_CursorRect.left + 1;
		m_CursorRect.top	= pLine->rect.top;
		m_CursorRect.bottom = m_CursorRect.top + (nHeight?nHeight-1:nDefaultHeight);
	}
}

void RtwTextBox::RefreshSelectionRect()
{
	int SelStartLine, SelEndLine, SelStartCol, SelEndCol;
	_getSelectionStartEnd(SelStartLine, SelStartCol, SelEndLine, SelEndCol);

	for (list<SLine>::iterator L = m_Lines.begin(); L != m_Lines.end(); ++L)
	{
		for (list<SItem>::iterator I = L->lineitems.begin(); I != L->lineitems.end(); ++I)
		{
			SItem& item = (*I);

			item.SelectBegin = 0;
			item.SelectEnd = 0;
			item.rcSelect.left	 = item.rect.left;
			item.rcSelect.right	 = item.rect.left;
			item.rcSelect.top    = item.rect.top;
			item.rcSelect.bottom = item.rect.bottom;
		}
	}

	bool bSameLine = (SelStartLine==SelEndLine);
	if (bSameLine && SelStartCol==SelEndCol)
	{
		//不需要绘制了
		return;
	}

	SLine *pStartLine, *pEndLine;
	pStartLine	= getLine(SelStartLine);
	pEndLine	= bSameLine?pStartLine:getLine(SelEndLine);
	if (!pStartLine || !pEndLine)
	{
		//开始或结束有错误，不搞了
		return;
	}

	int nColCount	= 0;
	int nPreCount	= 0;
	int nCurCol		= 0;

	if (bSameLine) //同一行
	{
		for (list<SItem>::iterator it = pStartLine->lineitems.begin(); it != pStartLine->lineitems.end(); ++it)
		{
			SItem& item = (*it);
			nPreCount	=	nColCount;
			nCurCol		=	item.getColCount();
			nColCount	+=	nCurCol;

			if ((nColCount>SelStartCol))
			{
				if (nPreCount<SelStartCol)
				{
					item.SelectBegin = SelStartCol-nPreCount;
					if (nColCount > SelEndCol)
					{
						item.SelectEnd = SelEndCol-nPreCount;
						goto findok; //over了
					}
					else
					{
						item.SelectEnd = nCurCol;
					}
				}
				else
				{
					if (nColCount>SelEndCol && nPreCount<SelEndCol)
					{
						item.SelectBegin	= 0;
						item.SelectEnd	= SelEndCol-nPreCount;
						goto findok; //over了
					}

					item.SelectBegin	= 0;
					item.SelectEnd	= nCurCol;
				}
			}
		}
	}
	else
	{
		list<SLine>::iterator s = m_Lines.begin();
		advance(s, SelStartLine + 1);

		if (s != m_Lines.end())
		{
			list<SLine>::iterator e = m_Lines.begin();
			advance(e, SelEndLine);

			while (s != e)
			{
				for (list<SItem>::iterator I = s->lineitems.begin(); I != s->lineitems.end(); ++I)
				{
					SItem& item = (*I);
					item.SelectBegin = 0;
					item.SelectEnd = item.getColCount();
				}

				++s;
			}
		}

		nColCount = 0;
		for (list<SItem>::iterator I = pStartLine->lineitems.begin(); I != pStartLine->lineitems.end(); ++I)
		{
			SItem& item = (*I);

			nPreCount	=	nColCount;
			nCurCol		=	item.getColCount();
			nColCount	+=	nCurCol;

			if ((nColCount>SelStartCol))
			{
				if (nPreCount<SelStartCol)
				{
					item.SelectBegin	= SelStartCol-nPreCount;
				}
				else
				{
					item.SelectBegin	= 0;
				}
				item.SelectEnd	= nCurCol;
			}
		}

		nColCount = 0;
		for (list<SItem>::iterator I = pEndLine->lineitems.begin(); I != pEndLine->lineitems.end(); ++I)
		{
			SItem& item = (*I);

			nPreCount	=	nColCount;
			nCurCol		=	item.getColCount();
			nColCount	+=	nCurCol;

			item.SelectBegin	= 0;

			if (nColCount>SelEndCol && nPreCount<SelEndCol)
			{
				item.SelectEnd	= SelEndCol-nPreCount;
				//找到了
				goto findok;
			}

			item.SelectEnd	= nCurCol;
		}
	}

findok:
	for (list<SLine>::iterator L = m_Lines.begin(); L != m_Lines.end(); ++L)
	{
		for (list<SItem>::iterator I = L->lineitems.begin(); I != L->lineitems.end(); ++I)
		{
			SItem& item = (*I);

			if ((item.SelectBegin==item.SelectEnd) || (item.SelectBegin==0 && item.SelectEnd==0))
			{
				continue;
			}

			switch (item.type)
			{
			case eit_Text:
				item.rcSelect.left	= item.rect.left + GetStringWidth(
					g_workspace.GetUiRender()->GetNativeFontId(item.nFontID),
					item.text.substr(0, item.SelectBegin).c_str(), 
					m_bPassword
					);
				item.rcSelect.right	= item.rect.left + GetStringWidth(
					g_workspace.GetUiRender()->GetNativeFontId(item.nFontID), 
					item.text.substr(0, item.SelectEnd).c_str(), 
					m_bPassword
					);
				break;
			case eit_Image:
				item.rcSelect = item.rect;
				break;
			default:
				break;
			}
		}
	}
}

void RtwTextBox::SetShowCount(int nCount)
{
	if (m_nShowCount == nCount)
	{
		return;
	}

	m_nShowCount = nCount;
	std::list<SLine>::iterator iterLine = m_Lines.begin();
	while (iterLine != m_Lines.end())
	{
		iterLine->showCount = m_nShowCount;
		iterLine++;
	}
}

void RtwTextBox::EnableRoll(bool bEnable) 
{ 
	m_bIsRoll = bEnable;
	std::list<SLine>::iterator iterLine = m_Lines.begin();
	while (iterLine != m_Lines.end())
	{
		std::list<SItem>::iterator si = iterLine->lineitems.begin();
		while (si != iterLine->lineitems.end())
		{
			si->bIsRoll = m_bIsRoll;

			++si;
		}

		++iterLine;
	}
}


//插入行
bool RtwTextBox::InsertLine(SLine& line, int indexBefore/* = -1*/)
{
	//ldr123
	//如果行数大于规定的行数,则删除最后一行
	int nLine = (int)m_Lines.size();
	if (m_nLineNum <= nLine)
	{
		RemoveFristLine();
		nLine--;
	}
	//end

	line.showCount = m_nShowCount;

	if (indexBefore<0 || indexBefore>nLine)
	{
		m_Lines.push_back(line);
		CalcLinePosition((int)m_Lines.size()-1);
	}
	else 
	{
		std::list<SLine>::iterator iterLine = m_Lines.begin();
		for (int i=0; i<indexBefore; ++i)
		{
			iterLine ++;
		}
		m_Lines.insert(iterLine, line);
		CalcLinePosition(indexBefore);
	}

	return true;
}

void RtwTextBox::InsertImage(const RtwImagePtr& image, const string& name)
{
	if (!image) return;
	if(m_Capacity>0 && m_Capacity<=getColCount())
	{
		return;
	}

	SItem item;
	item.type = eit_Image;
	item.SetImage(image);
	item.text = name;
	item.imgSize = SSize(14, 14);
	SLine *pLine = getLine(m_CursorPosLine);
	if (!pLine)
	{
		SLine line;
		InsertLine(line, -1);
		m_CursorPosCol = 0;
		getLine(m_CursorPosLine)->InsertItem(item);
	}
	else
	{
		SItem* _pItem = PickItem(SPoint(m_CursorRect.left, m_CursorRect.top));
		if (!_pItem || _pItem->type!=eit_Text)
		{
			std::list<SItem>::iterator sli = pLine->lineitems.begin();
			int nTmpIndex = 0;
			while (sli != pLine->lineitems.end())
			{
				if (&(*sli) == _pItem)
				{
					break;
				}

				nTmpIndex++;
				++sli;
			}

			pLine->InsertItem(item, _pItem?nTmpIndex+1:0);
		}
		else
		{
			std::list<SItem>::iterator sli = pLine->lineitems.begin();
			int nTmpIndex = 0;
			int nTmpPos = 0;
			while (sli != pLine->lineitems.end())
			{
				if (&(*sli) == _pItem)
				{
					break;
				}

				nTmpPos++;
				nTmpIndex += sli->getColCount();
				++sli;
			}

			int _InsertIndex = m_CursorPosCol-nTmpIndex;
			int nText = (int)_pItem->text.size();
			if (m_CursorPosCol > nText+nTmpIndex)
			{
				_InsertIndex = nText;
			}

			if(m_Capacity>0 && nText>=m_Capacity)
			{
				return;
			}

			SItem newItem;
			_DivideTextItem(*_pItem, _InsertIndex, newItem);
			pLine->InsertItem(item, ++nTmpPos);
			if (!newItem.text.empty())
			{
				pLine->InsertItem(newItem, ++nTmpPos);
			}
		}
	}

	m_CursorPosCol++;
	// 	m_addface=true;
	CalcLinePosition(m_CursorPosLine);
	RefreshCursorPosition();
	AutoAdjectCursor();
}

void RtwTextBox::SetTextnew(const std::string& text)
{
	if (text.empty())
		m_bShowDefaultText = true;
	std::string checkdString = text;
	CheckValidString(checkdString);

	RemoveAllItems(false);

	m_ViewOffsetLine	= 0;
	m_ViewLastLine		= 0;
	m_CursorPosLine		= 0;
	m_CursorPosCol		= 0;
	m_CursorPosColBak	= 0;
	m_SelStartLine		= 0;
	m_SelStartCol		= 0;
	m_SelEndLine		= 0;
	m_SelEndCol			= 0;
	m_SelText			= "";

	static const RtwPixel itemColorList[] = 
	{
		0xffffffff, 0xff30e512, 0xff00aaff, 0xffffd851
	};	//道具颜色列表

	static const RtwPixel itemBorderColorList[] = 
	{
		0xff290000, 0xff011500, 0xff02002b, 0xff160000
	};	//道具颜色列表

	int color = 0xffffffff;
	std::string strMessage		= checkdString;

	size_t nPos		= 0;	//遍历

	bool bFace			= true;    //是不是表情

	bool bItemLinkBegin	= false;	//item链接开始
	bool bItemLinkEnd	= false;	//item链接结束

	bool bTaskLinkBegin	= false;	//任务链接开始

	bool	bClientMoveToBegin	= false;
	bool	bClientMoveToEnd	= false;
	string	strMoveToLink		= "";
	int		nMoveX				= 0;
	int		nMoveY				= 0;

	bool bLocalImageBegin = false;	//本地文件开始
	bool bLocalImageEnd   = false;	//本地文件结束

	char szLinkInfo[1024];	//超链接内容
	char szFackToken[128];

	int		nItemColor;		//链接的颜色

	SItem	itemName;		//名字item
	SItem	itemTemp;		//临时item
	SItem	deTemp;			//description的item
	SItem	itemImage;		//图片item
	SItem	itemLink;		//超链接item

	SLine	lineTemp;		//临时line

	std::string strItemLink		= "";	//道具超链接内容
	std::string strmTaskLink	= "";	//任务超链接内容
	std::string strTempMsg		= "";	//临时用
	std::string strLocalImage	= "";	//本地文件

	itemName.type	= eit_Name;
	itemTemp.type	= eit_Text;
	itemImage.type	= eit_Image;

	itemName.nFontID = itemTemp.nFontID = m_nFontID;

	InsertLine(lineTemp);	//插入临时行

	int nLineWidth	= m_ViewRect.getWidth()-(m_pAutoScrollV?g_workspace.getDefaultScrollVWidth():3);

	while (nPos < strMessage.size())
	{
		if (
			strMessage[nPos]	== '<'
			&&  strMessage[nPos+1]	== 'm' 
			&&  strMessage[nPos+2]	== 'o'
			&&  strMessage[nPos+3]	== 'v'
			&&  strMessage[nPos+4]	== 'e'
			&&  strMessage[nPos+5]	== 't'
			&&  strMessage[nPos+6]	== 'o'
			&&  strMessage[nPos+7]	== '>'
			&&  !bClientMoveToBegin
			)
		{
			if (!strTempMsg.empty())
			{
				SLine *pLastLine = getLine((int)m_Lines.size() -1);
				SItem itemText;
				itemText.text = strTempMsg;
				itemText.type = eit_Text;
				itemText.color= RtwPixel(color);

				pLastLine->InsertItem(itemText);
				CalcLinePosition((int)m_Lines.size() - 1);
				strTempMsg = "";
			}

			//道具链接
			bClientMoveToBegin	= true;
			nPos += 8;
		}

		if (
			strMessage[nPos]	== '<'
			&&  strMessage[nPos+1]	== 'l' 
			&&  strMessage[nPos+2]	== 'o'
			&&  strMessage[nPos+3]	== 'c'
			&&  strMessage[nPos+4]	== 'a'
			&&  strMessage[nPos+5]	== 'l'
			&&  strMessage[nPos+6]	== 'f'
			&&  strMessage[nPos+7]	== 'i'
			&&  strMessage[nPos+8]	== 'l'
			&&  strMessage[nPos+9]	== 'e'
			&&  strMessage[nPos+10]	== ':'
			&&  strMessage[nPos+11]	== '/'
			&&  strMessage[nPos+12]	== '/'
			&&  !bLocalImageBegin
			)
		{
			if (!strTempMsg.empty())
			{
				SLine *pLastLine = getLine((int)m_Lines.size() -1);
				SItem itemText;
				itemText.text = strTempMsg;
				itemText.type = eit_Text;
				itemText.color= RtwPixel(color);

				pLastLine->InsertItem(itemText);
				CalcLinePosition((int)m_Lines.size() - 1);
				strTempMsg = "";
			}

			//道具链接
			bLocalImageBegin	= true;
			nPos += 13;
		}

		if (
			strMessage[nPos]		== 'i'
			&&  strMessage[nPos+1]	== 't' 
			&&  strMessage[nPos+2]	== 'e'
			&&  strMessage[nPos+3]	== 'm'
			&&  strMessage[nPos+4]	== '['
			&&  !bItemLinkBegin
			)
		{
			if (strTempMsg != "")
			{
				SLine *pLastLine = getLine((int)m_Lines.size() -1);

				SItem itemText;
				itemText.text = strTempMsg;
				itemText.type = eit_Text;
				itemText.color= RtwPixel(color);

				pLastLine->InsertItem(itemText);
				CalcLinePosition((int)m_Lines.size() - 1);
				strTempMsg = "";
			}

			//道具链接
			bItemLinkBegin	= true;
			nPos += 5;
		}
		else if (
			strMessage[nPos]	== 't'
			&&  strMessage[nPos+1]	== 'a' 
			&&  strMessage[nPos+2]	== 's'
			&&  strMessage[nPos+3]	== 'k'
			&&  strMessage[nPos+4]	== '['
			&&  bItemLinkBegin
			)
		{
			if (strTempMsg != "")
			{
				SLine *pLastLine = getLine((int)m_Lines.size() -1);
				SItem itemText;
				itemText.text = strTempMsg;
				itemText.type = eit_Text;
				itemText.color= RtwPixel(color);

				pLastLine->InsertItem(itemText);
				CalcLinePosition((int)m_Lines.size() - 1);
				strTempMsg = "";
			}

			//任务链接
			bTaskLinkBegin = true;
			nPos += 5;
		}
		else if (
			strMessage[nPos]=='#'
			&&  strMessage[nPos+1]=='['
			&&  strMessage[nPos+2]=='i'
			&&  strMessage[nPos+3]=='n'
			&&  strMessage[nPos+4]=='t'
			&&  strMessage[nPos+5]=='e'
			&&  strMessage[nPos+6]=='r'
			&&  strMessage[nPos+7]=='c'
			&&  strMessage[nPos+8]=='e'
			&&  strMessage[nPos+9]=='p'
			&&  strMessage[nPos+10]=='t'
			&&  strMessage[nPos+11]==']'
			)
		{
			if (!strTempMsg.empty())
			{
				SLine *pLastLine = getLine((int)m_Lines.size() -1);

				itemTemp.text = strTempMsg;
				itemTemp.type = eit_Text;
				itemTemp.color= RtwPixel(color);
				pLastLine->InsertItem(itemTemp);

				CalcLinePosition((int)m_Lines.size() - 1);
				strTempMsg = "";
			}
			_Intercept_Enter(m_CursorPosLine, m_CursorPosLine);
			nPos += 12;
			continue;
		}
		else if (
			strMessage[nPos]=='#'
			&&  strMessage[nPos+1] == '['
			&&  bFace
			)
		{
			if (!strTempMsg.empty())
			{
				SLine *pLastLine = getLine((int)m_Lines.size() -1);

				itemTemp.text = strTempMsg;
				itemTemp.type = eit_Text;
				itemTemp.color= RtwPixel(color);
				pLastLine->InsertItem(itemTemp);

				CalcLinePosition((int)m_Lines.size() - 1);
				strTempMsg = "";
			}

			std::string strTmp = strMessage.substr(nPos);
			size_t nFaceEnd = strTmp.find(']');
			if (nFaceEnd == std::string::npos)
			{
				//表情找不到结束节点
				bFace = false;
				continue;
			}

			strTmp = strMessage.substr(nPos+2, nFaceEnd-2);
			rt2_format(szFackToken, 128, "face\\%s.gif", strTmp.c_str());
			RtwImagePtr pImage = g_workspace.GetImageFactory()->CreateImage(szFackToken);
			if (!pImage)
			{
				//表情找不到结束节点
				bFace = false;
				continue;
			}

			itemImage.type = eit_Image;
			itemImage.SetImage(pImage);
			itemImage.imgSize = SSize(31, 31);

			SLine *pLastLine = getLine((int)m_Lines.size()-1);
			pLastLine->InsertItem(itemImage);
			nPos += ((int)strTmp.size()+3);
			CalcLinePosition((int)m_Lines.size()-1);
			continue;
		}

		if (bLocalImageBegin)
		{
			if (
				strMessage[nPos]	== 'l'
				&&  strMessage[nPos+1]	== 'o' 
				&&  strMessage[nPos+2]	== 'c'
				&&  strMessage[nPos+3]	== 'a'
				&&  strMessage[nPos+4]	== 'l'
				&&  strMessage[nPos+5]	== 'f'
				&&  strMessage[nPos+6]	== 'i'
				&&  strMessage[nPos+7]	== 'l'
				&&  strMessage[nPos+8]	== 'e'
				&&  strMessage[nPos+9]	== '/'
				&&  strMessage[nPos+10]	== '>'
				&&  !bLocalImageEnd
				)
			{
				//本地图片
				bLocalImageEnd	= true;
				bLocalImageBegin = false;
				nPos += 11;
			}

			if (bLocalImageBegin)
			{
				strLocalImage += strMessage[nPos];
				if (strMessage[nPos] & 0x80)
				{
					//汉字
					nPos++;
					strLocalImage += strMessage[nPos];
				}
				nPos++;
			}

			if (bLocalImageEnd)
			{
				rt2_format(szFackToken, 128, "%s", strLocalImage.c_str());
				RtwImagePtr pLocalImage = g_workspace.GetImageFactory()->CreateImage(szFackToken);

				if (!pLocalImage)
				{
					bFace = true;
					continue;
				}

				itemImage.type = eit_Image;
				itemImage.SetImage(pLocalImage);

				itemImage.imgSize = pLocalImage->GetSize();

				SLine *pLastLine = getLastLine();
				if ((pLastLine->rect.getWidth() + itemImage.imgSize.width > nLineWidth))
				{
					if (pLastLine)
					{
						SLine lineNew;
						InsertLine(lineNew); //插入空行

						pLastLine = getLastLine();
						pLastLine->InsertItem(itemImage);
						CalcLinePosition((int)m_Lines.size() - 1);

						strTempMsg = "";
					}
				}
				else
				{
					pLastLine->InsertItem(itemImage);
				}
				CalcLinePosition((int)m_Lines.size() - 1);
				bItemLinkEnd = false;
				strItemLink = "";
			}
		}
		else if (bItemLinkBegin)
		{
			//道具链接
			if (
				strMessage[nPos]		== ']'
				&&  strMessage[nPos+1]	== 'm' 
				&&  strMessage[nPos+2]	== 'e'
				&&  strMessage[nPos+3]	== 't'
				&&  strMessage[nPos+4]	== 'i'
				&&  bItemLinkBegin
				)
			{
				//道具链接结束
				bItemLinkEnd = true;
				bItemLinkBegin = false;
				nPos += 5;
			}

			if (bItemLinkBegin)
			{
				strItemLink += strMessage[nPos];
				if (strMessage[nPos] & 0x80)
				{
					//汉字
					nPos++;
					strItemLink += strMessage[nPos];
				}
				nPos++;
			}

			if (bItemLinkEnd)
			{
				//解析道具id,颜色,道具名
				sscanf(strItemLink.c_str(), "%d^)!%s", &nItemColor, &szLinkInfo);
				if (strlen(szLinkInfo) == 0)
				{
					bFace = true;

					continue;
				}

				bool bIsName = true;
				int i = 0;

				std::string tmpItemName = "";
				std::string tmpLinkInfo = "";

				while (i < (int)string(szLinkInfo).size())
				{
					if (szLinkInfo[i]=='^' && szLinkInfo[i+1]==')' && szLinkInfo[i+2]=='!')
					{
						bIsName = false;
						i += 3;
					}

					if (bIsName)
					{
						tmpItemName += szLinkInfo[i];
					}
					else
					{
						tmpLinkInfo += szLinkInfo[i];
					}
					i++;
				}

				if (tmpItemName.empty()||tmpItemName==""||tmpLinkInfo.empty()||tmpLinkInfo=="")
				{
					bItemLinkEnd = false;
					strItemLink = "";
					continue;
				}

				SHyperLink tmpLink;

				//道具链接
				tmpLink.childType = hlinkItem;
				tmpLink.Type = hlinkClient;

				rt2_format(szLinkInfo, 1024, "//showItemInfo %s", tmpLinkInfo.c_str());
				tmpLink.Text = szLinkInfo;

				itemLink.type =eit_Text;
				itemLink.text = "[";
				itemLink.text += tmpItemName.c_str();
				itemLink.text += "]";
				itemLink.color = itemColorList[nItemColor%4];
				itemLink.bordercolor = itemBorderColorList[nItemColor%4];
				itemLink.hyperLink = tmpLink;

				SLine *pLastLine = getLastLine();

				int nWidth = GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(itemLink.nFontID), itemLink.text.c_str(), m_bPassword); 

				if (pLastLine->rect.getWidth()+nWidth > nLineWidth)
				{
					string strTemp = itemLink.text;
					int num	= GetStringNumFromWidth(g_workspace.GetUiRender()->GetNativeFontId(itemLink.nFontID), itemLink.text, nLineWidth - pLastLine->rect.getWidth());

					itemLink.text = strTemp.substr(0, num);

					SItem* pLastItem= pLastLine->InsertItem(itemLink);
					CalcLinePosition((int)m_Lines.size() - 1);

					SLine lineNew;
					InsertLine(lineNew); //插入空行
					itemLink.text = strTemp.substr(num);
					SLine *pLastLine = getLastLine();
					SItem* pNextItem= pLastLine->InsertItem(itemLink);

					if (pNextItem && pLastItem)
					{
						pLastItem->lastItem= NULL;
						pLastItem->nextItem= pNextItem;
						pNextItem->lastItem= pLastItem;
						pNextItem->nextItem= NULL;
					}
					strTempMsg = "";
				}
				else
				{
					pLastLine->InsertItem(itemLink);
				}

				CalcLinePosition((int)m_Lines.size() - 1);

				bItemLinkEnd = false;

				strItemLink = "";
			}
		}
		else if (bClientMoveToBegin)
		{
			//寻路链接
			if (
				strMessage[nPos]		== '<'
				&&  strMessage[nPos+1]	== '/' 
				&&  strMessage[nPos+2]	== 'o'
				&&  strMessage[nPos+3]	== 't'
				&&  strMessage[nPos+4]	== 'e'
				&&  strMessage[nPos+5]	== 'v'
				&&  strMessage[nPos+6]	== 'o'
				&&  strMessage[nPos+7]	== 'm'
				&&  strMessage[nPos+8]	== '>'
				&&  bClientMoveToBegin
				)
			{
				//寻路链接结束
				bClientMoveToEnd   = true;
				bClientMoveToBegin = false;
				nPos += 9;
			}

			if (bClientMoveToBegin)
			{
				strMoveToLink += strMessage[nPos];
				if (strMessage[nPos] & 0x80)
				{
					//汉字
					nPos++;
					strMoveToLink += strMessage[nPos];
				}
				nPos++;
			}

			if (bClientMoveToEnd)
			{
				//解析移动的坐标
				char buf[128];
				sscanf(strMoveToLink.c_str(), "%d,%d,%s", &nMoveX, &nMoveY, buf);

				SHyperLink tmpLink;

				//道具链接
				tmpLink.childType = hlinkItem;
				tmpLink.Type = hlinkClient;

				rt2_format(szLinkInfo, 1024, "//clientmoveto %d %d", nMoveX, nMoveY);
				tmpLink.Text = szLinkInfo;

				itemLink.type		=eit_Text;
				itemLink.text		= "[";
				itemLink.text		+= buf;
				itemLink.text		+= "]";
				itemLink.color		= 0xaabbcc00;
				itemLink.bUnderLine = true;
				itemLink.hyperLink	= tmpLink;

				SLine *pLastLine = getLastLine();
				int nWidth = GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(itemLink.nFontID), itemLink.text.c_str(), m_bPassword); 

				if (pLastLine->rect.getWidth()+nWidth > nLineWidth)
				{
					string strTemp = itemLink.text;
					int num	= GetStringNumFromWidth(g_workspace.GetUiRender()->GetNativeFontId(itemLink.nFontID), itemLink.text, nLineWidth - pLastLine->rect.getWidth());

					itemLink.text = strTemp.substr(0, num);

					SItem* pLastItem= pLastLine->InsertItem(itemLink);
					CalcLinePosition((int)m_Lines.size() - 1);

					SLine lineNew;
					InsertLine(lineNew); //插入空行
					itemLink.text = strTemp.substr(num);
					SLine *pLastLine = getLastLine();
					SItem* pNextItem= pLastLine->InsertItem(itemLink);

					CalcLinePosition((int)m_Lines.size() - 1);
					pLastItem->lastItem= NULL;
					pLastItem->nextItem= pNextItem;
					pNextItem->lastItem= pLastItem;
					pNextItem->nextItem= NULL;
					strTempMsg = "";
				}
				else
				{
					pLastLine->InsertItem(itemLink);
				}

				CalcLinePosition((int)m_Lines.size() - 1);

				bClientMoveToEnd = false;

				strMoveToLink = "";

			}
		}
		else if (bTaskLinkBegin)
		{
			//任务链接开启
			bTaskLinkBegin = false; //暂未开放任务链接功能
		}
		else 
		{
			SLine *pLastLine = getLine((int)m_Lines.size() -1);

			//普通文本
			strTempMsg += strMessage[nPos];
			if (strMessage[nPos] & 0x80)
			{
				nPos++;
				strTempMsg += strMessage[nPos];
			}
			nPos++;

			int nWidth = GetStringWidth(g_workspace.GetUiRender()->GetNativeFontId(0), strTempMsg.c_str(), m_bPassword); 
			if (nWidth == 0)
				return;

			if (pLastLine->rect.getWidth() + nWidth > nLineWidth)
			{
				SItem itemText;
				itemText.text = strTempMsg;
				itemText.type = eit_Text;
				itemText.color= RtwPixel(color);

				pLastLine->InsertItem(itemText);

				CalcLinePosition((int)m_Lines.size()-1);

// 				SLine lineTemp;
// 				InsertLine(lineTemp);
				strTempMsg = "";
			}
		}

		bFace = true;
	}

	SLine *pLastLine = getLastLine();
	if (pLastLine && strTempMsg != "")
	{
		SItem itemText;
		itemText.text = strTempMsg;
		itemText.type = eit_Text;
		itemText.color= RtwPixel(color);

		pLastLine->InsertItem(itemText);
		CalcLinePosition((int)m_Lines.size() - 1);
		strTempMsg = "";
	}


	CalcLinePosition((int)m_Lines.size() - 1);
	SetCursorPos(0, (int)text.size());
}

int RtwTextBox::GetStringWidth(int nFontID, string strString, bool bPass)
{
	if (strString.empty())
	{
		return 0;
	}

	return bPass?g_workspace.GetUiRender()->GetStringWidth(nFontID, '*', strString.size()):g_workspace.GetUiRender()->GetStringWidth(nFontID, strString.c_str());
}

int RtwTextBox::GetStringWidth(int nFontID, string strBegin, int number, bool bPass)
{
	if (strBegin.empty())
	{
		return 0;
	}

	if (number <= 0 || number>=(int)strBegin.size())
	{
		return GetStringWidth(nFontID, strBegin, bPass);
	}

	return bPass?g_workspace.GetUiRender()->GetStringWidth(nFontID, '*', number):g_workspace.GetUiRender()->GetStringWidth(nFontID, strBegin.c_str(), strBegin.c_str()+number);
}

bool RtwTextBox::GetLinkItem(const string &strLink, SItem &itemLink)
{
	static const RtwPixel itemColorList[] = 
	{
		0xffffffff, 0xff30e512, 0xff00aaff, 0xffffd851
	};	//道具颜色列表

	static const RtwPixel itemBorderColorList[] = 
	{
		0xff290000, 0xff011500, 0xff02002b, 0xff160000
	};	//道具颜色列表

	int nItemColor;
	char szLinkInfo[1024];
	int i = 0;
	bool bIsName = true;

	//解析道具id,颜色,道具名
	sscanf(strLink.c_str(), "%d^)!%s", &nItemColor, &szLinkInfo);
	if (strlen(szLinkInfo) == 0)
	{
		return false;
	}

	std::string tmpItemName = "";
	std::string tmpLinkInfo = "";

	while (i < (int)string(szLinkInfo).size())
	{
		if (szLinkInfo[i]=='^' && szLinkInfo[i+1]==')' && szLinkInfo[i+2]=='!')
		{
			bIsName = false;
			i += 3;
		}

		if (bIsName)
		{
			tmpItemName += szLinkInfo[i];
		}
		else
		{
			tmpLinkInfo += szLinkInfo[i];
		}
		i++;
	}

	if (tmpItemName.empty()||tmpItemName==""||tmpLinkInfo.empty()||tmpLinkInfo=="")
	{
		return false;
	}

	SHyperLink tmpLink;

	//道具链接
	tmpLink.childType = hlinkItem;
	tmpLink.Type = hlinkClient;

	rt2_format(szLinkInfo, 1024, "//showItemInfo %s", tmpLinkInfo.c_str());
	tmpLink.Text = szLinkInfo;

	itemLink.type =eit_Text;
	itemLink.text = "[";
	itemLink.text += tmpItemName.c_str();
	itemLink.text += "]";
	itemLink.color = itemColorList[nItemColor%4];
	itemLink.bordercolor = itemBorderColorList[nItemColor%4];
	itemLink.hyperLink = tmpLink;

	return true;
}
