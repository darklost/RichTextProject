#ifndef _RTUI_EDITBOX_H
#define _RTUI_EDITBOX_H
//********************************************************************
//	created:	2010.04.08 10:00
//	filename: 	rtw_text.cpp
//	author:		ldr123
//	purpose:	文本输入框
//  other:		局部重构添加注释
//********************************************************************

namespace ui 
{
	//文本框
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
			eit_Text,			//文本
			eit_Image,			//图像
			eit_Name,			//名称
			eit_Enter			//回车符号
		};

		struct SItem
		{
		public:
			SItem*			lastItem;
			SItem*			nextItem;
			EEditItemType	type;			//类型
			std::string		text;			//文本内容	
			RtwPixel		color;			//文本颜色
			RtwPixel		bordercolor;	//文本描边颜色
			RtwPixel		colorDelta;		//颜色的每秒钟增量
			SHyperLink		hyperLink;      //超链接属性
			void*           param1;         //参数1(浅复制)
			void*           param2;         //参数2(浅复制)
			long			id;				//保存玩家ID
			int				showcount;		//循环次数

			bool			bIsRoll;		//是否参与滚动

			//ldr123 支持字体自定义
			int				nFontID;
			bool			bUnderLine;		//是否下划线
			bool			bItalic;		//是否斜体
			bool			bBold;			//粗体

			bool	bDraw;
			RtwRect rect;
			RtwRect rcFrame;
			int		DrawOffset;             // 从第几个字符开始显示
			int		SelectBegin;            // 选中的开始字符
			int		SelectEnd;              // 选中的结束字符
			RtwRect	rcSelect;				// 选中字符的区域
			Real	colorChangeOwe[4];		// 颜色渐变的赊欠
			SSize	imgSize;				//要显示的大小

			//ldr123 添加高度和宽度函数,用以实现图片的自由大小
			int		getLeft()	{ return rect.left;			}
			int		getTop()	{ return rect.top;			}
			int		getRight()	{ return rect.right;		}
			int		getBottom()	{ return rect.bottom;		}
			int		getHeight()	{ return rect.getHeight();	}
			int		getWidth()	{ return rect.getWidth();	}
			int		getWidthTrue();
			//end

			//设置图片(前景图?)
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

			//赋值操作符
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

			//克隆
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

		struct SLine // SLine中有可能没有任何SItem
		{
			int					height;			// 这一行的高度
			std::list<SItem>	lineitems; 
			bool				bWordWrapTail;	// 是否是自动换行所增加出来的
			int					absHeight;		// 屏幕的绝对高度
			RtwRect				rect;
			int					m_type;			//类型0为普通 ，1为HintText框
			int					slleft;			//行左边增加的距离
			int					slright;		//行右边增加的距离
			int					slbottom;		//行底部增加的距离
			int					sltop;			//行顶部增加的距离
			DWORD				dwBuildTime;	//生成时间
			int					showCount;		//滚动次数
			RtwRect				imgRect;		//背景渲染矩形
			float				step;			//每次配合背景图移动的步长
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

			//赋值
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

			//ldr123 添加高度和宽度函数,用以实现图片的自由大小
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

		//当前位置添加文本
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

		//多行
		void SetMultiLine(bool bMultiLine)
		{
			m_bMultiLine = bMultiLine;

			if (!m_bMultiLine)
				m_bWordWrap = false;
		}

		//可输入
		void SetEnableInput(bool bEnableInput)
		{
			m_bEnableInput = bEnableInput;
		}

		void SetEnableRMenu(bool bEnable = true)
		{
			m_bEnableMenu = bEnable;
		}

		//短字(当一行显示不了一个单词时,这个单词移动到下一行)
		void SetWordWrap(bool bWordWrap, bool bRefresh = true)
		{
			m_bWordWrap = bWordWrap;

			if (bRefresh)
				Refresh();
		}

// 		//自动纵向滚动条
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

		//设置密码
		void SetPassword(bool bPassword)
		{
			m_bPassword = bPassword;
		}

		//设置是否只输入数字
		void SetNumeric(bool bNumeric, bool bMatheMatics = true)
		{
			m_bNumeric		= bNumeric;
			m_bMatheMatics	= bMatheMatics; //允许负数
		}

		//设置能力(显示个数)
		void SetCapacity(int capacity)
		{
			m_Capacity = capacity;
		}

		//设置最大宽度，仅在横向自动调整时有用，hint相关使用，
		//使其在一定范围内自动调整宽度，使其表现更美观，默认为0，即不做宽度设置
		void SetMaxWidth(int nWidth)
		{
			m_nMaxWidth = nWidth;
		}
		//以指定字符代替显示的文字
		void SetPasswordChar(char ShowChar)
		{
			if (ShowChar)
				m_Password_ShowChar = ShowChar;
		}

		void SetAutoSizeV(bool bAutoSizeV) { m_bAutoSizeV = bAutoSizeV; }
		void SetAutoSizeH(bool bAutoSizeH) { m_bAutoSizeH = bAutoSizeH; }

		void SelectAll(); // 选取所有
		void RemoveSelect(); // 删除选取

		void ScrollToTop();
		void ScrollToBottom();
		int GetHyperLinkNumber();	//获取超链接数量
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

		bool PickItem(const SPoint& point, int& outLine, int& outCol, int& outItemIndex, bool& bDoubleByte, bool& outNear, bool& bhlink); // 从一个点来选取行、列、Item位置、是否双字节字符、靠近左右
		SItem* PickItem(const SPoint& point);
		SItem* GetLinkItemByText(const char* szText);

		void CalcLinePosition(int LineIndex, const SPoint& StartPoint);
		void CalcLinePosition(int LineIndex);
		void RefreshPosition();
		void OffsetItemPosition(const SSize& Delta, int LineFrom = 0);
		void RefreshCursorPosition();
		void RefreshSelectionRect();
		void RefreshScrollBarVData();

		void AutoAdjectCursor();                // 自动调整光标，使光标可见
		void SetCursorPos(int line, int col);   // 设置光标位置
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
		virtual void CalcClient();      // 根据m_rcFrameRect计算m_rcClientRect
		virtual void _ResetSelf();
		bool _Intercept_Enter(int line, int col);
		int GetStringWidth(int nFontID, string strString, bool bPass);
		int GetStringWidth(int nFontID, string strBegin, int number=0, bool bPass=false);
	private:
		bool _DivideTextItem(SItem& ItemToDivide, int DivideIndexFrom, SItem& outNewItem);
		bool _MergeNeighborItem(SItem* pItem1, SItem* pItem2);  // 合并两个Item(不更新合并后的Item区域)
		bool _MergeWrapedLine(int line);                        // 把line行后的自动换行都合并到line中(不更新合并后的Item区域)

		bool _GetItemByLineCol(int line, int col, SItem** ppItem, int* pTextItemIndex, SItem** ppItemBetweenLeft, SItem** ppItemBetweenRight);

		bool _PrepareEnterOk(bool bChinese=false); //计算是否需要换行
		SPoint _CalcPoints(int line, int col, int lineStart = 0, int colStart = 0); // 
		void _CalcPoints();              // 计算光标、选中文字等的屏幕位置
		bool _MergeItem(SItem* pItem1, SItem* pItem2);


		// 回车换行和自动换行的处理
		bool _Intercept(int line, int col, bool bWordWrap); // 把line行col列后分割出来（，插入到下一行）
		bool _Intercept(int line, int itemIndex, int itemCol, bool bWordWrap); // 把line行col列后分割出来（，插入到下一行）
		bool _Intercept(int line, int col); // 把line行col列后分割出来

		bool _MoveTailItemToNextLine(int LineIndex, int ItemIndexFrom,int top=0 ,int bottom=0);
		
		void _SetSelectionStart(int line, int col);
		void _SetSelectionEnd(int line, int col);
		void _getSelectionStartEnd(int& lineStart, int& colStart, int& lineEnd, int& colEnd);
	public:
		//设置最大行数
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
        bool m_bEnableMenu  : 1;				// 右键功能菜单是否开启
        bool m_bEnableInput : 1;            // 是否允许输入
        bool m_bWordWrap    : 1;               // 是否自动换行
        bool m_bMultiLine   : 1;              // 是否可以多行
        bool m_bPassword    : 1;               // 是否为密码显示方式
        bool m_bNumeric     : 1;                // 是否只能输入数字
        bool m_bMatheMatics : 1;			// 可以输入负数
        bool m_addface      : 1;					//是否加入表情
        bool m_bAutoSizeV   : 1;              // 是否根据子Widget来适应大小(V)
        bool m_bAutoSizeH   : 1;              // 是否根据子Widget来适应大小(H)
        bool m_bNameColorChange : 1;		//名字变色
        bool m_bCanPenetrate    : 1;			//当前是否可以穿透
        bool m_bChatCheckValid  : 1;
        bool m_bCheckValid      : 1;
		bool m_bIsRoll			: 1;
		bool m_bUseGameClickEvent:1;

		RtwVScrollBar*	m_pScrollBarV;			// 垂直滚动条
		RtwPopupMenu*	m_pRMenu;
        int  m_Capacity;                // 限制输入的字符数
		int  m_nMaxWidth;
		int m_nShowCount;
		char m_Password_ShowChar;       // 密码方式显示时的字符
 
		std::list<SLine> m_Lines;       // 内容

		RtwPixel m_CursorColor;         // 光标的颜色

		int m_ViewOffsetLine;           // 显示的偏移量(行)
		int m_ViewOffsetCol;            // 显示的偏移量(列)
		int m_ViewLastLine;             // 显示的最后一行
		int m_CursorPosLine;            // 光标的位置(行)
		int m_CursorPosCol;             // 光标的位置(列)
		int m_CursorPosColBak;          // 光标的位置(列)(用于储存光标真实的列位置)

		int m_SelStartLine;             // 选中文字的开始(行)
		int m_SelStartCol;              // 选中文字的开始(列)
		int m_SelEndLine;               // 选中文字的结束(行)
		int m_SelEndCol;                // 选中文字的结束(列)
		RtwRect m_ViewRect;             // 显示的区域
		RtwRect m_CursorRect;           // 光标的区域


		CUiRichText* m_pRichTextLoader;   // RichText的Loader
		CUiHtmlText* m_pHtmlTextLoader;   // HtmlText的Loader

		int m_LeftMargin;
		int m_RightMargin;
		int m_BottomMargin;
		int m_TopMargin;
		char m_acDoubleByte[2];

		std::string m_SelText;          // 选中文字
		std::string m_strDefaultText;
		bool		m_bShowDefaultText;

        

		//ldr123
		//设置文本行数
		int	m_nLineNum;
	public:
		RtwEventDispatcher EvUpdateText;
		RtwEventDispatcher EvInputChar;
		RtwEventDispatcher  EvHyperLink;

		static RtwEventDispatcher EvGameClickEvent;
	public:
		//违禁字符
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
