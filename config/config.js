// Open the settings dialog
Pebble.addEventListener('showConfiguration',
    function () {
        console.log('Showing config screen');
        Pebble.openURL('data:text/html,' + encodeURIComponent(config_html + '<!--.html'));
    }
);

Pebble.addEventListener('webviewclosed',
    function(e) {
        var config = JSON.parse(decodeURIComponent(e.response));
        console.log('Received config: ' + JSON.stringify(config));
    }
);
