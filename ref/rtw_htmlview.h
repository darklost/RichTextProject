#ifndef _HTML_VIEW_H
#define _HTML_VIEW_H
namespace ui
{
	class RtwHtmlView : public RtwTextBox
	{
		RT_DECLARE_DYNCREATE(RtwHtmlView, RtwTextBox, 0, "ui")
	public:
        enum { WidgetType = wtHtmlView };

		RtwHtmlView();
		~RtwHtmlView();
		typedef bool (CBTranslate) (string&, const char*, void*);
	public:

		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);
	public:
		void Reset();
		bool LoadFromFile(const std::string& FileName);
		bool LoadFromString(const std::string& strHtml);

		bool SetKeyValue(const std::string& strKey, const std::string& strValue);
		void Reload();
		RtsXmlDoc* getXmlDoc() {return m_pXmlDoc;}

		void SetText(const std::string text);
		std::string GetPath(){ return m_path; }
		void AddPath(char* path);
		void ClearPath(){m_path = "";}
	protected:
		RtsXmlDoc* m_pXmlDoc;
		RtwTextBox::SItem m_Itemtmp;
		RtwTextBox::SLine m_Linetmp;
	private:
		bool _Process_UiHtmlText(RtsXmlDoc::NodePtr* pNode);
		bool _Process_Lines(RtsXmlDoc::NodePtr* pNode);
		bool _Process_Image(RtsXmlDoc::NodePtr* pNode);
		bool _ProcessNode(RtsXmlDoc::NodePtr* pNode);
		void _Process_Prize(RtsXmlDoc::NodePtr* pNode);

		RtwTextBox::SItem* _Process_Item(RtsXmlDoc::NodePtr *pNode);
		RtwTextBox::SItem* _Process_ChildItem(RtsXmlDoc::NodePtr *pNode);
		RtwTextBox::SItem* _Process_ImageItem(RtsXmlDoc::NodePtr *pNode);
		RtwTextBox::SLine* _Process_Line(RtsXmlDoc::NodePtr *pNode);
		void ReplaceString(std::string& replace);
		std::string ReadFileToString(const std::string& FileName);
	protected:
	private:
		std::string m_path;
		std::string m_strHtmlString;
	public:
		static		void SetTranslateText(CBTranslate* cb);
		static		CBTranslate* ms_cbTranslateFunc;

		RtwEventDispatcher EvAction;
	};
}
#endif