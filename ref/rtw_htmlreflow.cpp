#include "ui/rtw_ui.h"

CUiHtmlText::CUiHtmlText()
{
	m_pXmlDoc = RT_NEW RtsXmlDoc();
}

CUiHtmlText::~CUiHtmlText()
{
	DEL_ONE(m_pXmlDoc);
}

bool CUiHtmlText::Reset()
{
	m_Lines.clear();

	if(m_pXmlDoc)
		m_pXmlDoc->Cleanup();
	return true;
}

bool CUiHtmlText::LoadFromFile(const std::string& FileName)
{
	Reset();
	if(!m_pXmlDoc->ParserFile(FileName.c_str()) == 0)
	{
		RtCoreLog().Error("[CUiHtmlText::LoadFromFile] Parse file %s  error\n",FileName.c_str());
		return false;
	}
	if(!_ProcessNode((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot()))
	{
		RtCoreLog().Error("[CUiHtmlText::LoadFromFile] Process file %s error\n",FileName.c_str());
		return false;
	}
	return true;
}

bool CUiHtmlText::LoadFromString(const std::string& strHtml)
{
	Reset();
	if(m_pXmlDoc->ParserString(strHtml.c_str()))
	{
		return false;
	}
	if(!_ProcessNode((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot()))
	{
		return false;
	}
	return true;
}

bool CUiHtmlText::_ProcessNode(RtsXmlDoc::NodePtr* pNode)
{
	if(!pNode)
		return true;
	const std::string& TagName = pNode->strName;
	if(TagName == "html")
	{
		RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
		if(pChildNode->strName == "body")
			UI_ENSURE_B(_Process_UiHtmlText(pChildNode))
	}
	else
	{

	}
	return true;
}

bool CUiHtmlText::_Process_UiHtmlText(RtsXmlDoc::NodePtr* pNode)
{
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while(pChildNode)
	{
		if(pChildNode->strName == "P" || pChildNode->strName == "p")
		{
			RtwTextBox::SLine* pLine = _Process_Line(pChildNode);
			std::string strAlign = pChildNode->GetProp_s("align");//Î»ÖÃ
			
			if(pLine)
			{
				/*if(strAlign == "center")
				{
					pLine->SlAlign.align=alignCenter;
				}else
				{
					pLine->SlAlign.align=alignNear;
				}*/

				pLine->m_type	= 1;
				pLine->slbottom	= 4;
				pLine->sltop	= 4;
				pLine->slleft	= 4;
				pLine->slright	= 4;

				m_Lines.push_back(*pLine);
			}
		}	
		pChildNode = pChildNode->pNext;
	}
	return true;
}

RtwTextBox::SItem* CUiHtmlText::_Process_ImageItem(RtsXmlDoc::NodePtr *pNode)
{
	m_Itemtmp.Reset();

	if(pNode)
	{
		std::string strSrc			= pNode->GetProp_s("src");
		std::string strWidth		= pNode->GetProp_s("width");
		std::string	strHeight		= pNode->GetProp_s("height");
		std::string strSource		= pNode->GetProp_s("source");
		std::string strHref			= pNode->GetProp_s("href");
		std::string strHightImage	= pNode->GetProp_s("hight");
		std::string strHightRect	= pNode->GetProp_s("hightsource");
		std::string strPushedImage	= pNode->GetProp_s("pushed");
		std::string strPushedRect	= pNode->GetProp_s("pushedsource");


		//.memcpy(m_Itemtmp.text,strText,m_Itemtmp.text.size());
		m_Itemtmp.type = RtwTextBox::eit_Image;
		m_Itemtmp.text = pNode->strName;

		RtwImagePtr pImage, pHightImage, pPushedImage;
		pImage = pHightImage = pPushedImage = NullImagePtr;

		if(!strSrc.empty())
		{
			std::string imagepath = strSrc;
			pImage = g_workspace.GetImageFactory()->CreateImage(imagepath.c_str());
			if (pImage)
			{
				////pImage->SetBlend(true);
				if (!strSource.empty())
				{
					RtwRect rc;
					rc.FromString(strSource);
					pImage->SetTextRect(rc);
				}
			}
		}


		m_Itemtmp.SetImage(pImage, pHightImage, pPushedImage);

		if (!strHref.empty())
		{
			m_Itemtmp.hyperLink.Type = hlinkClient;
			m_Itemtmp.hyperLink.Text =  strHref;
		}
		else
		{
			m_Itemtmp.hyperLink.Text = "";
		}

		SSize tmpSize;
		if (strWidth.size() > 1 && strHeight.size() > 1)
		{
			tmpSize = SSize(atoi(strWidth.c_str()),atoi(strHeight.c_str()));
		}
		else
		{	
			tmpSize = SSize(31, 31);
		}
		m_Itemtmp.rect.ReSize(tmpSize);
		m_Itemtmp.imgSize = tmpSize;
	}
	return &m_Itemtmp;
}

RtwTextBox::SLine* CUiHtmlText::_Process_Line(RtsXmlDoc::NodePtr *pNode)
{
	m_Linetmp.Reset();
	
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while(pChildNode)
	{
 		if(pChildNode->strName == "FONT" || pChildNode->strName == "font" || pChildNode->strName=="a" || pChildNode->strName=="A")
 		{
			RtwTextBox::SItem* pItem = _Process_Item(pChildNode);
			if(pItem)
				m_Linetmp.lineitems.push_back(*pItem);
		}
		else if (pChildNode->strName == "img")
		{
			RtwTextBox::SItem* pItem = _Process_ImageItem(pChildNode);
			if(pItem)
			{
				m_Linetmp.lineitems.push_back(*pItem);
			}
		}
		else
		{
			m_Itemtmp.Reset();
			std::string strText = pChildNode->strName;
			m_Itemtmp.text = strText;
			m_Itemtmp.type = RtwTextBox::eit_Text;
			RtwTextBox::SItem* pItem = &m_Itemtmp;
			if(pItem)
				m_Linetmp.lineitems.push_back(*pItem);     //max
		}
		pChildNode = pChildNode->pNext;
	}
	return &m_Linetmp;
}

RtwTextBox::SItem* CUiHtmlText::_Process_Item(RtsXmlDoc::NodePtr *pNode)
{
	m_Itemtmp.Reset();
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	if( pChildNode && pChildNode->IsItem() )
	{
		std::string strText = pChildNode->GetName();
		std::string strColor     = pNode->GetProp_s("color");
		std::string strBoderColor= pNode->GetProp_s("bordercolor"); //×ÖÌåÃè±ßÑÕÉ«
		std::string strhyperLink = pNode->GetProp_s("href");
		std::string strClass	 = pNode->GetProp_s("class");
		std::string strFontID	= pNode->GetProp_s("TextFont");

		//.memcpy(m_Itemtmp.text,strText,m_Itemtmp.text.size());
		m_Itemtmp.type = RtwTextBox::eit_Text;
		m_Itemtmp.text = strText;
		if (!strFontID.empty())
		{
			m_Itemtmp.nFontID = atoi(strFontID.c_str());
		}

		if(strColor.size() > 1)
		{
			m_Itemtmp.color = strtoul(strColor.c_str() + 1, NULL, 16);
			if (m_Itemtmp.color.a == 0)
			{
				m_Itemtmp.color.a = 255;
			}
			else if (m_Itemtmp.color.a > 255)
			{
				m_Itemtmp.color.a = 255;
			}
		}

		if(strBoderColor.size() > 1)
		{
			m_Itemtmp.bordercolor = strtoul(strBoderColor.c_str() + 1, NULL, 16);
			if (m_Itemtmp.bordercolor.a == 0)
			{
				m_Itemtmp.bordercolor.a = 255;
			}
			else if (m_Itemtmp.bordercolor.a > 255)
			{
				m_Itemtmp.bordercolor.a = 255;
			}
		}

		if (strhyperLink.size() > 1)
		{
			m_Itemtmp.hyperLink.FromString(strhyperLink);

			if (strColor.empty())
			{
				m_Itemtmp.color = 0xaabbcc00;
			}

			m_Itemtmp.bUnderLine = true;
		}
	}
	return &m_Itemtmp;
}