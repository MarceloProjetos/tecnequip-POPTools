#include "poptools.h"
#include "XMLWrapper.h"

#import <msxml6.dll> raw_interfaces_only

void XMLWrapper::Init(void)
{
	bstrErr = NULL;
	pXMLDom = NULL;
	pXMLActiveSelection = NULL;

    VariantInit(&varFileName);
	VariantFromString(L"", varFileName);

	CreateAndInitDOM();
}

void XMLWrapper::DeInit(void)
{
    SAFE_RELEASE(pXMLDom);
    SysFreeString(bstrErr);
    VariantClear(&varFileName);
	CoUninitialize();
}

// Helper function to create a DOM instance. 
void XMLWrapper::CreateAndInitDOM(void)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    hr = CoCreateInstance(__uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pXMLDom));
    if (SUCCEEDED(hr)) {
        // these methods should not fail so don't inspect result
        pXMLDom->put_async           (VARIANT_FALSE);  
        pXMLDom->put_validateOnParse (VARIANT_FALSE);
        pXMLDom->put_resolveExternals(VARIANT_FALSE);
	}
}

void XMLWrapper::VariantFromPChar(char *wszValue, VARIANT &Variant)
{
  int len = strlen(wszValue)+1;
  wchar_t *wText = new wchar_t[len];
  if ( wText == 0 )
    return;
  memset(wText,0,len);
  ::MultiByteToWideChar(  CP_ACP, NULL,wszValue, -1, wText,len );

  //now pass wText
  VariantFromString(wText, Variant);

  // when finish using wText dont forget to delete it
  delete []wText;
}

// Helper function to create a VT_BSTR variant from a null terminated string. 
void XMLWrapper::VariantFromString(PCWSTR wszValue, VARIANT &Variant)
{
	XML_OK();

    BSTR bstr = SysAllocString(wszValue);
    CHK_ALLOC(bstr);

    V_VT(&Variant)   = VT_BSTR;
    V_BSTR(&Variant) = bstr;

CleanUp:
	return;
}

bool XMLWrapper::Open(BSTR filename)
{
	bool ret = FALSE;
    IXMLDOMParseError *pXMLErr = NULL;

    VARIANT_BOOL varStatus;

	XML_OK();

    // XML file name to load
    VariantFromString(filename, varFileName);
	XML_OK();

	CHK_HR(pXMLDom->load(varFileName, &varStatus));
    if (varStatus == VARIANT_TRUE)
    {
		ret = TRUE;
    }
    else
    {
        // Failed to load xml, get last parsing error
        CHK_HR(pXMLDom->get_parseError(&pXMLErr));
        CHK_HR(pXMLErr->get_reason(&bstrErr));

		// Error while loading, not inside parsing
		// Cleanup errors now so we can save it later
		bstrErr = NULL;
		hr = S_OK;
    }

CleanUp:
    SAFE_RELEASE(pXMLErr);

	return ret;
}

bool XMLWrapper::Save(BSTR filename)
{
	XML_OK();

	if(filename != NULL) {
	    // XML file name to save
		VariantFromString(filename, varFileName);
		XML_OK();
	}

	CHK_HR(pXMLDom->save(varFileName));

	return TRUE;

CleanUp:
    VariantClear(&varFileName);

	return FALSE;
}

void XMLWrapper::PrintXML(void)
{
    BSTR bstrXML = NULL;

	XML_OK();

	CHK_HR(pXMLDom->get_xml(&bstrXML));
	printf("%S", bstrXML);

CleanUp:
    SysFreeString(bstrXML);
}

long XMLWrapper::SelectElements(BSTR bstrName, long index)
{
	long			 lCount;
	IXMLDOMNode		*pXMLNode;

	if(pXMLActiveSelection == NULL) {
		CHK_HR(pXMLDom->getElementsByTagName(bstrName, &pXMLActiveSelection));
	} else {
		CHK_HR(pXMLActiveSelection->get_item   (index   , &pXMLNode));
		SAFE_RELEASE(pXMLActiveSelection);
		XML_OK();
		if(pXMLNode == NULL) {
			goto CleanUp;
		}
		CHK_HR(pXMLNode           ->selectNodes(bstrName, &pXMLActiveSelection));
		SAFE_RELEASE(pXMLNode);
		XML_OK();
	}

    // Retrieve total number of nodes in the list.
    CHK_HR(pXMLActiveSelection->get_length(&lCount));
	XML_OK();

	return lCount;

CleanUp:
	return 0;
}

