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

function install_lightbox()
{
	const links = document.querySelectorAll('a.poxy-lightbox');
	if (!links.length)
		return;

	let overlay = null;
	let image = null;
	let caption = null;
	let opener = null;

	function hide()
	{
		if (!overlay || overlay.hidden)
			return;
		overlay.classList.remove('poxy-lightbox-visible');
		overlay.hidden = true;
		image.removeAttribute('src');
		document.body.classList.remove('poxy-lightbox-open');
		if (opener)
			opener.focus();
		opener = null;
	}

	function build()
	{
		overlay = document.createElement('div');
		overlay.className = 'poxy-lightbox-overlay';
		overlay.hidden = true;
		overlay.setAttribute('role', 'dialog');
		overlay.setAttribute('aria-modal', 'true');

		const close_button = document.createElement('button');
		close_button.className = 'poxy-lightbox-close';
		close_button.setAttribute('type', 'button');
		close_button.setAttribute('aria-label', 'Close');
		close_button.innerHTML = '&times;';
		overlay.appendChild(close_button);

		image = document.createElement('img');
		overlay.appendChild(image);

		caption = document.createElement('p');
		caption.className = 'poxy-lightbox-caption';
		overlay.appendChild(caption);

		overlay.addEventListener('click', hide);
		document.body.appendChild(overlay);
	}

	// doxygen gives a captioned image alt="Image" and puts the real text in the figcaption
	function caption_text(link, img)
	{
		const figure = link.closest('figure');
		const figcaption = figure ? figure.querySelector('figcaption') : null;
		if (figcaption && figcaption.textContent.trim())
			return figcaption.textContent.trim();
		return img ? img.alt : '';
	}

	function show(href, alt, text)
	{
		if (!overlay)
			build();
		image.src = href;
		image.alt = alt || '';
		caption.textContent = text || '';
		overlay.hidden = false;
		document.body.classList.add('poxy-lightbox-open');
		// a frame between display and opacity, or the transition never runs
		requestAnimationFrame(function() { overlay.classList.add('poxy-lightbox-visible'); });
		overlay.querySelector('.poxy-lightbox-close').focus();
	}

	links.forEach(function(link)
	{
		link.addEventListener('click', function(e)
		{
			// leave the modified clicks alone: they are how you open the original in a new tab
			if (e.ctrlKey || e.metaKey || e.shiftKey || e.altKey || e.button !== 0)
				return;
			const img = link.querySelector('img');
			e.preventDefault();
			opener = link;
			show(link.href, img ? img.alt : '', caption_text(link, img));
		});
	});

	document.addEventListener('keydown', function(e)
	{
		if (e.key === 'Escape')
			hide();
	});
}

function install_mcss_search_shim()
{
	let showSearch_impl = window.showSearch;
	window.showSearch = function()
	{
		if (window.location.hash == '#search')
		{
			document.getElementById('search-input').focus();
			return false;
		}
		return showSearch_impl.apply(null);
	};
}

document.addEventListener('DOMContentLoaded', install_lightbox);

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
