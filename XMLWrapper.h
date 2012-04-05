#ifndef XMLWRAPPER_H
#define XMLWRAPPER_H

// Macro that calls a COM method returning HRESULT value.
#define CHK_HR(stmt) do { hr=(stmt); if (FAILED(hr)) goto CleanUp; } while(0)
#define XML_OK()     do { if (bstrErr != NULL || FAILED(hr)) goto CleanUp; } while(0)

// Macro to verify memory allcation.
#define CHK_ALLOC(p)        do { if (!(p)) { hr = E_OUTOFMEMORY; goto CleanUp; } } while(0)

// Macro that releases a COM object if not NULL.
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)

struct XMLWrapperPairList
{
	BSTR name;
	BSTR value;
	struct XMLWrapperPairList *next;
};

struct XMLWrapperElementList
{
	struct XMLWrapperPairList		 element;
	struct XMLWrapperPairList		*attributes;
	struct XMLWrapperElementList	*children;
	struct XMLWrapperElementList	*next;
};

class XMLWrapper
{
private:
    BSTR             bstrErr;
	HRESULT          hr;
    VARIANT			 varFileName;
	IXMLDOMDocument	*pXMLDom;
	IXMLDOMNodeList	*pXMLActiveSelection;

	void							 Init					(void);
	void							 DeInit					(void);
	void							 FreePairList			(struct XMLWrapperPairList *pPairList);
	struct XMLWrapperPairList		*AllocPairList			(long arraysize);
	struct XMLWrapperElementList	*AllocElementList		(long arraysize);
	void							 CreateAndInitDOM		(void);
	void							 VariantFromPChar		(char   *wszValue, VARIANT &Variant);
	void							 VariantFromString		(PCWSTR  wszValue, VARIANT &Variant);
	void							 PopulateElementList	(IXMLDOMNode *pXMLNode, XMLWrapperElementList *pElementList);

public:
	// Constructors
	XMLWrapper(void)  { Init();   }

	// Destructors
	~XMLWrapper(void) { DeInit(); }

	// Functions
	bool							 Open					(BSTR filename);
	bool							 Save					(BSTR filename);
	void							 PrintXML				(void);
	BSTR							 GetError				(void) { return bstrErr; }
	long							 SelectElements			(BSTR name, long index);
	void							 ClearActiveSelection	(void) { SAFE_RELEASE(pXMLActiveSelection); }
	struct XMLWrapperElementList	*GetElementList			(int index);
	void							 FreeElementList		(struct XMLWrapperElementList *pElementList);
	void							 PrintElementList		(XMLWrapperElementList *pElementList);
	void							 ChangeElement			(int index, BSTR value);
	void							 DeleteChildren			(int index);
	void							 AddElement				(int index, int pos, char *name, char *value);
};

#endif