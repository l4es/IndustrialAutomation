#include "StdAfx.h"
#include ".\channelsview.h"
#include "MainFrm.h"

inline int GetDigitalPos(CString &str)
{
	for(int i=0;i<str.GetLength();i++)
		if(isdigit(static_cast<unsigned char>(str[i])))
			return i;
	return -1;
}

inline int GetDigit(int start, CString &str)
{
	CString tmp;
	int i=start;
	while(isdigit(static_cast<unsigned char>(str[i])) && i<str.GetLength())
		tmp += str[i++];
	return _wtoi(tmp);
};

//Сравнение пунктов (для сортировки)
int CALLBACK CompareChannelItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListViewCtrl List((HWND)lParamSort);

	//Если пункты содержат цифры, сортируем с учетом их
	CString Name1,Name2;
	List.GetItemText(lParam1, 0, Name1);
	List.GetItemText(lParam2, 0, Name2);

	const int	pos1 = GetDigitalPos(Name1),
		pos2 = GetDigitalPos(Name2);
	if(pos1>=0 && pos2 >=0 && pos1 == pos2)
	{
		if(Name1.Mid(0,pos1) != Name2.Mid(0,pos2))
			return lstrcmpi(Name1, Name2);

		const int	digit1 = GetDigit(pos1, Name1),
					digit2 = GetDigit(pos2, Name2);
		if(digit1 > digit2)
			return 1;
		else
		{
			if(digit1 == digit2)
				return 0;
			else
				return -1;
		}
	}
	else
		return  lstrcmpi(Name1, Name2);
}

LRESULT CChannelsView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES );

	bHandled = FALSE;
	return 0;
};

void CChannelsView::OnInitialUpdate()
{
	InsertColumn(0, _T("Каналы"),	LVCFMT_LEFT, -1, -1);

	DeleteAllItems();
	CDocument* pDoc = (CDocument*)GetDocument();
	CTagDB::TVChannel vec = pDoc->m_DB.GetHumanReadableChannelsList();

	for(CTagDB::TVChannel::iterator i=vec.begin();i!=vec.end();i++)
	{
		int item = InsertItem(i-vec.begin(),i->Name);
		SetItemData(item, i->id);
	}
	
	SortItemsEx(CompareChannelItems, (LPARAM)m_hWnd);
}

void CChannelsView::OnUpdate()
{
}

LRESULT CChannelsView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetColumnWidth(0,LVSCW_AUTOSIZE_USEHEADER);

	return 0;
}

LRESULT CChannelsView::OnLVSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	//Если изменилось состояние галочки, вносим коррективы в БД
	LPNMLISTVIEW lpNLV = (LPNMLISTVIEW)pnmh;

	long ns, os;
	ns = (lpNLV->uNewState & LVIS_STATEIMAGEMASK) >> 12; // new state
	os = (lpNLV->uOldState & LVIS_STATEIMAGEMASK) >> 12; // old state
	if (os != ns)
	{
		//Помещаем все отмеченые каналы в список

		CDocument* pDoc = (CDocument*)GetDocument();
		pDoc->m_GraphChannels.clear();
		for(int i=0;i<GetItemCount();i++)
		{
			long state = (GetItemState(i, LVIS_STATEIMAGEMASK) & LVIS_STATEIMAGEMASK) >> 12;
			if(state == 2)
			{
				CTagDB::sChannel chnl;
				GetItemText(i,0,chnl.Name);
				chnl.id = GetItemData(i);
				pDoc->m_GraphChannels.push_back(chnl);
			}
		};
//		pDoc->UpdateAllViews(this);
	}

	return 0;
}