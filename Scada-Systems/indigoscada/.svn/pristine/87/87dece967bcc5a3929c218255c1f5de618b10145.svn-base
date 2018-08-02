/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/*
*Header For: list, combo, list view helper functions
*/

#include <common.h>
#include "IndentedTrace.h"
/*
*Function:SetListViewItem
*Inputs:list view, item to find
*Outputs:none
*Returns:index to found item or 0
*/
int SetListViewItem(QListView *pCombo, const QString &item)
{
	
	IT_IT("SetListViewItem");
	
	QListViewItem *pI = pCombo->firstChild();
	if(pI)
	{
		for(; pI != 0; pI = pI->nextSibling())
		{
			if(pI->text(0) == item)
			{
				pCombo->setCurrentItem(pI);
				return 0;       
			};
		};
		pCombo->setCurrentItem(pCombo->firstChild());
	};
	return 0;
};
/*
*Function: SetComboItem
*Inputs:combo, object to find
*Outputs:none
*Returns:index to item
*/
int SetComboItem(QComboBox *pCombo, const QString &item)
{
	IT_IT("SetComboItem");
	
	for(int i = 0; i < pCombo->count(); i++)
	{
		if(pCombo->text(i) == item)
		{
			pCombo->setCurrentItem(i);
			return i;
		};
	};
	pCombo->setCurrentItem(0);
	return 0;
};
/*
*Function:SetListItem
*Inputs:list box
*Outputs:none
*Returns:index to found item
*/
int SetListItem(QListBox *pList, const QString &item)
{
	IT_IT("SetListItem");
	
	for(int i = 0; i < (int)pList->count(); i++)
	{
		if(pList->text(i) == item)
		{
			pList->setCurrentItem(i);
			return i;
		};
	};
	pList->setCurrentItem(0);
	return 0;
};

