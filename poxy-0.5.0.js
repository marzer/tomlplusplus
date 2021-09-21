function do_with_debounce(fn)
{
	fn();
	setTimeout(function() { fn(); }, 200);
}

/*
$(function()
{
	page_header = $('body > header')[0]

	fix_body_header_padding = function()
	{
		document.body.style.paddingTop = page_header.offsetHeight + 'px';
	};

	$(page_header).resize(function()
	{
		do_with_debounce(fix_body_header_padding);
	});

	do_with_debounce(fix_body_header_padding);
});
*/
