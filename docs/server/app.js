var express = require('express');
var app = express();
var path = require('path');
var font_ver = '?v=4.6.3';

// viewed at http://localhost:8080
app.get('/', function(req, res) {
    res.sendFile(path.join(__dirname + '/../index.html'));
});

app.get('/assets/fonts/*', function(req, res) {
    console.log('SPLIT URL FOR FONTS: ' + req.url.split(font_ver)[0]);
    res.sendFile(path.join(__dirname + '/../' + req.url.split(font_ver)[0]));
});

// for all other files
app.get('/*', function(req, res) {
    //console.log('URL: ' + req.url);
    res.sendFile(path.join(__dirname + '/../' + req.url));
});

app.listen(8080);