function ToggleEnableIf(anchor)
{
	if (!anchor.hasOwnProperty('enableIfHidden'))
		anchor['enableIfHidden'] = true;
	anchor.enableIfHidden = !anchor.enableIfHidden;
	
	content = anchor.parentNode.getElementsByTagName('span')[0]
	if (anchor.enableIfHidden)
	{
		anchor.style.display = "";
		content.style.display = "";
	}
	else
	{
		anchor.style.display = "none";
		content.style.display = "block";
	}
}