void XMLWrapper::PopulateElementList(IXMLDOMNode *pXMLNode, XMLWrapperElementList *pElementList)
{
	struct XMLWrapperPairList		*pAttrList;
	struct XMLWrapperElementList	*pChildrenList;
	IXMLDOMNamedNodeMap				*pXMLNamedNodeMap;
	IXMLDOMNodeList					*pXMLChildNodeList;
	IXMLDOMNode						*pXMLChildNode;
	IXMLDOMNode						*pXMLAttNode;

	VARIANT					 varNodeValue;
	long					 lAttCount, lChildCount;

	CHK_HR(pXMLNode->get_nodeName  (&(pElementList->element.name)));
	CHK_HR(pXMLNode->get_attributes(&pXMLNamedNodeMap));
	CHK_HR(pXMLNode->get_childNodes(&pXMLChildNodeList));

	// Retrieve total number of nodes in the list.
	if(pXMLNamedNodeMap == NULL) {
		lAttCount = 0;
	} else {
		CHK_HR(pXMLNamedNodeMap ->get_length(&lAttCount  ));
	}

	if(pXMLChildNodeList == NULL) {
		lChildCount = 0;
	} else {
		CHK_HR(pXMLChildNodeList->get_length(&lChildCount));
	}

	if(lAttCount) {
		pAttrList = pElementList->attributes = AllocPairList(lAttCount);
		for(int j = 0; j < lAttCount; j++) {
			CHK_HR(pXMLNamedNodeMap->get_item     (j, &pXMLAttNode ));
			CHK_HR(pXMLAttNode     ->get_nodeName (&pAttrList->name));
			CHK_HR(pXMLAttNode     ->get_nodeValue(   &varNodeValue));
			pAttrList->value = varNodeValue.bstrVal;

			SAFE_RELEASE(pXMLAttNode);
		}
	}

	if(!lChildCount) {
		CHK_HR(pXMLNode->get_nodeValue(&varNodeValue));
		pElementList->element.value = varNodeValue.bstrVal;
	} else {
		pChildrenList = pElementList->children = AllocElementList(lChildCount);
		for(int child = 0; child < lChildCount; child++) {
			CHK_HR(pXMLChildNodeList->get_item(child, &pXMLChildNode));

			PopulateElementList(pXMLChildNode, pChildrenList);
			if(lChildCount == 1 && !wcscmp(L"#text", pChildrenList->element.name)) {
				pElementList->element.value = pChildrenList->element.value;
				FreeElementList(pChildrenList);
				pElementList->children = NULL;
			} else {
				pChildrenList = pChildrenList->next;
			}

			SAFE_RELEASE(pXMLChildNode);
		}
	}

	// Release temporary COM objects.
	SAFE_RELEASE(pXMLChildNodeList);
	SAFE_RELEASE(pXMLNamedNodeMap );

CleanUp:
	return;
}

void XMLWrapper::ChangeElement(int index, BSTR value)
{
	IXMLDOMNode				*pXMLNode = NULL;
	IXMLDOMNodeList			*pXMLChildrenList = NULL;
	long					 lCount;
	DOMNodeType				 Type;
	VARIANT					 varNodeValue;

	// No current selection
	if(pXMLActiveSelection == NULL) {
		return;
	}

    // Retrieve total number of nodes in the list.
    CHK_HR(pXMLActiveSelection->get_length(&lCount));

	// index out of range
	if(index >= lCount || index < 0) {
		return;
	}

	CHK_HR(pXMLActiveSelection->get_item(index, &pXMLNode));

	VariantFromString(value, varNodeValue);

	CHK_HR(pXMLNode->get_childNodes(&pXMLChildrenList));
	SAFE_RELEASE(pXMLNode);

	CHK_HR(pXMLChildrenList->get_item(0, &pXMLNode));
	CHK_HR(pXMLNode->get_nodeType (&Type));
	if(Type == NODE_TEXT) {
		CHK_HR(pXMLNode->put_nodeValue(varNodeValue));
	}

CleanUp:
	SAFE_RELEASE(pXMLNode);
	SAFE_RELEASE(pXMLChildrenList);

	return;
}

