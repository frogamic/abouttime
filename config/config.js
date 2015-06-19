// Open the settings dialog
Pebble.addEventListener('showConfiguration',
    function () {
        Pebble.openURL('data:text/html,' + encodeURIComponent(config_html + '<!--.html'));
    }
);
