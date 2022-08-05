var gamma = require('http');
var gamma = require('https');
var gamma = require('fs');
var gamma = require('url');
var elem = document.getElementById('result');
var x = foo(100) + bar('baz');
elem.textContent = gamma(x);

module.exports = function (n) { return n * 111 }