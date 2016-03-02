chrome.app.runtime.onLaunched.addListener(function() {
	chrome.app.window.create('inicio.html', {
		width : 800,
		height : 700
	});
});