void XMLWrapper::DeleteChildren(int index)
{
	IXMLDOMNode				*pXMLNode = NULL, *pXMLChildNode = NULL, *pXMLExcludedNode = NULL;
	IXMLDOMNodeList			*pXMLChildrenList = NULL;
	long					 lCount;

	// No current selection
	if(pXMLActiveSelection == NULL) {
		return;
	}

    // Retrieve total number of nodes in the list.
    CHK_HR(pXMLActiveSelection->get_length(&lCount));

	// index out of range
	if(index >= lCount) {
		return;
	}

	CHK_HR(pXMLActiveSelection->get_item(index, &pXMLNode));

	CHK_HR(pXMLNode->get_childNodes(&pXMLChildrenList));

	while(TRUE) {
		CHK_HR(pXMLChildrenList->get_item(0, &pXMLChildNode));
		if(pXMLChildNode == NULL) // no more items in the list
			break;

		pXMLNode->removeChild(pXMLChildNode, &pXMLExcludedNode);
		SAFE_RELEASE(pXMLChildNode);
		SAFE_RELEASE(pXMLExcludedNode);
	}

CleanUp:
	SAFE_RELEASE(pXMLNode);
	SAFE_RELEASE(pXMLChildNode);
	SAFE_RELEASE(pXMLChildrenList);

	return;
}

void XMLWrapper::AddElement(int index, int pos, char *name, char *value)
{
	IXMLDOMNode				*pXMLNode = NULL, *pXMLNewNode = NULL, *pXMLNewChild = NULL;
	IXMLDOMNodeList			*pXMLChildrenList = NULL;
	BOOL					 isAppend = FALSE;
	VARIANT					 varNodeType, varChild, varNodeValue;
	long					 lCount;

	int len = strlen(name)+1;
	wchar_t *wName = new wchar_t[len], *wValue;
	if ( wName == 0 )
		return;

	memset(wName,0,len);
	::MultiByteToWideChar(  CP_ACP, NULL,name, -1, wName,len );

	len = strlen(value)+1;
	wValue = new wchar_t[len];
	if ( wValue == 0 ) {
		delete []wName;
		return;
	}

	memset(wValue,0,len);
	::MultiByteToWideChar(  CP_ACP, NULL,value, -1, wValue,len );

	// No current selection
	if(pXMLActiveSelection == NULL) {
		goto CleanUp;
	}

    // Retrieve total number of nodes in the list.
    CHK_HR(pXMLActiveSelection->get_length(&lCount));

	// index out of range
	if(index >= lCount)
		goto CleanUp;

	CHK_HR(pXMLActiveSelection->get_item(index, &pXMLNode));
	CHK_HR(pXMLNode->get_childNodes(&pXMLChildrenList));
    // Retrieve total number of nodes in the list.
    CHK_HR(pXMLChildrenList->get_length(&lCount));

	if(pos > lCount) {
		goto CleanUp;
	} else if (pos == lCount || pos < 0) {
		isAppend = TRUE;
	}

	VariantFromString(L"element", varNodeType);
	CHK_HR(pXMLDom->createNode(varNodeType, wName, NULL, &pXMLNewNode));

	VariantInit(&varChild);

	if(isAppend == FALSE) {
		CHK_HR(pXMLChildrenList->get_item(pos, &pXMLNewChild));

		varChild.vt = VT_BYREF;
		varChild.byref = pXMLNewChild;

		SAFE_RELEASE(pXMLNewChild);
	}

	CHK_HR(pXMLNode->insertBefore(pXMLNewNode, varChild, &pXMLNewChild));

	SAFE_RELEASE(pXMLNode);
	pXMLNode = pXMLNewChild;

	VariantFromString(L"text", varNodeType);
	CHK_HR(pXMLDom->createNode(varNodeType, NULL, NULL, &pXMLNewNode));

	VariantFromPChar(value, varNodeValue);
	CHK_HR(pXMLNode->appendChild(pXMLNewNode, &pXMLNewChild));
	CHK_HR(pXMLNewChild->put_nodeValue(varNodeValue));

CleanUp:
	SAFE_RELEASE(pXMLNode);
	SAFE_RELEASE(pXMLNewNode);
	SAFE_RELEASE(pXMLNewChild);
	SAFE_RELEASE(pXMLChildrenList);

	// when finish using wName and wValue dont forget to delete them
	delete []wName;
	delete []wValue;

	return;
}

