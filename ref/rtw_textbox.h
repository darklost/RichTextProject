#ifndef _RTUI_EDITBOX_H
#define _RTUI_EDITBOX_H
//********************************************************************
//	created:	2010.04.08 10:00
//	filename: 	rtw_text.cpp
//	author:		ldr123
//	purpose:	�ı������
//  other:		�ֲ��ع����ע��
//********************************************************************

namespace ui 
{
	//�ı���
	class RtwTextBox : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwTextBox, RtwWidget, 0, "ui")

		friend class RtwWidgetFactory;
		friend class CUiWidgetContainer;
		friend class RtwLayer;
		friend class CUiDesktop;
		typedef bool (CheckValid) (string&);
	public:
		enum EEditItemType
		{
			eit_Text,			//�ı�
			eit_Image,			//ͼ��
			eit_Name,			//����
			eit_Enter			//�س�����
		};

		struct SItem
		{
		public:
			SItem*			lastItem;
			SItem*			nextItem;
			EEditItemType	type;			//����
			std::string		text;			//�ı�����	
			RtwPixel		color;			//�ı���ɫ
			RtwPixel		bordercolor;	//�ı������ɫ
			RtwPixel		colorDelta;		//��ɫ��ÿ��������
			SHyperLink		hyperLink;      //����������
			void*           param1;         //����1(ǳ����)
			void*           param2;         //����2(ǳ����)
			long			id;				//�������ID
			int				showcount;		//ѭ������

			bool			bIsRoll;		//�Ƿ�������

			//ldr123 ֧�������Զ���
			int				nFontID;
			bool			bUnderLine;		//�Ƿ��»���
			bool			bItalic;		//�Ƿ�б��
			bool			bBold;			//����

			bool	bDraw;
			RtwRect rect;
			RtwRect rcFrame;
			int		DrawOffset;             // �ӵڼ����ַ���ʼ��ʾ
			int		SelectBegin;            // ѡ�еĿ�ʼ�ַ�
			int		SelectEnd;              // ѡ�еĽ����ַ�
			RtwRect	rcSelect;				// ѡ���ַ�������
			Real	colorChangeOwe[4];		// ��ɫ�������Ƿ
			SSize	imgSize;				//Ҫ��ʾ�Ĵ�С

			//ldr123 ��Ӹ߶ȺͿ�Ⱥ���,����ʵ��ͼƬ�����ɴ�С
			int		getLeft()	{ return rect.left;			}
			int		getTop()	{ return rect.top;			}
			int		getRight()	{ return rect.right;		}
			int		getBottom()	{ return rect.bottom;		}
			int		getHeight()	{ return rect.getHeight();	}
			int		getWidth()	{ return rect.getWidth();	}
			int		getWidthTrue();
			//end

			//����ͼƬ(ǰ��ͼ?)
			void SetImage(const RtwImagePtr& pImage=NullImagePtr, const RtwImagePtr& HightImage=NullImagePtr, const RtwImagePtr& PushedImage=NullImagePtr)
			{
				image = pImage;
				pHoverImage = HightImage;
				pPushedImage = PushedImage;
			}

            void Offset(const SSize& s)
            {
                rect.Offset(s);
            }


			const RtwImagePtr& getImage() { return image; }

			RtwImagePtr			image;
			RtwImagePtr			pPushedImage;
			RtwImagePtr			pHoverImage;
		public:
			SItem()
			{
				Reset();
			}

			~SItem(){}

			SItem(const SItem& Other)
			{
				lastItem	= NULL;
				nextItem	= NULL;
				type		= Other.type;
				text		= Other.text;
				nFontID		= Other.nFontID;

				bUnderLine	= Other.bUnderLine;
				bBold		= Other.bBold;
				bItalic		= Other.bItalic;

				color		= Other.color;
				bordercolor = Other.bordercolor;
				colorDelta	= Other.colorDelta;
				bDraw		= Other.bDraw;
				rect		= Other.rect;
				rcFrame		= Other.rcFrame;
				DrawOffset	= Other.DrawOffset;
				SelectBegin = Other.SelectBegin;
				SelectEnd	= Other.SelectEnd;
				rcSelect	= Other.rcSelect;
				hyperLink	= Other.hyperLink;
				param1		= Other.param1;
				param2		= Other.param2;
				id			= Other.id; //heten
				nFontID		= Other.nFontID;
				imgSize		= Other.imgSize;
                image = Other.image;
                pPushedImage = Other.pPushedImage;
                pHoverImage = Other.pHoverImage;
				bIsRoll		= Other.bIsRoll;
	
				for (int i = 0; i < 4; ++i)
				{
					colorChangeOwe[i] = Other.colorChangeOwe[i];
				}
			}

			//��ֵ������
			const SItem& operator=(const SItem& Other)
			{
				lastItem	= NULL;
				nextItem	= NULL;
				type		= Other.type;
				text		= Other.text;

				bUnderLine	= Other.bUnderLine;
				bBold		= Other.bBold;
				bItalic		= Other.bItalic;

				color		= Other.color;
				bordercolor = Other.bordercolor;
				colorDelta	= Other.colorDelta;
				bDraw		= Other.bDraw;
				rect		= Other.rect;
				rcFrame		= Other.rcFrame;
				DrawOffset	= Other.DrawOffset;
				SelectBegin = Other.SelectBegin;
				SelectEnd	= Other.SelectEnd;
				rcSelect	= Other.rcSelect;
				hyperLink	= Other.hyperLink;
				param1		= Other.param1;
				param2		= Other.param2;
				id			= Other.id; //heten
				nFontID		= Other.nFontID;
				imgSize		= Other.imgSize;
                image = Other.image;
                pPushedImage = Other.pPushedImage;
                pHoverImage = Other.pHoverImage;
				bIsRoll		= Other.bIsRoll;

				for (int i = 0; i < 4; ++i)
				{
					colorChangeOwe[i] = Other.colorChangeOwe[i];
				}

				return *this;
			}

			//��¡
			bool CloneFrom(const SItem* pItem)
			{
				lastItem	= NULL;
				nextItem	= NULL;
				type		= pItem->type;
				text		= pItem->text;

				bUnderLine	= pItem->bUnderLine;
				bBold		= pItem->bBold;
				bItalic		= pItem->bItalic;

				color		= pItem->color;
				bordercolor = pItem->bordercolor;
				colorDelta	= pItem->colorDelta;
				bDraw		= pItem->bDraw;
				rect		= pItem->rect;
				rcFrame		= pItem->rcFrame;
				DrawOffset	= pItem->DrawOffset;
				SelectBegin = pItem->SelectBegin;
				SelectEnd	= pItem->SelectEnd;
				rcSelect	= pItem->rcSelect;
				hyperLink	= pItem->hyperLink;
				param1		= pItem->param1;
				param2		= pItem->param2;
				id			= pItem->id; //heten
				imgSize		= pItem->imgSize;
                image = pItem->image;
                pPushedImage = pItem->pPushedImage;
                pHoverImage = pItem->pHoverImage;
				bIsRoll		= pItem->bIsRoll;

				for (int i = 0; i < 4; ++i)
				{
					colorChangeOwe[i] = pItem->colorChangeOwe[i];
				}

				return true;
			}

			int getColCount();

			void Reset()
			{
				lastItem	= NULL;
				nextItem	= NULL;
				type		= eit_Text;
				text		= "";
				color		= RtwPixel(0xffffffff);
				bordercolor = RtwPixel(0xff194a67);
				bUnderLine	= false;
				bDraw		= false;
				bBold		= false;
				bItalic		= false;
				colorDelta	= DWORD(0);
				param1		= NULL;
				param2		= NULL;
				id			= 0;
				nFontID		= 0;
				bIsRoll		= false;
				SelectBegin	= 0;
				imgSize		= SSize(0, 0);
				lastItem	= NULL;
				nextItem	= NULL;

				for (int i = 0; i < 4; ++i)
					colorChangeOwe[i] = 0;

				hyperLink.Reset();
				SetImage(NullImagePtr);
			}
		};

		struct SLine // SLine���п���û���κ�SItem
		{
			int					height;			// ��һ�еĸ߶�
			std::list<SItem>	lineitems; 
			bool				bWordWrapTail;	// �Ƿ����Զ����������ӳ�����
			int					absHeight;		// ��Ļ�ľ��Ը߶�
			RtwRect				rect;
			int					m_type;			//����0Ϊ��ͨ ��1ΪHintText��
			int					slleft;			//��������ӵľ���
			int					slright;		//���ұ����ӵľ���
			int					slbottom;		//�еײ����ӵľ���
			int					sltop;			//�ж������ӵľ���
			DWORD				dwBuildTime;	//����ʱ��
			int					showCount;		//��������
			RtwRect				imgRect;		//������Ⱦ����
			float				step;			//ÿ����ϱ���ͼ�ƶ��Ĳ���
			float				remainderlStep;

			SLine():height(0),absHeight(0),bWordWrapTail(false),m_type(0),slleft(2),slright(0),slbottom(0),sltop(2),dwBuildTime(0),showCount(3),remainderlStep(0.f),step(0.f){	}

			~SLine(){}

			SLine(const SLine& Other)
			{
				height			= Other.height;
				bWordWrapTail	= Other.bWordWrapTail;
				absHeight		= Other.absHeight;
				rect			= Other.rect;
				m_type			= Other.m_type;
				slleft			= Other.slleft;
				slright			= Other.slright;
				slbottom		= Other.slbottom;
				sltop			= Other.sltop;
				dwBuildTime		= Other.dwBuildTime;
				showCount		= Other.showCount;
				imgRect			= Other.imgRect;
				step			= Other.step;
				remainderlStep		= Other.remainderlStep;

				lineitems.clear();
				lineitems		= Other.lineitems;
			}

			//��ֵ
			const SLine& operator=(const SLine& Other)
			{
				height			= Other.height;
				bWordWrapTail	= Other.bWordWrapTail;
				absHeight		= Other.absHeight;
				rect			= Other.rect;
				m_type			= Other.m_type;
				slleft			= Other.slleft;
				slright			= Other.slright;
				slbottom		= Other.slbottom;
				sltop			= Other.sltop;
				dwBuildTime		= Other.dwBuildTime;
				showCount		= Other.showCount;
				imgRect			= Other.imgRect;
				step			= Other.step;
				remainderlStep		= Other.remainderlStep;

				lineitems.clear();
				lineitems		= Other.lineitems;
				return *this;
			}
			bool CloneFrom(const SLine* pLine)
			{
				height			= pLine->height;
				bWordWrapTail	= pLine->bWordWrapTail;
				absHeight		= pLine->absHeight;
				rect			= pLine->rect;
				m_type			= pLine->m_type;
				slleft			= pLine->slleft;
				slright			= pLine->slright;
				slbottom		= pLine->slbottom;
				sltop			= pLine->sltop;
				dwBuildTime		= pLine->dwBuildTime;
				showCount		= pLine->showCount;
				imgRect			= pLine->imgRect;
				step			= pLine->step;
				remainderlStep		= pLine->remainderlStep;

				lineitems.clear();
				lineitems		= pLine->lineitems;
				return true;
			}

            void Offset(const SSize& s)
            {
                for (list<SItem>::iterator it = lineitems.begin(); it != lineitems.end(); ++it)
                    it->rect.Offset(s);
                rect.Offset(s);
            }

			//ldr123 ��Ӹ߶ȺͿ�Ⱥ���,����ʵ��ͼƬ�����ɴ�С
			int		getLeft() {return rect.left;}
			int		getTop()  {return rect.top;}
			int		getRight(){return rect.right;}
			int		getBottom(){return rect.bottom;}
			int		getHeight(){return rect.getHeight();}
			int		getWidth(){return rect.getWidth();}
			int		getWidthTrue();
			//end

			SItem*	getItem(int index);
			int		getItemIndex(const SItem* pItem);
			int		getColCount();
			SItem*	InsertItem(SItem& item, int index = -1);
			bool	RemoveItem(int index);
			bool	RemoveItem(SItem* pItem);

			void	Reset()
			{
				height	= 0;
				rect	= RtwRect(0, 0, 0, 0);
				bWordWrapTail = false;
				imgRect = RtwRect(0, 0, 0, 0);
				lineitems.clear();
				step = 0.f;
			}
		};

	public :
        enum { WidgetType = wtEditBox };
		RtwTextBox();
		virtual ~RtwTextBox();
		virtual void Destroy();
	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);
		virtual void OnMove(const RtwRect& OrigFrameRect);
		virtual void OnOffset(const SSize& size);
        virtual void SetScrollPosition(int pos);

		//��ǰλ������ı�
		void AddHyperLinkText(const std::string &text,const std::string &link, RtwPixel color = 0xffffffff);
		void AddHyperLinkText(const std::string &text, const SHyperLink &hyperLink, RtwPixel color = 0xffffffff);

		void AddText(const std::string &text,  RtwPixel color = 0xffffffff);
		void AddColorText(string &vrAddText, RtwPixel color = 0xffffffff);
		//end

        virtual void ClearText();
		virtual void SetText(const std::string& text);
		void SetTextnew(const std::string& text);
		void SetRichText(const std::string& richtext);
		void SetHtmlText(const std::string& htmltext, bool bWordWrap=true, bool bMultiLine=true);
		bool LoadHtmlFile(const std::string& htmlFile);

		//����
		void SetMultiLine(bool bMultiLine)
		{
			m_bMultiLine = bMultiLine;

			if (!m_bMultiLine)
				m_bWordWrap = false;
		}

		//������
		void SetEnableInput(bool bEnableInput)
		{
			m_bEnableInput = bEnableInput;
		}

		void SetEnableRMenu(bool bEnable = true)
		{
			m_bEnableMenu = bEnable;
		}

		//����(��һ����ʾ����һ������ʱ,��������ƶ�����һ��)
		void SetWordWrap(bool bWordWrap, bool bRefresh = true)
		{
			m_bWordWrap = bWordWrap;

			if (bRefresh)
				Refresh();
		}

