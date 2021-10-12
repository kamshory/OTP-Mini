var ajax = {};
ajax.create = function () {
    if (typeof XMLHttpRequest !== 'undefined') {
        return new XMLHttpRequest();
    }
    var versions = [
        "MSXML2.XmlHttp.6.0",
        "MSXML2.XmlHttp.5.0",
        "MSXML2.XmlHttp.4.0",
        "MSXML2.XmlHttp.3.0",
        "MSXML2.XmlHttp.2.0",
        "Microsoft.XmlHttp"
    ];

    var xhr;
    for (var i = 0; i < versions.length; i++) {
        try {
            xhr = new ActiveXObject(versions[i]);
            break;
        } catch (e) 
        {
        }
    }
    return xhr;
};

ajax.send = function (url, callback, method, data, async) {
    if (async === undefined) {
        async = true;
    }
    var x = ajax.create();
    x.open(method, url, async);
    x.onreadystatechange = function () {
        if (x.readyState == 4) {
            callback(x.responseText)
        }
    };
    if (method == 'POST') {
        x.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    }
    x.send(data)
};

ajax.get = function (url, data, callback, async) {
    var query = [];
    for (var key in data) {
        if(data.hasOwnProperty(key))
        {
            query.push(encodeURIComponent(key) + '=' + encodeURIComponent(data[key]));
        }
    }
    ajax.send(url + (query.length ? '?' + query.join('&') : ''), callback, 'GET', null, async)
};

ajax.post = function (url, data, callback, async) {
    var query = [];
    for (var key in data) {
        if(data.hasOwnProperty(key))
        {
            query.push(encodeURIComponent(key) + '=' + encodeURIComponent(data[key]));
        }
    }
    ajax.send(url, callback, 'POST', query.join('&'), async)
};
function saveSubData()
{
    var ssid_name = document.querySelector('#ssid_name').value;
    var ssid_password = document.querySelector('#ssid_password').value;
    var mqtt_host = document.querySelector('#mqtt_host').value;
    var mqtt_port = document.querySelector('#mqtt_port').value;
    var mqtt_client = document.querySelector('#mqtt_client').value;
    var mqtt_username = document.querySelector('#mqtt_username').value;
    var mqtt_password = document.querySelector('#mqtt_password').value;
    var mqtt_topic = document.querySelector('#mqtt_topic').value;
    var mqtt_qos = document.querySelector('#mqtt_qos').value;
    var enable = document.querySelector('#enable').value;
    ajax.post(
        'save-subscribtion', 
        {
            action:'save-subscribtion',
            ssid_name:ssid_name, 
            ssid_password:ssid_password, 
            mqtt_host:mqtt_host, 
            mqtt_port:mqtt_port, 
            mqtt_client:mqtt_client, 
            mqtt_username:mqtt_username, 
            mqtt_password:mqtt_password,
            mqtt_topic:mqtt_topic,
            mqtt_qos:mqtt_qos,
            enable:enable
        },
        function(response){

        },
        true
    );
    return false;
}
function loadSubData()
{
    ajax.get(
        'subscribtion-configuration.json', 
        {
            
        },
        function(response){
            var data = JSON.parse(response);
            document.querySelector('#ssid_name').value = data.ssid_name;
            document.querySelector('#ssid_password').value = data.ssid_password;
            document.querySelector('#mqtt_host').value = data.mqtt_host;
            document.querySelector('#mqtt_port').value = data.mqtt_port;
            document.querySelector('#mqtt_client').value = data.mqtt_client;
            document.querySelector('#mqtt_username').value = data.mqtt_username;
            document.querySelector('#mqtt_password').value = data.mqtt_password;
        },
        true
    );
}
function loadAPData()
{
    ajax.get(
        'ap-configuration.json', 
        {
            
        },
        function(response){
            var data = JSON.parse(response);
            document.querySelector('#ssid_name').value = data.ssid_name;
            document.querySelector('#ssid_password').value = data.ssid_password;
        },
        true
    );
}
function saveAPData()
{
    var ssid_name = document.querySelector('#ssid_name').value;
    var ssid_password = document.querySelector('#ssid_password').value;
    ajax.post(
        'save-ap', 
        {
            action:'save-ap',
            ssid_name:ssid_name, 
            ssid_password:ssid_password
        },
        function(response){

        },
        true
    );
    return false;
}
window.onload = function()
{
    var uri = window.location.toString();
    if(uri.indexOf('ap-configuration.html') > -1)
    {
        loadAPData();
    }
    if(uri.indexOf('subscribtion-configuration.html') > -1)
    {
        loadSubData();
    }
}