void XMLWrapper::PrintElementList(XMLWrapperElementList *pElementList)
{/*
	IXMLDOMNamedNodeMap		*pXMLNamedNodeMap;
	IXMLDOMNodeList			*pXMLChildNodeList;
	IXMLDOMNode				*pXMLChildNode;
	IXMLDOMNode				*pXMLAttNode;
	IXMLDOMNode				*pXMLNode;

	VARIANT				 varNodeValue;
	BSTR				 bstrNodeName;
	long				 lCount, lAttCount, lChildCount;

	CHK_HR(pXMLNode    ->get_nodeName  (&bstrNodeName));
	CHK_HR(pXMLNode    ->get_attributes(&pXMLNamedNodeMap));
	// Retrieve total number of nodes in the list.
	CHK_HR(pXMLNamedNodeMap->get_length(&lAttCount));

	printf("%d - %S\n", i, bstrNodeName);

	printf("\tAtributos:\n");
	for(int j = 0; j < lAttCount; j++) {
		CHK_HR(pXMLNamedNodeMap->get_item     (j, &pXMLAttNode ));
		CHK_HR(pXMLAttNode     ->get_nodeName (   &bstrNodeName));
		CHK_HR(pXMLAttNode     ->get_nodeValue(   &varNodeValue));
		printf("\t\t%d - %S = %S\n", j, bstrNodeName, varNodeValue.bstrVal);

		pXMLAttNode->Release();
	}

	CHK_HR(pXMLNode->get_childNodes(&pXMLChildNodeList));

	// Retrieve total number of nodes in the list.
	CHK_HR(pXMLChildNodeList->get_length(&lChildCount));

	printf("\tFilhos:\n");
	for(int child = 0; child < lChildCount; child++) {
		CHK_HR(pXMLChildNodeList->get_item      (child, &pXMLChildNode));
		CHK_HR(pXMLChildNode->get_nodeName      (       &bstrNodeName ));
		CHK_HR(pXMLChildNode->get_nodeTypedValue(       &varNodeValue ));
		printf("\t\t%d - %S = %S\n", child, bstrNodeName, varNodeValue.bstrVal);

		pXMLChildNode->Release();
	}

	// Release temporary COM objects.
	pXMLChildNodeList->Release();
	pXMLNode         ->Release();
	pXMLNamedNodeMap ->Release();*/
}

struct XMLWrapperElementList *XMLWrapper::AllocElementList(long arraysize)
{
	struct XMLWrapperElementList *pElementList, *pCurrentElementList;

	if(!arraysize)
		return NULL;

	pElementList = new XMLWrapperElementList;
	pCurrentElementList = pElementList;

	for(int i=0; i<arraysize; i++) {
		if(i) {
			pCurrentElementList->next = new XMLWrapperElementList;
			pCurrentElementList = pCurrentElementList->next;
		}

		memset((void*)pCurrentElementList, 0, sizeof(struct XMLWrapperElementList));
	}

	return pElementList;
}

void XMLWrapper::FreeElementList(struct XMLWrapperElementList *pElementList)
{
	struct XMLWrapperElementList *pElementListNext;

	while(pElementList != NULL) {
		pElementListNext = pElementList->next;

		FreePairList   (pElementList->attributes);
		FreeElementList(pElementList->children  );
		delete pElementList;

		pElementList = pElementListNext;
	}
}

struct XMLWrapperPairList *XMLWrapper::AllocPairList(long arraysize)
{
	struct XMLWrapperPairList *pPairList, *pCurrentPairList;

	if(!arraysize)
		return NULL;

	pPairList = new XMLWrapperPairList;
	pCurrentPairList = pPairList;

	for(int i=0; i<arraysize; i++) {
		if(i) {
			pCurrentPairList->next = new XMLWrapperPairList;
			pCurrentPairList = pCurrentPairList->next;
		}

		memset((void*)pCurrentPairList, 0, sizeof(struct XMLWrapperPairList));
	}

	return pPairList;
}

void XMLWrapper::FreePairList(struct XMLWrapperPairList *pPairList)
{
	struct XMLWrapperPairList *pPairListNext;

	while(pPairList != NULL) {
		pPairListNext = pPairList->next;
		delete pPairList;
		pPairList = pPairListNext;
	}
}

struct XMLWrapperElementList *XMLWrapper::GetElementList(int index)
{
	XMLWrapperElementList	*pElementList = NULL, **pCurrentElementList;
	IXMLDOMNode				*pXMLNode;
	long					 lCount;

	// No current selection
	if(pXMLActiveSelection == NULL) {
		return NULL;
	}

    // Retrieve total number of nodes in the list.
    CHK_HR(pXMLActiveSelection->get_length(&lCount));

	// index out of range
	if(index >= lCount) {
		return NULL;
	}

	pCurrentElementList = &pElementList;

    // Iterate through the node list and display 
    // the node value of every attribute
    // for every node in the list.
    for (int i = index < 0 ? 0 : index; i < lCount; i++) {
		CHK_HR(pXMLActiveSelection->get_item(i, &pXMLNode));

		*pCurrentElementList = AllocElementList(1);
		PopulateElementList(pXMLNode, *pCurrentElementList);
		pCurrentElementList = &((*pCurrentElementList)->next);

		SAFE_RELEASE(pXMLNode);

		// using index, retrieve only one element
		if(index >= 0) {
			break;
		}
	}

	return pElementList;

CleanUp:
	return NULL;
}