// 		//�Զ����������
// 		void SetAutoScrollV(bool bAutoScrollV, bool bRefresh = true)
// 		{
// 			RtwWidget::SetAutoScrollV(bAutoScrollV);
// 			m_bAutoScrollV = bAutoScrollV;
// 			if (m_bAutoScrollV)
// 			{
// 				m_pAutoScrollV->Show();
// 			}
// 			else 
// 				m_pAutoScrollV->Hide();
// 			if (bRefresh)
// 				Refresh();
// 		}

		//��������
		void SetPassword(bool bPassword)
		{
			m_bPassword = bPassword;
		}

		//�����Ƿ�ֻ��������
		void SetNumeric(bool bNumeric, bool bMatheMatics = true)
		{
			m_bNumeric		= bNumeric;
			m_bMatheMatics	= bMatheMatics; //������
		}

		//��������(��ʾ����)
		void SetCapacity(int capacity)
		{
			m_Capacity = capacity;
		}

		//��������ȣ����ں����Զ�����ʱ���ã�hint���ʹ�ã�
		//ʹ����һ����Χ���Զ�������ȣ�ʹ����ָ����ۣ�Ĭ��Ϊ0���������������
		void SetMaxWidth(int nWidth)
		{
			m_nMaxWidth = nWidth;
		}
		//��ָ���ַ�������ʾ������
		void SetPasswordChar(char ShowChar)
		{
			if (ShowChar)
				m_Password_ShowChar = ShowChar;
		}

		void SetAutoSizeV(bool bAutoSizeV) { m_bAutoSizeV = bAutoSizeV; }
		void SetAutoSizeH(bool bAutoSizeH) { m_bAutoSizeH = bAutoSizeH; }

		void SelectAll(); // ѡȡ����
		void RemoveSelect(); // ɾ��ѡȡ

		void ScrollToTop();
		void ScrollToBottom();
		int GetHyperLinkNumber();	//��ȡ����������
		virtual std::string getText() const;
		SLine* getLine(int index);

        SLine* RtwTextBox::getFirstLine()
        {
            if (m_Lines.empty())
                return NULL;
            return &m_Lines.front();
        }

        SLine* RtwTextBox::getLastLine()
        {
            if (m_Lines.empty())
                return NULL;
            return &m_Lines.back();
        }

		int getLineCount()		{ return (int)m_Lines.size(); }
		bool getEnableInput()	{ return m_bEnableInput; }
		bool getMultiLine()		{ return m_bMultiLine; }
		bool getWordWrap()		{ return m_bWordWrap; }
		int getCursorLine()		{ return m_CursorPosLine; }
		int getCursorCol()		{ return m_CursorPosCol; }
		RtwRect getRect()		{return m_ViewRect;}
		RtwVScrollBar* getScrollBarV() { return m_pScrollBarV; }
		std::string getSelectText();
		bool getAutoSizeV()		{ return m_bAutoSizeV; }
		bool getAutoSizeH()		{ return m_bAutoSizeH; }
		virtual void SetAutoScrollV(bool bAutoScrollV);
		virtual void SetAutoScrollH(bool bAutoScrollH);
		virtual int getColCount();

		void AutoSize(bool bV, bool bH);

		void SetCursorColor(RtwPixel color){m_CursorColor = color;}
		bool LoadFromFile(const char* szFileName);
		void MergeAllLine();

	public:
		bool AddItem(SLine& line, int indexBefore = -1, bool bRefreshItemPos = true);
		bool RemoveAllItems(bool bRefreshItemPos = true);

		bool InsertLine(SLine& line, int indexBefore = -1);
		bool RemoveLine(int line,bool bDown = true);
        bool RemoveFristLine();
		bool RemoveAllLines();

		bool InsertCharacterToCurrentCursor(const char character);
		bool InsertStringToCurrentCursor(const std::string& str, bool bEnter=true);
		bool InsertItemToCurrentCursor(SItem& item);
		bool GetLinkItem(const string &strLink, SItem &itemLink);

		int  GetStringNumFromWidth(int nFontID, string str, int nWidth);

		int getLineIndex(SLine* pLine);
		int getItemIndex(SLine* pLine, SItem* pItem);
		SItem* getNextItem(SLine* pLine, SItem* pItem);

		bool PickItem(const SPoint& point, int& outLine, int& outCol, int& outItemIndex, bool& bDoubleByte, bool& outNear, bool& bhlink); // ��һ������ѡȡ�С��С�Itemλ�á��Ƿ�˫�ֽ��ַ�����������
		SItem* PickItem(const SPoint& point);
		SItem* GetLinkItemByText(const char* szText);

		void CalcLinePosition(int LineIndex, const SPoint& StartPoint);
		void CalcLinePosition(int LineIndex);
		void RefreshPosition();
		void OffsetItemPosition(const SSize& Delta, int LineFrom = 0);
		void RefreshCursorPosition();
		void RefreshSelectionRect();
		void RefreshScrollBarVData();

		void AutoAdjectCursor();                // �Զ�������꣬ʹ���ɼ�
		void SetCursorPos(int line, int col);   // ���ù��λ��
		void SetLeftMargin(int margin){m_LeftMargin = margin;}
		void SetRightMargin(int margin){m_RightMargin = margin;}
		void SetBottomMargin(int margin){m_BottomMargin = margin;}
		void SetTopMargin(int margin){m_TopMargin = margin;}
		int GetLeftMargin(){return m_LeftMargin;}
		int GetRightMargin(){return m_RightMargin;}
		int GetBottomMargin(){return m_BottomMargin;}
		int GetTopMargin(){return m_TopMargin;}
		void SetNumberOnly(bool bNum);
		void SetAlphaNumberOnly(bool bAlphaNum){}
		void SetCapsLock(int lock){}
		void InsertImage(const RtwImagePtr& image, const string& name);
		void SetShowCount(int nCount);
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfFocus | wfInput | wfMouseMove;
		static const int m_LeftSpace = 0;
		static const int m_LineSpace = 1;
		RtwRect GetCursorRect()const {return m_CursorRect;}

		bool CanPenetrate()const {return m_bCanPenetrate;}
	protected:
		virtual void CalcClient();      // ����m_rcFrameRect����m_rcClientRect
		virtual void _ResetSelf();
		bool _Intercept_Enter(int line, int col);
		int GetStringWidth(int nFontID, string strString, bool bPass);
		int GetStringWidth(int nFontID, string strBegin, int number=0, bool bPass=false);
	private:
		bool _DivideTextItem(SItem& ItemToDivide, int DivideIndexFrom, SItem& outNewItem);
		bool _MergeNeighborItem(SItem* pItem1, SItem* pItem2);  // �ϲ�����Item(�����ºϲ����Item����)
		bool _MergeWrapedLine(int line);                        // ��line�к���Զ����ж��ϲ���line��(�����ºϲ����Item����)

		bool _GetItemByLineCol(int line, int col, SItem** ppItem, int* pTextItemIndex, SItem** ppItemBetweenLeft, SItem** ppItemBetweenRight);

		bool _PrepareEnterOk(bool bChinese=false); //�����Ƿ���Ҫ����
		SPoint _CalcPoints(int line, int col, int lineStart = 0, int colStart = 0); // 
		void _CalcPoints();              // �����ꡢѡ�����ֵȵ���Ļλ��
		bool _MergeItem(SItem* pItem1, SItem* pItem2);


		// �س����к��Զ����еĴ���
		bool _Intercept(int line, int col, bool bWordWrap); // ��line��col�к�ָ�����������뵽��һ�У�
		bool _Intercept(int line, int itemIndex, int itemCol, bool bWordWrap); // ��line��col�к�ָ�����������뵽��һ�У�
		bool _Intercept(int line, int col); // ��line��col�к�ָ����

		bool _MoveTailItemToNextLine(int LineIndex, int ItemIndexFrom,int top=0 ,int bottom=0);
		
		void _SetSelectionStart(int line, int col);
		void _SetSelectionEnd(int line, int col);
		void _getSelectionStartEnd(int& lineStart, int& colStart, int& lineEnd, int& colEnd);
	public:
		//�����������
		//begin
		inline void SetTextLineNum(int nLineNum) {if (nLineNum >0)m_nLineNum = nLineNum;}
		inline void SetNameColorChange(bool bChange=true){m_bNameColorChange=bChange;}

        list<SLine>* getLineContainer() { return &m_Lines; }
		//end

		void EnableRoll(bool bEnable);
		void EnableGameClickEvent(bool bEnable){m_bUseGameClickEvent = bEnable;}
		void SetDefaultText(string str){m_strDefaultText = str;}
	private:
		void OnScroll_ScrollBarV(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		void OnRMenu(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnHideMenu(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseRDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnKeyDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnKeyChar_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnLClick_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnEvHyperLink(RtwWidget* pWidget, RtwEventDelegate* pEvent);


		void _OnCopy();
		void _OnCut();
		void _OnPaste();
		void _OnSelectAll();
	protected:
        bool m_bEnableMenu  : 1;				// �Ҽ����ܲ˵��Ƿ���
        bool m_bEnableInput : 1;            // �Ƿ���������
        bool m_bWordWrap    : 1;               // �Ƿ��Զ�����
        bool m_bMultiLine   : 1;              // �Ƿ���Զ���
        bool m_bPassword    : 1;               // �Ƿ�Ϊ������ʾ��ʽ
        bool m_bNumeric     : 1;                // �Ƿ�ֻ����������
        bool m_bMatheMatics : 1;			// �������븺��
        bool m_addface      : 1;					//�Ƿ�������
        bool m_bAutoSizeV   : 1;              // �Ƿ������Widget����Ӧ��С(V)
        bool m_bAutoSizeH   : 1;              // �Ƿ������Widget����Ӧ��С(H)
        bool m_bNameColorChange : 1;		//���ֱ�ɫ
        bool m_bCanPenetrate    : 1;			//��ǰ�Ƿ���Դ�͸
        bool m_bChatCheckValid  : 1;
        bool m_bCheckValid      : 1;
		bool m_bIsRoll			: 1;
		bool m_bUseGameClickEvent:1;

		RtwVScrollBar*	m_pScrollBarV;			// ��ֱ������
		RtwPopupMenu*	m_pRMenu;
        int  m_Capacity;                // ����������ַ���
		int  m_nMaxWidth;
		int m_nShowCount;
		char m_Password_ShowChar;       // ���뷽ʽ��ʾʱ���ַ�
 
		std::list<SLine> m_Lines;       // ����

		RtwPixel m_CursorColor;         // ������ɫ

		int m_ViewOffsetLine;           // ��ʾ��ƫ����(��)
		int m_ViewOffsetCol;            // ��ʾ��ƫ����(��)
		int m_ViewLastLine;             // ��ʾ�����һ��
		int m_CursorPosLine;            // ����λ��(��)
		int m_CursorPosCol;             // ����λ��(��)
		int m_CursorPosColBak;          // ����λ��(��)(���ڴ�������ʵ����λ��)

		int m_SelStartLine;             // ѡ�����ֵĿ�ʼ(��)
		int m_SelStartCol;              // ѡ�����ֵĿ�ʼ(��)
		int m_SelEndLine;               // ѡ�����ֵĽ���(��)
		int m_SelEndCol;                // ѡ�����ֵĽ���(��)
		RtwRect m_ViewRect;             // ��ʾ������
		RtwRect m_CursorRect;           // ��������


		CUiRichText* m_pRichTextLoader;   // RichText��Loader
		CUiHtmlText* m_pHtmlTextLoader;   // HtmlText��Loader

		int m_LeftMargin;
		int m_RightMargin;
		int m_BottomMargin;
		int m_TopMargin;
		char m_acDoubleByte[2];

		std::string m_SelText;          // ѡ������
		std::string m_strDefaultText;
		bool		m_bShowDefaultText;

        

		//ldr123
		//�����ı�����
		int	m_nLineNum;
	public:
		RtwEventDispatcher EvUpdateText;
		RtwEventDispatcher EvInputChar;
		RtwEventDispatcher  EvHyperLink;

		static RtwEventDispatcher EvGameClickEvent;
	public:
		//Υ���ַ�
		static CheckValid*	m_pChatCheckValidFunc;
		static CheckValid*	m_pCheckValidFunc;

		void EnableCheckValid(bool bChatCheck=true, bool bCheck=true)
		{
			m_bCheckValid		= bCheck;
			m_bChatCheckValid	= bChatCheck;
		}

		void CheckValidString(string &msg)
		{
			if (m_bChatCheckValid && RtwTextBox::m_pChatCheckValidFunc)
			{
				if (!(*RtwTextBox::m_pChatCheckValidFunc)(msg))
				{
					return;
				}
			}

			if (m_bCheckValid && RtwTextBox::m_pCheckValidFunc)
			{
				if (!(*RtwTextBox::m_pCheckValidFunc)(msg))
				{
					return;
				}
			}
		}
	};
} 
#endif 
