function do_with_debounce(fn)
{
	fn();
	setTimeout(function() { fn(); }, 200);
}

function set_theme(theme)
{
	localStorage.setItem('poxy-theme', theme);
	document.documentElement.className = 'poxy-theme-' + theme;
	console.log("poxy theme set to '" + theme + "'");
}

function initialize_theme(default_theme)
{
	current = localStorage.getItem('poxy-theme');
	if (!current)
		current = default_theme
	set_theme(current);
}

function toggle_theme()
{
	current = localStorage.getItem('poxy-theme');
	if (!current || current === 'light')
		set_theme('dark');
	else
		set_theme('light');
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
